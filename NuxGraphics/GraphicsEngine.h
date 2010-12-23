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


#ifndef OPENGLENGINE_H
#define OPENGLENGINE_H

#include "GLResource.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"
#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GLRenderStates.h"
#include "FontTexture.h"
#include "RenderingPipe.h"
#include "GLShader.h"

#if defined(NUX_OS_WINDOWS)
  #include "GraphicsDisplay.h"
  #include "GLWindowManager.h"
#elif defined(NUX_OS_LINUX)
  #include "GraphicsDisplay.h"
  #include "GLWindowManager.h"
#endif

namespace nux
{

  class FontTexture;
  class FontRenderer;
  class FilePath;
  class BaseTexture;
  class TextureRectangle;

  class ROPConfig
  {
  public:
    ROPConfig();
    ~ROPConfig();

    bool Blend;
    t_u32 SrcBlend;
    t_u32 DstBlend;

    static ROPConfig Default;
  };

  typedef struct
  {
    int x;
    int y;
    int width;
    int height;

  } OpenGLContext;

  class GraphicsEngine
  {
  public:

    GraphicsEngine (GraphicsDisplay &GlWindow);
    ~GraphicsEngine();

    void SetContext (int x, int y, int width, int height);
    void GetContextSize (int &w, int &h) const;
    int GetContextWidth() const;
    int GetContextHeight() const;
    int GetContextX() const;
    int GetContextY() const;

    void GetWindowSize (int &w, int &h) const;
    int GetWindowWidth() const;
    int GetWindowHeight() const;

    OpenGLContext m_CurrrentContext;

    // Load Textures
    BaseTexture *Load2DTextureFile (const char *filename);
    BaseTexture *Load2DRectangleTextureFile (const char *filename);
    BaseTexture *Load2DTextureFileGenerateAlpha (const char *filename, int red, int green, int blue);

    void SetTexture (int TextureUnit, BaseTexture *Texture);
    void SetTexture (int TextureUnit, ObjectPtr< IOpenGLBaseTexture > Texture);
    void EnableTextureMode (int TextureUnit, int TextureMode);
    void DisableTextureMode (int TextureUnit, int TextureMode);
    void DisableAllTextureMode (int TextureUnit);


    ///////////////////
    // DRAW TEXTURE  //
    ///////////////////

    // Neutral
    void QRP_1Tex (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> Tex0, TexCoordXForm &texxform, const Color &color0);
    void QRP_Color (int x, int y, int width, int height, const Color &c0);
    void QRP_Color (int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3);
    void QRP_ColorModTexAlpha (int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color);

    void QRP_2Tex (int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1);

    void QRP_2TexMod (int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1);

    void QRP_4Tex (int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm &texxform2, const Color &color2,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm &texxform3, const Color &color3);

    void QRP_Triangle (int x0, int y0, int x1, int y1, int x2, int y2, Color c0);
    void QRP_Triangle (int x0, int y0, int x1, int y1, int x2, int y2, Color c0, Color c1, Color c2);
    void QRP_Line (int x0, int y0, int x1, int y1, Color c0);
    void QRP_Line (int x0, int y0, int x1, int y1, Color c0, Color c1);
    void QRP_QuadWireframe (int x0, int y0, int width, int height, Color c0, Color c1, Color c2, Color c3);


    // ASM
    void QRP_ASM_1Tex (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> Tex0, TexCoordXForm &texxform, const Color &color0);
    void QRP_ASM_Color (int x, int y, int width, int height, const Color &c0);
    void QRP_ASM_Color (int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3);
    void QRP_ASM_ColorModTexAlpha (int x, int y, int width, int height,
                               ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color);

    void QRP_ASM_2Tex (int x, int y, int width, int height,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1);

    void QRP_ASM_2TexMod (int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1);

    void QRP_ASM_4Tex (int x, int y, int width, int height,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm &texxform2, const Color &color2,
                   ObjectPtr<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm &texxform3, const Color &color3);

    void QRP_ASM_Triangle (int x0, int y0, int x1, int y1, int x2, int y2, Color c0);
    void QRP_ASM_Triangle (int x0, int y0, int x1, int y1, int x2, int y2, Color c0, Color c1, Color c2);
    void QRP_ASM_Line (int x0, int y0, int x1, int y1, Color c0);
    void QRP_ASM_Line (int x0, int y0, int x1, int y1, Color c0, Color c1);
    void QRP_ASM_QuadWireframe (int x0, int y0, int width, int height, Color c0, Color c1, Color c2, Color c3);

