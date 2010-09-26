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


#ifndef IOPENGLVERTEXBUFFER_H
#define IOPENGLVERTEXBUFFER_H

NAMESPACE_BEGIN_OGL

class IOpenGLResource;
class IOpenGLVertexBuffer: public IOpenGLResource
{
    NUX_DECLARE_OBJECT_TYPE(IOpenGLVertexBuffer, IOpenGLResource);

public:
    virtual ~IOpenGLVertexBuffer();

    int Lock(
        t_u32 OffsetToLock,
        t_u32 SizeToLock,
        void ** ppbData);
    int Unlock();

    int GetDescription(VERTEXBUFFER_DESC* desc) const
    {
        desc->Usage = _Usage;
        desc->Size = _Length;
        return OGL_OK;
    }

    void BindVertexBuffer();
    t_u32 GetSize();

private:
    IOpenGLVertexBuffer(t_u32 Length, VBO_USAGE Usage);

    t_u32 _Length;
    VBO_USAGE _Usage;

    BYTE* _MemMap;
    t_u32  _OffsetToLock;
    t_u32  _SizeToLock;
    friend class GLDeviceFactory;
};

NAMESPACE_END_OGL

#endif // IOPENGLVERTEXBUFFER_H
