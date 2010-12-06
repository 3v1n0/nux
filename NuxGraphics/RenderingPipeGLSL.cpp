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

#include "NuxImage/Tga.h"
#include "NuxImage/ImageSurface.h"
#include "NuxMesh/NTextureArchiveManager.h"

#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "RenderingPipe.h"
#include "GraphicsEngine.h"


namespace nux
{

// For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
// other attributes. Otherwise you get a bug on NVidia! Why is that???
// [Update]: it seems that the vertex attributes have to be declared in alphabetic order in the shader. It does not matter if the vertex
// attribute is declared first in the alphabetic order.

// For some other strange reason, on Intel GMA, the order in which attributes are used in the vertex shaders, is the order used to associated them with a
// and attribute location. One has to make sure that the vertex attribute get index 0. So use the vertex attribute first. All of this does not make any sense.
// Need more info from driver developers.

#define USE_ARB 1
  bool USE_ARB_SHADERS = true;

  void GraphicsEngine::InitSlColorShader()
  {
    IntrusiveSP<IOpenGLVertexShader> VS = m_GLWindow.m_DeviceFactory->CreateVertexShader();
    IntrusiveSP<IOpenGLPixelShader> PS = m_GLWindow.m_DeviceFactory->CreatePixelShader();
    NString VSString;
    NString PSString;

    VSString =  TEXT ("#version 110   \n\
                     uniform mat4 ViewProjectionMatrix;                 \n\
                     attribute vec4 AVertex;                            \n\
                     attribute vec4 VertexColor;                        \n\
                     void main()                                        \n\
                     {                                                  \n\
                         gl_Position = ViewProjectionMatrix * AVertex;  \n\
                         gl_FrontColor = VertexColor;                   \n\
                     }");

    VS->SetShaderCode (TCHAR_TO_ANSI (*VSString) );

    PSString =  TEXT ("#version 110                  \n\
                     void main()                    \n\
                     {                              \n\
                         gl_FragColor = gl_Color;   \n\
                     }");
    PS->SetShaderCode (TCHAR_TO_ANSI (*PSString) );

    m_SlColor = m_GLWindow.m_DeviceFactory->CreateShaderProgram();
    m_SlColor->ClearShaderObjects();
    m_SlColor->AddShaderObject (VS);
    m_SlColor->AddShaderObject (PS);
    m_SlColor->Link();
  }

