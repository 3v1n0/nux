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


#ifndef GLTEXTURESTATES_H
#define GLTEXTURESTATES_H

#include "NuxCore/NuxCore.h"

namespace nux
{

  enum
  {
    GFXTS_ADDRESSU,                          // GL_REPEAT
    GFXTS_ADDRESSV,                          // GL_REPEAT
    GFXTS_ADDRESSW,                          // GL_REPEAT

    GFXTS_MINFILTER,                         // GL_NEAREST
    GFXTS_MAGFILTER,                         // GL_NEAREST
    GFXTS_MIPFILTER,                         // GL_NEAREST

    GFXTS_MIP_BASE_LEVEL,                    // 0
    GFXTS_MIP_MAX_LEVEL,                     // 1000
    GFXTS_MIN_LOD,                           // -1000
    GFXTS_MAX_LOD,                           // +1000

    GFXTS_BORDERCOLOR,                       // 0x00000000


    GFXTS_MAX_TEXTURESTATES,
  };

  struct TextureStateMap
  {
    bool           Checked;
    unsigned int    State;
    unsigned int    Dirty;
    unsigned int    iValue;
    float           fValue;
    TextureStateMap()
    {
      Dirty = true;
    }
  };

  class GpuDevice;
  class GLTextureStates
  {
  public:
    GLTextureStates(GLuint Type = GL_TEXTURE_2D);
    ~GLTextureStates();

    void ResetDefault();
    void ResetStateChangeToDefault();
    void SetRenderStates();

    void SetType(GLuint Type);

    void SetFiltering(
      unsigned int MIN = GL_NEAREST,
      unsigned int MAG = GL_NEAREST/*,
        unsigned int MIP = GL_NEAREST*/);

    void SetWrap(
      unsigned int U = GL_REPEAT,
      unsigned int V = GL_REPEAT,
      unsigned int W = GL_REPEAT);

    void SetLOD(
      float MinLod = -1000.0f,
      float MaxLod = +1000.0f);

    void SetMipLevel(
      unsigned int MinMip = 0,
      unsigned int MaxMip = 1000);

    void SetBorderColor(
      float R,
      float G,
      float B,
      float A);

  private:
    void HW_SetFiltering();
    void HW_SetWrap();
    void HW_SetLOD();
    void HW_SetMipLevel();
    void HW_SetBorderColor();

    GLuint m_Type;
    TextureStateMap m_TextureStateChanges[GFXTS_MAX_TEXTURESTATES];

    friend class GpuDevice;
  };

// #define SET_TS_VALUE(a, b)  (a).iValue = (b); (a).Dirty = true;
// #define TS_VALUE(a, b)      (a).iValue
//
// #define SET_TS_VALUE_FLOAT(a, b)  (a).fValue = (b); (a).Dirty = true;
// #define TS_VALUE_FLOAT(a, b)      (a).fValue
//
//
// inline void GLTextureStates::SetFiltering(
//                          unsigned int MIN,
//                          unsigned int MAG
//                          /*,unsigned int MIP*/)
// {
//     nuxAssertMsg(
//         (MIN == GL_LINEAR) ||
//         (MIN == GL_NEAREST) ||
//         (MIN == GL_NEAREST_MIPMAP_NEAREST) ||
//         (MIN == GL_LINEAR_MIPMAP_NEAREST) ||
//         (MIN == GL_NEAREST_MIPMAP_LINEAR) ||
//         (MIN == GL_LINEAR_MIPMAP_LINEAR),
//         "Error[GLTextureStates::SetFiltering]: Invalid Min. Filter State");
//
//     nuxAssertMsg(
//         (MAG == GL_LINEAR) ||
//         (MAG == GL_NEAREST),
//         "Error[GLTextureStates::SetFiltering]: Invalid Mag. Filter State");
//
// //    nuxAssertMsg(
// //        (MIP == GL_LINEAR) ||
// //        (MIP == GL_NEAREST),
// //        "Error[GLTextureStates::SetFiltering]: Invalid Mipmap Filter State");
//
//     SET_TS_VALUE(m_TextureStateChanges[GFXTS_MINFILTER], MIN);
//     SET_TS_VALUE(m_TextureStateChanges[GFXTS_MAGFILTER], MAG);
//     //SET_TS_VALUE(m_TextureStateChanges[GFXTS_MIPFILTER], MIP);
// }
//
// inline void GLTextureStates::SetWrap(
//                     unsigned int U,
//                     unsigned int V,
//                     unsigned int W)
// {
//     nuxAssertMsg(
//         (U == GL_CLAMP) ||
//         (U == GL_CLAMP_TO_EDGE) ||
//         (U == GL_REPEAT),
//         "Error[GLTextureStates::SetWrap]: Invalid U Wrap State");
//     nuxAssertMsg(
//         (V == GL_CLAMP) ||
//         (V == GL_CLAMP_TO_EDGE) ||
//         (V == GL_REPEAT),
//         "Error[GLTextureStates::SetWrap]: Invalid V Wrap State");
//     nuxAssertMsg(
//         (W == GL_CLAMP) ||
//         (W == GL_CLAMP_TO_EDGE) ||
//         (W == GL_REPEAT),
//         "Error[GLTextureStates::SetWrap]: Invalid W Wrap State");
//
//     SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSU], U);
//     SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSV], V);
//     SET_TS_VALUE(m_TextureStateChanges[GFXTS_ADDRESSW], W);
//
//
// }
//
// inline void GLTextureStates::SetLOD(float MinLod,
//                                     float MaxLod)
// {
//     SET_TS_VALUE_FLOAT(m_TextureStateChanges[GFXTS_MIN_LOD], MinLod);
//     SET_TS_VALUE_FLOAT(m_TextureStateChanges[GFXTS_MAX_LOD], MaxLod);
// }
//
// inline void GLTextureStates::SetMipLevel(
//                         unsigned int MinMip,
//                         unsigned int MaxMip)
// {
//     SET_TS_VALUE(m_TextureStateChanges[GFXTS_MIP_BASE_LEVEL], MinMip);
//     SET_TS_VALUE(m_TextureStateChanges[GFXTS_MIP_MAX_LEVEL], MaxMip);
// }
//
// inline void GLTextureStates::SetBorderColor(
//                            float R,
//                            float G,
//                            float B,
//                            float A)
// {
//     unsigned int r, g, b, a;
//     r = 255 * Clamp(R, 0.0f, 1.0f);
//     g = 255 * Clamp(G, 0.0f, 1.0f);
//     b = 255 * Clamp(B, 0.0f, 1.0f);
//     a = 255 * Clamp(A, 0.0f, 1.0f);
//     unsigned int color = (unsigned int) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b));
//
//     SET_TS_VALUE(m_TextureStateChanges[GFXTS_BORDERCOLOR], color);
// }
//
// #undef SET_TS_VALUE
// #undef TS_VALUE
//
// #undef SET_TS_VALUE_FLOAT
// #undef TS_VALUE_FLOAT

}

#endif // GLTEXTURESTATES_H

