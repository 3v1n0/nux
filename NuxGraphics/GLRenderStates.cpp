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
 *
 */


#include "GLResource.h"
#include "GLRenderStates.h"

namespace nux
{

  const struct StateLookUpTable
  {
    StateLookUpTable()
    {
      // setup render state map
#define UL_MAP(state__, default_value__, checked__)                         \
        default_render_state[GFXRS_##state__].iValue     = default_value__;      \
        default_render_state[GFXRS_##state__].Checked   = checked__;

#define UL_MAP_FLOAT(state__, default_value__, checked__)                         \
    default_render_state[GFXRS_##state__].fValue     = default_value__;      \
    default_render_state[GFXRS_##state__].Checked   = checked__;            \
 

#ifndef NUX_OPENGLES_20
      UL_MAP(FRONT_POLYGONMODE            ,           GL_FILL         ,       1);
      UL_MAP(BACK_POLYGONMODE             ,           GL_FILL         ,       1);
#endif
      UL_MAP(CULLFACEENABLE               ,           GL_FALSE        ,       1);
      UL_MAP(CULLFACE                     ,           GL_BACK         ,       1);
      UL_MAP(FRONTFACE                    ,           GL_CCW          ,       1);

      UL_MAP(SCISSORTESTENABLE            ,           GL_FALSE        ,       1);

      UL_MAP(ZTESTENABLE                  ,           GL_FALSE        ,       1);
      UL_MAP(ZWRITEENABLE                 ,           GL_TRUE         ,       1);
      UL_MAP(ZFUNC                        ,           GL_LESS         ,       1);
      UL_MAP_FLOAT(ZNEAR                  ,           static_cast<unsigned int> (0.0f)            ,       1);
      UL_MAP_FLOAT(ZFAR                   ,           static_cast<unsigned int> (1.0f)            ,       1);

      UL_MAP(ALPHABLENDENABLE             ,           GL_FALSE        ,       1);
      UL_MAP(BLENDOP                      ,           GL_FUNC_ADD     ,       1);
      UL_MAP(BLENDOPALPHA                 ,           GL_FUNC_ADD     ,       1);
      UL_MAP(SRCBLEND                     ,           GL_ONE          ,       1);
      UL_MAP(DESTBLEND                    ,           GL_ZERO         ,       1);
      UL_MAP(SRCBLENDALPHA                ,           GL_ONE          ,       1);
      UL_MAP(DESTBLENDALPHA               ,           GL_ZERO         ,       1);

      UL_MAP(ALPHATESTENABLE              ,           GL_FALSE        ,       1);
      UL_MAP(ALPHATESTREF                 ,           0x0             ,       1);
      UL_MAP(ALPHATESTFUNC                ,           GL_ALWAYS       ,       1);

      UL_MAP(STENCILENABLE                ,           GL_FALSE        ,       1);
      UL_MAP(TWOSIDEDSTENCILENABLE        ,           GL_FALSE        ,       1);
      UL_MAP(FRONT_STENCILWRITEMASK       ,           0xFFFFFFFF      ,       1);
      UL_MAP(BACK_STENCILWRITEMASK        ,           0xFFFFFFFF      ,       1);

      UL_MAP(FRONT_STENCILFUNC            ,           GL_ALWAYS       ,       1);
      UL_MAP(FRONT_STENCILREF             ,           0x0             ,       1);
      UL_MAP(FRONT_STENCILMASK            ,           0xFF            ,       1);
      UL_MAP(FRONT_STENCILFAIL            ,           GL_KEEP         ,       1);
      UL_MAP(FRONT_STENCILZFAIL           ,           GL_KEEP         ,       1);
      UL_MAP(FRONT_STENCILZPASS           ,           GL_KEEP         ,       1);

      UL_MAP(BACK_STENCILFUNC             ,           GL_ALWAYS       ,       1);
      UL_MAP(BACK_STENCILREF              ,           0x0             ,       1);
      UL_MAP(BACK_STENCILMASK             ,           0xFF            ,       1);
      UL_MAP(BACK_STENCILFAIL             ,           GL_KEEP         ,       1);
      UL_MAP(BACK_STENCILZFAIL            ,           GL_KEEP         ,       1);
      UL_MAP(BACK_STENCILZPASS            ,           GL_KEEP         ,       1);


      UL_MAP(LINESMOOTHENABLE             ,           GL_FALSE        ,       1);
      UL_MAP(LINEWIDTH                    ,           1               ,       1);
      UL_MAP(LINEHINT                     ,           GL_FASTEST      ,       1);

      UL_MAP(COLORWRITEENABLE_R           ,           GL_TRUE         ,       1);
      UL_MAP(COLORWRITEENABLE_G           ,           GL_TRUE         ,       1);
      UL_MAP(COLORWRITEENABLE_B           ,           GL_TRUE         ,       1);
      UL_MAP(COLORWRITEENABLE_A           ,           GL_TRUE         ,       1);

#undef UL_MAP
#undef UL_MAP_FLOAT
    };

    RenderStateMap default_render_state[GFXRS_MAX_RENDERSTATES];
    RenderStateMap sampler_state_map[GFXSS_MAX_SAMPLERSTATES];

  } s_StateLUT;


  GpuRenderStates::GpuRenderStates(GpuBrand board, GpuInfo* info)
  {
    gpu_brand_ = board;
    gpu_info_ = info;
    Memcpy(&render_state_changes_, &s_StateLUT.default_render_state, sizeof(render_state_changes_));
  }

  GpuRenderStates::~GpuRenderStates()
  {

  }

  void GpuRenderStates::ResetDefault()
  {
    HW__EnableCulling( s_StateLUT.default_render_state[GFXRS_CULLFACEENABLE].iValue );
    HW__SetFrontFace( s_StateLUT.default_render_state[GFXRS_FRONTFACE].iValue );
    HW__SetCullFace( s_StateLUT.default_render_state[GFXRS_CULLFACE].iValue );

    HW__SetDepthMask( s_StateLUT.default_render_state[GFXRS_ZWRITEENABLE].iValue );
    HW__SetDepthFunc( s_StateLUT.default_render_state[GFXRS_ZFUNC].iValue );
    HW__SetEnableDepthTest( s_StateLUT.default_render_state[GFXRS_ZTESTENABLE].iValue );
    HW__SetDepthRange( s_StateLUT.default_render_state[GFXRS_ZNEAR].fValue, s_StateLUT.default_render_state[GFXRS_ZFAR].fValue );

    HW__EnableAlphaBlend( s_StateLUT.default_render_state[GFXRS_ALPHABLENDENABLE].iValue );
    HW__SetAlphaBlendOp( s_StateLUT.default_render_state[GFXRS_BLENDOP].iValue, s_StateLUT.default_render_state[GFXRS_BLENDOPALPHA].iValue );
    HW__SetSeparateAlphaBlendFactors(
      s_StateLUT.default_render_state[GFXRS_SRCBLEND].iValue, s_StateLUT.default_render_state[GFXRS_DESTBLEND].iValue,
      s_StateLUT.default_render_state[GFXRS_SRCBLENDALPHA].iValue, s_StateLUT.default_render_state[GFXRS_DESTBLENDALPHA ].iValue );

#ifndef NUX_OPENGLES_20
    HW__EnableAlphaTest( s_StateLUT.default_render_state[GFXRS_ALPHATESTENABLE].iValue );
    HW__SetAlphaTestFunc( s_StateLUT.default_render_state[GFXRS_ALPHATESTFUNC].iValue, s_StateLUT.default_render_state[GFXRS_ALPHATESTREF].iValue );
#endif

    HW__EnableStencil( s_StateLUT.default_render_state[GFXRS_STENCILENABLE].iValue );
    HW__SetStencilFunc(
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILFUNC].iValue,
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILREF].iValue,
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILMASK].iValue);
    HW__SetStencilOp(
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILFAIL].iValue,
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILZFAIL].iValue,
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILZPASS].iValue);

