#ifndef _NUX_MAIN_LOOP_GLIB_H
#define _NUX_MAIN_LOOP_GLIB_H

#include <glib.h>

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))

namespace nux
{
  class ExternalGLibSources
  {
    public:

      void AddFdToGLibLoop(gint fd, gpointer data, GSourceFunc callback, GMainContext *context);
      void RemoveFdFromGLibLoop(gpointer data);
  };
}

#endif

#endif
