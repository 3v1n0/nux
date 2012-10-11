/*
 * Copyright 2010-2012 Inalogic® Inc.
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
 *              Robert Carr <racarr@canonical.com>
 */


#include "NuxCore/NuxCore.h"
#include "ImageSurface.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"
#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GraphicsEngine.h"

#include "RenderingPipeTextureBlendShaderSource.h"

namespace nux
{
  const char*
  GraphicsEngine::GetBlendModeBlendFunc(LayerBlendMode layer_blend_mode)
  {
    switch (layer_blend_mode)
      {
      case LAYER_BLEND_MODE_NORMAL:
  return BlendNormalShader;
        break;
      case LAYER_BLEND_MODE_OVERLAY:
  return BlendOverlayShader;
  break;
  
      default:
  return NULL;
      }
  }

const char*
GraphicsEngine::GetBlendModeString(LayerBlendMode layer_blend_mode)
{
  switch (layer_blend_mode)
    {
    case LAYER_BLEND_MODE_NORMAL:
      return "BlendNormal";
      break;
    case LAYER_BLEND_MODE_OVERLAY:
      return "BlendOverlay";
      break;
    default:
      return NULL;
    }
}

ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetColorBlendOverTexProgram(LayerBlendMode layer_blend_mode)
{
  if (blend_tex_color_prog_[layer_blend_mode].IsValid())
  {
    return blend_tex_color_prog_[layer_blend_mode];
  }
  
  ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();
  
  const char* VSString =
                    NUX_VERTEX_SHADER_HEADER
                    "attribute vec4 AVertex;                                 \n\
                    attribute vec4 MyTextureCoord0;                          \n\
                    attribute vec4 VertexColor;                              \n\
                    uniform mat4 ViewProjectionMatrix;                       \n\
                    varying vec4 varyTexCoord0;                              \n\
                    varying vec4 varyVertexColor;                            \n\
                    void main()                                              \n\
                    {                                                        \n\
                      gl_Position =  ViewProjectionMatrix*  (AVertex);       \n\
                      varyTexCoord0 = MyTextureCoord0;                       \n\
                      varyVertexColor = VertexColor;                         \n\
                    }";

  const char* PSString =
                    NUX_FRAGMENT_SHADER_HEADER
                    "uniform vec4 color0;                                           \n\
                    uniform sampler2D TextureObject0;                               \n\
                    varying vec4 varyTexCoord0;                                     \n\
                    varying vec4 varyVertexColor;                                   \n\
                    %s                                                              \n\
                    void main()                                                     \n\
                    {                                                               \n\
                      vec4 tex = texture2D(TextureObject0, varyTexCoord0.st)*varyVertexColor;       \n\
                      gl_FragColor.rgb = mix(tex.rgb,%s(tex.rgb,color0.rgb),color0.a);  \n\
                      gl_FragColor.a = 1.0;                                         \n\
                    }";

    int l = strlen(PSString);
    l += strlen(GetBlendModeBlendFunc(layer_blend_mode));
    l += strlen(GetBlendModeString(layer_blend_mode)) + 1;
    
    char* shader_prog = new char[l];

    sprintf(shader_prog, PSString, GetBlendModeBlendFunc(layer_blend_mode),
	    GetBlendModeString(layer_blend_mode));
    
    blend_tex_color_prog_[layer_blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(shader_prog, "#define SAMPLERTEX2D");
    
    delete [] shader_prog;
    
    blend_tex_color_prog_[layer_blend_mode]->ClearShaderObjects();
    blend_tex_color_prog_[layer_blend_mode]->AddShaderObject(VS);
    blend_tex_color_prog_[layer_blend_mode]->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(blend_tex_color_prog_[layer_blend_mode]->GetOpenGLID(), 0, "AVertex"));
    blend_tex_color_prog_[layer_blend_mode]->Link();
    
    return blend_tex_color_prog_[layer_blend_mode];    
}

  ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetTexBlendOverColorProgram(LayerBlendMode layer_blend_mode)
{
  if (blend_color_tex_prog_[layer_blend_mode].IsValid())
  {
    return blend_color_tex_prog_[layer_blend_mode];
  }
  
  ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();
  
  const char* VSString =
                NUX_VERTEX_SHADER_HEADER
                "attribute vec4 AVertex;                               \n\
                attribute vec4 MyTextureCoord0;                        \n\
                attribute vec4 VertexColor;                            \n\
                uniform mat4 ViewProjectionMatrix;                     \n\
                varying vec4 varyTexCoord0;                            \n\
                varying vec4 varyVertexColor;                          \n\
                void main()                                            \n\
                {                                                      \n\
                  gl_Position =  ViewProjectionMatrix * (AVertex);     \n\
                  varyTexCoord0 = MyTextureCoord0;                     \n\
                  varyVertexColor = VertexColor;                       \n\
                }";

    const char* PSString =  
                NUX_FRAGMENT_SHADER_HEADER
                "uniform vec3 color0;                                       \n\
                uniform float BlendOpacity;                                 \n\
                uniform sampler2D TextureObject0;                           \n\
                varying vec4 varyTexCoord0;                                 \n\
                varying vec4 varyVertexColor;                               \n\
                %s                                                          \n\
                void main()                                                 \n\
                {                                                           \n\
                  vec4 tex = texture2D(TextureObject0, varyTexCoord0.st) * varyVertexColor;     \n\
                  gl_FragColor.rgb = mix(color0,%s(color0.rgb,tex.rgb),tex.a);  \n\
                  gl_FragColor.a = 1.0;                                     \n\
                }";

    int l = strlen(PSString);
    l += strlen(GetBlendModeBlendFunc(layer_blend_mode));
    l += strlen(GetBlendModeString(layer_blend_mode)) + 1;
    
    char* shader_prog = new char[l];

    sprintf(shader_prog, PSString, GetBlendModeBlendFunc(layer_blend_mode),
	    GetBlendModeString(layer_blend_mode));
    
    blend_color_tex_prog_[layer_blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(shader_prog, "#define SAMPLERTEX2D");
    
    delete [] shader_prog;
    
    blend_color_tex_prog_[layer_blend_mode]->ClearShaderObjects();
    blend_color_tex_prog_[layer_blend_mode]->AddShaderObject(VS);
    blend_color_tex_prog_[layer_blend_mode]->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(blend_color_tex_prog_[layer_blend_mode]->GetOpenGLID(), 0, "AVertex"));
    blend_color_tex_prog_[layer_blend_mode]->Link();
    
    return blend_color_tex_prog_[layer_blend_mode];
}

ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetBlendTexTexProgram(LayerBlendMode layer_blend_mode)
{
  if (blend_tex_tex_prog_[layer_blend_mode].IsValid())
  {
    return blend_tex_tex_prog_[layer_blend_mode];
  }

  ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();
  
    const char* VSString =      NUX_VERTEX_SHADER_HEADER
                    "uniform mat4 ViewProjectionMatrix;                     \n\
                    attribute vec4 AVertex;                                 \n\
                    attribute vec4 MyTextureCoord0;                         \n\
                    attribute vec4 MyTextureCoord1;                         \n\
                    varying vec4 varyTexCoord0;                             \n\
                    varying vec4 varyTexCoord1;                             \n\
                    void main()                                             \n\
                    {                                                       \n\
                      varyTexCoord0 = MyTextureCoord0;                      \n\
                      varyTexCoord1 = MyTextureCoord1;                      \n\
                      gl_Position =  ViewProjectionMatrix * (AVertex);      \n\
                    }";
  
    const char* PSString =      NUX_FRAGMENT_SHADER_HEADER
                    "varying vec4 varyTexCoord0;                                \n\
                    varying vec4 varyTexCoord1;                                 \n\
                    uniform vec4 color0;                                        \n\
                    uniform vec4 color1;                                        \n\
                    uniform sampler2D TextureObject0;                           \n\
                    uniform sampler2D TextureObject1;                           \n\
                    %s                                                          \n\
                    void main()                                                 \n\
                    {                                                           \n\
                      vec4 bkg_tex = color0*texture2D(TextureObject0, varyTexCoord0.st);     \n\
                      vec4 frg_tex = color1*texture2D(TextureObject1, varyTexCoord1.st);     \n\
                      gl_FragColor = %s(bkg_tex.rgb, frg_tex.rgb);              \n\
                    }";

    int l = strlen(PSString);
    l += strlen(GetBlendModeBlendFunc(layer_blend_mode));
    l += strlen(GetBlendModeString(layer_blend_mode)) + 1;
    
    char* shader_prog = new char[l];

    sprintf(shader_prog, PSString, GetBlendModeBlendFunc(layer_blend_mode),
	    GetBlendModeString(layer_blend_mode));
    
    blend_tex_tex_prog_[layer_blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(VSString));
    PS->SetShaderCode(shader_prog, "#define SAMPLERTEX2D");
    
    delete [] shader_prog;
    
    blend_tex_tex_prog_[layer_blend_mode]->ClearShaderObjects();
    blend_tex_tex_prog_[layer_blend_mode]->AddShaderObject(VS);
    blend_tex_tex_prog_[layer_blend_mode]->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(blend_tex_tex_prog_[layer_blend_mode]->GetOpenGLID(), 0, "AVertex"));
    blend_tex_tex_prog_[layer_blend_mode]->Link();
    
