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

#include "NuxCore/NuxCore.h"
#include "NuxCore/Math/Matrix4.h"
#include "GLResource.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"
#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GLTemplatePrimitiveBuffer.h"
#include "GraphicsEngine.h"
#include "GLShaderParameter.h"

#include "GLSh_ColorPicker.h"

namespace nux
{

// The GLSL shaders may contain branches. Intel GPU so far fails on these shaders.
// Use assembly shaders for Intel GPUs: ARB_fragment_program does not have the required
// instruction to implement the HSV to RGB color conversion.

  const char* VtxShader = "            \n\
        uniform mat4 ViewProjectionMatrix;      \n\
        attribute vec4 AVertex;                 \n\
        attribute vec4 VertexColor;             \n\
        void main()                             \n\
        {                                       \n\
            gl_Position = ViewProjectionMatrix * AVertex;   \n\
        }";

  const char* RedFrgShader = "         \n\
        #ifdef GL_ES                            \n\
        precision mediump float;                \n\
        #endif                                  \n\
        uniform vec4 RectPosition;              \n\
        uniform vec4 RectDimension;             \n\
        uniform vec4 Color;                     \n\
        void main(void)                         \n\
        {                                       \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            gl_FragColor = vec4(Color.r, y, x, 1.0);                        \n\
        }";

  const char* GreenFrgShader = "       \n\
        #ifdef GL_ES                            \n\
        precision mediump float;                \n\
        #endif                                  \n\
        uniform vec4 RectPosition;              \n\
        uniform vec4 RectDimension;             \n\
        uniform vec4 Color;                     \n\
        void main(void)                         \n\
        {                                       \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            gl_FragColor = vec4(y, Color.g, x, 1.0);                        \n\
        }";

  const char* BlueFrgShader = "        \n\
        #ifdef GL_ES                            \n\
        precision mediump float;                \n\
        #endif                                  \n\
        uniform vec4 RectPosition;              \n\
        uniform vec4 RectDimension;             \n\
        uniform vec4 Color;                     \n\
        void main(void)                         \n\
        {                                       \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            gl_FragColor = vec4(x, y, Color.b, 1.0);                        \n\
        }";

  const char* HueFrgShader = "                 \n\
        #ifdef GL_ES                                    \n\
        precision mediump float;                        \n\
        #endif                                          \n\
        vec3 HSV_To_RGB(vec3 HSV);                      \n\
        uniform vec4 RectPosition;                      \n\
        uniform vec4 RectDimension;                     \n\
        uniform vec4 Color;                             \n\
        void main(void)                                 \n\
        {                                               \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            vec3 rgb = HSV_To_RGB(vec3(Color.x, x, y));         \n\
            gl_FragColor = vec4(rgb, 1.0);                      \n\
        }";

  const char* SaturationFrgShader = "              \n\
        #ifdef GL_ES                                        \n\
        precision mediump float;                            \n\
        #endif                                              \n\
        vec3 HSV_To_RGB(vec3 HSV);                          \n\
        uniform vec4 RectPosition;                          \n\
        uniform vec4 RectDimension;                         \n\
        uniform vec4 Color;                                 \n\
        void main(void)                                     \n\
        {                                                   \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            vec3 rgb = HSV_To_RGB(vec3(x, Color.y, y));         \n\
            gl_FragColor = vec4(rgb, 1.0);                      \n\
        }";

  const char* ValueFrgShader = "           \n\
        #ifdef GL_ES                                \n\
        precision mediump float;                    \n\
        #endif                                      \n\
        vec3 HSV_To_RGB(vec3 HSV);                  \n\
        uniform vec4 RectPosition;                  \n\
        uniform vec4 RectDimension;                 \n\
        uniform vec4 Color;                         \n\
        void main(void)                             \n\
        {                                           \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            vec3 rgb = HSV_To_RGB(vec3(x, y, Color.z));                     \n\
            gl_FragColor = vec4(rgb, 1.0);                                  \n\
        }";

  const char* HSV_To_RGBFrgShader = "                                  \n\
        #ifdef GL_ES                                                            \n\
        precision mediump float;                                                \n\
        #endif                                                                  \n\
        vec3 HSV_To_RGB(vec3 HSV)                                               \n\
        {                                                                       \n\
            vec3 RGB = vec3(HSV.z);                                             \n\
            if ( HSV.y != 0.0 )                                                 \n\
            {                                                                   \n\
                float var_h = HSV.x * 6.0;                                      \n\
                float var_i = floor(var_h);   // Or ... var_i = floor( var_h )  \n\
                float var_1 = HSV.z * (1.0 - HSV.y);                            \n\
                float var_2 = HSV.z * (1.0 - HSV.y * (var_h-var_i));            \n\
                float var_3 = HSV.z * (1.0 - HSV.y * (1.0-(var_h-var_i)));      \n\
                if      (var_i == 0.0) { RGB = vec3(HSV.z, var_3, var_1); }     \n\
                else if (var_i == 1.0) { RGB = vec3(var_2, HSV.z, var_1); }     \n\
                else if (var_i == 2.0) { RGB = vec3(var_1, HSV.z, var_3); }     \n\
                else if (var_i == 3.0) { RGB = vec3(var_1, var_2, HSV.z); }     \n\
                else if (var_i == 4.0) { RGB = vec3(var_3, var_1, HSV.z); }     \n\
                else                 { RGB = vec3(HSV.z, var_1, var_2); }       \n\
            }                                                                   \n\
            return(RGB);                                                       \n\
        }";

//////////////////////////////////////////////////////////////////////////////

