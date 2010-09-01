/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "Nux.h"
#include "TimerProc.h"

NAMESPACE_BEGIN_GUI

/**
    Return true if t1 is a time reference that will happen after time t2 has expired.
*/
static bool TimeIsGreater(_Time t1, _Time t2);
static void TimeRightNow(_Time *tv);
static void Addmillisecs(_Time *tv, unsigned int milliseconds);
static t_u32 TimeDiff(_Time t1, _Time t2);

TimerHandle::TimerHandle()
{
    Type            = 0;
    CallbackData    = 0;
    TimerCallback   = 0;
    Param           = 0;
    Period          = 0;
    Duration        = 0;
    ElapsedTime     = 0;
    Iteration       = 0;
    IterationCount  = 0;
    MarkedForRemoval= 0;
    next            = 0;
    prev            = 0;
    glibid          = 0;
}

TimerHandler::TimerHandler()
:   TimerHandleQueue(0)
,   m_IsProceesingTimers(false)
{

}

TimerHandler::~TimerHandler()
{

}

// TimerHandle* TimerHandler::AddTimerHandler(unsigned int Milliseconds, TimerFunctor* Callback, void *Data)
// {
//     TimerHandle *handler, *tmp;
// 
//     handler = new TimerHandle();
// 
//     TimeRightNow(&handler->when);
//     Addmillisecs(&handler->when, Milliseconds);
//     handler->CallbackData = Data;
//     handler->TimerCallback = Callback;
// 
//     handler->Duration = Milliseconds;
//     //handler->Frequency = Milliseconds;
//     handler->ElapsedTime = 0;
// 
//     // If the queue is empty or the new timer will expire sooner than the first timer in the queue
//     // then add the new timer at the start of the queue.
//     if((TimerHandleQueue == NULL) || TimeIsGreater(TimerHandleQueue->when, handler->when))
//     {
//         // first in the queue 
//         handler->next = TimerHandleQueue;
//         TimerHandleQueue = handler;
//         return handler;
//     } 
// 
//     tmp = TimerHandleQueue;
//     while(tmp->next != NULL)
//     {
//         if(TimeIsGreater(handler->when, tmp->next->when)) 
//         {
//             tmp = tmp->next;
//         }
//         else
//         {
//             handler->next = tmp->next;
//             tmp->next = handler;
//             return handler;
//         }
//     }
// 
//     handler->next = NULL;
//     tmp->next = handler;
// 
//     return handler;
// }
// 
// void TimerHandler::RemoveTimerWithClientData(void *Data)
// {
//     INL_RETURN_IF_NULL(Data);
//     INL_RETURN_IF_NULL(TimerHandleQueue);
// 
//     TimerHandle *handler, *tmp;
// 
//     handler = TimerHandleQueue;
//     if(handler->CallbackData == Data) 
//     {
//         TimerHandleQueue = handler->next;
//         delete handler;
//     } 
//     else 
//     { 
//         while(handler->next != NULL) 
//         {
//             if(handler->next->CallbackData == Data) 
//             {
//                 tmp = handler->next;
//                 handler->next = handler->next->next;
//                 delete tmp;
//                 break;
//             }
//             handler = handler->next;
//         }
//     }
// }
// 
// void TimerHandler::RemoveTimerHandler(TimerHandle *handler)
// {
//     INL_RETURN_IF_NULL(handler);
//     INL_RETURN_IF_NULL(TimerHandleQueue);
// 
//     TimerHandle *tmp;
// 
//     tmp = TimerHandleQueue;
//     if(tmp == handler)
//     {
//         TimerHandleQueue = handler->next;
//         delete handler;
//     } 
//     else 
//     {
//         while(tmp->next)
//         {
//             if(tmp->next == handler)
//             {
//                 tmp->next = handler->next;
//                 delete handler;
//                 break;
//             }
//             tmp = tmp->next;
//         }
//     }
// }
// 
// int TimerHandler::ExecTimerHandler()
// {
//     INL_RETURN_VALUE_IF_NULL(TimerHandleQueue, 0);
// 
//     TimerHandle *handler;
//     _Time now;
// 
//     int timer_executed = 0;
// 
//     TimeRightNow(&now);
// 
//     while(TimerHandleQueue != NULL)
//     {
//         if(TimeIsGreater(now, TimerHandleQueue->when)) 
//         { 
//             long elaps = now.sec - TimerHandleQueue->when.sec;
//             long uelaps = now.usec - TimerHandleQueue->when.usec;
//             if(uelaps < 0)
//             {
//                 uelaps += 1000000;
//                 nuxAssert(elaps > 0);
//                 elaps -= 1;
//             }
// 
//             handler = TimerHandleQueue;
//             TimerHandleQueue = TimerHandleQueue->next;
// 
//             handler->ElapsedTime += elaps*1000 + uelaps / 1000;
//             handler->Param = float(handler->ElapsedTime) / float(handler->Duration);
//             if(handler->Param > 1.0f)
//                 handler->Param = 1.0f;
// 
//             if(handler->TimerCallback != 0)
//             {
//                 handler->TimerCallback->OnTimerExpired.emit(handler->CallbackData);
//             }
//             delete handler;
//             timer_executed++;
//         }
//         else
//             return timer_executed;
//     }
//     return timer_executed;
// }
////

