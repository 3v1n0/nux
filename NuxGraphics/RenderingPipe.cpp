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


#include "GLResource.h"
#include "IOpenGLBaseTexture.h"
#include "IOpenGLTexture2D.h"
#include "IOpenGLRectangleTexture.h"
#include "RenderingPipe.h"
#include "GraphicsEngine.h"

namespace nux
{

  struct TexWrapMapping
  {
    TexWrap tex_wrap_mode;
    t_u32 opengl_wrap_mode;
  };

  struct TexWrapMapping TexWrapMappingArray [] =
  {
    {TEXWRAP_REPEAT,                        GL_REPEAT},
    {TEXWRAP_CLAMP,                         GL_CLAMP},
    {TEXWRAP_CLAMP_TO_EDGE,                 GL_CLAMP_TO_EDGE},
    {TEXWRAP_CLAMP_TO_BORDER,               GL_CLAMP_TO_BORDER},
    {TEXWRAP_MIRRORED_REPEAT,               GL_MIRRORED_REPEAT},
    {TEXWRAP_MIRROR_CLAMP_EXT,              GL_MIRROR_CLAMP_EXT},
    {TEXWRAP_MIRROR_CLAMP_TO_EDGE_EXT,      GL_MIRROR_CLAMP_TO_EDGE_EXT},
    {TEXWRAP_MIRROR_CLAMP_TO_BORDER_EXT,    GL_MIRROR_CLAMP_TO_BORDER_EXT},
    {TEXWRAP_UNKNOWN,                       0}
  };

  GLenum TexWrapGLMapping (TexWrap tex_wrap_mode)
  {
    int i = 0;

    while (TexWrapMappingArray[i].tex_wrap_mode != TEXWRAP_UNKNOWN)
    {
      if (TexWrapMappingArray[i].tex_wrap_mode == tex_wrap_mode)
      {
        return TexWrapMappingArray[i].opengl_wrap_mode;
      }

      ++i;
    }

    nuxAssertMsg (0, TEXT ("[TexWrapGLMapping] Invalid texture wrap mode.") );
    return GL_CLAMP;
  }

  struct TexFilterMapping
  {
    TexFilter tex_filter_mode;
    t_u32 opengl_filter_mode;
  };

  struct TexFilterMapping TexFilterMappingArray [] =
  {
    {TEXFILTER_LINEAR,                  GL_LINEAR},
    {TEXFILTER_NEAREST,                 GL_NEAREST},
    {TEXFILTER_NEAREST_MIPMAP_NEAREST,  GL_NEAREST_MIPMAP_NEAREST},
    {TEXFILTER_LINEAR_MIPMAP_NEAREST,   GL_LINEAR_MIPMAP_NEAREST},
    {TEXFILTER_NEAREST_MIPMAP_LINEAR,   GL_NEAREST_MIPMAP_LINEAR},
    {TEXFILTER_LINEAR_MIPMAP_LINEAR,    GL_LINEAR_MIPMAP_LINEAR},
    {TEXFILTER_UNKNOWN,                 0},
  };

  GLenum TexFilterGLMapping (TexFilter tex_filter_mode)
  {
    int i = 0;

    while (TexFilterMappingArray[i].tex_filter_mode != TEXFILTER_UNKNOWN)
    {
      if (TexFilterMappingArray[i].tex_filter_mode == tex_filter_mode)
      {
        return TexFilterMappingArray[i].opengl_filter_mode;
      }

      ++i;
    }

    nuxAssertMsg (0, TEXT ("[TexFilterGLMapping] Invalid texture filter mode.") );
    return GL_REPEAT;
  }

  struct RopBlendMapping
  {
    RopBlend rop_blend_mode;
    t_u32 opengl_blend_op;
  };

