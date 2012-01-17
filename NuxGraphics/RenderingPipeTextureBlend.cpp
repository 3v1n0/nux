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

static std::string LayerBlendingVertexShader ="#version 110                   \n\
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
                      }";

static std::string blend_code_normal = "#version 110                                          \n\
vec3 BlendNormal(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  return L;                                                                                   \n\
}";

static std::string blend_code_lighten = "#version 110                                         \n\
vec3 BlendLighten(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  // This mode is commutative                                                                 \n\
  vec3 V = max(L, B);                                                                         \n\
  return V;                                                                                   \n\
}";

static std::string blend_code_darken = "#version 110                                          \n\
vec3 BlendDarken(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  vec3 V = min(L, B);                                                                         \n\
  return V;                                                                                   \n\
}";

static std::string BlendMultiplyShader = "#version 110                                        \n\
vec3 BlendMultiply(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                  \n\
{                                                                                             \n\
  return(B * L);                                                                              \n\
}";

static std::string BlendAverageShader = "#version 110                                         \n\
vec3 BlendAverage(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  // This mode is commutative                                                                 \n\
  return((B + L) / 2.0);                                                                      \n\
}";

// *** Additive Modes ***
static std::string BlendAddShader = "#version 110                                             \n\
vec3 BlendAdd(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                       \n\
{                                                                                             \n\
  return min(B + L, vec3(1.0));                                                               \n\
}";

static std::string BlendSubstractShader = "#version 110                                       \n\
vec3 BlendSubstract(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                 \n\
{                                                                                             \n\
  return max(B + L - vec3(1.0), vec3(0.0));                                                   \n\
}";

// *** Difference Modes ***
static std::string BlendDifferenceShader = "#version 110                                      \n\
vec3 BlendDifference(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                \n\
{                                                                                             \n\
  return abs(B - L);                                                                          \n\
}";

static std::string BlendNegationShader = "#version 110                                        \n\
vec3 BlendNegation(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                  \n\
{                                                                                             \n\
  return(vec3(1.0) - abs(vec3(1.0) - B - L));                                                 \n\
}";

static std::string BlendExclusionShader = "#version 110                                       \n\
vec3 BlendExclusion(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                 \n\
{                                                                                             \n\
  return(B + L - 2.0 * B * L);                                                                \n\
}";

static std::string BlendScreenShader = "#version 110                                          \n\
vec3 BlendScreen(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                    \n\
{                                                                                             \n\
  return(1.0 - (1.0 - B) * (1.0 - L));                                                        \n\
}";

static std::string BlendOverlayShader = "#version 110                                         \n\
float _BlendOverlay(float B /*background layer*/, float L /*foreground layer*/)               \n\
{                                                                                             \n\
  if (L < 0.5)                                                                                \n\
    return(2.0 * B * L);                                                                      \n\
  else                                                                                        \n\
    return(1.0 - 2.0 * (1.0 - B) * (1.0 - L));                                                \n\
}                                                                                             \n\
vec3 BlendOverlay(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                   \n\
{                                                                                             \n\
  return vec3(_BlendOverlay(B.r + L.r), _BlendOverlay(B.g + L.g), _BlendOverlay(B.b + L.b));  \n\
}";

static std::string BlendSoftLightShader = "#version 110                                             \n\
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

static std::string BlendHardLightShader = "#version 110                                             \n\
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

static std::string BlendColorDodgeShader = "#version 110                                              \n\
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

static std::string BlendLinearDodgeShader = "#version 110                                                 \n\
float _BlendLinearDodge(float B /*background layer*/, float L /*foreground layer*/)                       \n\
{                                                                                                         \n\
  return min(B + L, 1.0);                                                                                 \n\
}                                                                                                         \n\
vec3 BlendLinearDodge(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                           \n\
{                                                                                                         \n\
  return vec3(_BlendLinearDodge(B.r + L.r), _BlendLinearDodge(B.g + L.g), _BlendLinearDodge(B.b + L.b));  \n\
}";

