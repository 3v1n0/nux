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


#include "NuxCore/NuxCore.h"

#include "NuxImage/ImageSurface.h"
#include "NuxMesh/NTextureArchiveManager.h"

#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"

#include "FontTexture.h"
#include "FontRenderer.h"
#include "UIColorTheme.h"

#include "GraphicsEngine.h"

namespace nux
{

  ROPConfig ROPConfig::Default;
  ROPConfig::ROPConfig()
  {
    Blend = false;
    SrcBlend = GL_SRC_ALPHA;
    DstBlend = GL_ONE_MINUS_SRC_ALPHA;
  }
  ROPConfig::~ROPConfig()
  {
  }


  GraphicsEngine::GraphicsEngine (GraphicsDisplay &GlWindow)
    :   _graphics_display (GlWindow)
  {
    m_ScissorX = 0;
    m_ScissorY = 0;
    m_ScissorXOffset = 0;
    m_ScissorYOffset = 0;
    m_font_renderer = 0;

    _use_glsl_shaders = false;
    // Evaluate the features provided by the GPU.
    EvaluateGpuCaps ();

    GlWindow.m_GraphicsContext = this;
    ResetStats();

    m_ProjectionMatrix.Identity();
    m_ModelViewMatrix.Identity();

    ResourceCache.InitializeResourceFactories();

    m_CurrrentContext.x = 0;
    m_CurrrentContext.y = 0;
    m_CurrrentContext.width = _graphics_display.GetWindowWidth();
    m_CurrrentContext.height = _graphics_display.GetWindowHeight();

    SetViewport (0, 0, _graphics_display.GetWindowWidth(), _graphics_display.GetWindowHeight() );
    SetScissor (0, 0, _graphics_display.GetWindowWidth(), _graphics_display.GetWindowHeight() );
    SetScissorOffset (0, 0);
    EnableScissoring (true);

#ifndef NUX_OPENGLES_20
    if (UsingGLSLCodePath ())
    {
      InitSlColorShader();
      InitSlTextureShader();
      InitSlColorModTexMaskAlpha ();
      InitSl2TextureAdd();
      InitSl2TextureMod();
      InitSl4TextureAdd();

      InitSLComponentExponentiation ();
      InitSLAlphaReplicate ();
      InitSLHorizontalGaussFilter ();
      InitSLVerticalGaussFilter ();
      InitSLColorMatrixFilter ();

      InitSl2TextureDepRead ();

      InitSLHorizontalHQGaussFilter ();
      InitSLVerticalHQGaussFilter ();
    }
    else
    {
      InitAsmColorShader();
      InitAsmTextureShader();
      InitAsmColorModTexMaskAlpha();
      InitAsm2TextureAdd();
      InitAsm2TextureMod();
      InitAsm4TextureAdd();
      InitAsmBlendModes();

      InitAsmComponentExponentiation ();
      InitAsmAlphaReplicate ();
      InitAsmSeparableGaussFilter ();
      InitAsmColorMatrixFilter ();

      //InitAsm2TextureDepRead (); // NUXTODO: fix the shader
    }
#else
    InitSlColorShader();
    InitSlTextureShader();
    InitSlColorModTexMaskAlpha ();
    InitSl2TextureAdd();
    InitSl2TextureMod();
    InitSl4TextureAdd();

    InitSLComponentExponentiation ();
    InitSLAlphaReplicate ();
    InitSLHorizontalGaussFilter ();
    InitSLVerticalGaussFilter ();
    InitSLColorMatrixFilter ();
#endif

    //GNuxGraphicsResources.CacheFontTextures (ResourceCache);

    _offscreen_fbo        = GetGpuDevice()->CreateFrameBufferObject ();
    _offscreen_color_rt0  = GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8);
    //_offscreen_depth_rt0  = GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_D24S8);
    _offscreen_color_rt1  = GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8);
    //_offscreen_depth_rt1  = GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_D24S8);
    _offscreen_color_rt2  = GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8);
    _offscreen_color_rt3  = GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8);

#if defined (NUX_OS_WINDOWS)
    if (_normal_font == 0)
    {
      FontTexture* fnt = new FontTexture (GNuxGraphicsResources.FindResourceLocation (TEXT ("Fonts/Tahoma_size_8.txt"), true).GetTCharPtr(), NUX_TRACKER_LOCATION);
      _normal_font = ObjectPtr<FontTexture> (fnt);
      fnt->UnReference ();
    }

    if (_bold_font == 0)
    {
      FontTexture* fnt = new FontTexture (GNuxGraphicsResources.FindResourceLocation (TEXT ("Fonts/Tahoma_size_8_bold.txt"), true).GetTCharPtr(), NUX_TRACKER_LOCATION);
      _bold_font = ObjectPtr<FontTexture> (fnt);
      fnt->UnReference ();
    }
