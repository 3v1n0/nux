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


#ifndef GLRENDERSTATES_H
#define GLRENDERSTATES_H

#include "NuxCore/NuxCore.h"
#include "GpuDevice.h"

namespace nux
{

  enum
  {
    GFXRS_FRONT_POLYGONMODE,                                    // GL_FILL
    GFXRS_BACK_POLYGONMODE,                                     // GL_FILL
    GFXRS_CULLFACEENABLE,                                       // GL_FALSE
    GFXRS_CULLFACE,                                             // GL_BACK
    GFXRS_FRONTFACE,                                            // GL_CCW

    GFXRS_SCISSORTESTENABLE,                                    // GL_FALSE

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


    GFXRS_LINESMOOTHENABLE,                                     // GL_FALSE
    GFXRS_LINEWIDTH,                                            // 1.0f
    GFXRS_LINEHINT,                                             // GL_FASTEST

    GFXRS_COLORWRITEENABLE_R,                                   // TRUE or FALSE
    GFXRS_COLORWRITEENABLE_G,                                   // TRUE or FALSE
    GFXRS_COLORWRITEENABLE_B,                                   // TRUE or FALSE
    GFXRS_COLORWRITEENABLE_A,                                   // TRUE or FALSE

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
    CLEAR = 0,
    SRC,
    DST,
    SRC_OVER,
    DST_OVER,
    SRC_IN,
    DST_IN,
    SRC_OUT,
    DST_OUT,
    SRC_ATOP,
    DST_ATOP,
    XOR,
    PLUS
  } PorterDuffOperator;

  struct RenderStateMap
  {
    unsigned int   Checked;
    unsigned int    State;
    unsigned int    iValue;
    float   fValue;
  };

  class GpuRenderStates
  {
  public:
    GpuRenderStates(GpuBrand board, GpuInfo* info);
    ~GpuRenderStates();

    void ResetDefault();
    void ResetStateChangeToDefault();
    void CommitStateChangeToDefault();
    void CheckStateChange();

    void SubmitChangeStates();
    //! Check that all render states ar conform to the default
    void CheckRenderStatesConformity();

    // Render states
#ifndef NUX_OPENGLES_20    
    inline void SetAlphaTest(
      bool EnableAlphaTest_,
      unsigned int AlphaTestFunc_    = GL_ALWAYS,
      BYTE  AlphaTestRef_     = 0);
#endif

    inline void SetBlend(bool AlphaBlendEnable_);
    inline void SetBlend(bool AlphaBlendEnable_,
                          unsigned int SrcBlendFactor_   /*= GL_ONE*/,
                          unsigned int DestBlendFactor_  /*= GL_ZERO*/);

    inline void GetBlend(unsigned int& AlphaBlendEnable_,
                      unsigned int& SrcBlendFactor_,
                      unsigned int& DestBlendFactor_);
    
    inline void SetSeparateBlend(bool _bEnable,
                                  unsigned int SrcBlendFactor_   /*= GL_ONE*/,
                                  unsigned int DestBlendFactor_  /*= GL_ZERO*/,
                                  unsigned int SrcFactorAlpha_   /*= GL_ONE*/,
                                  unsigned int DestFactorAlpha_  /*= GL_ZERO*/);

    inline void SetPremultipliedBlend(PorterDuffOperator op);

    inline void SetBlendOp(
      unsigned int BlendOp       = GL_FUNC_ADD);

    inline void SetSeparateBlendOp(
      unsigned int BlendOpRGB_       = GL_FUNC_ADD,
      unsigned int BlendOpAlpha_     = GL_FUNC_ADD);

    inline void SetCullMode(bool EnableCullFace,
                             unsigned int FrontFace_        = GL_CCW,
                             unsigned int Cullface_         = GL_BACK);

    inline void SetDepthTest(bool EnableDepthTest,
                              unsigned int WriteEnable_      = GL_TRUE,
                              unsigned int DepthFunc_        = GL_LEQUAL);

    inline void SetDepthRange(
      float zNear = 0.0f,
      float zFar = 1.0f);


    inline void SetStencil(bool enable_stencil);
    inline void SetStencilFunc(unsigned int func, int ref, unsigned int mask);
    inline void SetStencilOp(unsigned int stencil_fail, unsigned int stencil_pass_depth_fail, unsigned int stencil_pass_depth_pass);

#if 0
    // If two sided stencil is not activated, the setting is also used for the back face.
    inline void SetStencilFrontFace(
      bool EnableStencil_,                            // GL_TRUE enable stencil test
      unsigned int Func_             = GL_ALWAYS,
      unsigned int FailOp_           = GL_KEEP,
      unsigned int ZFailOp_          = GL_KEEP,
      unsigned int ZPassOp_          = GL_KEEP,
      unsigned int Ref_              = 0,
      unsigned int Mask_             = 0xffffffff);

    // Set stencil test for the back face.
    inline void SetStencilBackFace(
      bool EnableTwoSideStencil_,                     // GL_TRUE enable Two Sided Stencil test
      unsigned int Func_             = GL_ALWAYS,
      unsigned int FailOp_           = GL_KEEP,
      unsigned int ZFailOp_          = GL_KEEP,
      unsigned int ZPassOp_          = GL_KEEP,
      unsigned int Ref_              = 0,
      unsigned int Mask_             = 0xffffffff);

    inline void SetFrontFaceStencilWriteMask(
      unsigned int WriteMask_        = 0xffffffff);

    inline void SetBackFaceStencilWriteMask(
      unsigned int WriteMask_        = 0xffffffff);
#endif

    inline void EnableLineSmooth(
      bool EnableLineSmooth = TRUE,
      unsigned int  LineWidth = 1,
      unsigned int HINT = GL_FASTEST);

    inline void SetColorMask(
      unsigned int bRed      = TRUE,
      unsigned int bGreen    = TRUE,
      unsigned int bBlue     = TRUE,
      unsigned int bAlpha    = TRUE);

    inline void GetColorMask(
      unsigned int& bRed,
      unsigned int& bGreen,
      unsigned int& bBlue,
      unsigned int& bAlpha);
    
    inline void SetDepthMask(unsigned int bDepth = TRUE);

