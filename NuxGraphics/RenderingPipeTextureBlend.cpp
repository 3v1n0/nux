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

namespace nux
{

  // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
  // other attributes. Otherwise you get a bug on NVidia! Why is that???
  // [Update]: it seems that the vertex attributes have to be declared in alphabetic order in the shader. It does not matter if the vertex
  // attribute is declared first in the alphabetic order.

  // For some other strange reason, on Intel GMA, the order in which attributes are used in the vertex shaders, is the order used to associated them with a
  // and attribute location. One has to make sure that the vertex attribute get index 0. So use the vertex attribute first. All of this does not make any sense.
  // Need more info from driver developers.

  static const char * const LayerBlendingVertexShader ="#version 110                   \n\
                      uniform mat4 ViewProjectionMatrix;                      \n \
                      attribute vec4 AVertex;                                 \n \
                      attribute vec4 MyTextureCoord0;                         \n \
                      attribute vec4 MyTextureCoord1;                         \n \
                      varying vec4 varyTexCoord0;                             \n \
                      varying vec4 varyTexCoord1;                             \n\
                      void main()                                             \n \
                      {                                                       \n\
                      varyTexCoord0 = MyTextureCoord0;                        \n\
                      varyTexCoord1 = MyTextureCoord1;                        \n\
                      gl_Position =  ViewProjectionMatrix * (AVertex);        \n\
                      }";

  static const char * const blend_code_normal = "#version 110                                          \n\
vec3 BlendNormal(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  return L;                                                                                   \n\
}";

  static const char * const blend_code_lighten = "#version 110                                         \n\
vec3 BlendLighten(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  // This mode is commutative                                                                 \n\
  vec3 V = max(L, B);                                                                         \n\
  return V;                                                                                   \n\
}";

  static const char * const blend_code_darken = "#version 110                                          \n\
vec3 BlendDarken(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  vec3 V = min(L, B);                                                                         \n\
  return V;                                                                                   \n\
}";

  static const char * const BlendMultiplyShader = "#version 110                                        \n\
vec3 BlendMultiply(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                  \n\
{                                                                                             \n\
  return(B * L);                                                                              \n\
}";

  static const char * const BlendAverageShader = "#version 110                                         \n\
vec3 BlendAverage(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  // This mode is commutative                                                                 \n\
  return((B + L) / 2.0);                                                                      \n\
}";

  // *** Additive Modes ***
  static const char * const BlendAddShader = "#version 110                                             \n\
vec3 BlendAdd(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                             \n\
  return min(B + L, vec3(1.0));                                                               \n\
}";

  static const char * const BlendSubstractShader = "#version 110                                       \n\
vec3 BlendSubstract(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                 \n\
{                                                                                             \n\
  return max(B + L - vec3(1.0), vec3(0.0));                                                   \n\
}";

// *** Difference Modes ***
static const char * const BlendDifferenceShader = "#version 110                                      \n\
vec3 BlendDifference(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                \n\
{                                                                                             \n\
  return abs(B - L);                                                                          \n\
}";

static const char * const BlendNegationShader = "#version 110                                        \n\
vec3 BlendNegation(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                  \n\
{                                                                                             \n\
  return(vec3(1.0) - abs(vec3(1.0) - B - L));                                                 \n\
}";

static const char * const BlendExclusionShader = "#version 110                                       \n\
vec3 BlendExclusion(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                 \n\
{                                                                                             \n\
  return(B + L - 2.0 * B * L);                                                                \n\
}";

static const char * const BlendScreenShader = "#version 110                                          \n\
vec3 BlendScreen(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  return(1.0 - (1.0 - B) * (1.0 - L));                                                        \n\
}";

static const char * const BlendOverlayShader = "                                         \n\
float _BlendOverlay(float B /*background layer*/, float L /*foreground layer*/)               \n\
{                                                                                             \n\
  if (L < 0.5)                                                                                \n\
    return(2.0 * B * L);                                                                      \n\
  else                                                                                        \n\
    return(1.0 - 2.0 * (1.0 - B) * (1.0 - L));                                                \n\
}                                                                                             \n\
vec3 BlendOverlay(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  return vec3(_BlendOverlay(B.r,L.r), _BlendOverlay(B.g ,L.g), _BlendOverlay(B.b,L.b));  \n\
}";

static const char * const BlendSoftLightShader = "#version 110                                             \n\
float _BlendSoftLight(float B /*background layer*/, float L /*foreground layer*/)                   \n\
{                                                                                                   \n\
  if (L < 0.5)                                                                                      \n\
    return(2.0 * B * L + B * B * (1.0 - 2.0 * L);                                                   \n\
  else                                                                                              \n\
    return sqrt(B) * (2.0 * L - 1.0) + 2.0 * B * (1.0 - L);                                         \n\
}                                                                                                   \n\
vec3 BlendSoftLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                                   \n\
  return vec3(_BlendSoftLight(B.r + L.r), _BlendSoftLight(B.g + L.g), _BlendSoftLight(B.b + L.b));  \n\
}";

