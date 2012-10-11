#include "Nux.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Validator.h"

#include "cairo/cairo.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"
#include "NuxGraphics/CairoGraphics.h"

#include "PangoText.h"

namespace nux
{
  PangoText::PangoText(const char* text, NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
  {
    _pango_font_map   = pango_ft2_font_map_new();
    _pango_context    = pango_font_map_create_context(PANGO_FONT_MAP(_pango_font_map));
    _pango_layout     = pango_layout_new(_pango_context);
    _pango_font_desc  = pango_font_description_from_string("Sans, 20");

    _size_match_text = true;
    _textColor  = color::White;
    _texture2D  = 0;

    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    SetText(text);
  }

  PangoText::~PangoText()
  {
    if (_cairoGraphics == 0)
      delete(_cairoGraphics);

    if (_texture2D == 0)
      delete(_texture2D);
  }

  void PangoText::PreLayoutManagement()
  {
    int          textWidth  = 0;
    int          textHeight = 0;

    GetTextExtents("Ubuntu", textWidth, textHeight);

    _pre_layout_width = GetBaseWidth();
    _pre_layout_height = GetBaseHeight();

    SetBaseSize(textWidth, textHeight);

    if (_texture2D == 0)
    {
      UpdateTextLayout();
    }

    View::PreLayoutManagement();
  }

  long PangoText::PostLayoutManagement(long layoutResult)
  {
    //  long result = View::PostLayoutManagement(layoutResult);

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

  void PangoText::SetSizeMatchText(bool size_match_text)
  {
    _size_match_text = size_match_text;
  }

  bool PangoText::GetSizeMatchText() const
  {
    return _size_match_text;
  }

  void PangoText::Draw(GraphicsEngine& gfxContext, bool forceDraw)
  {
    Geometry base = GetGeometry();

    gfxContext.PushClippingRectangle(base);

    TexCoordXForm texxform;
    texxform.SetWrap(TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);

    gfxContext.GetRenderStates().SetBlend(true,
      GL_ONE,
      GL_ONE_MINUS_SRC_ALPHA);

    gfxContext.QRP_1Tex(base.x,
      base.y,
      base.width,
      base.height,
      _texture2D->GetDeviceTexture(),
      texxform,
      _textColor);

    gfxContext.GetRenderStates().SetBlend(false);

    gfxContext.PopClippingRectangle();
  }

  void PangoText::DrawContent(GraphicsEngine& gfxContext, bool forceDraw)
  {

  }

  void PangoText::SetText(std::string text)
  {
    if (_text != text)
    {
      _text = text;
      UpdateTextLayout();

      sigTextChanged.emit(this);
    }
  }

  void PangoText::SetTextColor(Color textColor)
  {
    if (_textColor != textColor)
    {
      _textColor = textColor;
      sigTextColorChanged.emit(this);
    }
  }

  void PangoText::GetTextExtents(int &width, int &height)
  {
    GetTextExtents("Ubuntu", width, height);
  }

  void PangoText::GetTextExtents(const char* font, int& width, int& height)
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

    surface = cairo_image_surface_create(CAIRO_FORMAT_A1, 1, 1);
    cr = cairo_create(surface);

    CairoFontOptions font_options;

    cairo_font_options_set_antialias      (font_options, CAIRO_ANTIALIAS_DEFAULT);
    cairo_font_options_set_subpixel_order(font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
    cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
    cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);

    cairo_set_font_options(cr, font_options);

    layout = pango_cairo_create_layout(cr);
    desc = pango_font_description_from_string(font);
    pango_font_description_set_weight(desc, PANGO_WEIGHT_NORMAL);
    pango_layout_set_font_description(layout, desc);
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_text(layout, _text.c_str(), -1);

    pangoCtx = pango_layout_get_context(layout); // is not ref'ed
    pango_cairo_context_set_font_options(pangoCtx, font_options);

    // use some default DPI-value
    pango_cairo_context_set_resolution(pangoCtx, dpi);

    pango_layout_context_changed(layout);
    pango_layout_get_extents(layout, NULL, &logRect);

    width  = logRect.width / PANGO_SCALE;
    height = logRect.height / PANGO_SCALE;

    // clean up
    pango_font_description_free(desc);
    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
  }

  void PangoText::DrawText(void* cairo_context, int width, int height, Color color)
  {
    cairo_t* cr = (cairo_t*) cairo_context;

    int                   textWidth  = 0;
    int                   textHeight = 0;
    PangoLayout*          layout     = NULL;
    PangoFontDescription* desc       = NULL;
    PangoContext*         pangoCtx   = NULL;
    int                   dpi        = 0;

    GetTextExtents("Ubuntu", textWidth, textHeight);

    CairoFontOptions font_options;

    cairo_font_options_set_antialias      (font_options, CAIRO_ANTIALIAS_DEFAULT);
    cairo_font_options_set_subpixel_order(font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
    cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
    cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);

    cairo_set_font_options(cr, font_options);

    layout = pango_cairo_create_layout(cr);
    desc = pango_font_description_from_string("Ubuntu");
    pango_layout_set_font_description(layout, desc);

    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_text(layout, _text.c_str(), -1);

    pangoCtx = pango_layout_get_context(layout);
    pango_cairo_context_set_font_options(pangoCtx, font_options);

    pango_cairo_context_set_resolution(pangoCtx, dpi);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(cr, 0.0f, 0.0f, 0.0f, 0.0f);
    cairo_paint(cr);
    cairo_set_source_rgba(cr, color.red, color.green, color.blue, color.alpha);

    pango_layout_context_changed(layout);

    cairo_move_to(cr, 0.0f, 0.0f);
    pango_cairo_show_layout(cr, layout);

    // clean up
    pango_font_description_free(desc);
    g_object_unref(layout);
  }

  void PangoText::UpdateTextLayout()
  {
    pango_layout_set_text(_pango_layout, _text.c_str(), -1);

    // reset the glyph string
    if (_glyph_text)
      delete(_glyph_text);

    _glyph_text = 0;
    _glyph_length = 0;
    int max_g = 0;
    double pango_to_ink = (1.0/((double) PANGO_SCALE));

    PangoLayoutIter* pIter = pango_layout_get_iter(_pango_layout); // and go!
    do
    {
//       typedef struct {
//         PangoLayout *layout;
//         gint         start_index;            // start of line as byte index into layout->text.
//         gint         length;                 // length of line in bytes.
//         GSList      *runs;                   // a list containing the runs of the line in visual order.
//         guint        is_paragraph_start : 1; // TRUE if this is the first line of the paragraph.
//         guint        resolved_dir : 3;       // Resolved direction of line.
//       } PangoLayoutLine;

      PangoLayoutLine* pLine = pango_layout_iter_get_line(pIter);  // no need for unref
      int              plOffset = pLine->start_index;               // start of the line in the uni32_text
      PangoRectangle   ink_r;
      PangoRectangle   log_r;

      pango_layout_iter_get_line_extents(pIter, &ink_r, &log_r);
      double           plY = (1.0 / ((double) PANGO_SCALE)) * ((double) log_r.y); // start position of this line of the layout
      double           plX = (1.0 / ((double) PANGO_SCALE)) * ((double) log_r.x);
      GSList* curR = pLine->runs;

       // Iterate over the runs of this line
      while (curR)
      {
//         typedef struct {
//           PangoItem        *item;    // a PangoItem structure that provides information about a segment of text.
//           PangoGlyphString *glyphs;  // the glyphs obtained by shaping the text corresponding to item.
//         } PangoLayoutRun, PangoGlyphItem;
//
//         typedef struct {
//           gint offset;     // the offset of the segment from the beginning of the string in bytes.
//           gint length;     // the length of the segment in bytes.
//           gint num_chars;  // the length of the segment in characters.
//           PangoAnalysis analysis;  // the properties of the segment.
//         } PangoItem;

//         typedef struct {
//           gint num_glyphs;
//
//           PangoGlyphInfo *glyphs;
//
//           /* This is a memory inefficient way of representing the information
//            * here - each value gives the byte index within the text
//            * corresponding to the glyph string of the start of the cluster to
//            * which the glyph belongs.
//            */
//           gint *log_clusters;
//         } PangoGlyphString;

        PangoLayoutRun* pRun = (PangoLayoutRun*) curR->data;
        int prOffset = pRun->item->offset; // the offset of the segment from the beginning of the line in bytes
        if (pRun)
        {
          // a run has uniform font/directionality/etc...
          int o_g_l = _glyph_length; // save the index of the first glyph we'll add
          for (int i = 0; i < pRun->glyphs->num_glyphs; i++)
          {
            // add glyph sequentially, reading them from the run. realloc space for the glyphs
            if (_glyph_length >= max_g)
            {
              max_g = 2 * _glyph_length + 1;
              pango_glyph* temp = new pango_glyph [max_g + 1];
              if (_glyph_length == 0)
              {
                _glyph_text = temp;
              }
              else
              {
                std::memcpy(temp, _glyph_text, _glyph_length * sizeof(pango_glyph));
                delete _glyph_text;
                _glyph_text = temp;
              }
            }

            // fill the glyph info
            _glyph_text[_glyph_length].font   = pRun->item->analysis.font;
            _glyph_text[_glyph_length].gl     = pRun->glyphs->glyphs[i].glyph;
            _glyph_text[_glyph_length].uni_st = plOffset + prOffset + pRun->glyphs->log_clusters[i];
            // depending on the directionality, the last uni32 codepoint for this glyph is the first of the next char
            // or the first of the previous
            if (pRun->item->analysis.level == 1)
            {
              // rtl
              if (i < pRun->glyphs->num_glyphs - 1)
              {
                _glyph_text[_glyph_length + 1].uni_en = _glyph_text[_glyph_length].uni_st;
              }
              _glyph_text[_glyph_length].uni_dir = 1;
              _glyph_text[_glyph_length + 1].uni_dir = 1; // set the directionality for the next too, so that the last glyph in
              // the array has the correct direction
            }
            else
            {
              // ltr
              if (i > 0)
              {
                _glyph_text[_glyph_length - 1].uni_en = _glyph_text[_glyph_length].uni_st;
              }
              _glyph_text[_glyph_length].uni_dir = 0;
              _glyph_text[_glyph_length + 1].uni_dir = 0;
            }
            // set the position
            // the layout is an infinite line
            _glyph_text[_glyph_length].x = plX + pango_to_ink * ((double) pRun->glyphs->glyphs[i].geometry.x_offset);
            _glyph_text[_glyph_length].y = plY + pango_to_ink * ((double) pRun->glyphs->glyphs[i].geometry.y_offset);
            // advance to the next glyph
            plX += pango_to_ink * ((double) pRun->glyphs->glyphs[i].geometry.width);
            // and set the next glyph's position, in case it's the terminating glyph
            _glyph_text[_glyph_length+1].x = plX;
            _glyph_text[_glyph_length+1].y = plY;
            _glyph_length++;
          }
          // and finish filling the info
          // notably, the uni_en of the last char in ltr text and the uni_en of the first in rtl are still not set
          if (pRun->item->analysis.level == 1)
          {
            // rtl
            if (_glyph_length > o_g_l)
              _glyph_text[o_g_l].uni_en=plOffset+prOffset+pRun->item->length;
          }
          else
          {
            if (_glyph_length > 0)
              _glyph_text[_glyph_length-1].uni_en=plOffset+prOffset+pRun->item->length;
          }
          // the terminating glyph has glyph_id=0 because it means 'no glyph'
          _glyph_text[_glyph_length].gl = 0;
          // and is associated with no text(but you cannot set uni_st=uni_en=0, because the termination
          // is expected to be the glyph for the termination of the uni32_text)
          _glyph_text[_glyph_length].uni_st = _glyph_text[_glyph_length].uni_en = plOffset + prOffset + pRun->item->length;
        }
        curR = curR->next;
      }

    } while ( pango_layout_iter_next_line(pIter));

    pango_layout_iter_free(pIter);


  }

  void PangoText::ComputeTextLayout()
  {
    PangoLayout*          layout   = NULL;
    PangoFontDescription* desc     = NULL;
    PangoContext*         pangoCtx = NULL;
    PangoRectangle        logRect  = {0, 0, 0, 0};
    int                   dpi      = 96;


  }
}
