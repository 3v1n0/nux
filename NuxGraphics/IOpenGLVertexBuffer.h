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


#ifndef IOPENGLVERTEXBUFFER_H
#define IOPENGLVERTEXBUFFER_H

namespace nux
{

  class IOpenGLResource;
  class IOpenGLVertexBuffer: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE (IOpenGLVertexBuffer, IOpenGLResource);

  public:
    virtual ~IOpenGLVertexBuffer();

    int Lock (
      t_u32 OffsetToLock,
      t_u32 SizeToLock,
      void **ppbData);
    int Unlock();

    int GetDescription (VERTEXBUFFER_DESC *desc) const
    {
      desc->Usage = _Usage;
      desc->Size = _Length;
      return OGL_OK;
    }

    void BindVertexBuffer();
    t_u32 GetSize();

  private:
    IOpenGLVertexBuffer (t_u32 Length, VBO_USAGE Usage, NUX_FILE_LINE_PROTO);

    t_u32 _Length;
    VBO_USAGE _Usage;

    BYTE *_MemMap;
    t_u32  _OffsetToLock;
    t_u32  _SizeToLock;
    friend class GpuDevice;
  };

}

#endif // IOPENGLVERTEXBUFFER_H
