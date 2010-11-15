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

#ifndef TIMELINE_H
#define TIMELINE_H

namespace nux
{
  class Timeline : public Object
  {
  public:

    Timeline (unsigned int msecs, const TCHAR *Caption, NUX_FILE_LINE_DECL);
    ~Timeline();

    void Stop ();
    void Start ();
    void Pause ();
    void Rewind ();

    double GetProgress (); //ranges from 0.0 -> 1.0
    virtual double GetEasing (); // override to provide easing values

    void DoTick (unsigned long msecs); // If you use this, you make baby kittens cry

    bool Looping;
    unsigned long Duration;
    bool IsPlaying;

    sigc::signal <void, unsigned long> NewFrame;
    sigc::signal <void> Completed;
    sigc::signal <void> Started;
    sigc::signal <void> Paused;

  protected:
    unsigned long _ElapsedTime;
  };
}

#endif // TIMELINE_H