TimerHandle* TimerHandler::AddTimerHandler(unsigned int Period, TimerFunctor* Callback, void *Data)
{
    TimerHandle *handler = new TimerHandle();
    TimeRightNow(&handler->when);
    Addmillisecs(&handler->when, Period);
    handler->CallbackData = Data;
    handler->TimerCallback = Callback;

    handler->Period = Period;
    handler->Type = TIMERTYPE_PERIODIC;

    AddHandle(handler);

#if (defined(INL_OS_LINUX) || defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(INL_DISABLE_GLIB_LOOP))
    {
        handler->glibid = GetGraphicsThread()->AddGLibTimeout(Period);
        if(handler->glibid == 0)
        {
            // Probably trying to set a timeout before Glib main context and loop have been created.
            // Sometimes later, this timer will be examined when ExecTimerHandler is called.
            // This happens when trying to set a callback before the mainloop has been initialized.
        }
        //nuxDebugMsg(TEXT("[TimerHandler::AddTimerHandler] Adding Timeout ID: %d"), handler->glibid);
    }
#endif

    return handler;
}

TimerHandle* TimerHandler::AddPeriodicTimerHandler(unsigned int Period, int Duration, TimerFunctor* Callback, void *Data)
{
    TimerHandle *handler = new TimerHandle();
    TimeRightNow(&handler->when);
    Addmillisecs(&handler->when, Period);
    handler->CallbackData = Data;
    handler->TimerCallback = Callback;

    handler->Period = Period;
    handler->Duration = (Duration < 0) ? -1 : Duration;
    handler->Type = TIMERTYPE_DURATION;
    return AddHandle(handler);
}

TimerHandle* TimerHandler::AddCountIterationTimerHandler(unsigned int Period, int NumberOfIterations, TimerFunctor* Callback, void *Data)
{
    TimerHandle *handler = new TimerHandle();
    TimeRightNow(&handler->when);
    Addmillisecs(&handler->when, Period);
    handler->CallbackData = Data;
    handler->TimerCallback = Callback;

    handler->Period = Period;
    handler->Iteration  = (NumberOfIterations < 0) ? -1 : NumberOfIterations;
    handler->Type = TIMERTYPE_ITERATION;
    return AddHandle(handler);
}

// Sort timers and add them to the queue
TimerHandle* TimerHandler::AddHandle(TimerHandle *handle)
{
    // If the queue is empty or the new timer will expire sooner than the first timer in the queue
    // then add the new timer at the start of the queue.
    if((TimerHandleQueue == NULL) || TimeIsGreater(TimerHandleQueue->when, handle->when))
    {
        // Add the timer handle at the head of the queue
        handle->next = TimerHandleQueue;
        if(TimerHandleQueue)
            TimerHandleQueue->prev = handle;
        handle->prev = 0;
        TimerHandleQueue = handle;
        return handle;
    } 

    TimerHandle* tmp = TimerHandleQueue;
    while(tmp->next != NULL)
    {
        // Is the time to wait for tmp->next to expire smaller than for handle
        if(TimeIsGreater(handle->when, tmp->next->when)) 
        {
            // keep searching
            tmp = tmp->next;
        }
        else
        {
            handle->next = tmp->next;
            tmp->next->prev = handle;
            tmp->next = handle;
            handle->prev = tmp;
            return handle;
        }
    }

    tmp->next = handle;
    handle->next = NULL;
    handle->prev = tmp;
    return handle;
}

t_u32 TimerHandler::GetNumPendingHandler()
{
    t_u32 count = 0;
    TimerHandle* head = TimerHandleQueue;
    while(head)
    {
        count++;
        head = head->next;
    }
    return count;
}

void TimerHandler::RemoveTimerWithClientData(void *Data)
{
    INL_RETURN_IF_NULL(Data);
    INL_RETURN_IF_NULL(TimerHandleQueue);

    TimerHandle *handler, *tmp;

    handler = TimerHandleQueue;
    if(handler->CallbackData == Data)
    {
        TimerHandleQueue = handler->next;
        if(TimerHandleQueue)
            TimerHandleQueue->prev = 0;
        delete handler;
    }
    else
    {
        while(handler) 
        {
            if(handler->CallbackData == Data) 
            {
                tmp = handler;
                if(handler->next)
                    handler->next->prev = handler->prev;
                if(handler->prev)
                    handler->prev->next = handler->next;
                delete tmp;
                break;
            }
            handler = handler->next;
        }
    }
}

