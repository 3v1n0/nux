#ifndef PANGOTEXT_H
#define PANGOTEXT_H

#include "pango/pango.h"
#include "pango/pango-fontmap.h"
#include <pango/pangoft2.h>

namespace nux
{
  // taken from inkscape
  typedef struct
  {
    int               gl;             // glyph_id
    double            x,y;            // glyph position in the layout(nominal sizes, in the [0..1] range)
    bool              char_start;     // is this glyph the beginning of a letter(rtl is taken in account)
    bool              word_start;     // is this glyph the beginning of a word
    bool              para_start;     // is this glyph the beginning of a paragraph(for indentation)
    char              uni_dir;        // bidi orientation of the run containing this glyph
    int               uni_st,uni_en;  // start and end positions of the text corresponding to this glyph: you always have uni_st < uni_en
    PangoFont*        font;           // font this glyph uses(for bidi text, you need several fonts)
  } pango_glyph;


  class CairoGraphics;
  class PangoText: public View
  {
  public:
    PangoText(const char* text, NUX_FILE_LINE_PROTO);

    ~PangoText();


    //! Set size of widget according to the text extent.
    /*!
        @param size_match_text If true, the widget size is set to match the size of the text on the screen.
    */
    void SetSizeMatchText(bool size_match_text);

    //! Return true if the widget with changes to match the text width.
    bool GetSizeMatchText() const;

    void SetText(std::string text);
    void SetTextColor(Color textColor);

    sigc::signal<void, PangoText*> sigTextChanged;
    sigc::signal<void, PangoText*> sigTextColorChanged;

  protected:
    void PreLayoutManagement();
    long PostLayoutManagement(long layoutResult);

    void Draw(GraphicsEngine& gfxContext, bool forceDraw);
    void DrawContent(GraphicsEngine& gfxContext, bool forceDraw);

    void GetTextExtents(int &width, int &height);

    typedef struct
    {
      int               gl;  // glyph_id
      double            x,y; // glyph position in the layout(nominal sizes, in the [0..1] range)
      bool              char_start; // is this glyph the beginning of a letter(rtl is taken in account)
      bool              word_start; // is this glyph the beginning of a word
      bool              para_start; // is this glyph the beginning of a paragraph(for indentation)
      char                                                    uni_dir;    // bidi orientation of the run containing this glyph
      int               uni_st,uni_en; // start and end positions of the text corresponding to this glyph
      // you always have uni_st < uni_en
      PangoFont*        font;  // font this glyph uses(for bidi text, you need several fonts)
    } pango_glyph;

    std::string _text;
    Color _textColor;

    CairoGraphics *_cairoGraphics;
    BaseTexture *_texture2D;

    int _pre_layout_width;
    int _pre_layout_height;
    bool _size_match_text;

    PangoLayout*          _pango_layout;
    PangoContext*         _pango_context;
    PangoFontMap*         _pango_font_map;
    PangoFontDescription* _pango_font_desc;

    pango_glyph*      _glyph_text; // glyph string computed for uni32_text
    int             _glyph_length; // number of glyph in the glyph_text array



    void ComputeTextLayout();
    void GetTextExtents(const char* font, int& width, int& height);
    void DrawText(void* cairo_context, int width, int height, Color color);

    void UpdateTextLayout();


  };
}

#endif // PANGOTEXT_H

