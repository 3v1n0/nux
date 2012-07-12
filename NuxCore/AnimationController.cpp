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

#include "AnimationController.h"

#include "Logger.h"

namespace na = nux::animation;
namespace nl = nux::logging;

namespace
{
// Yes I know the compiler does that, but can't help being explicit.
na::Controller* controller_instance = nullptr;
nl::Logger logger("nux.animation");
}

na::TickSource::~TickSource()
{}


na::Controller* na::Controller::Instance()
{
  return controller_instance;
}

na::Controller::Controller()
{
  if (controller_instance)
  {
    LOG_WARNING(logger) << "Multiple animation controllers created.";
  }
  else
  {
    controller_instance = this;
  }
}

na::Controller::~Controller()
{
  if (controller_instance == this)
  {
    controller_instance = nullptr;
  }
}

struct na::AnimationController::Impl
{
  Impl()
    : last_tick_(0)
    {}

  long long last_tick_;
};

na::AnimationController::AnimationController(na::TickSource& tick_source)
  : pimpl(new Impl)
{
}

na::AnimationController::~AnimationController()
{
  delete pimpl;
}

// tick is expected to be ever increasing
void na::AnimationController::OnTick(long long tick)
{
}

void na::AnimationController::AddAnimation(na::Animation* animation)
{
}

void na::AnimationController::RemoveAnimation(na::Animation* animation)
{
}
