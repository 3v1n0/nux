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


#ifndef IOPENGLPIXELBUFFEROBJECT_H
#define IOPENGLPIXELBUFFEROBJECT_H

namespace nux { //NUX_NAMESPACE_BEGIN

class IOpenGLPixelBufferObject: public IOpenGLResource
{
    NUX_DECLARE_OBJECT_TYPE(IOpenGLPixelBufferObject, IOpenGLResource);

public:
    virtual ~IOpenGLPixelBufferObject();

    int Lock(
        unsigned int OffsetToLock,
        unsigned int SizeToLock,
        void ** ppbData);
    int Unlock();

    int GetDescription(INDEXBUFFER_DESC* desc) const
    {
        desc->Usage = _Usage;
        desc->Size = _Length;
        return OGL_OK;
    }

    void BindPackPixelBufferObject();
    void BindUnpackPixelBufferObject();

    t_u32 GetSize();

private:
    IOpenGLPixelBufferObject(unsigned int Size, VBO_USAGE Usage);

    unsigned int _Length;
    VBO_USAGE _Usage;

    BYTE* _MemMap;
    unsigned int  _OffsetToLock;
    unsigned int  _SizeToLock;
    friend class GLDeviceFactory;
};

} //NUX_NAMESPACE_END

#endif // IOPENGLPIXELBUFFEROJECT_H
