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
#include "TimerProc.h"
#include "WindowCompositor.h"
#include "WindowThread.h"

class BaseWindow;
    
namespace nux
{

  typedef struct
  {
    long    sec;         // seconds
    long    usec;        // and microseconds
  } TimeStruct;

  /**
      Return true if t1 is a time reference that will happen after time t2 has expired.
  */
  static bool TimeIsGreater(TimeStruct t1, TimeStruct t2);
  static void TimeRightNow(TimeStruct *tv);
  static void Addmillisecs(TimeStruct *tv, unsigned int milliseconds);
//static unsigned int TimeDiff(TimeStruct t1, TimeStruct t2);

  static NThreadSafeCounter TimerUID = 0x01234567;

  class TimerObject
  {
  public:
    TimerObject();

    bool operator == (const TimerObject &timer_object);

    //! Delay before the callback expires
    TimeStruct      when;
    gint64          ms_time; // milliseconds 
    void            *CallbackData;
    TimeOutSignal    *timeout_signal;

    //! time progression factor between [0.0, 1.0]
    float           Param;
    float           ProgressDelta;
    int             Type;
    int             ScheduledIteration;     //!< The number of scheduled iterations.
    int             ProgressIterationCount; //!< Number of times the timer has been executed.
    int             Period;                 //!< The periode of the timer interuption(in milliseconds).
    int             Duration;               //!< How long the timer will be running from start to finish(in milliseconds);
    int             ElapsedTime;            //!< Elapsed time during execution(in milliseconds).
    bool            marked_for_removal_;
    BaseWindow      *Window;                 //!< BaseWindow from where the timer was created.
    TimerObject     *next;
    TimerObject     *prev;
    unsigned int           glibid;
    unsigned int           uid;
    TimerHandler::TimerState      state_;
  };

  TimerObject::TimerObject()
  {
    ms_time         = 0;
    Type            = 0;
    CallbackData    = 0;
    timeout_signal   = 0;
    Param           = 0;
    Period          = 0;
    Duration        = 0;
    ElapsedTime     = 0;
    ScheduledIteration       = 0;
    ProgressIterationCount  = 0;
    marked_for_removal_ = 0;
    Window          = 0;
    next            = 0;
    prev            = 0;
    glibid          = 0;
    uid             = 0;
    state_          = TimerHandler::TIMER_STATE_STOPED;
  }

  TimerHandle::TimerHandle()
  {
    m_d = 0;
  }

  TimerHandle::TimerHandle(TimerObject *timer_object)
  {
    m_d = timer_object;
  }

  TimerHandle::~TimerHandle()
  {
    m_d = 0;
  }

  TimerHandle::TimerHandle(const TimerHandle &timer_handle)
  {
    m_d = timer_handle.m_d;
  }

  TimerHandle &TimerHandle::operator = (const TimerHandle &timer_handle)
  {
    m_d = timer_handle.m_d;
    return *this;
  }

  bool TimerHandle::IsValid() const
  {
    return Activated();
  }

  bool TimerHandle::Activated() const
  {
    return m_d != 0;
  }

  float TimerHandle::GetProgress() const
  {
    if (m_d)
      return m_d->Param;
    return 0.0f;
  }

  float TimerHandle::GetProgressDelta() const
  {
    if (m_d)
      return m_d->ProgressDelta;
    return 0;
  }

  int TimerHandle::GetScheduledIterationCount() const
  {
    if (m_d)
      return m_d->ScheduledIteration;
    return 0;
  }

  int TimerHandle::GetProgressIterationCount() const
  {
    if (m_d)
      return m_d->ProgressIterationCount;
    return 0;
  }

  int TimerHandle::GetElapsedTimed() const
  {
    if (m_d)
      return m_d->ElapsedTime;
    return 0;
  }

////////////////////////////////////////////////////
  TimerHandler::TimerHandler(WindowThread* window_thread)
  : window_thread_(window_thread)
  , is_processing_timers_(false)
  , timer_object_queue_(NULL)
  {
  }

