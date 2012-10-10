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


#include "GpuDevice.h"
#include "GLTemplatePrimitiveBuffer.h"

namespace nux
{

  TemplateQuadBuffer::TemplateQuadBuffer(GpuDevice *pDeviceFactory, ShaderType Type, int NumQuads)
    :   m_ShaderType(Type)
  {
    Memset(VertexAttributeBuffer, 0, 16);

    nuxAssert(pDeviceFactory);
    m_pDeviceFactory = pDeviceFactory;
    m_NumQuad = NumQuads;
    nuxAssert(m_NumQuad > 0);

    if (m_NumQuad < 0)
      m_NumQuad = 256;

    m_NumVertex = m_NumQuad * 6;

    FormatQuads();
  }

  TemplateQuadBuffer::~TemplateQuadBuffer()
  {
    m_IB.Release();

    for (unsigned int i = 0; i < 16; i++)
    {
      VertexAttributeBuffer[i].Release();
    }
  }

  void TemplateQuadBuffer::FormatQuads()
  {
    // create vertex buffers for quad rendering
    // NumQuads * (4 vertices per Quad) * (4 Bytes per vertices)
    VertexAttributeBuffer[0] = m_pDeviceFactory->CreateVertexBuffer(m_NumQuad * 4 * 16, VBO_USAGE_DYNAMIC);

    // fill quad vertex buffer
    unsigned idx = 0;
    //BYTE* data0;
    float *data0;
    VertexAttributeBuffer[0]->Lock(0, m_NumQuad * 4 * 16, (void **) &data0);

    for (INT i = 0; i < m_NumQuad; ++i)
    {
      //         (0, 0) _________(1, 0)
      //                |      /|
      //                |     / |
      //                |    /  |
      //                |   /   |
      //                |  /    |
      //                | /     |
      //                |/      |
      //         (0, 1) --------- (1, 1)


      ((float *) data0) [idx++] = 0; // x
      ((float *) data0) [idx++] = 0; // y
      ((float *) data0) [idx++] = 0; // z
      ((float *) data0) [idx++] = 1.0f; // w


      ((float *) data0) [idx++] = 0; // x
      ((float *) data0) [idx++] = 1.0f; // y
      ((float *) data0) [idx++] = 0; // z
      ((float *) data0) [idx++] = 1; // w

      ((float *) data0) [idx++] = 1.0f; // x
      ((float *) data0) [idx++] = 1.0f; // y
      ((float *) data0) [idx++] = 0; // z
      ((float *) data0) [idx++] = 1; // w


      ((float *) data0) [idx++] = 1.0f; // x
      ((float *) data0) [idx++] = 0; // y
      ((float *) data0) [idx++] = 0; // z
      ((float *) data0) [idx++] = 1; // w


//         ((DWORD*)data0)[idx++]=((255*(0&1)))|((255*((0&2)>>1))<<8)|(0<<16)|(255<<24);
//         ((DWORD*)data0)[idx++]=((255*(1&1)))|((255*((1&2)>>1))<<8)|(0<<16)|(255<<24);
//         ((DWORD*)data0)[idx++]=((255*(3&1)))|((255*((3&2)>>1))<<8)|(0<<16)|(255<<24);
//         ((DWORD*)data0)[idx++]=((255*(2&1)))|((255*((2&2)>>1))<<8)|(0<<16)|(255<<24);
    }

    VertexAttributeBuffer[0]->Unlock();

    // create index buffers for quad rendering
    // NumQuads * (6 index per Quad) * (2 Bytes per index) : each quad is 2 triangles => 6 indices.
    m_IB = m_pDeviceFactory->CreateIndexBuffer(m_NumQuad * 6 * 2, VBO_USAGE_DYNAMIC, INDEX_FORMAT_USHORT);

    // Fill quad index buffer
    // The index buffers defines 2 triangles for each quad.
    // Triangles are arranged as strips(that is 6 triangle index for a quad).
    WORD *data1;
    m_IB->Lock(0, m_NumQuad * 6 * 2, (void **) &data1);

    for (INT i = 0; i < m_NumQuad; ++i)
    {
      //
      //            (0) _________(1)
      //                |      /|
      //                |     / |
      //                |    /  |
      //                |   /   |
      //                |  /    |
      //                | /     |
      //                |/      |
      //            (3) --------- (2)
      //
      ((WORD *) data1) [i*6+0] = i * 4 + 0;
      ((WORD *) data1) [i*6+1] = i * 4 + 1;
      ((WORD *) data1) [i*6+2] = i * 4 + 3;
      ((WORD *) data1) [i*6+3] = i * 4 + 3;
      ((WORD *) data1) [i*6+4] = i * 4 + 1;
      ((WORD *) data1) [i*6+5] = i * 4 + 2;

    }

    m_IB->Unlock();
  }

