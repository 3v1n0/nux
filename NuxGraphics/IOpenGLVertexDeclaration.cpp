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


#include "GLResource.h"
#include "GLDeviceObjects.h"
#include "IOpenGLVertexDeclaration.h"

NAMESPACE_BEGIN_OGL

IMPLEMENT_OBJECT_TYPE(IOpenGLVertexDeclaration);

IOpenGLVertexDeclaration::IOpenGLVertexDeclaration(const VERTEXELEMENT* pVertexElements)
:IOpenGLResource(RTVERTEXDECLARATION)
{
    for(int i = 0; i < 8; i++)
        Stride[i] = 0;

    int index = 0;
    _DeclarationsArray.clear();
    while(pVertexElements[index].Stream != 0xFF)
    {
        VERTEXELEMENT *pVtxElement = new VERTEXELEMENT;
        *pVtxElement = pVertexElements[index];
        _DeclarationsArray.push_back(pVtxElement);
        Stride[pVertexElements[index].Stream] += GetVertexElementSize(*pVtxElement);
        ++index;
    }
    // Add in the invalid vertex element stream at the end.
    VERTEXELEMENT *pVtxElement = new VERTEXELEMENT;
    pVtxElement->Stream = 0xFF; // invalid stream
    _DeclarationsArray.push_back(pVtxElement);
};

IOpenGLVertexDeclaration::~IOpenGLVertexDeclaration()
{
    // IOpenGLVertexDeclaration is an abstraction. Is does not have an opengl id.
    _OpenGLID = 0;
}

int IOpenGLVertexDeclaration::GetDeclaration(
    VERTEXELEMENT* pDecl,
    unsigned int* pNumElements)
{
    *pNumElements = (unsigned int)_DeclarationsArray.size();
    pDecl = _DeclarationsArray[0];

    return 1;
}


VERTEXELEMENT IOpenGLVertexDeclaration::GetUsage(ATTRIB_USAGE_DECL usage)
{
    VERTEXELEMENT vtxelt;
    vtxelt.Stream = 0xFF; // invalid stream;

    for(unsigned int i = 0; _DeclarationsArray.size(); i++)
    {
        if(_DeclarationsArray[i]->Usage == usage)
        {
            vtxelt = *(_DeclarationsArray[i]);
            return vtxelt;
        }
    }
    return vtxelt;
}

// This is a simple check to comply with the documentation of DrawPrimitiveUP in DirectX
bool IOpenGLVertexDeclaration::IsUsingMoreThanStreamZero()
{
    for(unsigned int i = 0; i < _DeclarationsArray.size(); i++)
    {
        if(_DeclarationsArray[i]->Stream != 0)
        {
            return true;
        }
    }
    return false;
}

NAMESPACE_END_OGL
