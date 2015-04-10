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


#include "NuxCore/NuxCore.h"

#include "ImageSurface.h"

#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"

#include "FontTexture.h"
#include "FontRenderer.h"
#include "GraphicsEngine.h"

namespace nux
{

  BlendOperator::BlendOperator()
  {
    _enable = true;
    _src_blend = GL_ONE;
    _dst_blend = GL_ONE_MINUS_SRC_ALPHA;
  }

  BlendOperator::~BlendOperator()
  {
    _enable = true;
    _src_blend = GL_ONE;
    _dst_blend = GL_ONE_MINUS_SRC_ALPHA;
  }

  void BlendOperator::EnableBlending(bool enable)
  {
    _enable = enable;
  }

  void BlendOperator::SetPorterDuffOperator(PorterDuffOperator op)
  {
    switch(op)
    {
    case CLEAR:
      _src_blend = GL_ZERO;
      _dst_blend = GL_ZERO;
      break;
    case SRC:
      _src_blend = GL_ONE;
      _dst_blend = GL_ZERO;
      break;
    case DST:
      _src_blend = GL_ZERO;
      _dst_blend = GL_ONE;
      break;
    case SRC_OVER:
      _src_blend = GL_ONE;
      _dst_blend = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case DST_OVER:
      _src_blend = GL_ONE_MINUS_DST_ALPHA;
      _dst_blend = GL_ONE;
      break;
    case SRC_IN:
      _src_blend = GL_DST_ALPHA;
      _dst_blend = GL_ZERO;
      break;
    case DST_IN:
      _src_blend = GL_ZERO;
      _dst_blend = GL_SRC_ALPHA;
      break;
    case SRC_OUT:
      _src_blend = GL_ONE_MINUS_DST_ALPHA;
      _dst_blend = GL_ZERO;
      break;
    case DST_OUT:
      _src_blend = GL_ZERO;
      _dst_blend = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case SRC_ATOP:
      _src_blend = GL_DST_ALPHA;
      _dst_blend = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case DST_ATOP:
      _src_blend = GL_ONE_MINUS_DST_ALPHA;
      _dst_blend = GL_SRC_ALPHA;
      break;
    case XOR:
      _src_blend = GL_ONE_MINUS_DST_ALPHA;
      _dst_blend = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case PLUS:
      _src_blend = GL_ONE;
      _dst_blend = GL_ONE;
      break;
    default:
      // Use SRC_OVER
      _src_blend = GL_ONE;
      _dst_blend = GL_ONE_MINUS_SRC_ALPHA;
      break;
    }
  }

  void BlendOperator::SetCustomBlendOperator(unsigned int /* src_blend */, unsigned int /* dst_blend */)
  {

  }


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


