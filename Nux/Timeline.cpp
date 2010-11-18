/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3.0 for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authored by Gordon Allott <gord.allott@canonical.com>
 */
#include "Nux.h"

#include "Timeline.h"

namespace nux
{

  Timeline::Timeline (unsigned int msecs, const TCHAR *Caption, NUX_FILE_LINE_DECL)
    :   Object (Caption, NUX_FILE_LINE_PARAM)
  {
    Looping = false;
    IsPlaying = false;
    Duration = msecs;
    Rewind ();
    Reference (); // get rid of our floating reference
  }

  Timeline::~Timeline ()
  {
  }

  void Timeline::Stop ()
  {
    Pause ();
    Rewind ();
  }

  void Timeline::Start ()
  {
    IsPlaying = true;
    Started.emit ();
  }

  void Timeline::Pause ()
  {
    IsPlaying = false;
    Paused.emit ();
  }

  void Timeline::Rewind ()
  {
    _ElapsedTime = 0;
  }

  double Timeline::GetProgress ()
  {
    return _ElapsedTime / Duration;
  }

  double Timeline::GetEasing ()
  {
    // no easing for the base class
    return GetProgress ();
  }

  void Timeline::DoTick (unsigned long msecs)
  {
    //printf ("number of msecs this dotick: %lu\n", msecs);
    if (msecs < 1)
      return;

    _ElapsedTime += msecs;
    //printf ("_ElapsedTime! %lu\n", _ElapsedTime);
    if (Looping)
      _ElapsedTime %= Duration;

    //printf ("_ElapsedTime! post looping %lu\n", _ElapsedTime);
    unsigned long remainder = 0;
    if (_ElapsedTime > Duration)
    {
      remainder = _ElapsedTime - Duration;
      _ElapsedTime = Duration;
    }

    NewFrame.emit (msecs);

    if (remainder > 0)
      {
        IsPlaying = false;
        Completed.emit ();
        UnReference ();
      }

    //printf ("Number of references %i\n", GetReferenceCount ());
  }
}