#else
    if (_normal_font == 0)
    {
      FontTexture* fnt = new FontTexture (GNuxGraphicsResources.FindResourceLocation (TEXT ("Fonts/Ubuntu_size_10.txt"), true).GetTCharPtr(), NUX_TRACKER_LOCATION);
      _normal_font = ObjectPtr<FontTexture> (fnt);
      fnt->UnReference ();
    }

    if (_bold_font == 0)
    {
      FontTexture* fnt = new FontTexture (GNuxGraphicsResources.FindResourceLocation (TEXT ("Fonts/Ubuntu_size_10_bold.txt"), true).GetTCharPtr(), NUX_TRACKER_LOCATION);
      _bold_font = ObjectPtr<FontTexture> (fnt);
      fnt->UnReference ();
    }
#endif

    m_font_renderer = new FontRenderer (*this);
  }

  GraphicsEngine::~GraphicsEngine()
  {
    _offscreen_color_rt0.Release ();
    _offscreen_color_rt1.Release ();
    _offscreen_depth_rt0.Release ();
    _offscreen_depth_rt1.Release ();
    _offscreen_fbo.Release ();

    ResourceCache.Flush();
    NUX_SAFE_DELETE (m_font_renderer);
  }

  void GraphicsEngine::EvaluateGpuCaps ()
  {
    if (_graphics_display.GetGpuDevice ()->GetGpuInfo ().Support_ARB_Vertex_Shader() &&
      _graphics_display.GetGpuDevice ()->GetGpuInfo ().Support_ARB_Fragment_Shader() &&
      _graphics_display.GetGpuDevice ()->GetGPUBrand () ==  GPU_BRAND_NVIDIA)
    {
      _use_glsl_shaders = true;
    }
    else
    {
      _use_glsl_shaders = false;
    }
  }

  bool GraphicsEngine::UsingGLSLCodePath ()
  {
    return _use_glsl_shaders;
  }

  bool GraphicsEngine::UsingARBProgramCodePath ()
  {
    return !_use_glsl_shaders;
  }

  ObjectPtr<FontTexture> GraphicsEngine::GetFont()
  {
    return _normal_font;
  }

  ObjectPtr<FontTexture> GraphicsEngine::GetBoldFont()
  {
    return _bold_font;
  }

  void GraphicsEngine::SetContext (int x, int y, int width, int height)
  {
    m_CurrrentContext.x = x;
    m_CurrrentContext.y = y;

    if (width <= 0 || height <= 0)
    {
      //nuxAssertMsg(0, TEXT("[GraphicsEngine::SetContext] Incorrect context size.") );
      if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid() )
      {
        m_CurrrentContext.width = _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->GetWidth();
        m_CurrrentContext.height = _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->GetHeight();
      }
      else
      {
        m_CurrrentContext.width = GetWindowWidth();
        m_CurrrentContext.height = GetWindowHeight();
      }
    }
    else
    {
      m_CurrrentContext.width = width;
      m_CurrrentContext.height = height;
    }
  }

  void GraphicsEngine::GetContextSize (int &w, int &h) const
  {
    w = m_CurrrentContext.width;
    h = m_CurrrentContext.height;
  }

  int GraphicsEngine::GetContextWidth() const
  {
    return m_CurrrentContext.width;
  }

  int GraphicsEngine::GetContextHeight() const
  {
    return m_CurrrentContext.height;
  }

  int GraphicsEngine::GetContextX() const
  {
    return m_CurrrentContext.x;
  }

  int GraphicsEngine::GetContextY() const
  {
    return m_CurrrentContext.y;
  }

  void GraphicsEngine::GetWindowSize (int &w, int &h) const
  {
    _graphics_display.GetWindowSize (w, h);
  }

  int GraphicsEngine::GetWindowWidth() const
  {
    return _graphics_display.GetWindowWidth();
  }

  int GraphicsEngine::GetWindowHeight() const
  {
    return _graphics_display.GetWindowHeight();
  }

  int GraphicsEngine::RenderColorText (ObjectPtr<FontTexture> Font, int x, int y, const NString &Str,
                                        const Color &TextColor,
                                        bool WriteAlphaChannel,
                                        int NumCharacter)
  {
    if (m_font_renderer)
      return m_font_renderer->RenderColorText (Font, x, y, Str, TextColor, WriteAlphaChannel, NumCharacter);

    return 0;
  }

  int GraphicsEngine::RenderColorTextLineStatic (ObjectPtr<FontTexture> Font, const PageBBox &pageSize, const NString &Str,
      const Color &TextColor,
      bool WriteAlphaChannel,
      TextAlignment alignment)
  {
    if (m_font_renderer)
      return m_font_renderer->RenderColorTextLineStatic (Font, pageSize, Str, TextColor, WriteAlphaChannel, alignment);

    return 0;
  }

  int GraphicsEngine::RenderColorTextLineEdit (ObjectPtr<FontTexture> Font, const PageBBox &pageSize, const NString &Str,
      const Color &TextColor,
      bool WriteAlphaChannel,
      const Color &SelectedTextColor,
      const Color &SelectedTextBackgroundColor,
      const Color &TextBlinkColor,
      const Color &CursorColor,
      bool ShowCursor, unsigned int CursorPosition, int offset, int selection_start, int selection_end)
  {
    if (m_font_renderer)
      return m_font_renderer->RenderColorTextLineEdit (Font, pageSize, Str,
             TextColor,
             WriteAlphaChannel,
             SelectedTextColor,
             SelectedTextBackgroundColor,
             TextBlinkColor,
             CursorColor,
             ShowCursor, CursorPosition, offset, selection_start, selection_end);

    return 0;
  }

  void GraphicsEngine::SetTexture (int TextureUnit, BaseTexture *Texture)
  {
    nuxAssertMsg (Texture != 0, TEXT ("[GraphicsEngine::SetTexture] Texture is NULL.") );

    if ( (TextureUnit < GL_TEXTURE0) || (TextureUnit > GL_TEXTURE31) )
      return;

    ObjectPtr <CachedBaseTexture> CachedTexture = ResourceCache.GetCachedResource (Texture);
    SetTexture (TextureUnit, CachedTexture->m_Texture);
  }

  void GraphicsEngine::SetTexture (int TextureUnit, ObjectPtr< IOpenGLBaseTexture > DeviceTexture)
  {
    NUX_RETURN_IF_FALSE (DeviceTexture.IsValid() );

    CHECKGL ( glActiveTextureARB (TextureUnit) );
    DeviceTexture->BindTextureToUnit (TextureUnit);
  }

  void GraphicsEngine::EnableTextureMode (int TextureUnit, int TextureMode)
  {
    if ( (TextureUnit < GL_TEXTURE0) || (TextureUnit > GL_TEXTURE31) )
      return;

    CHECKGL ( glActiveTextureARB (TextureUnit) );
    CHECKGL ( glEnable (TextureMode) );
  }

  void GraphicsEngine::DisableTextureMode (int TextureUnit, int TextureMode)
  {
    if ( (TextureUnit < GL_TEXTURE0) || (TextureUnit > GL_TEXTURE31) )
      return;

    CHECKGL ( glActiveTextureARB (TextureUnit) );
    CHECKGL ( glDisable (TextureMode) );
    CHECKGL ( glBindTexture (TextureMode, 0) );
  }

  void GraphicsEngine::DisableAllTextureMode (int TextureUnit)
  {
    if ( (TextureUnit < GL_TEXTURE0) || (TextureUnit > GL_TEXTURE31) )
      return;

    GetGpuDevice()->InvalidateTextureUnit (TextureUnit);
  }