void TimerHandler::RemoveTimerHandler(TimerHandle *handler)
{
    INL_RETURN_IF_NULL(handler);
    INL_RETURN_IF_NULL(TimerHandleQueue);

    TimerHandle *tmp;

    tmp = TimerHandleQueue;
//     if(tmp == handler)
//     {
//         if(!m_IsProceesingTimers)
//         {
//             TimerHandleQueue = handler->next;
//             if(TimerHandleQueue)
//                 TimerHandleQueue->prev = 0;
//             delete handler;
//         }
//         else
//         {
//             handler->MarkedForRemoval = true;
//         }
//     } 
//     else 
    {
        while(tmp)
        {
            if(tmp == handler)
            {
                if(!m_IsProceesingTimers)
                {
                    if(tmp->next)
                        tmp->next->prev = tmp->prev;
                    if(tmp->prev)
                        tmp->prev->next = tmp->next;
                    if(handler == TimerHandleQueue)
                        TimerHandleQueue = handler->next;
                    delete handler;
                }
                else
                {
                    handler->MarkedForRemoval = true;
                }
                break;
            }
            tmp = tmp->next;
        }
    }
}

#if (defined(INL_OS_LINUX) || defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(INL_DISABLE_GLIB_LOOP))
int TimerHandler::ExecTimerHandler(t_u32 timer_id)
#else
int TimerHandler::ExecTimerHandler()
#endif
{
    INL_RETURN_VALUE_IF_NULL(TimerHandleQueue, 0);

    TimerHandle *handler = TimerHandleQueue;
    _Time now;

    int timer_executed = 0;

    TimeRightNow(&now);

    m_IsProceesingTimers = true;
    while(handler != NULL)
    {
#if (defined(INL_OS_LINUX) || defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(INL_DISABLE_GLIB_LOOP))
        if((TimeIsGreater(now, handler->when) || (handler->glibid == timer_id)) && (handler->MarkedForRemoval == false))
#else
        if(TimeIsGreater(now, handler->when))
#endif
        { 
            long elaps = now.sec - handler->when.sec;
            long uelaps = now.usec - handler->when.usec;
            if(uelaps < 0)
            {
                uelaps += 1000000;
                //nuxAssert(elaps > 0);
                elaps -= 1;
            }

            handler->ElapsedTime += elaps*1000 + uelaps / 1000; // milliseconds

            if(handler->Type == TIMERTYPE_PERIODIC)
            {
                handler->Param = float(handler->ElapsedTime) / float(handler->Period);
            }
            else if(handler->Type == TIMERTYPE_DURATION)
            {
                handler->Param = float(handler->ElapsedTime) / float(handler->Duration);
            }
            else if(handler->Type == TIMERTYPE_ITERATION)
            {
                handler->IterationCount += 1;
                int duration = handler->Period * handler->Iteration;
                handler->Param = float(handler->ElapsedTime) / float(duration);
            }
            else
            {
                nuxAssertMsg(0, TEXT("[TimerHandler::ExecTimerHandler] Unknown timer type."));
            }

            if(handler->Param > 1.0f)
                handler->Param = 1.0f;

            handler->MarkedForRemoval = false;
            if(handler->TimerCallback != 0)
            {
                handler->TimerCallback->OnTimerExpired.emit(handler->CallbackData);
                // Reset glibid to 0. glibid is not null, if this element ever happened to be at the head of the queue 
                // and we set a timer for it.
                //nuxDebugMsg(TEXT("[TimerHandler::ExecTimerHandler] Executed Timeout ID: %d"), handler->glibid);
                handler->glibid = 0;
            }

            TimerHandle *expired_handler = 0;
            if(handler->MarkedForRemoval)
            {
                // RemoveTimerHandler was called during the callback execution
                expired_handler = handler;
            }
            else if(handler->Type == TIMERTYPE_PERIODIC)
            {
                expired_handler = handler;
            }
            else if((handler->Type == TIMERTYPE_PERIODIC) && (handler->Param >= 1.0f))
            {
                expired_handler = handler;
            }
            else if((handler->Type == TIMERTYPE_ITERATION) && (handler->IterationCount >= handler->Iteration))
            {
                expired_handler = handler;
            }
            else
            {
                nuxAssertMsg(0, TEXT("[TimerHandler::ExecTimerHandler] Unknown timer type."));
            }

            if(expired_handler)
            {
                if(handler->next)
                    handler->next->prev = handler->prev;
                if(handler->prev)
                    handler->prev->next = handler->next;
                
                if(handler == TimerHandleQueue)
                {
                    // handler is the first element of the queue.
                    TimerHandleQueue = handler->next;
                    if(TimerHandleQueue)
                    {
                        TimerHandleQueue->prev = 0;
                    }
                }

                handler = handler->next;

                delete expired_handler;
            }
            else
            {
                handler = handler->next;
            }
            timer_executed++;
        }
        else
        {
            handler = handler->next;
        }
    }

//     // Look at the head of the queue and set a glib timeout for the first element, if one wasn't set already.
//     if(TimerHandleQueue && (TimerHandleQueue->glibid == 0))
//     {
//         // How long (in milliseconds) between now and the moment the timeout expires?
//         t_u32 time_difference = TimeDiff(now, TimerHandleQueue->when);
//         
//         TimerHandleQueue->glibid = GetGraphicsThread()->AddGLibTimeout(time_difference);
//         //nuxDebugMsg(TEXT("[TimerHandler::ExecTimerHandler] Adding Timeout ID: %d"), TimerHandleQueue->glibid);
//     }

    // Purge handles that have been marked for removal
    handler = TimerHandleQueue;
    while(handler)
    {
        if(handler->MarkedForRemoval)
        {
            TimerHandle *expired_handler = handler;
            if(handler->next)
                handler->next->prev = handler->prev;
            if(handler->prev)
                handler->prev->next = handler->next;
            else
            {
                // handler is the first element of the queue.
                TimerHandleQueue = handler->next;
                if(TimerHandleQueue)
                {
                    TimerHandleQueue->prev = 0;
                }
            }

            handler = handler->next;
            delete expired_handler;
        }
        else
        {
            handler = handler->next;
        }
    } 

    m_IsProceesingTimers = false;
    return timer_executed;
}

