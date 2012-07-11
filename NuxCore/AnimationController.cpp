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

namespace na = nux::animation;

namespace
{
na::AnimationController* instance;
}

na::TickSource::~TickSource()
{}


na::AnimationController& na::AnimationController::Instance()
{
  return *instance;
}

na::AnimationController::AnimationController(na::TickSource& tick_source)
  : last_tick_(0)
{
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
