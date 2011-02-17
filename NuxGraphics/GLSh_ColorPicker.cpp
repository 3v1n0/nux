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

  extern bool USE_ARB_SHADERS;

// The GLSL shaders may contain branches. Intel GPU so far fails on these shaders.
// Use assembly shaders for Intel GPUs: ARB_fragment_program does not have the required
// instruction to implement the HSV to RGB color conversion.

  static NString VtxShader = TEXT ("#version 110   \n\
        uniform mat4 ViewProjectionMatrix;      \n\
        attribute vec4 AVertex;                 \n\
        attribute vec4 VertexColor;             \n\
        void main()                             \n\
        {                                       \n\
            gl_Position = ViewProjectionMatrix * AVertex;   \n\
        }");

  static NString RedFrgShader = TEXT ("#version 110                            \n\
        uniform vec4 RectPosition;              \n\
        uniform vec4 RectDimension;             \n\
        uniform vec4 Color;                     \n\
        void main(void)                         \n\
        {                                       \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            gl_FragColor = vec4(Color.r, y, x, 1.0);                        \n\
        }");

  static NString GreenFrgShader = TEXT ("#version 110                            \n\
        uniform vec4 RectPosition;              \n\
        uniform vec4 RectDimension;             \n\
        uniform vec4 Color;                     \n\
        void main(void)                         \n\
        {                                       \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            gl_FragColor = vec4(y, Color.g, x, 1.0);                        \n\
        }");

  static NString BlueFrgShader = TEXT ("#version 110                            \n\
        uniform vec4 RectPosition;              \n\
        uniform vec4 RectDimension;             \n\
        uniform vec4 Color;                     \n\
        void main(void)                         \n\
        {                                       \n\
            float x = (gl_FragCoord.x - RectPosition.x) / RectDimension.x;  \n\
            float y = (gl_FragCoord.y - RectPosition.y) / RectDimension.y;  \n\
            gl_FragColor = vec4(x, y, Color.b, 1.0);                        \n\
        }");

  static NString HueFrgShader = TEXT ("#version 110        \n\
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
        }");

  static NString SaturationFrgShader = TEXT ("#version 110     \n\
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
        }");

  static NString ValueFrgShader = TEXT ("#version 110  \n\
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
        }");

  static NString HSV_To_RGBFrgShader = TEXT ("#version 110  \n\
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
            return (RGB);                                                       \n\
        }");

