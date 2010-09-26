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


#ifndef GLRENDERSTATES_H
#define GLRENDERSTATES_H

#include "NuxCore/NKernel.h"

NAMESPACE_BEGIN_OGL

enum
{
    GFXRS_FRONT_POLYGONMODE,                                    // GL_FILL
    GFXRS_BACK_POLYGONMODE,                                     // GL_FILL
    GFXRS_CULLFACEENABLE,                                       // GL_FALSE
    GFXRS_CULLFACE,                                             // GL_BACK
    GFXRS_FRONTFACE,                                            // GL_CCW

    GFXRS_SCISSORTESTENABLE,                                    // GL_FALSE
    GFXRS_FOGENABLE,                                            // GL_FALSE

    GFXRS_ZTESTENABLE,                                          // GL_TRUE
    GFXRS_ZWRITEENABLE,                                         // GL_TRUE
    GFXRS_ZFUNC,                                                // GL_LESS
    GFXRS_ZNEAR,                                                // 0.0f
    GFXRS_ZFAR,                                                 // 1.0f

    GFXRS_ALPHABLENDENABLE,                                     // GL_FALSE
    GFXRS_BLENDOP,                                              // GL_FUNC_ADD
    GFXRS_BLENDOPALPHA,                                         // GL_FUNC_ADD
    GFXRS_SRCBLEND,                                             // GL_ONE
    GFXRS_DESTBLEND,                                            // GL_ZERO
    GFXRS_SRCBLENDALPHA,                                        // GL_ONE
    GFXRS_DESTBLENDALPHA,                                       // GL_ZERO

    GFXRS_ALPHATESTENABLE,                                      // GL_FALSE
    GFXRS_ALPHATESTREF,                                         // 0x0
    GFXRS_ALPHATESTFUNC,                                        // GL_ALWAYS

    GFXRS_STENCILENABLE,                                        // GL_FALSE
    GFXRS_TWOSIDEDSTENCILENABLE,                                // GL_FALSE
    GFXRS_FRONT_STENCILWRITEMASK,                               // 0xFFFFFFFF
    GFXRS_BACK_STENCILWRITEMASK,                                // 0xFFFFFFFF

    GFXRS_FRONT_STENCILFUNC,                                    // GL_ALWAYS
    GFXRS_FRONT_STENCILREF,                                     // 0x0
    GFXRS_FRONT_STENCILMASK,                                    // 0xFF
    GFXRS_FRONT_STENCILFAIL,                                    // GL_KEEP
    GFXRS_FRONT_STENCILZFAIL,                                   // GL_KEEP
    GFXRS_FRONT_STENCILZPASS,                                   // GL_KEEP

    GFXRS_BACK_STENCILFUNC,                                     // GL_ALWAYS
    GFXRS_BACK_STENCILREF,                                      // 0x0
    GFXRS_BACK_STENCILMASK,                                     // 0xFF
    GFXRS_BACK_STENCILFAIL,                                     // GL_KEEP
    GFXRS_BACK_STENCILZFAIL,                                    // GL_KEEP
    GFXRS_BACK_STENCILZPASS,                                    // GL_KEEP


    GFXRS_POINTSMOOTHENABLE,                                    // GL_FLASE
    GFXRS_LINESMOOTHENABLE,                                     // GL_FALSE
    GFXRS_POINTSIZE,                                            // 1.0f
    GFXRS_LINEWIDTH,                                            // 1.0f
    GFXRS_POINTHINT,                                            // GL_FASTEST
    GFXRS_LINEHINT,                                             // GL_FASTEST

    GFXRS_COLORWRITEENABLE_R,                                   // TRUE or FALSE
    GFXRS_COLORWRITEENABLE_G,                                   // TRUE or FALSE
    GFXRS_COLORWRITEENABLE_B,                                   // TRUE or FALSE
    GFXRS_COLORWRITEENABLE_A,                                   // TRUE or FALSE

    GFXRS_POLYGONOFFSETENABLE,                                  // GL_FALSE
    GFXRS_POLYGONOFFSETFACTOR,                                  // 0.0f
    GFXRS_POLYGONOFFSETUNITS,                                   // 0.0f

    GFXRS_MAX_RENDERSTATES,
};

enum
{
    GFXSS_ADDRESSU,
    GFXSS_ADDRESSV,
    GFXSS_ADDRESSW,
    GFXSS_MINFILTER,
    GFXSS_MAGFILTER,
    GFXSS_MIPMAPFILTER,
    GFXSS_MIPMAPLODBIAS,
    GFXSS_MAXANISOTROPY,
    GFXSS_SRGBWRITEENABLE,

    GFXSS_MAX_SAMPLERSTATES,
};

typedef enum
{
    BOARD_UNKNOWN = 0,
    BOARD_ATI,
    BOARD_NVIDIA,
    BOARD_INTEL,
} eGraphicsBoardVendor;

struct RenderStateMap
{
    t_u32   Checked;
    t_u32    State;
    t_u32    iValue;
    FLOAT   fValue;
};

class GLRenderStates
{
public:
    GLRenderStates(eGraphicsBoardVendor board);
    ~GLRenderStates();

    void ResetDefault();
    void ResetStateChangeToDefault();
    void CommitStateChangeToDefault();
    void CheckStateChange();

    void SubmitChangeStates();
    //! Check that all render states ar conform to the default
    void CheckRenderStatesConformity();


    void SetRenderStates(t_u32 rs, t_u32 value);

    // Render states
    inline void SetAlphaTest(
        bool EnableAlphaTest_,
        t_u32 AlphaTestFunc_    = GL_ALWAYS,
        BYTE  AlphaTestRef_     = 0);

    inline void SetBlend(bool AlphaBlendEnable_);
    inline void SetBlend(bool AlphaBlendEnable_,
        t_u32 SrcBlendFactor_   /*= GL_ONE*/,
        t_u32 DestBlendFactor_  /*= GL_ZERO*/);

    inline void SetSeparateBlend(bool _bEnable,
        t_u32 SrcBlendFactor_   /*= GL_ONE*/,
        t_u32 DestBlendFactor_  /*= GL_ZERO*/,
        t_u32 SrcFactorAlpha_   /*= GL_ONE*/,
        t_u32 DestFactorAlpha_  /*= GL_ZERO*/);

    inline void SetBlendOp(
        t_u32 BlendOp       = GL_FUNC_ADD);

    inline void SetSeparateBlendOp(
        t_u32 BlendOpRGB_       = GL_FUNC_ADD,
        t_u32 BlendOpAlpha_     = GL_FUNC_ADD);