  TimerHandler::~TimerHandler()
  {
  }

  void TimerHandler::StartEarlyTimerObjects()
  {
    std::list<TimerObject*>::iterator it;
    for (it = _early_timer_objects.begin(); it != _early_timer_objects.end(); it++)
    {
      TimerObject *timer_object = *it;
      timer_object->ms_time = g_get_monotonic_time() / 1000;

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
        timer_object->glibid = GetWindowThread()->AddTimeout(timer_object->Period);
#endif
    }

    _early_timer_objects.clear();
  }

  TimerHandle TimerHandler::AddOneShotTimer(unsigned int Period, TimeOutSignal* timeout_signal, void* Data, WindowThread* window_thread)
  {
    TimerObject *timer_object = new TimerObject();

    timer_object->ms_time = g_get_monotonic_time() / 1000;

    timer_object->CallbackData  = Data;
    timer_object->timeout_signal = timeout_signal;
    timer_object->Period        = Period;
    timer_object->Type          = TIMERTYPE_PERIODIC;
    timer_object->state_        = TimerHandler::TIMER_STATE_RUNNING;
    if (window_thread)
      timer_object->Window        = window_thread->GetWindowCompositor().GetProcessingTopView();
    else
      timer_object->Window        = GetWindowThread()->GetWindowCompositor().GetProcessingTopView();
    
    AddHandle(timer_object);

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    {
      if (window_thread)
        timer_object->glibid = window_thread->AddTimeout(Period);
      else
        timer_object->glibid = GetWindowThread()->AddTimeout(Period);
      
      if (timer_object->glibid == 0)
      {
        _early_timer_objects.push_back(timer_object);
        // Probably trying to set a timeout before Glib main context and loop have been created.
        // Sometimes later, this timer will be examined when ExecTimerHandler is called.
        // This happens when trying to set a callback before the main loop has been initialized.
      }

      //nuxDebugMsg("[TimerHandler::AddOneShotTimer] Adding Timeout ID: %d", timer_object->glibid);
    }
#endif

    TimerHandle handle(timer_object);
    return handle;
  }

  TimerHandle TimerHandler::AddDurationTimer(unsigned int Period, int Duration, TimeOutSignal *timeout_signal, void *Data)
  {
    TimerObject* timer_object = new TimerObject();

    timer_object->ms_time = g_get_monotonic_time() / 1000;
    timer_object->CallbackData = Data;
    timer_object->timeout_signal = timeout_signal;

    timer_object->Period    = Period;
    timer_object->Duration  = (Duration < 0) ? -1 : Duration;
    timer_object->Type      = TIMERTYPE_DURATION;
    timer_object->state_    = TimerHandler::TIMER_STATE_RUNNING;
    AddHandle(timer_object);

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    {
      timer_object->glibid = GetWindowThread()->AddTimeout(Period);

      if (timer_object->glibid == 0)
      {
        _early_timer_objects.push_back(timer_object);
        // Probably trying to set a timeout before Glib main context and loop have been created.
        // Sometimes later, this timer will be examined when ExecTimerHandler is called.
        // This happens when trying to set a callback before the mainloop has been initialized.
      }

      //nuxDebugMsg("[TimerHandler::AddOneShotTimer] Adding Timeout ID: %d", timer_object->glibid);
    }
#endif

    TimerHandle handle(timer_object);
    return handle;
  }