#ifndef NUX_OPENGLES_20
    HW__EnableLineSmooth(s_StateLUT.default_render_state[GFXRS_LINESMOOTHENABLE].iValue);
#endif
    HW__SetLineWidth(s_StateLUT.default_render_state[GFXRS_LINEWIDTH].iValue, s_StateLUT.default_render_state[GFXRS_LINEHINT].iValue);

#if 0
    HW__EnableTwoSidedStencil( s_StateLUT.default_render_state[GFXRS_TWOSIDEDSTENCILENABLE].iValue );

    HW__SetFrontFaceStencilFunc(
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILFUNC].iValue,
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILREF].iValue,
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILMASK].iValue );
    HW__SetFrontFaceStencilOp(
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILFAIL].iValue,
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILZFAIL].iValue,
      s_StateLUT.default_render_state[GFXRS_FRONT_STENCILZPASS].iValue );

    HW__SetBackFaceStencilFunc(
      s_StateLUT.default_render_state[GFXRS_BACK_STENCILFUNC].iValue,
      s_StateLUT.default_render_state[GFXRS_BACK_STENCILREF].iValue,
      s_StateLUT.default_render_state[GFXRS_BACK_STENCILMASK].iValue );
    HW__SetBackFaceStencilOp(
      s_StateLUT.default_render_state[GFXRS_BACK_STENCILFAIL].iValue,
      s_StateLUT.default_render_state[GFXRS_BACK_STENCILZFAIL].iValue,
      s_StateLUT.default_render_state[GFXRS_BACK_STENCILZPASS].iValue );
