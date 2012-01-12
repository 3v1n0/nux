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


#include "GLResource.h"
#include "GLTextureStates.h"

namespace nux
{

  const struct TextureStateLookUpTable
  {
    TextureStateLookUpTable()
    {
      // setup render state map
#define UL_MAP(state__, default_value__, checked__)                          \
        default_texture_state[GFXTS_##state__].iValue     = default_value__;  \
        default_texture_state[GFXTS_##state__].Checked   = checked__;

#define UL_MAP_FLOAT(state__, default_value__, checked__)                    \
    default_texture_state[GFXTS_##state__].fValue     = default_value__;      \
    default_texture_state[GFXTS_##state__].Checked   = checked__;

      UL_MAP(ADDRESSU               ,           GL_REPEAT       ,       1);
      UL_MAP(ADDRESSV               ,           GL_REPEAT       ,       1);
      UL_MAP(ADDRESSW               ,           GL_REPEAT       ,       1);

      UL_MAP(MINFILTER              ,           GL_NEAREST      ,       1);
      UL_MAP(MAGFILTER              ,           GL_NEAREST      ,       1);
      UL_MAP(MIPFILTER              ,           GL_NEAREST      ,       1);

      UL_MAP(MIP_BASE_LEVEL         ,           0               ,       1);
      UL_MAP(MIP_MAX_LEVEL          ,           1000            ,       1);
      UL_MAP_FLOAT(MIN_LOD          ,           -1000           ,       1);
      UL_MAP_FLOAT(MAX_LOD          ,           +1000           ,       1);

      UL_MAP(BORDERCOLOR            ,           0x0             ,       1);

#undef UL_MAP
#undef UL_MAP_FLOAT
    };

    TextureStateMap default_texture_state[GFXTS_MAX_TEXTURESTATES];
  } s_TextureStateLUT;


  GLTextureStates::GLTextureStates(GLuint Type)
  {
    SetType(Type);
    Memcpy(&m_TextureStateChanges, &s_TextureStateLUT.default_texture_state, sizeof(m_TextureStateChanges));
  }

  GLTextureStates::~GLTextureStates()
  {

  }

  void GLTextureStates::SetType(GLuint Type)
  {
#ifndef NUX_OPENGLES_20
    nuxAssertMsg(
      (Type == GL_TEXTURE_1D) ||
      (Type == GL_TEXTURE_2D) ||
      (Type == GL_TEXTURE_RECTANGLE_ARB) ||
      (Type == GL_TEXTURE_3D) ||
      (Type == GL_TEXTURE_CUBE_MAP_ARB),
      "Error[GLTextureStates::GLTextureStates]: Invalid texture type.");
#else
    nuxAssertMsg(
      (Type == GL_TEXTURE_2D),
      "Error[GLTextureStates::GLTextureStates]: Invalid texture type.");
#endif

    m_Type = Type;
  }

  void GLTextureStates::ResetDefault()
  {

  }

  void GLTextureStates::ResetStateChangeToDefault()
  {

  }

  void GLTextureStates::SetRenderStates()
  {
    HW_SetFiltering();
    HW_SetWrap();
    HW_SetLOD();
    HW_SetMipLevel();
  }

#define SET_TS_VALUE(a, b)  if (a.iValue != b){(a).iValue = (b); (a).Dirty = true;}
#define TS_VALUE(a, b)      (a).iValue

#define SET_TS_VALUE_FLOAT(a, b)  if (a.fValue != b){(a).fValue = (b); (a).Dirty = true;}
#define TS_VALUE_FLOAT(a, b)      (a).fValue


  void GLTextureStates::HW_SetFiltering()
  {
    if (m_TextureStateChanges[GFXTS_MINFILTER].Dirty || m_TextureStateChanges[GFXTS_MAGFILTER].Dirty)
    {
      CHECKGL(glTexParameteri(m_Type, GL_TEXTURE_MIN_FILTER,   m_TextureStateChanges[GFXTS_MINFILTER].iValue ));
      CHECKGL(glTexParameteri(m_Type, GL_TEXTURE_MAG_FILTER,   m_TextureStateChanges[GFXTS_MAGFILTER].iValue));
      m_TextureStateChanges[GFXTS_MINFILTER].Dirty = false;
      m_TextureStateChanges[GFXTS_MAGFILTER].Dirty = false;
    }
  }

  void GLTextureStates::HW_SetWrap()
  {
    if (m_TextureStateChanges[GFXTS_ADDRESSU].Dirty ||
        m_TextureStateChanges[GFXTS_ADDRESSV].Dirty ||
        m_TextureStateChanges[GFXTS_ADDRESSW].Dirty)
    {
      CHECKGL(glTexParameteri(m_Type, GL_TEXTURE_WRAP_S,       m_TextureStateChanges[GFXTS_ADDRESSU].iValue));
      CHECKGL(glTexParameteri(m_Type, GL_TEXTURE_WRAP_T,       m_TextureStateChanges[GFXTS_ADDRESSV].iValue));
#ifndef NUX_OPENGLES_20
      CHECKGL(glTexParameteri(m_Type, GL_TEXTURE_WRAP_R,       m_TextureStateChanges[GFXTS_ADDRESSW].iValue));
#endif
      m_TextureStateChanges[GFXTS_ADDRESSU].Dirty = false;
      m_TextureStateChanges[GFXTS_ADDRESSV].Dirty = false;
      m_TextureStateChanges[GFXTS_ADDRESSW].Dirty = false;
    }
  }

  void GLTextureStates::HW_SetLOD()
  {
#ifndef NUX_OPENGLES_20
    if (m_Type == GL_TEXTURE_RECTANGLE_ARB)
    {
      // No support for mip LOP on rectangle texture.
      // ATI seems to not generate and Error.
      // Nvidia generates an error
      m_TextureStateChanges[GFXTS_MIN_LOD].Dirty = false;
      m_TextureStateChanges[GFXTS_MAX_LOD].Dirty = false;
      return;
    }

    if (m_TextureStateChanges[GFXTS_MIN_LOD].Dirty || m_TextureStateChanges[GFXTS_MAX_LOD].Dirty)
    {
      CHECKGL(glTexParameteri(m_Type, GL_TEXTURE_MIN_LOD,  m_TextureStateChanges[GFXTS_MIN_LOD].fValue));
      CHECKGL(glTexParameteri(m_Type, GL_TEXTURE_MAX_LOD,  m_TextureStateChanges[GFXTS_MAX_LOD].fValue));
      m_TextureStateChanges[GFXTS_MIN_LOD].Dirty = false;
      m_TextureStateChanges[GFXTS_MAX_LOD].Dirty = false;
    }
#endif
  }

  void GLTextureStates::HW_SetMipLevel()
  {
#ifndef NUX_OPENGLES_20
    if (m_TextureStateChanges[GFXTS_MIN_LOD].Dirty || m_TextureStateChanges[GFXTS_MAX_LOD].Dirty)
    {
      CHECKGL(glTexParameteri(m_Type, GL_TEXTURE_MIN_LOD,  m_TextureStateChanges[GFXTS_MIN_LOD].fValue));
      CHECKGL(glTexParameteri(m_Type, GL_TEXTURE_MAX_LOD,  m_TextureStateChanges[GFXTS_MAX_LOD].fValue));
      m_TextureStateChanges[GFXTS_MIN_LOD].Dirty = false;
      m_TextureStateChanges[GFXTS_MAX_LOD].Dirty = false;
    }
#endif
  }

  void GLTextureStates::HW_SetBorderColor()
  {

  }

  void GLTextureStates::SetFiltering(
    unsigned int MinFilter,
    unsigned int MagFilter
    /*,unsigned int MIP*/)
  {
    nuxAssertMsg(
      (MinFilter == GL_LINEAR) ||
      (MinFilter == GL_NEAREST) ||
      (MinFilter == GL_NEAREST_MIPMAP_NEAREST) ||
      (MinFilter == GL_LINEAR_MIPMAP_NEAREST) ||
      (MinFilter == GL_NEAREST_MIPMAP_LINEAR) ||
      (MinFilter == GL_LINEAR_MIPMAP_LINEAR),
      "Error[GLTextureStates::SetFiltering]: Invalid MinFilter state");

    nuxAssertMsg(
      (MagFilter == GL_LINEAR) ||
      (MagFilter == GL_NEAREST),
      "Error[GLTextureStates::SetFiltering]: Invalid MagFilter state");

    //    nuxAssertMsg(
    //        (MIP == GL_LINEAR) ||
    //        (MIP == GL_NEAREST),
    //        "Error[GLTextureStates::SetFiltering]: Invalid Mipmap Filter State");

#ifndef NUX_OPENGLES_20
    if (m_Type == GL_TEXTURE_RECTANGLE_ARB)
    {
      if ((MinFilter != GL_NEAREST) && (MinFilter != GL_LINEAR))
      {
        nuxError("[GLTextureStates::SetFiltering] Incorrect MinFilter for rectangle texture.");
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_MINFILTER], GL_LINEAR);
      }
      else
      {
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_MINFILTER], MinFilter);
      }

      if ((MagFilter != GL_NEAREST) && (MagFilter != GL_LINEAR))
      {
        nuxError("[GLTextureStates::SetFiltering] Incorrect MagFilter for rectangle texture.");
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_MAGFILTER], GL_LINEAR);
      }
      else
      {
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_MAGFILTER], MagFilter);
      }
    }
    else
    {
      SET_TS_VALUE(m_TextureStateChanges[GFXTS_MINFILTER], MinFilter);
      SET_TS_VALUE(m_TextureStateChanges[GFXTS_MAGFILTER], MagFilter);
    }
