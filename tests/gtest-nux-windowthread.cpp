#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>

#include <string>
#include <fstream>

#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>

#include "Nux/Nux.h"


using namespace testing;
using ::testing::Invoke;

namespace {

class MockFdDispatch
{
  public:
    MOCK_METHOD0(dispatch, void());

    MockFdDispatch(int fd) :
      mFd (fd)
    {
      ON_CALL(*this, dispatch())
        .WillByDefault(Invoke(this, &MockFdDispatch::ClearStream));
    }

  private:

    void ClearStream()
    {
      char buf[2];
      if (read(mFd, reinterpret_cast <void *>(buf), 2) == -1)
        throw std::runtime_error(strerror(errno));
    }

    int mFd;
};

class TestWindowThread : public ::testing::Test
{
  public:

    TestWindowThread()
      : seconds_to_quit(0)
      , quit_thread (NULL)
    {
      nux::NuxInitialize(0);
      wnd_thread.reset (nux::CreateNuxWindow("Nux Window", 300, 200,
                                             nux::WINDOWSTYLE_NORMAL, NULL,
                                             false, TestWindowThread::WindowThreadInit,
                                             this));

      if (pipe2(quit_pipe_, O_CLOEXEC) == -1)
        throw std::runtime_error(strerror(errno));

      if (pipe2(test_quit_pipe_, O_CLOEXEC) == -1)
        throw std::runtime_error(strerror(errno));

      WatchFdForEventsOnRun(quit_pipe_[0],
                            std::bind(&TestWindowThread::QuitMessage,
                                      this));


    }

    ~TestWindowThread()
    {
      if (quit_thread)
        delete quit_thread;

      close(quit_pipe_[0]);
      close(quit_pipe_[1]);
      close(test_quit_pipe_[0]);
      close(test_quit_pipe_[1]);
    }

  protected:

    std::unique_ptr <nux::WindowThread> wnd_thread;
    unsigned int                        seconds_to_quit;
    nux::SystemThread                   *quit_thread;

    void WatchFdForEventsOnRun(int fd,
                               const nux::WindowThread::FdWatchCallback &cb)
    {
      WatchedFd wfd;
      wfd.fd = fd;
      wfd.callback = cb;

      watched_fds.push_back(wfd);
    }

    /* Watchdog threads are really not that great, but ping-ponging between
     * the window and application threads would over-complicate this test */
    void QuitAfter(unsigned int seconds)
    {
      seconds_to_quit = seconds;
      quit_thread = nux::CreateSystemThread(NULL, TestWindowThread::QuitTask, this);
      quit_thread->Start(NULL);
    }

    void WaitForQuit()
    {
      struct pollfd pfd;

      pfd.events = POLLIN;
      pfd.revents = 0;
      pfd.fd = test_quit_pipe_[0];

      if (poll(&pfd, 1, -1) == -1)
        throw std::runtime_error(strerror(errno));

      /* Something was written to the read end of our pipe,
       * we can continue now */
    }

  private:

    typedef struct _WatchedFd
    {
      int                                fd;
      nux::WindowThread::FdWatchCallback callback;
    } WatchedFd;

    static void WindowThreadInit(nux::NThread *, void *data)
    {
      TestWindowThread *tw = reinterpret_cast <TestWindowThread *>(data);

      /* Monitor all nominated fd's */
      for (std::vector<WatchedFd>::iterator it = tw->watched_fds.begin();
           it != tw->watched_fds.end();
           ++it)
      {
        tw->wnd_thread->WatchFdForEvents(it->fd, it->callback);
      }
    }

    static void QuitTask(nux::NThread *, void *data)
    {
      TestWindowThread *tw = reinterpret_cast <TestWindowThread *> (data);

      nux::SleepForMilliseconds(tw->seconds_to_quit * 1000);

      const char *buf = "w\0";
      if (write(tw->quit_pipe_[1],
                reinterpret_cast<void *>(const_cast<char *>(buf)),
                2) == -1)
        throw std::runtime_error(strerror(errno));
    }