  TimerHandle TimerHandler::AddIterativeTimer(unsigned int Period, int NumberOfIterations, TimeOutSignal *timeout_signal, void *Data)
  {
    TimerObject* timer_object = new TimerObject();

    timer_object->ms_time = g_get_monotonic_time() / 1000;
    timer_object->CallbackData = Data;
    timer_object->timeout_signal = timeout_signal;

    timer_object->Period              = Period;
    timer_object->ScheduledIteration  = (NumberOfIterations < 0) ? -1 : NumberOfIterations;
    timer_object->Type                = TIMERTYPE_ITERATION;
    timer_object->state_              = TimerHandler::TIMER_STATE_RUNNING;
    AddHandle(timer_object);

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    {
      timer_object->glibid = GetWindowThread()->AddTimeout(Period);

      if (timer_object->glibid == 0)
      {
        _early_timer_objects.push_back(timer_object);
        // Probably trying to set a timeout before Glib main context and loop have been created.
        // Sometimes later, this timer will be examined when ExecTimerHandler is called.
        // This happens when trying to set a callback before the mainloop has been initialized.
      }

      //nuxDebugMsg("[TimerHandler::AddOneShotTimer] Adding Timeout ID: %d", timer_object->glibid);
    }
#endif
    TimerHandle handle(timer_object);
    return handle;
  }

// Sort timers and add them to the queue
  TimerObject *TimerHandler::AddHandle(TimerObject* timer_object)
  {
    if (timer_object == NULL)
      return NULL;

    // Give the Timer a unique ID;
    timer_object->uid = TimerUID.GetValue();
    TimerUID.Increment();

    // If the queue is empty or the new timer will expire sooner than the first timer in the queue
    // then add the new timer at the start of the queue.
    if ((timer_object_queue_ == NULL) || (timer_object_queue_->ms_time > timer_object->ms_time))
    {
      // Add the timer timer_object at the head of the queue
      timer_object->next = timer_object_queue_;

      if (timer_object_queue_)
        timer_object_queue_->prev = timer_object;

      timer_object->prev = 0;
      timer_object_queue_ = timer_object;

      return timer_object;
    }

    TimerObject *tmp = timer_object_queue_;

    while (tmp->next != NULL)
    {
      // Is the time to wait for tmp->next to expire smaller than for timer_object
      if (timer_object->ms_time > tmp->next->ms_time)
      {
        // keep searching
        tmp = tmp->next;
      }
      else
      {
        timer_object->next = tmp->next;
        tmp->next->prev = timer_object;
        tmp->next = timer_object;
        timer_object->prev = tmp;
        return timer_object;
      }
    }

    tmp->next = timer_object;
    timer_object->next = NULL;
    timer_object->prev = tmp;
    return timer_object;
  }

  unsigned int TimerHandler::GetNumPendingHandler()
  {
    unsigned int count = 0;
    TimerObject *head = timer_object_queue_;

    while (head)
    {
      count++;
      head = head->next;
    }

    return count;
  }

  bool TimerHandler::RemoveTimerHandler(TimerHandle &handle)
  {
    NUX_RETURN_VALUE_IF_NULL(handle.m_d, false);
    NUX_RETURN_VALUE_IF_NULL(timer_object_queue_, false);

    TimerObject* tmp;

    tmp = timer_object_queue_;

    while (tmp)
    {
      if ((tmp == handle.m_d) && (tmp->uid == handle.m_d->uid))
      {
        if (!is_processing_timers_)
        {
          if (tmp->next)
            tmp->next->prev = tmp->prev;

          if (tmp->prev)
            tmp->prev->next = tmp->next;

          if ((handle.m_d == timer_object_queue_) && (handle.m_d->uid == timer_object_queue_->uid))
            timer_object_queue_ = handle.m_d->next;

          NUX_SAFE_DELETE(handle.m_d);
        }
        else
        {
          handle.m_d->marked_for_removal_ = true;
        }

        return true;
      }

      tmp = tmp->next;
    }

    return false;
  }