    return blend_tex_tex_prog_[layer_blend_mode];
}

ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetBlendColorColorProgram(LayerBlendMode layer_blend_mode)
{
  if (blend_color_color_prog_[layer_blend_mode].IsValid())
    return blend_color_color_prog_[layer_blend_mode];

  ObjectPtr<IOpenGLVertexShader> vertexShader = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> pixelShader = _graphics_display.m_DeviceFactory->CreatePixelShader();

  std::string vertexCode = NUX_VERTEX_SHADER_HEADER
  "uniform mat4 viewProjectionMatrix;                     \n\
  attribute vec4 vertexPos;                               \n\
  void main()                                             \n\
  {                                                       \n\
    gl_Position =  viewProjectionMatrix * vertexPos;      \n\
  }";

  std::string pixelCode = NUX_FRAGMENT_SHADER_HEADER
  "uniform vec4 color0;                                   \n\
  uniform vec4 color1;                                    \n\
  %s                                                      \n\
  void main()                                             \n\
  {                                                       \n\
    gl_FragColor = %s(color0.rgb, color1.rgb);            \n\
  }";

  int l = pixelCode.size();
  l += strlen(GetBlendModeBlendFunc(layer_blend_mode));
  l += strlen(GetBlendModeString(layer_blend_mode)) + 1;
    
  char* shader_prog = new char[l];
  sprintf(shader_prog, pixelCode.c_str(), GetBlendModeBlendFunc(layer_blend_mode),
  GetBlendModeString(layer_blend_mode));
    
  blend_color_color_prog_[layer_blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
  vertexShader->SetShaderCode(vertexCode.c_str());
  pixelShader->SetShaderCode(shader_prog, "#define SAMPLERTEX2D");
    
  delete [] shader_prog;
    
  blend_color_color_prog_[layer_blend_mode]->ClearShaderObjects();
  blend_color_color_prog_[layer_blend_mode]->AddShaderObject(vertexShader);
  blend_color_color_prog_[layer_blend_mode]->AddShaderObject(pixelShader);
  CHECKGL(glBindAttribLocation(blend_color_color_prog_[layer_blend_mode]->GetOpenGLID(), 0, "vertexPos"));
  blend_color_color_prog_[layer_blend_mode]->Link();
    
  return blend_color_color_prog_[layer_blend_mode];
}

void GraphicsEngine::QRP_GLSL_ColorBlendOverTex(int x, int y, int width, int height,
          ObjectPtr<IOpenGLBaseTexture> bkg_device_texture, TexCoordXForm& texxform, const Color& color0,
          const Color& foreground_color,
          LayerBlendMode layer_blend_mode)
{
  if (!UsingGLSLCodePath())
    return;

    ObjectPtr<IOpenGLShaderProgram> ShaderProg = GetColorBlendOverTexProgram(layer_blend_mode);

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, bkg_device_texture, texxform);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform.u0, texxform.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform.u1, texxform.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
    };
    
    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int BlendColorLocation = ShaderProg->GetUniformLocationARB("color0");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0"); 
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");

    SetTexture(GL_TEXTURE0, bkg_device_texture);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));
    
    int VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat*) & (MVPMatrix.m));

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
    
    if (BlendColorLocation != -1)
    {
      CHECKGL(glUniform4fARB(BlendColorLocation, foreground_color.red, foreground_color.green, foreground_color.blue, foreground_color.alpha));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();;
}

