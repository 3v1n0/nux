/*
 * Copyright 2010-2013 Inalogic® Inc.
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
 *              Marco Trevisan <marco.trevisan@canonical.com>
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
//static unsigned int TimeDiff(TimeStruct t1, TimeStruct t2);

  static NThreadSafeCounter TimerUID = 0x01234567;

  class TimerObject
  {
  public:
    TimerObject();

    bool operator == (const TimerObject &timer_object);

    //! Delay before the callback expires
    TimeStruct      when;
    int             Type;
    gint64          ms_time; // milliseconds 
    void           *CallbackData;
    TimeOutSignal  *timeout_signal;

    //! time progression factor between [0.0, 1.0]
    float           Param;
    float           ProgressDelta;
    int             Period;                 //!< The periode of the timer interuption(in milliseconds).
    int             Duration;               //!< How long the timer will be running from start to finish(in milliseconds);
    int             ElapsedTime;            //!< Elapsed time during execution(in milliseconds).
    int             ScheduledIteration;     //!< The number of scheduled iterations.
    int             ProgressIterationCount; //!< Number of times the timer has been executed.
    bool            marked_for_removal_;
    BaseWindow      *Window;                 //!< BaseWindow from where the timer was created.
    unsigned int           glibid;
    unsigned int           uid;
    TimerHandler::TimerState      state_;
  };

  TimerObject::TimerObject()
    : Type(0)
    , ms_time(0)
    , CallbackData(nullptr)
    , timeout_signal(nullptr)
    , Param(0)
    , ProgressDelta(0)
    , Period(0)
    , Duration(0)
    , ElapsedTime(0)
    , ScheduledIteration(0)
    , ProgressIterationCount(0)
    , marked_for_removal_(0)
    , Window(nullptr)
    , glibid(0)
    , uid(0)
    , state_(TimerHandler::TIMER_STATE_STOPED)
  {}

  TimerHandle::TimerHandle()
  {}

  TimerHandle::TimerHandle(TimerObject *timer_object)
    : m_d(timer_object)
  {}

  TimerHandle::TimerHandle(const TimerHandle &timer_handle)
    : m_d(timer_handle.m_d)
  {}

  TimerHandle& TimerHandle::operator = (const TimerHandle &timer_handle)
  {
    TimerHandle copy(timer_handle);
    std::swap(m_d, copy.m_d);

    return *this;
  }

  std::shared_ptr<TimerObject> const& TimerHandle::operator->() const
  {
    return m_d;
  }

  bool TimerHandle::operator==(TimerHandle const& h) const
  {
    bool equal = (m_d == h.m_d);

    if (equal && m_d)
      equal = (m_d->uid == h.m_d->uid);

    return equal;
  }

  bool TimerHandle::IsValid() const
  {
    return Activated();
  }

  bool TimerHandle::Activated() const
  {
    return m_d != nullptr;
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
  {}

  void TimerHandler::StartEarlyTimerObjects()
  {
    for (auto const& timer_object : early_timer_handlers_)
    {
      timer_object->ms_time = g_get_monotonic_time() / 1000;
      timer_object->glibid = GetWindowThread()->AddTimeout(timer_object->Period);
    }

    early_timer_handlers_.clear();
  }

  TimerHandle TimerHandler::AddOneShotTimer(unsigned int Period, TimeOutSignal* timeout_signal, void* Data, WindowThread* window_thread)
  {
    TimerHandle timer_object(new TimerObject);

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

    {
      if (window_thread)
        timer_object->glibid = window_thread->AddTimeout(Period);
      else
        timer_object->glibid = GetWindowThread()->AddTimeout(Period);

      if (timer_object->glibid == 0)
      {
        early_timer_handlers_.push_back(timer_object);
        // Probably trying to set a timeout before Glib main context and loop have been created.
        // Sometimes later, this timer will be examined when ExecTimerHandler is called.
        // This happens when trying to set a callback before the main loop has been initialized.
      }

      //nuxDebugMsg("[TimerHandler::AddOneShotTimer] Adding Timeout ID: %d", timer_object->glibid);
    }

    return timer_object;
  }

  TimerHandle TimerHandler::AddDurationTimer(unsigned int Period, int Duration, TimeOutSignal *timeout_signal, void *Data)
  {
    TimerHandle timer_object(new TimerObject);

    timer_object->ms_time = g_get_monotonic_time() / 1000;
    timer_object->CallbackData = Data;
    timer_object->timeout_signal = timeout_signal;

    timer_object->Period    = Period;
    timer_object->Duration  = (Duration < 0) ? -1 : Duration;
    timer_object->Type      = TIMERTYPE_DURATION;
    timer_object->state_    = TimerHandler::TIMER_STATE_RUNNING;
    AddHandle(timer_object);

    {
      timer_object->glibid = GetWindowThread()->AddTimeout(Period);

      if (timer_object->glibid == 0)
      {
        early_timer_handlers_.push_back(timer_object);
        // Probably trying to set a timeout before Glib main context and loop have been created.
        // Sometimes later, this timer will be examined when ExecTimerHandler is called.
        // This happens when trying to set a callback before the mainloop has been initialized.
      }

      //nuxDebugMsg("[TimerHandler::AddOneShotTimer] Adding Timeout ID: %d", timer_object->glibid);
    }

    return timer_object;
  }

  TimerHandle TimerHandler::AddIterativeTimer(unsigned int Period, int NumberOfIterations, TimeOutSignal *timeout_signal, void *Data)
  {
    TimerHandle timer_object(new TimerObject);

    timer_object->ms_time = g_get_monotonic_time() / 1000;
    timer_object->CallbackData = Data;
    timer_object->timeout_signal = timeout_signal;

    timer_object->Period              = Period;
    timer_object->ScheduledIteration  = (NumberOfIterations < 0) ? -1 : NumberOfIterations;
    timer_object->Type                = TIMERTYPE_ITERATION;
    timer_object->state_              = TimerHandler::TIMER_STATE_RUNNING;
    AddHandle(timer_object);

    {
      timer_object->glibid = GetWindowThread()->AddTimeout(Period);

      if (timer_object->glibid == 0)
      {
        early_timer_handlers_.push_back(timer_object);
        // Probably trying to set a timeout before Glib main context and loop have been created.
        // Sometimes later, this timer will be examined when ExecTimerHandler is called.
        // This happens when trying to set a callback before the mainloop has been initialized.
      }

      //nuxDebugMsg("[TimerHandler::AddOneShotTimer] Adding Timeout ID: %d", timer_object->glibid);
    }

    return timer_object;
  }

// Sort timers and add them to the queue
  void TimerHandler::AddHandle(TimerHandle const& timer_handle)
  {
    if (!timer_handle.Activated())
      return;

    // Give the Timer a unique ID;
    timer_handle->uid = TimerUID.GetValue();
    TimerUID.Increment();

    auto tmp = timer_handler_queue_.begin();

    while (tmp != timer_handler_queue_.end() && timer_handle->ms_time >= (*tmp)->ms_time)
      ++tmp;

    timer_handler_queue_.insert(tmp, timer_handle);
  }

  unsigned int TimerHandler::GetNumPendingHandler()
  {
    return timer_handler_queue_.size();
  }

  bool TimerHandler::RemoveTimerHandler(TimerHandle &handle)
  {
    if (!handle.Activated() || timer_handler_queue_.empty())
      return false;

    for (auto it = timer_handler_queue_.begin(); it != timer_handler_queue_.end(); ++it)
    {
      if (*it == handle)
      {
        if (!is_processing_timers_)
        {
          handle = nullptr;
          timer_handler_queue_.erase(it);
          return true;
        }
        else
        {
          handle->marked_for_removal_ = true;
        }
      }
    }

    return false;
  }

  bool TimerHandler::PauseTimer(TimerHandle& handle)
  {
    if (!handle.Activated() || timer_handler_queue_.empty())
      return false;

    if (handle->state_ != TimerHandler::TIMER_STATE_RUNNING)
      return false;

    for (auto const& h : timer_handler_queue_)
    {
      if (h == handle)
      {
        handle->state_ = TimerHandler::TIMER_STATE_PAUSED;

        if (!is_processing_timers_)
        {
          gint64 ms_time_now = g_get_monotonic_time() / 1000;

          if (handle->Type == TIMERTYPE_PERIODIC)
          {
            handle->ElapsedTime += (ms_time_now - handle->ms_time);
            handle->ProgressDelta = float(handle->ElapsedTime) / float(handle->Period);

            if (handle->Param + handle->ProgressDelta > 1.0f)
              handle->ProgressDelta = 1.0f - handle->Param;

            handle->Param = float(handle->ElapsedTime) / float(handle->Period);
          }
        }

        return true;
      }
    }

    return false;
  }

  bool TimerHandler::ResumeTimer(TimerHandle& handle)
  {
    if (handle.Activated() || timer_handler_queue_.empty())
      return false;

    if (handle->state_ != TimerHandler::TIMER_STATE_PAUSED)
      return false;

    for (auto const& h : timer_handler_queue_)
    {
      if (h == handle)
      {
        handle->state_ = TimerHandler::TIMER_STATE_RUNNING;

        if (handle->Type == TIMERTYPE_PERIODIC)
        {
          handle->glibid = GetWindowThread()->AddTimeout(handle->Period * (1.0f - handle->Param));
        }
        else
        {
          handle->glibid = GetWindowThread()->AddTimeout(handle->Period);
        }

        handle->ms_time = g_get_monotonic_time() / 1000;

        if (handle->glibid == 0)
          early_timer_handlers_.push_back(handle);

        return true;
      }
    }

    return false;
  }

  int TimerHandler::ExecTimerHandler(unsigned int timer_id)
  {
    if (timer_handler_queue_.empty())
      return 0;

    bool repeat = false;
    is_processing_timers_ = true;

    for (auto it = timer_handler_queue_.begin(); it != timer_handler_queue_.end(); ++it)
    {
      auto const& timer_object = *it;

      if (timer_object->glibid == timer_id && !timer_object->marked_for_removal_)
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

        if (timer_object->timeout_signal)
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

        bool expired_handler = false;

        if (timer_object->marked_for_removal_)
        {
          // RemoveTimerHandler was called during the callback execution
          expired_handler = true;
        }
        else if (timer_object->Type == TIMERTYPE_PERIODIC)
        {
          // A one shot timer expires after the first execution.
          expired_handler = true;
        }
        else if ((timer_object->Type == TIMERTYPE_DURATION) && (timer_object->Param >= 1.0f))
        {
          // A timer delay timer expires after the duration of the timer as expired.
          expired_handler = true;
        }
        else if ((timer_object->Type == TIMERTYPE_ITERATION) && (timer_object->ProgressIterationCount >= timer_object->ScheduledIteration))
        {
          // An iterative timer expires after the timer as been executedN times.
          expired_handler = true;
        }

        if (!expired_handler && (timer_object->state_ == TIMER_STATE_PAUSED))
        {
          // The state has been changed to "paused".
          return false;
        }

        if (expired_handler)
        {
          if (timer_object->timeout_signal)
          {
            GetWindowThread()->GetWindowCompositor().SetProcessingTopView(timer_object->Window);
            timer_object->timeout_signal->expired.emit(timer_object->CallbackData);
            GetWindowThread()->GetWindowCompositor().SetProcessingTopView(NULL);
          }

          timer_object->state_ = TIMER_STATE_STOPED;
          timer_handler_queue_.erase(it);
        }
        else
        {
          repeat = true;
        }

        break;
      }
    }

    // Purge handles that have been marked for removal
    for (auto it = timer_handler_queue_.begin(); it != timer_handler_queue_.end();)
    {
      auto const& timer_object = *it;

      if (timer_object->marked_for_removal_)
      {
        it = timer_handler_queue_.erase(it);
        continue;
      }

      ++it;
    }

    is_processing_timers_ = false;
    return repeat;
  }

  bool TimerHandler::FindTimerHandle(TimerHandle &timer_object)
  {
    auto it = std::find(timer_handler_queue_.begin(), timer_handler_queue_.end(), timer_object);

    return (it != timer_handler_queue_.end());
  }

//----------------------------------------------------------------------------
  int TimerHandler::DelayUntilNextTimerExpires()
  {
    if (timer_handler_queue_.empty())
      return 0;

    TimeStruct now;
    TimeStruct delay;

    TimeRightNow(&now);
    auto const& first = timer_handler_queue_.front();

    if (TimeIsGreater(now, first->when))
    {
      return 0;
    }
    else
    {
      delay.sec = first->when.sec - now.sec;
      delay.usec = first->when.usec - now.usec;

      // make sure that usec cannot be less than -1000000 before applying this code
      if (delay.usec < 0)
      {
        delay.usec += 1000000;
        delay.sec--;
      }

      return  (delay.sec * 1000000 + delay.usec) / 1000; // return delay in milliseconds
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

}