  void GraphicsEngine::InitSlTextureShader()
  {
    IntrusiveSP<IOpenGLVertexShader> VS = m_GLWindow.m_DeviceFactory->CreateVertexShader();
    IntrusiveSP<IOpenGLPixelShader> PS = m_GLWindow.m_DeviceFactory->CreatePixelShader();
    NString VSString;
    NString PSString;

    VSString =  TEXT ("#version 110   \n\
                     attribute vec4 AVertex;                                 \n\
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
                     }");

    PSString =  TEXT ("#version 110                                               \n\
                     #extension GL_ARB_texture_rectangle : enable                \n\
                     varying vec4 varyTexCoord0;                                 \n\
                     varying vec4 varyVertexColor;                               \n\
                     #ifdef SAMPLERTEX2D                                         \n\
                     uniform sampler2D TextureObject0;                           \n\
                     vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
                     {                                                           \n\
                     return texture2D(TexObject, TexCoord.st);                   \n\
                     }                                                           \n\
                     #elif defined SAMPLERTEX2DRECT                              \n\
                     uniform sampler2DRect TextureObject0;                       \n\
                     vec4 SampleTexture(sampler2DRect TexObject, vec4 TexCoord)  \n\
                     {                                                           \n\
                     return texture2DRect(TexObject, TexCoord.st);               \n\
                     }                                                           \n\
                     #endif                                                      \n\
                     void main()                                                 \n\
                     {                                                           \n\
                     vec4 v = SampleTexture(TextureObject0, varyTexCoord0);      \n\
                     gl_FragColor = v*varyVertexColor;                           \n\
                     }");

    // Textured 2D Primitive Shader
    m_SlTextureModColor = m_GLWindow.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode (TCHAR_TO_ANSI (*VSString) );
    PS->SetShaderCode (TCHAR_TO_ANSI (*PSString), TEXT ("#define SAMPLERTEX2D") );

    m_SlTextureModColor->ClearShaderObjects();
    m_SlTextureModColor->AddShaderObject (VS);
    m_SlTextureModColor->AddShaderObject (PS);
    CHECKGL ( glBindAttribLocation (m_SlTextureModColor->GetOpenGLID(), 0, "AVertex") );
    m_SlTextureModColor->Link();
  }

  void GraphicsEngine::InitSlColorModTexMaskAlpha()
  {
    IntrusiveSP<IOpenGLVertexShader> VS = m_GLWindow.m_DeviceFactory->CreateVertexShader();
    IntrusiveSP<IOpenGLPixelShader> PS = m_GLWindow.m_DeviceFactory->CreatePixelShader();
    NString VSString;
    NString PSString;

    VSString =  TEXT ("#version 110   \n\
                     attribute vec4 AVertex;                                 \n\
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
                     }");

    PSString =  TEXT ("#version 110                                                      \n\
                     #extension GL_ARB_texture_rectangle : enable                       \n\
                     varying vec4 varyTexCoord0;                                        \n\
                     varying vec4 varyVertexColor;                                      \n\
                     #ifdef SAMPLERTEX2D                                                \n\
                     uniform sampler2D TextureObject0;                                  \n\
                     vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)             \n\
                     {                                                                  \n\
                     return texture2D(TexObject, TexCoord.st);                          \n\
                     }                                                                  \n\
                     #elif defined SAMPLERTEX2DRECT                                     \n\
                     uniform sampler2DRect TextureObject0;                              \n\
                     vec4 SampleTexture(sampler2DRect TexObject, vec4 TexCoord)         \n\
                     {                                                                  \n\
                     return texture2DRect(TexObject, TexCoord.st);                      \n\
                     }                                                                  \n\
                     #endif                                                             \n\
                     void main()                                                        \n\
                     {                                                                  \n\
                     float alpha = SampleTexture(TextureObject0, varyTexCoord0).w;      \n\
                     gl_FragColor = vec4(varyVertexColor.xyz, alpha*varyVertexColor.a); \n\
                     }");

    m_SlColorModTexMaskAlpha = m_GLWindow.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode (TCHAR_TO_ANSI (*VSString) );
    PS->SetShaderCode (TCHAR_TO_ANSI (*PSString), TEXT ("#define SAMPLERTEX2D") );
    m_SlColorModTexMaskAlpha->ClearShaderObjects();
    m_SlColorModTexMaskAlpha->AddShaderObject (VS);
    m_SlColorModTexMaskAlpha->AddShaderObject (PS);
    CHECKGL ( glBindAttribLocation (m_SlColorModTexMaskAlpha->GetOpenGLID(), 0, "AVertex") );
    CHECKGL ( glBindAttribLocation (m_SlColorModTexMaskAlpha->GetOpenGLID(), 1, "MyTextureCoord0") );
    CHECKGL ( glBindAttribLocation (m_SlColorModTexMaskAlpha->GetOpenGLID(), 2, "VectexColor") );
    m_SlColorModTexMaskAlpha->Link();

    m_SlColorModTexRectMaskAlpha = m_GLWindow.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode (TCHAR_TO_ANSI (*VSString) );
    PS->SetShaderCode (TCHAR_TO_ANSI (*PSString), TEXT ("#define SAMPLERTEX2DRECT") );
    m_SlColorModTexRectMaskAlpha->ClearShaderObjects();
    m_SlColorModTexRectMaskAlpha->AddShaderObject (VS);
    m_SlColorModTexRectMaskAlpha->AddShaderObject (PS);
    CHECKGL ( glBindAttribLocation (m_SlColorModTexRectMaskAlpha->GetOpenGLID(), 0, "AVertex") );
    CHECKGL ( glBindAttribLocation (m_SlColorModTexRectMaskAlpha->GetOpenGLID(), 1, "MyTextureCoord0") );
    CHECKGL ( glBindAttribLocation (m_SlColorModTexRectMaskAlpha->GetOpenGLID(), 2, "VectexColor") );
    m_SlColorModTexRectMaskAlpha->Link();
  }

  void GraphicsEngine::InitSl2TextureAdd()
  {
    IntrusiveSP<IOpenGLVertexShader> VS = m_GLWindow.m_DeviceFactory->CreateVertexShader();
    IntrusiveSP<IOpenGLPixelShader> PS = m_GLWindow.m_DeviceFactory->CreatePixelShader();
    NString VSString;
    NString PSString;

    // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
    // other  attributes. Otherwise you get a bug on NVidia! Why is that???

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    VSString =  TEXT (  "#version 110                                           \n\
                        uniform mat4 ViewProjectionMatrix;                      \n\
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
                        }");

    PSString =  TEXT (  "#version 110                                               \n\
                        #extension GL_ARB_texture_rectangle : enable                \n\
                        varying vec4 varyTexCoord0;                                 \n\
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
                        }");

    m_Sl2TextureAdd = m_GLWindow.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode (TCHAR_TO_ANSI (*VSString) );
    PS->SetShaderCode (TCHAR_TO_ANSI (*PSString), TEXT ("#define SAMPLERTEX2D") );

    m_Sl2TextureAdd->ClearShaderObjects();
    m_Sl2TextureAdd->AddShaderObject (VS);
    m_Sl2TextureAdd->AddShaderObject (PS);
    CHECKGL ( glBindAttribLocation (m_Sl2TextureAdd->GetOpenGLID(), 0, "AVertex") );
    m_Sl2TextureAdd->Link();
  }

  void GraphicsEngine::InitSl2TextureMod()
  {
    IntrusiveSP<IOpenGLVertexShader> VS = m_GLWindow.m_DeviceFactory->CreateVertexShader();
    IntrusiveSP<IOpenGLPixelShader> PS = m_GLWindow.m_DeviceFactory->CreatePixelShader();
    NString VSString;
    NString PSString;

    // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
    // other  attributes. Otherwise you get a bug on NVidia! Why is that???

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    VSString =  TEXT (   "#version 110                                           \n\
                         uniform mat4 ViewProjectionMatrix;                      \n\
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
                         }");

    PSString =  TEXT (   "#version 110                                               \n\
                         #extension GL_ARB_texture_rectangle : enable                \n\
                         varying vec4 varyTexCoord0;                                 \n\
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
                         }");

    m_Sl2TextureMod = m_GLWindow.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode (TCHAR_TO_ANSI (*VSString) );
    PS->SetShaderCode (TCHAR_TO_ANSI (*PSString), TEXT ("#define SAMPLERTEX2D") );

    m_Sl2TextureMod->ClearShaderObjects();
    m_Sl2TextureMod->AddShaderObject (VS);
    m_Sl2TextureMod->AddShaderObject (PS);
    CHECKGL ( glBindAttribLocation (m_Sl2TextureMod->GetOpenGLID(), 0, "AVertex") );
    m_Sl2TextureMod->Link();
  }

  void GraphicsEngine::InitSl4TextureAdd()
  {
    IntrusiveSP<IOpenGLVertexShader> VS = m_GLWindow.m_DeviceFactory->CreateVertexShader();
    IntrusiveSP<IOpenGLPixelShader> PS = m_GLWindow.m_DeviceFactory->CreatePixelShader();
    NString VSString;
    NString PSString;

    // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
    // other  attributes. Otherwise you get a bug on NVidia! Why is that???

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    VSString =  TEXT (   "#version 110                                           \n\
                        uniform mat4 ViewProjectionMatrix;                      \n\
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
                        }");

    PSString =  TEXT (   "#version 110                                               \n\
                        #extension GL_ARB_texture_rectangle : enable                \n\
                        varying vec4 varyTexCoord0;                                 \n\
                        varying vec4 varyTexCoord1;                                 \n\
                        varying vec4 varyTexCoord2;                                 \n\
                        varying vec4 varyTexCoord3;                                 \n\
                        uniform vec4 color0;                                        \n\
                        uniform vec4 color1;                                        \n\
                        uniform vec4 color2;                                        \n\
                        uniform vec4 color3;                                        \n\
                        #ifdef SAMPLERTEX2D                                         \n\
                        uniform sampler2D TextureObject0;                           \n\
                        uniform sampler2D TextureObject1;                           \n\
                        uniform sampler2D TextureObject2;                           \n\
                        uniform sampler2D TextureObject3;                           \n\
                        vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
                        {                                                           \n\
                        return texture2D(TexObject, TexCoord.st);                   \n\
                        }                                                           \n\
                        #elif defined SAMPLERTEX2DRECT                              \n\
                        uniform sampler2DRect TextureObject0;                       \n\
                        uniform sampler2DRect TextureObject1;                       \n\
                        uniform sampler2DRect TextureObject2;                       \n\
                        uniform sampler2DRect TextureObject3;                       \n\
                        vec4 SampleTexture(sampler2DRect TexObject, vec4 TexCoord)  \n\
                        {                                                           \n\
                        return texture2DRect(TexObject, TexCoord.st);               \n\
                        }                                                           \n\
                        #endif                                                      \n\
                        void main()                                                 \n\
                        {                                                           \n\
                        vec4 b0 = color0*SampleTexture(TextureObject0, varyTexCoord0);  \n\
                        vec4 b1 = color1*SampleTexture(TextureObject1, varyTexCoord1);  \n\
                        vec4 b2 = color2*SampleTexture(TextureObject2, varyTexCoord2);  \n\
                        vec4 b3 = color3*SampleTexture(TextureObject3, varyTexCoord3);  \n\
                        gl_FragColor = b0+b1+b2+b3;                                     \n\
                        }");

    //vec4(v.w, v.w, v.w, v.w)

    // Textured 2D Primitive Shader
    m_Sl4TextureAdd = m_GLWindow.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode (TCHAR_TO_ANSI (*VSString) );
    PS->SetShaderCode (TCHAR_TO_ANSI (*PSString), TEXT ("#define SAMPLERTEX2D") );

    m_Sl4TextureAdd->ClearShaderObjects();
    m_Sl4TextureAdd->AddShaderObject (VS);
    m_Sl4TextureAdd->AddShaderObject (PS);
    CHECKGL ( glBindAttribLocation (m_Sl4TextureAdd->GetOpenGLID(), 0, "AVertex") );
    m_Sl4TextureAdd->Link();

//     // Textured Rect Primitive Shader
//     m_4TexBlendRectProg = m_GLWindow.m_DeviceFactory->CreateShaderProgram();
//     VS->SetShaderCode(TCHAR_TO_ANSI(*VSString));
//     PS->SetShaderCode(TCHAR_TO_ANSI(*PSString), TEXT("#define SAMPLERTEX2DRECT"));
//     m_4TexBlendRectProg->ClearShaderObjects();
//     m_4TexBlendRectProg->AddShaderObject(VS);
//     m_4TexBlendRectProg->AddShaderObject(PS);
//     CHECKGL( glBindAttribLocation(m_4TexBlendRectProg->GetOpenGLID(), 0, "AVertex") );
//     m_4TexBlendRectProg->Link();
  }

  void GraphicsEngine::QRP_GLSL_Color (int x, int y, int width, int height, const Color &color)
  {
#if USE_ARB
    QRP_Color (x, y, width, height, color, color, color, color);
    return;
#endif

    QRP_GLSL_Color (x, y, width, height, color, color, color, color);
  }

  void GraphicsEngine::QRP_GLSL_Color (int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3)
  {
#if USE_ARB
    QRP_Color (x, y, width, height, c0, c1, c2, c3);
    return;
#endif

    m_quad_tex_stats++;

    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, c0.R(), c0.G(), c0.B(), c0.A(),
      x,          y + height, 0.0f, 1.0f, c1.R(), c1.G(), c1.B(), c1.A(),
      x + width,  y + height, 0.0f, 1.0f, c2.R(), c2.G(), c2.B(), c2.A(),
      x + width,  y,          0.0f, 1.0f, c3.R(), c3.G(), c3.B(), c3.A(),
    };

    IntrusiveSP<IOpenGLShaderProgram> ShaderProg = m_SlColor;

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    int VertexLocation = ShaderProg->GetAttributeLocation ("AVertex");
    int VertexColorLocation = ShaderProg->GetAttributeLocation ("VertexColor");

    int VPMatrixLocation = ShaderProg->GetUniformLocationARB ("ViewProjectionMatrix");
    ShaderProg->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetModelViewProjectionMatrix().m) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer) );

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (VertexColorLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );

    ShaderProg->End();
  }

  void GraphicsEngine::QRP_GLSL_1Tex (int x, int y, int width, int height, IntrusiveSP<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color0)
  {
#if USE_ARB
    QRP_1Tex (x, y, width, height, DeviceTexture, texxform0, color0);
    return;
#endif

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord (width, height, DeviceTexture, texxform0);
    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0, color0.R(), color0.G(), color0.B(), color0.A(),
      x,          y + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0, color0.R(), color0.G(), color0.B(), color0.A(),
      x + width,  y + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0, color0.R(), color0.G(), color0.B(), color0.A(),
      x + width,  y,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0, color0.R(), color0.G(), color0.B(), color0.A(),
    };

    IntrusiveSP<IOpenGLShaderProgram> ShaderProg;

    if (DeviceTexture->Type().IsDerivedFromType (IOpenGLTexture2D::StaticObjectType) )
    {
      ShaderProg = m_SlTextureModColor;
    }

