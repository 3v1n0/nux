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


#ifndef IOPENGLPIXELBUFFEROBJECT_H
#define IOPENGLPIXELBUFFEROBJECT_H

namespace nux
{

  class IOpenGLPixelBufferObject: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLPixelBufferObject, IOpenGLResource);

  public:
    virtual ~IOpenGLPixelBufferObject();

    int Lock(
      unsigned int OffsetToLock,
      unsigned int SizeToLock,
      void **ppbData);
    int Unlock();

    int GetDescription(INDEXBUFFER_DESC *desc) const
    {
      desc->Usage = _Usage;
      desc->Size = _Length;
      return OGL_OK;
    }

    void BindPackPixelBufferObject();
    void BindUnpackPixelBufferObject();

    unsigned int GetSize();

  private:
    IOpenGLPixelBufferObject(unsigned int Size, VBO_USAGE Usage, NUX_FILE_LINE_PROTO);

    unsigned int _Length;
    VBO_USAGE _Usage;

    BYTE *_MemMap;
    unsigned int  _OffsetToLock;
    unsigned int  _SizeToLock;
    friend class GpuDevice;
  };

}

#endif // IOPENGLPIXELBUFFEROJECT_H