//////////////////////
// DRAW CLIPPING    //
//////////////////////
  void GraphicsEngine::PushClippingRectangle (Rect A)
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid() )
    {
      // There is an active framebuffer set. Push the clipping rectangles to that fbo clipping stack.
      _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->PushClippingRegion (A);
      return;
    }

    A.OffsetPosition (m_CurrrentContext.x, m_CurrrentContext.y);

    Rect B;
    UINT stacksize = (UINT) ClippingRect.size();
    INT x0, y0, x1, y1;

    int window_width, window_height;
    window_width = m_ViewportWidth;
    window_height = m_ViewportHeight;

    if (stacksize == 0)
    {
      B = Rect (0, 0, window_width, window_height);
    }
    else
    {
      B = ClippingRect[stacksize-1];
    }

//    http://www.codecomments.com/archive263-2004-12-350347.html
//    If your rectangles are given in 2D as Top,Left,Bottom,Right coordinates, as typical for GUI programming, then it's simply:
//        intersect.Left = max(a.Left, b.Left);
//        intersect.Top = max(a.Top, b.Top);
//        intersect.Right = min(a.Right, b.Right );
//        intersect.Bottom = min(a.Bottom, b.Bottom);
//    And the intersection is empty unless intersect.Right > intersect.Left && intersect.Bottom > intersect.Top

    x0 = Max (A.x, B.x);
    y0 = Max (A.y, B.y);
    x1 = Min (A.x + A.width, B.x + B.width);
    y1 = Min (A.y + A.height, B.y + B.height);

    if ( (x1 > x0) && (y1 > y0) )
    {
      ClippingRect.push_back (Rect (x0, y0, x1 - x0, y1 - y0) );

      EnableScissoring (true);
      SetDrawClippingRegion (x0, window_height - y0 - (y1 - y0), x1 - x0, y1 - y0);
    }
    else
    {
      ClippingRect.push_back (Rect (0, 0, 0, 0) );
      EnableScissoring (true);
      SetDrawClippingRegion (0, 0, 0, 0);
    }
  }

  void GraphicsEngine::PopClippingRectangle()
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid() )
    {
      _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->PopClippingRegion();
      return;
    }

    INT window_width, window_height;
    window_width = m_ViewportWidth;
    window_height = m_ViewportHeight;

    ClippingRect.pop_back();
    UINT stacksize = (UINT) ClippingRect.size();

    if (stacksize == 0)
    {
      EnableScissoring (true);
      SetDrawClippingRegion (0, 0, window_width, window_height);
    }
    else
    {
      Rect B = ClippingRect[stacksize-1];
      EnableScissoring (true);
      SetDrawClippingRegion (B.x, window_height - B.y - B.GetHeight(), B.GetWidth(), B.GetHeight() );
    }
  }

  void GraphicsEngine::ApplyClippingRectangle ()
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid() )
    {
      _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->ApplyClippingRegion();
      return;
    }

    INT window_width, window_height;
    window_width = m_ViewportWidth;
    window_height = m_ViewportHeight;

    UINT stacksize = (UINT) ClippingRect.size();

    if (stacksize == 0)
    {
      EnableScissoring (true);
      SetDrawClippingRegion (0, 0, window_width, window_height);
    }
    else
    {
      Rect B = ClippingRect[stacksize-1];
      EnableScissoring (true);
      SetDrawClippingRegion (B.x, window_height - B.y - B.GetHeight(), B.GetWidth(), B.GetHeight() );
    }
  }

  void GraphicsEngine::EmptyClippingRegion ()
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->EmptyClippingRegion ();
      return;
    }

    INT window_width, window_height;
    window_width = m_ViewportWidth;
    window_height = m_ViewportHeight;
    ClippingRect.clear();
    {
      EnableScissoring (true);
      SetDrawClippingRegion (0, 0, window_width, window_height);
    }
  }

  Rect GraphicsEngine::GetClippingRegion () const
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      Rect r = _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->GetClippingRegion ();
      return r;      
    }

    UINT stacksize = (UINT) ClippingRect.size();

    if (stacksize == 0)
    {
      return Rect (0, 0, m_ViewportWidth, m_ViewportHeight);
    }
    else
    {
      Rect r = ClippingRect[stacksize-1];
      return r;
    }
  }

  int GraphicsEngine::GetNumberOfClippingRegions () const
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      int n = _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->GetNumberOfClippingRegions ();
      return n;
    }

    return (int) ClippingRect.size();
  }

  void GraphicsEngine::SetDrawClippingRegion (int x, int y, unsigned int width, unsigned int height)
  {
    SetScissor (x, y, width, height);
  }

