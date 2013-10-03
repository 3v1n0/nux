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
#include "ImageSurface.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"
#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "RenderingPipe.h"
#include "GraphicsEngine.h"

namespace nux
{
  void GraphicsEngine::InitSlColorShader()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString =  
      NUX_VERTEX_SHADER_HEADER
      "uniform mat4 ViewProjectionMatrix;                \n\
      attribute vec4 AVertex;                            \n\
      attribute vec4 VertexColor;                        \n\
      varying vec4 vColor;                               \n\
      void main()                                        \n\
      {                                                  \n\
      gl_Position = ViewProjectionMatrix * AVertex;  \n\
      vColor = VertexColor;                          \n\
      }";

    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));

    const char* PSString =  
      NUX_FRAGMENT_SHADER_HEADER
      "varying vec4 vColor;           \n\
      void main()                    \n\
      {                              \n\
      gl_FragColor = vColor;     \n\
      }";
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString));

    m_SlColor = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    m_SlColor->ClearShaderObjects();
    m_SlColor->AddShaderObject(VS);
    m_SlColor->AddShaderObject(PS);
    m_SlColor->Link();
  }

  void GraphicsEngine::InitSlTextureShader()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString =  
        NUX_VERTEX_SHADER_HEADER
        "attribute vec4 AVertex;                                \n\
        attribute vec4 MyTextureCoord0;                         \n\
        attribute vec4 VertexColor;                             \n\
        uniform mat4 ViewProjectionMatrix;                      \n\
        varying vec4 varyTexCoord0;                             \n\
        varying vec4 varyVertexColor;                           \n\
        void main()                                             \n\
        {                                                       \n\
        gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
        varyTexCoord0 = MyTextureCoord0;                        \n\
        varyVertexColor = VertexColor;                          \n\
        }";

    const char* PSString =  
        NUX_FRAGMENT_SHADER_HEADER
        "varying vec4 varyTexCoord0;                                \n\
        varying vec4 varyVertexColor;                               \n\
        uniform sampler2D TextureObject0;                           \n\
        vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
        {                                                           \n\
        return texture2D(TexObject, TexCoord.st);                   \n\
        }                                                           \n\
        void main()                                                 \n\
        {                                                           \n\
        vec4 v = SampleTexture(TextureObject0, varyTexCoord0);      \n\
        gl_FragColor = v*varyVertexColor;                           \n\
        }";

    // Textured 2D Primitive Shader
    m_SlTextureModColor = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString), "#define SAMPLERTEX2D");

    m_SlTextureModColor->ClearShaderObjects();
    m_SlTextureModColor->AddShaderObject(VS);
    m_SlTextureModColor->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(m_SlTextureModColor->GetOpenGLID(), 0, "AVertex"));
    m_SlTextureModColor->Link();
  }

  void GraphicsEngine::InitSlColorModTexMaskAlpha()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString =  
                     NUX_VERTEX_SHADER_HEADER
                     "attribute vec4 AVertex;                                 \n\
                     attribute vec4 MyTextureCoord0;                         \n\
                     attribute vec4 VertexColor;                             \n\
                     uniform mat4 ViewProjectionMatrix;                      \n\
                     varying vec4 varyTexCoord0;                             \n\
                     varying vec4 varyVertexColor;                           \n\
                     void main()                                             \n\
                     {                                                       \n\
                     gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
                     varyTexCoord0 = MyTextureCoord0;                        \n\
                     varyVertexColor = VertexColor;                          \n\
                     }";

    const char* PSString =  
                    NUX_FRAGMENT_SHADER_HEADER
                    "varying vec4 varyTexCoord0;                                        \n\
                     varying vec4 varyVertexColor;                                      \n\
                     uniform sampler2D TextureObject0;                                  \n\
                     vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)             \n\
                     {                                                                  \n\
                     return texture2D(TexObject, TexCoord.st);                          \n\
                     }                                                                  \n\
                     void main()                                                        \n\
                     {                                                                  \n\
                     float alpha = SampleTexture(TextureObject0, varyTexCoord0).w;      \n\
                     gl_FragColor = vec4(varyVertexColor.xyz, alpha*varyVertexColor.a); \n\
                     }";

    m_SlColorModTexMaskAlpha = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString), "#define SAMPLERTEX2D");
    m_SlColorModTexMaskAlpha->ClearShaderObjects();
    m_SlColorModTexMaskAlpha->AddShaderObject(VS);
    m_SlColorModTexMaskAlpha->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(m_SlColorModTexMaskAlpha->GetOpenGLID(), 0, "AVertex"));
    CHECKGL(glBindAttribLocation(m_SlColorModTexMaskAlpha->GetOpenGLID(), 1, "MyTextureCoord0"));
    CHECKGL(glBindAttribLocation(m_SlColorModTexMaskAlpha->GetOpenGLID(), 2, "VectexColor"));
    m_SlColorModTexMaskAlpha->Link();
  }

  void GraphicsEngine::InitSl2TextureAdd()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
    // other  attributes. Otherwise you get a bug on NVidia! Why is that???

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    const char* VSString =        NUX_VERTEX_SHADER_HEADER
                      "uniform mat4 ViewProjectionMatrix;                      \n\
                      attribute vec4 AVertex;                                 \n\
                      attribute vec4 MyTextureCoord0;                         \n\
                      attribute vec4 MyTextureCoord1;                         \n\
                      varying vec4 varyTexCoord0;                             \n\
                      varying vec4 varyTexCoord1;                             \n\
                      void main()                                             \n\
                      {                                                       \n\
                      varyTexCoord0 = MyTextureCoord0;                        \n\
                      varyTexCoord1 = MyTextureCoord1;                        \n\
                      gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
                      }";

    const char* PSString =      NUX_FRAGMENT_SHADER_HEADER
                    "varying vec4 varyTexCoord0;                                 \n\
                    varying vec4 varyTexCoord1;                                 \n\
                    uniform vec4 color0;                                        \n\
                    uniform vec4 color1;                                        \n\
                    #ifdef SAMPLERTEX2D                                         \n\
                    uniform sampler2D TextureObject0;                           \n\
                    uniform sampler2D TextureObject1;                           \n\
                    vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
                    {                                                           \n\
                    return texture2D(TexObject, TexCoord.st);                   \n\
                    }                                                           \n\
                    #elif defined SAMPLERTEX2DRECT                              \n\
                    uniform sampler2DRect TextureObject0;                       \n\
                    uniform sampler2DRect TextureObject1;                       \n\
                    vec4 SampleTexture(sampler2DRect TexObject, vec4 TexCoord)  \n\
                    {                                                           \n\
                    return texture2DRect(TexObject, TexCoord.st);               \n\
                    }                                                           \n\
                    #endif                                                      \n\
                    void main()                                                 \n\
                    {                                                           \n\
                    vec4 b0 = color0*SampleTexture(TextureObject0, varyTexCoord0);     \n\
                    vec4 b1 = color1*SampleTexture(TextureObject1, varyTexCoord1);     \n\
                    gl_FragColor = b0 + b1;                                             \n\
                    }";

    m_Sl2TextureAdd = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString), "#define SAMPLERTEX2D");

    m_Sl2TextureAdd->ClearShaderObjects();
    m_Sl2TextureAdd->AddShaderObject(VS);
    m_Sl2TextureAdd->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(m_Sl2TextureAdd->GetOpenGLID(), 0, "AVertex"));
    m_Sl2TextureAdd->Link();
  }

  void GraphicsEngine::InitSl2TextureDepRead()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
    // other  attributes. Otherwise you get a bug on NVidia! Why is that???

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    const char* VSString =          NUX_VERTEX_SHADER_HEADER
                        "uniform mat4 ViewProjectionMatrix;                      \n\
                        attribute vec4 AVertex;                                 \n\
                        attribute vec4 MyTextureCoord0;                         \n\
                        attribute vec4 MyTextureCoord1;                         \n\
                        varying vec4 varyTexCoord0;                             \n\
                        varying vec4 varyTexCoord1;                             \n\
                        void main()                                             \n\
                        {                                                       \n\
                        varyTexCoord0 = MyTextureCoord0;                        \n\
                        varyTexCoord1 = MyTextureCoord1;                        \n\
                        gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
                        }";

    const char* PSString =      NUX_FRAGMENT_SHADER_HEADER
                    "varying vec4 varyTexCoord0;                                 \n\
                    varying vec4 varyTexCoord1;                                 \n\
                    uniform vec4 color0;                                        \n\
                    uniform vec4 color1;                                        \n\
                    uniform sampler2D TextureObject0;                           \n\
                    uniform sampler2D TextureObject1;                           \n\
                    vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
                    {                                                           \n\
                      return texture2D(TexObject, TexCoord.st);                 \n\
                    }                                                           \n\
                    void main()                                                 \n\
                    {                                                           \n\
                      vec4 noise = SampleTexture(TextureObject0, varyTexCoord0); \n\
                      vec4 noise_bx2 = color0 * (2.0 * noise - vec4(1.0, 1.0, 1.0, 1.0));     \n\
                      vec4 b1 = color1 * SampleTexture(TextureObject1, varyTexCoord1 + noise_bx2);     \n\
                      gl_FragColor = b1;                                          \n\
                    }";

    m_Sl2TextureDepRead = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString));

    m_Sl2TextureDepRead->ClearShaderObjects();
    m_Sl2TextureDepRead->AddShaderObject(VS);
    m_Sl2TextureDepRead->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(m_Sl2TextureDepRead->GetOpenGLID(), 0, "AVertex"));
    m_Sl2TextureDepRead->Link();
  }

  void GraphicsEngine::InitSl2TextureMod()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
    // other  attributes. Otherwise you get a bug on NVidia! Why is that???

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    const char* VSString =           NUX_VERTEX_SHADER_HEADER
                         "uniform mat4 ViewProjectionMatrix;                      \n\
                         attribute vec4 AVertex;                                 \n\
                         attribute vec4 MyTextureCoord0;                         \n\
                         attribute vec4 MyTextureCoord1;                         \n\
                         varying vec4 varyTexCoord0;                             \n\
                         varying vec4 varyTexCoord1;                             \n\
                         void main()                                             \n\
                         {                                                       \n\
                         varyTexCoord0 = MyTextureCoord0;                        \n\
                         varyTexCoord1 = MyTextureCoord1;                        \n\
                         gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
                         }";

    const char* PSString =           NUX_FRAGMENT_SHADER_HEADER
                         "varying vec4 varyTexCoord0;                                 \n\
                         varying vec4 varyTexCoord1;                                 \n\
                         uniform vec4 color0;                                        \n\
                         uniform vec4 color1;                                        \n\
                         #ifdef SAMPLERTEX2D                                         \n\
                         uniform sampler2D TextureObject0;                           \n\
                         uniform sampler2D TextureObject1;                           \n\
                         vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
                         {                                                           \n\
                         return texture2D(TexObject, TexCoord.st);                   \n\
                         }                                                           \n\
                         #elif defined SAMPLERTEX2DRECT                              \n\
                         uniform sampler2DRect TextureObject0;                       \n\
                         uniform sampler2DRect TextureObject1;                       \n\
                         vec4 SampleTexture(sampler2DRect TexObject, vec4 TexCoord)  \n\
                         {                                                           \n\
                         return texture2DRect(TexObject, TexCoord.st);               \n\
                         }                                                           \n\
                         #endif                                                      \n\
                         void main()                                                 \n\
                         {                                                           \n\
                         vec4 b0 = color0*SampleTexture(TextureObject0, varyTexCoord0);     \n\
                         vec4 b1 = color1*SampleTexture(TextureObject1, varyTexCoord1);     \n\
                         gl_FragColor = b0 * b1;                                            \n\
                         }";

    m_Sl2TextureMod = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString), "#define SAMPLERTEX2D");

    m_Sl2TextureMod->ClearShaderObjects();
    m_Sl2TextureMod->AddShaderObject(VS);
    m_Sl2TextureMod->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(m_Sl2TextureMod->GetOpenGLID(), 0, "AVertex"));
    m_Sl2TextureMod->Link();
  }

  void GraphicsEngine::InitSl4TextureAdd()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
    // other  attributes. Otherwise you get a bug on NVidia! Why is that???

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    const char* VSString =          NUX_VERTEX_SHADER_HEADER
                        "uniform mat4 ViewProjectionMatrix;                      \n\
                        attribute vec4 AVertex;                                 \n\
                        attribute vec4 MyTextureCoord0;                         \n\
                        attribute vec4 MyTextureCoord1;                         \n\
                        attribute vec4 MyTextureCoord2;                         \n\
                        attribute vec4 MyTextureCoord3;                         \n\
                        varying vec4 varyTexCoord0;                             \n\
                        varying vec4 varyTexCoord1;                             \n\
                        varying vec4 varyTexCoord2;                             \n\
                        varying vec4 varyTexCoord3;                             \n\
                        void main()                                             \n\
                        {                                                       \n\
                        varyTexCoord0 = MyTextureCoord0;                        \n\
                        varyTexCoord1 = MyTextureCoord1;                        \n\
                        varyTexCoord2 = MyTextureCoord2;                        \n\
                        varyTexCoord3 = MyTextureCoord3;                        \n\
                        gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
                        }";

    const char* PSString =          NUX_FRAGMENT_SHADER_HEADER
                        "varying vec4 varyTexCoord0;                                 \n\
                        varying vec4 varyTexCoord1;                                 \n\
                        varying vec4 varyTexCoord2;                                 \n\
                        varying vec4 varyTexCoord3;                                 \n\
                        uniform vec4 color0;                                        \n\
                        uniform vec4 color1;                                        \n\
                        uniform vec4 color2;                                        \n\
                        uniform vec4 color3;                                        \n\
                        uniform sampler2D TextureObject0;                           \n\
                        uniform sampler2D TextureObject1;                           \n\
                        uniform sampler2D TextureObject2;                           \n\
                        uniform sampler2D TextureObject3;                           \n\
                        vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
                        {                                                           \n\
                        return texture2D(TexObject, TexCoord.st);                   \n\
                        }                                                           \n\
                        void main()                                                 \n\
                        {                                                           \n\
                          vec4 b0 = color0*SampleTexture(TextureObject0, varyTexCoord0);  \n\
                          vec4 b1 = color1*SampleTexture(TextureObject1, varyTexCoord1);  \n\
                          vec4 b2 = color2*SampleTexture(TextureObject2, varyTexCoord2);  \n\
                          vec4 b3 = color3*SampleTexture(TextureObject3, varyTexCoord3);  \n\
                          gl_FragColor = b0+b1+b2+b3;                                     \n\
                        }";

    // Textured 2D Primitive Shader
    m_Sl4TextureAdd = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString));

    m_Sl4TextureAdd->ClearShaderObjects();
    m_Sl4TextureAdd->AddShaderObject(VS);
    m_Sl4TextureAdd->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(m_Sl4TextureAdd->GetOpenGLID(), 0, "AVertex"));
    m_Sl4TextureAdd->Link();
  }

  void GraphicsEngine::InitSLPower()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString =  NUX_VERTEX_SHADER_HEADER
        "uniform mat4 ViewProjectionMatrix;                  \n\
        attribute vec4 AVertex;                             \n\
        attribute vec4 MyTextureCoord0;                     \n\
        varying vec4 varyTexCoord0;                         \n\
        void main()                                         \n\
        {                                                   \n\
          varyTexCoord0 = MyTextureCoord0;                  \n\
          gl_Position =  ViewProjectionMatrix * (AVertex);  \n\
        }";

    const char* PSString =  NUX_FRAGMENT_SHADER_HEADER
        "varying vec4 varyTexCoord0;                                                                           \n\
        uniform sampler2D TextureObject0;                                                                     \n\
        uniform vec2 TextureSize0;                                                                            \n\
        uniform vec4 exponent;                                                                                \n\
        uniform vec4 color0;                                                                                  \n\
        vec4 SampleTexture(sampler2D TexObject, vec2 TexCoord)                                                \n\
        {                                                                                                     \n\
          return texture2D(TexObject, TexCoord.st);                                                           \n\
        }                                                                                                     \n\
        void main(void)                                                                                      \n\
        {	                                                                                                    \n\
          vec4 TexColor = SampleTexture(TextureObject0, varyTexCoord0.st);                                    \n\
          vec4 result = pow(TexColor, exponent);                                                              \n\
          gl_FragColor = color0*result;                                                                       \n\
        }";


    _component_exponentiation_prog = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString));

    _component_exponentiation_prog->ClearShaderObjects();
    _component_exponentiation_prog->AddShaderObject(VS);
    _component_exponentiation_prog->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_component_exponentiation_prog->GetOpenGLID(), 0, "AVertex"));
    _component_exponentiation_prog->Link();

  }

  void GraphicsEngine::InitSLAlphaReplicate()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString = NUX_VERTEX_SHADER_HEADER
        "uniform mat4 ViewProjectionMatrix;                  \n\
        attribute vec4 AVertex;                             \n\
        attribute vec4 MyTextureCoord0;                     \n\
        varying vec4 varyTexCoord0;                         \n\
        void main()                                         \n\
        {                                                   \n\
          varyTexCoord0 = MyTextureCoord0;                  \n\
          gl_Position =  ViewProjectionMatrix * (AVertex);  \n\
        }";

    const char* PSString = NUX_FRAGMENT_SHADER_HEADER
        "varying vec4 varyTexCoord0;                                   \n\
        uniform sampler2D TextureObject0;                             \n\
        uniform vec4 color0;                                          \n\
        vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)        \n\
        {                                                             \n\
          return texture2D(TexObject, TexCoord.st);                   \n\
        }                                                             \n\
        void main()                                                  \n\
        {                                                             \n\
          vec4 v = SampleTexture(TextureObject0, varyTexCoord0);      \n\
          gl_FragColor = vec4(v.a, v.a, v.a, v.a) * color0;           \n\
        }";

    _alpha_replicate_prog = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString));

    _alpha_replicate_prog->ClearShaderObjects();
    _alpha_replicate_prog->AddShaderObject(VS);
    _alpha_replicate_prog->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_alpha_replicate_prog->GetOpenGLID(), 0, "AVertex"));
    _alpha_replicate_prog->Link();
  }

  void GraphicsEngine::InitSLHorizontalHQGaussFilter(int sigma)
  {
    int k = Clamp<int>(sigma, NUX_MIN_GAUSSIAN_SIGMA, NUX_MAX_GAUSSIAN_SIGMA);
    if (_horizontal_hq_gauss_filter_prog[k-1].IsValid())
    {
      // Shader program already compiled
      return;
    }

    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString = NUX_VERTEX_SHADER_HEADER
                     "uniform mat4 ViewProjectionMatrix;   \n\
                     attribute vec4 AVertex;              \n\
                     attribute vec4 MyTextureCoord0;      \n\
                     attribute vec4 VertexColor;          \n\
                     varying vec4 varyTexCoord0;          \n\
                     varying vec4 varyVertexColor;        \n\
                     void main()                          \n\
                     {                                    \n\
                       varyTexCoord0 = MyTextureCoord0;     \n\
                       varyVertexColor = VertexColor;       \n\
                       gl_Position =  ViewProjectionMatrix * (AVertex);  \n\
                     }";


    const char* PSString =       NUX_FRAGMENT_SHADER_HEADER
                     "varying vec4 varyTexCoord0;                                  \n\
                     varying vec4 varyVertexColor;                                \n\
                     uniform sampler2D TextureObject0;                            \n\
                     uniform vec2 TextureSize0;                                   \n\
                     vec4 SampleTexture(sampler2D TexObject, vec2 TexCoord)       \n\
                     {                                                            \n\
                     return texture2D(TexObject, TexCoord.st);                    \n\
                     }                                                            \n\
                     #define NUM_SAMPLES %d                                       \n\
                     uniform float W[NUM_SAMPLES];                                \n\
                     void main()                                                  \n\
                     {                                                            \n\
                     vec4 sum   = vec4(0.0, 0.0, 0.0, 0.0);                       \n\
                     vec2 delta = vec2(1.0 / TextureSize0.x, 0.0);                \n\
                     vec2 texCoord = vec2(varyTexCoord0.s, varyTexCoord0.t);      \n\
                     texCoord.x -= float((NUM_SAMPLES - 1) / 2) / TextureSize0.x;      \n\
                     texCoord.y += 0.0 / TextureSize0.y;                          \n\
                     for (int i = 0; i < NUM_SAMPLES; i++)                         \n\
                     {                                                            \n\
                     sum += SampleTexture(TextureObject0, texCoord) * W[i];       \n\
                     texCoord += delta;                                           \n\
                     }                                                            \n\
                     gl_FragColor = vec4(sum.x, sum.y, sum.z, 1.0);             \n\
                     }";

    int l = strlen(PSString);
    char* shader_prog = new char[l+10];
    sprintf(shader_prog, PSString, 6 * k + 1);

    _horizontal_hq_gauss_filter_prog[k-1] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(shader_prog, "#define SAMPLERTEX2D");
    delete [] shader_prog;

    _horizontal_hq_gauss_filter_prog[k-1]->ClearShaderObjects();
    _horizontal_hq_gauss_filter_prog[k-1]->AddShaderObject(VS);
    _horizontal_hq_gauss_filter_prog[k-1]->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_horizontal_hq_gauss_filter_prog[k-1]->GetOpenGLID(), 0, "AVertex"));
    _horizontal_hq_gauss_filter_prog[k-1]->Link();

  }

  void GraphicsEngine::InitSLVerticalHQGaussFilter(int sigma)
  {
    int k = Clamp<int>(sigma, NUX_MIN_GAUSSIAN_SIGMA, NUX_MAX_GAUSSIAN_SIGMA);
    if (_vertical_hq_gauss_filter_prog[k-1].IsValid())
    {
      // Shader program already compiled
      return;
    }

    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString = 
                      NUX_VERTEX_SHADER_HEADER
                     "uniform mat4 ViewProjectionMatrix;   \n\
                     attribute vec4 AVertex;              \n\
                     attribute vec4 MyTextureCoord0;      \n\
                     attribute vec4 VertexColor;          \n\
                     varying vec4 varyTexCoord0;          \n\
                     varying vec4 varyVertexColor;        \n\
                     void main()                          \n\
                     {                                    \n\
                     varyTexCoord0 = MyTextureCoord0;     \n\
                     varyVertexColor = VertexColor;       \n\
                     gl_Position =  ViewProjectionMatrix * (AVertex);  \n\
                     }";


    const char* PSString = 
                    NUX_FRAGMENT_SHADER_HEADER
                     "varying vec4 varyTexCoord0;                                  \n\
                     varying vec4 varyVertexColor;                                \n\
                     uniform sampler2D TextureObject0;                            \n\
                     uniform vec2 TextureSize0;                                   \n\
                     vec4 SampleTexture(sampler2D TexObject, vec2 TexCoord)      \n\
                     {                                                            \n\
                     return texture2D(TexObject, TexCoord.st);                   \n\
                     }                                                            \n\
                     #define NUM_SAMPLES %d                                       \n\
                     uniform float W [NUM_SAMPLES];                               \n\
                     void main()                                                 \n\
                     {                                                            \n\
                     vec4 sum   = vec4(0.0, 0.0, 0.0, 0.0);                      \n\
                     vec2 delta = vec2(0.0, 1.0 / TextureSize0.y);               \n\
                     vec2 texCoord = vec2(varyTexCoord0.s, varyTexCoord0.t);     \n\
                     texCoord.x += 0.0 / TextureSize0.x;                          \n\
                     texCoord.y -= float((NUM_SAMPLES - 1) / 2) / TextureSize0.y;      \n\
                     for (int i = 0; i < NUM_SAMPLES; ++i)                        \n\
                     {                                                            \n\
                     sum += SampleTexture(TextureObject0, texCoord) * W[i];      \n\
                     texCoord += delta;                                           \n\
                     }                                                            \n\
                     gl_FragColor = vec4(sum.x, sum.y, sum.z, 1.0);            \n\
                     }";

    int l = strlen(PSString);
    char* shader_prog = new char[l+10];
    sprintf(shader_prog, PSString, 6 * k + 1);

    _vertical_hq_gauss_filter_prog[k-1] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(shader_prog, "#define SAMPLERTEX2D");
    delete [] shader_prog;

    _vertical_hq_gauss_filter_prog[k-1]->ClearShaderObjects();
    _vertical_hq_gauss_filter_prog[k-1]->AddShaderObject(VS);
    _vertical_hq_gauss_filter_prog[k-1]->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_vertical_hq_gauss_filter_prog[k-1]->GetOpenGLID(), 0, "AVertex"));
    _vertical_hq_gauss_filter_prog[k-1]->Link();
  }
  
  void GraphicsEngine::InitSLHorizontalLSGaussFilter(int k)
  {
    if (_horizontal_ls_gauss_filter_prog[k-1].IsValid())
    {
      // Shader program already compiled
      return;
    }

    ObjectPtr<IOpenGLVertexShader> vs = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> ps = _graphics_display.m_DeviceFactory->CreatePixelShader();

    std::string vs_string = NUX_VERTEX_SHADER_HEADER
    "uniform mat4 view_projection_matrix;               \n\
     attribute vec4 vertex;                             \n\
     attribute vec4 tex_coord;                          \n\
     varying vec4 v_tex_coord;                          \n\
     void main()                                        \n\
     {                                                  \n\
      v_tex_coord = tex_coord;                          \n\
      gl_Position = view_projection_matrix * vertex;    \n\
     }";

    std::string ps_string = NUX_FRAGMENT_SHADER_HEADER
    "varying vec4 v_tex_coord;                                              \n\
     uniform sampler2D tex_object;                                          \n\
     #define NUM_SAMPLES %d                                                 \n\
     uniform vec2 taps[NUM_SAMPLES];                                        \n\
     void main()                                                            \n\
     {                                                                      \n\
      vec3 acc = texture2D(tex_object, v_tex_coord.st).rgb * taps[0][1];    \n\
      for (int i = 1; i < NUM_SAMPLES; i++)                                 \n\
      {                                                                     \n\
        vec2 tap = taps[i];                                                 \n\
        vec2 offset = vec2(tap[0], 0.0);                                    \n\
        float weight = tap[1];                                              \n\
        acc += texture2D(tex_object, v_tex_coord.st + offset).rgb * weight; \n\
        acc += texture2D(tex_object, v_tex_coord.st - offset).rgb * weight; \n\
      }                                                                     \n\
      gl_FragColor = vec4(acc, 1.0);                                        \n\
     }";

    int l = ps_string.length();
    char* shader_prog = new char[l+10];
    sprintf(shader_prog, ps_string.c_str(), k);

    _horizontal_ls_gauss_filter_prog[k-1] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    vs->SetShaderCode(vs_string.c_str());
    ps->SetShaderCode(shader_prog);
    delete[] shader_prog;

    _horizontal_ls_gauss_filter_prog[k-1]->ClearShaderObjects();
    _horizontal_ls_gauss_filter_prog[k-1]->AddShaderObject(vs);
    _horizontal_ls_gauss_filter_prog[k-1]->AddShaderObject(ps);
    CHECKGL(glBindAttribLocation(_horizontal_ls_gauss_filter_prog[k-1]->GetOpenGLID(), 0, "vertex"));
    _horizontal_ls_gauss_filter_prog[k-1]->Link();
  }

  void GraphicsEngine::InitSLVerticalLSGaussFilter(int k)
  {
    if (_vertical_ls_gauss_filter_prog[k-1].IsValid())
    {
      // Shader program already compiled
      return;
    }

    ObjectPtr<IOpenGLVertexShader> vs = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> ps = _graphics_display.m_DeviceFactory->CreatePixelShader();

    std::string vs_string = NUX_VERTEX_SHADER_HEADER
    "uniform mat4 view_projection_matrix;               \n\
     attribute vec4 vertex;                             \n\
     attribute vec4 tex_coord;                          \n\
     varying vec4 v_tex_coord;                          \n\
     void main()                                        \n\
     {                                                  \n\
      v_tex_coord = tex_coord;                          \n\
      gl_Position = view_projection_matrix * vertex;    \n\
     }";

    std::string ps_string = NUX_FRAGMENT_SHADER_HEADER
    "varying vec4 v_tex_coord;                                              \n\
     uniform sampler2D tex_object;                                          \n\
     #define NUM_SAMPLES %d                                                 \n\
     uniform vec2 taps[NUM_SAMPLES];                                        \n\
     void main()                                                            \n\
     {                                                                      \n\
      vec3 acc = texture2D(tex_object, v_tex_coord.st).rgb * taps[0][1];    \n\
      for (int i = 1; i < NUM_SAMPLES; i++)                                 \n\
      {                                                                     \n\
        vec2 tap = taps[i];                                                 \n\
        vec2 offset = vec2(0.0, tap[0]);                                    \n\
        float weight = tap[1];                                              \n\
        acc += texture2D(tex_object, v_tex_coord.st + offset).rgb * weight; \n\
        acc += texture2D(tex_object, v_tex_coord.st - offset).rgb * weight; \n\
      }                                                                     \n\
      gl_FragColor = vec4(acc, 1.0);                                        \n\
     }";

    int l = ps_string.length();
    char* shader_prog = new char[l+10];
    sprintf(shader_prog, ps_string.c_str(), k);

    _vertical_ls_gauss_filter_prog[k-1] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    vs->SetShaderCode(vs_string.c_str());
    ps->SetShaderCode(shader_prog);
    delete[] shader_prog;

    _vertical_ls_gauss_filter_prog[k-1]->ClearShaderObjects();
    _vertical_ls_gauss_filter_prog[k-1]->AddShaderObject(vs);
    _vertical_ls_gauss_filter_prog[k-1]->AddShaderObject(ps);
    CHECKGL(glBindAttribLocation(_vertical_ls_gauss_filter_prog[k-1]->GetOpenGLID(), 0, "vertex"));
    _vertical_ls_gauss_filter_prog[k-1]->Link();
  }

  void GraphicsEngine::InitSLColorMatrixFilter()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString = 
                      NUX_VERTEX_SHADER_HEADER
        "uniform mat4 ViewProjectionMatrix;  \n\
        attribute vec4 AVertex;             \n\
        attribute vec4 MyTextureCoord0;     \n\
        varying vec4 varyTexCoord0;         \n\
        void main()                         \n\
        {                                   \n\
          varyTexCoord0 = MyTextureCoord0;  \n\
          gl_Position =  ViewProjectionMatrix * (AVertex);  \n\
        }";

    const char* PSString = 
                    NUX_FRAGMENT_SHADER_HEADER
        "varying vec4 varyTexCoord0;                                 \n\
        uniform sampler2D TextureObject0;                           \n\
        uniform vec4 color0;                                        \n\
        vec4 SampleTexture(sampler2D TexObject, vec2 TexCoord)      \n\
        {                                                           \n\
          return texture2D(TexObject, TexCoord.st);                 \n\
        }                                                           \n\
        // Color Matrix                                             \n\
        uniform float CM0[5];                                       \n\
        uniform float CM1[5];                                       \n\
        uniform float CM2[5];                                       \n\
        uniform float CM3[5];                                       \n\
        void main(void)                                            \n\
        {	                                                          \n\
          vec4 tex0 = SampleTexture(TextureObject0, varyTexCoord0.st); \n\
          float r = CM0[0]* tex0.r + CM0[1]* tex0.g + CM0[2]* tex0.b + CM0[3]* tex0.a + CM0[4]; \n\
          float g = CM1[0]* tex0.r + CM1[1]* tex0.g + CM1[2]* tex0.b + CM1[3]* tex0.a + CM1[4]; \n\
          float b = CM2[0]* tex0.r + CM2[1]* tex0.g + CM2[2]* tex0.b + CM2[3]* tex0.a + CM2[4]; \n\
          float a = CM3[0]* tex0.r + CM3[1]* tex0.g + CM3[2]* tex0.b + CM3[3]* tex0.a + CM3[4]; \n\
          gl_FragColor = color0 * vec4(r, g, b, tex0.a);                                        \n\
        }";

    _color_matrix_filter_prog = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString), "#define SAMPLERTEX2D");

    _color_matrix_filter_prog->ClearShaderObjects();
    _color_matrix_filter_prog->AddShaderObject(VS);
    _color_matrix_filter_prog->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_color_matrix_filter_prog->GetOpenGLID(), 0, "AVertex"));
    _color_matrix_filter_prog->Link();
  }

  void GraphicsEngine::QRP_GLSL_Color(int x, int y, int width, int height, const Color &color)
  {
    QRP_GLSL_Color(x, y, width, height, color, color, color, color);
  }

  void GraphicsEngine::QRP_GLSL_Color(int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3)
  {
    if (!m_SlColor.IsValid())
      InitSlColorShader();

    m_quad_tex_stats++;

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, c0.red, c0.green, c0.blue, c0.alpha,
      fx,          fy + height, 0.0f, 1.0f, c1.red, c1.green, c1.blue, c1.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, c2.red, c2.green, c2.blue, c2.alpha,
      fx + width,  fy,          0.0f, 1.0f, c3.red, c3.green, c3.blue, c3.alpha,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg = m_SlColor;

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();
  }

  void GraphicsEngine::QRP_GLSL_1Tex(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color0)
  {
    if (!m_SlTextureModColor.IsValid())
        InitSlTextureShader();

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, DeviceTexture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;

    if (DeviceTexture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      ShaderProg = m_SlTextureModColor;
    }

//     if (DeviceTexture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType) ||
//         DeviceTexture->Type().IsDerivedFromType(IOpenGLAnimatedTexture::StaticObjectType))
//     {
//         ShaderProg = m_TexturedRectProg;
//     }

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");

    SetTexture(GL_TEXTURE0, DeviceTexture);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();
  }

