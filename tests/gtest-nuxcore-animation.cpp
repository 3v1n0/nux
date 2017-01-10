/*
 * Now before you go changing any of the advance or tick values in the file,
 * it is worthwhile considering the impact that you'll see on 32 vs 64 bit
 * machines.  Having a 1000 ms animation with 10 x 100ms ticks on a 64 bit
 * machine will seem to give fine integer interpolations to be exactly what is
 * expected, but on a 32 bit machine there are rounding errors.  This is why
 * in a number of the tests, we'll advance 101 or 201 ms instead of a nice
 * round number.
 */

#include "NuxCore/Animation.h"
#include "NuxCore/AnimationController.h"
#include "NuxCore/EasingCurve.h"
#include "NuxCore/Property.h"
#include "NuxCore/PropertyAnimation.h"
#include "NuxCore/Point.h"
#include "NuxCore/Rect.h"
#include "NuxCore/Colors.h"

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
  MOCK_CONST_METHOD0(HasRunningAnimations, bool());
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
  MOCK_CONST_METHOD0(CurrentTimePosition, int());
  MOCK_METHOD1(Advance, void(int));
  MOCK_METHOD0(Restart, void());

};


TEST(TestAnimationController, HasRunningAnimations) {

  na::TickSource source;
  MockAnimationController controller(source);

  ASSERT_FALSE(controller.HasRunningAnimations());

  NiceMock<MockAnimation> animation1;
  NiceMock<MockAnimation> animation2;
  controller.AddAnimation(&animation1);
  controller.AddAnimation(&animation2);
  ASSERT_FALSE(controller.HasRunningAnimations());

  animation1.Start();
  ASSERT_TRUE(controller.HasRunningAnimations());

  animation1.Pause();
  ASSERT_FALSE(controller.HasRunningAnimations());

  animation1.Stop();
  ASSERT_FALSE(controller.HasRunningAnimations());
}

TEST(TestAnimationController, RemoveValueInTick)
{
  na::TickSource source;
  na::AnimationController controller(source);

  std::shared_ptr<na::AnimateValue<int>> animation1(new na::AnimateValue<int>(0,100,1000));
  std::shared_ptr<na::AnimateValue<int>> animation2(new na::AnimateValue<int>(0,100,1000));

  int i = 0;
  animation1->updated.connect([this, &i, &animation2](int)
  {
    if (++i == 2)
    {
      animation2.reset();
    }
  });

  animation1->Start();
  animation2->Start();
  source.tick.emit(10000);

  ASSERT_THAT(animation1->CurrentState(), Eq(na::Animation::Running));
  ASSERT_TRUE(animation2.get() == nullptr);

  animation1.reset();
  animation2.reset();
}

TEST(TestAnimation, TestInitialState)
{
  MockAnimation animation;
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Stopped));
}

TEST(TestAnimation, TestStarting)
{
  NiceMock<MockAnimation> animation; // don't care about restart here
  animation.Start();
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Running));
}

TEST(TestAnimation, TestStoppingEmitsFinished)
{
  NiceMock<MockAnimation> animation; // don't care about restart here
  nt::TestCallback finished_called;
  animation.finished.connect(finished_called.sigc_callback());
  animation.Start();
  animation.Stop();
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Stopped));
  ASSERT_TRUE(finished_called.happened);
}

TEST(TestAnimation, TestDestructorStops)
{
  na::TickSource source;
  MockAnimationController controller(source);

  nt::TestCallback finished_called;
  {
    NiceMock<MockAnimation> animation; // don't care about restart here
    animation.finished.connect(finished_called.sigc_callback());
    animation.Start();
    ASSERT_TRUE(controller.HasRunningAnimations());
  }
  ASSERT_FALSE(controller.HasRunningAnimations());
}

TEST(TestAnimation, TestStoppingStoppedDoesntEmitsFinished)
{
  NiceMock<MockAnimation> animation; // don't care about restart here
  nt::TestCallback finished_called;
  animation.finished.connect(finished_called.sigc_callback());
  animation.Stop();
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Stopped));
  ASSERT_FALSE(finished_called.happened);
}

TEST(TestAnimation, TestCantPauseStopped)
{
  NiceMock<MockAnimation> animation; // don't care about restart here
  animation.Pause();
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Stopped));
}

