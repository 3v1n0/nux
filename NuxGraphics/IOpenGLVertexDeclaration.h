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


#ifndef IOPENGLVERTEXDECLARATION_H
#define IOPENGLVERTEXDECLARATION_H

namespace nux { //NUX_NAMESPACE_BEGIN

class IOpenGLResource;
class IOpenGLVertexDeclaration: public IOpenGLResource
{
    NUX_DECLARE_OBJECT_TYPE(IOpenGLVertexDeclaration, IOpenGLResource);

public:
    virtual ~IOpenGLVertexDeclaration();

    int GetDeclaration(
        VERTEXELEMENT* pDecl,
        unsigned int* pNumElements);

    VERTEXELEMENT GetUsage(ATTRIB_USAGE_DECL usage);
    bool IsUsingMoreThanStreamZero();

private:
    IOpenGLVertexDeclaration(const VERTEXELEMENT* pVertexElements);

    int Stride[8]; // Stride for each stream
    int ValideVertexInput[16]; // Vertex Input valid for this vertex declaration
    std::vector<VERTEXELEMENT*> _DeclarationsArray;
    friend class GLDeviceFactory;
};

} //NUX_NAMESPACE_END

#endif // IOPENGLVERTEXDECLARATION_H