  bool TimerHandler::PauseTimer(TimerHandle& handle)
  {
    NUX_RETURN_VALUE_IF_NULL(handle.m_d, false);
    NUX_RETURN_VALUE_IF_NULL(timer_object_queue_, false);

    if (handle.m_d->state_ != TimerHandler::TIMER_STATE_RUNNING)
    {
      return false;
    }

    TimerObject* tmp;

    // Pointer to the start of the queue.
    tmp = timer_object_queue_;

    // Search for the timer handle
    while (tmp)
    {
      if ((tmp == handle.m_d) && (tmp->uid == handle.m_d->uid))
      {
        handle.m_d->state_ = TimerHandler::TIMER_STATE_PAUSED;


        if (!is_processing_timers_)
        {
          gint64 ms_time_now = g_get_monotonic_time() / 1000;
          
          if (handle.m_d->Type == TIMERTYPE_PERIODIC)
          {
            handle.m_d->ElapsedTime += (ms_time_now - handle.m_d->ms_time);
            handle.m_d->ProgressDelta = float(handle.m_d->ElapsedTime) / float(handle.m_d->Period);

            if (handle.m_d->Param + handle.m_d->ProgressDelta > 1.0f)
              handle.m_d->ProgressDelta = 1.0f - handle.m_d->Param;

            handle.m_d->Param = float(handle.m_d->ElapsedTime) / float(handle.m_d->Period);
          }
        }

        return true;
      }

      tmp = tmp->next;
    }
    return false;
  }

  bool TimerHandler::ResumeTimer(TimerHandle& handle)
  {
    NUX_RETURN_VALUE_IF_NULL(handle.m_d, false);
    NUX_RETURN_VALUE_IF_NULL(timer_object_queue_, false);

    if (handle.m_d->state_ != TimerHandler::TIMER_STATE_PAUSED)
    {
      return false;
    }

    TimerObject* tmp;

    // Pointer to the start of the queue.
    tmp = timer_object_queue_;

    // Search for the timer handle
    while (tmp)
    {
      if ((tmp == handle.m_d) && (tmp->uid == handle.m_d->uid))
      {
        handle.m_d->state_ = TimerHandler::TIMER_STATE_RUNNING;

        if (handle.m_d->Type == TIMERTYPE_PERIODIC)
        {
          handle.m_d->glibid = GetWindowThread()->AddTimeout(handle.m_d->Period * (1.0f - handle.m_d->Param));
        }
        else
        {
          handle.m_d->glibid = GetWindowThread()->AddTimeout(handle.m_d->Period);
        }

        handle.m_d->ms_time = g_get_monotonic_time() / 1000;

        if (handle.m_d->glibid == 0)
        {
          _early_timer_objects.push_back(handle.m_d);
        }
        return true;
      }

      tmp = tmp->next;
    }
    return false;
  }