//////////////////////////////////////////////////////////////////////////////

  static NString AsmVtxShader = TEXT ("!!ARBvp1.0                                 \n\
        ATTRIB iPos         = vertex.position;      \n\
        PARAM  mvp[4]       = {state.matrix.mvp};   \n\
        OUTPUT oPos         = result.position;      \n\
        # Transform the vertex to clip coordinates. \n\
        DP4   oPos.x, mvp[0], iPos;      \n\
        DP4   oPos.y, mvp[1], iPos;      \n\
        DP4   oPos.z, mvp[2], iPos;      \n\
        DP4   oPos.w, mvp[3], iPos;      \n\
        END");

  NString AsmRedFrgShader = TEXT ("!!ARBfp1.0                  \n\
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
        END");

  NString AsmGreenFrgShader = TEXT ("!!ARBfp1.0                  \n\
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
       END");

  NString AsmBlueFrgShader = TEXT ("!!ARBfp1.0                \n\
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
       END");


  NString AsmHueFrgShader = TEXT ("!!ARBfp1.0                  \n\
        MOV result.color, {0, 0, 0, 0};                     \n\
        END");

  NString AsmSaturationFrgShader = TEXT ("!!ARBfp1.0                  \n\
       MOV result.color, {0, 0, 0, 0};                      \n\
       END");

  NString AsmValueFrgShader = TEXT ("!!ARBfp1.0                  \n\
       MOV result.color, {0, 0, 0, 0};                      \n\
       END");



  GLSh_ColorPicker::GLSh_ColorPicker (eColorChannel cc)
    :   _R (1.0)
    ,   _G (0.0)
    ,   _B (0.0)
    ,   _A (1.0)
    ,   _ScreenOffsetX (0)
    ,   _ScreenOffsetY (0)
  {
    NString FrgShaderCode;
    m_ColorChannel = cc;

    if (!USE_ARB_SHADERS && (GetGpuDevice()->GetGPUBrand() != GPU_BRAND_INTEL) )
    {
      switch (m_ColorChannel)
      {
        case CC_RED:
        {
          FrgShaderCode = RedFrgShader;
          break;
        }
        case CC_GREEN:
        {
          FrgShaderCode = GreenFrgShader;
          break;
        }
        case CC_BLUE:
        {
          FrgShaderCode = BlueFrgShader;
          break;
        }
        case CC_HUE:
        {
          FrgShaderCode = HueFrgShader;
          break;
        }
        case CC_SATURATION:
        {
          FrgShaderCode = SaturationFrgShader;
          break;
        }
        case CC_VALUE:
        {
          FrgShaderCode = ValueFrgShader;
          break;
        }
        default:
        {
          nuxDebugMsg (TEXT ("[GLSh_ColorPicker::GLSh_ColorPicker] Unknown color channel") );
          FrgShaderCode = RedFrgShader;
          break;
        }
      }

      GlobalPixelShader = GetGpuDevice()->CreatePixelShader();
      sprog = GetGpuDevice()->CreateShaderProgram();

      GlobalPixelShader->SetShaderCode (HSV_To_RGBFrgShader.GetTCharPtr() );

      sprog->AddShaderObject (GlobalPixelShader);
      sprog->LoadVertexShader (VtxShader.GetTCharPtr(), NULL);
      sprog->LoadPixelShader (FrgShaderCode.GetTCharPtr(), NULL);
      sprog->Link();
    }
    else
    {
      switch (m_ColorChannel)
      {
        case CC_RED:
        {
          FrgShaderCode = AsmRedFrgShader;
          break;
        }
        case CC_GREEN:
        {
          FrgShaderCode = AsmGreenFrgShader;
          break;
        }
        case CC_BLUE:
        {
          FrgShaderCode = AsmBlueFrgShader;
          break;
        }
        case CC_HUE:
        {
          FrgShaderCode = AsmHueFrgShader;
          break;
        }
        case CC_SATURATION:
        {
          FrgShaderCode = AsmSaturationFrgShader;
          break;
        }
        case CC_VALUE:
        {
          FrgShaderCode = AsmValueFrgShader;
          break;
        }
        default:
        {
          nuxDebugMsg (TEXT ("[GLSh_ColorPicker::GLSh_ColorPicker] Unknown color channel") );
          FrgShaderCode = RedFrgShader;
          break;
        }
      }

      m_AsmProg = GetGpuDevice()->CreateAsmShaderProgram();
      m_AsmProg->LoadVertexShader (AsmVtxShader.GetTCharPtr() );
      m_AsmProg->LoadPixelShader (FrgShaderCode.GetTCharPtr() );
      m_AsmProg->Link();
    }
  }

  GLSh_ColorPicker::~GLSh_ColorPicker()
  {
    GlobalPixelShader = ObjectPtr<IOpenGLPixelShader> (0);
    sprog.Release();
    m_AsmProg.Release();
  }

  void GLSh_ColorPicker::SetColor (float R, float G, float B, float A)
  {
    _R = R;
    _G = G;
    _B = B;
    _A = A;
  }

  void GLSh_ColorPicker::SetScreenPositionOffset (float x, float y)
  {
    _ScreenOffsetX = x;
    _ScreenOffsetY = y;
  }

  void GLSh_ColorPicker::Render (int x, int y, int z, int width, int height, int WindowWidth, int WindowHeight)
  {
    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f,
      x,          y + height, 0.0f, 1.0f,
      x + width,  y + height, 0.0f, 1.0f,
      x + width,  y,          0.0f, 1.0f,
    };

    if (!USE_ARB_SHADERS && (GetGpuDevice()->GetGPUBrand() != GPU_BRAND_INTEL) )
    {
      CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
      CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
      sprog->Begin();

      int VertexLocation = sprog->GetAttributeLocation ("AVertex");

      int VPMatrixLocation = sprog->GetUniformLocationARB ("ViewProjectionMatrix");
      sprog->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetThreadGraphicsContext()->GetModelViewProjectionMatrix().m) );

      int ColorBase    = sprog->GetUniformLocationARB ("Color");
      int RectPosition    = sprog->GetUniformLocationARB ("RectPosition");
      int RectDimension   = sprog->GetUniformLocationARB ("RectDimension");

      if (ColorBase != -1)
        CHECKGL ( glUniform4fARB (ColorBase, _R, _G, _B, _A) );

      if (RectPosition != -1)
        CHECKGL ( glUniform4fARB (RectPosition, x + _ScreenOffsetX, WindowHeight - y - height - _ScreenOffsetY, z, 0.0f) );

      if (RectDimension != -1)
        CHECKGL ( glUniform4fARB (RectDimension, width, height, 0.0f, 0.0f) );

      CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 16, VtxBuffer) );

      CHECKGL ( glDrawArrays (GL_TRIANGLE_FAN, 0, 4) );

      CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

      sprog->End();
    }
#ifndef NUX_OPENGLES_20
    else
    {
      CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
      CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
      m_AsmProg->Begin();

      CHECKGL ( glMatrixMode (GL_MODELVIEW) );
      CHECKGL ( glLoadIdentity() );
      CHECKGL ( glLoadMatrixf ( (FLOAT *) GetThreadGraphicsContext()->GetModelViewMatrix().m) );
      CHECKGL ( glMatrixMode (GL_PROJECTION) );
      CHECKGL ( glLoadIdentity() );
      CHECKGL ( glLoadMatrixf ( (FLOAT *) GetThreadGraphicsContext()->GetProjectionMatrix().m) );

      int VertexLocation          = VTXATTRIB_POSITION;

      CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 0, x + _ScreenOffsetX, WindowHeight - y - height - _ScreenOffsetY, z, 0.0f) );
      CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 1, width, height, 0.0f, 0.0f) );
      CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 2, _R, _G, _B, _A) );

      CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 16, VtxBuffer) );

      CHECKGL ( glDrawArrays (GL_TRIANGLE_FAN, 0, 4) );

      CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

      m_AsmProg->End();
    }
#endif
  }

}