    inline void EnableScissor(unsigned int bScissor = FALSE);
#ifndef NUX_OPENGLES_20
    inline void SetPolygonMode(unsigned int FrontMode = GL_FILL, unsigned int BackMode = GL_FILL);
#else
    inline void SetPolygonMode(unsigned int FrontMode, unsigned int BackMode);
#endif

    inline void SetPolygonOffset(unsigned int bEnable,
                                  float Factor = 0.0f, float Units = 0.0f);

  private:

    GpuBrand gpu_brand_;
    GpuInfo* gpu_info_;

#ifndef NUX_OPENGLES_20
    inline void HW__EnableAlphaTest(unsigned int b);
    inline void HW__SetAlphaTestFunc(
      unsigned int AlphaTestFunc_,
      BYTE  AlphaTestRef_);
#endif

    inline void HW__EnableAlphaBlend(unsigned int b);

    inline void HW__SetSeparateAlphaBlend_Enable(unsigned int b);

    inline void HW__SetSeparateAlphaBlendFactors(
      unsigned int SrcBlendFactor_,
      unsigned int DestBlendFactor_,
      unsigned int SrcFactorAlpha_,
      unsigned int DestFactorAlpha_);

    inline void HW__SetAlphaBlendOp(unsigned int BlendOpRGB_,
                                     unsigned int BlendOpAlpha_);

    inline void HW__EnableCulling(unsigned int b);

    inline void HW__SetFrontFace(unsigned int FrontFace_);

    inline void HW__SetCullFace(unsigned int CullFace_);

    inline void HW__SetEnableDepthTest(unsigned int b);
    inline void HW__SetDepthFunc(unsigned int Func);
    inline void HW__SetDepthRange(float zNear, float zFar);


    inline void HW__EnableStencil(unsigned int b);
    inline void HW__SetStencilOp(unsigned int FailOp_, unsigned int ZFailOp_, unsigned int ZPassOp_);
    inline void HW__SetStencilFunc(unsigned int func, int ref, unsigned int mask);

#if 0
    inline void HW__EnableTwoSidedStencil(unsigned int b);
    inline void HW__SetStencilFrontFaceWriteMask(unsigned int WriteMask_);
    inline void HW__SetStencilBackFaceWriteMask(unsigned int WriteMask_);

    inline void HW__SetFrontFaceStencilFunc(unsigned int Func_,
        unsigned int Ref_,
        unsigned int Mask_);

    inline void HW__SetBackFaceStencilFunc(
      unsigned int Func_,
      unsigned int Ref_,
      unsigned int Mask_);

    inline void HW__SetFrontFaceStencilOp(
      unsigned int FailOp_,
      unsigned int ZFailOp_,
      unsigned int ZPassOp_);

    inline void HW__SetBackFaceStencilOp(
      unsigned int FailOp_,
      unsigned int ZFailOp_,
      unsigned int ZPassOp_);
#endif

#ifndef NUX_OPENGLES_20
    inline void HW__EnableLineSmooth(unsigned int EnableLineSmooth);
#endif
    inline void HW__SetLineWidth(unsigned int width, unsigned int HINT);

    inline void HW__SetColorMask(unsigned int bRed, unsigned int bGreen, unsigned int bBlue, unsigned int bAlpha);
    inline void HW__SetDepthMask(unsigned int bDepth);

