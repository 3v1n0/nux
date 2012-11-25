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
#include "AnimationController.h"
#include "Animation.h"

#include "Logger.h"

#include <vector>

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
    , ticking_(false)
    {}

  void Add(Animation* anim)
    {
      // never add the same twice
      Animations::iterator end = animations_.end();
      Animations::iterator i = std::find(animations_.begin(), end, anim);
      if (i == end)
      {
        animations_.push_back(anim);
      }
    }

  void Remove(Animation* anim)
    {
      // never add the same twice
      Animations::iterator end = animations_.end();
      Animations::iterator i = std::find(animations_.begin(), end, anim);
      if (i != end)
      {
        animations_.erase(i);
      }
    }

  bool HasRunningAnimations() const
    {
      Animations::const_iterator end = animations_.end();
      Animations::const_iterator it = std::find_if(animations_.begin(), end, [](Animations::value_type item) {
        return item->CurrentState() == Animation::Running;
      });

      return it != end;
    }

  void Tick(long long tick)
    {
      ticking_ = true;

      int ms_since_last_tick = static_cast<int>((tick - last_tick_) / 1000);
      last_tick_ = tick;
      for (Animations::iterator i = animations_.begin(),
             end = animations_.end(); i != end; ++i)
      {
        // only if it isn't pending.
        if (do_not_tick_.empty() ||
            do_not_tick_.find((*i)) == do_not_tick_.end())
        {
          (*i)->Advance(ms_since_last_tick);
        }
      }

      ticking_ = false;

      for (AnimationActions::iterator i = pending_.begin(),
             end = pending_.end(); i != end; ++i)
      {
        if (i->second)
          Add(i->first);
        else
          Remove(i->first);
      }
      pending_.clear();
      do_not_tick_.clear();
    }

  long long last_tick_;
  typedef std::vector<Animation*> Animations;
  Animations animations_;
  typedef std::vector<std::pair<Animation*, bool>> AnimationActions;
  AnimationActions pending_;
  bool ticking_;

  typedef std::set<Animation*> DoNotTickActions;
  DoNotTickActions do_not_tick_;
};

na::AnimationController::AnimationController(na::TickSource& tick_source)
  : pimpl(new Impl)
{
  tick_source.tick.connect(sigc::mem_fun(this, &na::AnimationController::OnTick));
}

na::AnimationController::~AnimationController()
{
  delete pimpl;
}

// tick is expected to be ever increasing
void na::AnimationController::OnTick(long long tick)
{
  pimpl->Tick(tick);
}

void na::AnimationController::AddAnimation(na::Animation* animation)
{
  if (pimpl->ticking_)
  {
    pimpl->pending_.push_back(std::make_pair(animation, true));
  }
  else
  {
    pimpl->Add(animation);
  }
}

void na::AnimationController::RemoveAnimation(na::Animation* animation)
{
  if (pimpl->ticking_)
  {
    pimpl->pending_.push_back(std::make_pair(animation, false));
    pimpl->do_not_tick_.insert(animation);
  }
  else
  {
    pimpl->Remove(animation);
  }
}

bool na::AnimationController::HasRunningAnimations() const
{
  return pimpl->HasRunningAnimations();
}