    inline void SetCullMode(bool EnableCullFace,
        t_u32 FrontFace_        = GL_CCW,
        t_u32 Cullface_         = GL_BACK);

    inline void SetDepthTest(bool EnableDepthTest,
        t_u32 WriteEnable_      = GL_TRUE,
        t_u32 DepthFunc_        = GL_LEQUAL);

    inline void SetDepthRange(
        FLOAT zNear = 0.0f,
        FLOAT zFar = 1.0f);

    // If two sided stencil is not activated, the setting is also used for the back face.
    inline void SetStencilFrontFace(
        bool EnableStencil_,                            // GL_TRUE enable stencil test
        t_u32 Func_             = GL_ALWAYS,
        t_u32 FailOp_           = GL_KEEP,
        t_u32 ZFailOp_          = GL_KEEP,
        t_u32 ZPassOp_          = GL_KEEP, 
        t_u32 Ref_              = 0,
        t_u32 Mask_             = 0xffffffff);

    // Set stencil test for the back face.
    inline void SetStencilBackFace(
        bool EnableTwoSideStencil_,                     // GL_TRUE enable Two Sided Stencil test
        t_u32 Func_             = GL_ALWAYS,
        t_u32 FailOp_           = GL_KEEP,
        t_u32 ZFailOp_          = GL_KEEP,
        t_u32 ZPassOp_          = GL_KEEP, 
        t_u32 Ref_              = 0,
        t_u32 Mask_             = 0xffffffff);
        
    inline void SetFrontFaceStencilWriteMask(
        t_u32 WriteMask_        = 0xffffffff);

    inline void SetBackFaceStencilWriteMask(
        t_u32 WriteMask_        = 0xffffffff);


    inline void EnableLineSmooth(
        bool EnableLineSmooth = TRUE,
        t_u32  LineWidth = 1,
        t_u32 HINT = GL_FASTEST);

    inline void EnablePointSmooth(
        bool EnablePointSmooth = TRUE,
        t_u32  PointSize = 1,
        t_u32 HINT = GL_FASTEST);

    inline void SetColorMask(
        t_u32 bRed      = TRUE,
        t_u32 bGreen    = TRUE,
        t_u32 bBlue     = TRUE,
        t_u32 bAlpha    = TRUE);

    inline void SetDepthMask(t_u32 bDepth = TRUE);

    inline void EnableScissor(t_u32 bScissor = FALSE);
    inline void EnableFog(t_u32 bFog = FALSE);
    inline void SetPolygonMode(t_u32 FrontMode = GL_FILL, t_u32 BackMode = GL_FILL);

    inline void SetPolygonOffset(t_u32 bEnable,
        FLOAT Factor = 0.0f, FLOAT Units = 0.0f);

private:

    eGraphicsBoardVendor m_BoardVendor;

        inline void HW__EnableAlphaTest(t_u32 b);
        
        inline void HW__SetAlphaTestFunc(
            t_u32 AlphaTestFunc_,
            BYTE  AlphaTestRef_);

        inline void HW__EnableAlphaBlend(t_u32 b);
        
        inline void HW__SetSeparateAlphaBlend_Enable(t_u32 b);

        inline void HW__SetSeparateAlphaBlendFactors(
            t_u32 SrcBlendFactor_,
            t_u32 DestBlendFactor_,
            t_u32 SrcFactorAlpha_,
            t_u32 DestFactorAlpha_);

        inline void HW__SetAlphaBlendOp(t_u32 BlendOpRGB_,
            t_u32 BlendOpAlpha_);

        inline void HW__EnableCulling(t_u32 b);

        inline void HW__SetFrontFace(t_u32 FrontFace_);

        inline void HW__SetCullFace(t_u32 CullFace_);

        inline void HW__SetEnableDepthTest(t_u32 b);
        inline void HW__SetDepthFunc(t_u32 Func);
        inline void HW__SetDepthRange(FLOAT zNear, FLOAT zFar);


        inline void HW__EnableStencil(t_u32 b);
        inline void HW__EnableTwoSidedStencil(t_u32 b);
        inline void HW__SetStencilFrontFaceWriteMask(t_u32 WriteMask_);
        inline void HW__SetStencilBackFaceWriteMask(t_u32 WriteMask_);
        
        inline void HW__SetFrontFaceStencilFunc(t_u32 Func_,
            t_u32 Ref_,
            t_u32 Mask_);

        inline void HW__SetBackFaceStencilFunc(
            t_u32 Func_,
            t_u32 Ref_,
            t_u32 Mask_);

        inline void HW__SetFrontFaceStencilOp(
            t_u32 FailOp_,
            t_u32 ZFailOp_,
            t_u32 ZPassOp_);

        inline void HW__SetBackFaceStencilOp(
            t_u32 FailOp_,
            t_u32 ZFailOp_,
            t_u32 ZPassOp_);

        inline void HW__EnableLineSmooth(t_u32 EnableLineSmooth);
        inline void HW__SetLineWidth(t_u32 width, t_u32 HINT);

        inline void HW__EnablePointSmooth(t_u32 EnablePointSmooth);
        inline void HW__SetPointSize(t_u32 size, t_u32 HINT);

        inline void HW__SetColorMask(t_u32 bRed, t_u32 bGreen, t_u32 bBlue, t_u32 bAlpha);
        inline void HW__SetDepthMask(t_u32 bDepth);

        inline void HW__EnableScissor(t_u32 bScissor);
        inline void HW__EnableFog(t_u32 bFog);
    
        inline void HW__SetPolygonMode(t_u32 FrontMode, t_u32 BackMode);
        inline void HW__EnablePolygonOffset(t_u32 EnablePolygonOffset);
        inline void HW__SetPolygonOffset(FLOAT Factor, FLOAT Units);

private:
    RenderStateMap *m_RenderStateChangeList;
    RenderStateMap *m_SamplerStateChangeList;
    RenderStateMap m_RenderStateChanges[GFXRS_MAX_RENDERSTATES];
    RenderStateMap m_SamplerStateChanges[4][GFXSS_MAX_SAMPLERSTATES];

};



#define SET_RS_VALUE(a, b)  (a).iValue = (b)
#define RS_VALUE(a)      (a).iValue

//#define SET_RS_VALUE_FLOAT(a, b)  (a).fValue = (b)
//#define RS_VALUE_FLOAT(a, b)      (a).fValue