TEST(TestAnimation, TestPause)
{
  NiceMock<MockAnimation> animation; // don't care about restart here
  animation.Start();
  animation.Pause();
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Paused));
}

TEST(TestAnimation, TestResume)
{
  NiceMock<MockAnimation> animation; // don't care about restart here
  animation.Start();
  animation.Pause();
  animation.Resume();
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Running));
}

TEST(TestAnimation, TestResumeStarted)
{
  NiceMock<MockAnimation> animation; // don't care about restart here
  animation.Start();
  animation.Resume();
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Running));
}

TEST(TestAnimation, TestResumeStopped)
{
  NiceMock<MockAnimation> animation; // don't care about restart here
  animation.Resume();
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


/**
 * Animating values
 */

TEST(TestAnimateValue, TestConstruction)
{
  na::AnimateValue<int> dafault_int_animation;
  ASSERT_THAT(dafault_int_animation.CurrentState(), Eq(na::Animation::Stopped));
  ASSERT_THAT(dafault_int_animation.GetStartValue(), Eq(0));
  ASSERT_THAT(dafault_int_animation.GetCurrentValue(), Eq(0));
  ASSERT_THAT(dafault_int_animation.GetFinishValue(), Eq(0));
  ASSERT_THAT(dafault_int_animation.Duration(), Eq(0));
  ASSERT_THAT(dafault_int_animation.CurrentTimePosition(), Eq(0));

  na::AnimateValue<int> duration_int_animation(10);
  ASSERT_THAT(duration_int_animation.CurrentState(), Eq(na::Animation::Stopped));
  ASSERT_THAT(duration_int_animation.GetStartValue(), Eq(0));
  ASSERT_THAT(duration_int_animation.GetCurrentValue(), Eq(0));
  ASSERT_THAT(duration_int_animation.GetFinishValue(), Eq(0));
  ASSERT_THAT(duration_int_animation.Duration(), Eq(10));
  ASSERT_THAT(duration_int_animation.CurrentTimePosition(), Eq(0));

  na::AnimateValue<int> value_int_animation(10, 20, 1000);
  ASSERT_THAT(value_int_animation.CurrentState(), Eq(na::Animation::Stopped));
  ASSERT_THAT(value_int_animation.GetStartValue(), Eq(10));
  ASSERT_THAT(value_int_animation.GetCurrentValue(), Eq(10));
  ASSERT_THAT(value_int_animation.GetFinishValue(), Eq(20));
  ASSERT_THAT(value_int_animation.Duration(), Eq(1000));
  ASSERT_THAT(value_int_animation.CurrentTimePosition(), Eq(0));
}

TEST(TestAnimateValue, TestStartEmitsInitialValue)
{
  nt::ChangeRecorder<int> recorder;
  na::AnimateValue<int> animation(10, 20, 1000);
  animation.updated.connect(recorder.listener());

  animation.Start();
  ASSERT_THAT(recorder.size(), Eq(1));
  ASSERT_THAT(recorder.changed_values[0], Eq(10));
}

TEST(TestAnimateValue, TestAdvance)
{
  nt::ChangeRecorder<int> recorder;
  nt::TestCallback finished_called;
  na::AnimateValue<int> animation(10, 20, 1000);
  animation.updated.connect(recorder.listener());
  animation.finished.connect(finished_called.sigc_callback());

  animation.Start();
  int advance_msec = 101;
  for (int i = 0; i < 10; ++i)
  {
    int expected_value = advance_msec * (i + 1);

    animation.Advance(advance_msec);

    if (expected_value < animation.Duration())
      ASSERT_THAT(animation.CurrentTimePosition(), Eq(101 * (i + 1)));
  }

  // Advance one more time than necessary
  animation.Advance(advance_msec);

  std::vector<int> expected = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

  ASSERT_THAT(recorder.changed_values, Eq(expected));
  ASSERT_TRUE(finished_called.happened);
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Stopped));
  ASSERT_THAT(animation.GetCurrentValue(), Eq(20));
}

TEST(TestAnimateValue, TestAdvanceOnlyRunning)
{
  nt::ChangeRecorder<int> recorder;
  na::AnimateValue<int> animation(10, 20, 1000);
  animation.updated.connect(recorder.listener());

  animation.Advance(100);
  ASSERT_THAT(recorder.size(), Eq(0));

  animation.Start();
  animation.Advance(400);
  std::vector<int> expected = {10, 14};
  ASSERT_THAT(recorder.changed_values, Eq(expected));

  animation.Pause();
  animation.Advance(400);
  ASSERT_THAT(recorder.changed_values, Eq(expected));

  animation.Resume();
  animation.Advance(400);
  expected.push_back(18);
  ASSERT_THAT(recorder.changed_values, Eq(expected));
}

