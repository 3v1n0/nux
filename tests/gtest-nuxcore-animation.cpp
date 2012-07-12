#include "NuxCore/Animation.h"
#include "NuxCore/AnimationController.h"
#include "NuxCore/EasingCurve.h"

#include "Helpers.h"

#include <gmock/gmock.h>

namespace na = nux::animation;
namespace nt = nux::testing;

using namespace testing;


namespace {

TEST(TestAnimationController, NoInstance) {
  ASSERT_THAT(na::Controller::Instance(), Eq(nullptr));
}

class MockController : public na::Controller
{
  MOCK_METHOD1(AddAnimation, void(na::Animation*));
  MOCK_METHOD1(RemoveAnimation, void(na::Animation*));
};

TEST(TestAnimationController, CreatingControllerSetsInstance) {

  StrictMock<MockController> controller;
  ASSERT_THAT(na::Controller::Instance(), Eq(&controller));
}

TEST(TestAnimationController, DestroyingControllerUnsetsInstance) {

  {
    StrictMock<MockController> controller;
  }
  ASSERT_THAT(na::Controller::Instance(), Eq(nullptr));
}

TEST(TestAnimationController, CreatingSecondControllerEmitsWarning) {

  nt::CaptureLogOutput log_output;
  StrictMock<MockController> controller;
  {
    StrictMock<MockController> controller2;
    // Second controller doesn't set controller
    ASSERT_THAT(na::Controller::Instance(), Eq(&controller));
  }
  // Destructor of second controller doesn't unset instance.
  ASSERT_THAT(na::Controller::Instance(), Eq(&controller));

  std::string log_text = log_output.GetOutput();

  EXPECT_THAT(log_text, StartsWith("WARN"));
  EXPECT_THAT(log_text, HasSubstr("nux.animation"));
  EXPECT_THAT(log_text, HasSubstr("Multiple animation controllers created"));
}


class MockAnimationController : public na::AnimationController
{
public:
  MockAnimationController(na::TickSource& tick_source)
    : na::AnimationController(tick_source)
    {}

  // tick is expected to be ever increasing
  virtual void OnTick(long long tick)
    {
      ticks.push_back(tick);
    }

  std::vector<long long> ticks;
};


TEST(TestAnimationController, TicksOnSourceAreListenedTo) {

  na::TickSource source;
  MockAnimationController controller(source);

  source.tick.emit(10);
  source.tick.emit(100);
  source.tick.emit(10000);

  ASSERT_THAT(controller.ticks.size(), Eq(3));
  ASSERT_THAT(controller.ticks[0], Eq(10));
  ASSERT_THAT(controller.ticks[1], Eq(100));
  ASSERT_THAT(controller.ticks[2], Eq(10000));
}

TEST(TestAnimationController, TicksAfterControllerDtorIgnored) {

  na::TickSource source;
  {
    MockAnimationController controller(source);

    source.tick.emit(10);
  }
  source.tick.emit(100);
}


/**
 * Animation base class testing connections to the AnimationController and
 * basic control functions.
 */

class MockAnimation : public na::Animation
{
public:
  MOCK_CONST_METHOD0(Duration, int());

};

class TestTicker : public na::TickSource
{
public:
  TestTicker() : tick_value(0) {}

  void ms_tick(int ms)
    {
      tick_value += ms * 1000;
      tick.emit(tick_value);
    }

  void multi_tick(int)
    {}

private:
  long long tick_value;
};

class TestAnimation : public Test
{
public:
  TestAnimation()
    : animation_controller(ticker)
    {}

protected:
  TestTicker ticker;
  na::AnimationController animation_controller;
};


TEST_F(TestAnimation, TestInitialState)
{
  MockAnimation animation;
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Stopped));
}

/**
 * Easing curves
 */

TEST(TestEasingCurve, TestLinear) {

  na::EasingCurve curve;
  ASSERT_THAT(curve.ValueForProgress(-0.5), DoubleEq(0));
  ASSERT_THAT(curve.ValueForProgress(0.0), DoubleEq(0));
  ASSERT_THAT(curve.ValueForProgress(0.1), DoubleEq(0.1));
  ASSERT_THAT(curve.ValueForProgress(0.2), DoubleEq(0.2));
  ASSERT_THAT(curve.ValueForProgress(0.3), DoubleEq(0.3));
  ASSERT_THAT(curve.ValueForProgress(0.4), DoubleEq(0.4));
  ASSERT_THAT(curve.ValueForProgress(0.5), DoubleEq(0.5));
  ASSERT_THAT(curve.ValueForProgress(0.6), DoubleEq(0.6));
  ASSERT_THAT(curve.ValueForProgress(0.7), DoubleEq(0.7));
  ASSERT_THAT(curve.ValueForProgress(0.8), DoubleEq(0.8));
  ASSERT_THAT(curve.ValueForProgress(0.9), DoubleEq(0.9));
  ASSERT_THAT(curve.ValueForProgress(1.0), DoubleEq(1));
  ASSERT_THAT(curve.ValueForProgress(1.5), DoubleEq(1));
}

