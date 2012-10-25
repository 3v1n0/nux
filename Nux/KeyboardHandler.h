/*
 * Copyright 2010 Inalogic® Inc.
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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <string>
#include "Utils.h"
#include "NuxGraphics/FontTexture.h"
#if defined(NUX_OS_WINDOWS)
#include "NuxGraphics/Events.h"
#elif defined(NUX_OS_LINUX)
#include "NuxGraphics/Events.h"
#endif
#include "NuxCore/Color.h"

namespace nux
{

  class GraphicsEngine;

  class BaseKeyboardHandler
  {
  public:
    enum eKeyEntryType
    {
      eAlphaNumeric,
      eDecimalNumber,
      eIntegerNumber,
      eHexadecimalNumber,
      eBinaryNumber
    };

    BaseKeyboardHandler();
    virtual ~BaseKeyboardHandler();


    virtual long ProcessKey(
      unsigned long    eventType    /*event type*/,
      unsigned long    keysym       /*event keysym*/,
      unsigned long    state        /*event state*/,
      char      character    /*character*/,
      const Geometry &g);

    std::string GetTextLine() const
    {
      return m_textline;
    }
    unsigned int GetLength() const
    {
      return (unsigned int) m_textline.length();
    }

    unsigned int GetCursorPosition() const
    {
      return m_caret;
    }
    bool QueueDraw() const
    {
      return m_need_redraw;
    }
    int GetPositionX() const
    {
      return m_text_positionx;
    }
    int GetPositionY() const
    {
      return m_text_positiony;
    }

    void SetText(const char *str);
    void SetText(const std::string &s);

    void ClearText();
    void PlaceCaret(unsigned int cp);

    void MoveCursorAtStart();
    void MoveCursorAtEnd();

    void SetKeyEntryType(eKeyEntryType keytype);
    eKeyEntryType GetKeyEntryType();

    void SetClipRegion(const Geometry &g);
    void GetTextSelection(int *start, int *end) const;
    int GetTextSelectionStart() const;
    int GetTextSelectionEnd() const;
    std::string GetSelectedText() const;
    void SelectAllText();
    void UnselectAllText();
    bool IsTextSelected();

    void SetPrefix(std::string); // 0x865, 25 rad, 25 degree...
    void SetPostfix(std::string);

    void EnterFocus();

    unsigned int NextWordPosition(unsigned int cp);
    unsigned int PrevWordPosition(unsigned int cp);
    void ResolveCaretPosition(int x, int y);
    void CaretAutoScroll(int x, int y, Geometry geo);
    void MouseDown(int x, int y);
    void MouseUp(int x, int y);
    void MouseDrag(int x, int y);
    //bool IsMouseDrag();

    void SetFont(ObjectPtr<FontTexture> Font);
    ObjectPtr<FontTexture> GetFont() const;

  protected:
    ObjectPtr<FontTexture> m_Font;
    std::string m_textline;
    int m_previous_cursor_position;
    bool m_need_redraw;
    int m_text_positionx;
    int m_text_positiony;
    Geometry m_clip_region;

    eKeyEntryType m_KeyType;

    int   m_caret;           //!< Caret position, in characters
    bool    m_insert_mode;     //!< If true, control is in insert mode. Else, overwrite mode.
    int   m_selection_start; //!< Starting position of the selection. The caret marks the end.
    int   m_first_visible_char;   //!< First visible character in the edit control

    void DeleteSelectionText();
    void InsertChar(unsigned int character);
    void AdjustCursorAndTextPosition();


    bool m_entering_focus;
    bool m_mouse_drag;
    // m_mouse_inside_text_area Detects the fast transition of the mouse from inside the text area to outside
    bool m_mouse_inside_text_area;

    /*!
        When the caret reaches the left or right border of the control and there are more characters to reveals,
        the caret jumps back in the opposite direction by a number of pixel at the same moment when new characters are revealed.
    */
    static int s_jump_offset_at_borders;
    static int s_cursor_width;
  };

}

#endif // KEYBOARDHANDLER_H