  static const char * const BlendHardLightShader = "#version 110                                             \n\
float _BlendHardLight(float B /*background layer*/, float L /*foreground layer*/)                   \n\
{                                                                                                   \n\
  if (L < 0.5)                                                                                      \n\
    return(2.0 * B * L);                                                                            \n\
  else                                                                                              \n\
    return(1.0 - 2.0 * (1.0 - B) * (1.0 - L));                                                      \n\
}                                                                                                   \n\
vec3 BlendHardLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                                   \n\
  return vec3(_BlendHardLight(B.r + L.r), _BlendHardLight(B.g + L.g), _BlendHardLight(B.b + L.b));  \n\
}";

  static const char * const BlendColorDodgeShader = "#version 110                                              \n\
float _BlendColorDodge(float B /*background layer*/, float L /*foreground layer*/)                    \n\
{                                                                                                     \n\
  if (L == 1.0)                                                                                       \n\
    return 1.0;                                                                                       \n\
  else                                                                                                \n\
    return min(B / (1.0 - L), 1.0);                                                                   \n\
}                                                                                                     \n\
vec3 BlendColorDodge(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                        \n\
{                                                                                                     \n\
  return vec3(_BlendColorDodge(B.r + L.r), _BlendColorDodge(B.g + L.g), _BlendColorDodge(B.b + L.b)); \n\
}";

  static const char * const BlendLinearDodgeShader = "#version 110                                                 \n\
float _BlendLinearDodge(float B /*background layer*/, float L /*foreground layer*/)                       \n\
{                                                                                                         \n\
  return min(B + L, 1.0);                                                                                 \n\
}                                                                                                         \n\
vec3 BlendLinearDodge(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  return vec3(_BlendLinearDodge(B.r + L.r), _BlendLinearDodge(B.g + L.g), _BlendLinearDodge(B.b + L.b));  \n\
}";

  static const char * const BlendColorBurnShader = "#version 110                                                   \n\
float _BlendColorBurn(float B /*background layer*/, float L /*foreground layer*/)                         \n\
{                                                                                                         \n\
  if (L == 0.0)                                                                                           \n\
    return 0.0;                                                                                           \n\
  else                                                                                                    \n\
    return max(1.0 - ((1.0 - B) / L), 0.0);                                                               \n\
}                                                                                                         \n\
vec3 BlendColorBurn(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                             \n\
{                                                                                                         \n\
  return vec3(_BlendColorBurn(B.r + L.r), _BlendColorBurn(B.g + L.g), _BlendColorBurn(B.b + L.b));        \n\
}";

  static const char * const BlendLinearBurnShader = "#version 110                                                  \n\
float _BlendLinearBurn(float B /*background layer*/, float L /*foreground layer*/)                        \n\
{                                                                                                         \n\
  return max(B + L - 1.0, 0.0);                                                                           \n\
}                                                                                                         \n\
vec3 BlendLinearBurn(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                            \n\
{                                                                                                         \n\
  return vec3(_BlendLinearBurn(B.r + L.r), _BlendLinearBurn(B.g + L.g), _BlendLinearBurn(B.b + L.b));     \n\
}";

  static const char * const BlendLinearLightShader = "#version 110                                                 \n\
float _BlendLinearLight(float B /*background layer*/, float L /*foreground layer*/)                       \n\
{                                                                                                         \n\
  if (L < 0.5)                                                                                            \n\
    return _BlendLinearBurn(B, (2.0 * L));                                                                \n\
  else                                                                                                    \n\
    return _BlendLinearDodge(B, (2.0 * (L - 0.5)));                                                       \n\
}                                                                                                         \n\
vec3 BlendLinearLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  return vec3(_BlendLinearLight(B.r + L.r), _BlendLinearLight(B.g + L.g), _BlendLinearLight(B.b + L.b));  \n\
}";