    void QuitMessage()
    {
      wnd_thread->ExitMainLoop();

      /* Send a message indicating that the main loop
       * exited and we can safely continue */
      const char *buf = "w\0";
      if (write(test_quit_pipe_[1],
                reinterpret_cast<void *>(const_cast<char *>(buf)),
                2) == -1)
        throw std::runtime_error(strerror(errno));
    }

    std::vector<WatchedFd> watched_fds;

    int quit_pipe_[2];
    int test_quit_pipe_[2];
};

TEST_F(TestWindowThread, Create)
{
  ASSERT_TRUE(wnd_thread != NULL);
  EXPECT_EQ(wnd_thread->GetWindowTitle(), std::string("Nux Window"));
  EXPECT_EQ(wnd_thread->IsModalWindow(), false);

  EXPECT_EQ(wnd_thread->IsComputingLayout(), false);
  EXPECT_EQ(wnd_thread->IsInsideLayoutCycle(), false);
}

TEST_F(TestWindowThread, WatchFd)
{
  int pipefd[2];

  if (pipe2 (pipefd, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));

  MockFdDispatch dispatch(pipefd[0]);

  EXPECT_CALL(dispatch, dispatch());

  WatchFdForEventsOnRun(pipefd[0], std::bind(&MockFdDispatch::dispatch, &dispatch));
  const char *buf = "w\0";
  if (write(pipefd[1], reinterpret_cast <void *> (const_cast <char *> (buf)), 2) == -1)
    throw std::runtime_error (strerror(errno));

  QuitAfter(3);
  wnd_thread->Run(NULL);

  /* We must wait for quit before closing the pipes */
  WaitForQuit();

  close (pipefd[0]);
  close (pipefd[1]);
}

TEST_F(TestWindowThread, MultiWatchFd)
{
  int pipefd[2], pipefd2[2];

  if (pipe2 (pipefd, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));
  if (pipe2 (pipefd2, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));

  MockFdDispatch dispatch(pipefd[0]), dispatch2(pipefd2[0]);

  EXPECT_CALL(dispatch, dispatch());
  EXPECT_CALL(dispatch2, dispatch());

  WatchFdForEventsOnRun(pipefd[0], std::bind(&MockFdDispatch::dispatch, &dispatch));
  WatchFdForEventsOnRun(pipefd2[0], std::bind(&MockFdDispatch::dispatch, &dispatch2));
  const char *buf = "w\0";
  if (write(pipefd[1], reinterpret_cast <void *> (const_cast <char *> (buf)), 2) == -1)
    throw std::runtime_error (strerror(errno));
  if (write(pipefd2[1], reinterpret_cast <void *> (const_cast <char *> (buf)), 2) == -1)
    throw std::runtime_error (strerror(errno));

  QuitAfter(3);
  wnd_thread->Run(NULL);

  /* We must wait for quit before closing the pipes */
  WaitForQuit();

  close (pipefd[0]);
  close (pipefd[1]);
  close (pipefd2[0]);
  close (pipefd2[1]);
}

TEST_F(TestWindowThread, OneFdEvent)
{
  int pipefd[2], pipefd2[2];

  if (pipe2 (pipefd, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));
  if (pipe2 (pipefd2, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));

  MockFdDispatch dispatch(pipefd[0]), dispatch2(pipefd2[0]);

  EXPECT_CALL(dispatch, dispatch());
  EXPECT_CALL(dispatch2, dispatch()).Times(0);

  WatchFdForEventsOnRun(pipefd[0], std::bind(&MockFdDispatch::dispatch, &dispatch));
  WatchFdForEventsOnRun(pipefd2[0], std::bind(&MockFdDispatch::dispatch, &dispatch2));
  const char *buf = "w\0";
  if (write(pipefd[1], reinterpret_cast <void *> (const_cast <char *> (buf)), 2) == -1)
    throw std::runtime_error (strerror(errno));

  QuitAfter(3);
  wnd_thread->Run(NULL);

  /* We must wait for quit before closing the pipes */
  WaitForQuit();

  close (pipefd[0]);
  close (pipefd[1]);
  close (pipefd2[0]);
  close (pipefd2[1]);
}


}
