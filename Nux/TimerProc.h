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

namespace nux
{

  class TimerHandler;
  class WindowThread;
  
  class TimerFunctor : public sigc::trackable
  {
  public:
    sigc::signal<void, void *> time_expires;
  };

  class TimerObject;

  class TimerHandle
  {
  public:
    TimerHandle();
    TimerHandle(TimerObject *timer_object);
    ~TimerHandle();
    TimerHandle(const TimerHandle &);

    TimerHandle &operator = (const TimerHandle &);
    bool IsValid() const;

    //! return a factor between 0.0f and 1.0f representing the progress of the timer.
    float GetProgress() const;
    float GetProgressDelta() const;

    int GetScheduledIterationCount() const;
    int GetProgressIterationCount() const;
    int GetElapsedTimed() const;

  private:
    TimerObject *m_d;

    friend class TimerHandler;
  };

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

    TimerHandler();
    ~TimerHandler();

    //! Add a timer callback.
    /*!
      Add a timer callback to the timer manager. When the timer expires, the callback function is executed.
      The returned TimerObject should not be deleted by the caller.

      @param Milliseconds   Period delay before the callback is executed.
      @param Callback       The callback to execute when the timer expires.
      @param Data           The callback data
      @param window_thread  Thread safety mesure. Pass the WindowThread associated to this TimerHandler if it is called from a diferent thread than the one where the main thread was created.
      @return               A handle to the timer.
    */
    TimerHandle AddTimerHandler (unsigned int Period, TimerFunctor *Callback, void *Data, WindowThread* window_thread = NULL);
    //! Add a periodic timer callback.
    /*!
      Add a timer callback to the timer manager. Every time the timer expires, the callback function is executed.
      The returned TimerHandle should not be deleted by the caller.

      @param Milliseconds   Period delay before the callback is executed.
      @param Duration       The duration over which the timer is repeated.
      @param Callback       The callback to execute when the timer expires.
      @param Data           The callback data
      @return               A handle to the timer.
    */
    TimerHandle AddPeriodicTimerHandler (unsigned int Period, int Duration, TimerFunctor *Callback, void *Data);
    //! Add a timer callback to be called a finite number of time.
    /*!
      Add a timer callback to the timer manager. The timer callback will be call N times exactly.
      Every time the timer expires, the callback function is executed.
      The returned TimerHandle should not be deleted by the caller.

      @param Milliseconds       Period delay before the callback is executed.
      @param NumberOfIteration  The number of time to repeat the the wait period.
      @param Callback           The callback to execute when the timer expires.
      @param Data               The callback data
      @return                   A handle to the timer.
    */
    TimerHandle AddCountIterationTimerHandler (unsigned int Period, int NumberOfIteration, TimerFunctor *Callback, void *Data);

    //! Search for a timer handle.
    /*!
      Search for a timer handle in the timer handle queue. Return true if the timer is found.

      @param handle             Timer handle to search.
      @return                   Return true if the timer is found; false otherwise.
    */
    bool FindTimerHandle (TimerHandle &handle);

    //! Remove a timer;
    /*!
      @param handle Timer handle to search.
      @return Return True if the timer is found.
    */
    bool RemoveTimerHandler (TimerHandle &handle);

    //! Return the delay until the next timer expires.
    /*!
      @return Delay to next timer expiration in milliseconds.
    */
    int DelayUntilNextTimerExpires();

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    int ExecTimerHandler (t_u32 timer_id);
#else
    int ExecTimerHandler();
#endif

    //! Start the timers that were sett before the system was fully initialized.
    void StartEarlyTimerObjects();

  private:
    bool m_IsProceesingTimers;
    TimerObject *AddHandle (TimerObject *handle);
    t_u32 GetNumPendingHandler();

    //! Single linked list of timer delays.
    TimerObject *m_timer_object_queue;
    std::list<TimerObject*> _early_timer_objects;  //!< timer objects that couldn't be started because the main loop is not runing yet.
  };

}

#endif // TIMERPROC_H

