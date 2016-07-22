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


#ifndef TIMERPROC_H
#define TIMERPROC_H

#include <memory>
namespace nux
{

  class TimerHandler;
  class WindowThread;

  //! A timeout callback. Fires a signal when a timer expires.
  class TimeOutSignal : public sigc::trackable
  {
  public:
    sigc::signal<void, void *> tick;
    sigc::signal<void, void *> expired;
  };
  typedef TimeOutSignal TimerFunctor;

  class TimerObject;

  class TimerHandle
  {
  public:
    TimerHandle();
    TimerHandle(TimerObject *timer_object);
    TimerHandle(const TimerHandle &);

    TimerHandle &operator = (const TimerHandle &);
    std::shared_ptr<TimerObject> const& operator->() const;
    bool operator==(TimerHandle const& h) const;

    bool IsValid() const; //!<deprecated. use Activated().
    bool Activated() const;

    //! return a factor between 0.0f and 1.0f representing the progress of the timer.
    float GetProgress() const;
    float GetProgressDelta() const;

    int GetScheduledIterationCount() const;
    int GetProgressIterationCount() const;
    int GetElapsedTimed() const;

  private:
    std::shared_ptr<TimerObject> m_d;
  };

  //! A timer manager class created by WindowThread.
  class TimerHandler
  {
  public:
    enum
    {
      TIMERTYPE_UNKNONW   = 0L,
      TIMERTYPE_PERIODIC,
      TIMERTYPE_DURATION,
      TIMERTYPE_ITERATION,
    };

    typedef enum
    {
      TIMER_STATE_STOPED,
      TIMER_STATE_PAUSED,
      TIMER_STATE_RUNNING,
    } TimerState;

    TimerHandler(WindowThread *window_thread);

    //! Add a timer callback.
    /*!
      Add a timer callback to the timer manager. When the timer expires, the callback function is executed.
      The returned TimerObject should not be deleted by the caller.

      @param Milliseconds   period Time before the timer ticks. The timer ticks only once and then it expires.
      @param timeout_signal The callback to execute when the timer expires.
      @param Data           The callback data
      @param window_thread  Thread safety measure. Pass the WindowThread associated to this TimerHandler if it is called from a different thread than the one where the main thread was created.
      @return               A handle to the timer.
    */
    TimerHandle AddOneShotTimer(unsigned int period, TimeOutSignal *timeout_signal, void *Data, WindowThread* window_thread = NULL);
    //! Add a periodic timer callback.
    /*!
      Add a timer callback to the timer manager. Every time the timer expires, the callback function is executed.
      The returned TimerHandle should not be deleted by the caller.

      @param Milliseconds   period Time between the timer ticks. The timer will tick 'NumberOfIteration' times.
      @param Duration       The duration over which the timer is repeated.
      @param timeout_signal The callback to execute when the timer expires.
      @param Data           The callback data
      @return               A handle to the timer.
    */
    TimerHandle AddDurationTimer(unsigned int period, int Duration, TimeOutSignal *timeout_signal, void *Data);
    //! Add a timer callback to be called a finite number of time.
    /*!
      Add a timer callback to the timer manager. The timer callback will be call N times exactly.
      Every time the timer expires, the callback function is executed.
      The returned TimerHandle should not be deleted by the caller.

      @param Milliseconds       period Time between the timer ticks. The timer will tick 'NumberOfIteration' times.
      @param NumberOfIteration  The number of time to repeat the wait period.
      @param timeout_signal     The callback to execute when the timer expires.
      @param Data               The callback data
      @return                   A handle to the timer.
    */
    TimerHandle AddIterativeTimer(unsigned int period, int NumberOfIteration, TimeOutSignal *timeout_signal, void *Data);

    //! Search for a timer handle.
    /*!
      Search for a timer handle in the timer handle queue. Return true if the timer is found.

      @param handle             Timer handle to search.
      @return                   Return true if the timer is found; false otherwise.
    */
    bool FindTimerHandle(TimerHandle& handle);

    //! Remove a timer;
    /*!
        This call stops the timer.
        @param handle Timer handle to search.
        @return Return True if the timer is found.
    */
    bool RemoveTimerHandler(TimerHandle& handle);

    //! Pause a timer.
    /*!
        This call pauses a timer.
        @param handle Timer handle to look for.
        @return True if the timer is paused.
    */
    bool PauseTimer(TimerHandle& handle);

    //! Resume the execution of a timer.
    /*!
        This call resumes the execution of a timer.
        @param handle Timer handle to look for.
        @return True if the timer execution has resumed..
    */
    bool ResumeTimer(TimerHandle& handle);

    //! Return the delay until the next timer expires.
    /*!
      @return Delay to next timer expiration in milliseconds.
    */
    int DelayUntilNextTimerExpires();

    int ExecTimerHandler (unsigned int timer_id);

    //! Start the timers that were sett before the system was fully initialized.
    void StartEarlyTimerObjects();

  private:
    // Prevent copy constructor.
    TimerHandler(const TimerHandler&);

    // Prevent assignment operator.
    TimerHandler& operator = (const TimerHandler&);

    WindowThread *window_thread_; //!< The WindowThread to which this object belongs.

    bool is_processing_timers_;
    void AddHandle(TimerHandle const&);
    unsigned int GetNumPendingHandler();

    std::deque<TimerHandle> timer_handler_queue_;
    std::list<TimerHandle> early_timer_handlers_;  //!< timer objects that couldn't be started because the main loop is not runing yet.
  };

}

#endif // TIMERPROC_H

