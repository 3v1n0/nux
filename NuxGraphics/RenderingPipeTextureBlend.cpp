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
#include "NuxImage/ImageSurface.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"
#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GraphicsEngine.h"

#include "RenderingPipeTextureBlendShaderSource.h"

namespace nux
{
  const char *const
  GraphicsEngine::GetBlendModeBlendFunc (BlendMode blend_mode)
  {
    switch (blend_mode)
      {
      case BLEND_MODE_NORMAL:
	return BlendNormalShader;
        break;
      case BLEND_MODE_OVERLAY:
	return BlendOverlayShader;
	break;
	
      default:
	return NULL;
      }
  }

const char *const
GraphicsEngine::GetBlendModeString (BlendMode blend_mode)
{
  switch (blend_mode)
    {
    case BLEND_MODE_NORMAL:
      return "BlendNormal";
      break;
    case BLEND_MODE_OVERLAY:
      return "BlendOverlay";
      break;
    default:
      return NULL;
    }
}

ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetBlendTexColorProgram (BlendMode blend_mode)
{
  if (_blend_tex_color_prog[blend_mode].IsValid())
    {
      return _blend_tex_color_prog[blend_mode];
    }
  
  ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();
  
  NString VSString;
  NString PSString;
  
  VSString =
                     VertexShaderHeader
                     "attribute vec4 AVertex;                                 \n\
                     attribute vec4 MyTextureCoord0;                         \n\
                     attribute vec4 VertexColor;                             \n\
                     uniform mat4 ViewProjectionMatrix;                      \n\
                     varying vec4 varyTexCoord0;                             \n\
                     varying vec4 varyVertexColor;                           \n	\
                     void main()                                             \n\
                     {                                                        \n\
                     gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
                     varyTexCoord0 = MyTextureCoord0;                        \n\
                     varyVertexColor = VertexColor;                          \n\
                     }";

    PSString =  
                    FragmentShaderHeader
                     "uniform vec3 color0;                                    \n\
                     uniform float BlendOpacity; \n\
                     uniform sampler2D TextureObject0;                           \n\
                     varying vec4 varyTexCoord0;                                 \n\
                     varying vec4 varyVertexColor;                               \n\
                     %s                                                          \n\
                     vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
                     {                                                           \n\
                     return texture2D(TexObject, TexCoord.st);                   \n\
                     }                                                           \n\
                     void main()                                                 \n\
                     {                                                           \n\
                     vec4 v = SampleTexture(TextureObject0, varyTexCoord0);      \n\
                     gl_FragColor.rgb = mix(v.rgb,%s(v.rgb,color0),BlendOpacity); \n\
                     gl_FragColor.a = 1.0; \n\
                     }";

    int l = PSString.Size();
    l += strlen (GetBlendModeBlendFunc (blend_mode));
    l += strlen (GetBlendModeString (blend_mode)) + 1;
    
    char *shader_prog = new char[l];
    sprintf(shader_prog, PSString.GetTCharPtr(), GetBlendModeBlendFunc (blend_mode),
	    GetBlendModeString (blend_mode));
    
    _blend_tex_color_prog[blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(*VSString));
    PS->SetShaderCode(shader_prog, "#define SAMPLERTEX2D");
    
    delete [] shader_prog;
    