TEST(TestEasingCurve, TestInQuad) {

  na::EasingCurve curve(na::EasingCurve::Type::InQuad);
  ASSERT_THAT(curve.ValueForProgress(-0.5), DoubleEq(0));
  ASSERT_THAT(curve.ValueForProgress(0.0), DoubleEq(0));
  ASSERT_THAT(curve.ValueForProgress(0.1), DoubleEq(0.01));
  ASSERT_THAT(curve.ValueForProgress(0.2), DoubleEq(0.04));
  ASSERT_THAT(curve.ValueForProgress(0.3), DoubleEq(0.09));
  ASSERT_THAT(curve.ValueForProgress(0.4), DoubleEq(0.16));
  ASSERT_THAT(curve.ValueForProgress(0.5), DoubleEq(0.25));
  ASSERT_THAT(curve.ValueForProgress(0.6), DoubleEq(0.36));
  ASSERT_THAT(curve.ValueForProgress(0.7), DoubleEq(0.49));
  ASSERT_THAT(curve.ValueForProgress(0.8), DoubleEq(0.64));
  ASSERT_THAT(curve.ValueForProgress(0.9), DoubleEq(0.81));
  ASSERT_THAT(curve.ValueForProgress(1.0), DoubleEq(1));
  ASSERT_THAT(curve.ValueForProgress(1.5), DoubleEq(1));
}

TEST(TestEasingCurve, TestOutQuad) {

  na::EasingCurve curve(na::EasingCurve::Type::OutQuad);
  ASSERT_THAT(curve.ValueForProgress(-0.5), DoubleEq(0));
  ASSERT_THAT(curve.ValueForProgress(0.0), DoubleEq(0));
  ASSERT_THAT(curve.ValueForProgress(0.1), DoubleEq(0.19));
  ASSERT_THAT(curve.ValueForProgress(0.2), DoubleEq(0.36));
  ASSERT_THAT(curve.ValueForProgress(0.3), DoubleEq(0.51));
  ASSERT_THAT(curve.ValueForProgress(0.4), DoubleEq(0.64));
  ASSERT_THAT(curve.ValueForProgress(0.5), DoubleEq(0.75));
  ASSERT_THAT(curve.ValueForProgress(0.6), DoubleEq(0.84));
  ASSERT_THAT(curve.ValueForProgress(0.7), DoubleEq(0.91));
  ASSERT_THAT(curve.ValueForProgress(0.8), DoubleEq(0.96));
  ASSERT_THAT(curve.ValueForProgress(0.9), DoubleEq(0.99));
  ASSERT_THAT(curve.ValueForProgress(1.0), DoubleEq(1));
  ASSERT_THAT(curve.ValueForProgress(1.5), DoubleEq(1));
}

TEST(TestEasingCurve, TestInOutQuad) {

  na::EasingCurve curve(na::EasingCurve::Type::InOutQuad);
  ASSERT_THAT(curve.ValueForProgress(-0.5), DoubleEq(0));
  ASSERT_THAT(curve.ValueForProgress(0.0), DoubleEq(0));
  ASSERT_THAT(curve.ValueForProgress(0.1), DoubleEq(0.02));
  ASSERT_THAT(curve.ValueForProgress(0.2), DoubleEq(0.08));
  ASSERT_THAT(curve.ValueForProgress(0.3), DoubleEq(0.18));
  ASSERT_THAT(curve.ValueForProgress(0.4), DoubleEq(0.32));
  ASSERT_THAT(curve.ValueForProgress(0.5), DoubleEq(0.5));
  ASSERT_THAT(curve.ValueForProgress(0.6), DoubleEq(0.68));
  ASSERT_THAT(curve.ValueForProgress(0.7), DoubleEq(0.82));
  ASSERT_THAT(curve.ValueForProgress(0.8), DoubleEq(0.92));
  ASSERT_THAT(curve.ValueForProgress(0.9), DoubleEq(0.98));
  ASSERT_THAT(curve.ValueForProgress(1.0), DoubleEq(1));
  ASSERT_THAT(curve.ValueForProgress(1.5), DoubleEq(1));
}

} // anon namespace