    inline void HW__EnableScissor(unsigned int bScissor);


#ifndef NUX_OPENGLES_20
    inline void HW__SetPolygonMode(unsigned int FrontMode, unsigned int BackMode);
#endif
  private:
    RenderStateMap render_state_changes_[GFXRS_MAX_RENDERSTATES];
    RenderStateMap sampler_state_changes_[4][GFXSS_MAX_SAMPLERSTATES];
  };



#define SET_RS_VALUE(a, b)  (a).iValue = (b)
#define RS_VALUE(a)      (a).iValue

//#define SET_RS_VALUE_FLOAT(a, b)  (a).fValue = (b)
//#define RS_VALUE_FLOAT(a, b)      (a).fValue

#ifndef NUX_OPENGLES_20
  inline void GpuRenderStates::SetAlphaTest(
    bool EnableAlphaTest_,
    unsigned int AlphaTestFunc_,
    BYTE AlphaTestRef_)
  {
    if (EnableAlphaTest_)
    {
      if (!RS_VALUE(render_state_changes_[GFXRS_ALPHATESTENABLE]))
      {
        HW__EnableAlphaTest(TRUE);
      }

      if ((RS_VALUE(render_state_changes_[GFXRS_ALPHATESTFUNC]) != AlphaTestFunc_) ||
           (RS_VALUE(render_state_changes_[GFXRS_ALPHATESTREF]) != AlphaTestRef_))
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
#endif

  inline void GpuRenderStates::SetBlend(bool AlphaBlendEnable_)
  {
    if (AlphaBlendEnable_)
    {
      if (!RS_VALUE(render_state_changes_[GFXRS_ALPHABLENDENABLE]))
      {
        HW__EnableAlphaBlend(TRUE);
      }
    }
    else
    {
      if (RS_VALUE(render_state_changes_[GFXRS_ALPHABLENDENABLE]))
      {
        HW__EnableAlphaBlend(GL_FALSE);
      }
    }
  }

  inline void GpuRenderStates::SetBlend(bool AlphaBlendEnable_,
                                        unsigned int SrcBlendFactor_,
                                        unsigned int DestBlendFactor_)
  {
    SetBlend(AlphaBlendEnable_);

    if ((RS_VALUE(render_state_changes_[GFXRS_SRCBLEND]) != SrcBlendFactor_) ||
         (RS_VALUE(render_state_changes_[GFXRS_DESTBLEND]) != DestBlendFactor_))
    {
      HW__SetSeparateAlphaBlendFactors(
        SrcBlendFactor_,
        DestBlendFactor_,
        SrcBlendFactor_,
        DestBlendFactor_);
    }
  }

  inline void GpuRenderStates::GetBlend(unsigned int& AlphaBlendEnable_,
                    unsigned int& SrcBlendFactor_,
                    unsigned int& DestBlendFactor_)
  {
    AlphaBlendEnable_ = RS_VALUE(render_state_changes_[GFXRS_ALPHABLENDENABLE]);
    SrcBlendFactor_   = RS_VALUE(render_state_changes_[GFXRS_SRCBLEND]);
    DestBlendFactor_  = RS_VALUE(render_state_changes_[GFXRS_DESTBLEND]);
  }
    
  inline void GpuRenderStates::SetSeparateBlend(bool EnableSeparateAlphaBlend,
      unsigned int SrcBlendFactor_,
      unsigned int DestBlendFactor_,
      unsigned int SrcBlendFactorAlpha_,
      unsigned int DestBlendFactorAlpha_)
  {
    SetBlend(EnableSeparateAlphaBlend);

//     if (EnableSeparateAlphaBlend)
//     {
//         if (!RS_VALUE(render_state_changes_[GFXRS_ALPHABLENDENABLE]))
//         {
//             HW__EnableAlphaBlend(TRUE);
//         }

    if ((RS_VALUE(render_state_changes_[GFXRS_SRCBLEND]) != SrcBlendFactor_) ||
         (RS_VALUE(render_state_changes_[GFXRS_DESTBLEND]) != DestBlendFactor_) ||
         (RS_VALUE(render_state_changes_[GFXRS_SRCBLENDALPHA]) != SrcBlendFactorAlpha_) ||
         (RS_VALUE(render_state_changes_[GFXRS_DESTBLENDALPHA]) != DestBlendFactorAlpha_))
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

  inline void GpuRenderStates::SetPremultipliedBlend(PorterDuffOperator op)
  {
    static const struct
    {
      const unsigned int src_blend;
      const unsigned int dst_blend;
    } factor[13] =
    {
      { GL_ZERO,                GL_ZERO                }, // CLEAR
      { GL_ONE,                 GL_ZERO                }, // SRC
      { GL_ZERO,                GL_ONE                 }, // DST
      { GL_ONE,                 GL_ONE_MINUS_SRC_ALPHA }, // SRC_OVER
      { GL_ONE_MINUS_DST_ALPHA, GL_ONE                 }, // DST_OVER
      { GL_DST_ALPHA,           GL_ZERO                }, // SRC_IN
      { GL_ZERO,                GL_SRC_ALPHA           }, // DST_IN
      { GL_ONE_MINUS_DST_ALPHA, GL_ZERO                }, // SRC_OUT
      { GL_ZERO,                GL_ONE_MINUS_SRC_ALPHA }, // DST_OUT
      { GL_DST_ALPHA,           GL_ONE_MINUS_SRC_ALPHA }, // SRC_ATOP
      { GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA           }, // DST_ATOP
      { GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA }, // XOR
      { GL_ONE,                 GL_ONE                 }  // PLUS
    };

    if ((RS_VALUE(render_state_changes_[GFXRS_SRCBLEND]) !=
         factor[op].src_blend) ||
        (RS_VALUE(render_state_changes_[GFXRS_DESTBLEND]) !=
         factor[op].dst_blend))
    {
      HW__SetSeparateAlphaBlendFactors
        (factor[op].src_blend, factor[op].dst_blend,
         factor[op].src_blend, factor[op].dst_blend);
    }
  }

  inline void GpuRenderStates::SetBlendOp(unsigned int BlendOp)
  {
    if ((RS_VALUE(render_state_changes_[GFXRS_BLENDOP]) != BlendOp))
    {
      HW__SetAlphaBlendOp(BlendOp, BlendOp);
    }
  }

  inline void GpuRenderStates::SetSeparateBlendOp(
    unsigned int BlendOpRGB_,
    unsigned int BlendOpAlpha_)
  {
    if ((RS_VALUE(render_state_changes_[GFXRS_BLENDOP]) != BlendOpRGB_) ||
         (RS_VALUE(render_state_changes_[GFXRS_BLENDOPALPHA]) != BlendOpAlpha_))
    {
      HW__SetAlphaBlendOp(BlendOpRGB_, BlendOpAlpha_);
    }
  }

  inline void GpuRenderStates::SetCullMode(bool EnableCullFace,
      unsigned int FrontFace_,
      unsigned int Cullface_)
  {
    if (EnableCullFace)
    {
      if (!RS_VALUE(render_state_changes_[GFXRS_CULLFACEENABLE]))
      {
        HW__EnableCulling(TRUE);
      }

      if (RS_VALUE(render_state_changes_[GFXRS_FRONTFACE]) != FrontFace_)
      {
        HW__SetFrontFace(FrontFace_);
      }

      if (RS_VALUE(render_state_changes_[GFXRS_CULLFACE]) != Cullface_)
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

  inline void GpuRenderStates::SetDepthTest(bool EnableDepthTest,
      unsigned int WriteEnable_,
      unsigned int DepthFunc_)
  {
    if (EnableDepthTest)
    {
      if (!RS_VALUE(render_state_changes_[GFXRS_ZTESTENABLE]))
      {
        HW__SetEnableDepthTest(TRUE);
      }

      if (RS_VALUE(render_state_changes_[GFXRS_ZWRITEENABLE]) != WriteEnable_)
      {
        HW__SetDepthMask(WriteEnable_);
      }

      if (RS_VALUE(render_state_changes_[GFXRS_ZFUNC]) != DepthFunc_)
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

  inline void GpuRenderStates::SetDepthRange(float zNear, float zFar)
  {
    if ((RS_VALUE(render_state_changes_[GFXRS_ZNEAR]) != static_cast<unsigned int> (zNear)) ||
         (RS_VALUE(render_state_changes_[GFXRS_ZFAR]) != static_cast<unsigned int> (zFar)))
    {
      HW__SetDepthRange(zNear, zFar);
    }
  }

  void GpuRenderStates::SetStencil(bool enable_stencil)
  {
    if (enable_stencil)
    {
      if (!RS_VALUE(render_state_changes_[GFXRS_STENCILENABLE]))
      {
        HW__EnableStencil(TRUE);
      }
    }
    else
    {
      HW__EnableStencil(FALSE);
    }
  }

  void GpuRenderStates::SetStencilFunc(unsigned int func, int ref, unsigned int mask)
  {
    if (
      (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILFUNC]) != func) ||
      (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILREF]) != (unsigned int)ref) ||
      (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILMASK]) != mask)
      )
    {
      HW__SetStencilFunc(func, ref, mask);
    }
  }

  void GpuRenderStates::SetStencilOp(unsigned int stencil_fail, unsigned int stencil_pass_depth_fail, unsigned int stencil_pass_depth_pass)
  {
    if (
      (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILFAIL]) != stencil_fail) ||
      (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILZFAIL]) != stencil_pass_depth_fail) ||
      (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILZPASS]) != stencil_pass_depth_pass)
      )
    {
      HW__SetStencilOp(stencil_fail, stencil_pass_depth_fail, stencil_pass_depth_pass);
    }
  }