    void QRP_ASM_Exponentiation  (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0, Vector4 exponent);
    void QRP_ASM_AlphaReplicate  (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0);
    void QRP_ASM_HorizontalGauss (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0);
    void QRP_ASM_VerticalGauss   (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0);
    void QRP_ASM_ColorMatrix     (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0, Matrix4 color_matrix, Vector4 offset);

    /*!
        @param device_texture The texture that is to be blurred.
        @param texxform       Texture transformation of device_texture.
        @param x              Position of the source texture in result buffer.
        @param y              Position of the source texture in result buffer.
        @param buffer_width   Width of result texture.
        @param buffer_height  Height of result texture.
    */
    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetBlurTexture (ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
      const Color& c0,
      int x, int y,
      int buffer_width, int buffer_height);

    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetAlphaTexture (ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
      const Color& c0,
      int x, int y,
      int buffer_width, int buffer_height);

    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetColorMatrixTexture (ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
      const Color& c0,
      Matrix4 color_matrix, Vector4 offset,
      int x, int y,
      int buffer_width, int buffer_height);

    ObjectPtr<IOpenGLBaseTexture> QRP_ASM_GetComponentExponentiation (ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
      const Color& c0,
      Vector4 exponent,
      int x, int y,
      int buffer_width, int buffer_height);
    // GLSL

    void QRP_GLSL_1Tex (int x, int y, int width, int height,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform, const Color &c0);
    void QRP_GLSL_Color (int x, int y, int width, int height, const Color &c0);
    void QRP_GLSL_Color (int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3);
    void QRP_GLSL_ColorModTexAlpha (int x, int y, int width, int height,
                                    ObjectPtr<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color);

    void QRP_GLSL_2Tex (int x, int y, int width, int height,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1);

    void QRP_GLSL_2TexMod (int x, int y, int width, int height,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
      ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1);


    void QRP_GLSL_4Tex (int x, int y, int width, int height,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm &texxform2, const Color &color2,
                        ObjectPtr<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm &texxform3, const Color &color3);

    void QRP_GLSL_Triangle      (int x0, int y0, int x1, int y1, int x2, int y2, Color c0);
    void QRP_GLSL_Triangle      (int x0, int y0, int x1, int y1, int x2, int y2, Color c0, Color c1, Color c2);
    void QRP_GLSL_Line          (int x0, int y0, int x1, int y1, Color c0);
    void QRP_GLSL_Line          (int x0, int y0, int x1, int y1, Color c0, Color c1);
    void QRP_GLSL_QuadWireframe (int x0, int y0, int width, int height, Color c0, Color c1, Color c2, Color c3);

    void QRP_GLSL_Exponentiation  (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0, Vector4 exponent);
    void QRP_GLSL_AlphaReplicate  (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0);
    void QRP_GLSL_HorizontalGauss (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0);
    void QRP_GLSL_VerticalGauss   (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0);
    void QRP_GLSL_ColorMatrix     (int x, int y, int width, int height, ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform, const Color &c0, Matrix4 color_matrix, Vector4 offset);

    /*!
        @param device_texture The texture that is to be blurred.
        @param texxform       Texture transformation of device_texture.
        @param x              Position of the source texture in result buffer.
        @param y              Position of the source texture in result buffer.
        @param buffer_width   Width of result texture.
        @param buffer_height  Height of result texture.
    */
    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetBlurTexture (ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
      const Color& c0,
      int x, int y,
      int buffer_width, int buffer_height);

    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetAlphaTexture (ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
      const Color& c0,
      int x, int y,
      int buffer_width, int buffer_height);

    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetColorMatrixTexture (ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
      const Color& c0,
      Matrix4 color_matrix, Vector4 offset,
      int x, int y,
      int buffer_width, int buffer_height);

    ObjectPtr<IOpenGLBaseTexture> QRP_GLSL_GetComponentExponentiation (ObjectPtr<IOpenGLBaseTexture> device_texture, TexCoordXForm &texxform,
      const Color& c0,
      Vector4 exponent,
      int x, int y,
      int buffer_width, int buffer_height);