  struct RopBlendMapping RopBlendMappingArray [] =
  {
    {ROPBLEND_ZERO,                             GL_ZERO},
    {ROPBLEND_ONE,                              GL_ONE},
    {ROPBLEND_SRC_COLOR,                        GL_SRC_COLOR},
    {ROPBLEND_ONE_MINUS_SRC_COLOR,              GL_ONE_MINUS_SRC_COLOR},
    {ROPBLEND_DST_COLOR,                        GL_DST_COLOR},
    {ROPBLEND_ONE_MINUS_DST_COLOR,              GL_ONE_MINUS_DST_COLOR},
    {ROPBLEND_SRC_ALPHA,                        GL_SRC_ALPHA},
    {ROPBLEND_ONE_MINUS_SRC_ALPHA,              GL_ONE_MINUS_SRC_ALPHA},
    {ROPBLEND_DST_ALPHA,                        GL_DST_ALPHA},
    {ROPBLEND_ONE_MINUS_DST_ALPHA,              GL_ONE_MINUS_DST_ALPHA},
    {ROPBLEND_CONSTANT_COLOR,                   GL_CONSTANT_COLOR},
    {ROPBLEND_ONE_MINUS_CONSTANT_COLOR,         GL_ONE_MINUS_CONSTANT_COLOR},
    {ROPBLEND_CONSTANT_ALPHA,                   GL_CONSTANT_ALPHA},
    {ROPBLEND_ONE_MINUS_CONSTANT_ALPHA,         GL_ONE_MINUS_CONSTANT_ALPHA},
    {ROPBLEND_SRC_ALPHA_SATURATE,               GL_SRC_ALPHA_SATURATE},
    {ROPBLEND_UNKNOWN,                          0},
  };

  GLenum RopBlendGLMapping (RopBlend rop_blend_mode)
  {
    int i = 0;

    while (RopBlendMappingArray[i].rop_blend_mode != ROPBLEND_UNKNOWN)
    {
      if (RopBlendMappingArray[i].rop_blend_mode == rop_blend_mode)
      {
        return RopBlendMappingArray[i].opengl_blend_op;
      }

      ++i;
    }

    nuxAssertMsg (0, TEXT ("[RopBlendGLMapping] Invalid texture ROP operation.") );
    return ROPBLEND_ONE;
  }

  TexCoordXForm::TexCoordXForm()
  {
    u0 = v0 = u1 = v1 = 0.0f;
    uscale  = 1.0f;
    vscale  = 1.0f;
    uoffset = 0.0f;
    voffset = 0.0f;
    flip_u_coord = false;
    flip_v_coord = false;
    uwrap = TEXWRAP_CLAMP;
    vwrap = TEXWRAP_CLAMP;
    min_filter = TEXFILTER_NEAREST;
    mag_filter = TEXFILTER_NEAREST;
    m_tex_coord_type = TexCoordXForm::OFFSET_SCALE_COORD;
  }

  void TexCoordXForm::FlipUCoord (bool b)
  {
    flip_u_coord = b;
  }

  void TexCoordXForm::FlipVCoord (bool b)
  {
    flip_v_coord = b;
  }

  void TexCoordXForm::FlipUVCoord (bool flip_u, bool flip_v)
  {
    flip_u_coord = flip_u;
    flip_v_coord = flip_v;
  }

  void TexCoordXForm::SetFilter (TexFilter minfitter, TexFilter magfilter)
  {
    min_filter = minfitter;
    mag_filter = magfilter;
  }

  void TexCoordXForm::SetWrap (TexWrap u_wrap, TexWrap v_wrap)
  {
    uwrap = u_wrap;
    vwrap = v_wrap;
  }

  void TexCoordXForm::SetTexCoordType (TexCoordType tex_coord_type)
  {
    m_tex_coord_type = tex_coord_type;
  }