// Render the texture alpha into RGB and modulated by a color.
  void GraphicsEngine::QRP_GLSL_ColorModTexAlpha(int x, int y, int width, int height,
      ObjectPtr< IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform, const Color &color)
  {
    if (!m_SlColorModTexMaskAlpha.IsValid())
        InitSlColorModTexMaskAlpha();

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, DeviceTexture, texxform);

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 0, color.red, color.green, color.blue, color.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 0, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 0, color.red, color.green, color.blue, color.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 0, color.red, color.green, color.blue, color.alpha,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;
//     if (DeviceTexture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      ShaderProg = m_SlColorModTexMaskAlpha;
    }

    if (DeviceTexture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType) ||
        DeviceTexture->Type().IsDerivedFromType(IOpenGLAnimatedTexture::StaticObjectType))
    {
      ShaderProg = m_SlColorModTexRectMaskAlpha;
    }

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");

    SetTexture(GL_TEXTURE0, DeviceTexture);

    if (TextureObjectLocation != -1)
    {
      CHECKGL(glUniform1iARB(TextureObjectLocation, 0));
    }

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    if (VertexLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));
    }

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArray(TextureCoord0Location));
      CHECKGL(glVertexAttribPointer((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    if (VertexLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));
    }

    ShaderProg->End();
  }