///////////////////
// DRAW TEXTURE  //
///////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering calls

//
//    From "OpenGL Programming Guide.pdf"
//
//    If exact two-dimensional rasterization is desired, you must carefully specify both the orthographic
//    projection and the vertices of primitives that are to be rasterized. The orthographic projection
//    should be specified with integer coordinates, as shown in the following example:
//    gluOrtho2D(0, width, 0, height);
//    where width and height are the dimensions of the viewport. Given this projection matrix, polygon
//    vertices and pixel image positions should be placed at integer coordinates to rasterize predictably.
//    For example, glRecti(0, 0, 1, 1) reliably fills the lower left pixel of the viewport, and glRasterPos2i(0,
//    0) reliably positions an unzoomed image at the lower left of the viewport. Point vertices, line
//    vertices, and bitmap positions should be placed at half-integer locations, however. For example, a
//    line drawn from (x1, 0.5) to (x2, 0.5) will be reliably rendered along the bottom row of pixels int the
//    viewport, and a point drawn at (0.5, 0.5) will reliably fill the same pixel as glRecti(0, 0, 1, 1).
//    An optimum compromise that allows all primitives to be specified at integer positions, while still
//    ensuring predictable rasterization, is to translate x and y by 0.375, as shown in the following code
//    fragment. Such a translation keeps polygon and pixel image edges safely away from the centers of
//    pixels, while moving line vertices close enough to the pixel centers.
//    glViewport(0, 0, width, height);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(0, width, 0, height);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glTranslatef(0.375, 0.375, 0.0);
  /* render all primitives at integer positions */

  const float RASTERIZATION_OFFSET = 0.375f;
  void GraphicsEngine::Push2DWindow (int w, int h)
  {
#ifndef NUX_OPENGLES_20
    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    {
      m_ModelViewMatrix.Translate (m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); it++)
      {
        temp = m_ModelViewMatrix;
        m_ModelViewMatrix = temp * (*it);
      }

      // m_ModelViewMatrix is row_major while opengl is column major. We need to transpose.
      m_ModelViewMatrix.Transpose();
      CHECKGL ( glLoadMatrixf ( (GLfloat *) (m_ModelViewMatrix.m) ) );
    }
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    {
      m_ProjectionMatrix.Orthographic (0, w, h, 0, -1.0f, 1.0f);
      // m_ProjectionMatrix is row_major while opengl is column major. We need to transpose.
      m_ProjectionMatrix.Transpose();
      CHECKGL ( glLoadMatrixf ( (GLfloat *) (m_ProjectionMatrix.m) ) );
    }