  void QRP_Compute_Texture_Coord (t_int32 quad_width, t_int32 quad_height, ObjectPtr<IOpenGLBaseTexture> tex, TexCoordXForm &texxform)
  {
    float tex_width = tex->GetWidth();
    float tex_height = tex->GetHeight();

    if (tex->Type().IsDerivedFromType (IOpenGLTexture2D::StaticObjectType) )
    {
      if (texxform.m_tex_coord_type == TexCoordXForm::OFFSET_SCALE_COORD)
      {
        texxform.u0 = texxform.uoffset;
        texxform.v0 = texxform.voffset;
        texxform.u1 = texxform.u0 + texxform.uscale;
        texxform.v1 = texxform.v0 + texxform.vscale;
      }
      else if (texxform.m_tex_coord_type == TexCoordXForm::OFFSET_COORD)
      {
        texxform.u0 = texxform.uoffset;
        texxform.v0 = texxform.voffset;
        texxform.u1 = texxform.u0 + (float) quad_width / tex_width;
        texxform.v1 = texxform.v0 + (float) quad_height / tex_height;
      }
      else if (texxform.m_tex_coord_type == TexCoordXForm::UNNORMALIZED_COORD)
      {
        texxform.u0 /= (float) tex_width;
        texxform.v0 /= (float) tex_height;
        texxform.u1 /= (float) tex_width;
        texxform.v1 /= (float) tex_height;
      }
      else if (texxform.m_tex_coord_type == TexCoordXForm::NORMALIZED_COORD || texxform.m_tex_coord_type == TexCoordXForm::FIXED_COORD)
      {
        // Use provided texture coordinates as is.
      }
    }
    else if (tex->Type().IsDerivedFromType (IOpenGLRectangleTexture::StaticObjectType) )
    {
      if (texxform.m_tex_coord_type == TexCoordXForm::OFFSET_SCALE_COORD)
      {
        texxform.u0 = t_int32 (texxform.uoffset * tex_width);
        texxform.v0 = t_int32 (texxform.voffset * tex_height);
        texxform.u1 = texxform.u0 + tex_width * texxform.uscale;
        texxform.v1 = texxform.v0 + tex_height * texxform.vscale;
      }
      else if (texxform.m_tex_coord_type == TexCoordXForm::OFFSET_COORD)
      {
        texxform.u0 = texxform.uoffset;
        texxform.v0 = texxform.voffset;
        texxform.u1 = texxform.u0 + quad_width;
        texxform.v1 = texxform.v0 + quad_height;
      }
      else if (texxform.m_tex_coord_type == TexCoordXForm::NORMALIZED_COORD)
      {
        texxform.u0 *= (float) tex_width;
        texxform.v0 *= (float) tex_height;
        texxform.u1 *= (float) tex_width;
        texxform.v1 *= (float) tex_height;
      }
      else if (texxform.m_tex_coord_type == TexCoordXForm::UNNORMALIZED_COORD || texxform.m_tex_coord_type == TexCoordXForm::FIXED_COORD)
      {
        // Use provided texture coordinates as is.
      }
    }

    if (texxform.flip_u_coord)
    {
      float temp = texxform.u0;
      texxform.u0 = texxform.u1;
      texxform.u1 = temp;
    }

    if (texxform.flip_v_coord)
    {
      float temp = texxform.v0;
      texxform.v0 = texxform.v1;
      texxform.v1 = temp;
    }

    if (tex->Type().IsDerivedFromType (IOpenGLRectangleTexture::StaticObjectType) )
    {
      // A chance to avoid some potential errors! Rectangle textures support only GL_CLAMP, GL_CLAMP_TO_EDGE, and GL_CLAMP_TO_BORDER.
      // See http://www.opengl.org/registry/specs/ARB/texture_rectangle.txt
      if(texxform.uwrap != TEXWRAP_CLAMP ||
        texxform.uwrap != TEXWRAP_CLAMP_TO_EDGE ||
        texxform.uwrap != TEXWRAP_CLAMP_TO_BORDER ||
        texxform.vwrap != TEXWRAP_CLAMP ||
        texxform.vwrap != TEXWRAP_CLAMP_TO_EDGE ||
        texxform.vwrap != TEXWRAP_CLAMP_TO_BORDER)
      {
        texxform.uwrap = TEXWRAP_CLAMP;
        texxform.vwrap = TEXWRAP_CLAMP;
      }
    }
    tex->SetWrap (TexWrapGLMapping (texxform.uwrap), TexWrapGLMapping (texxform.vwrap), GL_CLAMP);
    tex->SetFiltering (TexFilterGLMapping (texxform.min_filter), TexFilterGLMapping (texxform.mag_filter) );
  }