    _blend_tex_color_prog[blend_mode]->ClearShaderObjects();
    _blend_tex_color_prog[blend_mode]->AddShaderObject(VS);
    _blend_tex_color_prog[blend_mode]->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_blend_tex_color_prog[blend_mode]->GetOpenGLID(), 0, "AVertex"));
    _blend_tex_color_prog[blend_mode]->Link();
    
    return _blend_tex_color_prog[blend_mode];    
}

  ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetBlendColorTexProgram (BlendMode blend_mode)
{
  if (_blend_color_tex_prog[blend_mode].IsValid())
    {
      return _blend_color_tex_prog[blend_mode];
    }
  
  ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();
  
  NString VSString;
  NString PSString;
  
  VSString =
                     VertexShaderHeader
                     "attribute vec4 AVertex;                                 \n\
                     attribute vec4 MyTextureCoord0;                         \n\
                     attribute vec4 VertexColor;                             \n\
                     uniform mat4 ViewProjectionMatrix;                      \n\
                     varying vec4 varyTexCoord0;                             \n\
                     varying vec4 varyVertexColor;                           \n	\
                     void main()                                             \n\
                     {                                                        \n\
                     gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
                     varyTexCoord0 = MyTextureCoord0;                        \n\
                     varyVertexColor = VertexColor;                          \n\
                     }";

    PSString =  
                    FragmentShaderHeader
                     "uniform vec3 color0;                                    \n\
                     uniform float BlendOpacity; \n\
                     uniform sampler2D TextureObject0;                           \n\
                     varying vec4 varyTexCoord0;                                 \n\
                     varying vec4 varyVertexColor;                               \n\
                     %s                                                          \n\
                     vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)      \n\
                     {                                                           \n\
                     return texture2D(TexObject, TexCoord.st);                   \n\
                     }                                                           \n\
                     void main()                                                 \n\
                     {                                                           \n\
                     vec4 v = SampleTexture(TextureObject0, varyTexCoord0);      \n\
                     gl_FragColor.rgb = mix(color0,%s(color0,v.rgb),v.a); \n\
                     gl_FragColor.a = 1.0; \n\
                     }";

    int l = PSString.Size();
    l += strlen (GetBlendModeBlendFunc (blend_mode));
    l += strlen (GetBlendModeString (blend_mode)) + 1;
    
    char *shader_prog = new char[l];
    sprintf(shader_prog, PSString.GetTCharPtr(), GetBlendModeBlendFunc (blend_mode),
	    GetBlendModeString (blend_mode));
    
    _blend_color_tex_prog[blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(*VSString));
    PS->SetShaderCode(shader_prog, "#define SAMPLERTEX2D");
    
    delete [] shader_prog;
    
    _blend_color_tex_prog[blend_mode]->ClearShaderObjects();
    _blend_color_tex_prog[blend_mode]->AddShaderObject(VS);
    _blend_color_tex_prog[blend_mode]->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_blend_color_tex_prog[blend_mode]->GetOpenGLID(), 0, "AVertex"));
    _blend_color_tex_prog[blend_mode]->Link();
    
    return _blend_color_tex_prog[blend_mode];    
}

ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetBlendTexTexProgram (BlendMode blend_mode)
{
  if (_blend_tex_tex_prog[blend_mode].IsValid())
    {
      return _blend_tex_tex_prog[blend_mode];
    }
  
  ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();
  
  NString VSString;
  NString PSString;

    VSString =        VertexShaderHeader
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
  
    PSString =      FragmentShaderHeader
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
                    %s \n\
                    void main()                                                 \n\
                    {                                                           \n\
                    vec4 b0 = color0*SampleTexture(TextureObject0, varyTexCoord0);     \n\
                    vec4 b1 = color1*SampleTexture(TextureObject1, varyTexCoord1);     \n\
                    gl_FragColor = %s(b0, b1);                                             \n\
                    }";

    int l = PSString.Size();
    l += strlen (GetBlendModeBlendFunc (blend_mode));
    l += strlen (GetBlendModeString (blend_mode)) + 1;
    
    char *shader_prog = new char[l];
    sprintf(shader_prog, PSString.GetTCharPtr(), GetBlendModeBlendFunc (blend_mode),
	    GetBlendModeString (blend_mode));
    
    _blend_tex_tex_prog[blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    VS->SetShaderCode(TCHAR_TO_ANSI(*VSString));
    PS->SetShaderCode(shader_prog, "#define SAMPLERTEX2D");
    
    delete [] shader_prog;
    
    _blend_tex_tex_prog[blend_mode]->ClearShaderObjects();
    _blend_tex_tex_prog[blend_mode]->AddShaderObject(VS);
    _blend_tex_tex_prog[blend_mode]->AddShaderObject(PS);
    CHECKGL(glBindAttribLocation(_blend_tex_tex_prog[blend_mode]->GetOpenGLID(), 0, "AVertex"));
    _blend_tex_tex_prog[blend_mode]->Link();
    
    return _blend_tex_tex_prog[blend_mode];    
}

void GraphicsEngine::QRP_GLSL_1TexBlendColor (int x, int y, int width, int height,
					      ObjectPtr<IOpenGLBaseTexture> DeviceTexture,
					      TexCoordXForm &texxform0, const Color & color0,
					      const Color &blend_color, BlendMode blend_mode)
{
    ObjectPtr<IOpenGLShaderProgram> ShaderProg = GetBlendTexColorProgram (blend_mode);

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, DeviceTexture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0, blend_color.red, blend_color.green, blend_color.blue, blend_color.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0, blend_color.red, blend_color.green, blend_color.blue, blend_color.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0, blend_color.red, blend_color.green, blend_color.blue, blend_color.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0, blend_color.red, blend_color.green, blend_color.blue, blend_color.alpha,
    };
    
    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int BlendColorLocation = ShaderProg->GetUniformLocationARB("color0");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0"); 
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");
    int OpacityLocation = ShaderProg->GetUniformLocationARB("BlendOpacity");

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
    
    if (BlendColorLocation != -1)
      {
	CHECKGL(glUniform3fARB(BlendColorLocation, blend_color.red, blend_color.green, blend_color.blue));
      } 
    if (OpacityLocation != -1)
      {
	CHECKGL(glUniform1fARB(OpacityLocation, blend_color.alpha));
      }

    
    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();;
}