#else
    SET_TS_VALUE(m_TextureStateChanges[GFXTS_MINFILTER], MinFilter);
    SET_TS_VALUE(m_TextureStateChanges[GFXTS_MAGFILTER], MagFilter);
#endif

    //SET_TS_VALUE(m_TextureStateChanges[GFXTS_MIPFILTER], MIP);
  }

  void GLTextureStates::SetWrap(
    unsigned int U,
    unsigned int V,
    unsigned int W)
  {
#ifndef NUX_OPENGLES_20
    nuxAssertMsg(
      (U == GL_CLAMP) ||
      (U == GL_CLAMP_TO_EDGE) ||
      (U == GL_CLAMP_TO_BORDER) ||
      (U == GL_MIRRORED_REPEAT) ||
      (U == GL_MIRROR_CLAMP_EXT) ||
      (U == GL_MIRROR_CLAMP_TO_EDGE_EXT) ||
      (U == GL_MIRROR_CLAMP_TO_BORDER_EXT) ||
      (U == GL_REPEAT),
      "Error[GLTextureStates::SetWrap]: Invalid U Wrap State");

    nuxAssertMsg(
      (V == GL_CLAMP) ||
      (V == GL_CLAMP_TO_EDGE) ||
      (V == GL_CLAMP_TO_BORDER) ||
      (V == GL_MIRRORED_REPEAT) ||
      (V == GL_MIRROR_CLAMP_EXT) ||
      (V == GL_MIRROR_CLAMP_TO_EDGE_EXT) ||
      (V == GL_MIRROR_CLAMP_TO_BORDER_EXT) ||
      (V == GL_REPEAT),
      "Error[GLTextureStates::SetWrap]: Invalid V Wrap State");
    nuxAssertMsg(
      (W == GL_CLAMP) ||
      (W == GL_CLAMP_TO_EDGE) ||
      (W == GL_CLAMP_TO_BORDER) ||
      (W == GL_MIRRORED_REPEAT) ||
      (W == GL_MIRROR_CLAMP_EXT) ||
      (W == GL_MIRROR_CLAMP_TO_EDGE_EXT) ||
      (W == GL_MIRROR_CLAMP_TO_BORDER_EXT) ||
      (W == GL_REPEAT),
      "Error[GLTextureStates::SetWrap]: Invalid W Wrap State");

    if (m_Type == GL_TEXTURE_RECTANGLE_ARB)
    {
      if ((U != GL_CLAMP) && (U != GL_CLAMP_TO_BORDER) && (U != GL_CLAMP_TO_EDGE))
      {
        nuxError("[GLTextureStates::SetFiltering] Incorrect warp for rectangle texture.");
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSU], GL_CLAMP_TO_EDGE);
      }
      else
      {
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSU], U);
      }

      if ((V != GL_CLAMP) && (V != GL_CLAMP_TO_BORDER) && (V != GL_CLAMP_TO_EDGE))
      {
        nuxError("[GLTextureStates::SetFiltering] Incorrect warp for rectangle texture.");
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSV], GL_CLAMP_TO_EDGE);
      }
      else
      {
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSV], V);
      }

      if ((W != GL_CLAMP) && (W != GL_CLAMP_TO_BORDER) && (W != GL_CLAMP_TO_EDGE))
      {
        nuxError("[GLTextureStates::SetFiltering] Incorrect warp for rectangle texture.");
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSW], GL_CLAMP_TO_EDGE);
      }
      else
      {
        SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSW], W);
      }
    }
    else
    {
      SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSU], U);
      SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSV], V);
      SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSW], W);
    }
