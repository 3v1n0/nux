#ifndef STATICTEXT_H
#define STATICTEXT_H

#include "NuxCore/Size.h"

   #if defined(NUX_OS_WINDOWS)
     #define NUX_STATIC_TEXT_USE_DIRECT_WRITE
   #elif defined(NUX_OS_LINUX)
     #define NUX_STATIC_TEXT_USE_CAIRO
   #else
     #error Not implemented.
   #endif

namespace nux
{
  class CairoGraphics;
  class StaticText: public View
  {
    NUX_DECLARE_OBJECT_TYPE(StaticText, View);
  public:

    //! Text alignment inside the view.
    /*!
        When the text width is smaller than the view width, the text may be aligned
        inside the view. The options are:
            Left
            Right
            Center
        The default alignment is centered.
    */
    enum TextAlignment
    {
      ALIGN_CENTER = 0,
      ALIGN_LEFT,
      ALIGN_RIGHT,
    };

    StaticText(const std::string &text, NUX_FILE_LINE_PROTO);
    virtual ~StaticText();

    //! Set size of widget according to the text extent.
    /*!
        @param size_match_text If true, the widget size is set to match the size of the text on the screen.
    */
    void SetSizeMatchText(bool size_match_text);

    //! Return true if the widget with changes to match the text width.
    bool GetSizeMatchText() const;

    //! Set the text string.
    void SetText(const std::string &text);

    //! Get the text string.
    std::string GetText() const;

    //! Set text color.
    /*!
        Set the text color. The default color is white.

        @param text_color The text color.
    */
    void SetTextColor(const Color &text_color);
    
    //! Get text color.
    /*!
        Get the text color.

        @return The text color.
    */
    Color GetTextColor() const;

    //! Set the font name.
    /*!
        Set the font name. On Ubuntu, the default is "Ubuntu".

        @param font_name The font name.
    */
    void SetFontName(const std::string &font_name);

    //! Get the font name.
    /*!
        Get the font name.

        @return The font name.
    */
    std::string GetFontName() const;

    //! Set text point size.
    /*
        Set the text point size. The value should be greater than 0. Otherwise, the 
        text point size is not changed. \n
        The default value is 10.

        @param size The text point size.
    */
    void SetFontSize(int size);
    void SetTextPointSize(int size); //deprecated: use SetFontSize

    //! Get text point size.
    int GetFontSize() const;
    int GetTextPointSize() const; //deprecated: use GetFontSize

    void GetTextLayoutSize(int &width, int &height) const;

    Size GetTextLayoutSize() const;

    //! Set text alignment inside the view.
    /*!
        @param alignment The text alignment inside the view.
    */
    void SetTextAlignment(TextAlignment alignment);

    //! Return the text alignment
    /*!
        @return The text alignment inside the view.
    */
    TextAlignment GetTextAlignment() const;

    sigc::signal<void, StaticText*> text_changed;

  protected:
    virtual void ApplyMinWidth();
    virtual long ComputeContentSize();
    virtual void Draw(GraphicsEngine& graphics_engine, bool forceDraw);

    int text_width_;  //!< Rasterized text width.
    int text_height_; //!< Rasterized text height.

    std::string text_;
    Color text_color_;

    BaseTexture *rasterized_text_texture_;

    int _pre_layout_width;
    int _pre_layout_height;

    bool _size_match_text;

    int clip_to_width_; //!< Wrapping of line.

    float font_size_;
    std::string font_name_;
    bool update_text_rendering_;

    ObjectPtr<nux::IOpenGLBaseTexture> dw_texture_;
    
    TextAlignment text_alignment_;

    void SetClipping(int clipping);
    int GetClipping() const;

#if defined(NUX_STATIC_TEXT_USE_DIRECT_WRITE)
    Size ComputeTextSize(bool assign = true, bool with_clipping = true);
    void RasterizeText(Color color);
    void UpdateTextRendering();

    float layout_left_;
    float layout_top_;
    float dpi_scale_x;
    float dpi_scale_y;
    
#elif defined (NUX_STATIC_TEXT_USE_CAIRO)
    float dpy_;
    std::string pango_font_name_;  //!< Input to pango_font_description_from_string.

    Size ComputeTextSize(bool assign = true, bool with_clipping = true);
    void RasterizeText(void* cairo_context, Color color);
    void UpdateTextRendering();

    CairoGraphics *cairo_graphics_;
#endif

  private:
    //! Override of Area::SetMinimumHeight and made private.
    /*!
        Prevent changing the minimum height of the StaticText view.
    */
    virtual void SetMinimumHeight(){};

    //! Override of Area::SetMaximumHeight and made private.
    /*!
        Prevent changing the maximum height of the StaticText view.
    */
    virtual void SetMaximumHeight(){};

    //! Compute the full text size.
    /*!
        Compute the full text size, but do not change the parameters of this class.

        @return The full text size.
    */
    Size GetTextSizeNoClip();
    float padding_x_; //!< Adds a padding around the entire text box.
    float padding_y_; //!< Adds a padding around the entire text box.
  };

}

#endif // STATICTEXT_H