inline void GLRenderStates::SetAlphaTest(
    bool EnableAlphaTest_,
    t_u32 AlphaTestFunc_,
    BYTE AlphaTestRef_)
{
    if(EnableAlphaTest_)
    {
        if(!RS_VALUE(m_RenderStateChanges[GFXRS_ALPHATESTENABLE]))
        {
            HW__EnableAlphaTest(TRUE);
        }

        if((RS_VALUE(m_RenderStateChanges[GFXRS_ALPHATESTFUNC]) != AlphaTestFunc_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_ALPHATESTREF]) != AlphaTestRef_))
        {
            HW__SetAlphaTestFunc(AlphaTestFunc_, AlphaTestRef_);
        }
    }
    else
    {
        HW__EnableAlphaTest(GL_FALSE);
        HW__SetAlphaTestFunc(AlphaTestFunc_, AlphaTestRef_);
    }
}

inline void GLRenderStates::SetBlend(bool AlphaBlendEnable_)
{
    if(AlphaBlendEnable_)
    {
        if(!RS_VALUE(m_RenderStateChanges[GFXRS_ALPHABLENDENABLE]))
        {
            HW__EnableAlphaBlend(TRUE);
        }
    }
    else
    {
        if(RS_VALUE(m_RenderStateChanges[GFXRS_ALPHABLENDENABLE]))
        {
            HW__EnableAlphaBlend(GL_FALSE);
        }
    }
}

inline void GLRenderStates::SetBlend(bool AlphaBlendEnable_,
                                     t_u32 SrcBlendFactor_,
                                     t_u32 DestBlendFactor_)
{
    SetBlend(AlphaBlendEnable_);
    if((RS_VALUE(m_RenderStateChanges[GFXRS_SRCBLEND])!= SrcBlendFactor_) ||
        (RS_VALUE(m_RenderStateChanges[GFXRS_DESTBLEND]) != DestBlendFactor_))
    {
        HW__SetSeparateAlphaBlendFactors(
            SrcBlendFactor_,
            DestBlendFactor_,
            SrcBlendFactor_,
            DestBlendFactor_);
    }
}

inline void GLRenderStates::SetSeparateBlend(bool EnableSeparateAlphaBlend,
                                             t_u32 SrcBlendFactor_,
                                             t_u32 DestBlendFactor_,
                                             t_u32 SrcBlendFactorAlpha_,
                                             t_u32 DestBlendFactorAlpha_)
{
    SetBlend(EnableSeparateAlphaBlend);

//     if(EnableSeparateAlphaBlend)
//     {
//         if(!RS_VALUE(m_RenderStateChanges[GFXRS_ALPHABLENDENABLE]))
//         {
//             HW__EnableAlphaBlend(TRUE);
//         }

        if((RS_VALUE(m_RenderStateChanges[GFXRS_SRCBLEND])!= SrcBlendFactor_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_DESTBLEND]) != DestBlendFactor_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_SRCBLENDALPHA]) != SrcBlendFactorAlpha_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_DESTBLENDALPHA]) != DestBlendFactorAlpha_))
        {
            HW__SetSeparateAlphaBlendFactors(
                SrcBlendFactor_,
                DestBlendFactor_,
                SrcBlendFactorAlpha_,
                DestBlendFactorAlpha_);
        }
//     }
//     else
//     {
//         HW__EnableAlphaBlend(GL_FALSE);
//         HW__SetSeparateAlphaBlendFactors(
//             SrcBlendFactor_,
//             DestBlendFactor_,
//             SrcBlendFactorAlpha_,
//             DestBlendFactorAlpha_);
//     }
}

inline void GLRenderStates::SetBlendOp(t_u32 BlendOp)
{
    if((RS_VALUE(m_RenderStateChanges[GFXRS_BLENDOP])!= BlendOp))
    {
        HW__SetAlphaBlendOp(BlendOp, BlendOp);
    }
}

inline void GLRenderStates::SetSeparateBlendOp(
    t_u32 BlendOpRGB_,
    t_u32 BlendOpAlpha_)
{
    if((RS_VALUE(m_RenderStateChanges[GFXRS_BLENDOP])!= BlendOpRGB_) ||
        (RS_VALUE(m_RenderStateChanges[GFXRS_BLENDOPALPHA])!= BlendOpAlpha_))
    {
        HW__SetAlphaBlendOp(BlendOpRGB_, BlendOpAlpha_);
    }
}

inline void GLRenderStates::SetCullMode(bool EnableCullFace,
                                        t_u32 FrontFace_,
                                        t_u32 Cullface_)
{
    if(EnableCullFace)
    {
        if(!RS_VALUE(m_RenderStateChanges[GFXRS_CULLFACEENABLE]))
        {
            HW__EnableCulling(TRUE);
        }

        if(RS_VALUE(m_RenderStateChanges[GFXRS_FRONTFACE])!= FrontFace_)
        {
            HW__SetFrontFace(FrontFace_);
        }

        if(RS_VALUE(m_RenderStateChanges[GFXRS_CULLFACE]) != Cullface_)
        {
            HW__SetCullFace(Cullface_);
        }
    }
    else
    {
        HW__EnableCulling(FALSE);
        HW__SetFrontFace(FrontFace_);
        HW__SetCullFace(Cullface_);
    }
}

inline void GLRenderStates::SetDepthTest(bool EnableDepthTest,
                                         t_u32 WriteEnable_,
                                         t_u32 DepthFunc_)
{
    if(EnableDepthTest)
    {
        if(!RS_VALUE(m_RenderStateChanges[GFXRS_ZTESTENABLE]))
        {
            HW__SetEnableDepthTest(TRUE);
        }

        if(RS_VALUE(m_RenderStateChanges[GFXRS_ZWRITEENABLE])!= WriteEnable_)
        {
            HW__SetDepthMask(WriteEnable_);
        }

        if(RS_VALUE(m_RenderStateChanges[GFXRS_ZFUNC]) != DepthFunc_)
        {
            HW__SetDepthFunc(DepthFunc_);
        }
    }
    else
    {
        HW__SetEnableDepthTest(FALSE);
        HW__SetDepthMask(WriteEnable_);
        HW__SetDepthFunc(DepthFunc_);
    }
}

inline void GLRenderStates::SetDepthRange(FLOAT zNear, FLOAT zFar)
{
    if((RS_VALUE(m_RenderStateChanges[GFXRS_ZNEAR]) != static_cast<t_u32>(zNear)) ||
        (RS_VALUE(m_RenderStateChanges[GFXRS_ZFAR]) != static_cast<t_u32>(zFar)))
    {
        HW__SetDepthRange(zNear, zFar);
    }
}

