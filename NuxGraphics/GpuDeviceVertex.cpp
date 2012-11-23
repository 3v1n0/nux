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
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GLDeviceFrameBufferObject.h"
#include "GLTemplatePrimitiveBuffer.h"
#include "GraphicsEngine.h"

namespace nux
{
  static STREAMSOURCE _StreamSource[MAX_NUM_STREAM];

  ObjectPtr<IOpenGLVertexBuffer> GpuDevice::CreateVertexBuffer(int Length,
      VBO_USAGE Usage)
  {
    ObjectPtr<IOpenGLVertexBuffer> result;
    result.Adopt(new IOpenGLVertexBuffer(Length, Usage, NUX_TRACKER_LOCATION));
    return result;
  }

  ObjectPtr<IOpenGLIndexBuffer> GpuDevice::CreateIndexBuffer(int Length, VBO_USAGE Usage
      , INDEX_FORMAT Format)
  {
    ObjectPtr<IOpenGLIndexBuffer> result;
    result.Adopt(new IOpenGLIndexBuffer(Length, Usage, Format, NUX_TRACKER_LOCATION));
    return result;
  }

  ObjectPtr<IOpenGLPixelBufferObject> GpuDevice::CreatePixelBufferObject(int Size, VBO_USAGE Usage)
  {
    ObjectPtr<IOpenGLPixelBufferObject> result;
    result.Adopt(new IOpenGLPixelBufferObject(Size, Usage, NUX_TRACKER_LOCATION));
    return result;
  }

  ObjectPtr<IOpenGLVertexDeclaration> GpuDevice::CreateVertexDeclaration(
    const VERTEXELEMENT *pVertexElements)
  {
    ObjectPtr<IOpenGLVertexDeclaration> result;
    result.Adopt(new IOpenGLVertexDeclaration(pVertexElements));
    return result;
  }

  void GpuDevice::InvalidateVertexBuffer()
  {
    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
  }

  void GpuDevice::InvalidateIndexBuffer()
  {
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
  }