  void GraphicsEngine::QRP_Color (int x, int y, int width, int height, const Color &color)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_Color (x, y, width, height, color, color, color, color);
    else
      QRP_ASM_Color (x, y, width, height, color, color, color, color);
#else
    QRP_GLSL_Color (x, y, width, height, color, color, color, color);
#endif
  }

  void GraphicsEngine::QRP_Color (int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_Color (x, y, width, height, c0, c1, c2, c3);
    else
      QRP_ASM_Color (x, y, width, height, c0, c1, c2, c3);
#else
    QRP_GLSL_Color (x, y, width, height, c0, c1, c2, c3);
#endif
  }

  void GraphicsEngine::QRP_1Tex (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color0)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_1Tex (x, y, width, height, DeviceTexture, texxform0, color0);
    else
      QRP_ASM_1Tex (x, y, width, height, DeviceTexture, texxform0, color0);
#else
    QRP_GLSL_1Tex (x, y, width, height, DeviceTexture, texxform0, color0);
#endif
  }

  // Render the texture alpha into RGB and modulated by a color.
  void GraphicsEngine::QRP_ColorModTexAlpha (int x, int y, int width, int height,
    ObjectPtr< IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform, const Color &color)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_ColorModTexAlpha (x, y, width, height, DeviceTexture, texxform, color);
    else
      QRP_ASM_ColorModTexAlpha (x, y, width, height, DeviceTexture, texxform, color);
#else
    QRP_GLSL_ColorModTexAlpha (x, y, width, height, DeviceTexture, texxform, color);
#endif
  }

  // Blend 2 textures together
  void GraphicsEngine::QRP_2Tex (int x, int y, int width, int height,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_2Tex (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1);
    else
      QRP_ASM_2Tex (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1);
#else
    QRP_GLSL_2Tex (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1);
#endif
  }


  void GraphicsEngine::QRP_2TexMod (int x, int y, int width, int height,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_2TexMod (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1);
    else
      QRP_ASM_2TexMod (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1);
#else
    QRP_GLSL_2TexMod (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1);
#endif
  }

  void GraphicsEngine::QRP_4Tex (int x, int y, int width, int height,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm &texxform2, const Color &color2,
    ObjectPtr<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm &texxform3, const Color &color3)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_4Tex (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1,
      DeviceTexture2, texxform2, color2, DeviceTexture3, texxform3, color3);
    else
      QRP_ASM_4Tex (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1,
      DeviceTexture2, texxform2, color2, DeviceTexture3, texxform3, color3);
#else
    QRP_GLSL_4Tex (x, y, width, height, DeviceTexture0, texxform0, color0, DeviceTexture1, texxform1, color1,
      DeviceTexture2, texxform2, color2, DeviceTexture3, texxform3, color3);
#endif
  }

  void GraphicsEngine::QRP_Triangle (int x0, int y0,
    int x1, int y1,
    int x2, int y2,
    Color c0)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_Triangle (x0, y0, x1, y1, x2, y2, c0, c0, c0);
    else
      QRP_ASM_Triangle (x0, y0, x1, y1, x2, y2, c0, c0, c0);
#else
    QRP_GLSL_Triangle (x0, y0, x1, y1, x2, y2, c0, c0, c0);
#endif
  }

  void GraphicsEngine::QRP_Triangle (int x0, int y0,
    int x1, int y1,
    int x2, int y2,
    Color c0, Color c1, Color c2)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_Triangle (x0, y0, x1, y1, x2, y2, c0, c1, c2);
    else
      QRP_ASM_Triangle (x0, y0, x1, y1, x2, y2, c0, c1, c2);
#else
    QRP_GLSL_Triangle (x0, y0, x1, y1, x2, y2, c0, c1, c2);
#endif
  }

  void GraphicsEngine::QRP_Line (int x0, int y0,
    int x1, int y1, Color c0)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_Line (x0, y0, x1, y1, c0, c0);
    else
      QRP_ASM_Line (x0, y0, x1, y1, c0, c0);
#else
    QRP_GLSL_Line (x0, y0, x1, y1, c0, c0);
#endif
  }

  void GraphicsEngine::QRP_Line (int x0, int y0,
    int x1, int y1, Color c0, Color c1)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_Line (x0, y0, x1, y1, c0, c1);
    else
      QRP_ASM_Line (x0, y0, x1, y1, c0, c1);
