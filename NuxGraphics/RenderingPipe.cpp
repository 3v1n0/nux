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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "GLResource.h"
#include "IOpenGLBaseTexture.h"
#include "IOpenGLTexture2D.h"
#include "IOpenGLRectangleTexture.h"
#include "RenderingPipe.h"

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
    {ROPBLEND_ZERO,                             ROPBLEND_ZERO},
    {ROPBLEND_ONE,                              ROPBLEND_ONE},
    {ROPBLEND_SRC_COLOR,                        ROPBLEND_SRC_COLOR},
    {ROPBLEND_ONE_MINUS_SRC_COLOR,              ROPBLEND_ONE_MINUS_SRC_COLOR},
    {ROPBLEND_DST_COLOR,                        ROPBLEND_DST_COLOR},
    {ROPBLEND_ONE_MINUS_DST_COLOR,              ROPBLEND_ONE_MINUS_DST_COLOR},
    {ROPBLEND_SRC_ALPHA,                        ROPBLEND_SRC_ALPHA},
    {ROPBLEND_ONE_MINUS_SRC_ALPHA,              ROPBLEND_ONE_MINUS_SRC_ALPHA},
    {ROPBLEND_DST_ALPHA,                        ROPBLEND_DST_ALPHA},
    {ROPBLEND_ONE_MINUS_DST_ALPHA,              ROPBLEND_ONE_MINUS_DST_ALPHA},
    {ROPBLEND_CONSTANT_COLOR,                   ROPBLEND_CONSTANT_COLOR},
    {ROPBLEND_ONE_MINUS_CONSTANT_COLOR,         ROPBLEND_ONE_MINUS_CONSTANT_COLOR},
    {ROPBLEND_CONSTANT_ALPHA,                   ROPBLEND_CONSTANT_ALPHA},
    {ROPBLEND_ONE_MINUS_CONSTANT_ALPHA,         ROPBLEND_ONE_MINUS_CONSTANT_ALPHA},
    {ROPBLEND_SRC_ALPHA_SATURATE,               ROPBLEND_SRC_ALPHA_SATURATE},
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

  void QRP_Compute_Texture_Coord (t_int32 quad_width, t_int32 quad_height, TRefGL<IOpenGLBaseTexture> tex, TexCoordXForm &texxform)
  {
    float tex_width = tex->GetWidth();
    float tex_height = tex->GetHeight();

    if (tex->Type().IsDerivedFromType (IOpenGLTexture2D::StaticObjectType) )
    {
      if (texxform.m_tex_coord_type == TexCoordXForm::OFFSET_SCALE_COORD)
      {
        texxform.u0 = texxform.uoffset;
        texxform.v0 = texxform.voffset;
        texxform.u1 = texxform.u0 + 1.0f / texxform.uscale;
        texxform.v1 = texxform.v0 + 1.0f / texxform.vscale;
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
        texxform.u1 = texxform.u0 + tex_width * 1.0f / texxform.uscale;
        texxform.v1 = texxform.v0 + tex_height * 1.0f / texxform.vscale;
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

    tex->SetWrap (TexWrapGLMapping (texxform.uwrap), TexWrapGLMapping (texxform.vwrap), GL_CLAMP);
    tex->SetFiltering (TexFilterGLMapping (texxform.min_filter), TexFilterGLMapping (texxform.mag_filter) );
  }

}
