#include <glib.h>

gint64 g_fake_monotonic_time = 0;

gint64 g_get_monotonic_time()
{
  return g_fake_monotonic_time;
}
