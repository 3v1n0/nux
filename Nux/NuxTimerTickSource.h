#ifndef NUX_TIMER_TICK_SOURCE_H
#define NUX_TIMER_TICK_SOURCE_H

#include "NuxCore/Animation.h"
#include "NuxCore/AnimationController.h"
#include "Nux/Nux.h"
#include "Nux/TimerProc.h"

namespace nux
{

// Everything inline, but should be extracted.
class NuxTimerTickSource: public animation::TickSource
{
public:
  NuxTimerTickSource()
    : foo(0)
    {
      timer_.tick.connect(sigc::mem_fun(this, &NuxTimerTickSource::Tick));
      timer_.expired.connect(sigc::mem_fun(this, &NuxTimerTickSource::TimerExpired));
      unsigned int period = 16; // ms
      int duration = -1; // run forever
      timer_handle_ = nux::GetTimer().AddDurationTimer(period, duration, &timer_, NULL);
    }

  ~NuxTimerTickSource()
    {
      if (timer_handle_.Activated())
        nux::GetTimer().RemoveTimerHandler(timer_handle_);
    }

private:
  void Tick(void*)
    {
      tick.emit(g_get_monotonic_time());
      if (++foo % 60 == 0)
      {
        // LOG_WARN(logger) << "tick...";
      }
    }
  void TimerExpired(void*)
    {
      //LOG_WARN(logger) << "Timer expired.";
    }

private:
  int foo;
  TimerFunctor timer_;
  TimerHandle timer_handle_;
};

}

#endif // NUX_TIMER_TICK_SOURCE_H
