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


#ifndef IOPENGLVERTEXDECLARATION_H
#define IOPENGLVERTEXDECLARATION_H

namespace nux
{

  class IOpenGLResource;
  class IOpenGLVertexDeclaration: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLVertexDeclaration, IOpenGLResource);

  public:
    virtual ~IOpenGLVertexDeclaration();

//     int GetDeclaration(
//       VERTEXELEMENT *pDecl,
//       unsigned int *pNumElements);

    VERTEXELEMENT GetUsage(ATTRIB_USAGE_DECL usage);
    bool IsUsingMoreThanStreamZero();

  private:
    IOpenGLVertexDeclaration (const VERTEXELEMENT *pVertexElements);

    int _stride[8]; //!< Stride for each stream
    int _valid_vertex_input[16]; // Vertex Input valid for this vertex declaration
    std::vector<VERTEXELEMENT> _declarations_array;
    friend class GpuDevice;
  };

}

#endif // IOPENGLVERTEXDECLARATION_H