#if 0
  inline void GpuRenderStates::SetStencilFrontFace(
    bool EnableStencil_,                            // GL_TRUE enable stencil test
    unsigned int Func_,
    unsigned int FailOp_,
    unsigned int ZFailOp_,
    unsigned int ZPassOp_,
    unsigned int Ref_,
    unsigned int Mask_)
  {
    if (EnableStencil_)
    {
      if (!RS_VALUE(render_state_changes_[GFXRS_STENCILENABLE]))
      {
        HW__EnableStencil(TRUE);
      }

      if (
        (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILFUNC]) != Func_) ||
        (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILREF]) != Ref_) ||
        (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILMASK]) != Mask_) ||
        (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILFAIL]) != FailOp_) ||
        (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILZFAIL]) != ZFailOp_) ||
        (RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILZPASS]) != ZPassOp_)
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

  inline void GpuRenderStates::SetStencilBackFace(
    bool EnableTwoSideStencil_,                     // GL_TRUE enable Two Sided Stencil test
    unsigned int Func_,
    unsigned int FailOp_,
    unsigned int ZFailOp_,
    unsigned int ZPassOp_,
    unsigned int Ref_,
    unsigned int Mask_)
  {
    if (EnableTwoSideStencil_)
    {
      if (!RS_VALUE(render_state_changes_[GFXRS_STENCILENABLE]))
      {
        HW__EnableStencil(TRUE);
      }

      if (!RS_VALUE(render_state_changes_[GFXRS_TWOSIDEDSTENCILENABLE]))
      {
        HW__EnableTwoSidedStencil(TRUE);
      }

      if (
        (RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILFUNC]) != Func_) ||
        (RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILREF]) != Ref_) ||
        (RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILMASK]) != Mask_) ||
        (RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILFAIL]) != FailOp_) ||
        (RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILZFAIL]) != ZFailOp_) ||
        (RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILZPASS]) != ZPassOp_)
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

  inline void GpuRenderStates::SetFrontFaceStencilWriteMask(
    unsigned int WriteMask_)
  {
    HW__SetStencilFrontFaceWriteMask(WriteMask_);
  }

  inline void GpuRenderStates::SetBackFaceStencilWriteMask(
    unsigned int WriteMask_)
  {
    HW__SetStencilBackFaceWriteMask(WriteMask_);
  }
#endif

  inline void GpuRenderStates::EnableLineSmooth(
    bool EnableLineSmooth,
    unsigned int  LineWidth,
    unsigned int Hint)
  {
    if (EnableLineSmooth)
    {
      if (!RS_VALUE(render_state_changes_[GFXRS_LINESMOOTHENABLE]))
      {
#ifndef NUX_OPENGLES_20        
        HW__EnableLineSmooth(GL_TRUE);
#endif
      }

      if ((RS_VALUE(render_state_changes_[GFXRS_LINEWIDTH]) != LineWidth) ||
           (RS_VALUE(render_state_changes_[GFXRS_LINEHINT]) != Hint))
      {
        HW__SetLineWidth(LineWidth, Hint);
      }
    }
    else
    {
#ifndef NUX_OPENGLES_20      
      HW__EnableLineSmooth(GL_FALSE);
#endif
      HW__SetLineWidth(LineWidth, Hint);
    }
  }

  inline void GpuRenderStates::SetColorMask(
    unsigned int bRed,
    unsigned int bGreen,
    unsigned int bBlue,
    unsigned int bAlpha)
  {
    if ((RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_R]) != bRed) ||
         (RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_G]) != bGreen) ||
         (RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_B]) != bBlue) ||
         (RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_A]) != bAlpha))
    {
      HW__SetColorMask(bRed, bGreen, bBlue, bAlpha);
    }

  }

  inline void GpuRenderStates::GetColorMask(
    unsigned int& bRed,
    unsigned int& bGreen,
    unsigned int& bBlue,
    unsigned int& bAlpha)
  {
    bRed    = RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_R]);
    bGreen  = RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_G]);
    bBlue   = RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_B]);
    bAlpha  = RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_A]);
  }
  
  inline void GpuRenderStates::SetDepthMask(unsigned int bDepth)
  {
    if ((RS_VALUE(render_state_changes_[GFXRS_ZWRITEENABLE]) != bDepth))
    {
      HW__SetDepthMask(bDepth);
    }
  }

  inline void GpuRenderStates::EnableScissor(unsigned int bScissor)
  {
    if ((RS_VALUE(render_state_changes_[GFXRS_SCISSORTESTENABLE]) != bScissor))
    {
      HW__EnableScissor(bScissor);
    }
  }

#ifndef NUX_OPENGLES_20
  inline void GpuRenderStates::SetPolygonMode(unsigned int FrontMode, unsigned int BackMode)
  {
    if ((RS_VALUE(render_state_changes_[GFXRS_FRONT_POLYGONMODE]) != FrontMode) ||
         (RS_VALUE(render_state_changes_[GFXRS_BACK_POLYGONMODE]) != BackMode))
    {
      HW__SetPolygonMode(FrontMode, BackMode);
    }
  }
#else
  inline void GpuRenderStates::SetPolygonMode(unsigned int /*FrontMode*/, unsigned int /*BackMode*/)
  {
  }
#endif

#ifndef NUX_OPENGLES_20
  inline void GpuRenderStates::HW__EnableAlphaTest(unsigned int b)
  {
    if (b)
    {
      CHECKGL(glEnable(GL_ALPHA_TEST));
    }
    else
    {
      CHECKGL(glDisable(GL_ALPHA_TEST));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_ALPHATESTENABLE], b ? GL_TRUE : GL_FALSE);
  }

  inline void GpuRenderStates::HW__SetAlphaTestFunc(unsigned int AlphaTestFunc_,
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
      "Error(HW__SetAlphaTestFunc): Invalid Alpha Test Function RenderState");

    CHECKGL(glAlphaFunc(AlphaTestFunc_, (float) AlphaTestRef_ * (1.0f / 255.0f)));
    SET_RS_VALUE(render_state_changes_[GFXRS_ALPHATESTFUNC], AlphaTestFunc_);
    SET_RS_VALUE(render_state_changes_[GFXRS_ALPHATESTREF], AlphaTestRef_);
  }
