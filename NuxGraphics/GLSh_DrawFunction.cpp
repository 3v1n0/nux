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
#include "GLResourceManager.h"
#include "GpuDevice.h"

#include "GLTemplatePrimitiveBuffer.h"
#include "GraphicsEngine.h"

#include "GLSh_DrawFunction.h"

namespace nux
{

  static std::string VtxShader = "            \n\
        uniform mat4 ViewProjectionMatrix;      \n\
        attribute vec4 AVertex;                 \n\
        attribute vec4 VertexColor;             \n\
        void main()                             \n\
        {                                       \n\
            gl_Position = ViewProjectionMatrix * AVertex;   \n\
        }";

  static std::string FrgShader = "                    \n\
        #ifdef GL_ES                                    \n\
        precision mediump float;                        \n\
        #endif                                          \n\
        uniform sampler2D TextureFunction;              \n\
        uniform vec4 RectPosition;                      \n\
        uniform vec4 RectDimension;                     \n\
        uniform vec4 Color;                             \n\
        void main()                                     \n\
        {                                               \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            float s = texture2D(TextureFunction, vec2(x, 0.0)).r;           \n\
            if (y > s)                                                       \n\
            {                                                               \n\
                s = 0.0;                                                    \n\
                gl_FragColor = Color;                                       \n\
                discard;                                                    \n\
            }                                                               \n\
            else                                                            \n\
            {                                                               \n\
                s = 1.0 - (s-y) / s;                                        \n\
                gl_FragColor = Color;                                       \n\
            }                                                               \n\
        }";


  static std::string AsmVtxShader = "!!ARBvp1.0                                 \n\
        ATTRIB iPos         = vertex.position;      \n\
        PARAM  mvp[4]       = {state.matrix.mvp};   \n\
        OUTPUT oPos         = result.position;      \n\
        # Transform the vertex to clip coordinates. \n\
        DP4   oPos.x, mvp[0], iPos;      \n\
        DP4   oPos.y, mvp[1], iPos;      \n\
        DP4   oPos.z, mvp[2], iPos;      \n\
        DP4   oPos.w, mvp[3], iPos;      \n\
        END";

  std::string AsmFrgShader = "!!ARBfp1.0                  \n\
        PARAM RectPosition = program.local[0];              \n\
        PARAM RectDimension = program.local[1];             \n\
        PARAM Color = program.local[2];                     \n\
        TEMP temp0;                                         \n\
        TEMP temp1;                                         \n\
        TEMP tex0;                                          \n\
        SUB temp0.x, fragment.position.x, RectPosition.x;   \n\
        SUB temp0.y, fragment.position.y, RectPosition.y;   \n\
        RCP temp1.x, RectDimension.x;                       \n\
        RCP temp1.y, RectDimension.y;                       \n\
        MUL temp1.xy, temp0, temp1;                         \n\
        TEX tex0, temp1, texture[0], 2D;       \n\
        SUB temp0, tex0.xxxx, temp1.yyyy;                   \n\
        KIL temp0;              \n\
        MOV result.color, Color;                            \n\
        END";


  GLSh_DrawFunction::GLSh_DrawFunction()
    :   _ScreenOffsetX(0)
    ,   _ScreenOffsetY(0)
  {
    if (GetGraphicsDisplay()->GetGraphicsEngine()->UsingGLSLCodePath() && (GetGraphicsDisplay()->GetGpuDevice()->GetGPUBrand() != GPU_BRAND_INTEL))
    {
      sprog = GetGraphicsDisplay()->GetGpuDevice()->CreateShaderProgram();
      sprog->LoadVertexShader(VtxShader.c_str(), NULL);
      sprog->LoadPixelShader(FrgShader.c_str(), NULL);
      sprog->Link();
    }
    else
    {
#ifndef NUX_OPENGLES_20
      m_AsmProg = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
      m_AsmProg->LoadVertexShader(AsmVtxShader.c_str());
      m_AsmProg->LoadPixelShader(AsmFrgShader.c_str());
      m_AsmProg->Link();
#endif
    }
  }