inline void GLRenderStates::SetStencilFrontFace(
    bool EnableStencil_,                            // GL_TRUE enable stencil test
    t_u32 Func_,
    t_u32 FailOp_,
    t_u32 ZFailOp_,
    t_u32 ZPassOp_,
    t_u32 Ref_,
    t_u32 Mask_)
{
    if(EnableStencil_)
    {
        if(!RS_VALUE(m_RenderStateChanges[GFXRS_STENCILENABLE]))
        {
            HW__EnableStencil(TRUE);
        }

        if(
            (RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILFUNC])!= Func_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILREF]) != Ref_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILMASK]) != Mask_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILFAIL]) != FailOp_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILZFAIL]) != ZFailOp_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILZPASS]) != ZPassOp_)
            )
        {
            HW__SetFrontFaceStencilFunc(Func_, Ref_, Mask_);
            HW__SetFrontFaceStencilOp(FailOp_, ZFailOp_, ZPassOp_);
        }
    }
    else
    {
        HW__EnableStencil(FALSE);
        HW__SetFrontFaceStencilFunc(Func_, Ref_, Mask_);
        HW__SetFrontFaceStencilOp(FailOp_, ZFailOp_, ZPassOp_);
    }
}

inline void GLRenderStates::SetStencilBackFace(
    bool EnableTwoSideStencil_,                     // GL_TRUE enable Two Sided Stencil test
    t_u32 Func_,
    t_u32 FailOp_,
    t_u32 ZFailOp_,
    t_u32 ZPassOp_,
    t_u32 Ref_,
    t_u32 Mask_)
{
    if(EnableTwoSideStencil_)
    {
        if(!RS_VALUE(m_RenderStateChanges[GFXRS_STENCILENABLE]))
        {
            HW__EnableStencil(TRUE);
        }

        if(!RS_VALUE(m_RenderStateChanges[GFXRS_TWOSIDEDSTENCILENABLE]))
        {
            HW__EnableTwoSidedStencil(TRUE);
        }

        if(
            (RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILFUNC])!= Func_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILREF]) != Ref_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILMASK]) != Mask_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILFAIL]) != FailOp_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILZFAIL]) != ZFailOp_) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILZPASS]) != ZPassOp_)
            )
        {
            HW__SetBackFaceStencilFunc(Func_, Ref_, Mask_);
            HW__SetBackFaceStencilOp(FailOp_, ZFailOp_, ZPassOp_);
        }
    }
    else
    {
        HW__EnableTwoSidedStencil(FALSE);
        HW__SetBackFaceStencilFunc(Func_, Ref_, Mask_);
        HW__SetBackFaceStencilOp(FailOp_, ZFailOp_, ZPassOp_);
    }
}

inline void GLRenderStates::SetFrontFaceStencilWriteMask(
    t_u32 WriteMask_)
{
    HW__SetStencilFrontFaceWriteMask(WriteMask_);
}

inline void GLRenderStates::SetBackFaceStencilWriteMask(
    t_u32 WriteMask_)
{
    HW__SetStencilBackFaceWriteMask(WriteMask_);
}

inline void GLRenderStates::EnableLineSmooth(
    bool EnableLineSmooth,
    t_u32  LineWidth,
    t_u32 Hint)
{
    if(EnableLineSmooth)
    {
        if(!RS_VALUE(m_RenderStateChanges[GFXRS_LINESMOOTHENABLE]))
        {
            HW__EnableLineSmooth(GL_TRUE);
        }

        if((RS_VALUE(m_RenderStateChanges[GFXRS_LINEWIDTH]) != LineWidth) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_LINEHINT]) != Hint))
        {
            HW__SetLineWidth(LineWidth, Hint);
        }
    }
    else
    {
        HW__EnableLineSmooth(GL_FALSE);
        HW__SetLineWidth(LineWidth, Hint);
    }
}

inline void GLRenderStates::EnablePointSmooth(
    bool EnablePointSmooth,
    t_u32  PointSize,
    t_u32 Hint)
{
    if(EnablePointSmooth)
    {
        if(!RS_VALUE(m_RenderStateChanges[GFXRS_POINTSMOOTHENABLE]))
        {
            HW__EnablePointSmooth(GL_TRUE);
        }

        if((RS_VALUE(m_RenderStateChanges[GFXRS_POINTSIZE]) != PointSize) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_POINTHINT]) != Hint))
        {
            HW__SetLineWidth(PointSize, Hint);
        }
    }
    else
    {
        HW__EnablePointSmooth(GL_FALSE);
        HW__SetLineWidth(PointSize, Hint);
    }
}

inline void GLRenderStates::SetColorMask(
                         t_u32 bRed,
                         t_u32 bGreen,
                         t_u32 bBlue,
                         t_u32 bAlpha)
{
    if((RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_R]) != bRed) ||
        (RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_G]) != bGreen) ||
        (RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_B]) != bBlue) ||
        (RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_A]) != bAlpha))
    {
        HW__SetColorMask(bRed, bGreen, bBlue, bAlpha);
    }

}

inline void GLRenderStates::SetDepthMask(t_u32 bDepth)
{
    if((RS_VALUE(m_RenderStateChanges[GFXRS_ZWRITEENABLE]) != bDepth))
    {
        HW__SetDepthMask(bDepth);
    }
}

inline void GLRenderStates::EnableScissor(t_u32 bScissor)
{
    if((RS_VALUE(m_RenderStateChanges[GFXRS_SCISSORTESTENABLE]) != bScissor))
    {
        HW__EnableScissor(bScissor);
    }
}

inline void GLRenderStates::EnableFog(t_u32 bFog)
{
    if((RS_VALUE(m_RenderStateChanges[GFXRS_FOGENABLE]) != bFog))
    {
        HW__EnableFog(bFog);
    }
}

inline void GLRenderStates::SetPolygonMode(t_u32 FrontMode, t_u32 BackMode)
{
    if((RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_POLYGONMODE]) != FrontMode) ||
        (RS_VALUE(m_RenderStateChanges[GFXRS_BACK_POLYGONMODE]) != BackMode))
    {
        HW__SetPolygonMode(FrontMode, BackMode);
    }
}