// Blend 2 textures together
  void GraphicsEngine::QRP_GLSL_2Tex(int x, int y, int width, int height,
                                       ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                                       ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1)
  {
    if (!m_Sl2TextureAdd.IsValid())
        InitSl2TextureAdd();

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;
    ShaderProg = m_Sl2TextureAdd;

    QRP_Compute_Texture_Coord(width, height, DeviceTexture0, texxform0);
    QRP_Compute_Texture_Coord(width, height, DeviceTexture1, texxform1);

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation0 = ShaderProg->GetUniformLocationARB("TextureObject0");
    int TextureObjectLocation1 = ShaderProg->GetUniformLocationARB("TextureObject1");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int TextureCoord1Location = ShaderProg->GetAttributeLocation("MyTextureCoord1");

    int TextureCoef0Location = ShaderProg->GetUniformLocationARB("color0");
    int TextureCoef1Location = ShaderProg->GetUniformLocationARB("color1");


    SetTexture(GL_TEXTURE0, DeviceTexture0);
    SetTexture(GL_TEXTURE1, DeviceTexture1);

    CHECKGL(glUniform1iARB(TextureObjectLocation0, 0));
    CHECKGL(glUniform1iARB(TextureObjectLocation1, 1));

    CHECKGL(glUniform4fARB(TextureCoef0Location, color0.red, color0.green, color0.blue, color0.alpha ));
    CHECKGL(glUniform4fARB(TextureCoef1Location, color1.red, color1.green, color1.blue, color1.alpha ));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));

    ShaderProg->End();
  }

  void GraphicsEngine::QRP_GLSL_DisturbedTexture(
    int x, int y, int width, int height,
    ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm &texxform0, const Color& c0,
    ObjectPtr<IOpenGLBaseTexture> src_device_texture, TexCoordXForm &texxform1, const Color& c1)
  {
    if (!m_Sl2TextureDepRead.IsValid())
        InitSl2TextureDepRead();

    ObjectPtr<IOpenGLShaderProgram> ShaderProg = m_Sl2TextureDepRead;

    QRP_Compute_Texture_Coord(width, height, distorsion_texture, texxform0);
    QRP_Compute_Texture_Coord(width, height, src_device_texture, texxform1);

    float fx = x, fy = y;
    float VtxBuffer [] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation0 = ShaderProg->GetUniformLocationARB("TextureObject0");
    int TextureObjectLocation1 = ShaderProg->GetUniformLocationARB("TextureObject1");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int TextureCoord1Location = ShaderProg->GetAttributeLocation("MyTextureCoord1");

    int TextureCoef0Location = ShaderProg->GetUniformLocationARB("color0");
    int TextureCoef1Location = ShaderProg->GetUniformLocationARB("color1");


    SetTexture(GL_TEXTURE0, distorsion_texture);
    SetTexture(GL_TEXTURE1, src_device_texture);

    CHECKGL(glUniform1iARB(TextureObjectLocation0, 0));
    CHECKGL(glUniform1iARB(TextureObjectLocation1, 1));

    CHECKGL(glUniform4fARB(TextureCoef0Location, c0.red, c0.green, c0.blue, c0.alpha ));
    CHECKGL(glUniform4fARB(TextureCoef1Location, c1.red, c1.green, c1.blue, c1.alpha ));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));

    ShaderProg->End();
  }

  void GraphicsEngine::QRP_GLSL_2TexMod(int x, int y, int width, int height,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1)
  {
    if (!m_Sl2TextureMod.IsValid())
        InitSl2TextureMod();

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;
    {
      ShaderProg = m_Sl2TextureMod;
    }

    QRP_Compute_Texture_Coord(width, height, DeviceTexture0, texxform0);
    QRP_Compute_Texture_Coord(width, height, DeviceTexture1, texxform1);

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation0 = ShaderProg->GetUniformLocationARB("TextureObject0");
    int TextureObjectLocation1 = ShaderProg->GetUniformLocationARB("TextureObject1");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int TextureCoord1Location = ShaderProg->GetAttributeLocation("MyTextureCoord1");

    int TextureCoef0Location = ShaderProg->GetUniformLocationARB("color0");
    int TextureCoef1Location = ShaderProg->GetUniformLocationARB("color1");


    SetTexture(GL_TEXTURE0, DeviceTexture0);
    SetTexture(GL_TEXTURE1, DeviceTexture1);

    CHECKGL(glUniform1iARB(TextureObjectLocation0, 0));
    CHECKGL(glUniform1iARB(TextureObjectLocation1, 1));

    CHECKGL(glUniform4fARB(TextureCoef0Location, color0.red, color0.green, color0.blue, color0.alpha ));
    CHECKGL(glUniform4fARB(TextureCoef1Location, color1.red, color1.green, color1.blue, color1.alpha ));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));

    ShaderProg->End();
  }

  void GraphicsEngine::QRP_GLSL_4Tex(int x, int y, int width, int height,
                                       ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                                       ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1,
                                       ObjectPtr<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm &texxform2, const Color &color2,
                                       ObjectPtr<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm &texxform3, const Color &color3)
  {
    if (!m_Sl4TextureAdd.IsValid())
        InitSl4TextureAdd();

    QRP_Compute_Texture_Coord(width, height, DeviceTexture0, texxform0);
    QRP_Compute_Texture_Coord(width, height, DeviceTexture1, texxform1);
    QRP_Compute_Texture_Coord(width, height, DeviceTexture2, texxform2);
    QRP_Compute_Texture_Coord(width, height, DeviceTexture3, texxform3);

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;
    ShaderProg = m_Sl4TextureAdd;

    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 1.0f, texxform1.u0, texxform1.v0, 0, 1.0f, texxform2.u0, texxform2.v0, 0, 1.0f, texxform3.u0, texxform3.v0, 0, 1.0f,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 1.0f, texxform1.u0, texxform1.v1, 0, 1.0f, texxform2.u0, texxform2.v1, 0, 1.0f, texxform3.u0, texxform3.v1, 0, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 1.0f, texxform1.u1, texxform1.v1, 0, 1.0f, texxform2.u1, texxform2.v1, 0, 1.0f, texxform3.u1, texxform3.v1, 0, 1.0f,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 1.0f, texxform1.u1, texxform1.v0, 0, 1.0f, texxform2.u1, texxform2.v0, 0, 1.0f, texxform3.u1, texxform3.v0, 0, 1.0f,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation0 = ShaderProg->GetUniformLocationARB("TextureObject0");
    int TextureObjectLocation1 = ShaderProg->GetUniformLocationARB("TextureObject1");
    int TextureObjectLocation2 = ShaderProg->GetUniformLocationARB("TextureObject2");
    int TextureObjectLocation3 = ShaderProg->GetUniformLocationARB("TextureObject3");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int TextureCoord1Location = ShaderProg->GetAttributeLocation("MyTextureCoord1");
    int TextureCoord2Location = ShaderProg->GetAttributeLocation("MyTextureCoord2");
    int TextureCoord3Location = ShaderProg->GetAttributeLocation("MyTextureCoord3");

    int TextureCoef0Location = ShaderProg->GetUniformLocationARB("color0");
    int TextureCoef1Location = ShaderProg->GetUniformLocationARB("color1");
    int TextureCoef2Location = ShaderProg->GetUniformLocationARB("color2");
    int TextureCoef3Location = ShaderProg->GetUniformLocationARB("color3");

    SetTexture(GL_TEXTURE0, DeviceTexture0);
    SetTexture(GL_TEXTURE1, DeviceTexture1);
    SetTexture(GL_TEXTURE2, DeviceTexture2);
    SetTexture(GL_TEXTURE3, DeviceTexture3);

    CHECKGL(glUniform1iARB(TextureObjectLocation0, 0));
    CHECKGL(glUniform1iARB(TextureObjectLocation1, 1));
    CHECKGL(glUniform1iARB(TextureObjectLocation2, 2));
    CHECKGL(glUniform1iARB(TextureObjectLocation3, 3));

    CHECKGL(glUniform4fARB(TextureCoef0Location, color0.red, color0.green, color0.blue, color0.alpha ));
    CHECKGL(glUniform4fARB(TextureCoef1Location, color1.red, color1.green, color1.blue, color1.alpha ));
    CHECKGL(glUniform4fARB(TextureCoef2Location, color2.red, color2.green, color2.blue, color2.alpha ));
    CHECKGL(glUniform4fARB(TextureCoef3Location, color3.red, color3.green, color3.blue, color3.alpha ));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 4));
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 8));
    }

    if (TextureCoord2Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord2Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord2Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 12));
    }

    if (TextureCoord3Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord3Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord3Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 16));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (TextureCoord1Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));

    if (TextureCoord2Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord2Location));

    if (TextureCoord3Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord3Location));

    ShaderProg->End();
  }