void GraphicsEngine::QRP_GLSL_ColorBlend1Tex (int x, int y, int width, int height,
					      const Color &base_color,
					      ObjectPtr<IOpenGLBaseTexture> DeviceTexture,
					      TexCoordXForm &texxform0, const Color & color0,
					      BlendMode blend_mode)
{
    ObjectPtr<IOpenGLShaderProgram> ShaderProg = GetBlendColorTexProgram (blend_mode);

    m_quad_tex_stats++;
    QRP_Compute_Texture_Coord(width, height, DeviceTexture, texxform0);
    float fx = x, fy = y;
    float VtxBuffer[] =
    {
      fx,          fy,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0, 0, base_color.red, base_color.green, base_color.blue, base_color.alpha,
      fx,          fy + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0, 0, base_color.red, base_color.green, base_color.blue, base_color.alpha,
      fx + width,  fy + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0, 0, base_color.red, base_color.green, base_color.blue, base_color.alpha,
      fx + width,  fy,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0, 0, base_color.red, base_color.green, base_color.blue, base_color.alpha,
    };
    
    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
    ShaderProg->Begin();

    int TextureObjectLocation = ShaderProg->GetUniformLocationARB("TextureObject0");
    int BlendColorLocation = ShaderProg->GetUniformLocationARB("color0");
    int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
    int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0"); 
    int VertexColorLocation = ShaderProg->GetAttributeLocation("VertexColor");
    int OpacityLocation = ShaderProg->GetUniformLocationARB("BlendOpacity");

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
    
    if (BlendColorLocation != -1)
      {
	CHECKGL(glUniform3fARB(BlendColorLocation, base_color.red, base_color.green, base_color.blue));
      } 
    if (OpacityLocation != -1)
      {
	CHECKGL(glUniform1fARB(OpacityLocation, base_color.alpha));
      }

    
    CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    ShaderProg->End();;
}



void GraphicsEngine::QRP_GLSL_2TexBlend(int x, int y, int width, int height,
					ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm& texxform0, const Color &color0,
					ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm& texxform1, const Color &color1,
					BlendMode blend_mode)
{
  ObjectPtr <IOpenGLShaderProgram> ShaderProg = GetBlendTexTexProgram (blend_mode);
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

}