void GraphicsEngine::QRP_GLSL_TexBlendOverColor(int x, int y, int width, int height,
          const Color& background_color,
          ObjectPtr<IOpenGLBaseTexture> frg_device_texture, TexCoordXForm& texxform0, const Color& color0,
          LayerBlendMode layer_blend_mode)
{
  if (!UsingGLSLCodePath())
    return;

    ObjectPtr<IOpenGLShaderProgram> ShaderProg = GetTexBlendOverColorProgram(layer_blend_mode);

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, frg_device_texture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0, color0.red, color0.green, color0.blue, color0.alpha,
    };

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int BlendColorLocation    = ShaderProg->GetUniformLocationARB("color0");
    int VertexLocation        = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0"); 
    int VertexColorLocation   = ShaderProg->GetAttributeLocation("VertexColor");
    int OpacityLocation       = ShaderProg->GetUniformLocationARB("BlendOpacity");

    SetTexture(GL_TEXTURE0, frg_device_texture);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    int VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
    Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
    ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat*) & (MVPMatrix.m));

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

    if (BlendColorLocation != -1)
    {
      CHECKGL(glUniform3fARB(BlendColorLocation, background_color.red, background_color.green, background_color.blue));
    } 
    if (OpacityLocation != -1)
    {
      CHECKGL(glUniform1fARB(OpacityLocation, background_color.alpha));
    }

    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();
}

