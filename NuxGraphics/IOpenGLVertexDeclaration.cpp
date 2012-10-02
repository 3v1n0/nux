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

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLVertexDeclaration);

  IOpenGLVertexDeclaration::IOpenGLVertexDeclaration(const VERTEXELEMENT* pVertexElements)
    : IOpenGLResource(RTVERTEXDECLARATION)
  {
    for (int i = 0; i < 8; i++)
    {
      _stride[i] = 0;
      vertex_buffer_array.push_back(ObjectPtr<IOpenGLVertexBuffer>(NULL));
    }

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
    _declarations_array.push_back(DECL_END);
  };

  IOpenGLVertexDeclaration::~IOpenGLVertexDeclaration()
  {
    // IOpenGLVertexDeclaration is an abstraction. Is does not have an opengl id.
    _OpenGLID = 0;
  }

//   int IOpenGLVertexDeclaration::GetDeclaration(
//     VERTEXELEMENT& pDecl,
//     unsigned int *pNumElements)
//   {
//     *pNumElements = (unsigned int) _declarations_array.size();
//     pDecl = _declarations_array[0];
// 
//     return 1;
//   }


  VERTEXELEMENT IOpenGLVertexDeclaration::GetUsage(ATTRIB_USAGE_DECL /* usage */)
  {
    VERTEXELEMENT vtxelt;
    return vtxelt;
//     vtxelt.Stream = 0xFF; // invalid stream;
// 
//     for (unsigned int i = 0; _declarations_array.size(); i++)
//     {
//       if (_declarations_array[i].Usage == usage)
//       {
//         vtxelt = _declarations_array[i];
//         return vtxelt;
//       }
//     }
// 
//     return vtxelt;
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

  int IOpenGLVertexDeclaration::GetStride(int vertex_input_number)
  {
    NUX_RETURN_VALUE_IF_FALSE(vertex_input_number >= 0, 0);
    NUX_RETURN_VALUE_IF_FALSE(vertex_input_number < 8, 0);

    return _stride[vertex_input_number];
  }


  void IOpenGLVertexDeclaration::SetVertexBuffer(int input_index, ObjectPtr<IOpenGLVertexBuffer> vertex_buffer)
  {
    NUX_RETURN_IF_FALSE(input_index >= 0);
    NUX_RETURN_IF_FALSE(input_index < 8);

    vertex_buffer_array[input_index] = vertex_buffer;
  }

  ObjectPtr<IOpenGLVertexBuffer> IOpenGLVertexDeclaration::GetVertexBuffer(int input_index)
  {
    NUX_RETURN_VALUE_IF_FALSE(input_index >= 0, ObjectPtr<IOpenGLVertexBuffer>(NULL));
    NUX_RETURN_VALUE_IF_FALSE(input_index < 8, ObjectPtr<IOpenGLVertexBuffer>(NULL));

    return vertex_buffer_array[input_index];
  }

  void IOpenGLVertexDeclaration::SetVertexShaderAttributeLocation(int input_index, int shader_attribute_location)
  {
    NUX_RETURN_IF_FALSE(input_index >= 0);
    NUX_RETURN_IF_FALSE(input_index < 8);

    shader_attribute_location_array[input_index] = shader_attribute_location;
  }

  int IOpenGLVertexDeclaration::GetVertexShaderAttributeLocation(int input_index)
  {
    NUX_RETURN_VALUE_IF_FALSE(input_index >= 0, -1);
    NUX_RETURN_VALUE_IF_FALSE(input_index < 8, -1);

    return shader_attribute_location_array[input_index];
  }
}
