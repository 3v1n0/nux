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


#include "GLDeviceFactory.h"
#include "GLDeviceObjects.h"
#include "IOpenGLIndexBuffer.h"

NAMESPACE_BEGIN_OGL

IMPLEMENT_OBJECT_TYPE(IOpenGLIndexBuffer);

IOpenGLIndexBuffer::IOpenGLIndexBuffer(t_u32 Length, VBO_USAGE Usage, INDEX_FORMAT Format)
:   _Length(Length)
,   _Format(Format)
,   _Usage(Usage)
,   _MemMap(0)
,   _OffsetToLock(0)
,   _SizeToLock(0)
,   IOpenGLResource(RTINDEXBUFFER)
{
    CHECKGL( glGenBuffersARB(1, &_OpenGLID) );
    CHECKGL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _OpenGLID) );
    CHECKGL( glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _Length, NULL, Usage));
    CHECKGL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    GRunTimeStats.Register(this);
}

IOpenGLIndexBuffer::~IOpenGLIndexBuffer()
{
    CHECKGL( glDeleteBuffersARB(1, &_OpenGLID) );
    _OpenGLID = 0;
    GRunTimeStats.UnRegister(this);
}

int IOpenGLIndexBuffer::Lock(
                             t_u32 OffsetToLock,
                             t_u32 SizeToLock,
                             void ** ppbData)
{
    nuxAssert(SizeToLock <= _Length);
    nuxAssert(OffsetToLock + SizeToLock <= _Length);

    if(SizeToLock == 0)
    {
        if(OffsetToLock == 0)
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

    CHECKGL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _OpenGLID) );
    // Map the Entire buffer into system memory
    _MemMap = (BYTE*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB); // todo: use Format instead of GL_WRITE_ONLY_ARB
    CHECKGL_MSG(glMapBufferARB);
    *ppbData = (void*)(_MemMap + OffsetToLock);

    _OffsetToLock   = OffsetToLock;
    _SizeToLock     = SizeToLock;

    CHECKGL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );

    return OGL_OK;
}

int IOpenGLIndexBuffer::Unlock()
{
    nuxAssert(_MemMap != 0);
    nuxAssert(_SizeToLock != 0);

    // No need to bind
    CHECKGL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _OpenGLID) );

    _MemMap         = 0;
    _OffsetToLock   = 0;
    _SizeToLock     = 0;

    CHECKGL( glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB) );
    CHECKGL( glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );

    return OGL_OK;
}

int IOpenGLIndexBuffer::GetStride()
{
    if(_Format == INDEX_FORMAT_USHORT)
    {
        return 2;
    }
    else if(_Format == INDEX_FORMAT_UINT)
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

t_u32 IOpenGLIndexBuffer::GetSize()
{
    return _Length;
}

NAMESPACE_END_OGL