TEST(TestAnimateValue, TestUsesEasingFunction)
{
  nt::ChangeRecorder<float> recorder;
  na::AnimateValue<float> animation(10, 20, 1000);
  animation.SetEasingCurve(na::EasingCurve(na::EasingCurve::Type::OutQuad));
  animation.updated.connect(recorder.listener());

  animation.Start();
  for (int i = 0; i < 10; ++i)
    animation.Advance(200);

  std::vector<float> expected = {10, 13.6, 16.4, 18.4, 19.6, 20};

  ASSERT_THAT(recorder.size(), Eq(expected.size()));
  // Use FloatEq to check values as calculations may give truncated values.
  for (std::size_t i = 0; i < expected.size(); ++i)
  {
    ASSERT_THAT(recorder.changed_values[i], FloatEq(expected[i]));
  }
}

TEST(TestAnimateValue, TestAnimateIntReverseStopped)
{
  na::AnimateValue<int> animation(10, 20, 1000);
  animation.Reverse();
  ASSERT_THAT(animation.GetStartValue(), Eq(20));
  ASSERT_THAT(animation.GetFinishValue(), Eq(10));
  ASSERT_THAT(animation.CurrentState(), Eq(na::Animation::Stopped));
}

TEST(TestAnimateValue, TestAnimateIntReverse)
{
  const int total_duration = 1000;
  nt::ChangeRecorder<int> recorder;
  na::AnimateValue<int> animation(10, 20, total_duration);
  animation.updated.connect(recorder.listener());

  animation.Start();
  for (int i = 0; i < 3; ++i)
    animation.Advance(201);

  double old_start = animation.GetStartValue();
  double old_finish = animation.GetFinishValue();
  int old_current_time_pos = animation.CurrentTimePosition();

  animation.Reverse();

  EXPECT_EQ(animation.GetStartValue(), old_finish);
  EXPECT_EQ(animation.GetFinishValue(), old_start);
  EXPECT_EQ(animation.CurrentTimePosition(), total_duration - old_current_time_pos);

  for (int i = 0; i < 6; ++i)
    animation.Advance(201);

  std::vector<int> expected = {10, 12, 14, 16, 14, 12, 10};

  ASSERT_THAT(recorder.changed_values, Eq(expected));
}

TEST(TestAnimateValue, TestAnimateIntReversePaused)
{
  const int total_duration = 1000;
  nt::ChangeRecorder<int> recorder;
  na::AnimateValue<int> animation(10, 20, total_duration);
  animation.updated.connect(recorder.listener());

  animation.Start();
  for (int i = 0; i < 3; ++i)
    animation.Advance(201);

  double old_start = animation.GetStartValue();
  double old_finish = animation.GetFinishValue();
  int old_current_time_pos = animation.CurrentTimePosition();

  animation.Pause();
  animation.Reverse();

  EXPECT_EQ(animation.GetStartValue(), old_finish);
  EXPECT_EQ(animation.GetFinishValue(), old_start);
  EXPECT_EQ(animation.CurrentTimePosition(), total_duration - old_current_time_pos);

  animation.Resume();

  for (int i = 0; i < 6; ++i)
    animation.Advance(201);

  std::vector<int> expected = {10, 12, 14, 16, 14, 12, 10};

  ASSERT_THAT(recorder.changed_values, Eq(expected));
}

