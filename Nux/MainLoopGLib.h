#ifndef _NUX_MAIN_LOOP_GLIB_H
#define _NUX_MAIN_LOOP_GLIB_H

#include <glib.h>

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
