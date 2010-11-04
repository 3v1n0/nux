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
#include "Gfx_OpenGL.h"
#include "GLWindowManager.h"
#elif defined(NUX_OS_LINUX)
#include "GfxSetupX11.h"
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

    GraphicsEngine (GLWindowImpl &GlWindow);
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
    void SetTexture (int TextureUnit, IntrusiveSP< IOpenGLBaseTexture > Texture);
    void EnableTextureMode (int TextureUnit, int TextureMode);
    void DisableTextureMode (int TextureUnit, int TextureMode);
    void DisableAllTextureMode (int TextureUnit);


    ///////////////////
    // DRAW TEXTURE  //
    ///////////////////

//     void QRP_GLSL_1Tex(int x, int y, int width, int height,
//         BaseTexture* Texture, TexCoordXForm& texxform0, Color& color);
    void QRP_1Tex (int x, int y, int width, int height, IntrusiveSP<IOpenGLBaseTexture> Tex0, TexCoordXForm texxform, Color Color0);
    void QRP_Color (int x, int y, int width, int height, const Color &c0);
    void QRP_Color (int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3);
    void QRP_ColorModTexAlpha (int x, int y, int width, int height,
                               IntrusiveSP<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color);

    void QRP_2Tex (int x, int y, int width, int height,
                   IntrusiveSP<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                   IntrusiveSP<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1);

    void QRP_4Tex (int x, int y, int width, int height,
                   IntrusiveSP<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                   IntrusiveSP<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1,
                   IntrusiveSP<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm &texxform2, const Color &color2,
                   IntrusiveSP<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm &texxform3, const Color &color3);

    void QRP_Triangle (int x0, int y0, int x1, int y1, int x2, int y2, Color c0);
    void QRP_Triangle (int x0, int y0, int x1, int y1, int x2, int y2, Color c0, Color c1, Color c2);
    void QRP_Line (int x0, int y0, int x1, int y1, Color c0);
    void QRP_Line (int x0, int y0, int x1, int y1, Color c0, Color c1);
    void QRP_QuadWireframe (int x0, int y0, int width, int height, Color c0, Color c1, Color c2, Color c3);


    void QRP_GLSL_1Tex (int x, int y, int width, int height,
                        IntrusiveSP<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform, const Color &color);

    void QRP_GLSL_Color (int x, int y, int width, int height, const Color &c0);
    void QRP_GLSL_Color (int x, int y, int width, int height, const Color &c0, const Color &c1, const Color &c2, const Color &c3);

    void QRP_GLSL_ColorModTexAlpha (int x, int y, int width, int height,
                                    IntrusiveSP<IOpenGLBaseTexture> DeviceTexture, TexCoordXForm &texxform0, const Color &color);

    void QRP_GLSL_2Tex (int x, int y, int width, int height,
                        IntrusiveSP<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                        IntrusiveSP<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1);

    void QRP_GLSL_4Tex (int x, int y, int width, int height,
                        IntrusiveSP<IOpenGLBaseTexture> DeviceTexture0, TexCoordXForm &texxform0, const Color &color0,
                        IntrusiveSP<IOpenGLBaseTexture> DeviceTexture1, TexCoordXForm &texxform1, const Color &color1,
                        IntrusiveSP<IOpenGLBaseTexture> DeviceTexture2, TexCoordXForm &texxform2, const Color &color2,
                        IntrusiveSP<IOpenGLBaseTexture> DeviceTexture3, TexCoordXForm &texxform3, const Color &color3);

    void QRP_GLSL_Triangle (int x0, int y0, int x1, int y1, int x2, int y2, Color c0);
    void QRP_GLSL_Triangle (int x0, int y0, int x1, int y1, int x2, int y2, Color c0, Color c1, Color c2);
    void QRP_GLSL_Line (int x0, int y0, int x1, int y1, Color c0);
    void QRP_GLSL_Line (int x0, int y0, int x1, int y1, Color c0, Color c1);
    void QRP_GLSL_QuadWireframe (int x0, int y0, int width, int height, Color c0, Color c1, Color c2, Color c3);

    //////////////////////
    // DRAW CLIPPING    //
    //////////////////////

    mutable std::vector<Rect> ClippingRect;
    void PushClippingRectangle (Rect rect);
    void PopClippingRectangle();
    void EmptyClippingRegion();
    void ApplyClippingRectangle();
    void SetDrawClippingRegion (int x, int y, unsigned int width, unsigned int height);
    Rect GetClippingRegion() const;
    int GetNumberOfClippingRegions() const;

    void ClearAreaColorDepthStencil (int x, int y, int width, int height, Color clearcolor, float cleardepth, int clearstencil);
    void ClearAreaColor (int x, int y, int width, int height, Color clearcolor);
    void ClearAreaDepthStencil (int x, int y, int width, int height, float cleardepth, int clearstencil);

    void Set3DView (int w, int h);
    void Push2DWindow (int w, int h);
    void Pop2DWindow();
    void Push2DModelViewMatrix (Matrix4 mat);
    Matrix4 Pop2DModelViewMatrix();
    void Clear2DModelViewMatrix();
    std::list<Matrix4> m_2DModelViewMatricesStack;

    void SetEnvModeTextureAlphaBlend (int TextureUnit);
    void SetEnvModeSelectTexture (int TextureUnit);
    void SetEnvModeSelectColor (int TextureUnit);
    void SetEnvModeModulateColorWithTexture (int TextureUnit);


    void SetViewport (int x, int y, int w, int h);
    Rect GetViewportRect();
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

    int RenderColorText (IntrusiveSP<FontTexture> Font, int x, int y, const NString &Str,
                         const Color &TextColor,
                         bool WriteAlphaChannel,
                         int NumCharacter);

    int RenderColorTextLineStatic (IntrusiveSP<FontTexture> Font, const PageBBox &pageSize, const NString &Str,
                                   const Color &TextColor,
                                   bool WriteAlphaChannel,
                                   TextAlignment alignment);

    int RenderColorTextLineEdit (IntrusiveSP<FontTexture> Font, const PageBBox &pageSize, const NString &Str,
                                 const Color &TextColor,
                                 bool WriteAlphaChannel,
                                 const Color &SelectedTextColor,
                                 const Color &SelectedTextBackgroundColor,
                                 const Color &TextBlinkColor,
                                 const Color &CursorColor,
                                 bool ShowCursor, unsigned int CursorPosition,
                                 int offset = 0, int selection_start = 0, int selection_end = 0);

    //Statistics
    void ResetStats();

    /*!
        Cache a resource if it has previously been cached. If the resource does not contain valid data
        then the returned value is not valid. Check that the returned hardware resource is valid by calling IntrusiveSP<CachedResourceData>.IsValid().
        @param Resource The resource to cache.
        @return A hardware resource.
    */
    IntrusiveSP<CachedResourceData> CacheResource (ResourceData *Resource);

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

    GLRenderStates &GetRenderStates()
    {
      return *m_GLWindow.m_DeviceFactory->m_RenderStates;
    }
    void ResetRenderStates()
    {
      m_GLWindow.m_DeviceFactory->m_RenderStates->ResetStateChangeToDefault();
    }
    void VerifyRenderStates()
    {
      m_GLWindow.m_DeviceFactory->m_RenderStates->CheckStateChange();
    }

    IntrusiveSP<FontTexture> GetFont();
    IntrusiveSP<FontTexture> GetBoldFont();

  private:

    IntrusiveSP<FontTexture> _normal_font;    //!< The normal font renderer
    IntrusiveSP<FontTexture> _bold_font;      //!< The bold font renderer

    void InitShaders();

    // Colored Quad shader
    IntrusiveSP<IOpenGLShaderProgram> m_ColoredQuadProg;
    IntrusiveSP<IOpenGLShaderProgram> m_ColoredPrimitiveProg;

    //////////////////////
    // ASM shaders

    void InitAsmColorShader();
    //! Render polygons with a single color.
    IntrusiveSP<IOpenGLAsmShaderProgram> m_AsmColor;

    void InitAsmTextureShader();
    //! Render polygons with a texture modulated by a color.
    IntrusiveSP<IOpenGLAsmShaderProgram> m_AsmTextureModColor;
    //! Same as m_AsmTextureModColor for rectangle textures.
    IntrusiveSP<IOpenGLAsmShaderProgram> m_AsmTextureRectModColor;

    void InitAsmColorModTexMaskAlpha();
    //! Render polygons with a color masked by the alpha provided sampling a texture.
    //! Requires: Enable GPU Alpha Blending
    IntrusiveSP<IOpenGLAsmShaderProgram> m_AsmColorModTexMaskAlpha;
    IntrusiveSP<IOpenGLAsmShaderProgram> m_AsmColorModTexRectMaskAlpha;

    void InitAsm2TextureAdd();
    //! Render polygons with 2 textures, each modulated by a color, and added together.
    IntrusiveSP<IOpenGLAsmShaderProgram> m_Asm2TextureAdd;
    IntrusiveSP<IOpenGLAsmShaderProgram> m_Asm2TextureRectAdd;

    void InitAsm4TextureAdd();
    //! Render polygons with 4 textures, each modulated by a color, and added together.
    IntrusiveSP<IOpenGLAsmShaderProgram> m_Asm4TextureAdd;
    IntrusiveSP<IOpenGLAsmShaderProgram> m_Asm4TextureRectAdd;

    void InitAsmBlendModes();

    IntrusiveSP<IOpenGLAsmShaderProgram> m_AsmPSBNormal;
    IntrusiveSP<IOpenGLAsmShaderProgram> m_AsmPSBLighten;
    IntrusiveSP<IOpenGLAsmShaderProgram> m_AsmPSBDarken;
    IntrusiveSP<IOpenGLAsmShaderProgram> m_AsmPSBMultiply;

    //////////////////////
    // GLSL shaders

    void InitSlColorShader();
    //! Render polygons with a single color.
    IntrusiveSP<IOpenGLShaderProgram> m_SlColor;

    void InitSlTextureShader();
    //! Render polygons with a texture modulated by a color.
    IntrusiveSP<IOpenGLShaderProgram> m_SlTextureModColor;

    void InitSlColorModTexMaskAlpha();
    //! Render polygons with a color masked by the alpha provided sampling a texture.
    //! Requires: Enable GPU Alpha Blending
    IntrusiveSP<IOpenGLShaderProgram> m_SlColorModTexMaskAlpha;
    IntrusiveSP<IOpenGLShaderProgram> m_SlColorModTexRectMaskAlpha;

    void InitSl2TextureAdd();
    //! Render polygons with 2 textures, each modulated by a color, and added together.
    IntrusiveSP<IOpenGLShaderProgram> m_Sl2TextureAdd;

    void InitSl4TextureAdd();
    //! Render polygons with 4 textures, each modulated by a color, and added together.
    IntrusiveSP<IOpenGLShaderProgram> m_Sl4TextureAdd;

    void InitSlBlendModes();

    IntrusiveSP<IOpenGLShaderProgram> m_SlPSBNormal;
    IntrusiveSP<IOpenGLShaderProgram> m_SlPSBLighten;
    IntrusiveSP<IOpenGLShaderProgram> m_SlPSBDarken;
    IntrusiveSP<IOpenGLShaderProgram> m_SlPSBMultiply;

    Matrix4 m_ProjectionMatrix;
    Matrix4 m_ModelViewMatrix;

    GLWindowImpl &m_GLWindow;

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

