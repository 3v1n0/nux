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


#ifndef TIMERPROC_H
#define TIMERPROC_H

NAMESPACE_BEGIN_GUI

typedef struct 
{
    long    sec;         // seconds 
    long    usec;        // and microseconds 
} _Time;

class TimerFunctor : public sigc::trackable
{
public:
    sigc::signal<void, void*> OnTimerExpired;
};

typedef void (*TimerHandleProc)(void *);
typedef void (*IdleHandleProc) (void *);

typedef t_integer TIMERHANDLE;

class TimerHandle
{
public:
    TimerHandle();
    //! Delay before the callback expires
    _Time           when;
    void 		    *CallbackData;
    TimerFunctor    *TimerCallback;

    //! time progression factor between [0.0, 1.0]
    float           Param;
    int             Type;
    int             Iteration;
    int             IterationCount;
    int             Period;         // milliseconds
    int             Duration;      // milliseconds
    int             ElapsedTime;    // milliseconds
    bool            MarkedForRemoval;
    TimerHandle     *next;
    TimerHandle     *prev;

    t_u32           glibid;
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
      The returned TimerHandle should not be deleted by the caller.

      @param Milliseconds   Period delay before the callback is executed.
      @param Callback       The callback to execute when the timer expires.
      @param Data           The callback data
      @return               A handle to the timer.
    */
    TimerHandle* AddTimerHandler(unsigned int Period, TimerFunctor* Callback, void *Data);
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
    TimerHandle* AddPeriodicTimerHandler(unsigned int Period, int Duration, TimerFunctor* Callback, void *Data);
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
    TimerHandle* AddCountIterationTimerHandler(unsigned int Period, int NumberOfIteration, TimerFunctor* Callback, void *Data);

    //! Search for a timer handle.
    /*!
      Search for a timer handle in the timer handle queue. Return true if the timer is found.

      @param handle             Timer handle to search.
      @return                   Return true if the timer is found; false otherwise.
    */
    bool FindTimerHandle(TimerHandle* handle);

    void RemoveTimerWithClientData(void *Data);
    void RemoveTimerHandler(TimerHandle *handler);
    void DelayUntilNextTimerExpires(_Time *delay);

#if (defined(INL_OS_LINUX) || defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(INL_DISABLE_GLIB_LOOP))
    int ExecTimerHandler(t_u32 timer_id);
#else
    int ExecTimerHandler();
#endif

private:
    bool m_IsProceesingTimers;
    TimerHandle* AddHandle(TimerHandle *handle);
    t_u32 GetNumPendingHandler();

    //! Single linked list of timer delays.
    TimerHandle *TimerHandleQueue;
};

NAMESPACE_END_GUI

#endif // TIMERPROC_H