    //////////////////////
    // DRAW CLIPPING    //
    //////////////////////

    mutable std::vector<Rect> ClippingRect;
    void PushClippingRectangle (Rect rect);
    void PopClippingRectangle ();
    void EmptyClippingRegion ();
    void ApplyClippingRectangle ();
    void SetDrawClippingRegion (int x, int y, unsigned int width, unsigned int height);
    Rect GetClippingRegion () const;
    int GetNumberOfClippingRegions () const;

    void ClearAreaColorDepthStencil (int x, int y, int width, int height, Color clearcolor, float cleardepth, int clearstencil);
    void ClearAreaColor (int x, int y, int width, int height, Color clearcolor);
    void ClearAreaDepthStencil (int x, int y, int width, int height, float cleardepth, int clearstencil);

    void Set3DView (int w, int h);
    void Push2DWindow (int w, int h);
    void Pop2DWindow ();
    void Push2DModelViewMatrix (Matrix4 mat);
    Matrix4 Pop2DModelViewMatrix ();
    void Clear2DModelViewMatrix ();
    std::list<Matrix4> m_2DModelViewMatricesStack;

    void SetEnvModeTextureAlphaBlend (int TextureUnit);
    void SetEnvModeSelectTexture (int TextureUnit);
    void SetEnvModeSelectColor (int TextureUnit);
    void SetEnvModeModulateColorWithTexture (int TextureUnit);


    void SetViewport (int x, int y, int w, int h);
    Rect GetViewportRect ();
    void SetScissor (int x, int y, int w, int h);

    /*!
        When setting a matrix to translate the widgets, the scissor region is not translated accordingly.
        This function provides a mean to offset the scissor region. It is useful when translating a widget during and overlay drawing.
        See an example in the code of NUX-01.
        @param x Clipping area offset.
        @param y Clipping area offset.
    */
    void SetScissorOffset (int x, int y);

    Rect GetScissorRect();
    void EnableScissoring (bool b);


//     void LoadPainterImages();
//     const PainterImage* GetImage(UXStyleImageRef style);
//     Rect GetImageGeometry(UXStyleImageRef style);
//     std::list<PainterImage*> m_PainterImageList;

    int RenderColorText (ObjectPtr<FontTexture> Font, int x, int y, const NString &Str,
                         const Color &TextColor,
                         bool WriteAlphaChannel,
                         int NumCharacter);

    int RenderColorTextLineStatic (ObjectPtr<FontTexture> Font, const PageBBox &pageSize, const NString &Str,
                                   const Color &TextColor,
                                   bool WriteAlphaChannel,
                                   TextAlignment alignment);

    int RenderColorTextLineEdit (ObjectPtr<FontTexture> Font, const PageBBox &pageSize, const NString &Str,
                                 const Color &TextColor,
                                 bool WriteAlphaChannel,
                                 const Color &SelectedTextColor,
                                 const Color &SelectedTextBackgroundColor,
                                 const Color &TextBlinkColor,
                                 const Color &CursorColor,
                                 bool ShowCursor, unsigned int CursorPosition,
                                 int offset = 0, int selection_start = 0, int selection_end = 0);

    ObjectPtr <IOpenGLBaseTexture> CreateTextureFromBackBuffer (int x, int y, int width, int height);

    //Statistics
    void ResetStats();

    /*!
        Cache a resource if it has previously been cached. If the resource does not contain valid data
        then the returned value is not valid. Check that the returned hardware resource is valid by calling ObjectPtr<CachedResourceData>.IsValid().
        @param Resource The resource to cache.
        @return A hardware resource.
    */
    ObjectPtr<CachedResourceData> CacheResource (ResourceData *Resource);

    /*!
        Update a resource if it has previously been cached.
        @param Resource The resource to cache.
    */
    void UpdateResource (ResourceData *Resource);

    bool FlushCachedResourceData (ResourceData *Resource);
    bool IsResourceCached (ResourceData *Resource);
    NResourceCache ResourceCache;

    Matrix4 GetProjectionMatrix();
    Matrix4 GetModelViewMatrix();
    Matrix4 GetModelViewProjectionMatrix();
    Matrix4 GetOpenGLModelViewProjectionMatrix();