//     if(DeviceTexture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType) ||
//         DeviceTexture->Type().IsDerivedFromType(IOpenGLAnimatedTexture::StaticObjectType))
//     {
//         ShaderProg = m_TexturedRectProg;
//     }

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB ("TextureObject0");
    int VertexLocation = ShaderProg->GetAttributeLocation ("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation ("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation ("VertexColor");

    SetTexture (GL_TEXTURE0, DeviceTexture);
    CHECKGL ( glUniform1iARB (TextureObjectLocation, 0) );

    int VPMatrixLocation = ShaderProg->GetUniformLocationARB ("ViewProjectionMatrix");
    ShaderProg->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetModelViewProjectionMatrix().m) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4) );
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (VertexColorLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );

    ShaderProg->End();
  }

// Render the texture alpha into RGB and modulated by a color.
  void GraphicsEngine::QRP_GLSL_ColorModTexAlpha (int x, int y, int width, int height,
      IntrusiveSP< IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform, const Color &color)
  {
#if USE_ARB
    QRP_ColorModTexAlpha (x, y, width, height, DeviceTexture, texxform, color);
    return;
#endif

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord (width, height, DeviceTexture, texxform);

    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 0, color.R(), color.G(), color.B(), color.A(),
      x,          y + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 0, color.R(), color.G(), color.B(), color.A(),
      x + width,  y + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 0, color.R(), color.G(), color.B(), color.A(),
      x + width,  y,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 0, color.R(), color.G(), color.B(), color.A(),
    };

    IntrusiveSP<IOpenGLShaderProgram> ShaderProg;
