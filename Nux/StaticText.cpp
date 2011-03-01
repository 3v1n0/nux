#include "Nux.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Validator.h"

#include "cairo/cairo.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"
#include "NuxImage/CairoGraphics.h"

#include "StaticText.h"

namespace nux
{
  StaticText::StaticText (const TCHAR* text, NUX_FILE_LINE_DECL)
    : View (NUX_FILE_LINE_PARAM)
  {
    _size_match_text = true;
    _textColor  = Color::White;
    _texture2D  = 0;
    _font_string = g_strdup ("Ubuntu 10");

    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    SetText (text);
  }

  StaticText::~StaticText ()
  {
    if (_cairoGraphics == 0)
      delete (_cairoGraphics);

    if (_texture2D == 0)
      delete (_texture2D);

    if (_font_string)
      g_free (_font_string);
  }

  void StaticText::PreLayoutManagement ()
  {
    int          textWidth  = 0;
    int          textHeight = 0;

    GetTextSize (textWidth, textHeight);

    _pre_layout_width = GetBaseWidth ();
    _pre_layout_height = GetBaseHeight ();

    SetBaseSize (textWidth, textHeight);

    if (_texture2D == 0)
    {
      UpdateTextRendering ();
    }

    View::PreLayoutManagement ();
  }

  long StaticText::PostLayoutManagement (long layoutResult)
  {
    //  long result = View::PostLayoutManagement (layoutResult);

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

  long StaticText::ProcessEvent (IEvent& event,
    long    traverseInfo,
    long    processEventInfo)
  {
    long ret = traverseInfo;

    ret = PostProcessEvent2 (event, ret, processEventInfo);
    return ret;
  }

  void StaticText::SetSizeMatchText (bool size_match_text)
  {
    _size_match_text = size_match_text;
  }

  bool StaticText::GetSizeMatchText () const
  {
    return _size_match_text;
  }

  void StaticText::Draw (GraphicsEngine& gfxContext, bool forceDraw)
  {
    Geometry base = GetGeometry ();

    // Get the current blend states. They will be restored later.
    t_u32 alpha = 0, src = 0, dest = 0;
    gfxContext.GetRenderStates ().GetBlend (alpha, src, dest);
    
    gfxContext.GetRenderStates ().SetBlend (true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    gfxContext.PushClippingRectangle (base);

    TexCoordXForm texxform;
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);

    gfxContext.QRP_1Tex (base.x,
      base.y,
      base.width,
      base.height,
      _texture2D->GetDeviceTexture(),
      texxform,
      _textColor);

    gfxContext.GetRenderStates ().SetBlend (alpha, src, dest);


    gfxContext.PopClippingRectangle ();
  }

  void StaticText::DrawContent (GraphicsEngine& gfxContext, bool forceDraw)
  {

  }

  void StaticText::PostDraw (GraphicsEngine& gfxContext, bool forceDraw)
  {
    // intentionally left empty
  }

  void StaticText::SetText (NString text)
  {
    if (_text != text)
    {
      _text = text;
      UpdateTextRendering ();

      sigTextChanged.emit (this);
    }
  }

  void StaticText::SetTextColor (Color textColor)
  {
    if (_textColor != textColor)
    {
      _textColor = textColor;
      sigTextColorChanged.emit (this);
    }
  }

  void StaticText::SetFontName (const char *font_name)
  {
    g_free (_font_string);
    _font_string = g_strdup (font_name);

    QueueDraw ();
  }

  void StaticText::GetTextSize (int &width, int &height)
  {
    GetTextSize (TEXT("Ubuntu"), _text.GetTCharPtr (), width, height);
  }

  void StaticText::GetTextSize (const TCHAR* font, const TCHAR *char_str, int& width, int& height)
  {
    cairo_surface_t*      surface  = NULL;
    cairo_t*              cr       = NULL;
    PangoLayout*          layout   = NULL;
    PangoFontDescription* desc     = NULL;
    PangoContext*         pangoCtx = NULL;
    PangoRectangle        logRect  = {0, 0, 0, 0};
    int                   dpi      = 96;

    // sanity check
    if (!font)
      return;

    surface = cairo_image_surface_create (CAIRO_FORMAT_A1, 1, 1);
    cr = cairo_create (surface);

    cairo_font_options_t* font_options = cairo_font_options_create();

    cairo_font_options_set_antialias      (font_options, CAIRO_ANTIALIAS_DEFAULT);
    cairo_font_options_set_subpixel_order (font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
    cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
    cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);

    cairo_set_font_options (cr, font_options);

    layout = pango_cairo_create_layout (cr);
    desc = pango_font_description_from_string (font);
    pango_font_description_set_weight (desc, PANGO_WEIGHT_NORMAL);
    pango_layout_set_font_description (layout, desc);
    pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_markup (layout, char_str, -1);

    pangoCtx = pango_layout_get_context (layout); // is not ref'ed
    pango_cairo_context_set_font_options (pangoCtx, font_options);

    // use some default DPI-value
    pango_cairo_context_set_resolution (pangoCtx, dpi);

    pango_layout_context_changed (layout);
    pango_layout_get_extents (layout, NULL, &logRect);

    width  = logRect.width / PANGO_SCALE;
    height = logRect.height / PANGO_SCALE;

    // clean up
    pango_font_description_free (desc);
    g_object_unref (layout);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
  }

  void StaticText::DrawText (void* cairo_context, int width, int height, Color color)
  {
    cairo_t* cr = (cairo_t*) cairo_context;

    int                   textWidth  = 0;
    int                   textHeight = 0;
    PangoLayout*          layout     = NULL;
    PangoFontDescription* desc       = NULL;
    PangoContext*         pangoCtx   = NULL;
    int                   dpi        = 0;

    GetTextSize (textWidth, textHeight);

    cairo_font_options_t* font_options = cairo_font_options_create();

    cairo_font_options_set_antialias      (font_options, CAIRO_ANTIALIAS_DEFAULT);
    cairo_font_options_set_subpixel_order (font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
    cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
    cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);

    cairo_set_font_options (cr, font_options);

    layout = pango_cairo_create_layout (cr);
    desc = pango_font_description_from_string (_font_string);
    pango_layout_set_font_description (layout, desc);

    pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_markup (layout, _text.GetTCharPtr(), -1);

    pangoCtx = pango_layout_get_context (layout);
    pango_cairo_context_set_font_options (pangoCtx, font_options);

    pango_cairo_context_set_resolution (pangoCtx, dpi);

    cairo_set_source_rgba (cr, color.R (),color.G (), color.B (), color.A ());

    pango_layout_context_changed (layout);

    cairo_move_to (cr, 0.0f, 0.0f);
    pango_cairo_show_layout (cr, layout);

    // clean up
    pango_font_description_free (desc);
    g_object_unref (layout);
  }

  void StaticText::UpdateTextRendering ()
  {
    int width = 0;
    int height = 0;
    GetTextSize(width, height);

    if (GetSizeMatchText () && (width != 0) && (height != 0))
    {
      SetMinMaxSize (width, height);
    }

    _cairoGraphics = new CairoGraphics (CAIRO_FORMAT_ARGB32, GetBaseWidth (), GetBaseHeight ());
    cairo_t *cr = _cairoGraphics->GetContext ();
    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint (cr);
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

    DrawText (cr, GetBaseWidth (), GetBaseHeight (), _textColor);

    NBitmapData* bitmap = _cairoGraphics->GetBitmap ();

    // NTexture2D is the high level representation of an image that is backed by
    // an actual opengl texture.

    if (_texture2D)
      _texture2D->UnReference ();

    _texture2D = GetGpuDevice()->CreateSystemCapableTexture ();
    _texture2D->Update (bitmap);

    delete _cairoGraphics;
    _cairoGraphics = 0;
  }

}
