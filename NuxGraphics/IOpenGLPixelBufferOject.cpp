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


#include "GLDeviceObjects.h"
#include "IOpenGLPixelBufferOject.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLPixelBufferObject);

  IOpenGLPixelBufferObject::IOpenGLPixelBufferObject(unsigned int Size, VBO_USAGE Usage, NUX_FILE_LINE_DECL)
    :   IOpenGLResource(RTVERTEXBUFFER, NUX_FILE_LINE_PARAM)
    ,   _Length(Size)
    ,   _Usage(Usage)
    ,   _MemMap(0)
    ,   _OffsetToLock(0)
    ,   _SizeToLock(0)
  {
    CHECKGL(glGenBuffersARB(1, &_OpenGLID));
    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, _OpenGLID));
    CHECKGL(glBufferDataARB(GL_ARRAY_BUFFER_ARB, _Length, NULL, Usage));
    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    GRunTimeStats.Register(this);
  }

  IOpenGLPixelBufferObject::~IOpenGLPixelBufferObject()
  {
    CHECKGL(glDeleteBuffersARB(1, &_OpenGLID));
    _OpenGLID = 0;
    GRunTimeStats.UnRegister(this);
  }

  int IOpenGLPixelBufferObject::Lock(
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

#ifndef NUX_OPENGLES_20
//FIXME: GLES 2.0 doesn't support PBOs, we need to allocate client memory and copy data there
    // When locking it shouldn't matter if we use GL_PIXEL_UNPACK_BUFFER_ARB or GL_PIXEL_PACK_BUFFER_ARB.
    // We just want a pointer to the data.
    CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _OpenGLID));
    // Map the Entire buffer into system memory
    _MemMap = (BYTE *) glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE); // we maybe reading or writing to the PBO.
    CHECKGL_MSG(glMapBufferARB);
    *ppbData = (void *) (_MemMap + OffsetToLock);

    _OffsetToLock   = OffsetToLock;
    _SizeToLock     = SizeToLock;

    CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0));
#endif

    return OGL_OK;
  }

  int IOpenGLPixelBufferObject::Unlock()
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _OpenGLID));
    //CHECKGL(glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, _OffsetToLock, _SizeToLock, _MemMap));

    CHECKGL(glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB));
    CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0));
#endif

    _MemMap         = 0;
    _OffsetToLock   = 0;
    _SizeToLock     = 0;
    return OGL_OK;
  }

  void IOpenGLPixelBufferObject::BindPackPixelBufferObject()
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, _OpenGLID));
#endif
  }

  void IOpenGLPixelBufferObject::BindUnpackPixelBufferObject()
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _OpenGLID));
#endif
  }

  unsigned int IOpenGLPixelBufferObject::GetSize()
  {
    return _Length;
  }

}
