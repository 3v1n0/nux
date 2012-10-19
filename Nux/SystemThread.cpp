/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"

#include "Layout.h"
#if defined(NUX_OS_WINDOWS)
#include "NuxGraphics/GraphicsDisplay.h"
#elif defined(NUX_OS_LINUX)
#include "NuxGraphics/GraphicsDisplay.h"
#endif
#include "NuxGraphics/GraphicsEngine.h"
#include "ClientArea.h"
#include "WindowCompositor.h"
#include "TimerProc.h"
#include "WindowCompositor.h"
#include "WindowThread.h"
#include "SystemThread.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(SystemThread);


  SystemThread::SystemThread(AbstractThread *Parent/* = 0*/)
    :   AbstractThread(Parent)
  {

  }

  SystemThread::~SystemThread()
  {
  }

  ThreadState SystemThread::Start(void * /* arg */)
  {
    if (!parent_)
    {
      return NThread::Start();
    }
    else
    {
      if (parent_->Type().IsObjectType(SystemThread::StaticObjectType))
        return static_cast<SystemThread *> (parent_)->StartChildThread(this, true);

      if (parent_->Type().IsObjectType(WindowThread::StaticObjectType))
        return static_cast<WindowThread *> (parent_)->StartChildThread(this, true);

      nuxAssertMsg(0, "[WindowThread::Start] This should not happen.");
      return THREAD_START_ERROR;
    }
  }

  int SystemThread::Run(void * /* arg */)
  {
    if (user_init_func_)
    {
      (*user_init_func_) (this, initialization_data_);
    }

    if (parent_)
    {
      if (parent_->Type().IsObjectType(SystemThread::StaticObjectType))
        static_cast<SystemThread *> (parent_)->ChildHasFinished(this);

      if (parent_->Type().IsObjectType(WindowThread::StaticObjectType))
        static_cast<WindowThread *> (parent_)->ChildHasFinished(this);
    }

    SetThreadState(THREADSTOP);
    TerminateChildThreads();
    return 0;
  }

  ThreadState SystemThread::StartChildThread(AbstractThread *thread, bool /* Modal */)
  {
    ThreadState state = thread->NThread::Start();
    //if(state == THREADRUNNING)
    AddChildThread(thread);
    return state;
  }

  void SystemThread::AddChildThread(AbstractThread *thread)
  {
    nuxAssert(thread);
    std::list<AbstractThread*>::iterator it;
    it = find(children_thread_list_.begin(), children_thread_list_.end(), thread);

    if (it == children_thread_list_.end())
    {
      children_thread_list_.push_back(thread);
    }
  }

  void SystemThread::RemoveChildThread(AbstractThread *window)
  {
    nuxAssert(window);
    std::list<AbstractThread*>::iterator it;
    it = find(children_thread_list_.begin(), children_thread_list_.end(), window);

    if (it != children_thread_list_.end())
    {
      children_thread_list_.erase(it);
    }
  }

  void SystemThread::ChildHasFinished(AbstractThread *thread)
  {
    RemoveChildThread(thread);

    if (thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
      //SuspendChildGraphics(static_cast<WindowThread*>(thread));
    }

    thread->SetThreadState(THREADSTOP);
  }

  void SystemThread::TerminateChildThreads()
  {
    std::list<AbstractThread*>::iterator it;

    for (it = children_thread_list_.begin(); it != children_thread_list_.end(); it++)
    {
      (*it)->SetThreadState(THREADSTOP);
    }

    children_thread_list_.clear();
  }

  bool SystemThread::ThreadCtor()
  {
    SetThreadState(THREADRUNNING);

    return true;
  }

  bool SystemThread::ThreadDtor()
  {
    return true;
  }

}