//     if(DeviceTexture->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      ShaderProg = m_SlColorModTexMaskAlpha;
    }

    if (DeviceTexture->Type().IsDerivedFromType (IOpenGLRectangleTexture::StaticObjectType) ||
        DeviceTexture->Type().IsDerivedFromType (IOpenGLAnimatedTexture::StaticObjectType) )
    {
      ShaderProg = m_SlColorModTexRectMaskAlpha;
    }

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB ("TextureObject0");
    int VertexLocation = ShaderProg->GetAttributeLocation ("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation ("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation ("VertexColor");

    SetTexture (GL_TEXTURE0, DeviceTexture);

    if (TextureObjectLocation != -1)
    {
      CHECKGL ( glUniform1iARB (TextureObjectLocation, 0) );
    }

    int VPMatrixLocation = ShaderProg->GetUniformLocationARB ("ViewProjectionMatrix");
    ShaderProg->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetModelViewProjectionMatrix().m) );

    if (VertexLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer) );
    }

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArray (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointer ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4) );
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    if (VertexLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );
    }

    ShaderProg->End();
  }

// Blend 2 textures together
  void GraphicsEngine::QRP_GLSL_2Tex (int x, int y, int width, int height,
                                       IntrusiveSP<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                                       IntrusiveSP<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1)
  {
#if USE_ARB
    QRP_2Tex (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1);
    return;
#endif

    IntrusiveSP<IOpenGLShaderProgram> ShaderProg;
//     if(SrcTexture0->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType) &&
//        SrcTexture1->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      ShaderProg = m_Sl2TextureAdd;
    }

