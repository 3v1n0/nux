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
//static t_u32 TimeDiff(TimeStruct t1, TimeStruct t2);

  static NThreadSafeCounter TimerUID = 0x01234567;

  class TimerObject
  {
  public:
    TimerObject();

    bool operator == (const TimerObject &timer_object);

    //! Delay before the callback expires
    TimeStruct           when;
    void 		    *CallbackData;
    TimerFunctor    *TimerCallback;

    //! time progression factor between [0.0, 1.0]
    float           Param;
    float           ProgressDelta;
    int             Type;
    int             ScheduledIteration;     //!< The number of scheduled iterations.
    int             ProgressIterationCount; //!< Number of times the timer has been executed.
    int             Period;                 //!< The periode of the timer interuption(in milliseconds).
    int             Duration;               //!< How long the timer will be running from start to finish(in milliseconds);
    int             ElapsedTime;            //!< Elapsed time during execution(in milliseconds).
    bool            MarkedForRemoval;
    BaseWindow      *Window;                 //!< BaseWindow from where the timer was created.
    TimerObject     *next;
    TimerObject     *prev;
    t_u32           glibid;
    t_u32           uid;
  };

  TimerObject::TimerObject()
  {
    Type            = 0;
    CallbackData    = 0;
    TimerCallback   = 0;
    Param           = 0;
    Period          = 0;
    Duration        = 0;
    ElapsedTime     = 0;
    ScheduledIteration       = 0;
    ProgressIterationCount  = 0;
    MarkedForRemoval = 0;
    Window          = 0;
    next            = 0;
    prev            = 0;
    glibid          = 0;
    uid             = 0;
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
  TimerHandler::TimerHandler()
  {
    m_timer_object_queue = 0;
    m_IsProceesingTimers = false;
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
      TimeRightNow(&timer_object->when);
      Addmillisecs(&timer_object->when, timer_object->Period);

#if(defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
        timer_object->glibid = GetWindowThread()->AddGLibTimeout(timer_object->Period);
#endif
    }

    _early_timer_objects.clear();
  }

  TimerHandle TimerHandler::AddTimerHandler(unsigned int Period, TimerFunctor *Callback, void *Data, WindowThread* window_thread)
  {
    TimerObject *timer_object = new TimerObject();

    TimeRightNow(&timer_object->when);
    Addmillisecs(&timer_object->when, Period);

    timer_object->CallbackData  = Data;
    timer_object->TimerCallback = Callback;
    timer_object->Period        = Period;
    timer_object->Type          = TIMERTYPE_PERIODIC;
    if (window_thread)
      timer_object->Window        = window_thread->GetWindowCompositor().GetProcessingTopView();
    else
      timer_object->Window        = GetWindowCompositor().GetProcessingTopView();
    
    AddHandle(timer_object);

#if(defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    {
      if (window_thread)
        timer_object->glibid = window_thread->AddGLibTimeout(Period);
      else
        timer_object->glibid = GetWindowThread()->AddGLibTimeout(Period);
      
      if (timer_object->glibid == 0)
      {
        _early_timer_objects.push_back(timer_object);
        // Probably trying to set a timeout before Glib main context and loop have been created.
        // Sometimes later, this timer will be examined when ExecTimerHandler is called.
        // This happens when trying to set a callback before the mainloop has been initialized.
      }

      //nuxDebugMsg("[TimerHandler::AddTimerHandler] Adding Timeout ID: %d", timer_object->glibid);
    }
#endif

    TimerHandle handle(timer_object);
    return handle;
  }

  TimerHandle TimerHandler::AddPeriodicTimerHandler(unsigned int Period, int Duration, TimerFunctor *Callback, void *Data)
  {
    TimerObject *timer_object = new TimerObject();
    TimeRightNow(&timer_object->when);
    Addmillisecs(&timer_object->when, Period);
    timer_object->CallbackData = Data;
    timer_object->TimerCallback = Callback;

    timer_object->Period = Period;
    timer_object->Duration = (Duration < 0) ? -1 : Duration;
    timer_object->Type = TIMERTYPE_DURATION;
    AddHandle(timer_object);

#if(defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    {
      timer_object->glibid = GetWindowThread()->AddGLibTimeout(Period);

      if (timer_object->glibid == 0)
      {
        _early_timer_objects.push_back(timer_object);
        // Probably trying to set a timeout before Glib main context and loop have been created.
        // Sometimes later, this timer will be examined when ExecTimerHandler is called.
        // This happens when trying to set a callback before the mainloop has been initialized.
      }

      //nuxDebugMsg("[TimerHandler::AddTimerHandler] Adding Timeout ID: %d", timer_object->glibid);
    }
