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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "IOpenGLVertexBuffer.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (IOpenGLVertexBuffer);

  IOpenGLVertexBuffer::IOpenGLVertexBuffer (t_u32 Length, VBO_USAGE Usage, NUX_FILE_LINE_DECL)
    :   IOpenGLResource (RTVERTEXBUFFER, NUX_FILE_LINE_PARAM)
    ,   _Length (Length)
    ,   _Usage (Usage)
    ,   _MemMap (0)
    ,   _OffsetToLock (0)
    ,   _SizeToLock (0)
  {
    CHECKGL ( glGenBuffersARB (1, &_OpenGLID) );
    CHECKGL ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, _OpenGLID) );
    CHECKGL ( glBufferDataARB (GL_ARRAY_BUFFER_ARB, _Length, NULL, Usage) );
    CHECKGL ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    GRunTimeStats.Register (this);
  }

  IOpenGLVertexBuffer::~IOpenGLVertexBuffer()
  {
    CHECKGL ( glDeleteBuffersARB (1, &_OpenGLID) );
    _OpenGLID = 0;
    GRunTimeStats.UnRegister (this);
  }

  int IOpenGLVertexBuffer::Lock (
    t_u32 OffsetToLock,
    t_u32 SizeToLock,
    void **ppbData)
  {
    nuxAssert (SizeToLock <= _Length);
    nuxAssert (OffsetToLock + SizeToLock <= _Length);

    if (SizeToLock == 0)
    {
      if (OffsetToLock == 0)
      {
        // lock the entire buffer
        SizeToLock = _Length;
      }
      else
      {
        nuxDebugMsg (TEXT ("[IOpenGLVertexBuffer::Lock] Invalid parameters.") );
        return OGL_INVALID_CALL;
      }
    }

    // If _MemMap, _OffsetToLock and _SizeToLock are not equal to zero, then we have already mapped the buffer
    // Unlock it before locking again.
    nuxAssert (_MemMap == 0);
    nuxAssert (_OffsetToLock == 0);
    nuxAssert (_SizeToLock == 0);

    CHECKGL ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, _OpenGLID) );
    // Map the Entire buffer into system memory
    _MemMap = (BYTE *) glMapBufferARB (GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    CHECKGL_MSG (glMapBufferARB);
    *ppbData = (void *) (_MemMap + OffsetToLock);

    _OffsetToLock   = OffsetToLock;
    _SizeToLock     = SizeToLock;

    CHECKGL ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );

    return OGL_OK;
  }

  int IOpenGLVertexBuffer::Unlock()
  {
    CHECKGL ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, _OpenGLID) );
    //CHECKGL( glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, _OffsetToLock, _SizeToLock, _MemMap) );

    CHECKGL ( glUnmapBufferARB (GL_ARRAY_BUFFER_ARB) );
    CHECKGL ( glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );

    _MemMap         = 0;
    _OffsetToLock   = 0;
    _SizeToLock     = 0;
    return OGL_OK;
  }

  void IOpenGLVertexBuffer::BindVertexBuffer()
  {
    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, _OpenGLID) );
  }

  t_u32 IOpenGLVertexBuffer::GetSize()
  {
    return _Length;
  }

}
