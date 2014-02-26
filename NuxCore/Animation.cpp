// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2012 Inalogic® Inc.
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
 * Authored by: Tim Penhey <tim.penhey@canonical.com>
 *
 */

#include "NuxCore.h"
#include "Animation.h"
#include "AnimationController.h"

namespace na = nux::animation;

na::Animation::Animation()
  : state_(Stopped)
{}

na::Animation::~Animation()
{
  if (state_ != Stopped)
  {
    if (Controller* controller = Controller::Instance())
      controller->RemoveAnimation(this);
  }
}

void na::Animation::Pause()
{
  if (state_ == Running)
  {
    state_ = Paused;
  }
}

void na::Animation::Resume()
{
  if (state_ == Paused)
  {
    state_ = Running;
  }
}

void na::Animation::Start()
{
  if (state_ == Stopped)
  {
    state_ = Running;
    Restart();
    Controller* controller = Controller::Instance();
    if (controller)
      controller->AddAnimation(this);
  }
}

void na::Animation::Stop()
{
  if (state_ != Stopped)
  {
    state_ = Stopped;
    finished.emit();
    Controller* controller = Controller::Instance();
    if (controller)
      controller->RemoveAnimation(this);
  }
}

na::Animation::State na::Animation::CurrentState() const
{
  return state_;
}
