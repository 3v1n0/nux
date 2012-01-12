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


#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "IOpenGLIndexBuffer.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLIndexBuffer);

  IOpenGLIndexBuffer::IOpenGLIndexBuffer(unsigned int Length, VBO_USAGE Usage, INDEX_FORMAT Format, NUX_FILE_LINE_DECL)
    :   IOpenGLResource(RTINDEXBUFFER, NUX_FILE_LINE_PARAM)
    ,   _Length(Length)
    ,   _Format(Format)
    ,   _Usage(Usage)
    ,   _MemMap(0)
    ,   _OffsetToLock(0)
    ,   _SizeToLock(0)
  {
    CHECKGL(glGenBuffersARB(1, &_OpenGLID));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _OpenGLID));
    CHECKGL(glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _Length, NULL, Usage));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    GRunTimeStats.Register(this);
  }

  IOpenGLIndexBuffer::~IOpenGLIndexBuffer()
  {
    CHECKGL(glDeleteBuffersARB(1, &_OpenGLID));
    _OpenGLID = 0;
    GRunTimeStats.UnRegister(this);
  }

  int IOpenGLIndexBuffer::Lock(
    unsigned int OffsetToLock,
    unsigned int SizeToLock,
    void **ppbData)
  {
    nuxAssert(SizeToLock <= _Length);
    nuxAssert(OffsetToLock + SizeToLock <= _Length);

    if (SizeToLock == 0)
    {
      if (OffsetToLock == 0)
      {
        // lock the entire buffer
        SizeToLock = _Length;
      }
      else
        return OGL_INVALID_CALL;
    }

    // If _MemMap, _OffsetToLock and _SizeToLock are not equal to zero, then we have already mapped the buffer
    // Unlock it before locking again.
    nuxAssert(_MemMap == 0);
    nuxAssert(_OffsetToLock == 0);
    nuxAssert(_SizeToLock == 0);

    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _OpenGLID));
    // Map the Entire buffer into system memory
#ifndef NUX_OPENGLES_20
    _MemMap = (BYTE *) glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB); // todo: use Format instead of GL_WRITE_ONLY_ARB
    CHECKGL_MSG(glMapBufferARB);
    *ppbData = (void *) (_MemMap + OffsetToLock);
#else
    _MemMap = new BYTE[SizeToLock];
    *ppbData = _MemMap;
#endif

    _OffsetToLock   = OffsetToLock;
    _SizeToLock     = SizeToLock;

    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    return OGL_OK;
  }

  int IOpenGLIndexBuffer::Unlock()
  {
    nuxAssert(_MemMap != 0);
    nuxAssert(_SizeToLock != 0);

    // No need to bind
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _OpenGLID));

#ifndef NUX_OPENGLES_20
    CHECKGL(glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB));
#else
    CHECKGL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER_ARB, _OffsetToLock, _SizeToLock, _MemMap));
    delete [] _MemMap;
#endif

    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    _MemMap         = 0;
    _OffsetToLock   = 0;
    _SizeToLock     = 0;

    return OGL_OK;
  }

  int IOpenGLIndexBuffer::GetStride()
  {
    if (_Format == INDEX_FORMAT_USHORT)
    {
      return 2;
    }
    else if (_Format == INDEX_FORMAT_UINT)
    {
      return 4;
    }
    else
      return 0;
  }

  void IOpenGLIndexBuffer::BindIndexBuffer()
  {
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _OpenGLID));
  }

  unsigned int IOpenGLIndexBuffer::GetSize()
  {
    return _Length;
  }

}
