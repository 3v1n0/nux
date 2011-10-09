#include "Nux.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Validator.h"

#include "cairo/cairo.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"
#include "NuxImage/CairoGraphics.h"
#include "NuxGraphics/GraphicsDisplay.h"

#include "StaticText.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(StaticText);

  StaticText::StaticText (const std::string &text, NUX_FILE_LINE_DECL)
    : View (NUX_FILE_LINE_PARAM)
  {
    padding_ = 2;
    text_width_ = 0;
    text_height_ = 0;

#if defined(NUX_OS_WINDOWS)
    layout_left_ = 0;
    layout_top_ = 0;
    
    GetGraphicsDisplay()->GetDirect2DFactory()->GetDesktopDpi(&dpi_scale_x, &dpi_scale_y);
    dpi_scale_x /= 96.0f;
    dpi_scale_y /= 96.0f;

    // 1 DIP = (1/96) inch;  1 inch = 96 DIP
    // PixelToDIP = (pixels/dpi)*96.0f

    font_size_ = 12.0f;
    font_name_ = "Tahoma";

#else
    font_size_ = 20.0f;
    font_name_ = "Ubuntu";
    std::ostringstream os;
    os << font_name_ << " " << font_size_;
    pango_font_name_ = std::string(os.str());

    dpy_ = 96.0f;
#endif

    _size_match_text = true;
    text_color_ = color::White;
    clip_to_width_ = 0;

    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    SetText(text);
  }

  StaticText::~StaticText ()
  {
#if defined(NUX_OS_LINUX)
    if (cairo_graphics_ != 0)
      delete (cairo_graphics_);
#endif

    if (dw_texture_.IsValid())
      dw_texture_.Release();
  }

  void StaticText::PreLayoutManagement()
  {
    int textWidth  = 0;
    int textHeight = 0;

    GetTextSize(textWidth, textHeight);

    _pre_layout_width = GetBaseWidth();
    _pre_layout_height = GetBaseHeight();

    SetBaseSize(textWidth, textHeight);

    if (dw_texture_.IsNull())
    {
      UpdateTextRendering();
    }

    View::PreLayoutManagement();
  }

  long StaticText::PostLayoutManagement(long layoutResult)
  {
    long result = 0;

    int w = GetBaseWidth();
    int h = GetBaseHeight();

    if (_pre_layout_width < w)
      result |= eLargerWidth;
    else if (_pre_layout_width > w)
      result |= eSmallerWidth;
    else
      result |= eCompliantWidth;

    if (_pre_layout_height < h)
      result |= eLargerHeight;
    else if (_pre_layout_height > h)
      result |= eSmallerHeight;
    else
      result |= eCompliantHeight;

    return result;
  }

  void StaticText::SetSizeMatchText(bool size_match_text)
  {
    _size_match_text = size_match_text;
  }

  bool StaticText::GetSizeMatchText() const
  {
    return _size_match_text;
  }

  void StaticText::SetClipping(int clipping)
  {
    if (clip_to_width_ == clipping)
      return;

    clip_to_width_ = clipping;

    if (clip_to_width_ < 0)
      clip_to_width_ = 0;

    UpdateTextRendering();
  }

  int StaticText::GetClipping() const
  {
    return clip_to_width_;
  }

  void StaticText::Draw(GraphicsEngine& gfxContext, bool forceDraw)
  {
    if(!dw_texture_.IsValid())
      return;

    Geometry base = GetGeometry();

    nux::GetPainter().PaintBackground(gfxContext, base);

    // Get the current blend states. They will be restored later.
    t_u32 alpha = 0, src = 0, dest = 0;
    gfxContext.GetRenderStates().GetBlend (alpha, src, dest);
    
    gfxContext.GetRenderStates().SetBlend (true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    gfxContext.PushClippingRectangle(base);

    TexCoordXForm texxform;
    texxform.SetWrap(TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);

    gfxContext.QRP_1Tex(base.x,
      base.y,
      base.width,
      base.height,
      dw_texture_,
      texxform,
      text_color_);

    gfxContext.GetRenderStates().SetBlend(alpha, src, dest);
    gfxContext.PopClippingRectangle();
  }

  void StaticText::SetText(const std::string &text)
  {
    if (text_ == text)
      return;

    text_ = text;

    ComputeTextSize();
    UpdateTextRendering();

    text_changed.emit(this);
    QueueDraw();
  }

  std::string StaticText::GetText() const
  {
    return text_;
  }

  void StaticText::SetTextColor(const Color &textColor)
  {
    if (text_color_ == textColor)
      return;

    text_color_ = textColor;
    text_color_changed.emit(this);
  }

  void StaticText::SetFontName(const std::string &font_name)
  {
    if (font_name_ == font_name)
      return;

    font_name_ = font_name;

#if defined(NUX_OS_LINUX)
    std::ostringstream os;
    os << font_name_ << " " << font_size_;
    pango_font_name_ = std::string(os.str());
#endif

    ComputeTextSize();
    UpdateTextRendering();

    QueueDraw();
  }

  void StaticText::GetTextSize(int &width, int &height) const
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

  Size StaticText::GetTextSize() const
  {
    Size sz;
    GetTextSize(sz.width, sz.height);
    return sz;
  }

#if defined(NUX_OS_WINDOWS)
  void StaticText::ComputeTextSize()
  {
    HRESULT hr;
    IDWriteFactory *pDWriteFactory = GetGraphicsDisplay()->GetDirectWriteFactory();

    ID2D1RenderTarget *pRT = NULL;
    IDWriteTextFormat *pTextFormat = NULL;

    hr = pDWriteFactory->CreateTextFormat(
        ANSICHAR_TO_UNICHAR(font_name_.c_str()),                  // Font family name.
        NULL,                       // Font collection (NULL sets it to use the system font collection).
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        font_size_,
        L"en-us",
        &pTextFormat);

    // Center align (horizontally) the text.
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
      pTextFormat,                         // The text format to apply to the string (contains font information, etc).
      1,                                    // The width of the layout box.
      1,                                    // The height of the layout box.
      &pTextLayout_                         // The IDWriteTextLayout interface pointer.
      );

    DWRITE_TEXT_METRICS metrics;
    pTextLayout_->GetMetrics(&metrics);

    text_width_ = metrics.widthIncludingTrailingWhitespace * dpi_scale_x + 2 * padding_;
    text_height_ = metrics.height * dpi_scale_y + 2 * padding_;
    
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
  }

  void StaticText::RasterizeText(Color color)
  {
    if ((text_width_ <=0) || (text_height_ <= 0))
    {
      dw_texture_.Release();
      return;
    }

    HRESULT hr;

    ID2D1Factory *pD2DFactory = GetGraphicsDisplay()->GetDirect2DFactory();
    IDWriteFactory *pDWriteFactory = GetGraphicsDisplay()->GetDirectWriteFactory();
    IWICImagingFactory *pWICFactory = GetGraphicsDisplay()->GetWICFactory();

    ID2D1RenderTarget *pRT = NULL;
    IDWriteTextFormat *pTextFormat = NULL;

    hr = pDWriteFactory->CreateTextFormat(
      ANSICHAR_TO_UNICHAR(font_name_.c_str()),                  // Font family name.
      NULL,                       // Font collection (NULL sets it to use the system font collection).
      DWRITE_FONT_WEIGHT_REGULAR,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      font_size_,
      L"en-us",
      &pTextFormat);

    // Center align (horizontally) the text.
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
      pTextFormat,                         // The text format to apply to the string (contains font information, etc).
      text_width_ / dpi_scale_x,                                    // The width of the layout box.
      text_height_ / dpi_scale_y,                                    // The height of the layout box.
      &pTextLayout_                         // The IDWriteTextLayout interface pointer.
      );

    IWICBitmap *pWICBitmap = NULL;

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


    ID2D1SolidColorBrush *pBrush;
    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(color.red, color.green, color.blue, color.alpha)), &pBrush);


    D2D1_POINT_2F origin = D2D1::Point2F(static_cast<FLOAT>(padding_ / dpi_scale_x), static_cast<FLOAT>(padding_ / dpi_scale_y));

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

    {
      unsigned int width, height;
      pWICBitmap->GetSize(&width, &height);

      dw_texture_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateTexture(width, height, 1, nux::BITFMT_B8G8R8A8);

      nux::SURFACE_LOCKED_RECT lock_rect;
      dw_texture_->LockRect(0, &lock_rect, NULL);

      pWICBitmap->CopyPixels(NULL, lock_rect.Pitch, 4*width*height, (BYTE*) lock_rect.pBits);

      dw_texture_->UnlockRect(0);

    }

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
     int width = 0;
     int height = 0;
     GetTextSize(width, height);

     if (GetSizeMatchText() && (width != 0) && (height != 0))
     {
       SetMinMaxSize(width, height);
     }

     RasterizeText(color::White);
   }
