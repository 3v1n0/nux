#ifndef TEXTENTRY_H
#define TEXTENTRY_H

// Heavily inspired from google gadget code
/*
 * Copyright 2011-2013 Canonical Ltd.
 * Copyright 2010 Inalogic Inc.
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
 * Copyright 2008 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cairo/cairo.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"
#include "NuxGraphics/CairoGraphics.h"

namespace nux
{
  class CairoGraphics;
  class IBusIMEContext;

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

    virtual Style GetStyle() const {return style_;}
    virtual Weight GetWeight() const {return weight_;}
    virtual double GetPointSize() const {return size_;}

    virtual void Destroy() {delete this;}

    const PangoFontDescription *GetFontDescription() const {return font_;}

  private:
    PangoFontDescription *font_;
    double size_;
    Style style_;
    Weight weight_;
  };

  class TextEntry: public View
  {
    NUX_DECLARE_OBJECT_TYPE(TextEntry, View);
  public:
    TextEntry(const char* text, NUX_FILE_LINE_PROTO);
    ~TextEntry();

    Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);
    virtual void Draw(GraphicsEngine& graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine& graphics_engine, bool force_draw);

    void PreLayoutManagement();
    long PostLayoutManagement(long layoutResult);

    //  Receivers

    void RecvMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
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

    void FocusInx();
    void FocusOutx();

    //! Text changed signal
    /*!
        This signal is emitted when the text has changed.
    */
    sigc::signal <void, TextEntry*> text_changed;
    sigc::signal <void> activated;
    sigc::signal <void, int> cursor_moved;

    void SetText(const char* text);
    std::string const& GetText() const;
    std::string GetTextSelection() const;

    void SetCompletion(const char* text); // Should use std::string, does not for consistancy
    std::string const& GetCompletion() const;

    void SetCompletionColor(const Color& color);
    Color const& GetCompletionColor() const;

    void SetTextColor(const Color& color);
    Color GetTextColor() const;
    void SetFontFamily(const char* font);
    void SetFontSize(double font_size);
    void SetFontOptions(const cairo_font_options_t* options);

    /** Select text between start and end. */
    void Select(int start, int end);
    /** Select all text */
    void SelectAll();

    CairoGraphics::Alignment GetAlign() const;
    void SetAlign(CairoGraphics::Alignment align);

    bool im_active();
    bool im_running();

    void MoveCursorToLineStart();
    void MoveCursorToLineEnd();

    void SetToggleCursorVisibilityOnKeyFocus(bool b);
    bool GetToggleCursorVisibilityOnKeyFocus() const;

    /*!
        When the text entry is in single line mode, the keyboard arrow up/down may be used
        to navigate to another view. If the parameter of this function is set to true then
        the text entry returns false on NUX_VK_UP in InspectKeyEvent(). This allows the parent of the
        text entry to look for another view that can receive the keyboard focus. The default value is true.\n
        In multi-line mode, keyboard arrow up/down are used to navigate in the text.

        @param b True to allow the key navigation to move away from the text entry on NUX_VK_UP.
    */
    void SetLoseKeyFocusOnKeyNavDirectionUp(bool b);

    bool GetLoseKeyFocusOnKeyNavDirectionUp() const;

    /*!
        When the text entry is in single line mode, the keyboard arrow up/down may be used
        to navigate to another view. If the parameter of this function is set to true then
        the text entry returns false on NUX_VK_DOWN in InspectKeyEvent(). This allows the parent of the
        text entry to look for another view that can receive the keyboard focus.  The default value is true.\n
        In multi-line mode, keyboard arrow up/down are used to navigate in the text.

        @param b True to allow the key navigation to move away from the text entry on NUX_VK_DOWN.
    */
    void SetLoseKeyFocusOnKeyNavDirectionDown(bool b);

    bool GetLoseKeyFocusOnKeyNavDirectionDown() const;

    /*!
        Return True if the text has been modified after the text entry has received the keyboard focus.\n
        If the text entry already has characters typed in and it gets the keyboard focus, this function return false
        while the text has not been modified. The text can be modified either by typing new characters or addind text
        through TextEntry::SetText, TextEntry::EnterText, TextEntry::DeleteText.
        If this text entry does not have the keyboard focus, this function returns false.

        @return True after the text entry has received the keyboard focus and text has been typed in.
    */
    bool IsInTextInputMode() const;

    /*!
        Insert text at current caret position.
    */
    void EnterText(const char* str);

    /*!
        Delete text in a specified range, in number of characters.
    */
    void DeleteText(int start, int end);

    void SetPasswordMode(bool visible);
    void SetVisibility(bool visible);
    void SetPasswordChar(const char* c);
    bool PasswordMode() const;
    std::string GetPasswordChar();

    void PreeditStarted();
    void UpdatePreedit(std::string const& preedit, int cursor);
    void UpdatePreeditAttribs(PangoAttrList* list);
    void ClearPreedit();

  protected:
    bool _block_focus; // used to selectively ignore focus keyevents

    virtual void GeometryChanged(bool position_has_changed, bool size_has_changed);

    /**
     * Enum used to specify different motion types.
     */
    enum MovementStep {
      VISUALLY,
      WORDS,
      DISPLAY_LINES,
      DISPLAY_LINE_ENDS,
      PAGES,
      BUFFER
    };

    /**
     * Enum used for the search state of the compose list
     */
    enum SearchState {
      NO_MATCH,
      PARTIAL,
      MATCH
    };

    /** Checks for possible composition sequences */
    bool IsInitialCompositionKeySym(unsigned long keysym) const;
    bool IsInCompositionMode() const;
    bool HandleComposition(unsigned long keysym);
    SearchState GetCompositionForList(std::vector<unsigned long> const& input, std::string& composition);

    void QueueTextDraw();
    /** Remove the cached layout. */
    void ResetLayout();
    /**
     * Create pango layout on-demand. If the layout is not changed, return the
     * cached one.
     */
    PangoLayout* EnsureLayout();
    /** Create a new layout containning current edit content */
    PangoLayout* CreateLayout();
    /** Create cairo canvas on-demand. */
    CairoGraphics* EnsureCanvas();
    /** Adjust the scroll information */
    void AdjustScroll();
    /**
     * Send out a request to refresh all informations of the edit control
     * and queue a draw request.
     * If @c relayout is true then the layout will be regenerated.
     * */
    void QueueRefresh(bool relayout, bool adjust_scroll);
    /** Reset the input method context */
    void ResetImContext();
    /** Reset preedit text */
    void ResetPreedit();
    /** Send out a request to blink the cursor if necessary */
    void QueueCursorBlink();
    static bool CursorBlinkCallback(TextEntry* data);

    void ShowCursor();
    void HideCursor();

    /** Draw the Cursor to the canvas */
    void DrawCursor(CairoGraphics* canvas);
    /** Draw the text to the canvas */
    virtual void DrawText(CairoGraphics* canvas);

    void GetCursorRects(Rect* strong, Rect* weak);

    void UpdateCursorRegion();

    void UpdateSelectionRegion();

    /** Move cursor */
    void MoveCursor(MovementStep step, int count, bool extend_selection);
    /** Move cursor visually, meaning left or right */
    int MoveVisually(int current_pos, int count);
    /** Move cursor in words */
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
    /** Get the offset range that is currently selected,in number of characters.*/
    bool GetSelectionBounds(int* start, int* end) const;
    /** Set the offest range that should be selected, in number of characters. */
    void SetSelectionBounds(int selection_bound, int cursor);

    /** Convert index in text_ into index in layout text. */
    int TextIndexToLayoutIndex(int text_index, bool consider_preedit_cursor);

    /** Convert index in layout text into index in text_. */
    int LayoutIndexToTextIndex(int layout_index);

    /** Get char length at index, in number of bytes. */
    int GetCharLength(int index);

    /** Get previous char length before index, in number of bytes. */
    int GetPrevCharLength(int index);

    /** Select the current word under cursor */
    void SelectWord();
    /** Select the current display line under cursor */
    void SelectLine();
    /** Delete the text that is currently selected */
    void DeleteSelection();

    /** Cut the current selected text to the clipboard */
    virtual void CutClipboard();
    /** Copy the current selected text to the clipboard */
    virtual void CopyClipboard();
    /** Paste the text in the clipboard to current offset */
    virtual void PasteClipboard();
