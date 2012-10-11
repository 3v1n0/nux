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

#ifndef NUX_KS_TICK_SOURCE_H
#define NUX_KS_TICK_SOURCE_H

#include <stdint.h>
#include <Nux/TimerProc.h>

/*****
 * OBS: Would be good have animation::TickSource used instead of this,
 *      but that's not posssible at the moment.
 ****/

namespace nux
{
namespace kinetic_scrolling
{

/*!
  A class that emits a signal at constant intervals.
 */
class TickSourceInterface
{
 public:
  virtual ~TickSourceInterface() {}
  virtual void Start() = 0;
  virtual void Stop() = 0;

  /*!
    Emitted at a constant frequency.
    Its parameter is the time elapsed since the previous tick or, if this is
    the very first tick, since the tick source was started, in milliseconds.
   */
  sigc::signal<void, int> tick;
};

/*!
  A TickSource implemented using nux::TimerHandler

  It's way more convenient and testable to use this than nux::TimerHandler
  if all you want is something that calls you at regular intervals.
 */
class TimerTickSource : public TickSourceInterface
{
 public:
  TimerTickSource();
  virtual ~TimerTickSource();

  virtual void Start();
  virtual void Stop();

 private:
  /* update period in milliseconds */
  static const int UPDATE_PERIOD = 16 /* 60 fps */;

  void Tick(void*);
  void TimerExpired(void*);

  TimerHandle timer_handle_;
  TimerFunctor timer_functor_;
  int64_t last_elapsed_time_;
};

} // namespace kinetic_scrolling
} // namespace nux

#endif // NUX_KS_TICK_SOURCE_H
