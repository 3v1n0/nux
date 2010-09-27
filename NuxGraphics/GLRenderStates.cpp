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
#include "GLRenderStates.h"

namespace nux { //NUX_NAMESPACE_BEGIN

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


        UL_MAP(FRONT_POLYGONMODE            ,           GL_FILL         ,       1);
        UL_MAP(BACK_POLYGONMODE             ,           GL_FILL         ,       1);
        UL_MAP(CULLFACEENABLE               ,           GL_FALSE        ,       1);
        UL_MAP(CULLFACE                     ,           GL_BACK         ,       1);
        UL_MAP(FRONTFACE                    ,           GL_CCW          ,       1);

        UL_MAP(SCISSORTESTENABLE            ,           GL_FALSE        ,       1);
        UL_MAP(FOGENABLE                    ,           GL_FALSE        ,       1);

        UL_MAP(ZTESTENABLE                  ,           GL_FALSE        ,       1);
        UL_MAP(ZWRITEENABLE                 ,           GL_TRUE         ,       1);
        UL_MAP(ZFUNC                        ,           GL_LESS         ,       1);
        UL_MAP_FLOAT(ZNEAR                  ,           static_cast<t_u32>(0.0f)            ,       1);
        UL_MAP_FLOAT(ZFAR                   ,           static_cast<t_u32>(1.0f)            ,       1);

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


        UL_MAP(POINTSMOOTHENABLE            ,           GL_FALSE        ,       1);
        UL_MAP(LINESMOOTHENABLE             ,           GL_FALSE        ,       1);
        UL_MAP(POINTSIZE                    ,           1               ,       1);
        UL_MAP(LINEWIDTH                    ,           1               ,       1);
        UL_MAP(POINTHINT                    ,           GL_FASTEST      ,       1);
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


GLRenderStates::GLRenderStates(eGraphicsBoardVendor board)
{
    m_BoardVendor = board;
    Memcpy(&m_RenderStateChanges, &s_StateLUT.default_render_state, sizeof(m_RenderStateChanges));
}

GLRenderStates::~GLRenderStates()
{

}

void GLRenderStates::ResetDefault()
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


    HW__EnableAlphaTest( s_StateLUT.default_render_state[GFXRS_ALPHATESTENABLE].iValue );
    HW__SetAlphaTestFunc( s_StateLUT.default_render_state[GFXRS_ALPHATESTFUNC].iValue, s_StateLUT.default_render_state[GFXRS_ALPHATESTREF].iValue );


    HW__EnableStencil( s_StateLUT.default_render_state[GFXRS_STENCILENABLE].iValue );
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

    HW__EnableScissor( s_StateLUT.default_render_state[GFXRS_SCISSORTESTENABLE].iValue );

    HW__EnableFog( s_StateLUT.default_render_state[GFXRS_FOGENABLE].iValue );
}

void GLRenderStates::SubmitChangeStates()
{
    HW__EnableCulling( m_RenderStateChanges[GFXRS_CULLFACEENABLE].iValue );
    HW__SetFrontFace( m_RenderStateChanges[GFXRS_FRONTFACE].iValue );
    HW__SetCullFace( m_RenderStateChanges[GFXRS_CULLFACE].iValue );

    HW__SetDepthMask( m_RenderStateChanges[GFXRS_ZWRITEENABLE].iValue );
    HW__SetDepthFunc( m_RenderStateChanges[GFXRS_ZFUNC].iValue );
    HW__SetEnableDepthTest( m_RenderStateChanges[GFXRS_ZTESTENABLE].iValue );
    HW__SetDepthRange( m_RenderStateChanges[GFXRS_ZNEAR].fValue, m_RenderStateChanges[GFXRS_ZFAR].fValue );

    HW__EnableAlphaBlend( m_RenderStateChanges[GFXRS_ALPHABLENDENABLE].iValue );
    HW__SetAlphaBlendOp( m_RenderStateChanges[GFXRS_BLENDOP].iValue, m_RenderStateChanges[GFXRS_BLENDOPALPHA].iValue );
    HW__SetSeparateAlphaBlendFactors(
        m_RenderStateChanges[GFXRS_SRCBLEND].iValue, m_RenderStateChanges[GFXRS_DESTBLEND].iValue,
        m_RenderStateChanges[GFXRS_SRCBLENDALPHA].iValue, m_RenderStateChanges[GFXRS_DESTBLENDALPHA ].iValue );


    HW__EnableAlphaTest( m_RenderStateChanges[GFXRS_ALPHATESTENABLE].iValue );
    HW__SetAlphaTestFunc( m_RenderStateChanges[GFXRS_ALPHATESTFUNC].iValue, m_RenderStateChanges[GFXRS_ALPHATESTREF].iValue );


    HW__EnableStencil( m_RenderStateChanges[GFXRS_STENCILENABLE].iValue );
    HW__EnableTwoSidedStencil( s_StateLUT.default_render_state[GFXRS_TWOSIDEDSTENCILENABLE].iValue );

    HW__SetFrontFaceStencilFunc(
        m_RenderStateChanges[GFXRS_FRONT_STENCILFUNC].iValue,
        m_RenderStateChanges[GFXRS_FRONT_STENCILREF].iValue,
        m_RenderStateChanges[GFXRS_FRONT_STENCILMASK].iValue );
    HW__SetFrontFaceStencilOp(
        m_RenderStateChanges[GFXRS_FRONT_STENCILFAIL].iValue,
        m_RenderStateChanges[GFXRS_FRONT_STENCILZFAIL].iValue,
        m_RenderStateChanges[GFXRS_FRONT_STENCILZPASS].iValue );

    HW__SetBackFaceStencilFunc(
        m_RenderStateChanges[GFXRS_BACK_STENCILFUNC].iValue,
        m_RenderStateChanges[GFXRS_BACK_STENCILREF].iValue,
        m_RenderStateChanges[GFXRS_BACK_STENCILMASK].iValue );
    HW__SetBackFaceStencilOp(
        m_RenderStateChanges[GFXRS_BACK_STENCILFAIL].iValue,
        m_RenderStateChanges[GFXRS_BACK_STENCILZFAIL].iValue,
        m_RenderStateChanges[GFXRS_BACK_STENCILZPASS].iValue );

    HW__EnableScissor( m_RenderStateChanges[GFXRS_SCISSORTESTENABLE].iValue );

    HW__EnableFog( m_RenderStateChanges[GFXRS_FOGENABLE].iValue );

}