#else
    QRP_GLSL_Line (x0, y0, x1, y1, c0, c1);
#endif
  }

  void GraphicsEngine::QRP_QuadWireframe (int x0, int y0, int width, int height,
    Color c0,
    Color c1,
    Color c2,
    Color c3)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_QuadWireframe (x0, y0, width, height, c0, c1, c2, c3);
    else
      QRP_ASM_QuadWireframe (x0, y0, width, height, c0, c1, c2, c3);
#else
    QRP_GLSL_QuadWireframe (x0, y0, width, height, c0, c1, c2, c3);
#endif
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GetBlurTexture (
    int x, int y,
    int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0,
    float sigma, int num_pass)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      return QRP_GLSL_GetBlurTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0, sigma, num_pass);
    else
      return QRP_ASM_GetBlurTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0, sigma, num_pass);
#else
    return QRP_GLSL_GetBlurTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0, sigma, num_pass);
#endif
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GetAlphaTexture (
    int x, int y,
    int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      return QRP_GLSL_GetAlphaTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0);
    else
      return QRP_ASM_GetAlphaTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0);
#else
    return QRP_GLSL_GetAlphaTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0);
#endif
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GetColorMatrixTexture (
    int x, int y,
    int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0,
    Matrix4 color_matrix, Vector4 offset)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      return QRP_GLSL_GetColorMatrixTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0, color_matrix, offset);
    else
      return QRP_ASM_GetColorMatrixTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0, color_matrix, offset);
#else
    return QRP_GLSL_GetColorMatrixTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0, color_matrix, offset);
#endif
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GetComponentExponentiation (
    int x, int y,
    int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0,
    Vector4 exponent)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      return QRP_GLSL_GetComponentExponentiation (x, y, buffer_width, buffer_height, device_texture, texxform, c0, exponent);
    else
      return QRP_ASM_GetComponentExponentiation (x, y, buffer_width, buffer_height, device_texture, texxform, c0, exponent);
#else
    return QRP_GLSL_GetComponentExponentiation (x, y, buffer_width, buffer_height, device_texture, texxform, c0, exponent);
#endif
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GetLQBlur (
    int x, int y,
    int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      return QRP_GLSL_GetLQBlur (x, y, buffer_width, buffer_height, device_texture, texxform, c0);
    else
      return QRP_ASM_GetLQBlur (x, y, buffer_width, buffer_height, device_texture, texxform, c0);
#else
    return QRP_GLSL_GetLQBlur (x, y, buffer_width, buffer_height, device_texture, texxform, c0);
#endif
  }

  ObjectPtr<IOpenGLBaseTexture> GraphicsEngine::QRP_GetHQBlur (
    int x, int y, int buffer_width, int buffer_height,
    ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
    const Color& c0,
    float sigma, int num_pass)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath () && (GetGpuDevice ()->GetOpenGLMajorVersion () >= 3))
      return QRP_GLSL_GetHQBlur (x, y, buffer_width, buffer_height, device_texture, texxform, c0, sigma, num_pass);
    else
      return QRP_ASM_GetBlurTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0, sigma, num_pass);
#else
    return QRP_ASM_GetBlurTexture (x, y, buffer_width, buffer_height, device_texture, texxform, c0, sigma, num_pass);
#endif
  }

  void GraphicsEngine::QRP_DisturbedTexture (
    int x, int y, int width, int height,
    ObjectPtr<IOpenGLBaseTexture> distorsion_texture, TexCoordXForm &texxform0, const Color& c0,
    ObjectPtr<IOpenGLBaseTexture> src_device_texture, TexCoordXForm &texxform1, const Color& c1)
  {
#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
      QRP_GLSL_DisturbedTexture (x, y, width, height, distorsion_texture, texxform0, c0, src_device_texture, texxform1, c1);
    else
    {
      // NUXTODO
      //QRP_ASM_DisturbedTexture (x, y, width, height, distorsion_texture, texxform0, c0, src_device_texture, texxform1, c1);
    }
#else
    QRP_GLSL_DisturbedTexture (x, y, width, height, distorsion_texture, texxform0, c0, src_device_texture, texxform1, c1);
#endif
  }

}