TEST(TestAnimateValue, TestAnimateIntReverseMultipleTimes)
{
  const int total_duration = 1000;
  const int start_value = 10;
  const int finish_value = 20;

  bool finished = false;
  nt::ChangeRecorder<int> recorder;
  na::AnimateValue<int> animation(start_value, finish_value, total_duration);
  animation.updated.connect(recorder.listener());
  animation.finished.connect([&finished]{ finished = true; });

  animation.Start();
  animation.Advance(201); // 12
  animation.Advance(201); // 14
  animation.Advance(201); // 16
  ASSERT_FALSE(finished);

  int current_time_pos = animation.CurrentTimePosition();
  animation.Reverse(); // decrementing

  ASSERT_EQ(animation.GetStartValue(), finish_value);
  ASSERT_EQ(animation.GetFinishValue(), start_value);
  ASSERT_EQ(animation.CurrentTimePosition(), total_duration - current_time_pos);

  animation.Advance(201); // 14
  animation.Advance(201); // 12
  ASSERT_FALSE(finished);

  current_time_pos = animation.CurrentTimePosition();
  animation.Reverse(); // incrementing
  ASSERT_EQ(animation.GetStartValue(), start_value);
  ASSERT_EQ(animation.GetFinishValue(), finish_value);
  ASSERT_EQ(animation.CurrentTimePosition(), total_duration - current_time_pos);

  animation.Advance(201); // 14
  animation.Advance(201); // 16
  ASSERT_FALSE(finished);

  current_time_pos = animation.CurrentTimePosition();
  animation.Reverse(); // decrementing

  ASSERT_EQ(animation.GetStartValue(), finish_value);
  ASSERT_EQ(animation.GetFinishValue(), start_value);
  ASSERT_EQ(animation.CurrentTimePosition(), total_duration - current_time_pos);

  animation.Advance(201); // 14
  animation.Advance(201); // 12
  animation.Advance(201); // 10
  ASSERT_TRUE(finished);

  std::vector<int> expected = {10, 12, 14, 16, 14, 12, 14, 16, 14, 12, 10};
  ASSERT_EQ(recorder.changed_values, expected);
}


TEST(TestAnimateValue, TestAnimatePoint)
{
  nt::ChangeRecorder<nux::Point> recorder;
  na::AnimateValue<nux::Point> animation(nux::Point(10, 10),
                                         nux::Point(20, 20),
                                         1000);
  animation.updated.connect(recorder.listener());

  animation.Start();
  for (int i = 0; i < 10; ++i)
    animation.Advance(201);

  std::vector<nux::Point> expected = {nux::Point(10,10),
                                      nux::Point(12,12),
                                      nux::Point(14,14),
                                      nux::Point(16,16),
                                      nux::Point(18,18),
                                      nux::Point(20,20)};

  ASSERT_THAT(recorder.changed_values, Eq(expected));
}

TEST(TestAnimateValue, TestAnimateRect)
{
  nt::ChangeRecorder<nux::Rect> recorder;
  na::AnimateValue<nux::Rect> animation(nux::Rect(10, 10, 100, 200),
                                        nux::Rect(20, 20, 200, 400),
                                        1000);
  animation.updated.connect(recorder.listener());

  animation.Start();
  for (int i = 0; i < 10; ++i)
    animation.Advance(200);

  std::vector<nux::Rect> expected = {nux::Rect(10, 10, 100, 200),
                                     nux::Rect(12, 12, 120, 240),
                                     nux::Rect(14, 14, 140, 280),
                                     nux::Rect(16, 16, 160, 320),
                                     nux::Rect(18, 18, 180, 360),
                                     nux::Rect(20, 20, 200, 400)};

  ASSERT_THAT(recorder.changed_values, Eq(expected));
}

TEST(TestAnimateValue, TestAnimateColor)
{
  nt::ChangeRecorder<nux::Color> recorder;
  na::AnimateValue<nux::Color> animation(nux::Color(1.0f, 0.0f, 0.0f),
                                         nux::Color(0.0f, 0.5f, 0.0f),
                                         1000);
  animation.updated.connect(recorder.listener());

  animation.Start();
  for (int i = 0; i < 5; ++i)
    animation.Advance(250);

  std::vector<nux::Color> expected = {nux::Color(1.0f, 0.0f, 0.0f),
                                      nux::Color(0.75f, 0.125f, 0.0f),
                                      nux::Color(0.5f, 0.25f, 0.0f),
                                      nux::Color(0.25f, 0.375f, 0.0f),
                                      nux::Color(0.0f, 0.5f, 0.0f)};

  ASSERT_THAT(recorder.size(), Eq(expected.size()));
  // Use FloatEq to check values as calculations may give truncated values.
  for (std::size_t i = 0; i < expected.size(); ++i)
  {
    nux::Color const& c = recorder.changed_values[i];
    ASSERT_THAT(c.red, FloatEq(expected[i].red));
    ASSERT_THAT(c.green, FloatEq(expected[i].green));
    ASSERT_THAT(c.blue, FloatEq(expected[i].blue));
    ASSERT_THAT(c.alpha, FloatEq(expected[i].alpha));
  }
}