  GLSh_DrawFunction::~GLSh_DrawFunction()
  {
    sprog = ObjectPtr<IOpenGLShaderProgram> (0);
  }

  void GLSh_DrawFunction::SetBackgroundColor(Color const& color)
  {
    background_color_ = color;
  }

  void GLSh_DrawFunction::Render(int x, int y, int z, int width, int height, int /* WindowWidth */, int WindowHeight)
  {
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f,
      fx,          fy + height, 0.0f, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f,
      fx + width,  fy,          0.0f, 1.0f,
    };

    if (GetGraphicsDisplay()->GetGraphicsEngine()->UsingGLSLCodePath() && (GetGraphicsDisplay()->GetGpuDevice()->GetGPUBrand() != GPU_BRAND_INTEL))
    {
      CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
      CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
      sprog->Begin();

      int VertexLocation = sprog->GetAttributeLocation("AVertex");

      int     VPMatrixLocation = sprog->GetUniformLocationARB("ViewProjectionMatrix");
      Matrix4 MVPMatrix = GetGraphicsDisplay()->GetGraphicsEngine()->GetOpenGLModelViewProjectionMatrix();
      sprog->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

      GetGraphicsDisplay()->GetGraphicsEngine()->SetTexture(GL_TEXTURE0, m_device_texture);

      int ColorBase       = sprog->GetUniformLocationARB("Color");
      int RectPosition    = sprog->GetUniformLocationARB("RectPosition");
      int RectDimension   = sprog->GetUniformLocationARB("RectDimension");
      int TextureFunction = sprog->GetUniformLocationARB("TextureFunction");

      if (ColorBase != -1)
        CHECKGL(glUniform4fARB(ColorBase, background_color_.red, background_color_.green, background_color_.blue, background_color_.alpha));

      if (RectPosition != -1)
        CHECKGL(glUniform4fARB(RectPosition, x + _ScreenOffsetX, WindowHeight - y - height - _ScreenOffsetY, z, 0.0f));

      if (RectDimension != -1)
        CHECKGL(glUniform4fARB(RectDimension, width, height, 0.0f, 0.0f));

      if (TextureFunction != -1)
        CHECKGL(glUniform1iARB(TextureFunction, 0));

      CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 16, VtxBuffer));

      CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

      CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

      sprog->End();
    }
#ifndef NUX_OPENGLES_20
    else
    {
      CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
      CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
      m_AsmProg->Begin();

      CHECKGL(glMatrixMode(GL_MODELVIEW));
      CHECKGL(glLoadIdentity());
      CHECKGL(glLoadMatrixf((FLOAT *) GetGraphicsDisplay()->GetGraphicsEngine()->GetOpenGLModelViewMatrix().m));
      CHECKGL(glMatrixMode(GL_PROJECTION));
      CHECKGL(glLoadIdentity());
      CHECKGL(glLoadMatrixf((FLOAT *) GetGraphicsDisplay()->GetGraphicsEngine()->GetOpenGLProjectionMatrix().m));

      int VertexLocation          = VTXATTRIB_POSITION;

      GetGraphicsDisplay()->GetGraphicsEngine()->SetTexture(GL_TEXTURE0, m_device_texture);

      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, x + _ScreenOffsetX, WindowHeight - y - height - _ScreenOffsetY, z, 0.0f));
      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, width, height, 0.0f, 0.0f));
      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, background_color_.red, background_color_.green, background_color_.blue, background_color_.alpha));

      CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 16, VtxBuffer));

      CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

      CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

      m_AsmProg->End();
    }
#endif
  }

  void GLSh_DrawFunction::SetTextureFunction(ObjectPtr<IOpenGLBaseTexture> device_texture)
  {
    m_device_texture = device_texture;
  }

  void GLSh_DrawFunction::SetScreenPositionOffset(float x, float y)
  {
    _ScreenOffsetX = x;
    _ScreenOffsetY = y;
  }

}