  static const char * const BlendVividLightShader = "#version 110                                                  \n\
float _BlendVividLight(float B /*background layer*/, float L /*foreground layer*/)                        \n\
{                                                                                                         \n\
  if (L < 0.5)                                                                                            \n\
    return _BlendColorBurn(B, (2.0 * L));                                                                 \n\
  else                                                                                                    \n\
    return _BlendColorDodge(B, (2.0 * (L - 0.5))));                                                       \n\
}                                                                                                         \n\
vec3 BlendVividLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                            \n\
{                                                                                                         \n\
  return vec3(_BlendVividLight(B.r, L.r), _BlendVividLight(B.g, L.g), _BlendVividLight(B.b, L.b));        \n\
}";

static const char * const BlendPinLightShader = "#version 110                                                    \n\
float _BlendPinLight(float B /*background layer*/, float L /*foreground layer*/)                          \n\
{                                                                                                         \n\
  if (L < 0.5)                                                                                            \n\
    return _BlendDarken(B, (2.0 * L));                                                                    \n\
  else                                                                                                    \n\
    return _BlendLighten(B, (2.0 * (L - 0.5))));                                                          \n\
}                                                                                                         \n\
vec3 BlendPinLight(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                              \n\
{                                                                                                         \n\
  return vec3(_BlendPinLight(B.r, L.r), _BlendPinLight(B.g, L.g), _BlendPinLight(B.b, L.b));              \n\
}";

static const char * const BlendHardMixShader = "#version 110                                                     \n\
float _BlendHardMix(float B /*background layer*/, float L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  if (_BlendVividLight(B, L) < 0.5)                                                                       \n\
    return 0.0;                                                                                           \n\
  else                                                                                                    \n\
    return 1.0;                                                                                           \n\
}                                                                                                         \n\
vec3 BlendHardMix(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return vec3(_BlendHardMix(B.r, L.r), _BlendHardMix(B.g, L.g), _BlendHardMix(B.b, L.b));                 \n\
}";

static const char * const BlendReflectShader = "#version 110                                                     \n\
float _BlendReflect(float B /*background layer*/, float L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  if (L == 1.0)                                                                                           \n\
    return 1.0;                                                                                           \n\
  else                                                                                                    \n\
    return min(B * B / (1.0 - L), 1.0);                                                                   \n\
}                                                                                                         \n\
vec3 BlendReflect(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return vec3(_BlendReflect(B.r, L.r), _BlendReflect(B.g, L.g), _BlendReflect(B.b, L.b));                 \n\
}";

static const char * const BlendGlowShader = "#version 110                                                        \n\
float _BlendGlow(float B /*background layer*/, float L /*foreground layer*/)                              \n\
{                                                                                                         \n\
  if (L == 1.0)                                                                                           \n\
    return 1.0;                                                                                           \n\
  else                                                                                                    \n\
    return min(L * L / (1.0 - B), 1.0);                                                                   \n\
}                                                                                                         \n\
vec3 BlendGlow(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                                  \n\
{                                                                                                         \n\
  return vec3(_BlendGlow(B.r, L.r), _BlendGlow(B.g, L.g), _BlendGlow(B.b, L.b));                          \n\
}";

static const char * const BlendPhoenixShader = "#version 110                                                     \n\
vec3 BlendPhoenix(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return min(B, L) - max(B, L) + vec3(1.0);                                                               \n\
}";

static const char * const BlendOpacityShader = "#version 110                                                     \n\
vec3 BlendOpacity(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return O * L + (1 - O) * B;                                                                             \n\
}";

// Noise Shader: http://www.ozone3d.net/blogs/lab/20110427/glsl-random-generator/
// varying vec3 v;
// float rand(vec2 n)
// {
//   return 0.5 + 0.5 *
//     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
// }
// void main(void)
// {
//   float x = rand(v.xz);
//   gl_FragColor = vec4(x, x, x, 1.0);
// }

#ifndef NUX_OPENGLES_20
#define VertexShaderHeader "#version 110 \n"
#else
#define VertexShaderHeader
#endif

#ifndef NUX_OPENGLES_20
#define FragmentShaderHeader "#version 110 \n"
#else
#define FragmentShaderHeader "precision mediump float; \n"
#endif

const char *const
GraphicsEngine::GetBlendModeBlendFunc (BlendMode blend_mode)
{
switch (blend_mode)
  {
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
                     gl_FragColor.rgb = mix(v.rgb,%s(v.rgb,color0),color0.a); \n\
                     gl_FragColor.a = 1.0; \n\
                     }";

    //                     gl_FragColor = %s(v*varyVertexColor, blendColor);                           
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
    int OpacityLocation = ShaderProg->GetAttributeLocation("BlendOpacity");

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

}
