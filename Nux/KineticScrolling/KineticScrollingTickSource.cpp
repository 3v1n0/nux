/*
 * Copyright (C) 2012 - Canonical Ltd.
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
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 */

#include <Nux/Nux.h>
#include "KineticScrollingTickSource.h"

using namespace nux::kinetic_scrolling;

TimerTickSource::TimerTickSource()
{
  timer_functor_.tick.connect(sigc::mem_fun(this, &TimerTickSource::Tick));
  timer_functor_.expired.connect(sigc::mem_fun(this, &TimerTickSource::TimerExpired));
}

TimerTickSource::~TimerTickSource()
{
  Stop();
}

void TimerTickSource::Start()
{
  timer_handle_ =
    nux::GetTimer().AddDurationTimer(UPDATE_PERIOD,
                                     100000000 /* some arbitrarily long time */,
                                     &timer_functor_, NULL);

  last_elapsed_time_ = 0;
}

void TimerTickSource::Stop()
{
  if (timer_handle_.IsValid())
  {
    nux::GetTimer().RemoveTimerHandler(timer_handle_);
  }
}

void TimerTickSource::Tick(void*)
{
  int delta_time = timer_handle_.GetElapsedTimed() - last_elapsed_time_;
  last_elapsed_time_ = timer_handle_.GetElapsedTimed();

  tick.emit(delta_time);
}

void TimerTickSource::TimerExpired(void*)
{
}