  int TimerHandler::ExecTimerHandler(unsigned int timer_id)
  {
    NUX_RETURN_VALUE_IF_NULL(timer_object_queue_, 0);

    bool repeat = false;
    // Pointer to the start of the timer handler queue.
    TimerObject* timer_object = timer_object_queue_;

    is_processing_timers_ = true;

    while (timer_object != NULL)
    {
      if ((timer_object->glibid == timer_id) && (timer_object->marked_for_removal_ == false))
      {
        if (timer_object->state_ != TIMER_STATE_RUNNING)
        {
          return false;
        }
        // Find the first timer object that 

        timer_object->ElapsedTime += timer_object->Period;

        if (timer_object->Type == TIMERTYPE_PERIODIC)
        {
          timer_object->ProgressDelta = float(timer_object->ElapsedTime) / float(timer_object->Period) - timer_object->Param;
          // Clamp progress delta so(timer_object->Param + timer_object->ProgressDelta) <= 1.0f
          if (timer_object->Param + timer_object->ProgressDelta > 1.0f)
            timer_object->ProgressDelta = 1.0f - timer_object->Param;

          timer_object->Param = float(timer_object->ElapsedTime) / float(timer_object->Period);
        }
        else if (timer_object->Type == TIMERTYPE_DURATION)
        {
          timer_object->ProgressDelta = float(timer_object->ElapsedTime) / float(timer_object->Duration) - timer_object->Param;
          // Clamp progress delta so(timer_object->Param + timer_object->ProgressDelta) <= 1.0f
          if (timer_object->Param + timer_object->ProgressDelta > 1.0f)
            timer_object->ProgressDelta = 1.0f - timer_object->Param;

          if (timer_object->ProgressDelta < 0.0f)
            timer_object->ProgressDelta = 0.0f;

          timer_object->Param = float(timer_object->ElapsedTime) / float(timer_object->Duration);
        }
        else if (timer_object->Type == TIMERTYPE_ITERATION)
        {
          timer_object->ProgressIterationCount += 1;
          int duration = timer_object->Period * timer_object->ScheduledIteration;

          timer_object->ProgressDelta = float(timer_object->ElapsedTime) / float(duration) - timer_object->Param;
          // Clamp progress delta so(timer_object->Param + timer_object->ProgressDelta) <= 1.0f
          if (timer_object->Param + timer_object->ProgressDelta > 1.0f)
            timer_object->ProgressDelta = 1.0f - timer_object->Param;

          timer_object->Param = float(timer_object->ElapsedTime) / float(duration);
        }
        else
        {
          nuxAssertMsg(0, "[TimerHandler::ExecTimerHandler] Unknown timer type.");
        }


        if (timer_object->Param > 1.0f)
        {
          // correction.
          timer_object->Param = 1.0f;
        }

        timer_object->marked_for_removal_ = false;

        if (timer_object->timeout_signal != 0)
        {
          // Execute the signal
          GetWindowThread()->GetWindowCompositor().SetProcessingTopView(timer_object->Window);
          timer_object->timeout_signal->tick.emit(timer_object->CallbackData);
          GetWindowThread()->GetWindowCompositor().SetProcessingTopView(NULL);          
          // Reset glibid to 0. glibid is not null, if this element ever happened to be at the head of the queue
          // and we set a timer for it.
          //nuxDebugMsg("[TimerHandler::ExecTimerHandler] Executed Timeout ID: %d", timer_object->glibid);
          //timer_object->glibid = 0;
        }

        TimerObject* expired_handler = 0;

        if (timer_object->marked_for_removal_)
        {
          // RemoveTimerHandler was called during the callback execution
          expired_handler = timer_object;
        }
        else if (timer_object->Type == TIMERTYPE_PERIODIC)
        {
          // A one shot timer expires after the first execution.
          expired_handler = timer_object;
        }
        else if ((timer_object->Type == TIMERTYPE_DURATION) && (timer_object->Param >= 1.0f))
        {
          // A timer delay timer expires after the duration of the timer as expired.
          expired_handler = timer_object;
        }
        else if ((timer_object->Type == TIMERTYPE_ITERATION) && (timer_object->ProgressIterationCount >= timer_object->ScheduledIteration))
        {
          // An iterative timer expires after the timer as been executedN times.
          expired_handler = timer_object;
        }

        if (!expired_handler && (timer_object->state_ == TIMER_STATE_PAUSED))
        {
          // The state has been changed to "paused".
          return false;
        }

        if(expired_handler)
        {
          GetWindowThread()->GetWindowCompositor().SetProcessingTopView(timer_object->Window);
          timer_object->timeout_signal->expired.emit(timer_object->CallbackData);
          GetWindowThread()->GetWindowCompositor().SetProcessingTopView(NULL);          

          timer_object->state_ = TIMER_STATE_STOPED;

          if (timer_object->next)
            timer_object->next->prev = timer_object->prev;

          if (timer_object->prev)
            timer_object->prev->next = timer_object->next;

          if ((timer_object == timer_object_queue_) && (timer_object->uid == timer_object_queue_->uid))
          {
            // timer_object is the first element of the queue.
            timer_object_queue_ = timer_object->next;

            if (timer_object_queue_)
            {
              timer_object_queue_->prev = 0;
            }
          }

          timer_object = timer_object->next;
          delete expired_handler;
        }
        else
        {
          repeat = true;
        }

        break;
      }
      else
      {
        timer_object = timer_object->next;
      }
    }

    // Purge handles that have been marked for removal
    timer_object = timer_object_queue_;

    while (timer_object)
    {
      if (timer_object->marked_for_removal_)
      {
        TimerObject* expired_handler = timer_object;

        if (timer_object->next)
          timer_object->next->prev = timer_object->prev;

        if (timer_object->prev)
          timer_object->prev->next = timer_object->next;
        else
        {
          // timer_object is the first element of the queue.
          timer_object_queue_ = timer_object->next;

          if (timer_object_queue_)
          {
            timer_object_queue_->prev = 0;
          }
        }

        timer_object = timer_object->next;
        delete expired_handler;
      }
      else
      {
        timer_object = timer_object->next;
      }
    }

    is_processing_timers_ = false;
    return repeat;
  }