static std::string BlendColorBurnShader = "#version 110                                                   \n\
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

static std::string BlendLinearBurnShader = "#version 110                                                  \n\
float _BlendLinearBurn(float B /*background layer*/, float L /*foreground layer*/)                        \n\
{                                                                                                         \n\
  return max(B + L - 1.0, 0.0);                                                                           \n\
}                                                                                                         \n\
vec3 BlendLinearBurn(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                            \n\
{                                                                                                         \n\
  return vec3(_BlendLinearBurn(B.r + L.r), _BlendLinearBurn(B.g + L.g), _BlendLinearBurn(B.b + L.b));     \n\
}";

static std::string BlendLinearLightShader = "#version 110                                                 \n\
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

static std::string BlendVividLightShader = "#version 110                                                  \n\
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

static std::string BlendPinLightShader = "#version 110                                                    \n\
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

static std::string BlendHardMixShader = "#version 110                                                     \n\
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

static std::string BlendReflectShader = "#version 110                                                     \n\
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

static std::string BlendGlowShader = "#version 110                                                        \n\
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

static std::string BlendPhoenixShader = "#version 110                                                     \n\
vec3 BlendPhoenix(vec3 B /*background layer*/, vec3 L /*foreground layer*/)                               \n\
{                                                                                                         \n\
  return min(B, L) - max(B, L) + vec3(1.0);                                                               \n\
}";

static std::string BlendOpacityShader = "#version 110                                                     \n\
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

  void GraphicsEngine::InitBlendModeNormal()
  {
    ObjectPtr<IOpenGLVertexShader> vertex_shader = _graphics_display.m_DeviceFactory->CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> blend_modes = _graphics_display.m_DeviceFactory->CreatePixelShader();
    ObjectPtr<IOpenGLPixelShader> blend_operation = _graphics_display.m_DeviceFactory->CreatePixelShader();
    std::string blending_operation_code;

    vertex_shader->SetShaderCode(TCHAR_TO_ANSI(LayerBlendingVertexShader.c_str()));
    blend_operation->SetShaderCode(TCHAR_TO_ANSI(blend_code_normal.c_str()));

    // For some strange reason, make sure that the attribute holding the vertex position has a name that comes first in alphabetic order before all
    // other  attributes. Otherwise you get a bug on NVidia! Why is that???

    blending_operation_code = "#version 110                                                 \n\
                        varying vec4 varyTexCoord0;                                         \n\
                        varying vec4 varyTexCoord1;                                         \n\
                        uniform vec4 color0;                                                \n\
                        uniform vec4 color1;                                                \n\
                        uniform float alpha;                                                \n\
                        uniform float opacity;                                              \n\
                        uniform sampler2D TextureObject0;                                   \n\
                        uniform sampler2D TextureObject1;                                   \n\
                        vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)              \n\
                        {                                                                   \n\
                          return texture2D(TexObject, TexCoord.st);                         \n\
                        }                                                                   \n\
                        void main()                                                         \n\
                        {                                                                   \n\
                          vec4 b0 = color0*SampleTexture(TextureObject0, varyTexCoord0);    \n\
                          vec4 b1 = color1*SampleTexture(TextureObject1, varyTexCoord1);    \n\
                          gl_FragColor = vec4(BlendNormal(b0, b1, opacity), alpha);         \n\
                        }";

    _shader_layer_blend_normal = _graphics_display.m_DeviceFactory->CreateShaderProgram();
    blend_operation->SetShaderCode(TCHAR_TO_ANSI(blending_operation_code.c_str()));

    _shader_layer_blend_normal->ClearShaderObjects();
    _shader_layer_blend_normal->AddShaderObject(vertex_shader);
    _shader_layer_blend_normal->AddShaderObject(blend_modes);
    _shader_layer_blend_normal->AddShaderObject(blend_operation);
    CHECKGL(glBindAttribLocation(_shader_layer_blend_normal->GetOpenGLID(), 0, "AVertex"));
    _shader_layer_blend_normal->Link();
  }