  const char* AsmVtxShader = "!!ARBvp1.0                                 \n\
        ATTRIB iPos         = vertex.position;      \n\
        PARAM  mvp[4]       = {state.matrix.mvp};   \n\
        OUTPUT oPos         = result.position;      \n\
        # Transform the vertex to clip coordinates. \n\
        DP4   oPos.x, mvp[0], iPos;      \n\
        DP4   oPos.y, mvp[1], iPos;      \n\
        DP4   oPos.z, mvp[2], iPos;      \n\
        DP4   oPos.w, mvp[3], iPos;      \n\
        END";

  const char* AsmRedFrgShader = "!!ARBfp1.0                  \n\
        PARAM RectPosition = program.local[0];              \n\
        PARAM RectDimension = program.local[1];             \n\
        PARAM Color = program.local[2];                     \n\
        TEMP temp0;                                         \n\
        TEMP temp1;                                         \n\
        SUB temp0.x, fragment.position.x, RectPosition.x;   \n\
        SUB temp0.y, fragment.position.y, RectPosition.y;   \n\
        RCP temp1.x, RectDimension.x;                       \n\
        RCP temp1.y, RectDimension.y;                       \n\
        MUL temp0.xy, temp0, temp1;                         \n\
        MOV temp1.x, Color;                                 \n\
        MOV temp1.yz, temp0.yyxx;                           \n\
        MOV temp1.w, {1, 1, 1, 1};                          \n\
        MOV result.color, temp1;                            \n\
        END";

  const char* AsmGreenFrgShader = "!!ARBfp1.0                  \n\
       PARAM RectPosition = program.local[0];              \n\
       PARAM RectDimension = program.local[1];             \n\
       PARAM Color = program.local[2];                     \n\
       TEMP temp0;                                         \n\
       TEMP temp1;                                         \n\
       SUB temp0.x, fragment.position.x, RectPosition.x;   \n\
       SUB temp0.y, fragment.position.y, RectPosition.y;   \n\
       RCP temp1.x, RectDimension.x;                       \n\
       RCP temp1.y, RectDimension.y;                       \n\
       MUL temp0.xy, temp0, temp1;                         \n\
       MOV temp1.y, Color;                                 \n\
       MOV temp1.xz, temp0.yyxx;                           \n\
       MOV temp1.w, {1, 1, 1, 1};                          \n\
       MOV result.color, temp1;                            \n\
       END";

  const char* AsmBlueFrgShader = "!!ARBfp1.0                \n\
       PARAM RectPosition = program.local[0];              \n\
       PARAM RectDimension = program.local[1];             \n\
       PARAM Color = program.local[2];                     \n\
       TEMP temp0;                                         \n\
       TEMP temp1;                                         \n\
       SUB temp0.x, fragment.position.x, RectPosition.x;   \n\
       SUB temp0.y, fragment.position.y, RectPosition.y;   \n\
       RCP temp1.x, RectDimension.x;                       \n\
       RCP temp1.y, RectDimension.y;                       \n\
       MUL temp0.xy, temp0, temp1;                         \n\
       MOV temp1.z, Color;                                 \n\
       MOV temp1.xy, temp0.xyxx;                           \n\
       MOV temp1.w, {1, 1, 1, 1};                          \n\
       MOV result.color, temp1;                            \n\
       END";


  const char* AsmHueFrgShader = "!!ARBfp1.0                  \n\
        MOV result.color, {0, 0, 0, 0};                     \n\
        END";

  const char* AsmSaturationFrgShader = "!!ARBfp1.0                  \n\
       MOV result.color, {0, 0, 0, 0};                      \n\
       END";