  FxStructure::FxStructure()
  {
    src_texture = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateTexture(1, 1, 1, nux::BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
    dst_texture = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateTexture(1, 1, 1, nux::BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
    temp_texture = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateTexture(1, 1, 1, nux::BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
  }

  FxStructure::~FxStructure()
  {
  }

  GraphicsEngine::GraphicsEngine(GraphicsDisplay& GlWindow, bool create_rendering_data)
  : _graphics_display(GlWindow)
  {

    IOpenGLShaderProgram::SetShaderTracking(true);
    _scissor.x = 0;
    _scissor.y = 0;
    _clip_offset_x = 0;
    _clip_offset_y = 0;

    _font_renderer = 0;

    _use_glsl_shaders = false;
    _global_clipping_enabled = false;

    // Evaluate the features provided by the GPU.
    EvaluateGpuCaps();

    if (UsingGLSLCodePath())
    {
      nuxDebugMsg("The GraphicsEngine is using the GLSL shader code path.");
    }
    else
    {
      nuxDebugMsg("The GraphicsEngine is using the assenbly shader code path."); 
    }


    GlWindow.m_GraphicsContext = this;
    ResetStats();

    _projection_matrix.Identity();
    _model_view_matrix.Identity();

    ResourceCache.InitializeResourceFactories();

    m_CurrrentContext.x = 0;
    m_CurrrentContext.y = 0;
    m_CurrrentContext.width = _graphics_display.GetWindowWidth();
    m_CurrrentContext.height = _graphics_display.GetWindowHeight();

    SetViewport(0, 0, _graphics_display.GetWindowWidth(), _graphics_display.GetWindowHeight());
    SetScissor(0, 0, _graphics_display.GetWindowWidth(), _graphics_display.GetWindowHeight());
    EnableScissoring(true);

    GpuDevice* gpu_device = _graphics_display.GetGpuDevice();
    const GpuInfo& gpu_info = gpu_device->GetGpuInfo();

    if (create_rendering_data)
    {
#ifndef NUX_OPENGLES_20
      bool opengl_14_support = true;
      if ((gpu_device->GetOpenGLMajorVersion() == 1) &&
        (gpu_device->GetOpenGLMinorVersion() < 4))
      {
        // OpenGL version is less than OpenGL 1.4
        opengl_14_support = false;
      }

      if (gpu_info.Support_ARB_Fragment_Shader() &&
          gpu_info.Support_ARB_Vertex_Program() &&
          opengl_14_support)
      {
        InitAsmColorShader();
        InitAsmTextureShader();
        InitAsmPixelateShader();
        InitAsmColorModTexMaskAlpha();
        InitAsm2TextureAdd();
        InitAsm2TextureMod();
        InitAsm4TextureAdd();
        InitAsmBlendModes();

        InitAsmPower();
        InitAsmAlphaReplicate();
        InitAsmSeparableGaussFilter();
        InitAsmColorMatrixFilter();

        //InitAsm2TextureDepRead(); // NUXTODO: fix the shader
      }
#endif

      if ((gpu_info.Support_ARB_Vertex_Program() && gpu_info.Support_ARB_Fragment_Program())
          || (gpu_info.Support_ARB_Vertex_Shader() && gpu_info.Support_ARB_Fragment_Shader()))
      {
        _font_renderer = new FontRenderer(*this);
      }

      if (gpu_info.Support_EXT_Framebuffer_Object())
        _offscreen_fbo = gpu_device->CreateFrameBufferObject();

      _offscreen_color_rt0  = _graphics_display.GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
      _offscreen_color_rt1  = _graphics_display.GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
      _offscreen_color_rt2  = _graphics_display.GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
      _offscreen_color_rt3  = _graphics_display.GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);

      _offscreen_depth_rt0  = _graphics_display.GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
      _offscreen_depth_rt1  = _graphics_display.GetGpuDevice()->CreateTexture(2, 2, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
    }
  }

  GraphicsEngine::~GraphicsEngine()
  {
    _offscreen_color_rt0.Release();
    _offscreen_color_rt1.Release();
    _offscreen_depth_rt0.Release();
    _offscreen_depth_rt1.Release();
    _offscreen_fbo.Release();

    ResourceCache.Flush();
    NUX_SAFE_DELETE(_font_renderer);
  }

  void GraphicsEngine::EvaluateGpuCaps()
  {
#ifdef NUX_OS_WINDOWS
    if (_graphics_display.GetGpuDevice()->GetGpuInfo().Support_ARB_Vertex_Shader() &&
      _graphics_display.GetGpuDevice()->GetGpuInfo().Support_ARB_Fragment_Shader())
#else
    if (_graphics_display.GetGpuDevice()->GetGpuInfo().Support_ARB_Vertex_Shader() &&
      _graphics_display.GetGpuDevice()->GetGpuInfo().Support_ARB_Fragment_Shader() &&
      (_graphics_display.GetGpuDevice()->GetOpenGLMajorVersion() >= 2))
#endif
    {
      std::string renderer_string = ANSI_TO_TCHAR(NUX_REINTERPRET_CAST(const char* , glGetString(GL_RENDERER)));
      CHECKGL_MSG(glGetString(GL_RENDERER));

      // Exclude Geforce FX from using GLSL
      if (renderer_string.find("GeForce FX", 0) != tstring::npos)
      {
        _use_glsl_shaders = false;
        return;
      }

      // Exclude Geforce FX Go from using GLSL: this case is not needed since it is detected by the one above.
      if (renderer_string.find("GeForce FX Go", 0) != tstring::npos)
      {
        _use_glsl_shaders = false;
        return;
      }

      _use_glsl_shaders = true;
    }
    else
    {
      _use_glsl_shaders = false;
    }
  }

  bool GraphicsEngine::UsingGLSLCodePath()
  {
    return _use_glsl_shaders;
  }

  bool GraphicsEngine::UsingARBProgramCodePath()
  {
    return !_use_glsl_shaders;
  }

  ObjectPtr<FontTexture> GraphicsEngine::GetFont()
  {
#if defined(NUX_OS_WINDOWS)
      if (_normal_font.IsNull())
      {
        FontTexture* fnt = new FontTexture(GNuxGraphicsResources.FindResourceLocation("Fonts/Tahoma_size_8.txt", true).c_str(), NUX_TRACKER_LOCATION);
        _normal_font = ObjectPtr<FontTexture> (fnt);
        fnt->UnReference();
      }
#else
      if (_normal_font.IsNull())
      {
        FontTexture* fnt = new FontTexture(GNuxGraphicsResources.FindResourceLocation("Fonts/nuxfont_size_8.txt", true).c_str(), NUX_TRACKER_LOCATION);
        _normal_font = ObjectPtr<FontTexture> (fnt);
        fnt->UnReference();
      }
#endif
    return _normal_font;
  }

  ObjectPtr<FontTexture> GraphicsEngine::GetBoldFont()
  {
    #if defined(NUX_OS_WINDOWS)
      if (_bold_font.IsNull())
      {
        FontTexture* fnt = new FontTexture(GNuxGraphicsResources.FindResourceLocation("Fonts/Tahoma_size_8_bold.txt", true).c_str(), NUX_TRACKER_LOCATION);
        _bold_font = ObjectPtr<FontTexture> (fnt);
        fnt->UnReference();
      }
#else
      if (_bold_font.IsNull())
      {
        FontTexture* fnt = new FontTexture(GNuxGraphicsResources.FindResourceLocation("Fonts/nuxfont_size_8_bold.txt", true).c_str(), NUX_TRACKER_LOCATION);
        _bold_font = ObjectPtr<FontTexture> (fnt);
        fnt->UnReference();
      }
#endif
    return _bold_font;
  }

  void GraphicsEngine::SetContext(int x, int y, int width, int height)
  {
    m_CurrrentContext.x = x;
    m_CurrrentContext.y = y;

    if (width <= 0 || height <= 0)
    {
      //nuxAssertMsg(0, "[GraphicsEngine::SetContext] Incorrect context size.");
      if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
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

  void GraphicsEngine::GetContextSize(int& w, int& h) const
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

  void GraphicsEngine::GetWindowSize(int& w, int& h) const
  {
    _graphics_display.GetWindowSize(w, h);
  }

  int GraphicsEngine::GetWindowWidth() const
  {
    return _graphics_display.GetWindowWidth();
  }

  int GraphicsEngine::GetWindowHeight() const
  {
    return _graphics_display.GetWindowHeight();
  }

  int GraphicsEngine::RenderColorText(ObjectPtr<FontTexture> Font, int x, int y,
                                      std::string const& Str,
                                        const Color& TextColor,
                                        bool WriteAlphaChannel,
                                        int NumCharacter)
  {
    if (_font_renderer)
      return _font_renderer->RenderColorText(Font, x, y, Str, TextColor, WriteAlphaChannel, NumCharacter);

    return 0;
  }

int GraphicsEngine::RenderColorTextLineStatic(ObjectPtr<FontTexture> Font, const PageBBox& pageSize, std::string const& Str,
      const Color& TextColor,
      bool WriteAlphaChannel,
      TextAlignment alignment)
  {
    if (_font_renderer)
      return _font_renderer->RenderColorTextLineStatic(Font, pageSize, Str, TextColor, WriteAlphaChannel, alignment);

    return 0;
  }

int GraphicsEngine::RenderColorTextLineEdit(ObjectPtr<FontTexture> Font, const PageBBox& pageSize, std::string const& Str,
      const Color& TextColor,
      bool WriteAlphaChannel,
      const Color& SelectedTextColor,
      const Color& SelectedTextBackgroundColor,
      const Color& TextBlinkColor,
      const Color& CursorColor,
      bool ShowCursor, unsigned int CursorPosition, int offset, int selection_start, int selection_end)
  {
    if (_font_renderer)
      return _font_renderer->RenderColorTextLineEdit(Font, pageSize, Str,
             TextColor,
             WriteAlphaChannel,
             SelectedTextColor,
             SelectedTextBackgroundColor,
             TextBlinkColor,
             CursorColor,
             ShowCursor, CursorPosition, offset, selection_start, selection_end);

    return 0;
  }

  void GraphicsEngine::SetTexture(int TextureUnit, BaseTexture* Texture)
  {
    nuxAssertMsg(Texture != 0, "[GraphicsEngine::SetTexture] Texture is NULL.");

    if ((TextureUnit < GL_TEXTURE0) || (TextureUnit > GL_TEXTURE31))
      return;

    ObjectPtr <CachedBaseTexture> CachedTexture = ResourceCache.GetCachedResource(Texture);
    SetTexture(TextureUnit, CachedTexture->m_Texture);
  }

  void GraphicsEngine::SetTexture(int TextureUnit, ObjectPtr< IOpenGLBaseTexture > DeviceTexture)
  {
    NUX_RETURN_IF_FALSE(DeviceTexture.IsValid());

    CHECKGL(glActiveTextureARB(TextureUnit));
    DeviceTexture->BindTextureToUnit(TextureUnit);
  }

  void GraphicsEngine::EnableTextureMode(int TextureUnit, int TextureMode)
  {
    if ((TextureUnit < GL_TEXTURE0) || (TextureUnit > GL_TEXTURE31))
      return;

    CHECKGL(glActiveTextureARB(TextureUnit));
    CHECKGL(glEnable(TextureMode));
  }

  void GraphicsEngine::DisableTextureMode(int TextureUnit, int TextureMode)
  {
    if ((TextureUnit < GL_TEXTURE0) || (TextureUnit > GL_TEXTURE31))
      return;

    CHECKGL(glActiveTextureARB(TextureUnit));
    CHECKGL(glDisable(TextureMode));
    CHECKGL(glBindTexture(TextureMode, 0));
  }

  void GraphicsEngine::DisableAllTextureMode(int TextureUnit)
  {
    if ((TextureUnit < GL_TEXTURE0) || (TextureUnit > GL_TEXTURE31))
      return;

    _graphics_display.GetGpuDevice()->InvalidateTextureUnit(TextureUnit);
  }

//////////////////////
// DRAW CLIPPING    //
//////////////////////
  void GraphicsEngine::PushClippingRectangle(Rect const& rect)
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      // There is an active framebuffer set. Push the clipping rectangles to that fbo clipping stack.
      _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->PushClippingRegion(rect);
      return;
    }

    Rect r0 = ModelViewXFormRect(rect);

    Rect r1;
    unsigned int stacksize = (unsigned int) ClippingRect.size();
    int x0, y0, x1, y1;

    int window_width, window_height;
    window_width = _viewport.width;
    window_height = _viewport.height;

    if (stacksize == 0)
    {
      r1.Set(0, 0, window_width, window_height);
    }
    else
    {
      r1 = ClippingRect[stacksize-1];
    }

//    http://www.codecomments.com/archive263-2004-12-350347.html
//    If your rectangles are given in 2D as Top,Left,Bottom,Right coordinates, as typical for GUI programming, then it's simply:
//        intersect.Left = max(a.Left, b.Left);
//        intersect.Top = max(a.Top, b.Top);
//        intersect.Right = min(a.Right, b.Right );
//        intersect.Bottom = min(a.Bottom, b.Bottom);
//    And the intersection is empty unless intersect.Right > intersect.Left && intersect.Bottom > intersect.Top

    x0 = Max(r0.x, r1.x);
    y0 = Max(r0.y, r1.y);
    x1 = Min(r0.x + r0.width, r1.x + r1.width);
    y1 = Min(r0.y + r0.height, r1.y + r1.height);

    if ((x1 > x0) && (y1 > y0))
    {
      _clipping_rect.Set(x0, y0, x1 - x0, y1 - y0);
      ClippingRect.push_back(Rect(x0, y0, x1 - x0, y1 - y0));

      EnableScissoring(true);
      SetOpenGLClippingRectangle(x0, window_height - y0 - (y1 - y0), x1 - x0, y1 - y0);
    }
    else
    {
      _clipping_rect.Set(0, 0, 0, 0);
      ClippingRect.push_back(_clipping_rect);
      EnableScissoring(true);
      SetOpenGLClippingRectangle(0, 0, 0, 0);
    }
  }

  void GraphicsEngine::PopClippingRectangle()
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->PopClippingRegion();
      return;
    }

    ClippingRect.pop_back();
    unsigned int stacksize = (unsigned int) ClippingRect.size();

    if (stacksize == 0)
    {
      _clipping_rect.Set(0, 0, _viewport.width, _viewport.height);
      EnableScissoring(true);
      SetOpenGLClippingRectangle(0, 0, _viewport.width, _viewport.height);
    }
    else
    {
      _clipping_rect = ClippingRect [stacksize-1];
      Rect B = _clipping_rect;
      EnableScissoring(true);
      SetOpenGLClippingRectangle(B.x, _viewport.height - B.y - B.GetHeight(), B.GetWidth(), B.GetHeight());
    }
  }

  void GraphicsEngine::ApplyClippingRectangle()
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->ApplyClippingRegion();
      return;
    }