inline void GLRenderStates::SetPolygonOffset(t_u32 bEnable,
                             FLOAT Factor, FLOAT Units)
{
    if(bEnable)
    {
        if(!RS_VALUE(m_RenderStateChanges[GFXRS_POLYGONOFFSETENABLE]))
        {
            HW__EnablePolygonOffset(GL_TRUE);
        }

        if((RS_VALUE(m_RenderStateChanges[GFXRS_POLYGONOFFSETFACTOR]) != static_cast<t_u32>(Factor)) ||
            (RS_VALUE(m_RenderStateChanges[GFXRS_POLYGONOFFSETUNITS]) != static_cast<t_u32>(Units)))
        {
            HW__SetPolygonOffset(Factor, Units);
        }
    }
    else
    {
        HW__EnablePolygonOffset(GL_FALSE);
    }
}


//////////////////////////////////////
inline void GLRenderStates::HW__EnableAlphaTest(t_u32 b)
{
    if(b)
    {
        CHECKGL(glEnable(GL_ALPHA_TEST));
    }
    else
    {
        CHECKGL(glDisable(GL_ALPHA_TEST));
    }
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_ALPHATESTENABLE], b ? GL_TRUE : GL_FALSE);
}

inline void GLRenderStates::HW__SetAlphaTestFunc(t_u32 AlphaTestFunc_,
                                                 BYTE  AlphaTestRef_)
{
    nuxAssertMsg(
        (AlphaTestFunc_ == GL_NEVER) ||
        (AlphaTestFunc_ == GL_LESS) ||
        (AlphaTestFunc_ == GL_EQUAL) ||
        (AlphaTestFunc_ == GL_LEQUAL) ||
        (AlphaTestFunc_ == GL_GREATER) ||
        (AlphaTestFunc_ == GL_NOTEQUAL) ||
        (AlphaTestFunc_ == GL_GEQUAL) ||
        (AlphaTestFunc_ == GL_ALWAYS),
        TEXT("Error(HW__SetAlphaTestFunc): Invalid Alpha Test Function RenderState"));

    CHECKGL(glAlphaFunc(AlphaTestFunc_, (FLOAT)AlphaTestRef_ * (1.0f / 255.0f)));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_ALPHATESTFUNC], AlphaTestFunc_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_ALPHATESTREF], AlphaTestRef_);
}

inline void GLRenderStates::HW__EnableAlphaBlend(t_u32 b)
{
    if(b)
    {
        CHECKGL(glEnable(GL_BLEND));
    }
    else
    {
        CHECKGL(glDisable(GL_BLEND));
    }
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_ALPHABLENDENABLE], b ? GL_TRUE : GL_FALSE);
}

inline void GLRenderStates::HW__SetSeparateAlphaBlendFactors(
    t_u32 SrcBlendFactor_,
    t_u32 DestBlendFactor_,
    t_u32 SrcFactorAlpha_,
    t_u32 DestFactorAlpha_)
{
    nuxAssertMsg((SrcBlendFactor_ == GL_ZERO) || (SrcBlendFactor_ == GL_ONE) || (SrcBlendFactor_ == GL_SRC_COLOR) || (SrcBlendFactor_ == GL_ONE_MINUS_SRC_COLOR) ||(SrcBlendFactor_ == GL_DST_COLOR) ||(SrcBlendFactor_ == GL_ONE_MINUS_DST_COLOR) || (SrcBlendFactor_ == GL_SRC_ALPHA) ||(SrcBlendFactor_ == GL_ONE_MINUS_SRC_ALPHA) ||(SrcBlendFactor_ == GL_DST_ALPHA) ||(SrcBlendFactor_ == GL_ONE_MINUS_DST_ALPHA) ||(SrcBlendFactor_ == GL_CONSTANT_COLOR) ||(SrcBlendFactor_ == GL_ONE_MINUS_CONSTANT_COLOR) ||(SrcBlendFactor_ == GL_CONSTANT_ALPHA) ||(SrcBlendFactor_ == GL_ONE_MINUS_CONSTANT_ALPHA) ||(SrcBlendFactor_ == GL_SRC_ALPHA_SATURATE),
        TEXT("Error(HW__SetSeparateAlphaBlendFactors): Invalid Blend RenderState"));
    nuxAssertMsg((DestBlendFactor_ == GL_ZERO) || (DestBlendFactor_ == GL_ONE) || (DestBlendFactor_ == GL_SRC_COLOR) || (DestBlendFactor_ == GL_ONE_MINUS_SRC_COLOR) ||(DestBlendFactor_ == GL_DST_COLOR) ||(DestBlendFactor_ == GL_ONE_MINUS_DST_COLOR) || (DestBlendFactor_ == GL_SRC_ALPHA) ||(DestBlendFactor_ == GL_ONE_MINUS_SRC_ALPHA) ||(DestBlendFactor_ == GL_DST_ALPHA) ||(DestBlendFactor_ == GL_ONE_MINUS_DST_ALPHA) ||(DestBlendFactor_ == GL_CONSTANT_COLOR) ||(DestBlendFactor_ == GL_ONE_MINUS_CONSTANT_COLOR) ||(DestBlendFactor_ == GL_CONSTANT_ALPHA) ||(DestBlendFactor_ == GL_ONE_MINUS_CONSTANT_ALPHA),
        TEXT("Error(HW__SetSeparateAlphaBlendFactors): Invalid Blend RenderState"));
    nuxAssertMsg((SrcFactorAlpha_ == GL_ZERO) || (SrcFactorAlpha_ == GL_ONE) || (SrcFactorAlpha_ == GL_SRC_COLOR) || (SrcFactorAlpha_ == GL_ONE_MINUS_SRC_COLOR) ||(SrcFactorAlpha_ == GL_DST_COLOR) ||(SrcFactorAlpha_ == GL_ONE_MINUS_DST_COLOR) || (SrcFactorAlpha_ == GL_SRC_ALPHA) ||(SrcFactorAlpha_ == GL_ONE_MINUS_SRC_ALPHA) ||(SrcFactorAlpha_ == GL_DST_ALPHA) ||(SrcFactorAlpha_ == GL_ONE_MINUS_DST_ALPHA) ||(SrcFactorAlpha_ == GL_CONSTANT_COLOR) ||(SrcFactorAlpha_ == GL_ONE_MINUS_CONSTANT_COLOR) ||(SrcFactorAlpha_ == GL_CONSTANT_ALPHA) ||(SrcFactorAlpha_ == GL_ONE_MINUS_CONSTANT_ALPHA) ||(SrcFactorAlpha_ == GL_SRC_ALPHA_SATURATE),
        TEXT("Error(HW__SetSeparateAlphaBlendFactors): Invalid Blend RenderState"));
    nuxAssertMsg((DestFactorAlpha_ == GL_ZERO) || (DestFactorAlpha_ == GL_ONE) || (DestFactorAlpha_ == GL_SRC_COLOR) || (DestFactorAlpha_ == GL_ONE_MINUS_SRC_COLOR) ||(DestFactorAlpha_ == GL_DST_COLOR) ||(DestFactorAlpha_ == GL_ONE_MINUS_DST_COLOR) || (DestFactorAlpha_ == GL_SRC_ALPHA) ||(DestFactorAlpha_ == GL_ONE_MINUS_SRC_ALPHA) ||(DestFactorAlpha_ == GL_DST_ALPHA) ||(DestFactorAlpha_ == GL_ONE_MINUS_DST_ALPHA) ||(DestFactorAlpha_ == GL_CONSTANT_COLOR) ||(DestFactorAlpha_ == GL_ONE_MINUS_CONSTANT_COLOR) ||(DestFactorAlpha_ == GL_CONSTANT_ALPHA) ||(DestFactorAlpha_ == GL_ONE_MINUS_CONSTANT_ALPHA),
        TEXT("Error(HW__SetSeparateAlphaBlendFactors): Invalid Blend RenderState"));

    CHECKGL(glBlendFuncSeparate(
        SrcBlendFactor_,
        DestBlendFactor_,
        SrcFactorAlpha_,
        DestFactorAlpha_));

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_SRCBLEND], SrcBlendFactor_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_DESTBLEND], DestBlendFactor_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_SRCBLENDALPHA], SrcFactorAlpha_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_DESTBLENDALPHA], DestFactorAlpha_);
}

