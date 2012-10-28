/*
 * Copyright 2010 Inalogic® Inc.
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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include "GLResource.h"
#include "FontTexture.h"
#include "GLError.h"

#include "NuxCore/Logger.h"

namespace nux
{
DECLARE_LOGGER(logger, "nux.gl");
namespace
{
#ifdef NUX_DEBUG
bool BreakOnGLErrors = false;
#endif

bool debug_glerror_stack()
{
  // If the extra log environment variable is set the stack trace for failing
  // GLError checks will also get a backtrace.
  static bool extra_debugging(::getenv("NUX_DEBUG_GLERROR_STACK"));
  return extra_debugging;
}

}


// WARNING: never call glGetError between glBegin and glEnd.
  void CheckGLError(const char * /* GLcall */, const char *file, int line)
  {
    GLenum glErr;
    std::string error_msg;

    while ((glErr = glGetError()) != GL_NO_ERROR)
    {

      switch(glErr)
      {
        case GL_INVALID_ENUM:
          error_msg += "[CheckGLError] GL_INVALID_ENUM";
          break;
        case GL_INVALID_VALUE:
          error_msg += "[CheckGLError] GL_INVALID_VALUE";
          break;
        case GL_INVALID_OPERATION:
          error_msg += "[CheckGLError] GL_INVALID_OPERATION";
          break;
#ifndef NUX_OPENGLES_20
        case GL_STACK_OVERFLOW:
          error_msg += "[CheckGLError] GL_STACK_OVERFLOW";
          break;
        case GL_STACK_UNDERFLOW:
          error_msg += "[CheckGLError] GL_STACK_UNDERFLOW";
          break;
#endif
        case GL_OUT_OF_MEMORY:
          error_msg += "[CheckGLError] GL_OUT_OF_MEMORY";
          break;
        default:
          error_msg += "[CheckGLError] UNKNOWN ERROR, ";
      }

      if (Unwrap(logger).IsWarningEnabled() && !error_msg.empty())
      {
        std::string stacktrace;
#if defined(NUX_OS_LINUX)
        if (debug_glerror_stack())
          stacktrace = "\n" + logging::Backtrace();
#endif
        logging::LogStream(logging::Warning, Unwrap(logger).module(), file, line).stream()
#ifndef NUX_OPENGLES_20
          << "[CheckGLError] OpenGL Error " << glErr << " (" << gluErrorString(glErr) << ")"
#else
          << error_msg
#endif
          << stacktrace;
      }

#ifdef NUX_DEBUG
      // break on errors if asked to
      if (BreakOnGLErrors)
        inlDebugBreak();
#endif
    }
  }

}
