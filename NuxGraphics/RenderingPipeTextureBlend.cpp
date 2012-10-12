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
std::string GraphicsEngine::GetBlendModeBlendFunc(LayerBlendMode layer_blend_mode)
{
  switch (layer_blend_mode)
  {
  case LAYER_BLEND_MODE_NORMAL:       return BlendNormalShader;

  case LAYER_BLEND_MODE_DARKEN:       return BlendDarkenShader;
  case LAYER_BLEND_MODE_MULTIPLY:     return BlendMultiplyShader;
  case LAYER_BLEND_MODE_COLOR_BURN:   return BlendColorBurnShader;
  case LAYER_BLEND_MODE_LINEAR_BURN:  return BlendLinearBurnShader;

  case LAYER_BLEND_MODE_LIGHTEN:      return BlendLightenShader;
  case LAYER_BLEND_MODE_SCREEN:       return BlendScreenShader;
  case LAYER_BLEND_MODE_COLOR_DODGE:  return BlendColorDodgeShader;
  case LAYER_BLEND_MODE_LINEAR_DODGE: return BlendLinearDodgeShader;

  case LAYER_BLEND_MODE_OVERLAY:      return BlendOverlayShader;
  case LAYER_BLEND_MODE_SOFT_LIGHT:   return BlendSoftLightShader;
  case LAYER_BLEND_MODE_HARD_LIGHT:   return BlendHardLightShader; 
  case LAYER_BLEND_MODE_VIVID_LIGHT:  return BlendVividLightShader;
  case LAYER_BLEND_MODE_LINEAR_LIGHT: return BlendLinearLightShader;
  case LAYER_BLEND_MODE_PIN_LIGHT:    return BlendPinLightShader;
  case LAYER_BLEND_MODE_HARD_MIX:     return BlendHardMixShader;

  case LAYER_BLEND_MODE_DIFFERENCE:   return BlendDifferenceShader;
  case LAYER_BLEND_MODE_EXCLUSION:    return BlendExclusionShader;

  case LAYER_BLEND_MODE_SUBTRACT:     return BlendSubstractShader;
  
  case LAYER_BLEND_MODE_AVERAGE:      return BlendAverageShader;
  case LAYER_BLEND_MODE_ADD:          return BlendAddShader;
  case LAYER_BLEND_MODE_NEGATION:     return BlendNegationShader;
  case LAYER_BLEND_MODE_REFLECT:      return BlendReflectShader;
  case LAYER_BLEND_MODE_GLOW:         return BlendGlowShader;
  case LAYER_BLEND_MODE_PHOENIX:      return BlendPhoenixShader;

  case LAYER_BLEND_MODE_LAST:         return BlendNormalShader;
  default:                            return BlendNormalShader;
  }
}