inline void GLRenderStates::HW__SetAlphaBlendOp(
    t_u32 BlendOpRGB_,
    t_u32 BlendOpAlpha_)
{
    nuxAssertMsg(
        (BlendOpRGB_ == GL_FUNC_ADD) ||
        (BlendOpRGB_ == GL_FUNC_SUBTRACT) ||
        (BlendOpRGB_ == GL_FUNC_REVERSE_SUBTRACT) ||
        (BlendOpRGB_ == GL_MIN) ||
        (BlendOpRGB_ == GL_MAX),
        TEXT("Error(HW__SetAlphaBlendOp): Invalid Blend Equation RenderState"));
    nuxAssertMsg(
        (BlendOpAlpha_ == GL_FUNC_ADD) ||
        (BlendOpAlpha_ == GL_FUNC_SUBTRACT) ||
        (BlendOpAlpha_ == GL_FUNC_REVERSE_SUBTRACT) ||
        (BlendOpAlpha_ == GL_MIN) ||
        (BlendOpAlpha_ == GL_MAX),
        TEXT("Error(HW__SetAlphaBlendOp): Invalid Blend Equation RenderState"));

    CHECKGL(glBlendEquationSeparateEXT(BlendOpRGB_, BlendOpAlpha_));

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BLENDOP], BlendOpRGB_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BLENDOPALPHA], BlendOpAlpha_);
}

inline void GLRenderStates::HW__EnableCulling(t_u32 b)
{
    if(b)
    {
        CHECKGL(glEnable(GL_CULL_FACE));
    }
    else
    {
        CHECKGL(glDisable(GL_CULL_FACE));
    }
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_CULLFACEENABLE], b ? GL_TRUE : GL_FALSE);
}


inline void GLRenderStates::HW__SetFrontFace(t_u32 FrontFace_)
{
    nuxAssertMsg(
        (FrontFace_ == GL_CW) ||
        (FrontFace_ == GL_CCW),
        TEXT("Error(HW__SetFrontFace): Invalid Front Face RenderState"));

    CHECKGL(glFrontFace(FrontFace_));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FRONTFACE], FrontFace_);
}

inline void GLRenderStates::HW__SetCullFace(t_u32 CullFace_)
{
    nuxAssertMsg(
        (CullFace_ == GL_FRONT) ||
        (CullFace_ == GL_BACK) ||
        (CullFace_ == GL_FRONT_AND_BACK),
        TEXT("Error(HW__SetCullFace): Invalid Cull Face RenderState"));

    CHECKGL(glCullFace(CullFace_));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_CULLFACE], CullFace_);
}

inline void GLRenderStates::HW__SetEnableDepthTest(t_u32 b)
{
    if(b)
    {
        CHECKGL(glEnable(GL_DEPTH_TEST));
    }
    else
    {
        CHECKGL(glDisable(GL_DEPTH_TEST));
    }
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_ZTESTENABLE], b ? GL_TRUE : GL_FALSE);
}

inline void GLRenderStates::HW__SetDepthRange(FLOAT zNear, FLOAT zFar)
{
    CHECKGL(glDepthRange(zNear, zFar));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_ZNEAR], static_cast<t_u32>(Clamp(zNear, 0.0f, 1.0f)));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_ZFAR], static_cast<t_u32>(Clamp(zFar, 0.0f, 1.0f)));
}

inline void GLRenderStates::HW__SetDepthFunc(t_u32 Func)
{
    nuxAssertMsg(
        (Func == GL_NEVER) ||
        (Func == GL_LESS) ||
        (Func == GL_EQUAL) ||
        (Func == GL_LEQUAL) ||
        (Func == GL_GREATER) ||
        (Func == GL_NOTEQUAL) ||
        (Func == GL_GEQUAL) ||
        (Func == GL_ALWAYS),
        TEXT("Error(HW__SetDepthFunc): Invalid Depth Func RenderState"));

    CHECKGL(glDepthFunc(Func));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_ZFUNC], Func);
}

inline void GLRenderStates::HW__EnableStencil(t_u32 b)
{
    if(b)
    {
        CHECKGL(glEnable(GL_STENCIL_TEST));
    }
    else
    {
        CHECKGL(glDisable(GL_STENCIL_TEST));
    }

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_STENCILENABLE], b ? GL_TRUE : GL_FALSE);
}

inline void GLRenderStates::HW__EnableTwoSidedStencil(t_u32 b)
{
    if(b)
    {
        if(m_BoardVendor == BOARD_ATI)
        {
            CHECKGL(glEnable(GL_STENCIL_TEST));
        }
        else
        {
            CHECKGL(glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT));
        }
    }
    else
    {
        if(m_BoardVendor == BOARD_ATI)
        {
            CHECKGL(glDisable(GL_STENCIL_TEST));
        }
        else
        {
            CHECKGL(glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT));
        }
    }

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_TWOSIDEDSTENCILENABLE], b ? GL_TRUE : GL_FALSE);
}

