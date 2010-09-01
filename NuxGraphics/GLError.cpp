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


#include "NuxCore/NKernel.h"
#include "GLResource.h"
#include "GLError.h"

#ifdef _DEBUG
    UBOOL bCheckGLErrors = TRUE;
    UBOOL bBreakOnGLErrors = TRUE;
    UBOOL bVerboseOGLMsg    = FALSE;
#endif

// WARNING: never call glGetError between glBegin and glEnd.
int CheckGLError(const TCHAR* GLcall, const TCHAR *file, int line)
{
    GLenum glErr;
    int    retCode = 0;

    while ( (glErr=glGetError()) != GL_NO_ERROR)
    {

        switch(glErr)
        {
        case GL_INVALID_ENUM:
            nuxDebugMsg(TEXT("[CheckGLError] GL_INVALID_ENUM error in File %s at line: %d"), file, line);
            break;
        case GL_INVALID_VALUE:
            nuxDebugMsg(TEXT("[CheckGLError] GL_INVALID_VALUE error in File %s at line: %d"), file, line);
            break;
        case GL_INVALID_OPERATION:
            nuxDebugMsg(TEXT("[CheckGLError] GL_INVALID_OPERATION error in File %s at line: %d"), file, line);
            break;
        case GL_STACK_OVERFLOW:
            nuxDebugMsg(TEXT("[CheckGLError] GL_STACK_OVERFLOW error in File %s at line: %d"), file, line);
            break;
        case GL_STACK_UNDERFLOW:
            nuxDebugMsg(TEXT("[CheckGLError] GL_STACK_UNDERFLOW error in File %s at line: %d"), file, line);
            break;
        case GL_OUT_OF_MEMORY:
            nuxDebugMsg(TEXT("[CheckGLError] GL_OUT_OF_MEMORY error in File %s at line: %d"), file, line);
            break;
        default:
            nuxDebugMsg(TEXT("[CheckGLError] UNKNOWN ERROR in File %s at line: %d"), file, line);
        }
        nuxDebugMsg(TEXT("[CheckGLError] OpenGL Error %d ( %s )  in File %s at line: %d \n"), glErr, ANSI_TO_TCHAR(gluErrorString(glErr)), ANSI_TO_TCHAR(file), line);
        retCode = 1;

#ifdef _DEBUG
        // break on errors if asked to
        if (bBreakOnGLErrors)
#endif
            inlDebugBreak();
    }
    return retCode;
}