std::string GraphicsEngine::GetBlendModeString(LayerBlendMode layer_blend_mode)
{
  switch (layer_blend_mode)
  {
  case LAYER_BLEND_MODE_NORMAL:       return std::string("BlendNormal");

  case LAYER_BLEND_MODE_DARKEN:       return std::string("BlendDarken");
  case LAYER_BLEND_MODE_MULTIPLY:     return std::string("BlendMultiply");
  case LAYER_BLEND_MODE_COLOR_BURN:   return std::string("BlendColorBurn");
  case LAYER_BLEND_MODE_LINEAR_BURN:  return std::string("BlendLinearBurn");

  case LAYER_BLEND_MODE_LIGHTEN:      return std::string("BlendLighten");
  case LAYER_BLEND_MODE_SCREEN:       return std::string("BlendScreen");
  case LAYER_BLEND_MODE_COLOR_DODGE:  return std::string("BlendColorDodge");
  case LAYER_BLEND_MODE_LINEAR_DODGE: return std::string("BlendLinearDodge");

  case LAYER_BLEND_MODE_OVERLAY:      return std::string("BlendOverlay");
  case LAYER_BLEND_MODE_SOFT_LIGHT:   return std::string("BlendSoftLight");
  case LAYER_BLEND_MODE_HARD_LIGHT:   return std::string("BlendHardLight"); 
  case LAYER_BLEND_MODE_VIVID_LIGHT:  return std::string("BlendVividLight");
  case LAYER_BLEND_MODE_LINEAR_LIGHT: return std::string("BlendLinearLight");
  case LAYER_BLEND_MODE_PIN_LIGHT:    return std::string("BlendPinLight");
  case LAYER_BLEND_MODE_HARD_MIX:     return std::string("BlendHardMix");

  case LAYER_BLEND_MODE_DIFFERENCE:   return std::string("BlendDifference");
  case LAYER_BLEND_MODE_EXCLUSION:    return std::string("BlendExclusion");

  case LAYER_BLEND_MODE_SUBTRACT:     return std::string("BlendSubstract");

  case LAYER_BLEND_MODE_AVERAGE:      return std::string("BlendAverage");
  case LAYER_BLEND_MODE_ADD:          return std::string("BlendAdd");
  case LAYER_BLEND_MODE_NEGATION:     return std::string("BlendNegation");
  case LAYER_BLEND_MODE_REFLECT:      return std::string("BlendReflect");
  case LAYER_BLEND_MODE_GLOW:         return std::string("BlendGlow");
  case LAYER_BLEND_MODE_PHOENIX:      return std::string("BlendPhoenix");

  case LAYER_BLEND_MODE_LAST:         return std::string("BlendNormal");
  default:                            return std::string("BlendNormal");
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
  
  const std::string vertex_code =
  NUX_VERTEX_SHADER_HEADER
  "uniform mat4 view_projection_matrix;                       \n\
  attribute vec4 in_vertex;                                   \n\
  attribute vec4 in_bkg_tex_coord;                            \n\
  varying vec4 bkg_tex_coord;                                 \n\
  void main()                                                 \n\
  {                                                           \n\
    gl_Position =  view_projection_matrix * (in_vertex);      \n\
    bkg_tex_coord = in_bkg_tex_coord;                         \n\
  }";

  const std::string pixel_code =
  NUX_FRAGMENT_SHADER_HEADER
  "uniform vec4 bkg_color;                                                    \n\
  uniform sampler2D bkg_texture;                                              \n\
  uniform vec4 frg_color;                                                     \n\
  varying vec4 bkg_tex_coord;                                                 \n\
  %s                                                                          \n\
  void main()                                                                 \n\
  {                                                                           \n\
  vec4 tex = texture2D(bkg_texture, bkg_tex_coord.st) * bkg_color;            \n\
    gl_FragColor.rgb = mix(tex.rgb,%s(tex.rgb, frg_color.rgb), frg_color.a);  \n\
    gl_FragColor.a = 1.0;                                                     \n\
  }";

  int safe_area = 10;
  int code_buffer_size = pixel_code.length();
  code_buffer_size += GetBlendModeBlendFunc(layer_blend_mode).length();
  code_buffer_size += GetBlendModeString(layer_blend_mode).length() + safe_area;

  char* shader_prog = new char[code_buffer_size];
  std::memset(shader_prog, 0, code_buffer_size);

  sprintf(shader_prog, pixel_code.c_str(), GetBlendModeBlendFunc(layer_blend_mode).c_str(), GetBlendModeString(layer_blend_mode).c_str());

  blend_tex_color_prog_[layer_blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
  VS->SetShaderCode(vertex_code.c_str());
  PS->SetShaderCode(shader_prog);
  delete [] shader_prog;

  blend_tex_color_prog_[layer_blend_mode]->ClearShaderObjects();
  blend_tex_color_prog_[layer_blend_mode]->AddShaderObject(VS);
  blend_tex_color_prog_[layer_blend_mode]->AddShaderObject(PS);
  CHECKGL(glBindAttribLocation(blend_tex_color_prog_[layer_blend_mode]->GetOpenGLID(), 0, "in_vertex"));
  blend_tex_color_prog_[layer_blend_mode]->Link();

  return blend_tex_color_prog_[layer_blend_mode];
}

void GraphicsEngine::QRP_GLSL_ColorLayerOverTexture(int x, int y, int width, int height,
  ObjectPtr<IOpenGLBaseTexture> bkg_device_texture, TexCoordXForm& bkg_texxform, const Color& bkg_color,
  const Color& frg_color,
  LayerBlendMode layer_blend_mode)
{
  if (!UsingGLSLCodePath())
    return;

  ObjectPtr<IOpenGLShaderProgram> ShaderProg = GetColorBlendOverTexProgram(layer_blend_mode);

  m_quad_tex_stats++;
  QRP_Compute_Texture_Coord(width, height, bkg_device_texture, bkg_texxform);
  float fx = x, fy = y;
  float vtx_buffer[] =
  {
    fx,          fy,          0.0f, 1.0f, bkg_texxform.u0, bkg_texxform.v0, 0, 0,
    fx,          fy + height, 0.0f, 1.0f, bkg_texxform.u0, bkg_texxform.v1, 0, 0,
    fx + width,  fy + height, 0.0f, 1.0f, bkg_texxform.u1, bkg_texxform.v1, 0, 0,
    fx + width,  fy,          0.0f, 1.0f, bkg_texxform.u1, bkg_texxform.v0, 0, 0,
  };

  CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
  CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
  ShaderProg->Begin();

  int in_vertex_loc         = ShaderProg->GetAttributeLocation("in_vertex");
  int in_bkg_tex_coord_loc  = ShaderProg->GetAttributeLocation("in_bkg_tex_coord"); 
  int bkg_texture_loc       = ShaderProg->GetUniformLocationARB("bkg_texture");
  int bkg_color_loc         = ShaderProg->GetUniformLocationARB("bkg_color");
  int frg_color_loc         = ShaderProg->GetUniformLocationARB("frg_color");

  SetTexture(GL_TEXTURE0, bkg_device_texture);
  CHECKGL(glUniform1iARB(bkg_texture_loc, 0));
  if (bkg_color_loc != -1)
  {
    CHECKGL(glUniform4fARB(bkg_color_loc, bkg_color.red, bkg_color.green, bkg_color.blue, bkg_color.alpha));
  }
  if (frg_color_loc != -1)
  {
    CHECKGL(glUniform4fARB(frg_color_loc, frg_color.red, frg_color.green, frg_color.blue, frg_color.alpha));
  }

  int view_projection_matrix_loc = ShaderProg->GetUniformLocationARB("view_projection_matrix");
  Matrix4 mvp_matrix = GetOpenGLModelViewProjectionMatrix();
  ShaderProg->SetUniformLocMatrix4fv((GLint) view_projection_matrix_loc, 1, false, (GLfloat*) & (mvp_matrix.m));

  if (in_vertex_loc != -1)
  {
    CHECKGL(glEnableVertexAttribArrayARB(in_vertex_loc));
    CHECKGL(glVertexAttribPointerARB((GLuint) in_vertex_loc, 4, GL_FLOAT, GL_FALSE, 32, vtx_buffer));
  }

  if (in_bkg_tex_coord_loc != -1)
  {
    CHECKGL(glEnableVertexAttribArrayARB(in_bkg_tex_coord_loc));
    CHECKGL(glVertexAttribPointerARB((GLuint) in_bkg_tex_coord_loc, 4, GL_FLOAT, GL_FALSE, 32, vtx_buffer + 4));
  }

  CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

  if (in_vertex_loc != -1)
    CHECKGL(glDisableVertexAttribArrayARB(in_vertex_loc));

  if (in_bkg_tex_coord_loc != -1)
    CHECKGL(glDisableVertexAttribArrayARB(in_bkg_tex_coord_loc));

  ShaderProg->End();;
}

ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetTextureLayerOverColorProgram(LayerBlendMode layer_blend_mode)
{
  if (blend_color_tex_prog_[layer_blend_mode].IsValid())
  {
    return blend_color_tex_prog_[layer_blend_mode];
  }
  
  ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();
  
  std::string vertex_code =
    NUX_VERTEX_SHADER_HEADER
    "uniform mat4 view_projection_matrix;                   \n\
    attribute vec4 in_vertex;                               \n\
    attribute vec4 in_frg_tex_coord;                        \n\
    varying vec4 frg_tex_coord;                             \n\
    void main()                                             \n\
    {                                                       \n\
      gl_Position =  view_projection_matrix * (in_vertex);  \n\
      frg_tex_coord = in_frg_tex_coord;                     \n\
    }";

  std::string pixel_code =
    NUX_FRAGMENT_SHADER_HEADER
    "uniform vec4 frg_color;                                                          \n\
    uniform sampler2D frg_texture;                                                    \n\
    uniform vec4 bkg_color;                                                           \n\
    varying vec4 frg_tex_coord;                                                       \n\
    %s                                                                                \n\
    void main()                                                                       \n\
    {                                                                                 \n\
      vec3 tex = texture2D(frg_texture, frg_tex_coord.st).rgb * frg_color.rgb;        \n\
      gl_FragColor.rgb = %s(bkg_color.rgb * bkg_color.aaa, tex.rgb * frg_color.aaa);  \n\
      gl_FragColor.a = 1.0;                                                           \n\
    }";

  int safe_area = 10;
  int code_buffer_size = pixel_code.length();
  code_buffer_size += GetBlendModeBlendFunc(layer_blend_mode).length();
  code_buffer_size += GetBlendModeString(layer_blend_mode).length() + safe_area;

  char* shader_prog = new char[code_buffer_size];
  std::memset(shader_prog, 0, code_buffer_size);

  sprintf(shader_prog, pixel_code.c_str(), GetBlendModeBlendFunc(layer_blend_mode).c_str(), GetBlendModeString(layer_blend_mode).c_str());

  blend_color_tex_prog_[layer_blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
  VS->SetShaderCode(vertex_code.c_str());
  PS->SetShaderCode(shader_prog);

  delete [] shader_prog;

  blend_color_tex_prog_[layer_blend_mode]->ClearShaderObjects();
  blend_color_tex_prog_[layer_blend_mode]->AddShaderObject(VS);
  blend_color_tex_prog_[layer_blend_mode]->AddShaderObject(PS);
  CHECKGL(glBindAttribLocation(blend_color_tex_prog_[layer_blend_mode]->GetOpenGLID(), 0, "in_vertex"));
  blend_color_tex_prog_[layer_blend_mode]->Link();

  return blend_color_tex_prog_[layer_blend_mode];
}


void GraphicsEngine::QRP_GLSL_TextureLayerOverColor(int x, int y, int width, int height,
  const Color& bkg_color,
  ObjectPtr<IOpenGLBaseTexture> frg_device_texture, TexCoordXForm& frg_texxform, const Color& frg_color,
  LayerBlendMode layer_blend_mode)
{
  if (!UsingGLSLCodePath())
    return;

  ObjectPtr<IOpenGLShaderProgram> ShaderProg = GetTextureLayerOverColorProgram(layer_blend_mode);

  m_quad_tex_stats++;
  QRP_Compute_Texture_Coord(width, height, frg_device_texture, frg_texxform);
  float fx = x, fy = y;
  float vtx_buffer[] =
  {
    fx,          fy,          0.0f, 1.0f, frg_texxform.u0, frg_texxform.v0, 0, 0,
    fx,          fy + height, 0.0f, 1.0f, frg_texxform.u0, frg_texxform.v1, 0, 0,
    fx + width,  fy + height, 0.0f, 1.0f, frg_texxform.u1, frg_texxform.v1, 0, 0,
    fx + width,  fy,          0.0f, 1.0f, frg_texxform.u1, frg_texxform.v0, 0, 0,
  };

  CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
  CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
  ShaderProg->Begin();

  int frg_texture_loc       = ShaderProg->GetUniformLocationARB("frg_texture");
  int frg_color_loc         = ShaderProg->GetUniformLocationARB("frg_color");
  int bkg_color_loc         = ShaderProg->GetUniformLocationARB("bkg_color");
  int in_vertex_loc         = ShaderProg->GetAttributeLocation("in_vertex");
  int in_frg_tex_coord_loc  = ShaderProg->GetAttributeLocation("in_frg_tex_coord"); 
  

  SetTexture(GL_TEXTURE0, frg_device_texture);
  CHECKGL(glUniform1iARB(frg_texture_loc, 0));

  int view_projection_matrix_loc = ShaderProg->GetUniformLocationARB("view_projection_matrix");
  Matrix4 mvp_matrix = GetOpenGLModelViewProjectionMatrix();
  ShaderProg->SetUniformLocMatrix4fv((GLint) view_projection_matrix_loc, 1, false, (GLfloat*) & (mvp_matrix.m));

  if (in_vertex_loc != -1)
  {
    CHECKGL(glEnableVertexAttribArrayARB(in_vertex_loc));
    CHECKGL(glVertexAttribPointerARB((GLuint) in_vertex_loc, 4, GL_FLOAT, GL_FALSE, 32, vtx_buffer));
  }

  if (in_frg_tex_coord_loc != -1)
  {
    CHECKGL(glEnableVertexAttribArrayARB(in_frg_tex_coord_loc));
    CHECKGL(glVertexAttribPointerARB((GLuint) in_frg_tex_coord_loc, 4, GL_FLOAT, GL_FALSE, 32, vtx_buffer + 4));
  }

  if (frg_color_loc != -1)
  {
    CHECKGL(glUniform4fARB(frg_color_loc, frg_color.red, frg_color.green, frg_color.blue, frg_color.alpha));
  } 
  if (bkg_color_loc != -1)
  {
    CHECKGL(glUniform4fARB(bkg_color_loc, bkg_color.red, bkg_color.green, bkg_color.blue, bkg_color.alpha));
  }

  CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

  if (in_vertex_loc != -1)
    CHECKGL(glDisableVertexAttribArrayARB(in_vertex_loc));

  if (in_frg_tex_coord_loc != -1)
    CHECKGL(glDisableVertexAttribArrayARB(in_frg_tex_coord_loc));

  ShaderProg->End();
}


ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetColorLayerOverColorProgram(LayerBlendMode layer_blend_mode)
{
  if (blend_color_color_prog_[layer_blend_mode].IsValid())
    return blend_color_color_prog_[layer_blend_mode];

  ObjectPtr<IOpenGLVertexShader> vertexShader = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> pixelShader = _graphics_display.m_DeviceFactory->CreatePixelShader();

  std::string vertex_code = NUX_VERTEX_SHADER_HEADER
  "uniform mat4 view_projection_matrix;                   \n\
  attribute vec4 in_vertex;                               \n\
  void main()                                             \n\
  {                                                       \n\
    gl_Position =  view_projection_matrix * in_vertex;    \n\
  }";

  std::string pixel_code = NUX_FRAGMENT_SHADER_HEADER
  "uniform vec4 bkg_color;                                                                    \n\
  uniform vec4 frg_color;                                                                     \n\
  %s                                                                                          \n\
  void main()                                                                                 \n\
  {                                                                                           \n\
    gl_FragColor.rgb = %s(bkg_color.rgb * bkg_color.aaa, frg_color.rgb * frg_color.aaa);      \n\
    gl_FragColor.a = 1.0;                                                                     \n\
  }";

  int safe_area = 10;
  int code_buffer_size = pixel_code.length();
  code_buffer_size += GetBlendModeBlendFunc(layer_blend_mode).length();
  code_buffer_size += GetBlendModeString(layer_blend_mode).length() + safe_area;

  char* shader_prog = new char[code_buffer_size];
  std::memset(shader_prog, 0, code_buffer_size);

  sprintf(shader_prog, pixel_code.c_str(), GetBlendModeBlendFunc(layer_blend_mode).c_str(), GetBlendModeString(layer_blend_mode).c_str());

  blend_color_color_prog_[layer_blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
  vertexShader->SetShaderCode(vertex_code.c_str());
  pixelShader->SetShaderCode(shader_prog);
    
  delete [] shader_prog;
    
  blend_color_color_prog_[layer_blend_mode]->ClearShaderObjects();
  blend_color_color_prog_[layer_blend_mode]->AddShaderObject(vertexShader);
  blend_color_color_prog_[layer_blend_mode]->AddShaderObject(pixelShader);
  CHECKGL(glBindAttribLocation(blend_color_color_prog_[layer_blend_mode]->GetOpenGLID(), 0, "in_vertex"));
  blend_color_color_prog_[layer_blend_mode]->Link();
    
  return blend_color_color_prog_[layer_blend_mode];
}

void GraphicsEngine::QRP_GLSL_ColorLayerOverColor(int x, int y, int width, int height,
  const Color& bkg_color,
  const Color& frg_color,
  LayerBlendMode layer_blend_mode)
{
  if (!UsingGLSLCodePath())
    return;

  ObjectPtr <IOpenGLShaderProgram> shader_prog = GetColorLayerOverColorProgram(layer_blend_mode);

  float fx = x, fy = y;
  float vtx_buffer[] =
  {
    fx,          fy,          0.0f, 1.0f,
    fx,          fy + height, 0.0f, 1.0f,
    fx + width,  fy + height, 0.0f, 1.0f,
    fx + width,  fy,          0.0f, 1.0f,
  };

  CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
  CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
  shader_prog->Begin();

  int in_vertex_loc = shader_prog->GetAttributeLocation("in_vertex");
  int bkg_color_loc = shader_prog->GetUniformLocationARB("bkg_color");
  int frg_color_loc = shader_prog->GetUniformLocationARB("frg_color");

  if (bkg_color_loc != -1)
    CHECKGL(glUniform4fARB(bkg_color_loc, bkg_color.red, bkg_color.green, bkg_color.blue, bkg_color.alpha));

  if (frg_color_loc != -1)
    CHECKGL(glUniform4fARB(frg_color_loc, frg_color.red, frg_color.green, frg_color.blue, frg_color.alpha));

  int view_projection_matrix_loc = shader_prog->GetUniformLocationARB("view_projection_matrix");
  Matrix4 mvp_matrix = GetOpenGLModelViewProjectionMatrix();
  shader_prog->SetUniformLocMatrix4fv((GLint) view_projection_matrix_loc, 1, false, (GLfloat*)&mvp_matrix.m);

  if (in_vertex_loc != -1)
  {
    CHECKGL(glEnableVertexAttribArrayARB(in_vertex_loc));
    CHECKGL(glVertexAttribPointerARB((GLuint)in_vertex_loc, 4, GL_FLOAT, GL_FALSE, 16, vtx_buffer));
  }

  CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

  if (in_vertex_loc != -1)
    CHECKGL(glDisableVertexAttribArrayARB(in_vertex_loc));

  shader_prog->End();
}


ObjectPtr<IOpenGLShaderProgram> GraphicsEngine::GetTextureLayerOverTextureProgram(LayerBlendMode layer_blend_mode)
{
  if (blend_tex_tex_prog_[layer_blend_mode].IsValid())
  {
    return blend_tex_tex_prog_[layer_blend_mode];
  }

  ObjectPtr<IOpenGLVertexShader> VS = _graphics_display.m_DeviceFactory->CreateVertexShader();
  ObjectPtr<IOpenGLPixelShader> PS = _graphics_display.m_DeviceFactory->CreatePixelShader();

  std::string vertex_code = NUX_VERTEX_SHADER_HEADER
    "uniform mat4 view_projection_matrix;                       \n\
    attribute vec4 in_vertex;                                   \n\
    attribute vec4 in_bkg_tex_coord;                            \n\
    attribute vec4 in_frg_tex_coord;                            \n\
    varying vec4 bkg_tex_coord;                                 \n\
    varying vec4 frg_tex_coord;                                 \n\
    void main()                                                 \n\
    {                                                           \n\
      bkg_tex_coord = in_bkg_tex_coord;                         \n\
      frg_tex_coord = in_frg_tex_coord;                         \n\
      gl_Position =  view_projection_matrix * (in_vertex);      \n\
    }";

  std::string pixel_code = NUX_FRAGMENT_SHADER_HEADER
    "varying vec4 bkg_tex_coord;                                                        \n\
    varying vec4 frg_tex_coord;                                                         \n\
    uniform vec4 bkg_color;                                                             \n\
    uniform vec4 frg_color;                                                             \n\
    uniform sampler2D bkg_texture;                                                      \n\
    uniform sampler2D frg_texture;                                                      \n\
    %s                                                                                  \n\
    void main()                                                                         \n\
    {                                                                                   \n\
      vec3 bkg_tex = texture2D(bkg_texture, bkg_tex_coord.st).rgb * bkg_color.rgb;      \n\
      vec3 frg_tex = texture2D(frg_texture, frg_tex_coord.st).rgb * frg_color.rgb;      \n\
      gl_FragColor.rgb = %s(bkg_tex.rgb * bkg_color.aaa, frg_tex.rgb * frg_color.aaa);  \n\
      gl_FragColor.a = 1.0;                                                             \n\
    }";

  int safe_area = 10;
  int code_buffer_size = pixel_code.length();
  code_buffer_size += GetBlendModeBlendFunc(layer_blend_mode).length();
  code_buffer_size += GetBlendModeString(layer_blend_mode).length() + safe_area;

  char* shader_prog = new char[code_buffer_size];
  std::memset(shader_prog, 0, code_buffer_size);

  sprintf(shader_prog, pixel_code.c_str(), GetBlendModeBlendFunc(layer_blend_mode).c_str(), GetBlendModeString(layer_blend_mode).c_str());

  blend_tex_tex_prog_[layer_blend_mode] = _graphics_display.m_DeviceFactory->CreateShaderProgram();
  VS->SetShaderCode(vertex_code.c_str());
  PS->SetShaderCode(shader_prog);

  delete [] shader_prog;

  blend_tex_tex_prog_[layer_blend_mode]->ClearShaderObjects();
  blend_tex_tex_prog_[layer_blend_mode]->AddShaderObject(VS);
  blend_tex_tex_prog_[layer_blend_mode]->AddShaderObject(PS);
  CHECKGL(glBindAttribLocation(blend_tex_tex_prog_[layer_blend_mode]->GetOpenGLID(), 0, "in_vertex"));
  blend_tex_tex_prog_[layer_blend_mode]->Link();

  return blend_tex_tex_prog_[layer_blend_mode];
}

void GraphicsEngine::QRP_GLSL_TextureLayerOverTexture(int x, int y, int width, int height,
  ObjectPtr<IOpenGLBaseTexture> bkg_device_texture, TexCoordXForm& bkg_texxform, const Color& bkg_color,
  ObjectPtr<IOpenGLBaseTexture> frg_device_texture, TexCoordXForm& frg_texxform, const Color& frg_color,
  LayerBlendMode layer_blend_mode)
{
  if (!UsingGLSLCodePath())
    return;

  ObjectPtr <IOpenGLShaderProgram> ShaderProg = GetTextureLayerOverTextureProgram(layer_blend_mode);
  QRP_Compute_Texture_Coord(width, height, bkg_device_texture, bkg_texxform);
  QRP_Compute_Texture_Coord(width, height, frg_device_texture, frg_texxform);

  float fx = x, fy = y;
  float vtx_buffer[] =
  {
    fx,          fy,          0.0f, 1.0f, bkg_texxform.u0, bkg_texxform.v0, 0.0f, 1.0f, frg_texxform.u0, frg_texxform.v0, 0.0f, 1.0f,
    fx,          fy + height, 0.0f, 1.0f, bkg_texxform.u0, bkg_texxform.v1, 0.0f, 1.0f, frg_texxform.u0, frg_texxform.v1, 0.0f, 1.0f,
    fx + width,  fy + height, 0.0f, 1.0f, bkg_texxform.u1, bkg_texxform.v1, 0.0f, 1.0f, frg_texxform.u1, frg_texxform.v1, 0.0f, 1.0f,
    fx + width,  fy,          0.0f, 1.0f, bkg_texxform.u1, bkg_texxform.v0, 0.0f, 1.0f, frg_texxform.u1, frg_texxform.v0, 0.0f, 1.0f,
  };

  CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
  CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));
  ShaderProg->Begin();

  int bkg_texture_loc = ShaderProg->GetUniformLocationARB("bkg_texture");
  int frg_texture_loc = ShaderProg->GetUniformLocationARB("frg_texture");
  int in_vertex_loc = ShaderProg->GetAttributeLocation("in_vertex");
  int in_bkg_tex_coord_loc = ShaderProg->GetAttributeLocation("in_bkg_tex_coord");
  int in_frg_tex_coord_loc = ShaderProg->GetAttributeLocation("in_frg_tex_coord");
  
  int TextureCoef0Location = ShaderProg->GetUniformLocationARB("bkg_color");
  int TextureCoef1Location = ShaderProg->GetUniformLocationARB("frg_color");


  SetTexture(GL_TEXTURE0, bkg_device_texture);
  SetTexture(GL_TEXTURE1, frg_device_texture);

  CHECKGL(glUniform1iARB(bkg_texture_loc, 0));
  CHECKGL(glUniform1iARB(frg_texture_loc, 1));

  CHECKGL(glUniform4fARB(TextureCoef0Location, bkg_color.red, bkg_color.green, bkg_color.blue, bkg_color.alpha ));
  CHECKGL(glUniform4fARB(TextureCoef1Location, frg_color.red, frg_color.green, frg_color.blue, frg_color.alpha ));

  int     view_projection_matrix_loc = ShaderProg->GetUniformLocationARB("view_projection_matrix");
  Matrix4 mvp_matrix = GetOpenGLModelViewProjectionMatrix();
  ShaderProg->SetUniformLocMatrix4fv((GLint) view_projection_matrix_loc, 1, false, (GLfloat*) & (mvp_matrix.m));

  CHECKGL(glEnableVertexAttribArrayARB(in_vertex_loc));
  CHECKGL(glVertexAttribPointerARB((GLuint) in_vertex_loc, 4, GL_FLOAT, GL_FALSE, 48, vtx_buffer));

  if (in_bkg_tex_coord_loc != -1)
  {
    CHECKGL(glEnableVertexAttribArrayARB(in_bkg_tex_coord_loc));
    CHECKGL(glVertexAttribPointerARB((GLuint) in_bkg_tex_coord_loc, 4, GL_FLOAT, GL_FALSE, 48, vtx_buffer + 4));
  }

  if (in_frg_tex_coord_loc != -1)
  {
    CHECKGL(glEnableVertexAttribArrayARB(in_frg_tex_coord_loc));
    CHECKGL(glVertexAttribPointerARB((GLuint) in_frg_tex_coord_loc, 4, GL_FLOAT, GL_FALSE, 48, vtx_buffer + 8));
  }

  CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

  CHECKGL(glDisableVertexAttribArrayARB(in_vertex_loc));

  if (in_bkg_tex_coord_loc != -1)
    CHECKGL(glDisableVertexAttribArrayARB(in_bkg_tex_coord_loc));

  if (in_frg_tex_coord_loc != -1)
    CHECKGL(glDisableVertexAttribArrayARB(in_frg_tex_coord_loc));

  ShaderProg->End();
}



}