  bool TimerHandler::FindTimerHandle(TimerHandle &timer_object)
  {
    TimerObject *tmp = timer_object_queue_;

    while (tmp)
    {
      if (tmp == timer_object.m_d && (tmp->uid == timer_object.m_d->uid))
      {
        return true;
      }

      tmp = tmp->next;
    }

    return false;
  }

//----------------------------------------------------------------------------
  int TimerHandler::DelayUntilNextTimerExpires()
  {
    TimeStruct now;
    TimeStruct delay;

    if (timer_object_queue_ == NULL)
    {
      // The return value of this function is only valid if there _are_ timers active.
      return 0;
    }
    else
    {
      TimeRightNow(&now);

      if (TimeIsGreater(now, timer_object_queue_->when))
      {
        return 0;
      }
      else
      {
        delay.sec = timer_object_queue_->when.sec - now.sec;
        delay.usec = timer_object_queue_->when.usec - now.usec;

        // make sure that usec cannot be less than -1000000 before applying this code
        if (delay.usec < 0)
        {
          delay.usec += 1000000;
          delay.sec--;
        }

        return  (delay.sec * 1000000 + delay.usec) / 1000; // return delay in milliseconds
      }
    }
  }

  /*unsigned int TimeDiff( TimeStruct t1, TimeStruct t2)
  {
      int sec;
      int usec;
      if (t1.sec >= t2.sec)
      {
          sec = t1.sec - t2.sec;
          usec = t1.usec - t2.usec;
          while ((usec < 0) && (sec > 0))
          {
              usec += 1000000;
              sec -= 1;
          }
          if (usec < 0)
              usec = -usec;
      }

      if (t1.sec < t2.sec)
      {
          sec = t2.sec - t1.sec;
          usec = t2.usec - t1.usec;
          while ((usec < 0) && (sec > 0))
          {
              usec += 1000000;
              sec -= 1;
          }
          if (usec < 0)
              usec = -usec;
      }
      return sec*1000 + usec/1000; // time diff is millisecond
  }*/

  bool TimeIsGreater( TimeStruct t1, TimeStruct t2)
  {
    if ((t1.sec > t2.sec) || ((t1.sec == t2.sec) && (t1.usec > t2.usec)))
      return true;

    if ((t1.sec == t2.sec) && (t1.usec == t2.usec))
      return true;

    return false;
  }

  void TimeRightNow(TimeStruct *tv)
  {
#if defined(NUX_OS_WINDOWS)
    struct _timeb timebuffer;
    // Time in seconds since midnight(00:00:00), January 1, 1970, coordinated universal time(UTC).
    _ftime(&timebuffer);
    tv->sec = timebuffer.time;
    tv->usec = timebuffer.millitm * 1000;
#elif defined(NUX_OS_LINUX)
    timeval unix_timeval;
    gettimeofday(&unix_timeval, NULL);
    tv->sec = unix_timeval.tv_sec;
    tv->usec = unix_timeval.tv_usec;
#else
#error TimeRightNow is not implemented for this platform.
#endif
  }

  void Addmillisecs(TimeStruct *tv, unsigned int milliseconds)
  {
    tv->usec += milliseconds * 1000;
    tv->sec += tv->usec / 1000000;
    tv->usec = tv->usec % 1000000;
  }


}
