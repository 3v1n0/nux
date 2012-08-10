#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <glib.h>
#include <gtest/gtest.h>

namespace
{

class Utils
{
public:
  static void WaitUntil(bool& success, unsigned int max_wait = 10)
  {
    bool timeout_reached = false;
    guint32 timeout_id = ScheduleTimeout(&timeout_reached, max_wait * 1000);

    while (!success && !timeout_reached)
      g_main_context_iteration(g_main_context_get_thread_default(), TRUE);

    if (success)
      g_source_remove(timeout_id);

    EXPECT_TRUE(success);
  }

  static guint32 ScheduleTimeout(bool* timeout_reached, unsigned int timeout_duration = 10)
  {
    return g_timeout_add(timeout_duration, TimeoutCallback, timeout_reached);
  }

  static void WaitForTimeout(unsigned int timeout_duration = 10)
  {
    WaitForTimeoutMSec(timeout_duration * 1000);
  }

  static void WaitForTimeoutMSec(unsigned int timeout_duration = 10)
  {
    bool timeout_reached = false;
    guint32 timeout_id = ScheduleTimeout(&timeout_reached, timeout_duration);

    while (!timeout_reached)
      g_main_context_iteration(g_main_context_get_thread_default(), TRUE);

    g_source_remove(timeout_id);
  }

private:
  static gboolean TimeoutCallback(gpointer data)
  {
    *(bool*)data = true;
    return FALSE;
  };
};

}

#endif
