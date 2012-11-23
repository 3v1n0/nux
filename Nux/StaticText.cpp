/*
 * Copyright 2012 Inalogic® Inc.
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

#include "Nux.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Validator.h"

#include "cairo/cairo.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"
#include "NuxGraphics/CairoGraphics.h"
#include "NuxGraphics/GraphicsDisplay.h"

#include "StaticText.h"

#if defined(NUX_OS_WINDOWS)
  #include "D2DTextRenderer.h"
#endif

namespace nux
{
  // DIP = Device Independent Pixel unit
  // 1 DIP = 1/96 inch.
  // 1 inch = 72 points.
  float ConvertPointSizeToDIP(float points)
  {
    return points;
    //return (points/72.0f)*96.0f;
  }

  NUX_IMPLEMENT_OBJECT_TYPE(StaticText);

  StaticText::StaticText(const std::string& text, NUX_FILE_LINE_DECL)
  : View(NUX_FILE_LINE_PARAM)
  , text_width_(0)
  , text_height_(0)
  {
    padding_x_ = 0;
    padding_y_ = 0;
    update_text_rendering_ = true;
    text_alignment_ = ALIGN_CENTER;

#if defined(NUX_STATIC_TEXT_USE_DIRECT_WRITE)
    layout_left_ = 0;
    layout_top_ = 0;
    
    GetGraphicsDisplay()->GetDirect2DFactory()->GetDesktopDpi(&dpi_scale_x, &dpi_scale_y);
    dpi_scale_x /= 96.0f;
    dpi_scale_y /= 96.0f;

    // 1 DIP = (1/96) inch;  1 inch = 96 DIP
    // PixelToDIP = (pixels/dpi)*96.0f

    font_size_ = 12;
    font_name_ = "Tahoma";

#elif defined(NUX_STATIC_TEXT_USE_CAIRO)
    cairo_graphics_ = NULL;
    font_size_ = 10;
    font_name_ = "Ubuntu";
    std::ostringstream os;
    os << font_name_ << " " << font_size_;
    pango_font_name_ = std::string(os.str());

    dpy_ = 96.0f;
#endif

    size_match_text_ = true;
    text_color_ = color::White;
    clip_to_width_ = 0;

    SetMinimumSize(1, 1);
    SetText(text);
  }

  StaticText::~StaticText()
  {
#if defined(NUX_STATIC_TEXT_USE_CAIRO)
    if (cairo_graphics_ != 0)
      delete(cairo_graphics_);
#endif

    if (dw_texture_.IsValid())
      dw_texture_.Release();
  }

  void StaticText::ApplyMinWidth()
  {
    Size sz = GetTextSizeNoClip();

    if ((sz.width <= GetMaximumWidth()) && (sz.width >= GetMinimumWidth()))
    {
      SetBaseWidth(sz.width);
    }
    else if (sz.width >= GetMaximumWidth())
    {
      View::ApplyMaxWidth();
    }
    else
    {
      View::ApplyMinWidth();
    }
  }

  long StaticText::ComputeContentSize()
  {
    int pre_layout_width = GetBaseWidth();
    int pre_layout_height = GetBaseHeight();

    Size sz = GetTextSizeNoClip();

    // The height of the StaticText is the height of the text itself.
    SetBaseHeight(sz.height);

     if ((sz.width >= GetMinimumWidth()) && (sz.width <= GetMaximumWidth()))
     {
       // The text size is within the bounds of the view.
       SetBaseWidth(sz.width);
     }
     else if (sz.width > GetMaximumWidth())
     {
       // The text is bigger than the available size.
       ApplyMaxWidth();
     }
     else if (sz.width < GetMinimumWidth())
     {
       // The text size is smaller than the minimum width of the view.
       View::ApplyMinWidth();
     }

    // Get the width size of this area.
    int width = GetBaseWidth();
    int height = GetBaseHeight();

    if (sz.width > width)
    {
      // The text is too large to fit.
      // There will be ellipsis at the end of the text.
      SetClipping(width);
    }
    else
    {
      // The text fits inside the view.
      SetClipping(0);
    }

    long result = 0;
    if (pre_layout_width < width)
      result |= eLargerWidth;
    else if (pre_layout_width > width)
      result |= eSmallerWidth;
    else
      result |= eCompliantWidth;

    if (pre_layout_height < height)
      result |= eLargerHeight;
    else if (pre_layout_height > height)
      result |= eSmallerHeight;
    else
      result |= eCompliantHeight;

    return result;

  }

  void StaticText::SetSizeMatchText(bool size_match_text)
  {
    size_match_text_ = size_match_text;
  }

  bool StaticText::GetSizeMatchText() const
  {
    return size_match_text_;
  }

   void StaticText::SetClipping(int clipping)
   {
     if (clip_to_width_ == clipping)
       return;
 
     clip_to_width_ = clipping;
 
     if (clip_to_width_ < 0)
       clip_to_width_ = 0;
 
     ComputeTextSize();
     update_text_rendering_ = true;
   }
 
   int StaticText::GetClipping() const
   {
     return clip_to_width_;
   }

  void StaticText::Draw(GraphicsEngine& graphics_engine, bool /* forceDraw */)
  {
    if (update_text_rendering_)
    {
      UpdateTextRendering();
    }

    // Draw background texture here.

    if (!dw_texture_.IsValid())
      return;

    Geometry base = GetGeometry();
    graphics_engine.PushClippingRectangle(base);

    nux::GetPainter().PaintBackground(graphics_engine, base);

    // Get the current blend states. They will be restored later.
    unsigned int alpha = 0, src = 0, dest = 0;
    graphics_engine.GetRenderStates().GetBlend(alpha, src, dest);

    graphics_engine.GetRenderStates().SetBlend(true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    TexCoordXForm texxform;
    texxform.SetWrap(TEXWRAP_CLAMP, TEXWRAP_CLAMP);
    texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);

    // Compute and y coordinate centered on the view
    int y = base.y + (base.height - dw_texture_->GetHeight()) / 2;

    int x = base.x;
    
    //Special case: In the horizontal direction, if the width of the text is smaller than the 
    // width of the view, then center the text horizontally.
    if (dw_texture_->GetWidth() < base.width)
    {
      if (text_alignment_ == ALIGN_CENTER)
      {
        x = base.x + (base.width - dw_texture_->GetWidth()) / 2;
      }
      else if (text_alignment_ == ALIGN_LEFT)
      {
        x = base.x;
      }
      else if (text_alignment_ == ALIGN_RIGHT)
      {
        x = base.x + base.width - dw_texture_->GetWidth();
      }
    }

    graphics_engine.QRP_1Tex(x,
      y,
      dw_texture_->GetWidth(),
      dw_texture_->GetHeight(),
      dw_texture_,
      texxform,
      text_color_);

    graphics_engine.GetRenderStates().SetBlend(alpha, src, dest);
    graphics_engine.PopClippingRectangle();
  }

  void StaticText::SetFontSize(int font_size)
  {
    if (font_size <= 0)
      return;
      
#if defined(NUX_STATIC_TEXT_USE_DIRECT_WRITE)
    font_size_ = font_size;
#elif defined(NUX_STATIC_TEXT_USE_CAIRO)
    font_size_ = font_size;
    std::ostringstream os;
    os << font_name_ << " " << font_size_;
    pango_font_name_ = std::string(os.str());
#endif

    // reset cache
    no_clip_size_.width = 0;
    no_clip_size_.height = 0;
    // Changing the font can cause the StaticView to resize itself.
    Size sz = GetTextSizeNoClip();
    // Calling SetBaseSize will trigger a layout request of this view and all of its parents.
    SetBaseSize(sz.width, sz.height);
  }

  void StaticText::SetTextPointSize(int font_size)
  {
    SetFontSize(font_size);
  }

  int StaticText::GetFontSize() const
  {
    return font_size_;
  }

  int StaticText::GetTextPointSize() const
  {
    return GetFontSize();
  }

  void StaticText::SetText(const std::string& text)
  {
    if (text_ == text)
      return;

    text_ = text;

    // reset cache
    no_clip_size_.width = 0;
    no_clip_size_.height = 0;

    // Changing the font can cause the StaticView to resize itself.
    Size sz = GetTextSizeNoClip();
    // Calling SetBaseSize will trigger a layout request of this view and all of its parents.
    SetBaseSize(sz.width, sz.height);

    update_text_rendering_ = true;
    text_changed.emit(this);
    QueueDraw();
  }

  std::string StaticText::GetText() const
  {
    return text_;
  }

  void StaticText::SetTextColor(const Color& text_color)
  {
    text_color_ = text_color;
    QueueDraw();
  }

  Color StaticText::GetTextColor() const
  {
    return text_color_;
  }

  void StaticText::SetFontName(const std::string& font_name)
  {
    if (font_name_ == font_name)
      return;

    font_name_ = font_name;

#if defined(NUX_OS_LINUX)
    std::ostringstream os;
    os << font_name_ << " " << font_size_;
    pango_font_name_ = std::string(os.str());
#endif
    
    // reset cache
    no_clip_size_.width = 0;
    no_clip_size_.height = 0;

    // Changing the font can cause the StaticView to resize itself.
    Size sz = GetTextSizeNoClip();
    // Calling SetBaseSize will trigger a layout request of this view and all of its parents.
    SetBaseSize(sz.width, sz.height);
    QueueDraw();
  }

  std::string StaticText::GetFontName() const
  {
    return font_name_;
  }

  void StaticText::GetTextLayoutSize(int& width, int& height) const
  {
    if (text_ == "")
    {
      width = 0;
      height = 0;
      return;
    }

    width = text_width_;
    height = text_height_;
  }

  Size StaticText::GetTextLayoutSize() const
  {
    Size sz;
    GetTextLayoutSize(sz.width, sz.height);
    return sz;
  }

  void StaticText::SetTextAlignment(TextAlignment alignment)
  {
    text_alignment_ = alignment;
  }

  StaticText::TextAlignment StaticText::GetTextAlignment() const
  {
    return text_alignment_;
  }
  
  ObjectPtr<nux::IOpenGLBaseTexture> StaticText::GetTextTexture()
  {
    if (update_text_rendering_)
    {
      // If the text rendering needs to be updated, do it here.
      UpdateTextRendering();
    }

    return dw_texture_;
  }

  Size StaticText::GetTextSizeNoClip()
  {
    if (no_clip_size_.width == 0)
      no_clip_size_ = ComputeTextSize(false, false);
    return no_clip_size_;
  }