    unsigned int stacksize = (unsigned int) ClippingRect.size();

    if (stacksize == 0)
    {
      _clipping_rect.Set(0, 0, _viewport.width, _viewport.height);
      EnableScissoring(true);
      SetOpenGLClippingRectangle(0, 0, _viewport.width, _viewport.height);
    }
    else
    {
      _clipping_rect = ClippingRect[stacksize-1];
      Rect B = _clipping_rect;
      EnableScissoring(true);
      SetOpenGLClippingRectangle(B.x, _viewport.height - B.y - B.GetHeight(), B.GetWidth(), B.GetHeight());
    }
  }

  void GraphicsEngine::EmptyClippingRegion()
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->EmptyClippingRegion();
      return;
    }

    ClippingRect.clear();
    {
      _clipping_rect.Set(0, 0, _viewport.width, _viewport.height);
      EnableScissoring(true);
      SetOpenGLClippingRectangle(0, 0, _viewport.width, _viewport.height);
    }
  }

  void GraphicsEngine::SetGlobalClippingRectangle(Rect const& rect)
  {
    _global_clipping_enabled = true;
    _global_clipping_rect.Set(rect.x, _viewport.height - rect.y - rect.height, rect.width, rect.height);
    ApplyClippingRectangle();
  }

  void GraphicsEngine::DisableGlobalClippingRectangle()
  {
    _global_clipping_enabled = false;
    _global_clipping_rect.Set(0, 0, _viewport.width, _viewport.height);
    ApplyClippingRectangle();
  }

  Rect GraphicsEngine::GetClippingRegion() const
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      Rect r = _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->GetClippingRegion();
      return r;      
    }

    return _clipping_rect;

