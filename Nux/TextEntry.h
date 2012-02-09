#ifndef TEXTENTRY_H
#define TEXTENTRY_H



// Heavily inspired from google gadget code
/*
  Copyright 2008 Google Inc.

  Licensed under the Apache License, Version 2.0(the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/


#include "cairo/cairo.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"
#include "NuxImage/CairoGraphics.h"

#include "Nux/InputMethodContext.h"

namespace nux
{
  class CairoGraphics;

  class CairoFont
  {
  public:
    enum Style {
      STYLE_NORMAL,
      STYLE_ITALIC
    };

    /**
     * Enum used to specify font weight.
     */
    enum Weight {
      WEIGHT_NORMAL,
      WEIGHT_BOLD
    };

    /**
     * Constructor for CairoFont. Takes a PangoFontDescription object and its
     * ownership. Will free the PangoFontDescription object on destruction.
     */
    CairoFont(const std::string &family,
      /*PangoFontDescription *font,*/
      double size,
      Style style,
      Weight weight);

    virtual ~CairoFont();
    virtual Style GetStyle() const;
    virtual Weight GetWeight() const;
    virtual double GetPointSize() const;
    virtual void Destroy();

    const PangoFontDescription *GetFontDescription() const {return font_;}

  private:
    PangoFontDescription *font_;
    double size_;
    Style style_;
    Weight weight_;
  };

  class TextEntry: public View
  {
    enum MovementStep {
      VISUALLY,
      WORDS,
      DISPLAY_LINES,
      DISPLAY_LINE_ENDS,
      PAGES,
      BUFFER
    };

    enum AdjustScrollPolicy {
      NO_SCROLL = 0,
      CENTER_CURSOR,
      MINIMAL_ADJUST
    };

    NUX_DECLARE_OBJECT_TYPE(TextEntry, View);
  public:
    TextEntry(const char* text, NUX_FILE_LINE_PROTO);
    ~TextEntry();

    Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void PostDraw(GraphicsEngine &graphics_engine, bool force_draw);

    void PreLayoutManagement();
    long PostLayoutManagement(long layoutResult);

    //  Receivers

    void RecvMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvKeyEvent(
      unsigned long    eventType  ,   /*event type*/
      unsigned long    keysym     ,   /*event keysym*/
      unsigned long    state      ,   /*event state*/
      const char*     character  ,   /*character*/
      unsigned short   keyCount       /*key repeat count*/);

    void RecvStartKeyFocus();
    void RecvEndKeyFocus();

    bool _size_match_text;
    BaseTexture *_texture2D;

    void MainDraw();
    void ProcessMouseEvent(int event_type, int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void ProcessKeyEvent   (
      unsigned long    eventType  ,   /*event type*/
      unsigned long    keysym     ,   /*event keysym*/
      unsigned long    state      ,   /*event state*/
      const char*     character  ,   /*character*/
      unsigned short   keyCount       /*key repeat count*/);

    void StartKeyboardFocus();
    void EndKeyboardFocus();

    //! Text changed signal
    /*!
        This signal is emitted when the text has changed.
    */
    sigc::signal <void, TextEntry*> sigTextChanged;
    sigc::signal <void> activated;
    sigc::signal <void, int> cursor_moved;

    void SetText(const char *text);
    std::string const& GetText() const;

    void SetTextColor(const Color &color);
    Color const& GetTextColor() const;
    void SetFontFamily(const char *font);
    void SetFontSize(double font_size);
    void SetFontOptions(const cairo_font_options_t *options);

    /** Select text between start and end. */
    void Select(int start, int end);
    /** Select all text */
    void SelectAll();

    CairoGraphics::Alignment GetAlign() const;
    void SetAlign(CairoGraphics::Alignment align);

  protected:
    bool _block_focus; // used to selectively ignore focus key events
    TimeOutSignal *cursor_blink_signal_;
    TimerHandle   cursor_blink_handle_;

    virtual void GeometryChanged();

    void QueueTextDraw();
    
    /*!
        Remove the cached layout.
    */
    void ResetLayout();

    /*!
        Create pango layout on-demand. If the layout is not changed, return the cached one.

        @return A PangoLayout.
     */
    PangoLayout* EnsureLayout();

    /*!
        Create a new layout containing current edit content.

        @return A PangoLayout.
    */
    PangoLayout* CreateLayout();

    /** Create cairo canvas on-demand. */
    CairoGraphics* EnsureCanvas();
    /** Adjust the scroll information */
    void AdjustScroll(AdjustScrollPolicy policy);
    /*!
        Send out a request to refresh all informations of the edit control
        and queue a draw request.

        @param relayout If true, the layout will be regenerated.
        @param adjust_scroll If true, adjust the scrolling.
    */
    void QueueRefresh(bool relayout, AdjustScrollPolicy policy);

    /** Reset the input method context */
    void ResetImContext();
    /** Reset preedit text */
    void ResetPreedit();

    void CursorBlinkCallback(void *data);

    /*!
        Start the blinking of the cursor.
    */
    void StartCursorBlink();

    /*!
        Stop the blinking of the cursor.
    */
    void StopCursorBlink();

    void ShowCursor();
    void HideCursor();

    /** Draw the Cursor to the canvas */
    void DrawCursor(CairoGraphics *canvas);
    /** Draw the text to the canvas */
    void DrawText(CairoGraphics *canvas);

    void GetCursorRects(Rect *strong, Rect *weak);

    void UpdateCursorRegion();

    void UpdateSelectionRegion();

    void UpdateContentRegion();

    /** Move cursor */
    void MoveCursor(MovementStep step, int count, bool extend_selection);
    /** Move cursor visually, meaning left or right */
    int MoveVisually(int current_pos, int count);

    /** Move cursor logically, meaning towards head or end of the text. */
    int MoveLogically(int current_index, int count);

    /*!
        Return the next word boundary around the character position given by \i current_pos.\n
        The direction for which to search for the word boundary is determined by the \i count parameter.\n
        If \i count > 0, search forward for the next word boundary.\n
        If \i count < 0, search backward for the next word boundary.\n
        This call does not physically move the cursor.

        @param current_pos The character position.
        @param count The search direction.

        @return The character position of the word boundary around \i current_pos.
    */
    int MoveWords(int current_pos, int count);

    /** Move cursor in display lines */
    int MoveDisplayLines(int current_pos, int count);
    /** Move cursor in pages */
    int MovePages(int current_pos, int count);
    /** Move cursor to the beginning or end of a display line */
    int MoveLineEnds(int current_pos, int count);

    /** Set the current cursor offset, in number of characters. */
    void SetCursor(int cursor);
    /** Get the most reasonable character offset according to the pixel
     * coordinate in the layout */
    int XYToTextIndex(int x, int y);


    /*!
        Get the offset range that is currently selected, in number of characters.

        @param start Beginning of selection.
        @param end End of selection.

        @return True is there is a current active selection.
    */
    bool GetSelectionBounds(int *start, int *end);

    /*!
        Set the offset range that should be selected, in number of characters.

        @param selection_bound Beginning of selection.
        @param cursor End of selection (also the cursor position).
    */
    void SetSelectionBounds(int selection_bound, int cursor);

    /** Convert index in text_ into index in layout text. */
    int TextIndexToLayoutIndex(int text_index, bool consider_preedit_cursor);

    /** Convert index in layout text into index in text_. */
    int LayoutIndexToTextIndex(int layout_index);

    /** Get char length at index, in number of bytes. */
    int GetCharLength(int index);

    /** Get previous char length before index, in number of bytes. */
    int GetPrevCharLength(int index);

    /** Insert text at current caret position */
    void EnterText(const char *str);
    /** Delete text in a specified range, in number of characters. */
    void DeleteText(int start, int end);

    /** Select the current word under cursor */
    void SelectWord();
    /** Select the current display line under cursor */
    void SelectLine();
    /** Delete the text that is currently selected */
    void DeleteSelection();

    /** Cut the current selected text to the clipboard */
    void CutClipboard();
    /** Copy the current selected text to the clipboard */
    void CopyClipboard();
    /** Paste the text in the clipboard to current offset */
    void PasteClipboard();
    /** Delete a character before the offset of the cursor */
    void BackSpace(MovementStep step);
    /** Delete a character at the offset of the cursor */
    void Delete(MovementStep step);
    /** Switch between the overwrite mode and the insert mode*/
    void ToggleOverwrite();

    /** Gets the color of selection background. */
    Color GetSelectionBackgroundColor();
    /** Gets the color of selection text. */
    Color GetSelectionTextColor();

    /**
     * Gets the cursor location in pango layout. The unit is pixel.
     */
//     void GetCursorLocationInLayout(int *strong_x, int *strong_y, int *strong_height,
//                                    int *weak_x, int *weak_y, int *weak_height);

    void GetCursorLocationInLayout(PangoRectangle *strong, PangoRectangle *weak);

    /** The CairoCanvas which hold cairo_t inside */
    CairoGraphics* canvas_;

    PangoLayout* cached_layout_; //!< The cached Pango Layout.

    /** The text content of the edit control */
    std::string text_;

    /** The preedit text of the edit control */
    std::string preedit_;

    /** Attribute list of the pre-edit text */
    PangoAttrList *preedit_attrs_;

    /**
     *  The character that should be displayed in invisible mode.
     *  If this is empty, then the edit control is visible
     */
    std::string password_char_;

    /** Last time of mouse double click event. */
    unsigned long long last_dblclick_time_;

    /** The current cursor position in number of bytes. */
    int cursor_;
    /**
     * The preedit cursor position within the preedit string,
     * in number of bytes.
     */
    int preedit_cursor_;
    /**
     * The current selection bound in number of bytes,
     * range between cursor_ and selection_bound_ are selected.
     */
    int selection_bound_;

    /** X offset of current scroll, in pixels */
    int scroll_offset_x_;
    /** Y offset of current scroll, in pixels */
    int scroll_offset_y_;

    /**
     * Indicates the status of cursor blinking,
     * 0 means hide cursor
     * otherwise means show cursor.
     * The maximum value would be 2, and decrased by one in each cursor blink
     * callback, then there would be 2/3 visible time and 1/3 invisible time.
     */
    int cursor_blink_status_;

    
    bool characters_revealed_;        //!< Whether the text is visible, decided by password_char_.
    
    bool has_keyboard_focused_;       //!< Whether the text entry is focused.

    /** Whether the input method should be reset */
    bool need_im_reset_;
    /** Whether the keyboard in overwrite mode */
    bool overwrite_;
    /** Whether the button click should select words */
    bool select_words_;
    /** Whether the button click should select lines */
    bool select_lines_;
    /** Whether the left button is pressed */
    bool button_;
    /** Whether the text should be bold */
    bool bold_;
    /** Whether the text should be underlined */
    bool underline_;
    /** Whether the text should be struck-out */
    bool strikeout_;
    /** Whether the text should be italic */
    bool italic_;
    /** Whether the text could be shown in multilines */
    bool multiline_;
    /** Whether the text should be wrapped */
    bool wrap_;
    /** whether the cursor should be displayed */
    bool cursor_visible_;
    /** whether the edit control is readonly */
    bool readonly_;
    /**
     * Indicates if the content of the edit control has been modified
     * since last draw
     */
    bool content_modified_;

    /** Indicates if the selection region has been changed since last draw. */
    bool selection_changed_;

    /** Indicates if the cursor position has been moved since last draw. */
    bool cursor_moved_;

    /** Indicates if the canvas cache needs updating. */
    bool update_canvas_;

    /** The font family of the text */
    std::string font_family_;
    /** The font size of the text */
    double font_size_;

    cairo_font_options_t *font_options_;
    double font_dpi_;

    /** The text color of the edit control */
    Color _text_color;

    CairoGraphics::Alignment align_;
    CairoGraphics::VAlignment valign_;

    PangoRectangle strong_cursor_pos_;
    PangoRectangle weak_cursor_pos_;

    /**
    * Cursor index in layout, which shall be reset to -1 when resetting
    * layout or moving cursor so that it'll be recalculated.
    */
    int cursor_index_in_layout_;

    std::list<Rect> last_selection_region_;
    std::list<Rect> selection_region_;
    std::list<Rect> last_cursor_region_;
    std::list<Rect> cursor_region_;
    std::list<Rect> last_content_region_;
    std::list<Rect> content_region_;

    PangoWrapMode wrap_mode_;

    IMEContext *ime_;

  protected:
    bool text_input_mode_;
    bool key_nav_mode_;

    virtual bool InspectKeyEvent(unsigned int eventType,
      unsigned int keysym,
      const char* character);
};
}

#endif // TEXTENTRY_H