inline void GLRenderStates::HW__SetStencilFrontFaceWriteMask(t_u32 WriteMask_)
{
    CHECKGL(glActiveStencilFaceEXT(GL_FRONT));
    CHECKGL(glStencilMask(WriteMask_));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILWRITEMASK], WriteMask_);
}

inline void GLRenderStates::HW__SetStencilBackFaceWriteMask(t_u32 WriteMask_)
{
    CHECKGL(glActiveStencilFaceEXT(GL_BACK));
    CHECKGL(glStencilMask(WriteMask_));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILWRITEMASK], WriteMask_);
}


inline void GLRenderStates::HW__SetFrontFaceStencilFunc(t_u32 Func_,
                                                        t_u32 Ref_,
                                                        t_u32 Mask_)
{
    nuxAssertMsg(
        (Func_ == GL_NEVER) ||
        (Func_ == GL_LESS) ||
        (Func_ == GL_EQUAL) ||
        (Func_ == GL_LEQUAL) ||
        (Func_ == GL_GREATER) ||
        (Func_ == GL_NOTEQUAL) ||
        (Func_ == GL_GEQUAL) ||
        (Func_ == GL_ALWAYS),
        TEXT("Error(HW__SetFrontFaceStencilFunc): Invalid Stencil Function RenderState"));

    CHECKGL(glActiveStencilFaceEXT(GL_FRONT));
    CHECKGL(glStencilFunc(Func_, Ref_, Mask_));

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILFUNC], Func_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILREF], Ref_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILMASK], Mask_);
}

inline void GLRenderStates::HW__SetBackFaceStencilFunc(
    t_u32 Func_,
    t_u32 Ref_,
    t_u32 Mask_)
{
    nuxAssertMsg(
        (Func_ == GL_NEVER) ||
        (Func_ == GL_LESS) ||
        (Func_ == GL_EQUAL) ||
        (Func_ == GL_LEQUAL) ||
        (Func_ == GL_GREATER) ||
        (Func_ == GL_NOTEQUAL) ||
        (Func_ == GL_GEQUAL) ||
        (Func_ == GL_ALWAYS),
        TEXT("Error(HW__SetBackFaceStencilFunc): Invalid Stencil Function RenderState"));

    if(m_BoardVendor == BOARD_ATI)
    {
        CHECKGL(glStencilFuncSeparateATI(Func_/*Front function*/, Func_/*Back function*/, Ref_, Mask_));  // incorrect
    }
    else
    {
        CHECKGL(glActiveStencilFaceEXT(GL_BACK));
        CHECKGL(glStencilFunc(Func_, Ref_, Mask_));
    }

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILFUNC], Func_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILREF], Ref_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILMASK], Mask_);
}

inline void GLRenderStates::HW__SetFrontFaceStencilOp(
    t_u32 FailOp_,
    t_u32 ZFailOp_,
    t_u32 ZPassOp_)
{
    nuxAssertMsg(
        (FailOp_ == GL_KEEP) ||
        (FailOp_ == GL_ZERO) ||
        (FailOp_ == GL_REPLACE) ||
        (FailOp_ == GL_INCR) ||
        (FailOp_ == GL_INCR_WRAP) ||
        (FailOp_ == GL_DECR) ||
        (FailOp_ == GL_DECR_WRAP) ||
        (FailOp_ == GL_INVERT),
        TEXT("Error(HW__SetFrontFaceStencilOp): Invalid FailOp RenderState"));

    nuxAssertMsg(
        (ZFailOp_ == GL_KEEP) ||
        (ZFailOp_ == GL_ZERO) ||
        (ZFailOp_ == GL_REPLACE) ||
        (ZFailOp_ == GL_INCR) ||
        (ZFailOp_ == GL_INCR_WRAP) ||
        (ZFailOp_ == GL_DECR) ||
        (ZFailOp_ == GL_DECR_WRAP) ||
        (ZFailOp_ == GL_INVERT),
        TEXT("Error(HW__SetFrontFaceStencilOp): Invalid ZFailOp RenderState"));

    nuxAssertMsg(
        (ZPassOp_ == GL_KEEP) ||
        (ZPassOp_ == GL_ZERO) ||
        (ZPassOp_ == GL_REPLACE) ||
        (ZPassOp_ == GL_INCR) ||
        (ZPassOp_ == GL_INCR_WRAP) ||
        (ZPassOp_ == GL_DECR) ||
        (ZPassOp_ == GL_DECR_WRAP) ||
        (ZPassOp_ == GL_INVERT),
        TEXT("Error(HW__SetFrontFaceStencilOp): Invalid ZPassOp RenderState"));

    if(m_BoardVendor == BOARD_ATI)
    {
        CHECKGL(glStencilOpSeparateATI(GL_FRONT, FailOp_, ZFailOp_, ZPassOp_));
    }
    else
    {
        CHECKGL(glActiveStencilFaceEXT(GL_FRONT));
        CHECKGL(glStencilOp(FailOp_, ZFailOp_, ZPassOp_));
    }

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILFAIL], FailOp_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILZFAIL], ZFailOp_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_STENCILZPASS], ZPassOp_);
}

inline void GLRenderStates::HW__SetBackFaceStencilOp(
    t_u32 FailOp_,
    t_u32 ZFailOp_,
    t_u32 ZPassOp_)
{
    nuxAssertMsg(
        (FailOp_ == GL_KEEP) ||
        (FailOp_ == GL_ZERO) ||
        (FailOp_ == GL_REPLACE) ||
        (FailOp_ == GL_INCR) ||
        (FailOp_ == GL_INCR_WRAP) ||
        (FailOp_ == GL_DECR) ||
        (FailOp_ == GL_DECR_WRAP) ||
        (FailOp_ == GL_INVERT),
        TEXT("Error(HW__SetBackFaceStencilOp): Invalid FailOp RenderState"));

    nuxAssertMsg(
        (ZFailOp_ == GL_KEEP) ||
        (ZFailOp_ == GL_ZERO) ||
        (ZFailOp_ == GL_REPLACE) ||
        (ZFailOp_ == GL_INCR) ||
        (ZFailOp_ == GL_INCR_WRAP) ||
        (ZFailOp_ == GL_DECR) ||
        (ZFailOp_ == GL_DECR_WRAP) ||
        (ZFailOp_ == GL_INVERT),
        TEXT("Error(HW__SetBackFaceStencilOp): Invalid ZFailOp RenderState"));

    nuxAssertMsg(
        (ZPassOp_ == GL_KEEP) ||
        (ZPassOp_ == GL_ZERO) ||
        (ZPassOp_ == GL_REPLACE) ||
        (ZPassOp_ == GL_INCR) ||
        (ZPassOp_ == GL_INCR_WRAP) ||
        (ZPassOp_ == GL_DECR) ||
        (ZPassOp_ == GL_DECR_WRAP) ||
        (ZPassOp_ == GL_INVERT),
        TEXT("Error(HW__SetBackFaceStencilOp): Invalid ZPassOp RenderState"));

    if(m_BoardVendor == BOARD_ATI)
    {
        CHECKGL(glStencilOpSeparateATI(GL_BACK, FailOp_, ZFailOp_, ZPassOp_));
    }
    else
    {
        CHECKGL(glActiveStencilFaceEXT(GL_BACK));
        CHECKGL(glStencilOp(FailOp_, ZFailOp_, ZPassOp_));
    }

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILFAIL], FailOp_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILZFAIL], ZFailOp_);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BACK_STENCILZPASS], ZPassOp_);
}