#endif

    TimerHandle handle(timer_object);
    return handle;
  }

  TimerHandle TimerHandler::AddCountIterationTimerHandler(unsigned int Period, int NumberOfIterations, TimerFunctor *Callback, void *Data)
  {
    TimerObject *timer_object = new TimerObject();
    TimeRightNow(&timer_object->when);
    Addmillisecs(&timer_object->when, Period);
    timer_object->CallbackData = Data;
    timer_object->TimerCallback = Callback;

    timer_object->Period = Period;
    timer_object->ScheduledIteration  = (NumberOfIterations < 0) ? -1 : NumberOfIterations;
    timer_object->Type = TIMERTYPE_ITERATION;
    AddHandle(timer_object);

#if(defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    {
      timer_object->glibid = GetWindowThread()->AddGLibTimeout(Period);

      if (timer_object->glibid == 0)
      {
        _early_timer_objects.push_back(timer_object);
        // Probably trying to set a timeout before Glib main context and loop have been created.
        // Sometimes later, this timer will be examined when ExecTimerHandler is called.
        // This happens when trying to set a callback before the mainloop has been initialized.
      }

      //nuxDebugMsg("[TimerHandler::AddTimerHandler] Adding Timeout ID: %d", timer_object->glibid);
    }
#endif
    TimerHandle handle(timer_object);
    return handle;
  }