#endif

  inline void GpuRenderStates::HW__EnableAlphaBlend(unsigned int b)
  {
    if (b)
    {
      CHECKGL(glEnable(GL_BLEND));
    }
    else
    {
      CHECKGL(glDisable(GL_BLEND));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_ALPHABLENDENABLE], b ? GL_TRUE : GL_FALSE);
  }

  inline void GpuRenderStates::HW__SetSeparateAlphaBlendFactors(
    unsigned int SrcBlendFactor_,
    unsigned int DestBlendFactor_,
    unsigned int SrcFactorAlpha_,
    unsigned int DestFactorAlpha_)
  {
    nuxAssertMsg((SrcBlendFactor_ == GL_ZERO) || (SrcBlendFactor_ == GL_ONE) || (SrcBlendFactor_ == GL_SRC_COLOR) || (SrcBlendFactor_ == GL_ONE_MINUS_SRC_COLOR) || (SrcBlendFactor_ == GL_DST_COLOR) || (SrcBlendFactor_ == GL_ONE_MINUS_DST_COLOR) || (SrcBlendFactor_ == GL_SRC_ALPHA) || (SrcBlendFactor_ == GL_ONE_MINUS_SRC_ALPHA) || (SrcBlendFactor_ == GL_DST_ALPHA) || (SrcBlendFactor_ == GL_ONE_MINUS_DST_ALPHA) || (SrcBlendFactor_ == GL_CONSTANT_COLOR) || (SrcBlendFactor_ == GL_ONE_MINUS_CONSTANT_COLOR) || (SrcBlendFactor_ == GL_CONSTANT_ALPHA) || (SrcBlendFactor_ == GL_ONE_MINUS_CONSTANT_ALPHA) || (SrcBlendFactor_ == GL_SRC_ALPHA_SATURATE),
                   "Error(HW__SetSeparateAlphaBlendFactors): Invalid Blend RenderState");
    nuxAssertMsg((DestBlendFactor_ == GL_ZERO) || (DestBlendFactor_ == GL_ONE) || (DestBlendFactor_ == GL_SRC_COLOR) || (DestBlendFactor_ == GL_ONE_MINUS_SRC_COLOR) || (DestBlendFactor_ == GL_DST_COLOR) || (DestBlendFactor_ == GL_ONE_MINUS_DST_COLOR) || (DestBlendFactor_ == GL_SRC_ALPHA) || (DestBlendFactor_ == GL_ONE_MINUS_SRC_ALPHA) || (DestBlendFactor_ == GL_DST_ALPHA) || (DestBlendFactor_ == GL_ONE_MINUS_DST_ALPHA) || (DestBlendFactor_ == GL_CONSTANT_COLOR) || (DestBlendFactor_ == GL_ONE_MINUS_CONSTANT_COLOR) || (DestBlendFactor_ == GL_CONSTANT_ALPHA) || (DestBlendFactor_ == GL_ONE_MINUS_CONSTANT_ALPHA),
                   "Error(HW__SetSeparateAlphaBlendFactors): Invalid Blend RenderState");
    nuxAssertMsg((SrcFactorAlpha_ == GL_ZERO) || (SrcFactorAlpha_ == GL_ONE) || (SrcFactorAlpha_ == GL_SRC_COLOR) || (SrcFactorAlpha_ == GL_ONE_MINUS_SRC_COLOR) || (SrcFactorAlpha_ == GL_DST_COLOR) || (SrcFactorAlpha_ == GL_ONE_MINUS_DST_COLOR) || (SrcFactorAlpha_ == GL_SRC_ALPHA) || (SrcFactorAlpha_ == GL_ONE_MINUS_SRC_ALPHA) || (SrcFactorAlpha_ == GL_DST_ALPHA) || (SrcFactorAlpha_ == GL_ONE_MINUS_DST_ALPHA) || (SrcFactorAlpha_ == GL_CONSTANT_COLOR) || (SrcFactorAlpha_ == GL_ONE_MINUS_CONSTANT_COLOR) || (SrcFactorAlpha_ == GL_CONSTANT_ALPHA) || (SrcFactorAlpha_ == GL_ONE_MINUS_CONSTANT_ALPHA) || (SrcFactorAlpha_ == GL_SRC_ALPHA_SATURATE),
                   "Error(HW__SetSeparateAlphaBlendFactors): Invalid Blend RenderState");
    nuxAssertMsg((DestFactorAlpha_ == GL_ZERO) || (DestFactorAlpha_ == GL_ONE) || (DestFactorAlpha_ == GL_SRC_COLOR) || (DestFactorAlpha_ == GL_ONE_MINUS_SRC_COLOR) || (DestFactorAlpha_ == GL_DST_COLOR) || (DestFactorAlpha_ == GL_ONE_MINUS_DST_COLOR) || (DestFactorAlpha_ == GL_SRC_ALPHA) || (DestFactorAlpha_ == GL_ONE_MINUS_SRC_ALPHA) || (DestFactorAlpha_ == GL_DST_ALPHA) || (DestFactorAlpha_ == GL_ONE_MINUS_DST_ALPHA) || (DestFactorAlpha_ == GL_CONSTANT_COLOR) || (DestFactorAlpha_ == GL_ONE_MINUS_CONSTANT_COLOR) || (DestFactorAlpha_ == GL_CONSTANT_ALPHA) || (DestFactorAlpha_ == GL_ONE_MINUS_CONSTANT_ALPHA),
                   "Error(HW__SetSeparateAlphaBlendFactors): Invalid Blend RenderState");

    CHECKGL(glBlendFuncSeparate(
               SrcBlendFactor_,
               DestBlendFactor_,
               SrcFactorAlpha_,
               DestFactorAlpha_));

    SET_RS_VALUE(render_state_changes_[GFXRS_SRCBLEND], SrcBlendFactor_);
    SET_RS_VALUE(render_state_changes_[GFXRS_DESTBLEND], DestBlendFactor_);
    SET_RS_VALUE(render_state_changes_[GFXRS_SRCBLENDALPHA], SrcFactorAlpha_);
    SET_RS_VALUE(render_state_changes_[GFXRS_DESTBLENDALPHA], DestFactorAlpha_);
  }

  inline void GpuRenderStates::HW__SetAlphaBlendOp(
    unsigned int BlendOpRGB_,
    unsigned int BlendOpAlpha_)
  {
#ifdef NUX_OPENGLES_20
    nuxAssertMsg(
      (BlendOpRGB_ == GL_FUNC_ADD) ||
      (BlendOpRGB_ == GL_FUNC_SUBTRACT) ||
      (BlendOpRGB_ == GL_FUNC_REVERSE_SUBTRACT),
      "Error(HW__SetAlphaBlendOp): Invalid Blend Equation RenderState");
    nuxAssertMsg(
      (BlendOpAlpha_ == GL_FUNC_ADD) ||
      (BlendOpAlpha_ == GL_FUNC_SUBTRACT) ||
      (BlendOpAlpha_ == GL_FUNC_REVERSE_SUBTRACT),
      "Error(HW__SetAlphaBlendOp): Invalid Blend Equation RenderState");

    CHECKGL(glBlendEquationSeparate(BlendOpRGB_, BlendOpAlpha_));

#else
    nuxAssertMsg(
      (BlendOpRGB_ == GL_FUNC_ADD) ||
      (BlendOpRGB_ == GL_FUNC_SUBTRACT) ||
      (BlendOpRGB_ == GL_FUNC_REVERSE_SUBTRACT) ||
      (BlendOpRGB_ == GL_MIN) ||
      (BlendOpRGB_ == GL_MAX),
      "Error(HW__SetAlphaBlendOp): Invalid Blend Equation RenderState");
    nuxAssertMsg(
      (BlendOpAlpha_ == GL_FUNC_ADD) ||
      (BlendOpAlpha_ == GL_FUNC_SUBTRACT) ||
      (BlendOpAlpha_ == GL_FUNC_REVERSE_SUBTRACT) ||
      (BlendOpAlpha_ == GL_MIN) ||
      (BlendOpAlpha_ == GL_MAX),
      "Error(HW__SetAlphaBlendOp): Invalid Blend Equation RenderState");

    if (gpu_info_->SupportOpenGL20())
    {
      CHECKGL(glBlendEquationSeparate(BlendOpRGB_, BlendOpAlpha_));
    }
    else if (gpu_info_->Support_EXT_Blend_Equation_Separate())
    {
      CHECKGL(glBlendEquationSeparateEXT(BlendOpRGB_, BlendOpAlpha_));
    }
    else
    {
      CHECKGL(glBlendEquation(BlendOpRGB_));
    }
#endif

    SET_RS_VALUE(render_state_changes_[GFXRS_BLENDOP], BlendOpRGB_);
    SET_RS_VALUE(render_state_changes_[GFXRS_BLENDOPALPHA], BlendOpAlpha_);
  }

  inline void GpuRenderStates::HW__EnableCulling(unsigned int b)
  {
    if (b)
    {
      CHECKGL(glEnable(GL_CULL_FACE));
    }
    else
    {
      CHECKGL(glDisable(GL_CULL_FACE));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_CULLFACEENABLE], b ? GL_TRUE : GL_FALSE);
  }


  inline void GpuRenderStates::HW__SetFrontFace(unsigned int FrontFace_)
  {
    nuxAssertMsg(
      (FrontFace_ == GL_CW) ||
      (FrontFace_ == GL_CCW),
      "Error(HW__SetFrontFace): Invalid Front Face RenderState");

    CHECKGL(glFrontFace(FrontFace_));
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONTFACE], FrontFace_);
  }

  inline void GpuRenderStates::HW__SetCullFace(unsigned int CullFace_)
  {
    nuxAssertMsg(
      (CullFace_ == GL_FRONT) ||
      (CullFace_ == GL_BACK) ||
      (CullFace_ == GL_FRONT_AND_BACK),
      "Error(HW__SetCullFace): Invalid Cull Face RenderState");

    CHECKGL(glCullFace(CullFace_));
    SET_RS_VALUE(render_state_changes_[GFXRS_CULLFACE], CullFace_);
  }

  inline void GpuRenderStates::HW__SetEnableDepthTest(unsigned int b)
  {
    if (b)
    {
      CHECKGL(glEnable(GL_DEPTH_TEST));
    }
    else
    {
      CHECKGL(glDisable(GL_DEPTH_TEST));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_ZTESTENABLE], b ? GL_TRUE : GL_FALSE);
  }

  inline void GpuRenderStates::HW__SetDepthRange(float zNear, float zFar)
  {
    CHECKGL(glDepthRange(zNear, zFar));
    SET_RS_VALUE(render_state_changes_[GFXRS_ZNEAR], static_cast<unsigned int> (Clamp(zNear, 0.0f, 1.0f)));
    SET_RS_VALUE(render_state_changes_[GFXRS_ZFAR], static_cast<unsigned int> (Clamp(zFar, 0.0f, 1.0f)));
  }

  inline void GpuRenderStates::HW__SetDepthFunc(unsigned int Func)
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
      "Error(HW__SetDepthFunc): Invalid Depth Func RenderState");

    CHECKGL(glDepthFunc(Func));
    SET_RS_VALUE(render_state_changes_[GFXRS_ZFUNC], Func);
  }

  inline void GpuRenderStates::HW__EnableStencil(unsigned int b)
  {
    if (b)
    {
      CHECKGL(glEnable(GL_STENCIL_TEST));
    }
    else
    {
      CHECKGL(glDisable(GL_STENCIL_TEST));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_STENCILENABLE], b ? GL_TRUE : GL_FALSE);
  }

  inline void GpuRenderStates::HW__SetStencilFunc(unsigned int func, int ref, unsigned int mask)
  {
    nuxAssertMsg(
      (func == GL_NEVER) ||
      (func == GL_LESS) ||
      (func == GL_EQUAL) ||
      (func == GL_LEQUAL) ||
      (func == GL_GREATER) ||
      (func == GL_NOTEQUAL) ||
      (func == GL_GEQUAL) ||
      (func == GL_ALWAYS),
      "Error(HW__SetFrontFaceStencilFunc): Invalid Stencil Function RenderState");

    CHECKGL(glStencilFunc(func, ref, mask));

    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILFUNC], func);
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILREF], ref);
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILMASK], mask);
  }

  inline void GpuRenderStates::HW__SetStencilOp(unsigned int stencil_fail, unsigned int stencil_pass_depth_fail, unsigned int stencil_pass_depth_pass)
  {
    nuxAssertMsg(
      (stencil_fail == GL_KEEP) ||
      (stencil_fail == GL_ZERO) ||
      (stencil_fail == GL_REPLACE) ||
      (stencil_fail == GL_INCR) ||
      (stencil_fail == GL_INCR_WRAP) ||
      (stencil_fail == GL_DECR) ||
      (stencil_fail == GL_DECR_WRAP) ||
      (stencil_fail == GL_INVERT),
      "Error(HW__SetFrontFaceStencilOp): Invalid FailOp RenderState");

    nuxAssertMsg(
      (stencil_pass_depth_fail == GL_KEEP) ||
      (stencil_pass_depth_fail == GL_ZERO) ||
      (stencil_pass_depth_fail == GL_REPLACE) ||
      (stencil_pass_depth_fail == GL_INCR) ||
      (stencil_pass_depth_fail == GL_INCR_WRAP) ||
      (stencil_pass_depth_fail == GL_DECR) ||
      (stencil_pass_depth_fail == GL_DECR_WRAP) ||
      (stencil_pass_depth_fail == GL_INVERT),
      "Error(HW__SetFrontFaceStencilOp): Invalid ZFailOp RenderState");

    nuxAssertMsg(
      (stencil_pass_depth_pass == GL_KEEP) ||
      (stencil_pass_depth_pass == GL_ZERO) ||
      (stencil_pass_depth_pass == GL_REPLACE) ||
      (stencil_pass_depth_pass == GL_INCR) ||
      (stencil_pass_depth_pass == GL_INCR_WRAP) ||
      (stencil_pass_depth_pass == GL_DECR) ||
      (stencil_pass_depth_pass == GL_DECR_WRAP) ||
      (stencil_pass_depth_pass == GL_INVERT),
      "Error(HW__SetFrontFaceStencilOp): Invalid ZPassOp RenderState");

    CHECKGL(glStencilOp(stencil_fail, stencil_pass_depth_fail, stencil_pass_depth_pass));

    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILFAIL], stencil_fail);
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILZFAIL], stencil_pass_depth_fail);
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILZPASS], stencil_pass_depth_pass);
  }

