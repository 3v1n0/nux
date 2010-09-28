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


#ifndef IOPENGLINDEXBUFFER_H
#define IOPENGLINDEXBUFFER_H

namespace nux { //NUX_NAMESPACE_BEGIN

class IOpenGLResource;
class IOpenGLIndexBuffer: public IOpenGLResource
{
    NUX_DECLARE_OBJECT_TYPE(IOpenGLIndexBuffer, IOpenGLResource);

public:
    virtual ~IOpenGLIndexBuffer();

    int Lock(
        t_u32 OffsetToLock,
        t_u32 SizeToLock,
        void ** ppbData);
    int Unlock();

    int GetStride();

    int GetDescription(INDEXBUFFER_DESC* desc) const
    {
        desc->Format = _Format;
        desc->Usage = _Usage;
        desc->Size = _Length;
        return OGL_OK;
    }

    void BindIndexBuffer();
    t_u32 GetSize();

private:
    IOpenGLIndexBuffer(t_u32 Length, VBO_USAGE Usage, INDEX_FORMAT Format);

    t_u32        _Length;
    INDEX_FORMAT   _Format;
    VBO_USAGE      _Usage;

    BYTE*       _MemMap;
    t_u32        _OffsetToLock;
    t_u32        _SizeToLock;
    friend class GLDeviceFactory;
};

} //NUX_NAMESPACE_END

#endif // IOPENGLINDEXBUFFER_H