    GpuRenderStates &GetRenderStates()
    {
      return *_graphics_display.m_DeviceFactory->_gpu_render_states;
    }
    void ResetRenderStates()
    {
      _graphics_display.m_DeviceFactory->_gpu_render_states->ResetStateChangeToDefault();
    }
    void VerifyRenderStates()
    {
      _graphics_display.m_DeviceFactory->_gpu_render_states->CheckStateChange();
    }

    ObjectPtr<FontTexture> GetFont();
    ObjectPtr<FontTexture> GetBoldFont();

    //! Return True is GraphicsEngine is using the GLSL shader code path.
    /*!
        @return True is the system is using the GLSL shader code path.
    */
    bool UsingGLSLCodePath ();

    //! Return True is GraphicsEngine is using the ARB program code path.
    /*!
        @return True is the system is using the ARB program code path.
    */
    bool UsingARBProgramCodePath ();

  private:

    ObjectPtr<FontTexture> _normal_font;    //!< The normal font renderer
    ObjectPtr<FontTexture> _bold_font;      //!< The bold font renderer

    void InitShaders();

    //! Helper function to compute a Gaussian filter weights
    void GaussianWeights (float **weights, float sigma, unsigned int num_tap);

    //! Helper function to set an fbo
    void SetFrameBufferHelper(
      ObjectPtr<IOpenGLFrameBufferObject>& fbo,
      ObjectPtr<IOpenGLTexture2D>& colorbuffer,
      ObjectPtr<IOpenGLTexture2D>& depthbuffer,
      int width, int height);

#ifndef NUX_OPENGL_ES_20
    void InitAsmColorShader();
    //! Render polygons with a single color.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmColor;

    void InitAsmTextureShader();
    //! Render polygons with a texture modulated by a color.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmTextureModColor;
    //! Same as m_AsmTextureModColor for rectangle textures.
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmTextureRectModColor;

    void InitAsmColorModTexMaskAlpha();
    //! Render polygons with a color masked by the alpha provided sampling a texture.
    //! Requires: Enable GPU Alpha Blending
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmColorModTexMaskAlpha;
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmColorModTexRectMaskAlpha;

    void InitAsm2TextureAdd();
    //! Render polygons with 2 textures, each modulated by a color, and added together.
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm2TextureAdd;
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm2TextureRectAdd;

    void InitAsm2TextureMod();
    //! Render polygons with 2 textures, each modulated by a color, and then multiplied together.
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm2TextureMod;
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm2TextureRectMod;

    void InitAsm4TextureAdd();
    //! Render polygons with 4 textures, each modulated by a color, and added together.
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm4TextureAdd;
    ObjectPtr<IOpenGLAsmShaderProgram> m_Asm4TextureRectAdd;

    void InitAsmComponentExponentiation ();
    //! Raise a texture component to a power.
    /*!
        result = color0 * (tex0)^(exponent) = (tex0.r^exponent.r, tex0.g^exponent.g, tex0.b^exponent.b, tex0.a^exponent.a);
    */
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_component_exponentiation_prog;
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_component_exponentiation_prog;

    void InitAsmAlphaReplicate ();
    //! Replicate the alpha component into r, g and b.
    /*!
        result = color0 * (tex0.a, tex0.a, tex0.a, tex0.a);
    */
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_alpha_replicate_prog;
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_alpha_replicate_prog;

    void InitAsmSeparableGaussFilter ();
    //! Gauss horizontal filter.
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_separable_gauss_filter_prog;
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_separable_gauss_filter_prog;

//     void InitAsmSeparableGaussFilter2 ();
//     //! Gauss horizontal filter.
//     ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_separable_gauss_filter_prog2;
//     ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_separable_gauss_filter_prog2;

    void InitAsmColorMatrixFilter ();
    //! Color matrix filter.
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_tex_color_matrix_filter_prog;
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_texrect_color_matrix_filter_prog;


    void InitAsmBlendModes();

    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPSBNormal;
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPSBLighten;
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPSBDarken;
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmPSBMultiply;
#endif

    void InitSlColorShader();
    //! Render polygons with a single color.
    ObjectPtr<IOpenGLShaderProgram> m_SlColor;

    void InitSlTextureShader();
    //! Render polygons with a texture modulated by a color.
    ObjectPtr<IOpenGLShaderProgram> m_SlTextureModColor;