  int GpuDevice::DrawIndexedPrimitive(ObjectPtr<IOpenGLIndexBuffer> IndexBuffer,
      ObjectPtr<IOpenGLVertexDeclaration> VertexDeclaration,
      PRIMITIVE_TYPE PrimitiveType,
      int index_count)
  {
    nuxAssert(VertexDeclaration.IsValid());

    if (!VertexDeclaration.IsValid())
      return OGL_ERROR;

    int decl = 0;

    for (int i = 0; i < 16; i++)
    {
      VertexDeclaration->_valid_vertex_input[i] = 0;
    }

    while (VertexDeclaration->_declarations_array[decl].Stream != 0xFF)
    {
      VERTEXELEMENT vtxelement = VertexDeclaration->_declarations_array[decl];
      int shader_attribute_location = VertexDeclaration->GetVertexShaderAttributeLocation(decl);

      if (shader_attribute_location == -1)
      {
        ++decl;
        continue;
      }

      VertexDeclaration->GetVertexBuffer(vtxelement.Stream)->BindVertexBuffer();
      glEnableVertexAttribArrayARB(shader_attribute_location);

      CHECKGL(glVertexAttribPointer(shader_attribute_location,
        vtxelement.NumComponent,
        vtxelement.Type,
        GL_FALSE,
        vtxelement.stride_,
        (void*)vtxelement.Offset));

      VertexDeclaration->_valid_vertex_input[shader_attribute_location] = 1;
      ++decl;
    }

    {
      IndexBuffer->BindIndexBuffer();

      GLenum primitive = PrimitiveType;
      GLenum index_format = GL_UNSIGNED_SHORT;

      if (IndexBuffer->GetStride() == 4)
        index_format = GL_UNSIGNED_INT;

//       switch(PrimitiveType)
//       {
//         case PRIMITIVE_TYPE_POINTLIST:
//           ElementCount = PrimitiveCount;
//           break;
//
//         case PRIMITIVE_TYPE_LINELIST:
//           ElementCount = PrimitiveCount * 2;
//           break;
//
//         case PRIMITIVE_TYPE_LINESTRIP:
//           ElementCount = PrimitiveCount + 1;
//           break;
//
//         case PRIMITIVE_TYPE_TRIANGLELIST:
//           ElementCount = PrimitiveCount * 3;
//           break;
//
//         case PRIMITIVE_TYPE_TRIANGLEFAN:
//         case PRIMITIVE_TYPE_TRIANGLESTRIP:
//           ElementCount = PrimitiveCount + 2;
//           break;
//         default:
//           // Unknown primitive type. This should not happen.
//           nuxAssertMsg(0, "[GpuDevice::DrawIndexedPrimitive] Unknown Primitive Type.");
//       }

      CHECKGL(glDrawElements(primitive,
                                index_count,
                                index_format,
                                0));
    }

    {
      for (int index = 0; index < 16; index++)
      {
        if (VertexDeclaration->_valid_vertex_input[index])
          glDisableVertexAttribArrayARB(index);
      }

      InvalidateVertexBuffer();
      InvalidateIndexBuffer();
    }

    //    for (int i = 0; i < 8; i++)
    //    {
    //        CHECKGL(glClientActiveTexture(GL_TEXTURE0 + i));
    //        CHECKGL(glDisable(GL_TEXTURE_3D));
    //        CHECKGL(glDisable(GL_TEXTURE_2D));
    //        CHECKGL(glDisable(GL_TEXTURE_1D));
    //        CHECKGL(glDisable(GL_TEXTURE_CUBE_MAP_ARB));
    //        CHECKGL(glEnable(GL_TEXTURE_2D));
    //        CHECKGL(glBindTexture(GL_TEXTURE_2D, 0));
    //    }
    return OGL_OK;
  }


#ifndef NUX_OPENGLES_20
// Draw Primitive without index buffer
  int GpuDevice::DrawPrimitive(ObjectPtr<IOpenGLVertexDeclaration> /* VertexDeclaration */,
                                      PRIMITIVE_TYPE /* PrimitiveType */,
                                      unsigned /* vtx_start_ */,
                                      unsigned /* PrimitiveCount */)
  {
//     return OGL_OK;
//     glDisable(GL_CULL_FACE);
//     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//     // Maps a vertex declaration usage to a vertex shader input index.
//     // I want to make this a static array defined outside of this function but their seems to be a problem
//     // with initialization...
//     /*static*/
//     int sVertexInputMap[] =
//     {
//       0,   // ATTRIB_USAGE_DECL_POSITION
//       14,  // ATTRIB_USAGE_DECL_BLENDWEIGHT
//       15,  // ATTRIB_USAGE_DECL_BLENDINDICES
//       1,   // ATTRIB_USAGE_DECL_NORMAL
//       4,   // ATTRIB_USAGE_DECL_COLOR, ATTRIB_USAGE_DECL_COLOR0
//       5,   // ATTRIB_USAGE_DECL_COLOR1
//       6,   // ATTRIB_USAGE_DECL_FOGCOORD
//       7,   // ATTRIB_USAGE_DECL_PSIZE
//       8,   // ATTRIB_USAGE_DECL_TEXCOORD, ATTRIB_USAGE_DECL_TEXCOORD0
//       9,   // ATTRIB_USAGE_DECL_TEXCOORD1
//       10,  // ATTRIB_USAGE_DECL_TEXCOORD2
//       11,  // ATTRIB_USAGE_DECL_TEXCOORD3
//       12,  // ATTRIB_USAGE_DECL_TEXCOORD4
//       13,  // ATTRIB_USAGE_DECL_TEXCOORD5
//       14,  // ATTRIB_USAGE_DECL_TEXCOORD6
//       15,  // ATTRIB_USAGE_DECL_TEXCOORD7
//       2,   // ATTRIB_USAGE_DECL_TANGENT
//       3,   // ATTRIB_USAGE_DECL_BINORMAL
//     };
//
//     nuxAssert(VertexDeclaration.IsValid());
//
//     if (!VertexDeclaration.IsValid())
//       return OGL_ERROR;
//
//     int decl = 0;
//
//     for (int i = 0; i < 16; i++)
//       VertexDeclaration->_valid_vertex_input[i] = 0;
//
//     while (VertexDeclaration->_declarations_array[decl].Stream != 0xFF)
//     {
//       VERTEXELEMENT vtxelement = VertexDeclaration->_declarations_array[decl];
//       int vtxInput = sVertexInputMap[vtxelement.Usage + vtxelement.UsageIndex];
//
//       nuxAssert(vtxInput < GetGpuInfo().GetMaxFboAttachment());
//       // Eneble the vertex attribute(0 to 10)
//       glEnableVertexAttribArrayARB( vtxInput );
//       // Bind the vertex buffer
//       _StreamSource[vtxelement.Stream].VertexBuffer->BindVertexBuffer();
//
//       CHECKGL(glVertexAttribPointer(vtxInput,
//                                        vtxelement.NumComponent,
//                                        vtxelement.Type,
//                                        GL_FALSE,
//                                        _StreamSource[vtxelement.Stream].StreamStride,
//                                        (GLvoid *) (&_StreamSource[vtxelement.Stream].StreamOffset + vtxelement.Offset)));
//
//       VertexDeclaration->_valid_vertex_input[sVertexInputMap[vtxelement.Usage + vtxelement.UsageIndex]] = 1;
//       decl++;
//     }
//
//     {
//       InvalidateIndexBuffer();
//
//       GLenum primitive = PrimitiveType;
//       int ElementCount = 0;
//
//       switch(PrimitiveType)
//       {
//         case PRIMITIVE_TYPE_POINTLIST:
//           ElementCount = PrimitiveCount;
//           break;
//
//         case PRIMITIVE_TYPE_LINELIST:
//           ElementCount = PrimitiveCount * 2;
//           break;
//
//         case PRIMITIVE_TYPE_LINESTRIP:
//           ElementCount = PrimitiveCount + 1;
//           break;
//
//         case PRIMITIVE_TYPE_TRIANGLELIST:
//           ElementCount = PrimitiveCount * 3;
//           break;
//
//         case PRIMITIVE_TYPE_TRIANGLEFAN:
//         case PRIMITIVE_TYPE_TRIANGLESTRIP:
//           ElementCount = PrimitiveCount + 2;
//           break;
//         default:
//           // Unknown primitive type. This should not happen.
//           nuxAssertMsg(0, "[GpuDevice::DrawPrimitive] Unknown Primitive Type.");
//           return OGL_ERROR;
//       }
//
//       CHECKGL(glDrawArrays(primitive,
//                               ElementCount,
//                               vtx_start_));
//     }
//
//     {
//       for (int index = 0; index < 16; index++)
//       {
//         if (VertexDeclaration->_valid_vertex_input[index])
//           glDisableVertexAttribArrayARB( index );
//       }
//
//       InvalidateVertexBuffer();
//       InvalidateIndexBuffer();
//     }
//
//     //    for (int i = 0; i < 8; i++)
//     //    {
//     //        CHECKGL(glClientActiveTexture(GL_TEXTURE0 + i));
//     //        CHECKGL(glDisable(GL_TEXTURE_3D));
//     //        CHECKGL(glDisable(GL_TEXTURE_2D));
//     //        CHECKGL(glDisable(GL_TEXTURE_1D));
//     //        CHECKGL(glDisable(GL_TEXTURE_CUBE_MAP_ARB));
//     //        CHECKGL(glEnable(GL_TEXTURE_2D));
//     //        CHECKGL(glBindTexture(GL_TEXTURE_2D, 0));
//     //    }
    return OGL_OK;
  }

// Draw Primitive without index buffer, and use a user pointer for the source of the stream.
  int GpuDevice::DrawPrimitiveUP(ObjectPtr<IOpenGLVertexDeclaration> /* VertexDeclaration */,
                                        PRIMITIVE_TYPE /* PrimitiveType */,
                                        unsigned int /* PrimitiveCount */,
                                        const void * /* pVertexStreamZeroData */,
                                        unsigned int /* VertexStreamZeroStride */)
  {
//     nuxAssertMsg(VertexDeclaration->IsUsingMoreThanStreamZero(), "[GpuDevice::DrawPrimitiveUP] Declaration is using more than stream 0.");
//     VERTEXELEMENT vtxelement = VertexDeclaration->GetUsage(ATTRIB_USAGE_DECL_POSITION);
//     int Stream = vtxelement.Stream;
//
//     if (Stream != 0xFF)
//     {
//       glDisable(GL_CULL_FACE);
//       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//       InvalidateVertexBuffer();
//       InvalidateVertexBuffer();
//
//       size_t ptr = * (size_t *) pVertexStreamZeroData + _StreamSource[vtxelement.Stream].StreamOffset;
//       CHECKGL(glEnableClientState(GL_VERTEX_ARRAY));
//       CHECKGL(glVertexPointer( vtxelement.NumComponent,
//                                   vtxelement.Type,
//                                   VertexStreamZeroStride,
//                                   (GLvoid *) &ptr));
//
//       InvalidateIndexBuffer();
//
//       GLenum primitive = PrimitiveType;
//       int ElementCount = 0;
//
//       switch(PrimitiveType)
//       {
//         case PRIMITIVE_TYPE_POINTLIST:
//           ElementCount = PrimitiveCount;
//           break;
//
//         case PRIMITIVE_TYPE_LINELIST:
//           ElementCount = PrimitiveCount * 2;
//           break;
//
//         case PRIMITIVE_TYPE_LINESTRIP:
//           ElementCount = PrimitiveCount + 1;
//           break;
//
//         case PRIMITIVE_TYPE_TRIANGLELIST:
//           ElementCount = PrimitiveCount * 3;
//           break;
//
//         case PRIMITIVE_TYPE_TRIANGLEFAN:
//         case PRIMITIVE_TYPE_TRIANGLESTRIP:
//           ElementCount = PrimitiveCount + 2;
//           break;
//         default:
//           // Unknown primitive type. This should not happen.
//           nuxAssertMsg(0, "[GpuDevice::DrawPrimitiveUP] Unknown Primitive Type.");
//           return OGL_ERROR;
//       }
//
//       CHECKGL(glDrawArrays(primitive,
//                               ElementCount,
//                               0));
//
//       CHECKGL(glDisableClientState(GL_VERTEX_ARRAY));
//
//     }
//
//     //    for (int i = 0; i < 8; i++)
//     //    {
//     //        CHECKGL(glClientActiveTexture(GL_TEXTURE0 + i));
//     //        CHECKGL(glDisable(GL_TEXTURE_3D));
//     //        CHECKGL(glDisable(GL_TEXTURE_2D));
//     //        CHECKGL(glDisable(GL_TEXTURE_1D));
//     //        CHECKGL(glDisable(GL_TEXTURE_CUBE_MAP_ARB));
//     //        CHECKGL(glEnable(GL_TEXTURE_2D));
//     //        CHECKGL(glBindTexture(GL_TEXTURE_2D, 0));
//     //    }
    return OGL_OK;
  }
#endif

// //    DirectX Matrix
// //    |   2/W     0       0       -(W+1)/W    |
// //    |   0       -2/H    0       (H+1)/H     |
// //    |   0       0       1       0           |
// //    |   0       0       0       1           |
// //
// //    Screen Quad(0, 0, W, H)
// //                  Normalized Coord       (Xn+1)/2 * W         Screen Coord
// //        X: 0 ---> -(1+1/W)                --->                -0.5
// //        X: W ---> 1-1/W                   --->                W-0.5
// //                                          -(Yn-1)/2 * H
// //        Y: 0 ---> (1+1/H)                 --->                -0.5
// //        Y: W ---> -1-1/H                  --->                H-0.5
// //
// //    The Matrix above is good to do quad on screen in DirectX. DirectX requires that the vertex coordinates be shifted
// //    by(-0.5,-0.5) in order for the center of texel to be located at the center of pixel.
// //    Note: OpenGL maps integral texture coordinates to the texel center
// //          whereas Direct3D maps  integral texture coordinates to the upper, left texel corner.
// 
// //    In OpenGL we will use this matrix instead:
// //    |   2/W     0       0         -1  |
// //    |   0       -2/H    0         1   |
// //    |   0       0       1         0   |
// //    |   0       0       0         1   |
// //
// //    Screen Quad(0, 0, W, H)
// //                  Normalized Coord     (Xn+1)/2 * W         Screen Coord
// //        X: 0 ---> -1                      --->                0.0
// //        X: W ---> 1                       --->                W
// //                                       -(Yn-1)/2 * H
// //        Y: 0 ---> 1                       --->                0.0
// //        Y: W ---> -1


  int GpuDevice::SetStreamSource(
    unsigned int StreamNumber,
    ObjectPtr<IOpenGLVertexBuffer> pStreamData,
    unsigned int OffsetInBytes,
    unsigned int Stride)
  {
    nuxAssert(StreamNumber < MAX_NUM_STREAM);

    _StreamSource[StreamNumber].Stream = StreamNumber;
    _StreamSource[StreamNumber].StreamOffset = OffsetInBytes;
    _StreamSource[StreamNumber].VertexBuffer = pStreamData;
    _StreamSource[StreamNumber].StreamStride = Stride;

    return OGL_OK;
  }
}