inline void GLRenderStates::HW__EnableLineSmooth(t_u32 EnableLineSmooth)
{
    if(EnableLineSmooth)
    {
        CHECKGL(glEnable(GL_LINE_SMOOTH));
    }
    else
    {
        CHECKGL(glDisable(GL_LINE_SMOOTH));
    }
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_LINESMOOTHENABLE], EnableLineSmooth ? GL_TRUE : GL_FALSE);
}

inline void GLRenderStates::HW__SetLineWidth(t_u32 width,  t_u32 Hint)
{
    nuxAssertMsg(
        (Hint == GL_NICEST) ||
        (Hint == GL_FASTEST) ||
        (Hint == GL_DONT_CARE),
        TEXT("Error(HW__SetLineWidth): Invalid Line Hint RenderState"));

    CHECKGL(glLineWidth(width));
    CHECKGL(glHint(GL_LINE_SMOOTH_HINT, Hint));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_LINEWIDTH], width);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_LINEHINT], Hint);
}

inline void GLRenderStates::HW__EnablePointSmooth(t_u32 EnablePointSmooth)
{
    if(EnablePointSmooth)
    {
        CHECKGL(glEnable(GL_POINT_SMOOTH));
    }
    else
    {
        CHECKGL(glDisable(GL_POINT_SMOOTH));
    }
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_POINTSMOOTHENABLE], EnablePointSmooth ? GL_TRUE : GL_FALSE);
}

inline void GLRenderStates::HW__SetPointSize(t_u32 size,  t_u32 Hint)
{
    nuxAssertMsg(
        (Hint == GL_NICEST) ||
        (Hint == GL_FASTEST) ||
        (Hint == GL_DONT_CARE),
        TEXT("Error(HW__SetPointSize): Invalid Point Hint RenderState"));

    CHECKGL(glPointSize(size));
    CHECKGL(glHint(GL_POINT_SMOOTH_HINT, Hint);)
        SET_RS_VALUE(m_RenderStateChanges[GFXRS_POINTSIZE], size);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_POINTHINT], Hint);
}

inline void GLRenderStates::HW__SetColorMask(
    t_u32 bRed,
    t_u32 bGreen,
    t_u32 bBlue,
    t_u32 bAlpha)
{
    CHECKGL(glColorMask(bRed, bGreen, bBlue, bAlpha));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_R], bRed);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_G], bGreen);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_B], bBlue);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_COLORWRITEENABLE_A], bAlpha);
}

inline void GLRenderStates::HW__SetDepthMask(t_u32 bDepth)
{
    CHECKGL(glDepthMask(bDepth));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_ZWRITEENABLE], bDepth);
}

inline void GLRenderStates::HW__EnableScissor(t_u32 bScissor)
{
    if(bScissor)
    {
        CHECKGL(glEnable(GL_SCISSOR_TEST));
    }
    else
    {
        CHECKGL(glDisable(GL_SCISSOR_TEST));
    }

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_SCISSORTESTENABLE], bScissor ? GL_TRUE : GL_FALSE);
}

inline void GLRenderStates::HW__EnableFog(t_u32 bFog)
{
    if(bFog)
    {
        CHECKGL(glEnable(GL_FOG));
    }
    else
    {
        CHECKGL(glDisable(GL_FOG));
    }

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FOGENABLE], bFog ? GL_TRUE : GL_FALSE);
}

inline void GLRenderStates::HW__SetPolygonMode(t_u32 FrontMode, t_u32 BackMode)
{
    nuxAssertMsg(
        (FrontMode == GL_FILL) ||
        (FrontMode == GL_LINE) ||
        (FrontMode == GL_POINT),
        TEXT("Error(HW__SetPolygonMode): Invalid Point Hint RenderState"));

    nuxAssertMsg(
        (BackMode == GL_FILL) ||
        (BackMode == GL_LINE) ||
        (BackMode == GL_POINT),
        TEXT("Error(HW__SetPolygonMode): Invalid Point Hint RenderState"));

    CHECKGL(glPolygonMode(GL_FRONT, FrontMode));
    CHECKGL(glPolygonMode(GL_BACK, BackMode));

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_FRONT_POLYGONMODE], FrontMode);
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_BACK_POLYGONMODE], BackMode);
}

inline void GLRenderStates::HW__EnablePolygonOffset(t_u32 EnablePolygonOffset)
{
    if(EnablePolygonOffset)
    {
        CHECKGL(glEnable(GL_POLYGON_OFFSET_FILL));
    }
    else
    {
        CHECKGL(glDisable(GL_POLYGON_OFFSET_FILL));
    }

    SET_RS_VALUE(m_RenderStateChanges[GL_POLYGON_OFFSET_FILL], EnablePolygonOffset ? GL_TRUE : GL_FALSE);  
}

inline void GLRenderStates::HW__SetPolygonOffset(FLOAT Factor, FLOAT Units)
{
    CHECKGL(glPolygonOffset(Factor, Units));

    SET_RS_VALUE(m_RenderStateChanges[GFXRS_POLYGONOFFSETFACTOR], static_cast<t_u32>(Factor));
    SET_RS_VALUE(m_RenderStateChanges[GFXRS_POLYGONOFFSETUNITS], static_cast<t_u32>(Units));
}

#undef SET_RS_VALUE
#undef RS_VALUE

//#undef SET_RS_VALUE_FLOAT
//#undef RS_VALUE_FLOAT

NAMESPACE_END_OGL

#endif // GLRENDERSTATES_H

