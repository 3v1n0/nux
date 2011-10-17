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

    NString GetTextLine() const
    {
      return m_textline;
    }
    t_u32 GetLength() const
    {
      return (t_u32) m_textline.Length();
    }

    t_u32 GetCursorPosition() const
    {
      return m_caret;
    }
    bool QueueDraw() const
    {
      return m_need_redraw;
    }
    t_s32 GetPositionX() const
    {
      return m_text_positionx;
    }
    t_s32 GetPositionY() const
    {
      return m_text_positiony;
    }

    void SetText(const char *str);
    void SetText(const tstring &s);
    void SetText(const NString &s);

    void ClearText();
    void PlaceCaret(t_u32 cp);

    void MoveCursorAtStart();
    void MoveCursorAtEnd();

    void SetKeyEntryType(eKeyEntryType keytype);
    eKeyEntryType GetKeyEntryType();

    void SetClipRegion(const Geometry &g);
    void GetTextSelection(t_s32 *start, t_s32 *end) const;
    t_s32 GetTextSelectionStart() const;
    t_s32 GetTextSelectionEnd() const;
    NString GetSelectedText() const;
    void SelectAllText();
    void UnselectAllText();
    bool IsTextSelected();

    void SetPrefix(NString); // 0x865, 25 rad, 25 degree...
    void SetPostfix(NString);

    void EnterFocus();

    t_u32 NextWordPosition(t_u32 cp);
    t_u32 PrevWordPosition(t_u32 cp);
    void ResolveCaretPosition(t_s32 x, t_s32 y);
    void CaretAutoScroll(t_s32 x, t_s32 y, Geometry geo);
    void MouseDown(t_s32 x, t_s32 y);
    void MouseUp(t_s32 x, t_s32 y);
    void MouseDrag(t_s32 x, t_s32 y);
    //bool IsMouseDrag();

    void SetFont(ObjectPtr<FontTexture> Font);
    ObjectPtr<FontTexture> GetFont() const;

  protected:
    ObjectPtr<FontTexture> m_Font;
    NString m_textline;
    t_s32 m_previous_cursor_position;
    bool m_need_redraw;
    t_s32 m_text_positionx;
    t_s32 m_text_positiony;
    Geometry m_clip_region;

    eKeyEntryType m_KeyType;

    t_s32   m_caret;           //!< Caret position, in characters
    bool    m_insert_mode;     //!< If true, control is in insert mode. Else, overwrite mode.
    t_s32   m_selection_start; //!< Starting position of the selection. The caret marks the end.
    t_s32   m_first_visible_char;   //!< First visible character in the edit control

    void DeleteSelectionText();
    void InsertChar(t_u32 character);
    void AdjustCursorAndTextPosition();


    bool m_entering_focus;
    bool m_mouse_drag;
    // m_mouse_inside_text_area Detects the fast transition of the mouse from inside the text area to outside
    bool m_mouse_inside_text_area;

    /*!
        When the caret reaches the left or right border of the control and there are more characters to reveals,
        the caret jumps back in the opposite direction by a number of pixel at the same moment when new characters are revealed.
    */
    static t_s32 s_jump_offset_at_borders;
    static t_s32 s_cursor_width;
  };

}

#endif // KEYBOARDHANDLER_H
