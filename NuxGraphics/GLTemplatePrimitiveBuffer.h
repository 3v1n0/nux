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


#ifndef TEMPLATEPRIMITIVEBUFFER_H
#define TEMPLATEPRIMITIVEBUFFER_H

namespace nux
{

  class IOpenGLVertexBuffer;
  class IOpenGLIndexBuffer;
  class GpuDevice;

  typedef struct
  {
    Vector4 v0;
    Vector4 v1;
    Vector4 v2;
    Vector4 v3;
  } QuadAttributeParam;

  class TemplateQuadBuffer
  {
  public:
    TemplateQuadBuffer(GpuDevice *, ShaderType Type = SHADER_TYPE_GLSL, int NumQuads = 256);
    ~TemplateQuadBuffer();
    //! Bind GLSL parameter
    void BindAttribute(INT AttributeLocation, UINT AttributeIndex);
    //! Bind NVidia CG parameter

#if (NUX_ENABLE_CG_SHADERS)
    void BindCGAttribute(CGparameter AttributeLocation, UINT AttributeIndex);
    void UnBindCGAttribute(CGparameter AttributeLocation);
#endif
    void UnBindAttribute(INT AttributeLocation);
    void UnBind();
    void Render(INT NumPrimitives);
    //! Set the Vertices's attribute on a per quad basis.
    /*!
        Set the Vertices's attribute on a per quad basis.
        All vertex of the quad will have the same value for the attribute index.
    */
    void SetPerQuadAttribute(UINT AttributeIndex, INT Num, Vector4 *);

    //! Set the Vertices's attribute on a per vertex basis.
    /*!
        Set the vertex attribute on a per vertex basis.
    */
    void SetPerVertexAttribute(UINT AttributeIndex, INT Num, Vector4 *pVector);
    void UnSetQuadAttribute(UINT AttributeIndex);

    void SetNumQuads(int NumQuads);
    int GetNumQuads() const;

  protected:
    //IOpenGLVertexBuffer* m_VB;
    ObjectPtr<IOpenGLIndexBuffer> m_IB;
    void FormatQuads();

  private:

    ShaderType m_ShaderType;
    ObjectPtr<IOpenGLVertexBuffer> VertexAttributeBuffer[16];
    GpuDevice *m_pDeviceFactory;
    INT m_NumVertex;
    INT m_NumQuad;
  };

}

#endif //TEMPLATEPRIMITIVEBUFFER_H

