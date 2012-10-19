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


#ifndef GLERROR_H
#define GLERROR_H

// WARNING: Never call glGetError between glBegin and glEnd.
// WARNING: Never use CHECKGL between glBegin() and glEnd().
#ifdef NUX_DEBUG

#define CHECKGL(GLcall )                       \
        {                                           \
            GLcall;                                 \
            if (1)                                   \
            nux::CheckGLError( ANSI_TO_TCHAR(#GLcall), __FILE__, __LINE__ );   \
        }

#define CHECKGL_MSG( msg )                      \
        {                                           \
            if (1)                                   \
            nux::CheckGLError( ANSI_TO_TCHAR(#msg), __FILE__, __LINE__  );      \
        }

#else

#define CHECKGL(GLcall)                         \
        {                                           \
            GLcall;                                 \
        }

#define CHECKGL_MSG( msg )                      \
        {                                           \
        }

#endif

namespace nux
{

  void CheckGLError(const char *GLcall, const char *file, int line);

}

#endif // GLERROR_H