  void TemplateQuadBuffer::SetNumQuads(int NumQuads)
  {
    m_NumQuad = NumQuads;
    nuxAssert(m_NumQuad > 0);

    if (m_NumQuad < 0)
      m_NumQuad = 256;

    m_NumVertex = m_NumQuad * 6;

    m_IB = ObjectPtr<IOpenGLIndexBuffer> (0);;

    for (int i = 0; i < 16; i++)
      VertexAttributeBuffer[i] = ObjectPtr<IOpenGLVertexBuffer> (0);

    FormatQuads();
  }

  int TemplateQuadBuffer::GetNumQuads() const
  {
    return m_NumQuad;
  }

  void TemplateQuadBuffer::UnBind()
  {
    m_pDeviceFactory->InvalidateIndexBuffer();
    m_pDeviceFactory->InvalidateVertexBuffer();
  }

  void TemplateQuadBuffer::BindAttribute(INT AttributeLocation, UINT AttributeIndex)
  {
    if (AttributeLocation < 0)
    {
      // Exclude Attribute Location that are not valid. Not valid does not mean the program is wrong!
      return;
    }

    nuxAssert(AttributeIndex < 16);


    if (VertexAttributeBuffer[AttributeIndex].IsNull())
      return;

    if (m_ShaderType == SHADER_TYPE_GLSL)
    {
      if (AttributeIndex == 0)
      {
        VertexAttributeBuffer[AttributeIndex]->BindVertexBuffer();
        CHECKGL(glEnableVertexAttribArrayARB(AttributeLocation));
        CHECKGL(glVertexAttribPointerARB((GLuint) AttributeLocation, 4, GL_FLOAT, GL_FALSE, 0, NUX_BUFFER_OFFSET(0)));
      }
      else
      {
        VertexAttributeBuffer[AttributeIndex]->BindVertexBuffer();
        CHECKGL(glEnableVertexAttribArrayARB(AttributeLocation));
        CHECKGL(glVertexAttribPointerARB((GLuint) AttributeLocation, 4, GL_FLOAT, GL_FALSE, 0, NUX_BUFFER_OFFSET(0)));
      }
    }
    else
    {
      nuxAssertMsg(0, "[TemplateQuadBuffer::BindCgAttribute] Use BindCGAttribute for CG attributes.");
    }
  }

#if (NUX_ENABLE_CG_SHADERS)
  void TemplateQuadBuffer::BindCGAttribute(CGparameter AttributeLocation, UINT AttributeIndex)
  {
    if (AttributeLocation < 0)
    {
      // Exclude Attribute Location that are not valid. Not valid does not mean the program is wrong!
      return;
    }

    nuxAssert(AttributeIndex >= 0); // Index 0 is the vertex position attribute. This on is set in the constructor.
    nuxAssert(AttributeIndex < 16);
    nuxAssert(AttributeLocation >= 0);


    if (VertexAttributeBuffer[AttributeIndex].IsNull())
      return;

    if (m_ShaderType == SHADER_TYPE_CG)
    {
      if (AttributeIndex == 0)
      {
        VertexAttributeBuffer[AttributeIndex]->BindVertexBuffer();
        CHECKGL(cgGLEnableClientState((CGparameter) AttributeLocation));
        CHECKGL(cgGLSetParameterPointer((CGparameter) AttributeLocation, 4, GL_FLOAT, 0, NUX_BUFFER_OFFSET(0)));
      }
      else
      {
        VertexAttributeBuffer[AttributeIndex]->BindVertexBuffer();
        CHECKGL(cgGLEnableClientState((CGparameter) AttributeLocation));
        CHECKGL(cgGLSetParameterPointer((CGparameter) AttributeLocation, 4, GL_FLOAT, 0, NUX_BUFFER_OFFSET(0)));
      }
    }
    else
    {
      nuxAssertMsg(0, "[TemplateQuadBuffer::BindCGAttribute] Use BindAttribute for GLSL attributes.");
    }
  }
#endif

  void TemplateQuadBuffer::UnBindAttribute(INT AttributeLocation)
  {
    if (AttributeLocation < 0)
    {
      // Exclude Attribute Location that are not valid. Not valid does not mean the program is wrong!
      return;
    }

    if (m_ShaderType == SHADER_TYPE_GLSL)
    {
      CHECKGL(glDisableVertexAttribArrayARB(AttributeLocation));
    }
    else
    {
      nuxAssertMsg(0, "[TemplateQuadBuffer::UnBindAttribute] Use UnBindCGAttribute for CG attributes.");
    }
  }

#if (NUX_ENABLE_CG_SHADERS)
  void TemplateQuadBuffer::UnBindCGAttribute(CGparameter AttributeLocation)
  {
    if (AttributeLocation < 0)
    {
      // Exclude Attribute Location that are not valid. Not valid does not mean the program is wrong!
      return;
    }

    if (m_ShaderType == SHADER_TYPE_CG)
    {
      CHECKGL(cgGLDisableClientState((CGparameter) AttributeLocation));
    }
    else
    {
      nuxAssertMsg(0, "[TemplateQuadBuffer::UnBindCGAttribute] Use UnBindAttribute for GLSL attributes.");
    }
  }
#endif