#else
    // ModelView
    {
      m_ModelViewMatrix.Translate (m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); it++)
      {
        temp = m_ModelViewMatrix;
        m_ModelViewMatrix = temp * (*it);
      }

      // m_ModelViewMatrix is row_major while opengl is column major. We need to transpose.
      m_ModelViewMatrix.Transpose();
    }

    // Projection
    {
      m_ProjectionMatrix.Orthographic (0, w, h, 0, -1.0f, 1.0f);
      // m_ProjectionMatrix is row_major while opengl is column major. We need to transpose.
      m_ProjectionMatrix.Transpose();
    }
#endif
  }

  void GraphicsEngine::Pop2DWindow()
  {
#ifndef NUX_OPENGLES_20
    CHECKGL ( glMatrixMode (GL_PROJECTION) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glMatrixMode (GL_MODELVIEW) );
    CHECKGL ( glLoadIdentity() );
    CHECKGL ( glFrustum (
                -1.0,     // left
                1.0,      // right
                -1.0,     // bottom
                1.0,      // top
                0.1,    // near,
                2000.0  // far
              ) );
#endif
  }

  void GraphicsEngine::Push2DModelViewMatrix (Matrix4 mat)
  {
    m_2DModelViewMatricesStack.push_back (mat);
    {
      CHECKGL ( glMatrixMode (GL_MODELVIEW) );
      m_ModelViewMatrix.Translate (m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); it++)
      {
        temp = m_ModelViewMatrix;
        m_ModelViewMatrix = (*it) * temp;
      }

      // m_ModelViewMatrix is row_major while opengl is column major. We need to transpose.
      m_ModelViewMatrix.Transpose();
      CHECKGL ( glLoadMatrixf ( (GLfloat *) (m_ModelViewMatrix.m) ) );
    }
  }

  Matrix4 GraphicsEngine::Pop2DModelViewMatrix()
  {
    Matrix4 Mat;
    Mat.Zero();

    if (m_2DModelViewMatricesStack.size() <= 0)
      return Mat;

    std::list<Matrix4>::iterator it;
    it = m_2DModelViewMatricesStack.end();
    --it;
    Mat = (*it);
    m_2DModelViewMatricesStack.pop_back();

    {
      CHECKGL ( glMatrixMode (GL_MODELVIEW) );
      m_ModelViewMatrix.Translate (m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); it++)
      {
        temp = m_ModelViewMatrix;
        m_ModelViewMatrix = temp * (*it);
      }

      // m_ModelViewMatrix is row_major while opengl is column major. We need to transpose.
      m_ModelViewMatrix.Transpose();
      CHECKGL ( glLoadMatrixf ( (GLfloat *) (m_ModelViewMatrix.m) ) );
    }
    return Mat;
  }

  void GraphicsEngine::Clear2DModelViewMatrix()
  {
    m_2DModelViewMatricesStack.clear();

    {
      CHECKGL ( glMatrixMode (GL_MODELVIEW) );
      m_ModelViewMatrix.Translate (m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); it++)
      {
        temp = m_ModelViewMatrix;
        m_ModelViewMatrix = temp * (*it);
      }

      // m_ModelViewMatrix is row_major while opengl is column major. We need to transpose.
      m_ModelViewMatrix.Transpose();
      CHECKGL ( glLoadMatrixf ( (GLfloat *) (m_ModelViewMatrix.m) ) );
    }
  }

  Matrix4 GraphicsEngine::GetProjectionMatrix()
  {
    return m_ProjectionMatrix;
  }

  Matrix4 GraphicsEngine::GetModelViewMatrix()
  {
    return m_ModelViewMatrix;
  }

  Matrix4 GraphicsEngine::GetModelViewProjectionMatrix()
  {
    return m_ModelViewMatrix * m_ProjectionMatrix;
  }

  Matrix4 GraphicsEngine::GetOpenGLModelViewProjectionMatrix()
  {
    // This matrix is the transposed version of GetModelViewProjectionMatrix.
    return m_ModelViewMatrix * m_ProjectionMatrix;
  }

  void GraphicsEngine::SetEnvModeTextureAlphaBlend (int TextureUnit)
  {
    // Render RGBA bitmap texture and alpha blend with the background
    // Make sure you call EnableBlending(bool b) before.
    CHECKGL ( glActiveTextureARB (TextureUnit) );
    //glEnable(GL_BLEND);
    CHECKGL ( glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
    // TextureEnvironment
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB) );
    // RGB
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR) );
    // ALPHA
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA) );
  }

  void GraphicsEngine::SetEnvModeSelectTexture (int TextureUnit)
  {
    // Render RGBA bitmap texture.
    CHECKGL ( glActiveTextureARB (TextureUnit) );
    // TextureEnvironment
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB) );
    // RGB
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR) );
    // ALPHA
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA) );
  }

  void GraphicsEngine::SetEnvModeSelectColor (int TextureUnit)
  {
    // Render the color;
    CHECKGL ( glActiveTextureARB (TextureUnit) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB) );
    // RGB
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PRIMARY_COLOR) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR) );
    // ALPHA
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PRIMARY_COLOR) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA) );
  }

  void GraphicsEngine::SetEnvModeModulateColorWithTexture (int TextureUnit)
  {
    // Render RGBA bitmat texture and alpha blend with the background
    CHECKGL ( glActiveTextureARB (TextureUnit) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB) );
    // RGB
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR) );
    // ALPHA
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE) );
    CHECKGL ( glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA) );
  }

  void GraphicsEngine::SetViewport (int origin_x, int origin_y, int w, int h)
  {
    nuxAssert (w >= 0);
    nuxAssert (h >= 0);

    m_ViewportX = origin_x;
    m_ViewportY = origin_y;
    m_ViewportWidth = w;
    m_ViewportHeight = h;

    if (m_ViewportWidth < 0)
    {
      nuxAssertMsg (0, TEXT ("[GraphicsEngine::SetViewport] Incorrect context size.") );
      m_ViewportWidth = 1;
    }

    if (m_ViewportHeight < 0)
    {
      nuxAssertMsg (0, TEXT ("[GraphicsEngine::SetViewport] Incorrect context size.") );
      m_ViewportHeight = 1;
    }

    CHECKGL ( glViewport (origin_x, origin_y, m_ViewportWidth, m_ViewportHeight) );
  }

  Rect GraphicsEngine::GetViewportRect()
  {
    return Rect (m_ViewportX, m_ViewportY, m_ViewportWidth, m_ViewportHeight);
  }

  void GraphicsEngine::SetScissorOffset (int x, int y)
  {
    m_ScissorXOffset = x;
    m_ScissorYOffset = y;
  }

  void GraphicsEngine::SetScissor (int x, int y, int w, int h)
  {
    nuxAssert (w >= 0);
    nuxAssert (h >= 0);
    m_ScissorX = x;
    m_ScissorY = y;
    m_ScissorWidth = w;
    m_ScissorHeight = h;

    if (m_ScissorWidth < 0)
    {
      nuxAssertMsg (0, TEXT ("[GraphicsEngine::SetViewport] Incorrect context size.") );
      m_ScissorWidth = 1;
    }

    if (m_ScissorHeight < 0)
    {
      nuxAssertMsg (0, TEXT ("[GraphicsEngine::SetViewport] Incorrect context size.") );
      m_ScissorHeight = 1;
    }

    CHECKGL ( glScissor (m_ScissorX + m_ScissorXOffset, m_ScissorY + m_ScissorYOffset, m_ScissorWidth, m_ScissorHeight) );
  }

  Rect GraphicsEngine::GetScissorRect()
  {
    return Rect (m_ScissorX, m_ScissorY, m_ScissorWidth, m_ScissorHeight);
  }

  void GraphicsEngine::EnableScissoring (bool b)
  {
    GetRenderStates().EnableScissor (b);
  }

  /////////////////////////////////////////
  // 2D Area Clear Color Depth Stencil   //
  /////////////////////////////////////////

  void GraphicsEngine::ClearAreaColorDepthStencil (int x, int y, int width, int height, Color clear_color, float cleardepth, int clearstencil)
  {
    // enable stencil buffer
    CHECKGL ( glEnable (GL_STENCIL_TEST) );
    // write a one to the stencil buffer everywhere we are about to draw
    CHECKGL ( glStencilFunc (GL_ALWAYS, clearstencil, 0xFFFFFFFF) );
    // this is to always pass a one to the stencil buffer where we draw
    CHECKGL ( glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE) );

    //CHECKGL( glEnable(GL_DEPTH_TEST) );
    //CHECKGL( glDepthFunc(GL_ALWAYS) );

    QRP_Color(x, y, width, height, clear_color);

    //CHECKGL( glDepthFunc(GL_LESS) );
    //CHECKGL( glDisable(GL_DEPTH_TEST) );
    CHECKGL ( glDisable (GL_STENCIL_TEST) );
  }

  void GraphicsEngine::ClearAreaColor (int x, int y, int width, int height, Color clear_color)
  {
    QRP_Color(x, y, width, height, clear_color);
  }

  void GraphicsEngine::ClearAreaDepthStencil (int x, int y, int width, int height, float cleardepth, int clearstencil)
  {
    // enable stencil buffer
    CHECKGL (glEnable (GL_STENCIL_TEST));
    // write a one to the stencil buffer everywhere we are about to draw
    CHECKGL (glStencilFunc (GL_ALWAYS, clearstencil, 0xFFFFFFFF));
    // this is to always pass a one to the stencil buffer where we draw
    CHECKGL (glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE));

    CHECKGL (glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    //CHECKGL (glEnable(GL_DEPTH_TEST));
    //CHECKGL (glDepthFunc(GL_ALWAYS));

    QRP_Color(x, y, width, height, Color::Black);

    //CHECKGL( glDepthFunc(GL_LESS) );
    //CHECKGL( glDisable(GL_DEPTH_TEST) );
    CHECKGL (glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    CHECKGL (glDisable (GL_STENCIL_TEST));

  }

//Statistics
  void GraphicsEngine::ResetStats()
  {
    m_quad_stats            = 0;
    m_quad_tex_stats        = 0;
    m_triangle_stats        = 0;
    m_triangle_tex_stats    = 0;
    m_line_stats            = 0;
  }

  ObjectPtr< CachedResourceData > GraphicsEngine::CacheResource (ResourceData *Resource)
  {
    return ResourceCache.GetCachedResource (Resource);
  }

  bool GraphicsEngine::FlushCachedResourceData (ResourceData *Resource)
  {
    bool cached = IsResourceCached (Resource);
    if (cached == false)
      return false;

    ResourceCache.FlushResourceId (Resource->GetResourceIndex ());
    return true;
  }

  void GraphicsEngine::UpdateResource (ResourceData *Resource)
  {
    ObjectPtr< CachedResourceData > GLResource = ResourceCache.FindCachedResourceById (Resource->GetResourceIndex() ); //(CachedResourceData*)(*(ResourceCache.ResourceMap.find(Resource->ResourceIndex))).second;
    bool bUpdated = FALSE;

    if (GLResource.IsValid() )
    {
      // Iterate through all resource updater types (list is sorted by subclass depth).
      for (t_u32 i = 0; i < ResourceCache.GetResourceUpdaters().size(); ++i)
      {
        NResourceUpdater *ResourceUpdater = ResourceCache.GetResourceUpdaters() [i];
        nuxAssert (ResourceUpdater);

        // Check if the updater is valid for updating the resource.
        if ( ResourceUpdater->UpdatesThisResource (Resource) )
        {
          bUpdated = ResourceUpdater->UpdateResource (GLResource, Resource);
          break;
        }
      }
    }
  }

  bool GraphicsEngine::IsResourceCached (ResourceData *Resource)
  {
    return ResourceCache.IsCachedResource (Resource);
  }

  void GraphicsEngine::SetFrameBufferHelper (
    ObjectPtr<IOpenGLFrameBufferObject>& fbo,
    ObjectPtr<IOpenGLTexture2D>& colorbuffer,
    ObjectPtr<IOpenGLTexture2D>& depthbuffer,
    int width, int height)
  {
    //if ((colorbuffer->GetWidth() != width) || (depthbuffer->GetHeight() != height))
    {
      colorbuffer = GetGpuDevice ()->CreateTexture (width, height, 1, BITFMT_R8G8B8A8);
      depthbuffer = GetGpuDevice ()->CreateTexture (width, height, 1, BITFMT_D24S8);
    }

    fbo->FormatFrameBufferObject(width, height, BITFMT_R8G8B8A8);
    fbo->SetRenderTarget (0, colorbuffer->GetSurfaceLevel (0));
    fbo->SetDepthSurface (depthbuffer->GetSurfaceLevel (0));
    fbo->Activate ();
    fbo->EmptyClippingRegion ();
    SetContext (0, 0, width, height);
    SetViewport (0, 0, width, height);
    Push2DWindow (width, height);
  }

  void GraphicsEngine::GaussianWeights(float **weights, float sigma, unsigned int num_tap)
  {
    *weights = new float[num_tap];
    float sum = 0.0f;
    unsigned int i = 0;

    unsigned int half = (num_tap-1)/2;

    (*weights)[half] = (1.0f/(sqrt(2.0f*3.14159265358f)*sigma)) * exp(-0.0f/(2.0f*sigma*sigma));
    sum += (*weights)[half];
    for(i = 0; i < half; i++)
    {
      float X = (i + 1)*(i + 1);
      (*weights)[half - i - 1] = (*weights)[half + i + 1] = (1.0f/(sqrt(2.0f*3.14159265358f)*sigma)) * exp(-X/(2.0f*sigma*sigma));
      sum += 2.0f * ((*weights)[half - i - 1]);
    }

    /* normalization */
    for(i = 0; i < num_tap; i++)
    {
      (*weights)[i] = (*weights)[i] / sum;
    }
  }

  ObjectPtr <IOpenGLBaseTexture> GraphicsEngine::CreateTextureFromBackBuffer (int x, int y, int width, int height)
  {
    ObjectPtr<IOpenGLFrameBufferObject> fbo = _graphics_display.GetGpuDevice ()->GetCurrentFrameBufferObject ();

    int X, Y, W, H;
    if (fbo.IsValid())
    {
      int fbo_width = fbo->GetWidth ();
      int fbo_height = fbo->GetHeight ();

      X = Clamp<int> (x, 0, fbo_width);
      Y = Clamp<int> (y, 0, fbo_height);
      W = Min<int> (fbo_width - x, width);
      H = Min<int> (fbo_height - y, height);


      if ((W <= 0) || (H <= 0))
      {
        nuxAssertMsg (0, TEXT("[GraphicsEngine::CreateTextureFromBackBuffer] Invalid request."));
        return ObjectPtr<IOpenGLBaseTexture> (0);
      }
      
      // Inverse Y because of OpenGL upside-down nature
      Y = fbo_height - Y - H;
    }
    else
    {
      // There is no fbo. Reading directly from the back-buffer.
      int bb_width = _graphics_display.GetWindowWidth ();
      int bb_height = _graphics_display.GetWindowHeight ();

      X = Clamp<int> (x, 0, bb_width);
      Y = Clamp<int> (y, 0, bb_height);
      W = Min<int> (bb_width - x, width);
      H = Min<int> (bb_height - y, height);

      if ((W <= 0) || (H <= 0))
      {
        nuxAssertMsg (0, TEXT("[GraphicsEngine::CreateTextureFromBackBuffer] Invalid request."));
        return ObjectPtr<IOpenGLBaseTexture> (0);
      }

      // Inverse Y because of OpenGL upside-down nature
      Y = bb_height - Y - H;
    }

    ObjectPtr <IOpenGLBaseTexture> device_texture = _graphics_display.GetGpuDevice ()->CreateSystemCapableDeviceTexture (W, H, 1, BITFMT_R8G8B8A8);
    ObjectPtr <IOpenGLSurface> sfc = device_texture->GetSurfaceLevel (0);

    sfc->CopyRenderTarget (X, Y, W, H);

    return device_texture;
  }

}