//     if((SrcTexture0->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType) &&
//         SrcTexture1->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType)) /*||
//         (SrcTexture0->Type().IsDerivedFromType(TextureFrameAnimation::StaticObjectType) &&
//         SrcTexture1->Type().IsDerivedFromType(TextureFrameAnimation::StaticObjectType)*/)
//     {
//         bRectangleTexture = true;
//         ShaderProg = m_2TexBlendRectProg;
//     }

    QRP_Compute_Texture_Coord (width, height, DeviceTexture0, texxform0);
    QRP_Compute_Texture_Coord (width, height, DeviceTexture1, texxform1);

    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
      x,          y + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
      x + width,  y + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
      x + width,  y,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    int TextureObjectLocation0 = ShaderProg->GetUniformLocationARB ("TextureObject0");
    int TextureObjectLocation1 = ShaderProg->GetUniformLocationARB ("TextureObject1");
    int VertexLocation = ShaderProg->GetAttributeLocation ("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation ("MyTextureCoord0");
    int TextureCoord1Location = ShaderProg->GetAttributeLocation ("MyTextureCoord1");

    int TextureCoef0Location = ShaderProg->GetUniformLocationARB ("color0");
    int TextureCoef1Location = ShaderProg->GetUniformLocationARB ("color1");


    SetTexture (GL_TEXTURE0, DeviceTexture0);
    SetTexture (GL_TEXTURE1, DeviceTexture1);

    CHECKGL ( glUniform1iARB (TextureObjectLocation0, 0) );
    CHECKGL ( glUniform1iARB (TextureObjectLocation1, 1) );

    CHECKGL ( glUniform4fARB (TextureCoef0Location, color0.R(), color0.G(), color0.B(), color0.A() ) );
    CHECKGL ( glUniform4fARB (TextureCoef1Location, color1.R(), color1.G(), color1.B(), color1.A() ) );

    int VPMatrixLocation = ShaderProg->GetUniformLocationARB ("ViewProjectionMatrix");
    ShaderProg->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetModelViewProjectionMatrix().m) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4) );
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord1Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (TextureCoord1Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord1Location) );

    ShaderProg->End();
  }


  void GraphicsEngine::QRP_GLSL_2TexMod (int x, int y, int width, int height,
    IntrusiveSP<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
    IntrusiveSP<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1)
  {
#if USE_ARB
    QRP_2TexMod (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1);
    return;
#endif

    IntrusiveSP<IOpenGLShaderProgram> ShaderProg;
    {
      ShaderProg = m_Sl2TextureAdd;
    }

    QRP_Compute_Texture_Coord (width, height, DeviceTexture0, texxform0);
    QRP_Compute_Texture_Coord (width, height, DeviceTexture1, texxform1);

    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
      x,          y + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
      x + width,  y + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
      x + width,  y,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    int TextureObjectLocation0 = ShaderProg->GetUniformLocationARB ("TextureObject0");
    int TextureObjectLocation1 = ShaderProg->GetUniformLocationARB ("TextureObject1");
    int VertexLocation = ShaderProg->GetAttributeLocation ("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation ("MyTextureCoord0");
    int TextureCoord1Location = ShaderProg->GetAttributeLocation ("MyTextureCoord1");

    int TextureCoef0Location = ShaderProg->GetUniformLocationARB ("color0");
    int TextureCoef1Location = ShaderProg->GetUniformLocationARB ("color1");


    SetTexture (GL_TEXTURE0, DeviceTexture0);
    SetTexture (GL_TEXTURE1, DeviceTexture1);

    CHECKGL ( glUniform1iARB (TextureObjectLocation0, 0) );
    CHECKGL ( glUniform1iARB (TextureObjectLocation1, 1) );

    CHECKGL ( glUniform4fARB (TextureCoef0Location, color0.R(), color0.G(), color0.B(), color0.A() ) );
    CHECKGL ( glUniform4fARB (TextureCoef1Location, color1.R(), color1.G(), color1.B(), color1.A() ) );

    int VPMatrixLocation = ShaderProg->GetUniformLocationARB ("ViewProjectionMatrix");
    ShaderProg->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetModelViewProjectionMatrix().m) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4) );
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord1Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (TextureCoord1Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord1Location) );

    ShaderProg->End();
  }

  void GraphicsEngine::QRP_GLSL_4Tex (int x, int y, int width, int height,
                                       IntrusiveSP<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                                       IntrusiveSP<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1,
                                       IntrusiveSP<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm &texxform2, const Color &color2,
                                       IntrusiveSP<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm &texxform3, const Color &color3)
  {
#if USE_ARB
    QRP_4Tex (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1,
              DeviceTexture2, texxform2, color2, DeviceTexture3, texxform3, color3);
    return;
#endif

    QRP_Compute_Texture_Coord (width, height, DeviceTexture0, texxform0);
    QRP_Compute_Texture_Coord (width, height, DeviceTexture1, texxform1);
    QRP_Compute_Texture_Coord (width, height, DeviceTexture2, texxform2);
    QRP_Compute_Texture_Coord (width, height, DeviceTexture3, texxform3);

    IntrusiveSP<IOpenGLShaderProgram> ShaderProg;
//     if(SrcTexture0->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType) &&
//        SrcTexture1->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType) &&
//        SrcTexture2->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType) &&
//        SrcTexture3->Type().IsDerivedFromType(IOpenGLTexture2D::StaticObjectType))
    {
      ShaderProg = m_Sl4TextureAdd;
    }

//     if((SrcTexture0->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType) &&
//         SrcTexture1->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType) &&
//         SrcTexture2->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType) &&
//         SrcTexture3->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType)) /*||
//         (SrcTexture0->Type().IsDerivedFromType(TextureFrameAnimation::StaticObjectType) &&
//         SrcTexture1->Type().IsDerivedFromType(TextureFrameAnimation::StaticObjectType) &&
//         SrcTexture2->Type().IsDerivedFromType(TextureFrameAnimation::StaticObjectType) &&
//         SrcTexture3->Type().IsDerivedFromType(TextureFrameAnimation::StaticObjectType))*/)
//     {
//         bRectangleTexture = true;
//         ShaderProg = m_4TexBlendRectProg;
//     }

    float VtxBuffer[] =
    {
      x,          y,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 1.0f, texxform1.u0, texxform1.v0, 0, 1.0f, texxform2.u0, texxform2.v0, 0, 1.0f, texxform3.u0, texxform3.v0, 0, 1.0f,
      x,          y + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 1.0f, texxform1.u0, texxform1.v1, 0, 1.0f, texxform2.u0, texxform2.v1, 0, 1.0f, texxform3.u0, texxform3.v1, 0, 1.0f,
      x + width,  y + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 1.0f, texxform1.u1, texxform1.v1, 0, 1.0f, texxform2.u1, texxform2.v1, 0, 1.0f, texxform3.u1, texxform3.v1, 0, 1.0f,
      x + width,  y,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 1.0f, texxform1.u1, texxform1.v0, 0, 1.0f, texxform2.u1, texxform2.v0, 0, 1.0f, texxform3.u1, texxform3.v0, 0, 1.0f,
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    int TextureObjectLocation0 = ShaderProg->GetUniformLocationARB ("TextureObject0");
    int TextureObjectLocation1 = ShaderProg->GetUniformLocationARB ("TextureObject1");
    int TextureObjectLocation2 = ShaderProg->GetUniformLocationARB ("TextureObject2");
    int TextureObjectLocation3 = ShaderProg->GetUniformLocationARB ("TextureObject3");
    int VertexLocation = ShaderProg->GetAttributeLocation ("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation ("MyTextureCoord0");
    int TextureCoord1Location = ShaderProg->GetAttributeLocation ("MyTextureCoord1");
    int TextureCoord2Location = ShaderProg->GetAttributeLocation ("MyTextureCoord2");
    int TextureCoord3Location = ShaderProg->GetAttributeLocation ("MyTextureCoord3");

    int TextureCoef0Location = ShaderProg->GetUniformLocationARB ("color0");
    int TextureCoef1Location = ShaderProg->GetUniformLocationARB ("color1");
    int TextureCoef2Location = ShaderProg->GetUniformLocationARB ("color2");
    int TextureCoef3Location = ShaderProg->GetUniformLocationARB ("color3");

    SetTexture (GL_TEXTURE0, DeviceTexture0);
    SetTexture (GL_TEXTURE1, DeviceTexture1);
    SetTexture (GL_TEXTURE2, DeviceTexture2);
    SetTexture (GL_TEXTURE3, DeviceTexture3);

    CHECKGL ( glUniform1iARB (TextureObjectLocation0, 0) );
    CHECKGL ( glUniform1iARB (TextureObjectLocation1, 1) );
    CHECKGL ( glUniform1iARB (TextureObjectLocation2, 2) );
    CHECKGL ( glUniform1iARB (TextureObjectLocation3, 3) );

    CHECKGL ( glUniform4fARB (TextureCoef0Location, color0.R(), color0.G(), color0.B(), color0.A() ) );
    CHECKGL ( glUniform4fARB (TextureCoef1Location, color1.R(), color1.G(), color1.B(), color1.A() ) );
    CHECKGL ( glUniform4fARB (TextureCoef2Location, color2.R(), color2.G(), color2.B(), color2.A() ) );
    CHECKGL ( glUniform4fARB (TextureCoef3Location, color3.R(), color3.G(), color3.B(), color3.A() ) );

    int VPMatrixLocation = ShaderProg->GetUniformLocationARB ("ViewProjectionMatrix");
    ShaderProg->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetModelViewProjectionMatrix().m) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 4) );
    }

    if (TextureCoord1Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord1Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 8) );
    }

    if (TextureCoord2Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord2Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord2Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 12) );
    }

    if (TextureCoord3Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord3Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord3Location, 4, GL_FLOAT, GL_FALSE, 80, VtxBuffer + 16) );
    }

    CHECKGL ( glDrawArrays (GL_QUADS, 0, 4) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (TextureCoord1Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord1Location) );

    if (TextureCoord2Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord2Location) );

    if (TextureCoord3Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord3Location) );

    ShaderProg->End();
  }