// Sort timers and add them to the queue
  TimerObject *TimerHandler::AddHandle(TimerObject *timer_object)
  {
    // If the queue is empty or the new timer will expire sooner than the first timer in the queue
    // then add the new timer at the start of the queue.
    if ((m_timer_object_queue == NULL) || TimeIsGreater(m_timer_object_queue->when, timer_object->when))
    {
      // Add the timer timer_object at the head of the queue
      timer_object->next = m_timer_object_queue;

      if (m_timer_object_queue)
        m_timer_object_queue->prev = timer_object;

      timer_object->prev = 0;
      m_timer_object_queue = timer_object;
      return timer_object;
    }

    // Give the Timer a unique ID;
    timer_object->uid = TimerUID.GetValue();
    TimerUID.Increment();

    TimerObject *tmp = m_timer_object_queue;

    while (tmp->next != NULL)
    {
      // Is the time to wait for tmp->next to expire smaller than for timer_object
      if (TimeIsGreater(timer_object->when, tmp->next->when))
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

  t_u32 TimerHandler::GetNumPendingHandler()
  {
    t_u32 count = 0;
    TimerObject *head = m_timer_object_queue;

    while (head)
    {
      count++;
      head = head->next;
    }

    return count;
  }

  bool TimerHandler::RemoveTimerHandler(TimerHandle &timer_object)
  {
    NUX_RETURN_VALUE_IF_NULL(timer_object.m_d, false);
    NUX_RETURN_VALUE_IF_NULL(m_timer_object_queue, false);

    TimerObject *tmp;

    tmp = m_timer_object_queue;

    while (tmp)
    {
      if ((tmp == timer_object.m_d) && (tmp->uid == timer_object.m_d->uid))
      {
        if (!m_IsProceesingTimers)
        {
          if (tmp->next)
            tmp->next->prev = tmp->prev;

          if (tmp->prev)
            tmp->prev->next = tmp->next;

          if ((timer_object.m_d == m_timer_object_queue) && (timer_object.m_d->uid == m_timer_object_queue->uid))
            m_timer_object_queue = timer_object.m_d->next;

          NUX_SAFE_DELETE(timer_object.m_d);
        }
        else
        {
          timer_object.m_d->MarkedForRemoval = true;
        }

        return true;
      }

      tmp = tmp->next;
    }

    return false;
  }

#if(defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
  int TimerHandler::ExecTimerHandler(t_u32 timer_id)
#else
  int TimerHandler::ExecTimerHandler()
#endif
  {
    NUX_RETURN_VALUE_IF_NULL(m_timer_object_queue, 0);

    bool repeat = false;
    TimerObject *timer_object = m_timer_object_queue;
    TimeStruct now;

    int timer_executed = 0;

    TimeRightNow(&now);

    m_IsProceesingTimers = true;

    while (timer_object != NULL)
    {
#if(defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))

      if ((/*TimeIsGreater(now, timer_object->when) ||*/ (timer_object->glibid == timer_id)) && (timer_object->MarkedForRemoval == false))
#else
      if (TimeIsGreater(now, timer_object->when))
#endif
      {
        long elaps = now.sec - timer_object->when.sec;
        long uelaps = now.usec - timer_object->when.usec;

        if (uelaps < 0)
        {
          uelaps += 1000000;
          //nuxAssert(elaps > 0);
          elaps -= 1;
        }

        timer_object->ElapsedTime += timer_object->Period; //elaps * 1000 + uelaps / 1000; // milliseconds

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
          timer_object->Param = 1.0f;

        timer_object->MarkedForRemoval = false;

        if (timer_object->TimerCallback != 0)
        {
          GetWindowCompositor().SetProcessingTopView(timer_object->Window);
          timer_object->TimerCallback->OnTimerExpired.emit(timer_object->CallbackData);
          GetWindowCompositor().SetProcessingTopView(NULL);          
          // Reset glibid to 0. glibid is not null, if this element ever happened to be at the head of the queue
          // and we set a timer for it.
          //nuxDebugMsg("[TimerHandler::ExecTimerHandler] Executed Timeout ID: %d", timer_object->glibid);
          //timer_object->glibid = 0;
        }

        TimerObject *expired_handler = 0;

        if (timer_object->MarkedForRemoval)
        {
          // RemoveTimerHandler was called during the callback execution
          expired_handler = timer_object;
        }
        else if (timer_object->Type == TIMERTYPE_PERIODIC)
        {
          expired_handler = timer_object;
        }
        else if ((timer_object->Type == TIMERTYPE_DURATION) && (timer_object->Param >= 1.0f))
        {
          expired_handler = timer_object;
        }
        else if ((timer_object->Type == TIMERTYPE_ITERATION) && (timer_object->ProgressIterationCount >= timer_object->ScheduledIteration))
        {
          expired_handler = timer_object;
        }

        if (expired_handler)
        {
          if (timer_object->next)
            timer_object->next->prev = timer_object->prev;

          if (timer_object->prev)
            timer_object->prev->next = timer_object->next;

          if ((timer_object == m_timer_object_queue) && (timer_object->uid == m_timer_object_queue->uid))
          {
            // timer_object is the first element of the queue.
            m_timer_object_queue = timer_object->next;

            if (m_timer_object_queue)
            {
              m_timer_object_queue->prev = 0;
            }
          }

          timer_object = timer_object->next;

          delete expired_handler;
        }
        else
        {
          repeat = true;
          //timer_object = timer_object->next;
        }

        timer_executed++;
        break;
      }
      else
      {
        timer_object = timer_object->next;
      }
    }

//     // Look at the head of the queue and set a glib timeout for the first element, if one wasn't set already.
//     if (m_timer_object_queue && (m_timer_object_queue->glibid == 0))
//     {
//         // How long(in milliseconds) between now and the moment the timeout expires?
//         t_u32 time_difference = TimeDiff(now, m_timer_object_queue->when);
//
//         m_timer_object_queue->glibid = GetWindowThread()->AddGLibTimeout(time_difference);
//         //nuxDebugMsg("[TimerHandler::ExecTimerHandler] Adding Timeout ID: %d", m_timer_object_queue->glibid);
//     }

    // Purge handles that have been marked for removal
    timer_object = m_timer_object_queue;

    while (timer_object)
    {
      if (timer_object->MarkedForRemoval)
      {
        TimerObject *expired_handler = timer_object;

        if (timer_object->next)
          timer_object->next->prev = timer_object->prev;

        if (timer_object->prev)
          timer_object->prev->next = timer_object->next;
        else
        {
          // timer_object is the first element of the queue.
          m_timer_object_queue = timer_object->next;

          if (m_timer_object_queue)
          {
            m_timer_object_queue->prev = 0;
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

    m_IsProceesingTimers = false;
    //return timer_executed;
    return repeat;
  }

  bool TimerHandler::FindTimerHandle(TimerHandle &timer_object)
  {
    TimerObject *tmp = m_timer_object_queue;

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

    if (m_timer_object_queue == NULL)
    {
      // The return value of this function is only valid if there _are_ timers active.
      return 0;
    }
    else
    {
      TimeRightNow(&now);

      if (TimeIsGreater(now, m_timer_object_queue->when))
      {
        return 0;
      }
      else
      {
        delay.sec = m_timer_object_queue->when.sec - now.sec;
        delay.usec = m_timer_object_queue->when.usec - now.usec;

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

  /*t_u32 TimeDiff( TimeStruct t1, TimeStruct t2)
  {
      t_s32 sec;
      t_s32 usec;
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
