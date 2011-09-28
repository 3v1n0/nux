#ifndef STATICTEXT_H
#define STATICTEXT_H


namespace nux
{
  class CairoGraphics;
  class StaticText: public View
  {
    NUX_DECLARE_OBJECT_TYPE(StaticText, View);
  public:
    //StaticText (const TCHAR* text, NUX_FILE_LINE_PROTO);
    StaticText (const std::string &text, NUX_FILE_LINE_PROTO);

    ~StaticText ();


    //! Set size of widget according to the text extent.
    /*!
        @param size_match_text If true, the widget size is set to match the size of the text on the screen.
    */
    void SetSizeMatchText (bool size_match_text);

    //! Return true if the widget with changes to match the text width.
    bool GetSizeMatchText () const;

    void SetText(const std::string &text);
    std::string GetText() const;

    void SetTextColor (Color textColor);
    void SetFontName (const char *font_name);
    
    void GetTextSize (int &width, int &height, int clipping = 0);

    static void GetTextSize (const TCHAR* font, const TCHAR *char_str, int& width, int& height, int clipping = 0);

    void SetClipping (int clipping);
    int GetClipping () const;

    sigc::signal<void, StaticText*> sigTextChanged;
    sigc::signal<void, StaticText*> sigTextColorChanged;

  protected:
    void PreLayoutManagement ();
    long PostLayoutManagement (long layoutResult);

    long ProcessEvent (IEvent& event, long traverseInfo, long processEventInfo);
    void Draw (GraphicsEngine& gfxContext, bool forceDraw);
    void DrawContent (GraphicsEngine& gfxContext, bool forceDraw);
    void PostDraw (GraphicsEngine& gfxContext, bool forceDraw);

    std::string text_;
    Color _textColor;

    CairoGraphics *_cairoGraphics;
    BaseTexture *_texture2D;

    int _pre_layout_width;
    int _pre_layout_height;

    bool _size_match_text;

    void DrawText (void* cairo_context, int width, int height, Color color);

    void UpdateTextRendering ();

    char *_font_string;

    int _clipping;
  };

}

#endif // STATICTEXT_H