#else
    nuxAssertMsg(
      (U == GL_CLAMP) ||
      (U == GL_CLAMP_TO_EDGE) ||
      (U == GL_CLAMP_TO_BORDER) ||
      (U == GL_MIRRORED_REPEAT) ||
      (U == GL_REPEAT),
      "Error[GLTextureStates::SetWrap]: Invalid U Wrap State");

    nuxAssertMsg(
      (V == GL_CLAMP) ||
      (V == GL_CLAMP_TO_EDGE) ||
      (V == GL_CLAMP_TO_BORDER) ||
      (V == GL_MIRRORED_REPEAT) ||
      (V == GL_REPEAT),
      "Error[GLTextureStates::SetWrap]: Invalid V Wrap State");

    SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSU], U);
    SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSV], V);
#endif
  }

  void GLTextureStates::SetLOD(float MinLod,
                                float MaxLod)
  {
    SET_TS_VALUE_FLOAT(m_TextureStateChanges[GFXTS_MIN_LOD], MinLod);
    SET_TS_VALUE_FLOAT(m_TextureStateChanges[GFXTS_MAX_LOD], MaxLod);
  }

  void GLTextureStates::SetMipLevel(
    unsigned int MinMip,
    unsigned int MaxMip)
  {
    SET_TS_VALUE(m_TextureStateChanges[GFXTS_MIP_BASE_LEVEL], MinMip);
    SET_TS_VALUE(m_TextureStateChanges[GFXTS_MIP_MAX_LEVEL], MaxMip);
  }

  void GLTextureStates::SetBorderColor(
    float R,
    float G,
    float B,
    float A)
  {
    unsigned int r, g, b, a;
    r = 255 * Clamp(R, 0.0f, 1.0f);
    g = 255 * Clamp(G, 0.0f, 1.0f);
    b = 255 * Clamp(B, 0.0f, 1.0f);
    a = 255 * Clamp(A, 0.0f, 1.0f);
    unsigned int color = (unsigned int) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b));

    SET_TS_VALUE(m_TextureStateChanges[GFXTS_BORDERCOLOR], color);
  }

#undef SET_TS_VALUE
#undef TS_VALUE

#undef SET_TS_VALUE_FLOAT
#undef TS_VALUE_FLOAT

}