///////////////////////////////////////////
  void GraphicsEngine::QRP_GLSL_Triangle (int x0, int y0,
      int x1, int y1,
      int x2, int y2,
      Color c0)
  {
#if USE_ARB
    QRP_Triangle (x0, y0, x1, y1, x2, y2, c0, c0, c0);
    return;
#endif

    QRP_GLSL_Triangle (x0, y0, x1, y1, x2, y2, c0, c0, c0);
  }

  void GraphicsEngine::QRP_GLSL_Triangle (int x0, int y0,
      int x1, int y1,
      int x2, int y2,
      Color c0, Color c1, Color c2)
  {
#if USE_ARB
    QRP_Triangle (x0, y0, x1, y1, x2, y2, c0, c1, c2);
    return;
#endif

    float VtxBuffer[] =
    {
      x0, y0, 0.0f, 1.0f, c0.R(), c0.G(), c0.B(), c0.A(),
      x1, y1, 0.0f, 1.0f, c1.R(), c1.G(), c1.B(), c1.A(),
      x2, y2, 0.0f, 1.0f, c2.R(), c2.G(), c2.B(), c2.A(),
    };

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );

    m_SlColor->Begin();

    int VertexLocation = m_SlColor->GetAttributeLocation ("AVertex");
    int VertexColorLocation = m_SlColor->GetAttributeLocation ("VertexColor");

    int VPMatrixLocation = m_SlColor->GetUniformLocationARB ("ViewProjectionMatrix");
    m_SlColor->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetModelViewProjectionMatrix().m) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 32, VtxBuffer + 4) );

    CHECKGL ( glDrawArrays (GL_TRIANGLES, 0, 3) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );
    m_SlColor->End();

    m_triangle_stats++;
  }