  void TemplateQuadBuffer::Render(INT NumPrimitives)
  {
    INT NumVertex = NumPrimitives * 6;  // each quad is 2 triangles = 6 indices

    if (NumVertex > m_NumVertex)
    {
      NumVertex = m_NumVertex;
    }

    m_IB->BindIndexBuffer();
    CHECKGL(glDrawElements( GL_TRIANGLES, NumVertex, GL_UNSIGNED_SHORT,  NUX_BUFFER_OFFSET(0)));
    m_pDeviceFactory->InvalidateIndexBuffer();
  }

  void TemplateQuadBuffer::SetPerQuadAttribute(UINT AttributeIndex, INT Num, Vector4 *pVector)
  {
    nuxAssert(AttributeIndex > 0); // Index 0 is the vertex position attribute. This on is set in the constructor.
    nuxAssert(AttributeIndex < 16);

    // Destroy the vertex buffer by setting it to NULL;
    VertexAttributeBuffer[AttributeIndex] = ObjectPtr<IOpenGLVertexBuffer> (0);;

    VertexAttributeBuffer[AttributeIndex] = m_pDeviceFactory->CreateVertexBuffer(m_NumQuad * 4 * sizeof(Vector4), VBO_USAGE_DYNAMIC);

    FLOAT *data;
    VertexAttributeBuffer[AttributeIndex]->Lock(0, m_NumQuad * 4 * sizeof(Vector4), (void **) &data);

    INT i;

    for (i = 0; (i < m_NumQuad) && (i <  Num); i++)
    {
      for (INT j = 0; j < 4; j++)
      {
        data[16*i + 4*j + 0] = pVector[i].x;
        data[16*i + 4*j + 1] = pVector[i].y;
        data[16*i + 4*j + 2] = pVector[i].z;
        data[16*i + 4*j + 3] = pVector[i].w;
      }
    }

    while (i < m_NumQuad)
    {
      // this happens if Num < m_NumQuad.
      // Fill the rest with the last element of the parameter array passed as argument of the function.
      for (INT j = 0; j < 4; j++)
      {
        data[16*i + 4*j + 0] = pVector[Num-1].x;
        data[16*i + 4*j + 1] = pVector[Num-1].y;
        data[16*i + 4*j + 2] = pVector[Num-1].z;
        data[16*i + 4*j + 3] = pVector[Num-1].w;
      }

      i++;
    }

    VertexAttributeBuffer[AttributeIndex]->Unlock();
  }

  void TemplateQuadBuffer::SetPerVertexAttribute(UINT AttributeIndex, INT Num, Vector4 *pVector)
  {
    nuxAssert(AttributeIndex > 0); // Index 0 is the vertex position attribute. This on is set in the constructor.
    nuxAssert(AttributeIndex < 16);

    // Destroy the vertex buffer by setting it to NULL;
    VertexAttributeBuffer[AttributeIndex] = ObjectPtr<IOpenGLVertexBuffer> (0);;

    VertexAttributeBuffer[AttributeIndex] = m_pDeviceFactory->CreateVertexBuffer(m_NumQuad * 4 * sizeof(Vector4), VBO_USAGE_DYNAMIC);

    FLOAT *data;
    VertexAttributeBuffer[AttributeIndex]->Lock(0, m_NumQuad * 4 * sizeof(Vector4), (void **) &data);

    INT i;

    for (i = 0; (i < m_NumQuad * 4) && (i <  Num); i++)
    {
      data[4*i+0] = pVector[i].x;
      data[4*i+1] = pVector[i].y;
      data[4*i+2] = pVector[i].z;
      data[4*i+3] = pVector[i].w;
    }

    while (i < m_NumQuad * 4)
    {
      // this happens if Num < m_NumQuad.
      // Fill the rest with the last element of the parameter array passed as argument of the function.
      data[4*i+0] = pVector[Num-1].x;
      data[4*i+1] = pVector[Num-1].y;
      data[4*i+2] = pVector[Num-1].z;
      data[4*i+3] = pVector[Num-1].w;
      i++;
    }

    VertexAttributeBuffer[AttributeIndex]->Unlock();
  }

  void TemplateQuadBuffer::UnSetQuadAttribute(UINT /* AttributeIndex */)
  {

  }
}