//     unsigned int stacksize = (unsigned int) ClippingRect.size();
// 
//     if (stacksize == 0)
//     {
//       return Rect(0, 0, _viewport.width, _viewport.height);
//     }
//     else
//     {
//       Rect r = ClippingRect[stacksize-1];
//       return r;
//     }
  }

  void GraphicsEngine::SetClippingRectangle(const Rect& rect)
  {
    _clipping_rect = rect;
    SetOpenGLClippingRectangle(rect.x, _viewport.height - rect.y - rect.height, rect.width, rect.height);
  }

  void GraphicsEngine::SetOpenGLClippingRectangle(int x, int y, unsigned int width, unsigned int height)
  {
    if (!_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      if (_global_clipping_enabled)
      {
        Rect intersection = Rect(x, y, width, height).Intersect(_global_clipping_rect);
        SetScissor(intersection.x, intersection.y, intersection.width, intersection.height);
      }
      else
      {
        SetScissor(x, y, width, height);
      }
    }
    else
    {
      SetScissor(x, y, width, height);
    }
  }

  int GraphicsEngine::GetNumberOfClippingRegions() const
  {
    if (_graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject().IsValid())
    {
      int n = _graphics_display.m_DeviceFactory->GetCurrentFrameBufferObject()->GetNumberOfClippingRegions();
      return n;
    }

    return(int) ClippingRect.size();
  }

  void GraphicsEngine::AddClipOffset(int x, int y)
  {
    PushClipOffset(x, y);
  }

  void GraphicsEngine::PushClipOffset(int x, int y)
  {
    _clip_offset_stack.push_back(Point(x, y));

    _clip_offset_x = 0;
    _clip_offset_y = 0;

    std::list<Point>::iterator it;
    for (it = _clip_offset_stack.begin(); it != _clip_offset_stack.end(); ++it)
    {
      _clip_offset_x += (*it).x;
      _clip_offset_y += (*it).y;
    }
  }

  void GraphicsEngine::PopClipOffset()
  {
    if (_clip_offset_stack.empty())
    {
      _clip_offset_x = 0;
      _clip_offset_y = 0;
    }

    _clip_offset_stack.pop_back();

    _clip_offset_x = 0;
    _clip_offset_y = 0;

    std::list<Point>::iterator it;
    for (it = _clip_offset_stack.begin(); it != _clip_offset_stack.end(); ++it)
    {
      _clip_offset_x += (*it).x;
      _clip_offset_y += (*it).y;
    }
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
//    line drawn from(x1, 0.5) to(x2, 0.5) will be reliably rendered along the bottom row of pixels int the
//    viewport, and a point drawn at(0.5, 0.5) will reliably fill the same pixel as glRecti(0, 0, 1, 1).
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

  const float RASTERIZATION_OFFSET = 0.0f;
  void GraphicsEngine::Push2DWindow(int w, int h)
  {
#ifndef NUX_OPENGLES_20
    {
      _model_view_matrix.Translate(m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); ++it)
      {
        temp = _model_view_matrix;
        _model_view_matrix = temp * (*it);
      }

    }

    {
      _projection_matrix.Orthographic(0, w, h, 0, -1.0f, 1.0f);
    }
#else
    // ModelView
    {
      _model_view_matrix.Translate(m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); it++)
      {
        temp = _model_view_matrix;
        _model_view_matrix = temp * (*it);
      }

    }

    // Projection
    {
      _projection_matrix.Orthographic(0, w, h, 0, -1.0f, 1.0f);
    }