#endif

    HW__EnableScissor( s_StateLUT.default_render_state[GFXRS_SCISSORTESTENABLE].iValue );

  }

  void GpuRenderStates::SubmitChangeStates()
  {
    HW__EnableCulling( render_state_changes_[GFXRS_CULLFACEENABLE].iValue );
    HW__SetFrontFace( render_state_changes_[GFXRS_FRONTFACE].iValue );
    HW__SetCullFace( render_state_changes_[GFXRS_CULLFACE].iValue );

    HW__SetDepthMask( render_state_changes_[GFXRS_ZWRITEENABLE].iValue );
    HW__SetDepthFunc( render_state_changes_[GFXRS_ZFUNC].iValue );
    HW__SetEnableDepthTest( render_state_changes_[GFXRS_ZTESTENABLE].iValue );
    HW__SetDepthRange( render_state_changes_[GFXRS_ZNEAR].fValue, render_state_changes_[GFXRS_ZFAR].fValue );

    HW__EnableAlphaBlend( render_state_changes_[GFXRS_ALPHABLENDENABLE].iValue );
    HW__SetAlphaBlendOp( render_state_changes_[GFXRS_BLENDOP].iValue, render_state_changes_[GFXRS_BLENDOPALPHA].iValue );
    HW__SetSeparateAlphaBlendFactors(
      render_state_changes_[GFXRS_SRCBLEND].iValue, render_state_changes_[GFXRS_DESTBLEND].iValue,
      render_state_changes_[GFXRS_SRCBLENDALPHA].iValue, render_state_changes_[GFXRS_DESTBLENDALPHA ].iValue );

#ifndef NUX_OPENGLES_20
    HW__EnableAlphaTest( render_state_changes_[GFXRS_ALPHATESTENABLE].iValue );
    HW__SetAlphaTestFunc( render_state_changes_[GFXRS_ALPHATESTFUNC].iValue, render_state_changes_[GFXRS_ALPHATESTREF].iValue );
#endif

    HW__EnableStencil( render_state_changes_[GFXRS_STENCILENABLE].iValue );

    HW__SetStencilFunc(
      render_state_changes_[GFXRS_FRONT_STENCILFUNC].iValue,
      render_state_changes_[GFXRS_FRONT_STENCILREF].iValue,
      render_state_changes_[GFXRS_FRONT_STENCILMASK].iValue);
    HW__SetStencilOp(
      render_state_changes_[GFXRS_FRONT_STENCILFAIL].iValue,
      render_state_changes_[GFXRS_FRONT_STENCILZFAIL].iValue,
      render_state_changes_[GFXRS_FRONT_STENCILZPASS].iValue);

#ifndef NUX_OPENGLES_20
    HW__EnableLineSmooth(render_state_changes_[GFXRS_LINESMOOTHENABLE].iValue);
#endif
    HW__SetLineWidth(render_state_changes_[GFXRS_LINEWIDTH].iValue, render_state_changes_[GFXRS_LINEHINT].iValue);

#if 0
    HW__EnableTwoSidedStencil( s_StateLUT.default_render_state[GFXRS_TWOSIDEDSTENCILENABLE].iValue );

    HW__SetFrontFaceStencilFunc(
      render_state_changes_[GFXRS_FRONT_STENCILFUNC].iValue,
      render_state_changes_[GFXRS_FRONT_STENCILREF].iValue,
      render_state_changes_[GFXRS_FRONT_STENCILMASK].iValue );
    HW__SetFrontFaceStencilOp(
      render_state_changes_[GFXRS_FRONT_STENCILFAIL].iValue,
      render_state_changes_[GFXRS_FRONT_STENCILZFAIL].iValue,
      render_state_changes_[GFXRS_FRONT_STENCILZPASS].iValue );

    HW__SetBackFaceStencilFunc(
      render_state_changes_[GFXRS_BACK_STENCILFUNC].iValue,
      render_state_changes_[GFXRS_BACK_STENCILREF].iValue,
      render_state_changes_[GFXRS_BACK_STENCILMASK].iValue );
    HW__SetBackFaceStencilOp(
      render_state_changes_[GFXRS_BACK_STENCILFAIL].iValue,
      render_state_changes_[GFXRS_BACK_STENCILZFAIL].iValue,
      render_state_changes_[GFXRS_BACK_STENCILZPASS].iValue );
#endif

    HW__EnableScissor( render_state_changes_[GFXRS_SCISSORTESTENABLE].iValue );

  }

  void GpuRenderStates::ResetStateChangeToDefault()
  {
    for (unsigned int i = 0; i < GFXRS_MAX_RENDERSTATES; i++)
    {
      if (render_state_changes_[i].Checked &&
          ((render_state_changes_[i].iValue != s_StateLUT.default_render_state[i].iValue) ||
            (render_state_changes_[i].fValue != s_StateLUT.default_render_state[i].fValue)))
      {
        render_state_changes_[i].iValue = s_StateLUT.default_render_state[i].iValue;
        render_state_changes_[i].fValue = s_StateLUT.default_render_state[i].fValue;
      }
    }
  }

  void GpuRenderStates::CheckStateChange()
  {
    for (unsigned int i = 0; i < GFXRS_MAX_RENDERSTATES; i++)
    {
      if (render_state_changes_[i].Checked &&
          ((render_state_changes_[i].iValue != s_StateLUT.default_render_state[i].iValue) ||
            (render_state_changes_[i].fValue != s_StateLUT.default_render_state[i].fValue)))
      {
        render_state_changes_[i].iValue = s_StateLUT.default_render_state[i].iValue;
        render_state_changes_[i].fValue = s_StateLUT.default_render_state[i].fValue;
        nuxError("[GpuRenderStates::Check] Render state doesn't have default value");
      }
    }
  }
}

