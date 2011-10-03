#ifndef STATICTEXT_H
#define STATICTEXT_H


namespace nux
{
  class CairoGraphics;
  class StaticText: public View
  {
    NUX_DECLARE_OBJECT_TYPE(StaticText, View);
  public:
    StaticText (const std::string &text, NUX_FILE_LINE_PROTO);
    ~StaticText ();


    //! Set size of widget according to the text extent.
    /*!
        @param size_match_text If true, the widget size is set to match the size of the text on the screen.
    */
    void SetSizeMatchText (bool size_match_text);

    //! Return true if the widget with changes to match the text width.
    bool GetSizeMatchText () const;

    //! Set the text string.
    void SetText(const std::string &text);

    //! Get the text string.
    std::string GetText() const;

    //! Set text color.
    void SetTextColor(const Color &textColor);
    void SetFontName(const std::string &font_name);
    
    void GetTextSize(int &width, int &height) const;

    Size GetTextSize() const;

    

    void SetClipping (int clipping);
    int GetClipping () const;

    sigc::signal<void, StaticText*> text_changed;
    sigc::signal<void, StaticText*> text_color_changed;

  protected:
    int text_width_;  //!< Rasterized text width.
    int text_height_; //!< Rasterized text height.

    void PreLayoutManagement ();
    long PostLayoutManagement (long layoutResult);

    void Draw (GraphicsEngine& gfxContext, bool forceDraw);


    std::string text_;
    Color text_color_;

    BaseTexture *rasterized_text_texture_;

    int _pre_layout_width;
    int _pre_layout_height;

    bool _size_match_text;

    int clip_to_width_; //!< Wrapping of line.

    float font_size_;
    std::string font_name_;

    ObjectPtr<nux::IOpenGLBaseTexture> dw_texture_;

#if defined(NUX_OS_WINDOWS)
    void ComputeTextSize();
    void RasterizeText(Color color);
    void UpdateTextRendering ();

    float layout_left_;
    float layout_top_;
    float dpi_scale_x;
    float dpi_scale_y;
    
#else
    float dpy_;
    std::string pango_font_name_;

    void ComputeTextSize();
    void RasterizeText(void* cairo_context, Color color);
    void UpdateTextRendering ();

    CairoGraphics *cairo_graphics_;
#endif

  private:
    int padding_; //!< Adds a padding around the entire text box.
  };

}

#endif // STATICTEXT_H