#if 0
  inline void GpuRenderStates::HW__EnableTwoSidedStencil(unsigned int b)
  {
    if (b)
    {
      if (gpu_brand_ == GPU_BRAND_AMD)
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
      if (gpu_brand_ == GPU_BRAND_AMD)
      {
        CHECKGL(glDisable(GL_STENCIL_TEST));
      }
      else
      {
        CHECKGL(glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT));
      }
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_TWOSIDEDSTENCILENABLE], b ? GL_TRUE : GL_FALSE);
  }

  inline void GpuRenderStates::HW__SetStencilFrontFaceWriteMask(unsigned int WriteMask_)
  {
    CHECKGL(glActiveStencilFaceEXT(GL_FRONT));
    CHECKGL(glStencilMask(WriteMask_));
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILWRITEMASK], WriteMask_);
  }

  inline void GpuRenderStates::HW__SetStencilBackFaceWriteMask(unsigned int WriteMask_)
  {
    CHECKGL(glActiveStencilFaceEXT(GL_BACK));
    CHECKGL(glStencilMask(WriteMask_));
    SET_RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILWRITEMASK], WriteMask_);
  }

  inline void GpuRenderStates::HW__SetFrontFaceStencilFunc(unsigned int Func_,
      unsigned int Ref_,
      unsigned int Mask_)
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
      "Error(HW__SetFrontFaceStencilFunc): Invalid Stencil Function RenderState");

    CHECKGL(glActiveStencilFaceEXT(GL_FRONT));
    CHECKGL(glStencilFunc(Func_, Ref_, Mask_));

    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILFUNC], Func_);
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILREF], Ref_);
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILMASK], Mask_);
  }

  inline void GpuRenderStates::HW__SetBackFaceStencilFunc(
    unsigned int Func_,
    unsigned int Ref_,
    unsigned int Mask_)
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
      "Error(HW__SetBackFaceStencilFunc): Invalid Stencil Function RenderState");

    if (gpu_brand_ == GPU_BRAND_AMD)
    {
      CHECKGL(glStencilFuncSeparateATI(Func_/*Front function*/, Func_/*Back function*/, Ref_, Mask_)); // incorrect
    }
    else
    {
      CHECKGL(glActiveStencilFaceEXT(GL_BACK));
      CHECKGL(glStencilFunc(Func_, Ref_, Mask_));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILFUNC], Func_);
    SET_RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILREF], Ref_);
    SET_RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILMASK], Mask_);
  }

  inline void GpuRenderStates::HW__SetFrontFaceStencilOp(
    unsigned int FailOp_,
    unsigned int ZFailOp_,
    unsigned int ZPassOp_)
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
      "Error(HW__SetFrontFaceStencilOp): Invalid FailOp RenderState");

    nuxAssertMsg(
      (ZFailOp_ == GL_KEEP) ||
      (ZFailOp_ == GL_ZERO) ||
      (ZFailOp_ == GL_REPLACE) ||
      (ZFailOp_ == GL_INCR) ||
      (ZFailOp_ == GL_INCR_WRAP) ||
      (ZFailOp_ == GL_DECR) ||
      (ZFailOp_ == GL_DECR_WRAP) ||
      (ZFailOp_ == GL_INVERT),
      "Error(HW__SetFrontFaceStencilOp): Invalid ZFailOp RenderState");

    nuxAssertMsg(
      (ZPassOp_ == GL_KEEP) ||
      (ZPassOp_ == GL_ZERO) ||
      (ZPassOp_ == GL_REPLACE) ||
      (ZPassOp_ == GL_INCR) ||
      (ZPassOp_ == GL_INCR_WRAP) ||
      (ZPassOp_ == GL_DECR) ||
      (ZPassOp_ == GL_DECR_WRAP) ||
      (ZPassOp_ == GL_INVERT),
      "Error(HW__SetFrontFaceStencilOp): Invalid ZPassOp RenderState");

    if (gpu_brand_ == GPU_BRAND_AMD)
    {
      CHECKGL(glStencilOpSeparateATI(GL_FRONT, FailOp_, ZFailOp_, ZPassOp_));
    }
    else
    {
      CHECKGL(glActiveStencilFaceEXT(GL_FRONT));
      CHECKGL(glStencilOp(FailOp_, ZFailOp_, ZPassOp_));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILFAIL], FailOp_);
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILZFAIL], ZFailOp_);
    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_STENCILZPASS], ZPassOp_);
  }

  inline void GpuRenderStates::HW__SetBackFaceStencilOp(
    unsigned int FailOp_,
    unsigned int ZFailOp_,
    unsigned int ZPassOp_)
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
      "Error(HW__SetBackFaceStencilOp): Invalid FailOp RenderState");

    nuxAssertMsg(
      (ZFailOp_ == GL_KEEP) ||
      (ZFailOp_ == GL_ZERO) ||
      (ZFailOp_ == GL_REPLACE) ||
      (ZFailOp_ == GL_INCR) ||
      (ZFailOp_ == GL_INCR_WRAP) ||
      (ZFailOp_ == GL_DECR) ||
      (ZFailOp_ == GL_DECR_WRAP) ||
      (ZFailOp_ == GL_INVERT),
      "Error(HW__SetBackFaceStencilOp): Invalid ZFailOp RenderState");

    nuxAssertMsg(
      (ZPassOp_ == GL_KEEP) ||
      (ZPassOp_ == GL_ZERO) ||
      (ZPassOp_ == GL_REPLACE) ||
      (ZPassOp_ == GL_INCR) ||
      (ZPassOp_ == GL_INCR_WRAP) ||
      (ZPassOp_ == GL_DECR) ||
      (ZPassOp_ == GL_DECR_WRAP) ||
      (ZPassOp_ == GL_INVERT),
      "Error(HW__SetBackFaceStencilOp): Invalid ZPassOp RenderState");

    if (gpu_brand_ == GPU_BRAND_AMD)
    {
      CHECKGL(glStencilOpSeparateATI(GL_BACK, FailOp_, ZFailOp_, ZPassOp_));
    }
    else
    {
      CHECKGL(glActiveStencilFaceEXT(GL_BACK));
      CHECKGL(glStencilOp(FailOp_, ZFailOp_, ZPassOp_));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILFAIL], FailOp_);
    SET_RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILZFAIL], ZFailOp_);
    SET_RS_VALUE(render_state_changes_[GFXRS_BACK_STENCILZPASS], ZPassOp_);
  }
