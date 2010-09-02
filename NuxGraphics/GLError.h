/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef GLERROR_H
#define GLERROR_H

// WARNING: Never call glGetError between glBegin and glEnd.
// WARNING: Never use CHECKGL between glBegin() and glEnd().
#ifdef _DEBUG

    #define CHECKGL( GLcall )                       \
        {                                           \
            GLcall;                                 \
            if(1)                                   \
            CheckGLError( ANSI_TO_TCHAR(#GLcall), __FILE__, __LINE__ );   \
        }

    #define CHECKGL_MSG( msg )                      \
        {                                           \
            if(1)                                   \
            CheckGLError( ANSI_TO_TCHAR(#msg), __FILE__, __LINE__  );      \
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

int CheckGLError(const TCHAR* GLcall, const TCHAR *file, int line);

#endif // GLERROR_H
