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

#include <gtest/gtest.h>
#include "Nux/GestureBroker.h"
#include "FakeGestureEvent.h"

using namespace nux;

namespace {
class SimpleTarget : public nux::GestureTarget
{
  public:
    SimpleTarget(int id) : id(id) {}
    virtual ~SimpleTarget() {}
    virtual GestureDeliveryRequest GestureEvent(const nux::GestureEvent &/* event */)
    {
      return GestureDeliveryRequest::NONE;
    }

    void Die()
    {
      died.emit(*this);
    }

    int id;
};

class SimpleGestureBroker : public nux::GestureBroker
{
public:
  virtual ~SimpleGestureBroker() {}
  std::vector<ShPtGestureTarget> targets_to_be_found;

private:
  std::vector<ShPtGestureTarget>
  virtual FindGestureTargets(const nux::GestureEvent &/* event */)
  {
    return targets_to_be_found;
  }
};
}

class GestureBrokerTest : public ::testing::Test
{
 public:
  GestureBrokerTest()
  {
  }

  SimpleGestureBroker gesture_broker;
  nux::FakeGestureEvent fake_event;
};

TEST_F(GestureBrokerTest, RejectGestureIfTargetBecomesUnavailable)
{
  std::shared_ptr <SimpleTarget> target (new SimpleTarget (0));
  nux::TouchPoint touch (0, 1.0f, 1.0f);
  gesture_broker.targets_to_be_found.push_back (target);

  g_gesture_acceptance[0] = 0;

  fake_event.type = nux::EVENT_GESTURE_BEGIN;
  fake_event.gesture_id = 0;
  fake_event.gesture_classes = nux::TOUCH_GESTURE;
  fake_event.timestamp = 0;
  fake_event.touches.clear();
  fake_event.touches.push_back(touch);
  fake_event.is_construction_finished = false;
  gesture_broker.ProcessGestureBegin(fake_event.ToGestureEvent());

  ASSERT_EQ (0, g_gesture_acceptance[0]);

  /* This target is now unavailable */
  target->Die();

  /* Reject the gesture */
  EXPECT_EQ (-1, g_gesture_acceptance[0]);
}

/*
  Let's simulate a situation where a user lays 4 fingers over a trackpad.
  5 different gesture candidates will be generated out of this physical action.
  if touch ids are 0,1,2 and 3, the following gestures will be generated:
  {0,1,2}, {0,1,3}, {0,2,3}, {1,2,3} and {0,1,2,3}

  The gesture canditates with the most fingers must prevail and the others
  must be rejected.
 */
TEST_F(GestureBrokerTest, GestureWithMostTouchesTakesPrecedence)
{
  nux::TouchPoint all_touches[4];
  all_touches[0].id = 0;
  all_touches[0].x = all_touches[0].y = 1.0f;
  all_touches[1].id = 1;
  all_touches[1].x = all_touches[0].y = 2.0f;
  all_touches[2].id = 2;
  all_touches[2].x = all_touches[0].y = 3.0f;
  all_touches[3].id = 3;
  all_touches[3].x = all_touches[0].y = 4.0f;

  gesture_broker.targets_to_be_found.push_back(
      ShPtGestureTarget(new SimpleTarget(0)));

  for (int i = 0; i < 5; ++i)
    g_gesture_acceptance[i] = 0;

  // begin gesture 0 with touches {0,1,2}
  fake_event.type = nux::EVENT_GESTURE_BEGIN;
  fake_event.gesture_id = 0;
  fake_event.gesture_classes = nux::TOUCH_GESTURE;
  fake_event.timestamp = 0;
  fake_event.touches.clear();
  fake_event.touches.push_back(all_touches[0]);
  fake_event.touches.push_back(all_touches[1]);
  fake_event.touches.push_back(all_touches[2]);
  fake_event.is_construction_finished = false;
  gesture_broker.ProcessGestureBegin(fake_event.ToGestureEvent());

  // begin gesture 1 with touches {0,1,3}
  fake_event.gesture_id = 1;
  fake_event.touches.clear();
  fake_event.touches.push_back(all_touches[0]);
  fake_event.touches.push_back(all_touches[1]);
  fake_event.touches.push_back(all_touches[3]);
  gesture_broker.ProcessGestureBegin(fake_event.ToGestureEvent());

  // begin gesture 2 with touches {0,2,3}
  fake_event.gesture_id = 2;
  fake_event.touches.clear();
  fake_event.touches.push_back(all_touches[0]);
  fake_event.touches.push_back(all_touches[2]);
  fake_event.touches.push_back(all_touches[3]);
  gesture_broker.ProcessGestureBegin(fake_event.ToGestureEvent());

  // begin gesture 3 with touches {1,2,3}
  fake_event.gesture_id = 3;
  fake_event.touches.clear();
  fake_event.touches.push_back(all_touches[1]);
  fake_event.touches.push_back(all_touches[2]);
  fake_event.touches.push_back(all_touches[3]);
  gesture_broker.ProcessGestureBegin(fake_event.ToGestureEvent());

  // begin gesture 4 with touches {0,1,2,3}
  fake_event.gesture_id = 4;
  fake_event.touches.clear();
  fake_event.touches.push_back(all_touches[0]);
  fake_event.touches.push_back(all_touches[1]);
  fake_event.touches.push_back(all_touches[2]);
  fake_event.touches.push_back(all_touches[3]);
  gesture_broker.ProcessGestureBegin(fake_event.ToGestureEvent());

  ASSERT_EQ(-1, g_gesture_acceptance[0]);
  ASSERT_EQ(-1, g_gesture_acceptance[1]);
  ASSERT_EQ(-1, g_gesture_acceptance[2]);
  ASSERT_EQ(-1, g_gesture_acceptance[3]);
  ASSERT_EQ(0, g_gesture_acceptance[4]);
}
