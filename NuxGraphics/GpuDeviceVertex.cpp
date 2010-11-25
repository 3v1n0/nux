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

  IntrusiveSP<IOpenGLVertexBuffer> GpuDevice::CreateVertexBuffer (int Length,
      VBO_USAGE Usage)
  {
    IOpenGLVertexBuffer *ptr;
    CreateVertexBuffer (Length, Usage, (IOpenGLVertexBuffer **) &ptr);
    IntrusiveSP<IOpenGLVertexBuffer> h = IntrusiveSP<IOpenGLVertexBuffer> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateVertexBuffer (unsigned int Length,
      VBO_USAGE Usage,
      IOpenGLVertexBuffer **ppVertexBuffer)
  {
    *ppVertexBuffer = new IOpenGLVertexBuffer (Length, Usage, NUX_TRACKER_LOCATION);

    return OGL_OK;
  }

  IntrusiveSP<IOpenGLIndexBuffer> GpuDevice::CreateIndexBuffer (
    int Length
    , VBO_USAGE Usage    // Dynamic or WriteOnly
    , INDEX_FORMAT Format)
  {
    IOpenGLIndexBuffer *ptr;
    CreateIndexBuffer (Length, Usage, Format, (IOpenGLIndexBuffer **) &ptr);
    IntrusiveSP<IOpenGLIndexBuffer> h = IntrusiveSP<IOpenGLIndexBuffer> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateIndexBuffer (unsigned int Length,
                                          VBO_USAGE Usage,
                                          INDEX_FORMAT Format,
                                          IOpenGLIndexBuffer **ppIndexBuffer)
  {
    *ppIndexBuffer = new IOpenGLIndexBuffer (Length, Usage, Format, NUX_TRACKER_LOCATION);

    return OGL_OK;
  }

  IntrusiveSP<IOpenGLPixelBufferObject> GpuDevice::CreatePixelBufferObject (int Size, VBO_USAGE Usage)
  {
    IOpenGLPixelBufferObject *ptr;
    CreatePixelBufferObject (Size, Usage, (IOpenGLPixelBufferObject **) &ptr);
    IntrusiveSP<IOpenGLPixelBufferObject> h = IntrusiveSP<IOpenGLPixelBufferObject> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreatePixelBufferObject (int Size, VBO_USAGE Usage,
      IOpenGLPixelBufferObject **ppPixelBufferObject)
  {
    *ppPixelBufferObject = new IOpenGLPixelBufferObject (Size, Usage, NUX_TRACKER_LOCATION);

    return OGL_OK;
  }

  IntrusiveSP<IOpenGLVertexDeclaration> GpuDevice::CreateVertexDeclaration (
    const VERTEXELEMENT *pVertexElements)
  {
    IOpenGLVertexDeclaration *ptr;
    CreateVertexDeclaration (pVertexElements, (IOpenGLVertexDeclaration **) &ptr);
    IntrusiveSP<IOpenGLVertexDeclaration> h = IntrusiveSP<IOpenGLVertexDeclaration> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateVertexDeclaration (const VERTEXELEMENT *pVertexElements,
      IOpenGLVertexDeclaration **ppDecl)
  {
    *ppDecl = new IOpenGLVertexDeclaration (pVertexElements);

    return OGL_OK;
  }

  void GpuDevice::InvalidateVertexBuffer()
  {
    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
  }

  void GpuDevice::InvalidateIndexBuffer()
  {
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
  }

  int GpuDevice::DrawIndexedPrimitive (IntrusiveSP<IOpenGLIndexBuffer> IndexBuffer,
      IntrusiveSP<IOpenGLVertexDeclaration> VertexDeclaration,
      PRIMITIVE_TYPE PrimitiveType,
      int BaseVertexIndex,
      int MinIndex,
      int NumVertices,
      int StartIndex,
      int PrimitiveCount)
  {
    //    glDisable(GL_CULL_FACE);
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Maps a vertex declaration usage to a vertex shader input index.
    // I want to make this a static array defined outside of this function but their seems to be a problem
    // with initialization...
    /*static*/ int sVertexInputMap[] =
    {
      0,   // ATTRIB_USAGE_DECL_POSITION
      14,  // ATTRIB_USAGE_DECL_BLENDWEIGHT
      15,  // ATTRIB_USAGE_DECL_BLENDINDICES
      1,   // ATTRIB_USAGE_DECL_NORMAL
      4,   // ATTRIB_USAGE_DECL_COLOR, ATTRIB_USAGE_DECL_COLOR0
      5,   // ATTRIB_USAGE_DECL_COLOR1
      6,   // ATTRIB_USAGE_DECL_FOGCOORD
      7,   // ATTRIB_USAGE_DECL_PSIZE
      8,   // ATTRIB_USAGE_DECL_TEXCOORD, ATTRIB_USAGE_DECL_TEXCOORD0
      9,   // ATTRIB_USAGE_DECL_TEXCOORD1
      10,  // ATTRIB_USAGE_DECL_TEXCOORD2
      11,  // ATTRIB_USAGE_DECL_TEXCOORD3
      12,  // ATTRIB_USAGE_DECL_TEXCOORD4
      13,  // ATTRIB_USAGE_DECL_TEXCOORD5
      14,  // ATTRIB_USAGE_DECL_TEXCOORD6
      15,  // ATTRIB_USAGE_DECL_TEXCOORD7
      2,   // ATTRIB_USAGE_DECL_TANGENT
      3,   // ATTRIB_USAGE_DECL_BINORMAL
    };

    nuxAssert (VertexDeclaration.IsValid() );

    if (!VertexDeclaration.IsValid() )
      return OGL_ERROR;

    int decl = 0;

    for (int i = 0; i < 16; i++)
      VertexDeclaration->ValideVertexInput[i] = 0;

    while (VertexDeclaration->_DeclarationsArray[decl]->Stream != 0xFF)
    {
      VERTEXELEMENT vtxelement = *VertexDeclaration->_DeclarationsArray[decl];
      int vtxInput = sVertexInputMap[vtxelement.Usage + vtxelement.UsageIndex];
      glEnableVertexAttribArrayARB ( vtxInput );

      _StreamSource[vtxelement.Stream].VertexBuffer->BindVertexBuffer();
      CHECKGL ( glVertexAttribPointer (vtxInput,
                                       vtxelement.NumComponent,
                                       vtxelement.Type,
                                       GL_FALSE,
                                       _StreamSource[vtxelement.Stream].StreamStride,
                                       (GLvoid *) (&_StreamSource[vtxelement.Stream].StreamOffset + vtxelement.Offset) ) );

      VertexDeclaration->ValideVertexInput[sVertexInputMap[vtxelement.Usage + vtxelement.UsageIndex]] = 1;
      decl++;
    }

    {
      IndexBuffer->BindIndexBuffer();

      GLenum primitive = PrimitiveType;
      GLenum index_format = GL_UNSIGNED_SHORT;

      if (IndexBuffer->GetStride() == 4)
        index_format = GL_UNSIGNED_INT;

      int ElementCount = 0;

      switch (PrimitiveType)
      {
        case PRIMITIVE_TYPE_POINTLIST:
          ElementCount = PrimitiveCount;
          break;

        case PRIMITIVE_TYPE_LINELIST:
          ElementCount = PrimitiveCount * 2;
          break;

        case PRIMITIVE_TYPE_LINESTRIP:
          ElementCount = PrimitiveCount + 1;
          break;

        case PRIMITIVE_TYPE_TRIANGLELIST:
          ElementCount = PrimitiveCount * 3;
          break;

        case PRIMITIVE_TYPE_TRIANGLEFAN:
        case PRIMITIVE_TYPE_TRIANGLESTRIP:
          ElementCount = PrimitiveCount + 2;
          break;
        default:
          // Unknown primitive type. This should not happen.
          nuxAssertMsg (0, TEXT ("[GpuDevice::DrawIndexedPrimitive] Unknown Primitive Type.") );
      }

      t_size ptr = StartIndex * IndexBuffer->GetStride();
      CHECKGL ( glDrawElements (primitive,
                                ElementCount,
                                index_format,
                                (GLvoid *) &ptr) );
    }

    {
      for (int index = 0; index < 16; index++)
      {
        if (VertexDeclaration->ValideVertexInput[index])
          glDisableVertexAttribArrayARB ( index );
      }

      InvalidateVertexBuffer();
      InvalidateIndexBuffer();
    }

    //    for(int i = 0; i < 8; i++)
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


// Draw Primitive without index buffer
  int GpuDevice::DrawPrimitive (IntrusiveSP<IOpenGLVertexDeclaration> VertexDeclaration,
                                      PRIMITIVE_TYPE PrimitiveType,
                                      unsigned vtx_start_,
                                      unsigned PrimitiveCount)
  {
    return OGL_OK;
    glDisable (GL_CULL_FACE);
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    // Maps a vertex declaration usage to a vertex shader input index.
    // I want to make this a static array defined outside of this function but their seems to be a problem
    // with initialization...
    /*static*/
    int sVertexInputMap[] =
    {
      0,   // ATTRIB_USAGE_DECL_POSITION
      14,  // ATTRIB_USAGE_DECL_BLENDWEIGHT
      15,  // ATTRIB_USAGE_DECL_BLENDINDICES
      1,   // ATTRIB_USAGE_DECL_NORMAL
      4,   // ATTRIB_USAGE_DECL_COLOR, ATTRIB_USAGE_DECL_COLOR0
      5,   // ATTRIB_USAGE_DECL_COLOR1
      6,   // ATTRIB_USAGE_DECL_FOGCOORD
      7,   // ATTRIB_USAGE_DECL_PSIZE
      8,   // ATTRIB_USAGE_DECL_TEXCOORD, ATTRIB_USAGE_DECL_TEXCOORD0
      9,   // ATTRIB_USAGE_DECL_TEXCOORD1
      10,  // ATTRIB_USAGE_DECL_TEXCOORD2
      11,  // ATTRIB_USAGE_DECL_TEXCOORD3
      12,  // ATTRIB_USAGE_DECL_TEXCOORD4
      13,  // ATTRIB_USAGE_DECL_TEXCOORD5
      14,  // ATTRIB_USAGE_DECL_TEXCOORD6
      15,  // ATTRIB_USAGE_DECL_TEXCOORD7
      2,   // ATTRIB_USAGE_DECL_TANGENT
      3,   // ATTRIB_USAGE_DECL_BINORMAL
    };

    nuxAssert (VertexDeclaration.IsValid() );

    if (!VertexDeclaration.IsValid() )
      return OGL_ERROR;

    int decl = 0;

    for (int i = 0; i < 16; i++)
      VertexDeclaration->ValideVertexInput[i] = 0;

    while (VertexDeclaration->_DeclarationsArray[decl]->Stream != 0xFF)
    {
      VERTEXELEMENT vtxelement = *VertexDeclaration->_DeclarationsArray[decl];
      int vtxInput = sVertexInputMap[vtxelement.Usage + vtxelement.UsageIndex];

      nuxAssert (vtxInput < GetGpuInfo().GetMaxFboAttachment());
      // Eneble the vertex attribute (0 to 10)
      glEnableVertexAttribArrayARB ( vtxInput );
      // Bind the vertex buffer
      _StreamSource[vtxelement.Stream].VertexBuffer->BindVertexBuffer();

      CHECKGL ( glVertexAttribPointer (vtxInput,
                                       vtxelement.NumComponent,
                                       vtxelement.Type,
                                       GL_FALSE,
                                       _StreamSource[vtxelement.Stream].StreamStride,
                                       (GLvoid *) (&_StreamSource[vtxelement.Stream].StreamOffset + vtxelement.Offset) ) );

      VertexDeclaration->ValideVertexInput[sVertexInputMap[vtxelement.Usage + vtxelement.UsageIndex]] = 1;
      decl++;
    }

    {
      InvalidateIndexBuffer();

      GLenum primitive = PrimitiveType;
      int ElementCount = 0;

      switch (PrimitiveType)
      {
        case PRIMITIVE_TYPE_POINTLIST:
          ElementCount = PrimitiveCount;
          break;

        case PRIMITIVE_TYPE_LINELIST:
          ElementCount = PrimitiveCount * 2;
          break;

        case PRIMITIVE_TYPE_LINESTRIP:
          ElementCount = PrimitiveCount + 1;
          break;

        case PRIMITIVE_TYPE_TRIANGLELIST:
          ElementCount = PrimitiveCount * 3;
          break;

        case PRIMITIVE_TYPE_TRIANGLEFAN:
        case PRIMITIVE_TYPE_TRIANGLESTRIP:
          ElementCount = PrimitiveCount + 2;
          break;
        default:
          // Unknown primitive type. This should not happen.
          nuxAssertMsg (0, TEXT ("[GpuDevice::DrawPrimitive] Unknown Primitive Type.") );
          return OGL_ERROR;
      }

      CHECKGL ( glDrawArrays (primitive,
                              ElementCount,
                              vtx_start_) );
    }

    {
      for (int index = 0; index < 16; index++)
      {
        if (VertexDeclaration->ValideVertexInput[index])
          glDisableVertexAttribArrayARB ( index );
      }

      InvalidateVertexBuffer();
      InvalidateIndexBuffer();
    }

    //    for(int i = 0; i < 8; i++)
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

// Draw Primitive without index buffer, and use a user pointer for the source of the stream.
  int GpuDevice::DrawPrimitiveUP (IntrusiveSP<IOpenGLVertexDeclaration> VertexDeclaration,
                                        PRIMITIVE_TYPE PrimitiveType,
                                        unsigned int PrimitiveCount,
                                        const void *pVertexStreamZeroData,
                                        unsigned int VertexStreamZeroStride)
  {
    nuxAssertMsg (VertexDeclaration->IsUsingMoreThanStreamZero(), TEXT ("[GpuDevice::DrawPrimitiveUP] Declaration is using more than stream 0.") );
    VERTEXELEMENT vtxelement = VertexDeclaration->GetUsage (ATTRIB_USAGE_DECL_POSITION);
    int Stream = vtxelement.Stream;

    if (Stream != 0xFF)
    {
      glDisable (GL_CULL_FACE);
      glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

      InvalidateVertexBuffer();
      InvalidateVertexBuffer();

      t_size ptr = * (t_size *) pVertexStreamZeroData + _StreamSource[vtxelement.Stream].StreamOffset;
      CHECKGL ( glEnableClientState (GL_VERTEX_ARRAY) );
      CHECKGL ( glVertexPointer ( vtxelement.NumComponent,
                                  vtxelement.Type,
                                  VertexStreamZeroStride,
                                  (GLvoid *) &ptr) );

      InvalidateIndexBuffer();

      GLenum primitive = PrimitiveType;
      int ElementCount = 0;

      switch (PrimitiveType)
      {
        case PRIMITIVE_TYPE_POINTLIST:
          ElementCount = PrimitiveCount;
          break;

        case PRIMITIVE_TYPE_LINELIST:
          ElementCount = PrimitiveCount * 2;
          break;

        case PRIMITIVE_TYPE_LINESTRIP:
          ElementCount = PrimitiveCount + 1;
          break;

        case PRIMITIVE_TYPE_TRIANGLELIST:
          ElementCount = PrimitiveCount * 3;
          break;

        case PRIMITIVE_TYPE_TRIANGLEFAN:
        case PRIMITIVE_TYPE_TRIANGLESTRIP:
          ElementCount = PrimitiveCount + 2;
          break;
        default:
          // Unknown primitive type. This should not happen.
          nuxAssertMsg (0, TEXT ("[GpuDevice::DrawPrimitiveUP] Unknown Primitive Type.") );
          return OGL_ERROR;
      }

      CHECKGL ( glDrawArrays (primitive,
                              ElementCount,
                              0) );

      CHECKGL ( glDisableClientState (GL_VERTEX_ARRAY) );

    }

    //    for(int i = 0; i < 8; i++)
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

  void GpuDevice::DrawQuad_FixPipe (int x, int y, int width, int height,
                                          FLOAT R, FLOAT G, FLOAT B, FLOAT A)
  {
    Matrix4 matrix;
    matrix.Identity();
    matrix.m[0][0] = 2.0f / (FLOAT) _ViewportWidth;
    matrix.m[0][3] = -1.0f;
    matrix.m[1][1] = - (2.0f / (FLOAT) _ViewportHeight);
    matrix.m[1][3] = 1.0f;

    matrix.Transpose();

    FLOAT Vertices[16] =
    {
      x,          y,          0.0f, 1.0f,
      x,          y + height,   0.0f, 1.0f,
      x + width,  y + height,   0.0f, 1.0f,
      x + width,  y,          0.0f, 1.0f
    };

    unsigned int Indices[] = {0, 1, 2, 0, 2, 3};

    FLOAT VertexColors[16] =
    {
      R,        G,        B,       A,
      R,        G,        B,       A,
      R,        G,        B,       A,
      R,        G,        B,       A
    };

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) matrix.m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glPolygonMode (GL_FRONT_AND_BACK, GL_FILL) );

    CHECKGL ( glUseProgramObjectARB (0) );
    InvalidateVertexBuffer();
    InvalidateIndexBuffer();
    InvalidateTextureUnit (GL_TEXTURE0);

    CHECKGL ( glEnableClientState (GL_VERTEX_ARRAY) );
    CHECKGL ( glVertexPointer ( 4, GL_FLOAT, 0,  (void *) (&Vertices) ) );

    CHECKGL ( glEnableClientState (GL_COLOR_ARRAY) );
    CHECKGL ( glColorPointer ( 4, GL_FLOAT, 0,  (void *) (&VertexColors) ) );

    CHECKGL ( glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) (&Indices) ) );

    CHECKGL ( glDisableClientState (GL_VERTEX_ARRAY) );
    CHECKGL ( glDisableClientState (GL_COLOR_ARRAY) );
  }

  void GpuDevice::DrawTextureQuad_FixPipe (IntrusiveSP<IOpenGLTexture2D> texture, int x, int y, int width, int height,
      FLOAT u0, FLOAT v0, FLOAT u1, FLOAT v1)
  {
//    DirectX Matrix
//    |   2/W     0       0       -(W+1)/W    |
//    |   0       -2/H    0       (H+1)/H     |
//    |   0       0       1       0           |
//    |   0       0       0       1           |
//
//    Screen Quad(0, 0, W, H)
//                  Normalized Coord       (Xn+1)/2 * W         Screen Coord
//        X: 0 ---> -(1+1/W)                --->                -0.5
//        X: W ---> 1-1/W                   --->                W-0.5
//                                          -(Yn-1)/2 * H
//        Y: 0 ---> (1+1/H)                 --->                -0.5
//        Y: W ---> -1-1/H                  --->                H-0.5
//
//    The Matrix above is good to do quad on screen in DirectX. DirectX requires that the vertex coordinates be shifted
//    by (-0.5,-0.5) in order for the center of texel to be located at the center of pixel.
//    Note: OpenGL maps integral texture coordinates to the texel center
//          whereas Direct3D maps  integral texture coordinates to the upper, left texel corner.

//    In OpenGL we will use this matrix instead:
//    |   2/W     0       0         -1  |
//    |   0       -2/H    0         1   |
//    |   0       0       1         0   |
//    |   0       0       0         1   |
//
//    Screen Quad(0, 0, W, H)
//                  Normalized Coord     (Xn+1)/2 * W         Screen Coord
//        X: 0 ---> -1                      --->                0.0
//        X: W ---> 1                       --->                W
//                                       -(Yn-1)/2 * H
//        Y: 0 ---> 1                       --->                0.0
//        Y: W ---> -1


    Matrix4 matrix;
    matrix.Identity();
    matrix.m[0][0] = 2.0f / 1280.0f;
    matrix.m[3][0] = - (1280.0f + 1.0f) / 1280.0f;
    matrix.m[1][1] = - (2.0f / 720.0f);
    matrix.m[3][1] = (720.0f + 1.0f) / 720.0f;


    matrix.Transpose();

    FLOAT Vertices[16] =
    {
      x,          y,          0.0f, 1.0f,
      x,          y + height,   0.0f, 1.0f,
      x + width,  y + height,   0.0f, 1.0f,
      x + width,  y,          0.0f, 1.0f
    };

    unsigned int Indices[] = {0, 1, 2, 0, 2, 3};

    FLOAT UV[8] =
    {
      u0,          v1,
      u0,          v0,
      u1,          v0,
      u1,          v1,
    };

    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glLoadMatrixf ( (FLOAT *) matrix.m) );
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glPolygonMode (GL_FRONT_AND_BACK, GL_FILL) );

    CHECKGL ( glUseProgramObjectARB (0) );
    InvalidateVertexBuffer();
    InvalidateIndexBuffer();

    CHECKGL ( glEnableClientState (GL_VERTEX_ARRAY) );
    CHECKGL ( glVertexPointer ( 4, GL_FLOAT, 0,  (void *) (&Vertices) ) );

    CHECKGL ( glClientActiveTextureARB (GL_TEXTURE0) );
    texture->BindTexture();
    CHECKGL ( glEnableClientState (GL_TEXTURE_COORD_ARRAY) );
    CHECKGL ( glTexCoordPointer ( 2, GL_FLOAT, 0,  (void *) (&UV) ) );

    CHECKGL ( glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) (&Indices) ) );

    CHECKGL ( glDisableClientState (GL_VERTEX_ARRAY) );
    CHECKGL ( glDisableClientState (GL_TEXTURE_COORD_ARRAY) );
  }

  int GpuDevice::SetStreamSource (
    unsigned int StreamNumber,
    IntrusiveSP<IOpenGLVertexBuffer> pStreamData,
    unsigned int OffsetInBytes,
    unsigned int Stride,
    unsigned int NumComponent,
    unsigned int ComponentFormat)
  {
    nuxAssert (StreamNumber < MAX_NUM_STREAM);

    _StreamSource[StreamNumber].Stream = StreamNumber;
    _StreamSource[StreamNumber].StreamOffset = OffsetInBytes;
    _StreamSource[StreamNumber].VertexBuffer = pStreamData;
    _StreamSource[StreamNumber].StreamStride = Stride;

    return OGL_OK;
  }

}