void GraphicsEngine::QRP_GLSL_TexLayerOverTex(int x, int y, int width, int height,
          ObjectPtr<IOpenGLBaseTexture> bkg_device_texture, TexCoordXForm& texxform0, const Color& color0,
          ObjectPtr<IOpenGLBaseTexture> frg_device_texture, TexCoordXForm& texxform1, const Color& color1,
          LayerBlendMode layer_blend_mode)
{
  if (!UsingGLSLCodePath())
    return;

  ObjectPtr <IOpenGLShaderProgram> ShaderProg = GetBlendTexTexProgram(layer_blend_mode);
  QRP_Compute_Texture_Coord(width, height, bkg_device_texture, texxform0);
  QRP_Compute_Texture_Coord(width, height, frg_device_texture, texxform1);

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


  SetTexture(GL_TEXTURE0, bkg_device_texture);
  SetTexture(GL_TEXTURE1, frg_device_texture);

  CHECKGL(glUniform1iARB(TextureObjectLocation0, 0));
  CHECKGL(glUniform1iARB(TextureObjectLocation1, 1));

  CHECKGL(glUniform4fARB(TextureCoef0Location, color0.red, color0.green, color0.blue, color0.alpha ));
  CHECKGL(glUniform4fARB(TextureCoef1Location, color1.red, color1.green, color1.blue, color1.alpha ));

  int     VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
  Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
  ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat*) & (MVPMatrix.m));

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

void GraphicsEngine::QRP_GLSL_ColorLayerOverColor(int x, int y, int width, int height,
          const Color& bkg_color,
          const Color& color1,
          LayerBlendMode layer_blend_mode)
{
  if (!UsingGLSLCodePath())
    return;

  ObjectPtr <IOpenGLShaderProgram> shader_prog = GetBlendTexTexProgram(layer_blend_mode);

  float fx = x, fy = y;
  float vertexBuffer[] =
  {
      fx,          fy,          0.0f, 1.0f,
      fx,          fy + height, 0.0f, 1.0f,
      fx + width,  fy + height, 0.0f, 1.0f,
      fx + width,  fy,          0.0f, 1.0f,
  };

  CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
  //CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
  shader_prog->Begin();

  int vertexLocation = shader_prog->GetAttributeLocation("vertexPos");
  int colorLocation0 = shader_prog->GetUniformLocationARB("color0");
  int colorLocation1 = shader_prog->GetUniformLocationARB("color1");

  CHECKGL(glUniform4fARB(colorLocation0, bkg_color.red, bkg_color.green, bkg_color.blue, bkg_color.alpha ));
  CHECKGL(glUniform4fARB(colorLocation1, color1.red, color1.green, color1.blue, color1.alpha ));

  int VPMatrixLocation = shader_prog->GetUniformLocationARB("viewProjectionMatrix");
  Matrix4 MVPMatrix = GetOpenGLModelViewProjectionMatrix();
  shader_prog->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat*)&MVPMatrix.m);

  CHECKGL(glEnableVertexAttribArrayARB(vertexLocation));
  CHECKGL(glVertexAttribPointerARB((GLuint)vertexLocation, 4, GL_FLOAT, GL_FALSE, 48, vertexBuffer));

  CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

  CHECKGL(glDisableVertexAttribArrayARB(vertexLocation));

  shader_prog->End();
}

}