#endif
  }

  void GraphicsEngine::Pop2DWindow()
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glMatrixMode(GL_PROJECTION));
    CHECKGL(glLoadIdentity());
    CHECKGL(glFrustum(
                -1.0,     // left
                1.0,      // right
                -1.0,     // bottom
                1.0,      // top
                0.1,    // near,
                2000.0  // far
              ));
    CHECKGL(glMatrixMode(GL_MODELVIEW));
    CHECKGL(glLoadIdentity());
#endif
  }

  void GraphicsEngine::Push2DModelViewMatrix(Matrix4 mat)
  {
    m_2DModelViewMatricesStack.push_back(mat);
    {
      _model_view_matrix.Translate(m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); ++it)
      {
        temp = _model_view_matrix;
        _model_view_matrix = (*it) * temp;
      }

    }
  }

  Matrix4 GraphicsEngine::Pop2DModelViewMatrix()
  {
    Matrix4 Mat;
    Mat.Zero();

    if (m_2DModelViewMatricesStack.empty())
      return Mat;

    std::list<Matrix4>::iterator it;
    it = m_2DModelViewMatricesStack.end();
    --it;
    Mat = (*it);
    m_2DModelViewMatricesStack.pop_back();

    {
      _model_view_matrix.Translate(m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); ++it)
      {
        temp = _model_view_matrix;
        _model_view_matrix = temp * (*it);
      }
    }
    return Mat;
  }

  void GraphicsEngine::Clear2DModelViewMatrix()
  {
    m_2DModelViewMatricesStack.clear();

    {
      _model_view_matrix.Translate(m_CurrrentContext.x + RASTERIZATION_OFFSET, m_CurrrentContext.y + RASTERIZATION_OFFSET, 0);
      Matrix4 temp;
      std::list<Matrix4>::iterator it;

      for (it = m_2DModelViewMatricesStack.begin(); it != m_2DModelViewMatricesStack.end(); ++it)
      {
        temp = _model_view_matrix;
        _model_view_matrix = temp * (*it);
      }
    }
  }

  void GraphicsEngine::PushIdentityModelViewMatrix()
  {
    PushModelViewMatrix(Matrix4::IDENTITY());
  }

  void GraphicsEngine::PushModelViewMatrix(const Matrix4& matrix)
  {
    if (_model_view_stack.empty())
      _model_view_matrix = matrix;
    else
      _model_view_matrix = matrix * _model_view_stack.back();

    _model_view_stack.push_back(_model_view_matrix);
  }

  void GraphicsEngine::Push2DTranslationModelViewMatrix(float tx, float ty, float tz)
  {
    Matrix4 temp;
    temp.Translate(tx, ty, tz);

    PushModelViewMatrix(temp);
  }

  bool GraphicsEngine::PopModelViewMatrix()
  {
    if (!_model_view_stack.empty())
      _model_view_stack.pop_back();

    if (_model_view_stack.empty())
    {
      _model_view_matrix = Matrix4::IDENTITY();
      return false;
    }

    _model_view_matrix = _model_view_stack.back();

    return true;
  }

  void GraphicsEngine::ResetModelViewMatrixStack()
  {
    _model_view_stack.clear();
    _model_view_matrix = Matrix4::IDENTITY();
  }

  void GraphicsEngine::SetModelViewMatrix(const Matrix4& matrix)
  {
    _model_view_matrix = matrix;
  }

  void GraphicsEngine::ApplyModelViewMatrix()
  {
    if (_model_view_stack.empty())
      _model_view_matrix = Matrix4::IDENTITY();
    else
      _model_view_matrix = _model_view_stack.back();
  }

  Rect GraphicsEngine::ModelViewXFormRect(const Rect& rect)
  {
    Vector4 v0(rect.x, rect.y, 0.0f, 1.0f);
    Vector4 v1 = _model_view_matrix * v0;
    Rect r(v1.x, v1.y, rect.width, rect.height);
    return r;
  }

  int GraphicsEngine::ModelViewStackDepth()
  {
    return(int)_model_view_stack.size();
  }

  void GraphicsEngine::PushPorterDuffBlend(const PorterDuffOperator& porter_duff_op)
  {
    BlendOperator blend_op;
    blend_op.SetPorterDuffOperator(porter_duff_op);

    _blend_stack.push_front(blend_op);

    GetRenderStates().SetBlend(blend_op._enable, blend_op._src_blend, blend_op._dst_blend);
  }

  void GraphicsEngine::PushDisableBlend()
  {
    BlendOperator blend_op;
    blend_op.EnableBlending(false);

    _blend_stack.push_front(blend_op);

    GetRenderStates().SetBlend(blend_op._enable);
  }

  bool GraphicsEngine::PopBlend()
  {
    if (_blend_stack.empty())
    {
      GetRenderStates().SetBlend(false, GL_ONE, GL_ZERO);
      return false;
    }

    _blend_stack.pop_front();
    
    BlendOperator blend_op = (*_blend_stack.begin());
    GetRenderStates().SetBlend(blend_op._enable, blend_op._src_blend, blend_op._dst_blend);

    return true;
  }

  int GraphicsEngine::BlendStackDepth()
  {
    return(int) _blend_stack.size();
  }

  Matrix4 GraphicsEngine::GetProjectionMatrix()
  {
    return _projection_matrix;
  }

  Matrix4 GraphicsEngine::GetOpenGLProjectionMatrix()
  {
    Matrix4 mat = GetProjectionMatrix();
    mat.Transpose();
    return mat;
  }

  void GraphicsEngine::SetProjectionMatrix(const Matrix4& matrix)
  {
    _projection_matrix = matrix;
  }

  void GraphicsEngine::SetOrthographicProjectionMatrix(int viewport_width, int viewport_height)
  {
    _projection_matrix.Orthographic(0, viewport_width, viewport_height, 0, -1.0f, 1.0f);
  }

  void GraphicsEngine::SetOrthographicProjectionMatrix(int left, int right, int bottom, int top)
  {
    _projection_matrix.Orthographic(left, right, bottom, top, -1.0f, 1.0f);
  }

  void GraphicsEngine::ResetProjectionMatrix()
  {
    _projection_matrix = Matrix4::IDENTITY();
  }

  Matrix4 GraphicsEngine::GetModelViewMatrix()
  {
    return _model_view_matrix;
  }

  Matrix4 GraphicsEngine::GetOpenGLModelViewMatrix()
  {
    Matrix4 mat = _model_view_matrix;
    mat.Transpose();
    return mat;
  }

  Matrix4 GraphicsEngine::GetModelViewProjectionMatrix()
  {
    return _projection_matrix * _model_view_matrix;
  }

  Matrix4 GraphicsEngine::GetOpenGLModelViewProjectionMatrix()
  {
    // This matrix is the transposed version of GetModelViewProjectionMatrix.
    Matrix4 mat = _projection_matrix * _model_view_matrix;
    mat.Transpose();
    return mat;
  }

  void GraphicsEngine::SetViewport(int origin_x, int origin_y, int w, int h)
  {
    nuxAssert(w >= 0);
    nuxAssert(h >= 0);

    _viewport.x = origin_x;
    _viewport.y = origin_y;
    _viewport.width = w;
    _viewport.height = h;

    if (_viewport.width < 0)
    {
      nuxAssertMsg(0, "[GraphicsEngine::SetViewport] Incorrect context size.");
      _viewport.width = 1;
    }

    if (_viewport.height < 0)
    {
      nuxAssertMsg(0, "[GraphicsEngine::SetViewport] Incorrect context size.");
      _viewport.height = 1;
    }

    CHECKGL(glViewport(origin_x, origin_y, _viewport.width, _viewport.height));
  }