#if defined(NUX_OS_LINUX)
    /** Paste the text in the primary clipboard to current offset */
    virtual void PastePrimaryClipboard();
#endif
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
    void GetCursorLocationInLayout(int* strong_x, int* strong_y, int* strong_height,
                                   int* weak_x, int* weak_y, int* weak_height);

    /** The CairoCanvas which hold cairo_t inside */
    CairoGraphics* canvas_;

    /** The cached Pango Layout */
    PangoLayout* cached_layout_;

    /** The text content of the edit control */
    std::string text_;
    /** The preedit text of the edit control */
    std::string preedit_;
    /** Attribute list of the preedit text */
    PangoAttrList* preedit_attrs_;

    /**
     *  The character that should be displayed in invisible mode.
     *  If this is empty, then the edit control is visible
     */
    std::string password_char_;

    /** The completion string */
    std::string completion_;

    /** The completion colour */
    Color completion_color_;

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
    /** Timer id of cursor blink callback */
    int cursor_blink_timer_;
    /**
     * Indicates the status of cursor blinking,
     * 0 means hide cursor
     * otherwise means show cursor.
     * The maximum value would be 2, and decrased by one in each cursor blink
     * callback, then there would be 2/3 visible time and 1/3 invisible time.
     */
    int cursor_blink_status_;

    /*! Whether the text is visible, or its characters are masked by a default char (password mode). */
    bool visible_;
    /** Whether the edit control is focused */
    bool focused_;
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

    cairo_font_options_t* font_options_;
    double font_dpi_;

    /** The text color of the edit control */
    Color _text_color;

    CairoGraphics::Alignment align_;

#if defined(USE_X11)
    Cursor caret_cursor_;
#endif

    std::list<Rect> last_selection_region_;
    std::list<Rect> selection_region_;
    std::list<Rect> last_cursor_region_;
    std::list<Rect> cursor_region_;

#if defined(USE_X11)
    IBusIMEContext* ime_;
    friend class IBusIMEContext;
#endif

    bool ime_active_;

    bool text_input_mode_;    //!< Transient state of the TextEntry. \sa IsInTextInputMode.
    bool key_nav_mode_;

    bool lose_key_focus_on_key_nav_direction_up_;
    bool lose_key_focus_on_key_nav_direction_down_;
    bool cursor_visible_on_key_focus_only_;

    std::vector<unsigned long> composition_list_;

    virtual bool InspectKeyEvent(Event const& event);
    virtual bool InspectKeyEvent(unsigned int eventType, unsigned int keysym, const char* character);
};
}

#endif // TEXTENTRY_H