#if defined(NUX_STATIC_TEXT_USE_DIRECT_WRITE)
  Size StaticText::ComputeTextSize(bool assign, bool with_clipping)
  {
    HRESULT hr;
    IDWriteFactory* pDWriteFactory = GetGraphicsDisplay()->GetDirectWriteFactory();

    ID2D1RenderTarget* pRT = NULL;
    IDWriteTextFormat* pTextFormat = NULL;

    hr = pDWriteFactory->CreateTextFormat(
        ANSICHAR_TO_UNICHAR(font_name_.c_str()),                  // Font family name.
        NULL,                       // Font collection(NULL sets it to use the system font collection).
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        ConvertPointSizeToDIP(font_size_),
        L"en-us",
        &pTextFormat);

    // Center align(horizontally) the text.
    if (SUCCEEDED(hr))
    {
      hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    }

    if (SUCCEEDED(hr))
    {
      hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    if (SUCCEEDED(hr))
    {
      hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }

    IDWriteTextLayout* pTextLayout_;
    hr = pDWriteFactory->CreateTextLayout(
      TCHAR_TO_UNICHAR(text_.c_str()),      // The string to be laid out and formatted.
      text_.length(),                       // The length of the string.
      pTextFormat,                         // The text format to apply to the string(contains font information, etc).
      1,                                    // The width of the layout box.
      1,                                    // The height of the layout box.
      &pTextLayout_                         // The IDWriteTextLayout interface pointer.
      );

    DWRITE_TEXT_METRICS metrics;
    pTextLayout_->GetMetrics(&metrics);

    int text_width = std::ceil(metrics.widthIncludingTrailingWhitespace * dpi_scale_x);
    int text_height = std::ceil(metrics.height * dpi_scale_y);

    if (assign)
    {
      text_width_ = text_width;
      text_height_ = text_height;

      padding_x_ = text_width_ - metrics.widthIncludingTrailingWhitespace * dpi_scale_x;
      padding_y_ = text_height_ - metrics.height * dpi_scale_y;

      if (with_clipping && (clip_to_width_ > 0))
      {
        text_width_ = text_width_ <= clip_to_width_ ? text_width_ : clip_to_width_;
      }

      text_width = text_width_;
      text_height = text_height_;
    }

    layout_top_ = metrics.top * dpi_scale_x;
    layout_left_ = metrics.left * dpi_scale_y;

    if (pTextLayout_)
    {
      pTextLayout_->Release();
      pTextLayout_ = NULL;
    }

    if (pTextFormat)
    {
      pTextFormat->Release();
      pTextFormat = NULL;
    }

    return Size(text_width, text_height);
  }

  void StaticText::RasterizeText(Color color)
  {
    if ((text_width_ <=0) || (text_height_ <= 0))
    {
      dw_texture_.Release();
      return;
    }

    HRESULT hr;

    ID2D1Factory* pD2DFactory = GetGraphicsDisplay()->GetDirect2DFactory();
    IDWriteFactory* pDWriteFactory = GetGraphicsDisplay()->GetDirectWriteFactory();
    IWICImagingFactory* pWICFactory = GetGraphicsDisplay()->GetWICFactory();

    ID2D1RenderTarget* pRT = NULL;
    IDWriteTextFormat* pTextFormat = NULL;

    hr = pDWriteFactory->CreateTextFormat(
      ANSICHAR_TO_UNICHAR(font_name_.c_str()),                  // Font family name.
      NULL,                       // Font collection(NULL sets it to use the system font collection).
      DWRITE_FONT_WEIGHT_REGULAR,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      ConvertPointSizeToDIP(font_size_),
      L"en-us",
      &pTextFormat);


    // Center align(horizontally) the text.
    if (SUCCEEDED(hr))
    {
      hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    }

    if (SUCCEEDED(hr))
    {
      hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    if (SUCCEEDED(hr))
    {
      hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }

    IDWriteTextLayout* pTextLayout_ = NULL;

    hr = pDWriteFactory->CreateTextLayout(
      TCHAR_TO_UNICHAR(text_.c_str()),      // The string to be laid out and formatted.
      text_.length(),                       // The length of the string.
      pTextFormat,                         // The text format to apply to the string(contains font information, etc).
      text_width_ / dpi_scale_x,                                    // The width of the layout box.
      text_height_ / dpi_scale_y,                                    // The height of the layout box.
      &pTextLayout_                         // The IDWriteTextLayout interface pointer.
      );

    IDWriteInlineObject* inlineObject = NULL;
    if (SUCCEEDED(hr))
    {
      pDWriteFactory->CreateEllipsisTrimmingSign(
        pTextLayout_,
        &inlineObject);
    }
    DWRITE_TRIMMING trimming = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    hr = pTextLayout_->SetTrimming(&trimming, inlineObject);

    IWICBitmap* pWICBitmap = NULL;
    if (SUCCEEDED(hr))
    {
      hr = pWICFactory->CreateBitmap(
        text_width_ / dpi_scale_x,
        text_height_ / dpi_scale_y,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapCacheOnLoad,
        &pWICBitmap);
    }

    // Set the render target type to D2D1_RENDER_TARGET_TYPE_DEFAULT to use software rendering.
    if (SUCCEEDED(hr))
    {
      D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
     
      renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
      renderTargetProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
      renderTargetProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
      renderTargetProperties.dpiX = 0.0;
      renderTargetProperties.dpiY = 0.0;
      renderTargetProperties.usage = D2D1_RENDER_TARGET_USAGE_NONE;
      renderTargetProperties.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
     
      hr = pD2DFactory->CreateWicBitmapRenderTarget(
        pWICBitmap,
        renderTargetProperties, //D2D1::RenderTargetProperties(),
        &pRT);
    }

    ID2D1SolidColorBrush* pBrush;
    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(color.red, color.green, color.blue, color.alpha)), &pBrush);

    // Create the text renderer
    CustomTextRenderer* pTextRenderer_ = new (std::nothrow) CustomTextRenderer(
      pD2DFactory,
      pRT,
      pBrush,
      NULL);

    D2D1_POINT_2F origin = D2D1::Point2F(static_cast<FLOAT>(padding_x_ / dpi_scale_x), static_cast<FLOAT>(padding_y_ / dpi_scale_y));

    if (0)
    {
      if (SUCCEEDED(hr))
      {
        pRT->BeginDraw();

        pRT->SetTransform(D2D1::IdentityMatrix());

        pRT->Clear(D2D1::ColorF(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)));

        // Call the DrawText method of this class.
        hr = pTextLayout_->Draw(
          NULL,
          pTextRenderer_,  // Custom text renderer.
          origin.x,
          origin.y
          );

        hr = pRT->EndDraw();
      }
    }
    else
    {
      if (SUCCEEDED(hr))
      {
        pRT->BeginDraw();

        pRT->SetTransform(D2D1::IdentityMatrix());

        pRT->Clear(D2D1::ColorF(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)));

        // Call the DrawText method of this class.
        pRT->DrawTextLayout(
          origin,
          pTextLayout_,
          pBrush);

        hr = pRT->EndDraw();
      }
    }

    {
      unsigned int width, height;
      pWICBitmap->GetSize(&width, &height);

      dw_texture_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateTexture(width, height, 1, nux::BITFMT_B8G8R8A8, NUX_TRACKER_LOCATION);

      nux::SURFACE_LOCKED_RECT lock_rect;
      dw_texture_->LockRect(0, &lock_rect, NULL);

      pWICBitmap->CopyPixels(NULL, lock_rect.Pitch, 4*width*height, (BYTE*) lock_rect.pBits);

      dw_texture_->UnlockRect(0);

    }

    if (pTextRenderer_)
      delete pTextRenderer_;

    if (pBrush)
    {
      pBrush->Release();
      pBrush = NULL;
    }


    if (pRT)
    {
      pRT->Release();
      pRT = NULL;
    }

    if (pWICBitmap)
    {
      pWICBitmap->Release();
      pWICBitmap = NULL;
    }

    if (pTextLayout_)
    {
      pTextLayout_->Release();
      pTextLayout_ = NULL;
    }

    if (pTextFormat)
    {
      pTextFormat->Release();
      pTextFormat = NULL;
    }
  }

   void StaticText::UpdateTextRendering()
   {
     Size sz = ComputeTextSize();

     if (sz.width == 0 || sz.height == 0)
     {
       // Nothing to render
       if (dw_texture_.IsValid())
       {
         dw_texture_.Release();
       }
       return;
     }

     RasterizeText(color::White);
     update_text_rendering_ = false;
   }