/**
 * Test the ticker hooked up to the animation controller advances animations.
 */

class TestTicker : public na::TickSource
{
public:
  TestTicker() : tick_value(0) {}

  void ms_tick(int ms)
    {
      tick_value += ms * 1000;
      tick.emit(tick_value);
    }

private:
  long long tick_value;
};

class TestAnimationHookup : public Test
{
public:
  TestAnimationHookup()
    : animation_controller(ticker)
    {}

protected:
  TestTicker ticker;
  na::AnimationController animation_controller;
};



TEST_F(TestAnimationHookup, TestSingleAnimation)
{
  nt::ChangeRecorder<int> recorder;
  na::AnimateValue<int> animation(10, 20, 1000);
  animation.updated.connect(recorder.listener());

  // Ticking along with no animations has no impact.
  ticker.ms_tick(101);

  EXPECT_THAT(recorder.size(), Eq(0));

  animation.Start();

  ticker.ms_tick(201);

  std::vector<int> expected = {10, 12};
  EXPECT_THAT(recorder.changed_values, Eq(expected));

  // Pausing means no advancement
  animation.Pause();
  ticker.ms_tick(200);
  EXPECT_THAT(recorder.changed_values, Eq(expected));

  // Resuming allows updates.

  animation.Resume();
  ticker.ms_tick(201);
  expected.push_back(14);
  EXPECT_THAT(recorder.changed_values, Eq(expected));

  animation.Stop();
  ticker.ms_tick(201);
  EXPECT_THAT(recorder.changed_values, Eq(expected));
}


TEST_F(TestAnimationHookup, TestTwoAnimation)
{
  nt::ChangeRecorder<int> int_recorder;
  na::AnimateValue<int>* int_animation;

  nt::ChangeRecorder<double> double_recorder;
  na::AnimateValue<double>* double_animation;

  int_animation = new na::AnimateValue<int>(0, 100, 2000);
  int_animation->updated.connect(int_recorder.listener());
  int_animation->Start();
  ticker.ms_tick(201);

  double_animation = new na::AnimateValue<double>(0, 10, 1000);
  double_animation->updated.connect(double_recorder.listener());
  double_animation->Start();
  ticker.ms_tick(201);
  ticker.ms_tick(201);

  // Removing one animation doesn't impact the other.
  delete double_animation;

  std::vector<double> expected_doubles = {0, 2.01, 4.02};
  // Use DoubleEq to check values as calculations may give truncated values.
  for (std::size_t i = 0; i < expected_doubles.size(); ++i)
  {
    ASSERT_THAT(double_recorder.changed_values[i], DoubleEq(expected_doubles[i]));
  }

  ticker.ms_tick(201);
  ticker.ms_tick(201);
  std::vector<int> expected_ints = {0, 10, 20, 30, 40, 50};
  EXPECT_THAT(int_recorder.changed_values, Eq(expected_ints));

  int_animation->Stop();
  ticker.ms_tick(201);
  EXPECT_THAT(int_recorder.changed_values, Eq(expected_ints));

  delete int_animation;
  // Ticking away with no animations is fine.
  ticker.ms_tick(201);
}

TEST_F(TestAnimationHookup, TestIntProperty)
{
  nux::Property<int> int_property;
  EXPECT_THAT(int_property(), Eq(0));

  std::shared_ptr<na::AnimateValue<int> > anim = na::animate_property(int_property, 10, 20, 1000);
  anim->finished.connect([&anim]() {anim.reset();});

  anim->Start();
  EXPECT_THAT(int_property(), Eq(10));
  ticker.ms_tick(201);
  EXPECT_THAT(int_property(), Eq(12));
  ticker.ms_tick(201);
  EXPECT_THAT(int_property(), Eq(14));
  ticker.ms_tick(301);
  EXPECT_THAT(int_property(), Eq(17));
  ticker.ms_tick(201);
  EXPECT_THAT(int_property(), Eq(19));
  ticker.ms_tick(201);
  EXPECT_THAT(int_property(), Eq(20));
  EXPECT_FALSE(bool(anim));
  ticker.ms_tick(201);
  EXPECT_THAT(int_property(), Eq(20));
}

} // anon namespace