bool TimerHandler::FindTimerHandle(TimerHandle* handle)
{
    TimerHandle *tmp = TimerHandleQueue;

    while(tmp)
    {
        if(tmp == handle)
        {
            return true;
        }
        tmp = tmp->next;
    }
    return false;
}

//----------------------------------------------------------------------------
void TimerHandler::DelayUntilNextTimerExpires(_Time *delay)
{
    _Time now;

    if(TimerHandleQueue == NULL) 
    {
        // The return value of this function is only valid if there _are_ timers active. 
        delay->sec = 0;
        delay->usec = 0;
    }
    else
    {
        TimeRightNow(&now);
        if(TimeIsGreater(now, TimerHandleQueue->when)) 
        {
            delay->sec = 0;
            delay->usec = 0;
        } 
        else
        {
            delay->sec = TimerHandleQueue->when.sec - now.sec;
            delay->usec = TimerHandleQueue->when.usec - now.usec;

            // make sure that usec cannot be less than -1000000 before applying this code
            if(delay->usec < 0) 
            {
                delay->usec += 1000000;
                delay->sec--;
            }
        }
    }
}

t_u32 TimeDiff( _Time t1, _Time t2)
{
    t_s32 sec;
    t_s32 usec;
    if(t1.sec >= t2.sec)
    {
        sec = t1.sec - t2.sec;
        usec = t1.usec - t2.usec;
        while((usec < 0) && (sec > 0))
        {
            usec += 1000000;
            sec -= 1;
        }
        if(usec < 0)
            usec = -usec;
    }

    if(t1.sec < t2.sec)
    {
        sec = t2.sec - t1.sec;
        usec = t2.usec - t1.usec;
        while((usec < 0) && (sec > 0))
        {
            usec += 1000000;
            sec -= 1;
        }
        if(usec < 0)
            usec = -usec;
    }
    return sec*1000 + usec/1000; // time diff is millisecond
}

bool TimeIsGreater( _Time t1, _Time t2)
{
    if((t1.sec > t2.sec) || ((t1.sec == t2.sec) && (t1.usec > t2.usec)))
        return true;

    if((t1.sec == t2.sec) && (t1.usec == t2.usec))
        return true;

    return false;
}

void TimeRightNow(_Time *tv) 
{
#if defined(INL_OS_WINDOWS)
    struct _timeb timebuffer;
    // Time in seconds since midnight (00:00:00), January 1, 1970, coordinated universal time (UTC).
    _ftime(&timebuffer); 
    tv->sec = timebuffer.time;
    tv->usec = timebuffer.millitm * 1000;
#elif defined(INL_OS_LINUX)
    timeval unix_timeval;
    gettimeofday(&unix_timeval, NULL);
    tv->sec = unix_timeval.tv_sec;
    tv->usec = unix_timeval.tv_usec;
#else
    #error TimeRightNow is not implemented for this platform.
#endif
}

void Addmillisecs(_Time *tv, unsigned int milliseconds)
{
    tv->usec += milliseconds * 1000;
    tv->sec += tv->usec / 1000000;
    tv->usec = tv->usec % 1000000;
}


NAMESPACE_END_GUI
