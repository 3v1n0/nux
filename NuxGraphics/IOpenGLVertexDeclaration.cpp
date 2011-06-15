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


#include "GLResource.h"
#include "GLDeviceObjects.h"
#include "IOpenGLVertexDeclaration.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (IOpenGLVertexDeclaration);

  IOpenGLVertexDeclaration::IOpenGLVertexDeclaration(const VERTEXELEMENT* pVertexElements)
    : IOpenGLResource(RTVERTEXDECLARATION)
  {
    for(int i = 0; i < 8; i++)
      _stride[i] = 0;

    int index = 0;
    _declarations_array.clear();

    while (pVertexElements[index].Stream != 0xFF)
    {
      VERTEXELEMENT VtxElement = pVertexElements[index];
      _declarations_array.push_back(VtxElement);
      _stride[VtxElement.Stream] += GetVertexElementSize(VtxElement);
      ++index;
    }

    // Add in the invalid vertex element stream at the end.
    //VERTEXELEMENT *pVtxElement = new VERTEXELEMENT;
    //pVtxElement->Stream = 0xFF; // invalid stream
    _declarations_array.push_back (DECL_END);
  };

  IOpenGLVertexDeclaration::~IOpenGLVertexDeclaration()
  {
    // IOpenGLVertexDeclaration is an abstraction. Is does not have an opengl id.
    _OpenGLID = 0;
  }

//   int IOpenGLVertexDeclaration::GetDeclaration (
//     VERTEXELEMENT& pDecl,
//     unsigned int *pNumElements)
//   {
//     *pNumElements = (unsigned int) _declarations_array.size();
//     pDecl = _declarations_array[0];
// 
//     return 1;
//   }


  VERTEXELEMENT IOpenGLVertexDeclaration::GetUsage (ATTRIB_USAGE_DECL usage)
  {
    VERTEXELEMENT vtxelt;
    vtxelt.Stream = 0xFF; // invalid stream;

    for (unsigned int i = 0; _declarations_array.size(); i++)
    {
      if (_declarations_array[i].Usage == usage)
      {
        vtxelt = _declarations_array[i];
        return vtxelt;
      }
    }

    return vtxelt;
  }

// This is a simple check to comply with the documentation of DrawPrimitiveUP in DirectX
  bool IOpenGLVertexDeclaration::IsUsingMoreThanStreamZero()
  {
    for (unsigned int i = 0; i < _declarations_array.size(); i++)
    {
      if (_declarations_array[i].Stream != 0)
      {
        return true;
      }
    }

    return false;
  }

}