//   Rect GraphicsEngine::GetViewportRect()
//   {
//     return Rect(_viewport.x, _viewport.y, _viewport.width, _viewport.height);
//   }
  
  Rect GraphicsEngine::GetViewportRect() const
  {
    return _viewport;
  }

  int  GraphicsEngine::GetViewportWidth() const
  {
    return _viewport.width;
  }

  int  GraphicsEngine::GetViewportHeight() const
  {
    return _viewport.height;
  }

  int GraphicsEngine::GetViewportX() const
  {
    return _viewport.x;
  }

  int GraphicsEngine::GetViewportY() const
  {
    return _viewport.y;
  }

  void  GraphicsEngine::GetViewportSize(int& viewport_width, int& viewport_height) const
  {
    viewport_width = _viewport.width;
    viewport_height = _viewport.height;
  }

  void GraphicsEngine::SetScissorOffset(int /* x */, int /* y */)
  {
    nuxAssertMsg(0, "[GraphicsEngine::SetScissorOffset] SetScissorOffset is deprecated.");
//     m_ScissorXOffset = x;
//     m_ScissorYOffset = y;
  }

  void GraphicsEngine::SetScissor(int x, int y, int w, int h)
  {
    nuxAssert(w >= 0);
    nuxAssert(h >= 0);

    NUX_RETURN_IF_FALSE(w >= 0);
    NUX_RETURN_IF_FALSE(h >= 0);

    _scissor.x = x;
    _scissor.y = y;
    _scissor.width = w;
    _scissor.height = h;

    if (_scissor.x < 0)
    {
      _scissor.width += _scissor.x;
      _scissor.x = 0;
    }
    
    if (_scissor.y < 0)
    {
      _scissor.height += _scissor.y;
      _scissor.y = 0;
    }

    if (_scissor.width <= 0)
    {
      // jaytaoko: This is a hack for what looks like a bug(#726033) in the radeon opensource driver
      // on R300/400/500. Rather than passing a null region to glScissor, we give the clip area a 1 pixel width.
      //_scissor.width = 1;
      CHECKGL(glScissor(0, 0, 1, 1));
      return;
    }

    if (_scissor.height <= 0)
    {
      // jaytaoko: This is a hack for what looks like a bug(#726033) in the radeon opensource driver
      // on R300/400/500. Rather than passing a null region to glScissor, we give the clip area a 1 pixel height.
      //_scissor.height = 1;
      CHECKGL(glScissor(0, 0, 1, 1));
      return;
    }

    CHECKGL(glScissor(_scissor.x, _scissor.y, _scissor.width, _scissor.height));
  }

  Rect const& GraphicsEngine::GetScissorRect() const
  {
    return _scissor;
  }

  void GraphicsEngine::EnableScissoring(bool b)
  {
    GetRenderStates().EnableScissor(b);
  }

  /////////////////////////////////////////
  // 2D Area Clear Color Depth Stencil   //
  /////////////////////////////////////////

  void GraphicsEngine::ClearAreaColorDepthStencil(int x, int y, int width, int height, Color clear_color, float /* cleardepth */, int clearstencil)
  {
    // enable stencil buffer
    CHECKGL(glEnable(GL_STENCIL_TEST));
    // write a one to the stencil buffer everywhere we are about to draw
    CHECKGL(glStencilFunc(GL_ALWAYS, clearstencil, 0xFFFFFFFF));
    // this is to always pass a one to the stencil buffer where we draw
    CHECKGL(glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE));

    //CHECKGL(glEnable(GL_DEPTH_TEST));
    //CHECKGL(glDepthFunc(GL_ALWAYS));

    QRP_Color(x, y, width, height, clear_color);

    //CHECKGL(glDepthFunc(GL_LESS));
    //CHECKGL(glDisable(GL_DEPTH_TEST));
    CHECKGL(glDisable(GL_STENCIL_TEST));
  }

  void GraphicsEngine::ClearAreaColor(int x, int y, int width, int height, Color clear_color)
  {
    QRP_Color(x, y, width, height, clear_color);
  }

  void GraphicsEngine::ClearAreaDepthStencil(int x, int y, int width, int height, float /* cleardepth */, int clearstencil)
  {
    // enable stencil buffer
    CHECKGL(glEnable(GL_STENCIL_TEST));
    // write a one to the stencil buffer everywhere we are about to draw
    CHECKGL(glStencilFunc(GL_ALWAYS, clearstencil, 0xFFFFFFFF));
    // this is to always pass a one to the stencil buffer where we draw
    CHECKGL(glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE));

    CHECKGL(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    //CHECKGL(glEnable(GL_DEPTH_TEST));
    //CHECKGL(glDepthFunc(GL_ALWAYS));

    QRP_Color(x, y, width, height, color::Black);

    //CHECKGL(glDepthFunc(GL_LESS));
    //CHECKGL(glDisable(GL_DEPTH_TEST));
    CHECKGL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    CHECKGL(glDisable(GL_STENCIL_TEST));

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

  ObjectPtr< CachedResourceData > GraphicsEngine::CacheResource(ResourceData* Resource)
  {
    return ResourceCache.GetCachedResource(Resource);
  }

  bool GraphicsEngine::FlushCachedResourceData(ResourceData* Resource)
  {
    if (!IsResourceCached(Resource))
      return false;

    ResourceCache.FlushResourceId(Resource->GetResourceIndex());
    return true;
  }

  void GraphicsEngine::UpdateResource(ResourceData* Resource)
  {
    ObjectPtr<CachedResourceData> GLResource = ResourceCache.FindCachedResourceById(Resource->GetResourceIndex()); //(CachedResourceData*)(*(ResourceCache.ResourceMap.find(Resource->ResourceIndex))).second;

    if (GLResource.IsValid())
    {
      // Iterate through all resource updater types(list is sorted by subclass depth).
      for (unsigned int i = 0; i < ResourceCache.GetResourceUpdaters().size(); ++i)
      {
        NResourceUpdater* ResourceUpdater = ResourceCache.GetResourceUpdaters() [i];
        nuxAssert(ResourceUpdater);

        // Check if the updater is valid for updating the resource.
        if (ResourceUpdater->UpdatesThisResource(Resource))
        {
          ResourceUpdater->UpdateResource(GLResource, Resource);
          break;
        }
      }
    }
  }

  bool GraphicsEngine::IsResourceCached(ResourceData* Resource)
  {
    return ResourceCache.IsCachedResource(Resource);
  }

  void GraphicsEngine::SetFrameBufferHelper(
    ObjectPtr<IOpenGLFrameBufferObject>& fbo,
    ObjectPtr<IOpenGLBaseTexture>& colorbuffer,
    ObjectPtr<IOpenGLBaseTexture>& depthbuffer,
    int width, int height)
  {
    if ((colorbuffer.IsValid() == false) || (colorbuffer->GetWidth() != width) || (colorbuffer->GetHeight() != height))
    {
      colorbuffer = _graphics_display.GetGpuDevice()->CreateTexture(width, height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
    }

    bool use_depth_buffer = _graphics_display.GetGpuDevice()->GetGpuInfo().Support_Depth_Buffer();
    if (use_depth_buffer &&
        depthbuffer.IsValid() &&
        ((depthbuffer->GetWidth() != width) || (depthbuffer->GetHeight() != height)))
    {
      // Generate a new depth texture only if a valid one was passed to this function.
      depthbuffer = _graphics_display.GetGpuDevice()->CreateTexture(width, height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);      
    }

    fbo->FormatFrameBufferObject(width, height, BITFMT_R8G8B8A8);
    fbo->SetRenderTarget(0, colorbuffer->GetSurfaceLevel(0));
    
    if (use_depth_buffer && depthbuffer.IsValid())
      fbo->SetDepthSurface(depthbuffer->GetSurfaceLevel(0));
    else
      fbo->SetDepthSurface(ObjectPtr<IOpenGLSurface>(NULL));

    fbo->Activate();
    fbo->EmptyClippingRegion();
    SetContext(0, 0, width, height);
    SetViewport(0, 0, width, height);
    Push2DWindow(width, height);
  }
  
  /*! Description - This function calculates position offsets for our gaussian weight values to utilise the GPU bilinear sampling
   *                which gets neighbouring pixel's data in just one sample. This serves to halve the loop for both vertical and
   *                horizontal blur shaders.
   *  Params -      First two parameters are the weight and weight offsets which are passed as uniforms to our shader.
   *                our sigma dictates how strong our blur will be.
   *  Return -      We return our loop count which is a #define in our vertical and horizontal shaders.
   */
  int GraphicsEngine::LinearSampleGaussianWeights(std::vector<float>& weights, std::vector<float>& offsets, 
                                                  float sigma)
  {
    //Calculate our support which is used as our loop count.
    int support = int(sigma * 3.0f);
    
    weights.push_back(exp(-(0*0)/(2*sigma*sigma))/(sqrt(2*constants::pi)*sigma));
    
    float total = weights.back();
    
    //Our first weight has an offset of 0.
    offsets.push_back(0);
    
    for (int i = 1; i <= support; i++)
    {
      float w1 = exp(-(i*i)/(2*sigma*sigma))/(sqrt(2*constants::pi)*sigma);
      float w2 = exp(-((i+1)*(i+1))/(2*sigma*sigma))/(sqrt(2*constants::pi)*sigma);
      
      weights.push_back(w1 + w2);
      total += 2.0f * weights[i];
      
      //Calculate our offset to utilise our GPU's bilinear sampling capability. By sampling in between texel we get the data of
      //neighbouring pixels with only one sample.
      offsets.push_back((i * w1 + (i + 1) * w2) / weights[i]);
    }
    
    //Normalise our weights.
    for (int i = 0; i < support; i++)
    {
      weights[i] /= total;
    }
    
    return support;
  }

  void GraphicsEngine::GaussianWeights(float **weights, float sigma, unsigned int num_tap)
  {
    *weights = new float[num_tap];
    float sum = 0.0f;
    unsigned int i = 0;

    unsigned int half = (num_tap-1)/2;

    (*weights)[half] = (1.0f/(sqrt(2.0f*3.14159265358f)*sigma)) * exp(-0.0f/(2.0f*sigma*sigma));
    sum += (*weights)[half];
    for (i = 0; i < half; i++)
    {
      float X = (i + 1)*(i + 1);
      (*weights)[half - i - 1] = (*weights)[half + i + 1] = (1.0f/(sqrt(2.0f*3.14159265358f)*sigma)) * exp(-X/(2.0f*sigma*sigma));
      sum += 2.0f * ((*weights)[half - i - 1]);
    }

    /* normalization */
    for (i = 0; i < num_tap; i++)
    {
      (*weights)[i] = (*weights)[i] / sum;
    }
  }

  ObjectPtr <IOpenGLBaseTexture> GraphicsEngine::CreateTextureFromBackBuffer(int x, int y, int width, int height)
  {
    ObjectPtr<IOpenGLFrameBufferObject> fbo = _graphics_display.GetGpuDevice()->GetCurrentFrameBufferObject();

    int X, Y, W, H;
    if (fbo.IsValid())
    {
      int fbo_width = fbo->GetWidth();
      int fbo_height = fbo->GetHeight();

      X = Clamp<int> (x, 0, fbo_width);
      Y = Clamp<int> (y, 0, fbo_height);
      W = Min<int> (fbo_width - x, width);
      H = Min<int> (fbo_height - y, height);


      if ((W <= 0) || (H <= 0))
      {
        nuxAssertMsg(0, "[GraphicsEngine::CreateTextureFromBackBuffer] Invalid request.");
        return ObjectPtr<IOpenGLBaseTexture> (0);
      }
      
      // Inverse Y because of OpenGL upside-down nature
      Y = fbo_height - Y - H;
    }
    else
    {
      // There is no fbo. Reading directly from the back-buffer.
      int bb_width = _graphics_display.GetWindowWidth();
      int bb_height = _graphics_display.GetWindowHeight();

      X = Clamp<int> (x, 0, bb_width);
      Y = Clamp<int> (y, 0, bb_height);
      W = Min<int> (bb_width - x, width);
      H = Min<int> (bb_height - y, height);

      if ((W <= 0) || (H <= 0))
      {
        nuxAssertMsg(0, "[GraphicsEngine::CreateTextureFromBackBuffer] Invalid request.");
        return ObjectPtr<IOpenGLBaseTexture> (0);
      }

      // Inverse Y because of OpenGL upside-down nature
      Y = bb_height - Y - H;
    }

    ObjectPtr <IOpenGLBaseTexture> device_texture = _graphics_display.GetGpuDevice()->CreateSystemCapableDeviceTexture(W, H, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
    ObjectPtr <IOpenGLSurface> sfc = device_texture->GetSurfaceLevel(0);

    sfc->CopyRenderTarget(X, Y, W, H);

    return device_texture;
  }

}