//   void GraphicsEngine::QRP_GLSL_2TexMod(int x, int y, int width, int height,
//     ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
//     ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1)
//   {
//     NUX_RETURN_IF_FALSE(m_Sl2TextureMod.IsValid());
// 
//     ObjectPtr<IOpenGLShaderProgram> ShaderProg;
//     {
//       ShaderProg = m_Sl2TextureMod;
//     }
// 
//     QRP_Compute_Texture_Coord(width, height, DeviceTexture0, texxform0);
//     QRP_Compute_Texture_Coord(width, height, DeviceTexture1, texxform1);
// 
//     float VtxBuffer[] =
//     {
//       x,          y,          0.0f, 1.0f, texxform0.u0, texxform0.v0, 0.0f, 1.0f, texxform1.u0, texxform1.v0, 0.0f, 1.0f,
//       x,          y + height, 0.0f, 1.0f, texxform0.u0, texxform0.v1, 0.0f, 1.0f, texxform1.u0, texxform1.v1, 0.0f, 1.0f,
//       x + width,  y + height, 0.0f, 1.0f, texxform0.u1, texxform0.v1, 0.0f, 1.0f, texxform1.u1, texxform1.v1, 0.0f, 1.0f,
//       x + width,  y,          0.0f, 1.0f, texxform0.u1, texxform0.v0, 0.0f, 1.0f, texxform1.u1, texxform1.v0, 0.0f, 1.0f,
//     };
// 
//     CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
//     CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
//     ShaderProg->Begin();
// 
//     int TextureObjectLocation0 = ShaderProg->GetUniformLocationARB("TextureObject0");
//     int TextureObjectLocation1 = ShaderProg->GetUniformLocationARB("TextureObject1");
//     int VertexLocation = ShaderProg->GetAttributeLocation("AVertex");
//     int TextureCoord0Location = ShaderProg->GetAttributeLocation("MyTextureCoord0");
//     int TextureCoord1Location = ShaderProg->GetAttributeLocation("MyTextureCoord1");
// 
//     int TextureCoef0Location = ShaderProg->GetUniformLocationARB("color0");
//     int TextureCoef1Location = ShaderProg->GetUniformLocationARB("color1");
// 
// 
//     SetTexture(GL_TEXTURE0, DeviceTexture0);
//     SetTexture(GL_TEXTURE1, DeviceTexture1);
// 
//     CHECKGL(glUniform1iARB(TextureObjectLocation0, 0));
//     CHECKGL(glUniform1iARB(TextureObjectLocation1, 1));
// 
//     CHECKGL(glUniform4fARB(TextureCoef0Location, color0.red, color0.green, color0.blue, color0.alpha ));
//     CHECKGL(glUniform4fARB(TextureCoef1Location, color1.red, color1.green, color1.blue, color1.alpha ));
// 
//     int VPMatrixLocation = ShaderProg->GetUniformLocationARB("ViewProjectionMatrix");
//     ShaderProg->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, false, (GLfloat *) & (GetOpenGLModelViewProjectionMatrix().m));
// 
//     CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
//     CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));
// 
//     if (TextureCoord0Location != -1)
//     {
//       CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
//       CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
//     }
// 
//     if (TextureCoord1Location != -1)
//     {
//       CHECKGL(glEnableVertexAttribArrayARB(TextureCoord1Location));
//       CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord1Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
//     }
// 
//     CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));
// 
//     CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));
// 
//     if (TextureCoord0Location != -1)
//       CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));
// 
//     if (TextureCoord1Location != -1)
//       CHECKGL(glDisableVertexAttribArrayARB(TextureCoord1Location));
// 
//     ShaderProg->End();
//   }
}

