#include <gtest/gtest.h>
#include <glib-object.h>

#include "Nux/Nux.h"

#ifdef NUX_GESTURES_SUPPORT
#include "NuxGraphics/GestureEvent.h"
#include "FakeGestureEvent.h"

std::map<int, int> g_gesture_acceptance;

void nux::GestureEvent::Accept()
{
  g_gesture_acceptance[gesture_id_] = g_gesture_acceptance[gesture_id_] + 1;
}

void nux::GestureEvent::Reject()
{
  g_gesture_acceptance[gesture_id_] = g_gesture_acceptance[gesture_id_] - 1;
}
#endif // NUX_GESTURES_SUPPORT

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
#if G_ENCODE_VERSION (GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION) <= GLIB_VERSION_2_34
  g_type_init();
#endif
  nux::NuxInitialize(0);

  return RUN_ALL_TESTS();
}