///////////////////////////////////////////
  void GraphicsEngine::QRP_GLSL_Triangle(int x0, int y0,
      int x1, int y1,
      int x2, int y2,
      Color c0)
  {
    QRP_GLSL_Triangle(x0, y0, x1, y1, x2, y2, c0, c0, c0);
  }

  void GraphicsEngine::QRP_GLSL_Triangle(int x0, int y0,
      int x1, int y1,
      int x2, int y2,
      Color c0, Color c1, Color c2)
  {
    if (!m_SlColor.IsValid())
      InitSlColorShader();

    float VtxBuffer[] =
    {
      static_cast<float>(x0), static_cast<float>(y0), 0.0f, 1.0f, c0.red, c0.green, c0.blue, c0.alpha,
      static_cast<float>(x1), static_cast<float>(y1), 0.0f, 1.0f, c1.red, c1.green, c1.blue, c1.alpha,
      static_cast<float>(x2), static_cast<float>(y2), 0.0f, 1.0f, c2.red, c2.green, c2.blue, c2.alpha,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    m_SlColor->Begin();

    int VertexLocation = m_SlColor->GetAttributeLocation("AVertex");
    int VertexColorLocation = m_SlColor->GetAttributeLocation("VertexColor");

    int     VPMatrixLocation = m_SlColor->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    m_SlColor->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));

    CHECKGL(glDrawArrays(GL_TRIANGLES, 0, 3));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));
    m_SlColor->End();

    m_triangle_stats++;
  }