void GLRenderStates::ResetStateChangeToDefault()
{
    for(t_u32 i = 0; i < GFXRS_MAX_RENDERSTATES; i++)
    {
        if(m_RenderStateChanges[i].Checked &&
            ((m_RenderStateChanges[i].iValue != s_StateLUT.default_render_state[i].iValue) ||
            (m_RenderStateChanges[i].fValue != s_StateLUT.default_render_state[i].fValue) ))
        {
            m_RenderStateChanges[i].iValue = s_StateLUT.default_render_state[i].iValue;
            m_RenderStateChanges[i].fValue = s_StateLUT.default_render_state[i].fValue;
        }
    }
}

void GLRenderStates::CheckStateChange()
{
    for(t_u32 i = 0; i < GFXRS_MAX_RENDERSTATES; i++)
    {
        if(m_RenderStateChanges[i].Checked &&
            ((m_RenderStateChanges[i].iValue != s_StateLUT.default_render_state[i].iValue) ||
            (m_RenderStateChanges[i].fValue != s_StateLUT.default_render_state[i].fValue) ))
        {
            m_RenderStateChanges[i].iValue = s_StateLUT.default_render_state[i].iValue;
            m_RenderStateChanges[i].fValue = s_StateLUT.default_render_state[i].fValue;
            nuxError(TEXT("[GLRenderStates::Check] Render state doesn't have default value"));
        }
    }
}