#elif defined(NUX_STATIC_TEXT_USE_CAIRO)
  Size StaticText::ComputeTextSize(bool assign, bool with_clipping)
  {
    cairo_surface_t*      pango_surface = NULL;
    cairo_t*              cairo_ctx     = NULL;
    PangoLayout*          pango_layout  = NULL;
    PangoFontDescription* font_desc     = NULL;
    PangoContext*         pango_ctx     = NULL;
    PangoRectangle        ink_rect      = {0, 0, 0, 0};
    PangoRectangle        logical_rect  = {0, 0, 0, 0};
    int                   dpi           = 96;

    // Create Cairo surface.
    pango_surface = cairo_image_surface_create(CAIRO_FORMAT_A1, 1, 1);
    
    // Create Cairo context.
    cairo_ctx = cairo_create(pango_surface);
    
    // Create layout.
    pango_layout = pango_cairo_create_layout(cairo_ctx);
    {
      pango_layout_set_wrap     (pango_layout, PANGO_WRAP_WORD_CHAR);
      pango_layout_set_ellipsize(pango_layout, PANGO_ELLIPSIZE_END);
      pango_layout_set_markup   (pango_layout, text_.c_str(), -1);
    }

    // Create font description: "[FAMILY-LIST] [STYLE-OPTIONS] [SIZE]"
    font_desc = pango_font_description_from_string(pango_font_name_.c_str());
    {
      pango_font_description_set_weight(font_desc, PANGO_WEIGHT_NORMAL);
      pango_layout_set_font_description(pango_layout, font_desc);
    }

    // Get Pango context
    pango_ctx = pango_layout_get_context(pango_layout); // is not ref'ed

    // Set font options
    CairoFontOptions font_options;
    {
      cairo_font_options_set_antialias      (font_options, CAIRO_ANTIALIAS_DEFAULT);
      cairo_font_options_set_subpixel_order(font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
      cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
      cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);
      cairo_set_font_options(cairo_ctx, font_options);

      pango_cairo_context_set_font_options(pango_ctx, font_options);
    }

    // use some default DPI-value
    pango_cairo_context_set_resolution(pango_ctx, dpi);

    pango_layout_context_changed(pango_layout);
    pango_layout_get_extents(pango_layout, &ink_rect, &logical_rect);

    int text_width = std::ceil((float)logical_rect.width / PANGO_SCALE);
    int text_height = std::ceil((float)logical_rect.height / PANGO_SCALE);

    if (assign)
    {
      text_width_ = text_width;
      text_height_ = text_height;

      padding_x_ = text_width_ - logical_rect.width / PANGO_SCALE;
      padding_y_ = text_height_ - logical_rect.height / PANGO_SCALE;

      if (with_clipping && (clip_to_width_ > 0))
      {
        text_width_ = text_width_ <= clip_to_width_ ? text_width_ : clip_to_width_;
      }

      text_width = text_width_;
      text_height = text_height_;
    }

    // clean up
    pango_font_description_free(font_desc);
    g_object_unref(pango_layout);
    cairo_destroy(cairo_ctx);
    cairo_surface_destroy(pango_surface);

    return Size(text_width, text_height);
  }

  void StaticText::RasterizeText(void* cairo_context, Color /* color */)
  {
    cairo_t* cairo_ctx = (cairo_t*) cairo_context;

    PangoLayout*          pango_layout  = NULL;
    PangoFontDescription* font_desc     = NULL;
    PangoContext*         pango_ctx     = NULL;
    int                   dpi            = 96;

    // Create layout.
    pango_layout = pango_cairo_create_layout(cairo_ctx);
    {
      pango_layout_set_wrap     (pango_layout, PANGO_WRAP_WORD_CHAR);
      pango_layout_set_ellipsize(pango_layout, PANGO_ELLIPSIZE_END);
      pango_layout_set_markup   (pango_layout, text_.c_str(), -1);

      // Sets the width to which the lines of the PangoLayout should wrap or ellipsized. The default value is -1: no width set.
      pango_layout_set_width(pango_layout, (clip_to_width_ ? clip_to_width_ : text_width_) * PANGO_SCALE);
    }

    // Create font description: "[FAMILY-LIST] [STYLE-OPTIONS] [SIZE]"
    font_desc = pango_font_description_from_string(pango_font_name_.c_str());
    {
      pango_font_description_set_weight(font_desc, PANGO_WEIGHT_NORMAL);
      pango_layout_set_font_description(pango_layout, font_desc);
    }

    // Get Pango context
    pango_ctx = pango_layout_get_context(pango_layout); // is not ref'ed

    // Set font options
    CairoFontOptions font_options;
    {
      cairo_font_options_set_antialias      (font_options, CAIRO_ANTIALIAS_DEFAULT);
      cairo_font_options_set_subpixel_order (font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
      cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
      cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);
      cairo_set_font_options(cairo_ctx, font_options);

      pango_cairo_context_set_font_options(pango_ctx, font_options);
    }

    pango_cairo_context_set_resolution(pango_ctx, dpi);

    cairo_set_source_rgba(cairo_ctx, 1.0, 1.0, 1.0, 1.0);

    pango_layout_context_changed(pango_layout);

    cairo_move_to(cairo_ctx, padding_x_, padding_y_);
    pango_cairo_show_layout(cairo_ctx, pango_layout);

    // clean up
    pango_font_description_free(font_desc);
    g_object_unref(pango_layout);
  }

  void StaticText::UpdateTextRendering()
  {
    Size sz = ComputeTextSize();

    if (sz.width == 0 || sz.height == 0)
    {
      // Nothing to render
      if (dw_texture_.IsValid())
      {
        dw_texture_.Release();
      }
      return;
    }

    cairo_graphics_ = new CairoGraphics(CAIRO_FORMAT_ARGB32, sz.width, sz.height);
    cairo_t* cairo_ctx = cairo_graphics_->GetContext();
    cairo_set_operator(cairo_ctx, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cairo_ctx);
    cairo_set_operator(cairo_ctx, CAIRO_OPERATOR_OVER);

    RasterizeText(cairo_ctx, text_color_);

    NBitmapData* bitmap = cairo_graphics_->GetBitmap();

    // NTexture2D is the high level representation of an image that is backed by
    // an actual opengl texture.

    BaseTexture* rasterized_text_texture = NULL;

    rasterized_text_texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    rasterized_text_texture->Update(bitmap);
    dw_texture_ = rasterized_text_texture->GetDeviceTexture();

    rasterized_text_texture->UnReference();
    rasterized_text_texture = NULL;

    delete bitmap;
    cairo_destroy(cairo_ctx);
    delete cairo_graphics_;
    cairo_graphics_ = NULL;

    update_text_rendering_ = false;
  }

#endif
}