    void InitSlColorModTexMaskAlpha();
    //! Render polygons with a color masked by the alpha provided sampling a texture.
    //! Requires: Enable GPU Alpha Blending
    ObjectPtr<IOpenGLShaderProgram> m_SlColorModTexMaskAlpha;
    ObjectPtr<IOpenGLShaderProgram> m_SlColorModTexRectMaskAlpha;

    void InitSl2TextureAdd();
    //! Render polygons with 2 textures, each modulated by a color, and added together.
    //! result = (tex0*color0)+(tex1*color1)
    ObjectPtr<IOpenGLShaderProgram> m_Sl2TextureAdd;

    void InitSl2TextureMod();
    //! Render polygons with 2 textures, each modulated by a color, and then multiplied together.
    //! result = (tex0*color0)*(tex1*color1)
    ObjectPtr<IOpenGLShaderProgram> m_Sl2TextureMod;

    void InitSl4TextureAdd();
    //! Render polygons with 4 textures, each modulated by a color, and added together.
    ObjectPtr<IOpenGLShaderProgram> m_Sl4TextureAdd;

    void InitSLComponentExponentiation ();
    //! Raise a texture component to a power.
    /*!
        result = color0 * (tex0)^(exponent) = (tex0.r^exponent.r, tex0.g^exponent.g, tex0.b^exponent.b, tex0.a^exponent.a);
    */
    ObjectPtr<IOpenGLShaderProgram> _component_exponentiation_prog;

    void InitSLAlphaReplicate ();
    //! Replicate the alpha component into r, g and b.
    /*!
        result = color0 * (tex0.a, tex0.a, tex0.a, tex0.a);
    */
    ObjectPtr<IOpenGLShaderProgram> _alpha_replicate_prog;

    void InitSLHorizontalGaussFilter ();
    //! Gauss horizontal filter.
    ObjectPtr<IOpenGLShaderProgram> _horizontal_gauss_filter_prog;
    
    void InitSLVerticalGaussFilter ();
    //! Gauss vertical filter.
    ObjectPtr<IOpenGLShaderProgram> _vertical_gauss_filter_prog;
    
    void InitSLColorMatrixFilter ();
    //! Color matrix filter.
    ObjectPtr<IOpenGLShaderProgram> _color_matrix_filter_prog;

    void InitSlBlendModes();

    ObjectPtr<IOpenGLShaderProgram> m_SlPSBNormal;
    ObjectPtr<IOpenGLShaderProgram> m_SlPSBLighten;
    ObjectPtr<IOpenGLShaderProgram> m_SlPSBDarken;
    ObjectPtr<IOpenGLShaderProgram> m_SlPSBMultiply;


    //! Test the gpu features and set variables such as \e _use_glsl_shaders.
    void EvaluateGpuCaps ();

    bool _use_glsl_shaders; //!< True if the system is using the glsl code path.

    ObjectPtr<IOpenGLFrameBufferObject> _offscreen_fbo;
    ObjectPtr<IOpenGLTexture2D> _offscreen_color_rt0;
    ObjectPtr<IOpenGLTexture2D> _offscreen_depth_rt0;
    ObjectPtr<IOpenGLTexture2D> _offscreen_color_rt1;
    ObjectPtr<IOpenGLTexture2D> _offscreen_depth_rt1;

    Matrix4 m_ProjectionMatrix;
    Matrix4 m_ModelViewMatrix;

    //! The system GraphicsDisplay object
    GraphicsDisplay &_graphics_display;

    int m_ViewportWidth, m_ViewportHeight;
    int m_ViewportX, m_ViewportY;
    int m_ScissorWidth, m_ScissorHeight;
    int m_ScissorX, m_ScissorY;
    int m_ScissorXOffset, m_ScissorYOffset;

    FontRenderer *m_font_renderer;

    //static long ID;

    //FilePath
    FilePath m_FilePath;

    //Statistics
    mutable long m_quad_stats;
    mutable long m_quad_tex_stats;
    mutable long m_triangle_stats;
    mutable long m_triangle_tex_stats;
    mutable long m_line_stats;

    GraphicsEngine (const GraphicsEngine &);
    // Does not make sense for a singleton. This is a self assignment.
    GraphicsEngine &operator= (const GraphicsEngine &);
    // Declare operator address-of as private
    GraphicsEngine *operator &();
  };

}

#endif // OPENGLENGINE_H