void GLRenderStates::SetRenderStates(t_u32 rs, t_u32 value)
{
    #define RS_VALUE(a)      (a).iValue

    if (value != m_RenderStateChanges[rs].iValue)
    {
        m_RenderStateChanges[rs].iValue = static_cast<t_u32>(value);

        if(rs == GFXRS_ALPHATESTENABLE ||
            rs == GFXRS_ALPHATESTREF ||
            rs == GFXRS_ALPHATESTFUNC)
        {
            HW__EnableAlphaTest(RS_VALUE(m_RenderStateChanges[GFXRS_ALPHATESTENABLE]));
            HW__SetAlphaTestFunc(
                RS_VALUE(m_RenderStateChanges[GFXRS_ALPHATESTFUNC]),
                RS_VALUE(m_RenderStateChanges[GFXRS_ALPHATESTREF]));
        }

        if(rs == GFXRS_STENCILENABLE          ||
            rs == GFXRS_TWOSIDEDSTENCILENABLE ||
            rs == GFXRS_FRONT_STENCILFAIL           ||
            rs == GFXRS_FRONT_STENCILZFAIL          ||
            rs == GFXRS_FRONT_STENCILZPASS           ||
            rs == GFXRS_FRONT_STENCILFUNC           ||
            rs == GFXRS_FRONT_STENCILREF            ||
            rs == GFXRS_FRONT_STENCILMASK           ||
            rs == GFXRS_FRONT_STENCILWRITEMASK      ||
            rs == GFXRS_BACK_STENCILFAIL           ||
            rs == GFXRS_BACK_STENCILZFAIL          ||
            rs == GFXRS_BACK_STENCILZPASS           ||
            rs == GFXRS_BACK_STENCILFUNC           ||
            rs == GFXRS_BACK_STENCILREF            ||
            rs == GFXRS_BACK_STENCILMASK           ||
            rs == GFXRS_BACK_STENCILWRITEMASK)
        {
            HW__EnableStencil(RS_VALUE(m_RenderStateChanges[GFXRS_STENCILENABLE]));
            HW__EnableTwoSidedStencil(RS_VALUE(m_RenderStateChanges[GFXRS_TWOSIDEDSTENCILENABLE]));

            HW__SetFrontFaceStencilFunc(
                RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILFUNC]),
                RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILREF]),
                RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILMASK]));
            HW__SetFrontFaceStencilOp(
                RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILFAIL]),
                RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILZFAIL]),
                RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILZPASS]));
            HW__SetStencilFrontFaceWriteMask(
                RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILWRITEMASK]));

            HW__SetBackFaceStencilFunc(
                RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILFUNC]),
                RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILREF]),
                RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILMASK]));
            HW__SetBackFaceStencilOp(
                RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILFAIL]),
                RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILZFAIL]),
                RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILZPASS]));
            HW__SetStencilBackFaceWriteMask(
                RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILWRITEMASK]));
        }

        if(rs == GFXRS_ALPHABLENDENABLE   ||
            rs == GFXRS_BLENDOP           ||
            rs == GFXRS_BLENDOPALPHA      ||
            rs == GFXRS_SRCBLEND          ||
            rs == GFXRS_DESTBLEND         ||
            rs == GFXRS_SRCBLENDALPHA     ||
            rs == GFXRS_DESTBLENDALPHA)
        {
            HW__EnableAlphaBlend(
                RS_VALUE(m_RenderStateChanges[GFXRS_ALPHABLENDENABLE]));

            HW__SetSeparateAlphaBlendFactors(
                RS_VALUE(m_RenderStateChanges[GFXRS_SRCBLEND]),
                RS_VALUE(m_RenderStateChanges[GFXRS_DESTBLEND]),
                RS_VALUE(m_RenderStateChanges[GFXRS_SRCBLENDALPHA]),
                RS_VALUE(m_RenderStateChanges[GFXRS_DESTBLENDALPHA]));
            HW__SetAlphaBlendOp(
                RS_VALUE(m_RenderStateChanges[GFXRS_BLENDOP]),
                RS_VALUE(m_RenderStateChanges[GFXRS_BLENDOPALPHA]));
        }

        if(rs == GFXRS_COLORWRITEENABLE_R    ||
            rs == GFXRS_COLORWRITEENABLE_G   ||
            rs == GFXRS_COLORWRITEENABLE_B   ||
            rs == GFXRS_COLORWRITEENABLE_A)
        {
            HW__SetColorMask(
                RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_R]),
                RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_G]),
                RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_B]),
                RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_A]));
        }

        if(rs == GFXRS_ZTESTENABLE    ||
            rs == GFXRS_ZWRITEENABLE   ||
            rs == GFXRS_ZFUNC   ||
            rs == GFXRS_ZNEAR   ||
            rs == GFXRS_ZFAR)
        {
            HW__SetEnableDepthTest(RS_VALUE(m_RenderStateChanges[GFXRS_ZTESTENABLE]));
            HW__SetDepthMask(RS_VALUE(m_RenderStateChanges[GFXRS_ZWRITEENABLE]));
            HW__SetDepthFunc(RS_VALUE(m_RenderStateChanges[GFXRS_ZFUNC]));
            HW__SetDepthRange(
                RS_VALUE(m_RenderStateChanges[GFXRS_ZNEAR]),
                RS_VALUE(m_RenderStateChanges[GFXRS_ZFAR]));
        }

        if(rs == GFXRS_POLYGONOFFSETENABLE    ||
            rs == GFXRS_POLYGONOFFSETFACTOR   ||
            rs == GFXRS_POLYGONOFFSETUNITS)
        {
            HW__EnablePolygonOffset(RS_VALUE(m_RenderStateChanges[GFXRS_POLYGONOFFSETENABLE]));
            HW__SetPolygonOffset(
                RS_VALUE(m_RenderStateChanges[GFXRS_POLYGONOFFSETFACTOR]),
                RS_VALUE(m_RenderStateChanges[GFXRS_POLYGONOFFSETUNITS]));
        }

        if(rs == GFXRS_FRONT_POLYGONMODE    ||
            rs == GFXRS_BACK_POLYGONMODE)
        {
            HW__SetPolygonMode(
                RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_POLYGONMODE]),
                RS_VALUE(m_RenderStateChanges[GFXRS_BACK_POLYGONMODE]));
        }

        if(rs == GFXRS_CULLFACEENABLE     ||
            rs == GFXRS_CULLFACE          ||
            rs == GFXRS_FRONTFACE)
        {
            HW__EnableCulling(RS_VALUE(m_RenderStateChanges[GFXRS_CULLFACEENABLE]));
            HW__SetFrontFace(RS_VALUE(m_RenderStateChanges[GFXRS_FRONTFACE]));
            HW__SetCullFace(RS_VALUE(m_RenderStateChanges[GFXRS_CULLFACE]));
        }
    }

#undef RS_VALUE
}

} //NUX_NAMESPACE_END