//////////////////////
// DRAW LINES       //
//////////////////////
  void GraphicsEngine::QRP_GLSL_Line (int x0, int y0,
                                       int x1, int y1, Color c0)
  {
#if USE_ARB
    QRP_Line (x0, y0, x1, y1, c0, c0);
    return;
#endif

    QRP_Line (x0, y0, x1, y1, c0, c0);
  }

  void GraphicsEngine::QRP_GLSL_Line (int x0, int y0,
                                       int x1, int y1, Color c0, Color c1)
  {
#if USE_ARB
    QRP_Line (x0, y0, x1, y1, c0, c1);
    return;
#endif

    float VtxBuffer[] =
    {
      x0, y0, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c0.R(), c0.G(), c0.B(), c0.A(),
      x1, y1, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c1.R(), c1.G(), c1.B(), c1.A(),
    };

    IntrusiveSP<IOpenGLShaderProgram> ShaderProg = m_SlColor;


    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB ("TextureObject0");
    int VertexLocation = ShaderProg->GetAttributeLocation ("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation ("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation ("VertexColor");


    CHECKGL ( glUniform1iARB (TextureObjectLocation, 0) );

    int VPMatrixLocation = ShaderProg->GetUniformLocationARB ("ViewProjectionMatrix");
    ShaderProg->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetModelViewProjectionMatrix().m) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4) );
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_LINES, 0, 2) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (VertexColorLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );

    ShaderProg->End();

    m_line_stats++;
  }

  void GraphicsEngine::QRP_GLSL_QuadWireframe (int x0, int y0, int width, int height,
      Color c0,
      Color c1,
      Color c2,
      Color c3)
  {
#if USE_ARB
    QRP_QuadWireframe (x0, y0, width, height, c0, c1, c2, c3);
    return;
#endif

    float VtxBuffer[] =
    {
      x0, y0,                             0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c0.R(), c0.G(), c0.B(), c0.A(),
      x0, y0 + height - 1,                0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c1.R(), c1.G(), c1.B(), c1.A(),
      x0 + width - 1, y0 + height - 1,    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c2.R(), c2.G(), c2.B(), c2.A(),
      x0 + width - 1, y0,                 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c3.R(), c3.G(), c3.B(), c3.A(),
      x0, y0,                             0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c0.R(), c0.G(), c0.B(), c0.A(),
    };

    IntrusiveSP<IOpenGLShaderProgram> ShaderProg = m_SlColor;


    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB ("TextureObject0");
    int VertexLocation = ShaderProg->GetAttributeLocation ("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation ("MyTextureCoord0");
    int VertexColorLocation = ShaderProg->GetAttributeLocation ("VertexColor");


    CHECKGL ( glUniform1iARB (TextureObjectLocation, 0) );

    int VPMatrixLocation = ShaderProg->GetUniformLocationARB ("ViewProjectionMatrix");
    ShaderProg->SetUniformLocMatrix4fv ( (GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetModelViewProjectionMatrix().m) );

    CHECKGL ( glEnableVertexAttribArrayARB (VertexLocation) );
    CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer) );

    if (TextureCoord0Location != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (TextureCoord0Location) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4) );
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (VertexColorLocation) );
      CHECKGL ( glVertexAttribPointerARB ( (GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8) );
    }

    CHECKGL ( glDrawArrays (GL_LINE_STRIP, 0, 5) );

    CHECKGL ( glDisableVertexAttribArrayARB (VertexLocation) );

    if (TextureCoord0Location != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (TextureCoord0Location) );

    if (VertexColorLocation != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (VertexColorLocation) );

    ShaderProg->End();

    m_line_stats++;
  }

}