#endif

#ifndef NUX_OPENGLES_20
  inline void GpuRenderStates::HW__EnableLineSmooth(unsigned int EnableLineSmooth)
  {

    if (EnableLineSmooth)
    {
      CHECKGL(glEnable(GL_LINE_SMOOTH));
    }
    else
    {
      CHECKGL(glDisable(GL_LINE_SMOOTH));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_LINESMOOTHENABLE], EnableLineSmooth ? GL_TRUE : GL_FALSE);
  }
#endif

  inline void GpuRenderStates::HW__SetLineWidth(unsigned int width,  unsigned int Hint)
  {
    nuxAssertMsg(
      (Hint == GL_NICEST) ||
      (Hint == GL_FASTEST) ||
      (Hint == GL_DONT_CARE),
      "Error(HW__SetLineWidth): Invalid Line Hint RenderState");

    CHECKGL(glLineWidth(width));
    SET_RS_VALUE(render_state_changes_[GFXRS_LINEWIDTH], width);

#ifndef NUX_OPENGLES_20
    CHECKGL(glHint(GL_LINE_SMOOTH_HINT, Hint));
    SET_RS_VALUE(render_state_changes_[GFXRS_LINEHINT], Hint);
#endif
  }

  inline void GpuRenderStates::HW__SetColorMask(
    unsigned int bRed,
    unsigned int bGreen,
    unsigned int bBlue,
    unsigned int bAlpha)
  {
    CHECKGL(glColorMask(bRed, bGreen, bBlue, bAlpha));
    SET_RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_R], bRed);
    SET_RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_G], bGreen);
    SET_RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_B], bBlue);
    SET_RS_VALUE(render_state_changes_[GFXRS_COLORWRITEENABLE_A], bAlpha);
  }

  inline void GpuRenderStates::HW__SetDepthMask(unsigned int bDepth)
  {
    CHECKGL(glDepthMask(bDepth));
    SET_RS_VALUE(render_state_changes_[GFXRS_ZWRITEENABLE], bDepth);
  }

  inline void GpuRenderStates::HW__EnableScissor(unsigned int bScissor)
  {
    if (bScissor)
    {
      CHECKGL(glEnable(GL_SCISSOR_TEST));
    }
    else
    {
      CHECKGL(glDisable(GL_SCISSOR_TEST));
    }

    SET_RS_VALUE(render_state_changes_[GFXRS_SCISSORTESTENABLE], bScissor ? GL_TRUE : GL_FALSE);
  }

#ifndef NUX_OPENGLES_20
  inline void GpuRenderStates::HW__SetPolygonMode(unsigned int FrontMode, unsigned int BackMode)
  {
    nuxAssertMsg(
      (FrontMode == GL_FILL) ||
      (FrontMode == GL_LINE) ||
      (FrontMode == GL_POINT),
      "Error(HW__SetPolygonMode): Invalid Point Hint RenderState");

    nuxAssertMsg(
      (BackMode == GL_FILL) ||
      (BackMode == GL_LINE) ||
      (BackMode == GL_POINT),
      "Error(HW__SetPolygonMode): Invalid Point Hint RenderState");

    CHECKGL(glPolygonMode(GL_FRONT, FrontMode));
    CHECKGL(glPolygonMode(GL_BACK, BackMode));

    SET_RS_VALUE(render_state_changes_[GFXRS_FRONT_POLYGONMODE], FrontMode);
    SET_RS_VALUE(render_state_changes_[GFXRS_BACK_POLYGONMODE], BackMode);
  }
#endif

#undef SET_RS_VALUE
#undef RS_VALUE

//#undef SET_RS_VALUE_FLOAT
//#undef RS_VALUE_FLOAT

}

#endif // GLRENDERSTATES_H