#else
  void StaticText::ComputeTextSize()
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
      cairo_font_options_set_subpixel_order (font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
      cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
      cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);
      cairo_set_font_options(cairo_ctx, font_options);

      pango_cairo_context_set_font_options(pango_ctx, font_options);
    }

    if (clip_to_width_)
    {
      pango_layout_set_width(pango_layout, clip_to_width_ * PANGO_SCALE);
    }


    // use some default DPI-value
    pango_cairo_context_set_resolution(pango_ctx, dpi);

    pango_layout_context_changed(pango_layout);
    pango_layout_get_extents(pango_layout, &ink_rect, &logical_rect);

    text_width_ = logical_rect.width / PANGO_SCALE + 2 * padding_;
    text_height_ = logical_rect.height / PANGO_SCALE + 2 * padding_;

    // clean up
    pango_font_description_free(font_desc);
    g_object_unref(pango_layout);
    cairo_destroy(cairo_ctx);
    cairo_surface_destroy(pango_surface);
  }

  void StaticText::RasterizeText(void* cairo_context, Color color)
  {
    cairo_t* cairo_ctx = (cairo_t*) cairo_context;

    PangoLayout*          pango_layout  = NULL;
    PangoFontDescription* font_desc     = NULL;
    PangoContext*         pango_ctx     = NULL;
    int                   dpi           = 96;

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
      cairo_font_options_set_subpixel_order (font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
      cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
      cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);
      cairo_set_font_options(cairo_ctx, font_options);

      pango_cairo_context_set_font_options(pango_ctx, font_options);
    }

    if (clip_to_width_)
    {
      pango_layout_set_width(pango_layout, clip_to_width_ * PANGO_SCALE);
    }

    pango_cairo_context_set_resolution(pango_ctx, dpi);

    cairo_set_source_rgba(cairo_ctx, 1.0, 1.0, 1.0, 1.0);

    pango_layout_context_changed(pango_layout);

    cairo_move_to(cairo_ctx, padding_, padding_);
    pango_cairo_show_layout(cairo_ctx, pango_layout);

    // clean up
    pango_font_description_free(font_desc);
    g_object_unref(pango_layout);
  }

  void StaticText::UpdateTextRendering()
  {
    int width = 0;
    int height = 0;
    GetTextSize(width, height);

    if (GetSizeMatchText() && (width != 0) && (height != 0))
    {
      SetMinMaxSize(width, height);
    }

    cairo_graphics_ = new CairoGraphics(CAIRO_FORMAT_ARGB32, GetBaseWidth(), GetBaseHeight());
    cairo_t *cairo_ctx = cairo_graphics_->GetContext ();
    cairo_set_operator(cairo_ctx, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cairo_ctx);
    cairo_set_operator(cairo_ctx, CAIRO_OPERATOR_OVER);

    RasterizeText(cairo_ctx, text_color_);

    NBitmapData* bitmap = cairo_graphics_->GetBitmap();

    // NTexture2D is the high level representation of an image that is backed by
    // an actual opengl texture.

    BaseTexture *rasterized_text_texture = NULL;

    rasterized_text_texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    rasterized_text_texture->Update(bitmap);
    dw_texture_ = rasterized_text_texture->GetDeviceTexture();

    rasterized_text_texture->UnReference();
    rasterized_text_texture = NULL;

    delete bitmap;
    cairo_destroy(cairo_ctx);
    delete cairo_graphics_;
    cairo_graphics_ = 0;
  }

#endif
}