  const char* AsmValueFrgShader = "!!ARBfp1.0                  \n\
       MOV result.color, {0, 0, 0, 0};                      \n\
       END";



GLSh_ColorPicker::GLSh_ColorPicker(color::Channel color_channel)
    :   _R(1.0)
    ,   _G(0.0)
    ,   _B(0.0)
    ,   _A(1.0)
    ,   _ScreenOffsetX(0)
    ,   _ScreenOffsetY(0)
  {
    const char* FrgShaderCode;

#ifdef NUX_ARCH_ARM
    if (GetGraphicsDisplay()->GetGraphicsEngine()->UsingGLSLCodePath())
#else
    if (GetGraphicsDisplay()->GetGraphicsEngine()->UsingGLSLCodePath() && (GetGraphicsDisplay()->GetGpuDevice()->GetGPUBrand() != GPU_BRAND_INTEL))
#endif
    {
      switch(color_channel)
      {
        case color::RED:
        {
          FrgShaderCode = RedFrgShader;
          break;
        }
        case color::GREEN:
        {
          FrgShaderCode = GreenFrgShader;
          break;
        }
        case color::BLUE:
        {
          FrgShaderCode = BlueFrgShader;
          break;
        }
        case color::HUE:
        {
          FrgShaderCode = HueFrgShader;
          break;
        }
        case color::SATURATION:
        {
          FrgShaderCode = SaturationFrgShader;
          break;
        }
        case color::VALUE:
        {
          FrgShaderCode = ValueFrgShader;
          break;
        }
        default:
        {
          nuxDebugMsg("[GLSh_ColorPicker::GLSh_ColorPicker] Unknown color channel");
          FrgShaderCode = RedFrgShader;
          break;
        }
      }

      GlobalPixelShader = GetGraphicsDisplay()->GetGpuDevice()->CreatePixelShader();
      sprog = GetGraphicsDisplay()->GetGpuDevice()->CreateShaderProgram();

      GlobalPixelShader->SetShaderCode(HSV_To_RGBFrgShader);

      sprog->AddShaderObject(GlobalPixelShader);
      sprog->LoadVertexShader(VtxShader, NULL);
      sprog->LoadPixelShader(FrgShaderCode, NULL);
      sprog->Link();
    }
    else
    {
      switch(color_channel)
      {
        case color::RED:
        {
          FrgShaderCode = AsmRedFrgShader;
          break;
        }
        case color::GREEN:
        {
          FrgShaderCode = AsmGreenFrgShader;
          break;
        }
        case color::BLUE:
        {
          FrgShaderCode = AsmBlueFrgShader;
          break;
        }
        case color::HUE:
        {
          FrgShaderCode = AsmHueFrgShader;
          break;
        }
        case color::SATURATION:
        {
          FrgShaderCode = AsmSaturationFrgShader;
          break;
        }
        case color::VALUE:
        {
          FrgShaderCode = AsmValueFrgShader;
          break;
        }
        default:
        {
          nuxDebugMsg("[GLSh_ColorPicker::GLSh_ColorPicker] Unknown color channel");
          FrgShaderCode = RedFrgShader;
          break;
        }
      }
#ifndef NUX_OPENGLES_20
      m_AsmProg = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
      m_AsmProg->LoadVertexShader(AsmVtxShader);
      m_AsmProg->LoadPixelShader(FrgShaderCode);
      m_AsmProg->Link();
#endif
    }
  }

  GLSh_ColorPicker::~GLSh_ColorPicker()
  {
    GlobalPixelShader = ObjectPtr<IOpenGLPixelShader> (0);
    sprog.Release();
#ifndef NUX_OPENGLES_20
    m_AsmProg.Release();
#endif
  }

  void GLSh_ColorPicker::SetColor(float R, float G, float B, float A)
  {
    _R = R;
    _G = G;
    _B = B;
    _A = A;
  }

  void GLSh_ColorPicker::SetScreenPositionOffset(float x, float y)
  {
    _ScreenOffsetX = x;
    _ScreenOffsetY = y;
  }

  void GLSh_ColorPicker::Render(int x, int y, int z, int width, int height, int /* WindowWidth */, int WindowHeight)
  {
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f,
      fx,          fy + height, 0.0f, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f,
      fx + width,  fy,          0.0f, 1.0f,
    };

#ifdef NUX_ARCH_ARM
    if (GetGraphicsDisplay()->GetGraphicsEngine()->UsingGLSLCodePath())
#else
    if (GetGraphicsDisplay()->GetGraphicsEngine()->UsingGLSLCodePath() && (GetGraphicsDisplay()->GetGpuDevice()->GetGPUBrand() != GPU_BRAND_INTEL))
#endif
    {
      CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
      CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
      sprog->Begin();

      int VertexLocation = sprog->GetAttributeLocation("AVertex");

      int VPMatrixLocation = sprog->GetUniformLocationARB("ViewProjectionMatrix");
      Matrix4 MVPMatrix = GetGraphicsDisplay()->GetGraphicsEngine()->GetOpenGLModelViewProjectionMatrix();

      sprog->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

      int ColorBase    = sprog->GetUniformLocationARB("Color");
      int RectPosition    = sprog->GetUniformLocationARB("RectPosition");
      int RectDimension   = sprog->GetUniformLocationARB("RectDimension");

      if (ColorBase != -1)
        CHECKGL(glUniform4fARB(ColorBase, _R, _G, _B, _A));

      if (RectPosition != -1)
        CHECKGL(glUniform4fARB(RectPosition, x + _ScreenOffsetX, WindowHeight - y - height - _ScreenOffsetY, z, 0.0f));

      if (RectDimension != -1)
        CHECKGL(glUniform4fARB(RectDimension, width, height, 0.0f, 0.0f));

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

      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, x + _ScreenOffsetX, WindowHeight - y - height - _ScreenOffsetY, z, 0.0f));
      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, width, height, 0.0f, 0.0f));
      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, _R, _G, _B, _A));

      CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 16, VtxBuffer));

      CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

      CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

      m_AsmProg->End();
    }
#endif
  }

}
