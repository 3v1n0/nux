#ifndef STATICTEXT_H
#define STATICTEXT_H


namespace nux
{
  class CairoGraphics;
  class StaticText: public View
  {
  public:
    StaticText (const TCHAR* text, NUX_FILE_LINE_PROTO);

    ~StaticText ();


    //! Set size of widget according to the text extent.
    /*!
        @param size_match_text If true, the widget size is set to match the size of the text on the screen.
    */
    void SetSizeMatchText (bool size_match_text);

    //! Return true if the widget with changes to match the text width.
    bool GetSizeMatchText () const;

    void SetText (NString text);
    void SetTextColor (Color textColor);
    

    sigc::signal<void, StaticText*> sigTextChanged;
    sigc::signal<void, StaticText*> sigTextColorChanged;

  protected:
    void PreLayoutManagement ();
    long PostLayoutManagement (long layoutResult);

    long ProcessEvent (IEvent& event, long traverseInfo, long processEventInfo);
    void Draw (GraphicsEngine& gfxContext, bool forceDraw);
    void DrawContent (GraphicsEngine& gfxContext, bool forceDraw);
    void PostDraw (GraphicsEngine& gfxContext, bool forceDraw);

    void GetTextExtents (int &width, int &height);

    NString _text;
    Color _textColor;

    CairoGraphics *_cairoGraphics;
    BaseTexture *_texture2D;

    int _pre_layout_width;
    int _pre_layout_height;

    bool _size_match_text;

    void GetTextExtents (const TCHAR* font, int& width, int& height);
    void DrawText (void* cairo_context, int width, int height, Color color);

    void UpdateTextRendering ();

  };

}

#endif // STATICTEXT_H
