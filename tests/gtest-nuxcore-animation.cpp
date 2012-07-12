#include "NuxCore/Animation.h"
#include "NuxCore/AnimationController.h"

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




} // anon namespace