//////////////////////
// DRAW LINES       //
//////////////////////
  void GraphicsEngine::QRP_GLSL_Line(int x0, int y0,
                                       int x1, int y1, Color c0)
  {
    QRP_GLSL_Line(x0, y0, x1, y1, c0, c0);
  }

  void GraphicsEngine::QRP_GLSL_Line(int x0, int y0,
                                       int x1, int y1, Color c0, Color c1)
  {
    if (!m_SlColor.IsValid())
      InitSlColorShader();

    float VtxBuffer[] =
    {
      static_cast<float>(x0), static_cast<float>(y0), 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c0.red, c0.green, c0.blue, c0.alpha,
      static_cast<float>(x1), static_cast<float>(y1), 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c1.red, c1.green, c1.blue, c1.alpha,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg = m_SlColor;


    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");


    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_LINES, 0, 2));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();

    m_line_stats++;
  }

  void GraphicsEngine::QRP_GLSL_QuadWireframe(int x0, int y0, int width, int height,
      Color c0,
      Color c1,
      Color c2,
      Color c3)
  {
    if (!m_SlColor.IsValid())
      InitSlColorShader();

    float fx0 = x0, fy0 = y0;
    float VtxBuffer[] =
    {
      fx0, fy0,                             0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c0.red, c0.green, c0.blue, c0.alpha,
      fx0, fy0 + height - 1,                0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c1.red, c1.green, c1.blue, c1.alpha,
      fx0 + width - 1, fy0 + height - 1,    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c2.red, c2.green, c2.blue, c2.alpha,
      fx0 + width - 1, fy0,                 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c3.red, c3.green, c3.blue, c3.alpha,
      fx0, fy0,                             0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c0.red, c0.green, c0.blue, c0.alpha,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg = m_SlColor;


    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");


    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_LINE_STRIP, 0, 5));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();

    m_line_stats++;
  }

  void GraphicsEngine::QRP_GLSL_Power(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color &c0, Vector4 exponent)
  {
    if (!_component_exponentiation_prog.IsValid())
        InitSLPower();

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, device_texture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;

    if (!device_texture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      return;
    }

    ShaderProg = _component_exponentiation_prog;

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation   = ShaderProg->GetUniformLocationARB("TextureObject0");
    int Color0Location          = ShaderProg->GetUniformLocationARB("color0");
    int ExponentLocation        = ShaderProg->GetUniformLocationARB("exponent");
    int VertexLocation          = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location   = ShaderProg->GetAttributeLocation("MyTextureCoord0");


    SetTexture(GL_TEXTURE0, device_texture);
    
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    CHECKGL(glUniform4fARB(ExponentLocation, exponent.x, exponent.y, exponent.z, exponent.w ));

    CHECKGL(glUniform4fARB(Color0Location, c0.red, c0.green, c0.blue, c0.alpha));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix =  GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    ShaderProg->End();
  }

  void GraphicsEngine::QRP_GLSL_AlphaReplicate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color &c0)
  {
    if (!_alpha_replicate_prog.IsValid())
        InitSLAlphaReplicate();

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, device_texture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;

    if (!device_texture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      return;
    }

    ShaderProg = _alpha_replicate_prog;

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int Color0Location        = ShaderProg->GetUniformLocationARB("color0");
    int VertexLocation        = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");


    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    CHECKGL(glUniform4fARB(Color0Location, c0.red, c0.green, c0.blue, c0.alpha));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    ShaderProg->End();
  }

  void GraphicsEngine::InitSLHorizontalGaussFilter()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString = NUX_VERTEX_SHADER_HEADER
        "uniform mat4 ViewProjectionMatrix;  \n\
        attribute vec4 AVertex;             \n\
        attribute vec4 MyTextureCoord0;     \n\
        varying vec4 varyTexCoord0;         \n\
        void main()                         \n\
        {                                   \n\
          varyTexCoord0 = MyTextureCoord0;  \n\
          gl_Position =  ViewProjectionMatrix * (AVertex);  \n\
        }";


    const char* PSString = NUX_FRAGMENT_SHADER_HEADER
        "varying vec4 varyTexCoord0;                                   \n\
        uniform sampler2D TextureObject0;                             \n\
        uniform vec2 TextureSize0;                                    \n\
        vec4 SampleTexture(sampler2D TexObject, vec2 TexCoord)        \n\
        {                                                             \n\
          return texture2D(TexObject, TexCoord.st);                   \n\
        }                                                             \n\
        #define NUM_SAMPLES 7                                         \n\
        uniform float W[NUM_SAMPLES];                                 \n\
        void main()                                                   \n\
        {                                                             \n\
          vec4 sum   = vec4(0.0, 0.0, 0.0, 0.0);                     \n\
          vec2 delta = vec2(1.0 / TextureSize0.x, 0.0);              \n\
          vec2 texCoord = vec2(varyTexCoord0.s, varyTexCoord0.t);    \n\
          texCoord.x -= float((NUM_SAMPLES - 1) / 2) / TextureSize0.x; \n\
          texCoord.y += 0.0 / TextureSize0.y;                         \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[0];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[1];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[2];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[3];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[4];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[5];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[6];     \n\
          texCoord += delta;                                          \n\
          gl_FragColor = vec4(sum.x, sum.y, sum.z, sum.w);           \n\
        }";

    _horizontal_gauss_filter_prog = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString));

    _horizontal_gauss_filter_prog->ClearShaderObjects();
    _horizontal_gauss_filter_prog->AddShaderObject(VS);
    _horizontal_gauss_filter_prog->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_horizontal_gauss_filter_prog->GetOpenGLID(), 0, "AVertex"));
    _horizontal_gauss_filter_prog->Link();

  }

  void GraphicsEngine::QRP_GLSL_HorizontalGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color & /* c0 */, float sigma)
  {
    if (!_horizontal_gauss_filter_prog.IsValid())
        InitSLHorizontalGaussFilter();

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, device_texture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;

    if (!device_texture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      return;
    }

    ShaderProg = _horizontal_gauss_filter_prog;

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int WeightsLocation       = ShaderProg->GetUniformLocationARB("W");
    int TextureSizeLocation   = ShaderProg->GetUniformLocationARB("TextureSize0");
    int VertexLocation        = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");

    SetTexture(GL_TEXTURE0, device_texture);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));
    sigma = Clamp <float> (sigma, 0.1f, 9.0f);
    // Set the Gaussian weights
    {
      float *W;
      GaussianWeights(&W, sigma, 7);
      CHECKGL(glUniform1fv(WeightsLocation, 7, W));
      delete[] W;
    }

    CHECKGL(glUniform2fARB(TextureSizeLocation, width, height));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    ShaderProg->End();
  }

  void GraphicsEngine::InitSLVerticalGaussFilter()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString = NUX_VERTEX_SHADER_HEADER
        "uniform mat4 ViewProjectionMatrix;  \n\
        attribute vec4 AVertex;             \n\
        attribute vec4 MyTextureCoord0;     \n\
        varying vec4 varyTexCoord0;         \n\
        void main()                         \n\
        {                                   \n\
          varyTexCoord0 = MyTextureCoord0;  \n\
          gl_Position =  ViewProjectionMatrix * (AVertex);  \n\
        }";


    const char* PSString = NUX_FRAGMENT_SHADER_HEADER
        "varying vec4 varyTexCoord0;                                   \n\
        uniform sampler2D TextureObject0;                             \n\
        uniform vec2 TextureSize0;                                    \n\
        vec4 SampleTexture(sampler2D TexObject, vec2 TexCoord)       \n\
        {                                                             \n\
          return texture2D(TexObject, TexCoord.st);                  \n\
        }                                                             \n\
        #define NUM_SAMPLES 7                                         \n\
        uniform float W [NUM_SAMPLES];                                \n\
        void main()                                                  \n\
        {                                                             \n\
          vec4 sum   = vec4(0.0, 0.0, 0.0, 0.0);                     \n\
          vec2 delta = vec2(0.0, 1.0 / TextureSize0.y);              \n\
          vec2 texCoord = vec2(varyTexCoord0.s, varyTexCoord0.t);    \n\
          texCoord.x += 0.0 / TextureSize0.x;                         \n\
          texCoord.y -= float((NUM_SAMPLES - 1) / 2) / TextureSize0.y;     \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[0];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[1];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[2];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[3];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[4];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[5];     \n\
          texCoord += delta;                                          \n\
          sum += SampleTexture(TextureObject0, texCoord) * W[6];     \n\
          texCoord += delta;                                          \n\
          gl_FragColor = vec4(sum.x, sum.y, sum.z, sum.w);           \n\
        }";

    _vertical_gauss_filter_prog = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString));

    _vertical_gauss_filter_prog->ClearShaderObjects();
    _vertical_gauss_filter_prog->AddShaderObject(VS);
    _vertical_gauss_filter_prog->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_vertical_gauss_filter_prog->GetOpenGLID(), 0, "AVertex"));
    _vertical_gauss_filter_prog->Link();
  }

  void GraphicsEngine::QRP_GLSL_VerticalGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color & /* c0 */, float sigma)
  {
    if (!_vertical_gauss_filter_prog.IsValid())
        InitSLVerticalGaussFilter();

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, device_texture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;

    if (!device_texture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      return;
    }

    ShaderProg = _vertical_gauss_filter_prog;

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int WeightsLocation       = ShaderProg->GetUniformLocationARB("W");
    int TextureSizeLocation   = ShaderProg->GetUniformLocationARB("TextureSize0");
    int VertexLocation        = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    sigma = Clamp <float> (sigma, 0.1f, 9.0f);
    // Set the Gaussian weights
    {
      float *W;
      GaussianWeights(&W, sigma, 7);
      CHECKGL(glUniform1fv(WeightsLocation, 7, W));
      delete[] W;
    }

    CHECKGL(glUniform2fARB(TextureSizeLocation, width, height));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    ShaderProg->End();
  }

  void GraphicsEngine::QRP_GLSL_HorizontalHQGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color & /* c0 */, float sigma)
  {
    int k = Clamp<float>(sigma, NUX_MIN_GAUSSIAN_SIGMA, NUX_MAX_GAUSSIAN_SIGMA);

    if (_horizontal_hq_gauss_filter_prog[k-1].IsValid() == false)
    {
      InitSLHorizontalHQGaussFilter(k);
    }

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, device_texture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;

    if (!device_texture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      return;
    }

    ShaderProg = _horizontal_hq_gauss_filter_prog[k-1];

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int WeightsLocation       = ShaderProg->GetUniformLocationARB("W");
    int TextureSizeLocation   = ShaderProg->GetUniformLocationARB("TextureSize0");
    int VertexLocation        = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");

    SetTexture(GL_TEXTURE0, device_texture);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    sigma = Clamp <float> (sigma, 0.1f, NUX_MAX_GAUSSIAN_SIGMA);
    // Set the Gaussian weights
    {
      float *W;
      GaussianWeights(&W, sigma, 6*k+1);
      CHECKGL(glUniform1fv(WeightsLocation, 6*k+1, W));
      delete[] W;
    }

    CHECKGL(glUniform2fARB(TextureSizeLocation, width, height));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    ShaderProg->End();
  }

  void GraphicsEngine::QRP_GLSL_VerticalHQGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color & /* c0 */, float sigma)
  {
    int k = Clamp<float>(sigma, NUX_MIN_GAUSSIAN_SIGMA, NUX_MAX_GAUSSIAN_SIGMA);

    if (_vertical_hq_gauss_filter_prog[k-1].IsValid() == false)
    {
      InitSLVerticalHQGaussFilter(k);
    }

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, device_texture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;

    if (!device_texture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      return;
    }

    ShaderProg = _vertical_hq_gauss_filter_prog[k-1];

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int WeightsLocation       = ShaderProg->GetUniformLocationARB("W");
    int TextureSizeLocation   = ShaderProg->GetUniformLocationARB("TextureSize0");
    int VertexLocation        = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");


    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    sigma = Clamp <float> (sigma, 0.1f, NUX_MAX_GAUSSIAN_SIGMA);
    // Set the Gaussian weights
    {
      float *W;
      GaussianWeights(&W, sigma, 6*k+1);
      CHECKGL(glUniform1fv(WeightsLocation, 6*k+1, W));
      delete[] W;
    }

    CHECKGL(glUniform2fARB(TextureSizeLocation, width, height));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    ShaderProg->End();
  }
  
  void GraphicsEngine::QRP_GLSL_HorizontalLSGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color & /* c0 */, float sigma)
  {
    std::vector<float> weights(0);
    std::vector<float> offsets(0);
    
    int num_samples = LinearSampleGaussianWeights(weights, offsets, sigma);

    if (_horizontal_ls_gauss_filter_prog[num_samples-1].IsValid() == false)
    {
      InitSLHorizontalLSGaussFilter(num_samples);
    }

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, device_texture, texxform0);
    float fx = x, fy = y;
    float vtx_buffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0,
    };

    ObjectPtr<IOpenGLShaderProgram> shader_prog;

    if (!device_texture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      return;
    }

    shader_prog = _horizontal_ls_gauss_filter_prog[num_samples-1];

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    shader_prog->Begin();

    int tex_object_location = shader_prog->GetUniformLocationARB("tex_object");
    int taps_location       = shader_prog->GetUniformLocationARB("taps");
    int vertex_location     = shader_prog->GetAttributeLocation("vertex");
    int tex_coord_location  = shader_prog->GetAttributeLocation("tex_coord");

    SetTexture(GL_TEXTURE0, device_texture);
    CHECKGL(glUniform1iARB(tex_object_location, 0));

    std::vector<float> taps;
    for (int i = 0; i < num_samples; ++i)
    {
      taps.push_back(offsets[i] /= width);
      taps.push_back(weights[i]);
    }
    CHECKGL(glUniform2fv(taps_location, taps.size(), &taps[0]));

    int     VPMatrixLocation = shader_prog->GetUniformLocationARB("view_projection_matrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    shader_prog->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(vertex_location));
    CHECKGL(glVertexAttribPointerARB((GLuint) vertex_location, 4, GL_FLOAT, GL_FALSE, 32, vtx_buffer));

    if (tex_coord_location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(tex_coord_location));
      CHECKGL(glVertexAttribPointerARB((GLuint) tex_coord_location, 4, GL_FLOAT, GL_FALSE, 32, vtx_buffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(vertex_location));

    if (tex_coord_location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(tex_coord_location));

    shader_prog->End();
  }

  void GraphicsEngine::QRP_GLSL_VerticalLSGauss(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color & /* c0 */, float sigma)
  {
    std::vector<float> weights(0);
    std::vector<float> offsets(0);
    
    int num_samples = LinearSampleGaussianWeights(weights, offsets, sigma);

    if (_vertical_ls_gauss_filter_prog[num_samples-1].IsValid() == false)
    {
      InitSLVerticalLSGaussFilter(num_samples);
    }

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, device_texture, texxform0);
    float fx = x, fy = y;
    float vtx_buffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0,
    };

    ObjectPtr<IOpenGLShaderProgram> shader_prog;

    if (!device_texture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      return;
    }

    shader_prog = _vertical_ls_gauss_filter_prog[num_samples-1];

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    shader_prog->Begin();

    int tex_object_location     = shader_prog->GetUniformLocationARB("tex_object");
    int taps_location           = shader_prog->GetUniformLocationARB("taps");
    int vertex_location         = shader_prog->GetAttributeLocation("vertex");
    int tex_coord_location      = shader_prog->GetAttributeLocation("tex_coord");

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glUniform1iARB(tex_object_location, 0));

    std::vector<float> taps;
    for (int i = 0; i < num_samples; ++i)
    {
      taps.push_back(offsets[i] /= height);
      taps.push_back(weights[i]);
    }
    CHECKGL(glUniform2fv(taps_location, taps.size(), &taps[0]));

    int     VPMatrixLocation = shader_prog->GetUniformLocationARB("view_projection_matrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    shader_prog->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(vertex_location));
    CHECKGL(glVertexAttribPointerARB((GLuint) vertex_location, 4, GL_FLOAT, GL_FALSE, 32, vtx_buffer));

    if (tex_coord_location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(tex_coord_location));
      CHECKGL(glVertexAttribPointerARB((GLuint) tex_coord_location, 4, GL_FLOAT, GL_FALSE, 32, vtx_buffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(vertex_location));

    if (tex_coord_location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(tex_coord_location));

    shader_prog->End();
  }

  void GraphicsEngine::QRP_GLSL_ColorMatrix(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0,
    const Color &c0,
    Matrix4 color_matrix,
    Vector4 offset)
  {
    if (!_color_matrix_filter_prog.IsValid())
        InitSLColorMatrixFilter();

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, device_texture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;

    if (!device_texture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      return;
    }

    ShaderProg = _color_matrix_filter_prog;

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int Color0Location        = ShaderProg->GetUniformLocationARB("color0");
    int MatrixRow0Location    = ShaderProg->GetUniformLocationARB("CM0");
    int MatrixRow1Location    = ShaderProg->GetUniformLocationARB("CM1");
    int MatrixRow2Location    = ShaderProg->GetUniformLocationARB("CM2");
    int MatrixRow3Location    = ShaderProg->GetUniformLocationARB("CM3");

    int VertexLocation        = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");

    SetTexture(GL_TEXTURE0, device_texture);

    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    CHECKGL(glUniform4fARB(Color0Location, c0.red, c0.green, c0.blue, c0.alpha));

    float v[5];
    v[0] = color_matrix.m[0][0]; v[1] = color_matrix.m[0][1]; v[2] = color_matrix.m[0][2]; v[3] = color_matrix.m[0][3]; v[4] = offset.x;
    CHECKGL(glUniform1fvARB(MatrixRow0Location, 5, v));
    v[0] = color_matrix.m[1][0]; v[1] = color_matrix.m[1][1]; v[2] = color_matrix.m[1][2]; v[3] = color_matrix.m[1][3]; v[4] = offset.y;
    CHECKGL(glUniform1fvARB(MatrixRow1Location, 5, v));
    v[0] = color_matrix.m[2][0]; v[1] = color_matrix.m[2][1]; v[2] = color_matrix.m[2][2]; v[3] = color_matrix.m[2][3]; v[4] = offset.z;
    CHECKGL(glUniform1fvARB(MatrixRow2Location, 5, v));
    v[0] = color_matrix.m[3][0]; v[1] = color_matrix.m[3][1]; v[2] = color_matrix.m[3][2]; v[3] = color_matrix.m[3][3]; v[4] = offset.w;
    CHECKGL(glUniform1fvARB(MatrixRow3Location, 5, v));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    ShaderProg->End();
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GLSL_GetBlurTexture(
    int x, int y,
    int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0,
    float sigma, int num_pass)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    num_pass = Clamp<int> (num_pass, 1, 50);

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();

    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, buffer_width, buffer_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QRP_GLSL_1Tex(x, y, quad_width, quad_height, device_texture, texxform, color::White);

    TexCoordXForm texxform1;
    for (int i = 0; i < num_pass; i++)
    {
      SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      if (_offscreen_color_rt0.IsValid())
        QRP_GLSL_HorizontalGauss(0, 0, buffer_width, buffer_height, _offscreen_color_rt0, texxform1, c0, sigma);

      SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      if (_offscreen_color_rt1.IsValid())
        QRP_GLSL_VerticalGauss(0, 0, buffer_width, buffer_height, _offscreen_color_rt1, texxform1, c0, sigma);
    }

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }

    return _offscreen_color_rt0;
  }
  
  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GLSL_GetLSBlurTexture(
    int x, int y,
    int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0,
    float sigma, int num_pass)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    num_pass = Clamp<int> (num_pass, 1, 50);

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, buffer_width, buffer_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QRP_GLSL_1Tex(x, y, quad_width, quad_height, device_texture, texxform, color::White);

    TexCoordXForm texxform1;
    for (int i = 0; i < num_pass; i++)
    {
      SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_GLSL_HorizontalLSGauss(0, 0, buffer_width, buffer_height, _offscreen_color_rt0, texxform1, c0, sigma);

      SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_GLSL_VerticalLSGauss(0, 0, buffer_width, buffer_height, _offscreen_color_rt1, texxform1, c0, sigma);
    }

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }

    return _offscreen_color_rt0;
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GLSL_GetPower(
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0, const Vector4 &exponent)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_GLSL_1Tex(0, 0, quad_width, quad_height, device_texture, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_GLSL_Power(0, 0, quad_width, quad_height, _offscreen_color_rt0, texxform, c0, exponent);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }

    return _offscreen_color_rt1;
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GLSL_GetAlphaTexture(
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color & c0)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_GLSL_1Tex(0, 0, quad_width, quad_height, device_texture, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_GLSL_AlphaReplicate(0, 0, quad_width, quad_height, _offscreen_color_rt0, texxform, c0);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }

    return _offscreen_color_rt1;
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GLSL_GetColorMatrixTexture(
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color & c0, Matrix4 color_matrix, Vector4 offset)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_GLSL_1Tex(0, 0, quad_width, quad_height, device_texture, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, quad_width, quad_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_GLSL_ColorMatrix(0, 0, quad_width, quad_height, _offscreen_color_rt0, texxform, c0, color_matrix, offset);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }

    return _offscreen_color_rt1;
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GLSL_GetLQBlur(
    int /* x */, int /* y */,
    int /* buffer_width */, int /* buffer_height */,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& /* c0 */)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width  = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width  = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    texxform.mag_filter = TEXFILTER_LINEAR;
    texxform.min_filter = TEXFILTER_LINEAR;

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width/2, quad_height/2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_GLSL_1Tex(0, 0, quad_width / 2, quad_height / 2, device_texture, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, quad_width/4, quad_height/4);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_GLSL_1Tex(0, 0, quad_width / 4, quad_height / 4, _offscreen_color_rt0, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt2, _offscreen_depth_rt2, quad_width/8, quad_height/8);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QRP_GLSL_1Tex(0, 0, quad_width / 8, quad_height / 8, _offscreen_color_rt1, texxform, color::White);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt3, _offscreen_depth_rt3, quad_width, quad_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    TexCoordXForm texxform0;
    TexCoordXForm texxform1;
    TexCoordXForm texxform2;
    TexCoordXForm texxform3;

    texxform0.flip_v_coord = true;
    texxform2.flip_v_coord = true;
    QRP_GLSL_4Tex(0, 0, quad_width, quad_height,
      device_texture, texxform0, Color(0.25, 0.25, 0.25, 0.25),
      _offscreen_color_rt0, texxform1, Color(0.25, 0.25, 0.25, 0.25),
      _offscreen_color_rt1, texxform2, Color(0.25, 0.25, 0.25, 0.25),
      _offscreen_color_rt2, texxform3, Color(0.25, 0.25, 0.25, 0.25));

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    return _offscreen_color_rt3;
  }


  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GLSL_GetHQBlur(
    int x, int y,
    int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0,
    float sigma, int num_pass)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    num_pass = Clamp<int> (num_pass, 1, 50);

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    _offscreen_color_rt0->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt0->SetFiltering(GL_NEAREST, GL_NEAREST);
    _offscreen_color_rt1->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    _offscreen_color_rt1->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, buffer_width, buffer_height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QRP_GLSL_1Tex(x, y, quad_width, quad_height, device_texture, texxform, color::White);

    TexCoordXForm texxform1;
    for (int i = 0; i < num_pass; i++)
    {
      SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt1, _offscreen_depth_rt1, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_GLSL_HorizontalHQGauss(0, 0, buffer_width, buffer_height, _offscreen_color_rt0, texxform1, c0, sigma);

      SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_GLSL_VerticalHQGauss(0, 0, buffer_width, buffer_height, _offscreen_color_rt1, texxform1, c0, sigma);
    }

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }

    return _offscreen_color_rt0;
  }

  void GraphicsEngine::QRP_GLSL_GetHQBlurFx(
    int x, int y,
    int buffer_width, int buffer_height,
    FxStructure *fx_structure, TexCoordXForm &texxform,
    const Color& c0, float sigma, int num_pass)
  {
    int quad_width = fx_structure->src_texture->GetWidth();
    int quad_height = fx_structure->src_texture->GetHeight();

    num_pass = Clamp<int> (num_pass, 1, 50);

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    fx_structure->src_texture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    fx_structure->src_texture->SetFiltering(GL_NEAREST, GL_NEAREST);
    fx_structure->dst_texture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    fx_structure->dst_texture->SetFiltering(GL_NEAREST, GL_NEAREST);
    fx_structure->temp_texture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    fx_structure->temp_texture->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, fx_structure->dst_texture, _offscreen_depth_rt0, buffer_width, buffer_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QRP_GLSL_1Tex(x, y, quad_width, quad_height, fx_structure->src_texture, texxform, color::White);

    TexCoordXForm texxform1;
    for (int i = 0; i < num_pass; i++)
    {
      SetFrameBufferHelper(_offscreen_fbo, fx_structure->temp_texture, _offscreen_depth_rt1, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_GLSL_HorizontalGauss(0, 0, buffer_width, buffer_height, fx_structure->dst_texture, texxform1, c0, sigma);

      SetFrameBufferHelper(_offscreen_fbo, fx_structure->dst_texture, _offscreen_depth_rt0, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_GLSL_VerticalGauss(0, 0, buffer_width, buffer_height, fx_structure->temp_texture, texxform1, c0, sigma);
    }

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
  }
  
  void GraphicsEngine::QRP_GLSL_GetLSBlurFx(
    int x, int y,
    int buffer_width, int buffer_height,
    FxStructure *fx_structure, TexCoordXForm &texxform,
    const Color& c0, float sigma, int num_pass)
  {
    int quad_width = fx_structure->src_texture->GetWidth();
    int quad_height = fx_structure->src_texture->GetHeight();

    num_pass = Clamp<int> (num_pass, 1, 50);

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    fx_structure->src_texture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    fx_structure->src_texture->SetFiltering(GL_NEAREST, GL_NEAREST);
    fx_structure->dst_texture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    fx_structure->dst_texture->SetFiltering(GL_NEAREST, GL_NEAREST);
    fx_structure->temp_texture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    fx_structure->temp_texture->SetFiltering(GL_NEAREST, GL_NEAREST);

    SetFrameBufferHelper(_offscreen_fbo, fx_structure->dst_texture, _offscreen_depth_rt0, buffer_width, buffer_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QRP_GLSL_1Tex(x, y, quad_width, quad_height, fx_structure->src_texture, texxform, color::White);

    TexCoordXForm texxform1;
    for (int i = 0; i < num_pass; i++)
    {
      SetFrameBufferHelper(_offscreen_fbo, fx_structure->temp_texture, _offscreen_depth_rt1, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_GLSL_HorizontalLSGauss(0, 0, buffer_width, buffer_height, fx_structure->dst_texture, texxform1, c0, sigma);

      SetFrameBufferHelper(_offscreen_fbo, fx_structure->dst_texture, _offscreen_depth_rt0, buffer_width, buffer_height);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      QRP_GLSL_VerticalLSGauss(0, 0, buffer_width, buffer_height, fx_structure->temp_texture, texxform1, c0, sigma);
    }

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GLSL_GetDisturbedTexture(
      int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm &texxform0, const Color& c0,
      ObjectPtr<IOpenGLBaseTexture> src_device_texture, TexCoordXForm &texxform1, const Color& c1)
  {
    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QRP_GLSL_DisturbedTexture(x, y, width, height,
      distorsion_texture, texxform0, c0,
      src_device_texture, texxform1, c1);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }

    return _offscreen_color_rt0;
  }

  void GraphicsEngine::QRP_GLSL_GetDisturbedTextureFx(
    int x, int y, int width, int height,
    ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm &texxform0, const Color& c0,
    FxStructure *fx_structure, TexCoordXForm &texxform1, const Color& c1)
  {
    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    SetFrameBufferHelper(_offscreen_fbo, fx_structure->dst_texture, _offscreen_depth_rt0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QRP_GLSL_DisturbedTexture(x, y, width, height,
      distorsion_texture, texxform0, c0,
      fx_structure->src_texture, texxform1, c1);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
  }

  void GraphicsEngine::InitSlPixelateShader()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString =  
                      NUX_VERTEX_SHADER_HEADER
                      "attribute vec4 AVertex;                                 \n\
                      attribute vec4 MyTextureCoord0;                         \n\
                      attribute vec4 VertexColor;                             \n\
                      uniform mat4 ViewProjectionMatrix;                      \n\
                      varying vec4 varyTexCoord0;                             \n\
                      varying vec4 varyVertexColor;                           \n\
                      void main()                                             \n\
                      {                                                       \n\
                      gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
                      varyTexCoord0 = MyTextureCoord0;                        \n\
                      varyVertexColor = VertexColor;                          \n\
                      }";

    const char* PSString =  
                    NUX_FRAGMENT_SHADER_HEADER
                      "varying vec4 varyTexCoord0;                                 \n\
                      varying vec4 varyVertexColor;                               \n\
                      uniform vec4 pixel_size;                                    \n\
                      uniform vec4 pixel_size_inv;                                \n\
                      uniform sampler2D TextureObject0;                           \n\
                      vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
                      {                                                           \n\
                        return texture2D(TexObject, TexCoord.st);                 \n\
                      }                                                           \n\
                      void main()                                                 \n\
                      {                                                           \n\
                        vec4 tex_coord = floor(varyTexCoord0 * pixel_size_inv) * pixel_size;          \n\
                        vec4 v = SampleTexture(TextureObject0, tex_coord);        \n\
                        gl_FragColor = v*varyVertexColor;                         \n\
                      }";

    // Textured 2D Primitive Shader
    m_SLPixelate = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString), "#define SAMPLERTEX2D");

    m_SLPixelate->ClearShaderObjects();
    m_SLPixelate->AddShaderObject(VS);
    m_SLPixelate->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(m_SLPixelate->GetOpenGLID(), 0, "AVertex"));
    m_SLPixelate->Link();
  }

  void GraphicsEngine::QRP_GLSL_Pixelate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color0, int pixel_size)
  {
    if (!m_SLPixelate.IsValid())
        InitSlPixelateShader();

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, DeviceTexture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
    };

    float tex_width = DeviceTexture->GetWidth();
    float tex_height = DeviceTexture->GetHeight();

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;
    ShaderProg = m_SLPixelate;

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int PixelSizeLocation = ShaderProg->GetUniformLocationARB("pixel_size");
    int PixelSizeInvLocation = ShaderProg->GetUniformLocationARB("pixel_size_inv");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");

    SetTexture(GL_TEXTURE0, DeviceTexture);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    ShaderProg->SetUniform4f((GLint) PixelSizeLocation, (float)pixel_size / (float)tex_width, (float)pixel_size / (float)tex_height, 1.0f, 1.0f);
    ShaderProg->SetUniform4f((GLint) PixelSizeInvLocation, (float)tex_width / (float)pixel_size, (float)tex_height / (float)pixel_size, 1.0f, 1.0f);

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GLSL_GetPixelBlocks(
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform0, const Color& /* c0 */, int pixel_size)
  {
    int quad_width = device_texture->GetWidth();
    int quad_height = device_texture->GetHeight();

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    SetFrameBufferHelper(_offscreen_fbo, _offscreen_color_rt0, _offscreen_depth_rt0, quad_width, quad_height);
    CHECKGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    QRP_GLSL_Pixelate(0, 0, quad_width, quad_height, device_texture, texxform0, color::White, pixel_size);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    return _offscreen_color_rt0;
  }

  void GraphicsEngine::QRP_GLSL_GetCopyTexture(
    int width, int height,
    ObjectPtr<IOpenGLBaseTexture>& dst_device_texture,
    ObjectPtr<IOpenGLBaseTexture>& src_device_texture,
    TexCoordXForm &texxform0, const Color& c0)
  {
    if (src_device_texture.IsValid() == false)
    {
      return;
    }

    ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    int previous_width = 0;
    int previous_height = 0;
    
    if (prevFBO.IsValid())
    {
      previous_width = prevFBO->GetWidth();
      previous_height = prevFBO->GetHeight();
    }
    else
    {
      previous_width = _graphics_display.GetWindowWidth();
      previous_height = _graphics_display.GetWindowHeight();
    }

    if ((dst_device_texture.IsValid() == false) ||
      (dst_device_texture->GetWidth() != width) ||
      (dst_device_texture->GetHeight() != height) ||
      (dst_device_texture->GetPixelFormat() != src_device_texture->GetPixelFormat()))
    {
      dst_device_texture = _graphics_display.GetGpuDevice()->CreateTexture(width, height, 1, src_device_texture->GetPixelFormat());
    }

    CHECKGL(glClearColor(0, 0, 0, 0));
    SetFrameBufferHelper(_offscreen_fbo, dst_device_texture, _offscreen_depth_rt0, width, height);
    CHECKGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    QRP_GLSL_1Tex(0, 0, width, height, src_device_texture, texxform0, c0);

    _offscreen_fbo->Deactivate();

    if (prevFBO.IsValid())
    {
      prevFBO->Activate(true);
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
    else
    {
      SetViewport(0, 0, previous_width, previous_height);
      SetOrthographicProjectionMatrix(previous_width, previous_height);
    }
  }

  void GraphicsEngine::InitSlTexturePremultiplyShader()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();
    std::string VSString;
    std::string PSString;

    VSString =  
      NUX_VERTEX_SHADER_HEADER
      "attribute vec4 i_vertex;                               \n\
      attribute vec4 i_texture_coord0;                        \n\
      attribute vec4 i_vertex_color;                          \n\
      uniform mat4 ViewProjectionMatrix;                      \n\
      varying vec4 varyTexCoord0;                             \n\
      varying vec4 varyVertexColor;                           \n\
      void main()                                             \n\
      {                                                       \n\
      gl_Position =  ViewProjectionMatrix * (i_vertex);       \n\
      varyTexCoord0 = i_texture_coord0;                       \n\
      varyVertexColor = i_vertex_color;                       \n\
      }";

    PSString =  
      NUX_FRAGMENT_SHADER_HEADER
      "varying vec4 varyTexCoord0;                                \n\
      varying vec4 varyVertexColor;                               \n\
      uniform sampler2D TextureObject0;                           \n\
      void main()                                                 \n\
      {                                                           \n\
        vec4 tex = texture2D(TextureObject0, varyTexCoord0.st);   \n\
        gl_FragColor = vec4(tex.r * tex.a, tex.g * tex.a, tex.b * tex.a, tex.a) * varyVertexColor;    \n\
      }";

    // Textured 2D Primitive Shader
    m_SlTexturePremultiplyModColor = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(VSString.c_str());
    PS->SetShaderCode(PSString.c_str());

    m_SlTexturePremultiplyModColor->ClearShaderObjects();
    m_SlTexturePremultiplyModColor->AddShaderObject(VS);
    m_SlTexturePremultiplyModColor->AddShaderObject(PS);
    //CHECKGL(glBindAttribLocation(m_SlTexturePremultiplyModColor->GetOpenGLID(), 0, "i_vertex"));
    m_SlTexturePremultiplyModColor->Link();
  }

  void GraphicsEngine::QRP_GLSL_1TexPremultiply(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color0)
  {
    if (DeviceTexture.IsNull() || (!DeviceTexture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType)))
    {
      return;
    }

    if (!m_SlTexturePremultiplyModColor.IsValid())
      InitSlTexturePremultiplyShader();

    m_quad_tex_stats++;

    QRP_Compute_Texture_Coord(width, height, DeviceTexture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg = m_SlTexturePremultiplyModColor;

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int VertexLocation = ShaderProg->GetAttributeLocation("i_vertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("i_texture_coord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation("i_vertex_color");

    SetTexture(GL_TEXTURE0, DeviceTexture);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();
  }

  void GraphicsEngine::InitSLDesaturation()
  {
    ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

    const char* VSString =  
      NUX_VERTEX_SHADER_HEADER
      "attribute vec4 AVertex;                                \n\
      attribute vec4 MyTextureCoord0;                         \n\
      attribute vec4 VertexColor;                             \n\
      uniform mat4 ViewProjectionMatrix;                      \n\
      varying vec4 varyTexCoord0;                             \n\
      varying vec4 varyVertexColor;                           \n\
      void main()                                             \n\
      {                                                       \n\
        gl_Position =  ViewProjectionMatrix * (AVertex);      \n\
        varyTexCoord0 = MyTextureCoord0;                      \n\
        varyVertexColor = VertexColor;                        \n\
      }";

    const char* PSString =  
      NUX_FRAGMENT_SHADER_HEADER
      "varying vec4 varyTexCoord0;                                \n\
      varying vec4 varyVertexColor;                               \n\
      uniform sampler2D TextureObject0;                           \n\
      uniform float desat_factor;                                 \n\
      uniform vec3 luma;                                          \n\
      void main()                                                 \n\
      {                                                           \n\
        vec4 texel = texture2D(TextureObject0, varyTexCoord0.st); \n\
        vec4 desat_texel = vec4 (luma.r * texel.r + luma.g * texel.g + luma.b * texel.b);  \n\
        vec4 final = (vec4 (1.0, 1.0, 1.0, 1.0) - desat_factor) * desat_texel + desat_factor * texel; \n\
        final.a = texel.a;                                        \n\
        gl_FragColor = final * varyVertexColor;                   \n\
      }";

    // Textured 2D Primitive Shader
    desaturation_prog_ = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(TCHAR_TO_ANSI(PSString));

    desaturation_prog_->ClearShaderObjects();
    desaturation_prog_->AddShaderObject(VS);
    desaturation_prog_->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(desaturation_prog_->GetOpenGLID(), 0, "AVertex"));
    desaturation_prog_->Link();
  }

  void GraphicsEngine::QRP_GLSL_TexDesaturate(int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm& texxform0, const Color& color0, float desaturation_factor)
  {
    if (!desaturation_prog_.IsValid())
      InitSLDesaturation();

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, DeviceTexture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
    };

    ObjectPtr<IOpenGLShaderProgram> ShaderProg;

    if (DeviceTexture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      ShaderProg = desaturation_prog_;
    }

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int luma_location = ShaderProg->GetUniformLocationARB("luma");
    int desat_factor_location = ShaderProg->GetUniformLocationARB("desat_factor");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");

    SetTexture(GL_TEXTURE0, DeviceTexture);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    float luma[3] = {color::LumaRed, color::LumaGreen, color::LumaBlue};
    CHECKGL(glUniform3fv(luma_location, 3, luma));

    CHECKGL(glUniform1f(desat_factor_location, desaturation_factor));

    int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (MVPMatrix.m));

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();    
  }
}

