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


#include "Nux.h"
#include "KeyboardHandler.h"
#include "Utils.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "NuxCore/Win32Dialogs/NWin32Clipboard.h"

namespace nux
{

  t_s32 BaseKeyboardHandler::s_jump_offset_at_borders = 60;
  t_s32 BaseKeyboardHandler::s_cursor_width = 4;

  BaseKeyboardHandler::BaseKeyboardHandler()
  {
    m_text_positionx            = 0;
    m_text_positiony            = 0;
    m_previous_cursor_position  = 0;
    m_clip_region               = Rect (0, 0, 100, 20);
    m_KeyType                   = eAlphaNumeric;
    m_first_visible_char        = 0;
    m_mouse_drag                = false;
    m_mouse_inside_text_area    = true;
    m_entering_focus            = false;
    m_Font                      = GetSysFont();

    m_caret = m_selection_start = 0;
    m_insert_mode = true;
  }

  BaseKeyboardHandler::~BaseKeyboardHandler()
  {
  }

  void BaseKeyboardHandler::DeleteSelectionText()
  {
    t_s32 nFirst = Min (m_caret, m_selection_start);
    t_s32 nLast = Max (m_caret, m_selection_start);
    // Update caret and selection
    PlaceCaret (nFirst);
    m_selection_start = m_caret;

    // Remove the characters
    for (t_s32 i = nFirst; i < nLast; ++i)
      m_textline.Erase (nFirst, 1);
  }

  void BaseKeyboardHandler::InsertChar (t_u32 character)
  {

  }

  void BaseKeyboardHandler::ClearText()
  {
    m_text_positionx = 0;
    m_text_positiony = 0;
    m_caret = 0;

    m_previous_cursor_position = 0;
    m_textline.Clear();
  }

  void BaseKeyboardHandler::PlaceCaret (t_u32 nCP)
  {
    nuxAssert ( (nCP >= 0) && (nCP <= (t_u32) m_textline.Length() ) );
    m_previous_cursor_position = m_caret;
    m_caret = nCP;
  }

  t_u32 BaseKeyboardHandler::NextWordPosition (t_u32 cp)
  {
    t_u32 num_char = (t_u32) m_textline.Length();

    if (cp == num_char)
      return cp;

    t_u32 c = cp + 1;

    while (c < num_char - 1)
    {
      if ( ( m_textline[c] == TCHAR (' ') ) && (m_textline[c+1] != TCHAR (' ') ) )
        return c + 1;

      c++;
    }

    return num_char;
  }

  t_u32 BaseKeyboardHandler::PrevWordPosition (t_u32 cp)
  {
    if (cp == 0)
      return 0;

    t_u32 c = cp - 1;

    while (c > 1)
    {
      if ( ( m_textline[c] != TCHAR (' ') ) && (m_textline[c-1] == TCHAR (' ') ) )
        return c;

      c--;
    }

    return 0;
  }

  void BaseKeyboardHandler::MouseDown (t_s32 x, t_s32 y)
  {
    ResolveCaretPosition (x, y);
    m_mouse_drag = true;
    m_mouse_inside_text_area = true;
    m_entering_focus = false;
  }

  void BaseKeyboardHandler::MouseUp (t_s32 x, t_s32 y)
  {
    m_mouse_drag = false;
    m_entering_focus = false;
  }

  void BaseKeyboardHandler::MouseDrag (t_s32 x, t_s32 y)
  {
    if (m_entering_focus)
      return;

    ResolveCaretPosition (x, y);
    m_mouse_inside_text_area = true;
  }

  void BaseKeyboardHandler::EnterFocus()
  {
    m_entering_focus = true;
  }

  void BaseKeyboardHandler::ResolveCaretPosition (t_s32 x, t_s32 y)
  {
    if (m_entering_focus)
      return;

    if (m_textline.Length() == 0)
      return;

    t_u32 StrLength = (t_u32) m_textline.Length();
    t_s32 total = m_text_positionx;
    t_u32 nCP = StrLength;

    for (t_u32 i = 0; i < StrLength; i++)
    {
      t_u32 cw0 = GetFont ()->GetCharWidth (m_textline[i]);
      t_u32 cw1 = GetFont ()->GetCharWidth (m_textline[i+1]);
      {
        if (total == x)
        {
          nCP = i;
          break;
        }
        else if (x < total + (t_s32) cw0 / 2)
        {
          nCP = i;
          break;
        }
        else if (x < total + (t_s32) cw1 / 2)
        {
          // I don't quite understand why we need this???
          nCP = i + 1;
          break;
        }
      }
      total += cw0;
    }

    PlaceCaret (nCP);

    if (!m_mouse_drag)
    {
      m_selection_start = m_caret;
    }
  }

  void BaseKeyboardHandler::CaretAutoScroll (t_s32 x, t_s32 y, Geometry geo)
  {
    if (m_entering_focus)
      return;

    if (m_textline.Length() == 0)
      return;

    t_s32 StrLength = (t_s32) m_textline.Length();

    //nuxDebugMsg(TEXT("[BaseKeyboardHandler::ResolveCaretPosition]"));
    if (x < geo.x)
    {
      if (m_mouse_inside_text_area)
      {
        while (m_caret && (GetFont ()->GetCharStringWidth (m_textline.GetTCharPtr(), m_caret) + m_text_positionx > 0) )
        {
          --m_caret;
          //nuxDebugMsg(TEXT("Group Add: %c"), m_textline[m_caret]);
        }

        m_mouse_inside_text_area = false;
      }
      else if (m_caret)
      {
        --m_caret;
        //nuxDebugMsg(TEXT("Add Char: %c"), m_textline[m_caret]);
      }
      else
      {
        m_caret = 0;
      }
    }
    else if (x > geo.x + geo.GetWidth() )
    {
      if (m_mouse_inside_text_area)
      {
        while ( (m_caret != StrLength) && (GetFont ()->GetCharStringWidth (m_textline.GetTCharPtr(), m_caret) + m_text_positionx < geo.GetWidth() ) )
        {
          ++m_caret;
          //nuxDebugMsg(TEXT("Group Add: %c"), m_textline[m_caret-1]);
        }

        m_mouse_inside_text_area = false;
      }
      else if (m_caret < StrLength)
      {
        ++m_caret;
        //nuxDebugMsg(TEXT("Group Add: %c"), m_textline[m_caret-1]);
      }
      else
      {
        m_caret = StrLength;
      }
    }

    AdjustCursorAndTextPosition();
  }

  long BaseKeyboardHandler::ProcessKey (
    unsigned long    eventType    /*event type*/,
    unsigned long    keysym       /*event keysym*/,
    unsigned long    state        /*event state*/,
    TCHAR      character    /*character*/,
    const Geometry &g)
  {
    //if(ievent.event_type != I_AsciiKey && ievent.event_type != I_KeyPress && ievent.event_type != I_KeyRelease)
    if (eventType != NUX_KEYDOWN)
      return 0;

    m_need_redraw = true;
    //unsigned key = ievent.ascii_code;
    unsigned key = 0;

    if (character != 0)
      key = character;

    long virtual_code = keysym;

    if (key != 0)
    {
      switch (m_KeyType)
      {
        case eDecimalNumber:
        {
          // Check if Key code is in ".0123456789";
          // Be careful because WM_KEYDOWN and WM_KEYUP send a key value
          // that gets here. If key pad 0 is press/released, WM_KEYDOWN/WM_KEYUP will send key = key_kp0.
          // It is WM_CHAR who send key = 0x30 = '0'. Therefore, do not use strchr(".0123456789", key) to test
          // if key is a digit character. When using strchr(".0123456789", key_kp0) we get a positive result
          // and the result is unexpected.
          if ( (key >= TCHAR ('0') && key <= TCHAR ('9') ) || (key == TCHAR ('.') ) || (key == TCHAR ('-') ) || (key == TCHAR ('+') ) )
          {
            if ( m_caret != m_selection_start )
            {
              DeleteSelectionText();
            }

            // '-' and '+' can only be at position 0 of the number
            if ( (key == TCHAR ('-') ) || (key == TCHAR ('+') ) )
            {
              if ( (m_caret == 0) && (m_textline[0] != TCHAR ('+') ) && (m_textline[0] != TCHAR ('-') ) )
              {
                // If we are in overwrite mode and there is already
                // a char at the caret's position, simply replace it.
                // Otherwise, we insert the char as normal.
                if ( !m_insert_mode && m_caret < (t_s32) m_textline.Length() )
                {
                  m_textline[m_caret] = key;
                  PlaceCaret (m_caret + 1 );
                  m_selection_start = m_caret;
                }
                else
                {
                  // Insert the char
                  if ( m_caret <= (t_s32) m_textline.Length() )
                  {
                    m_textline.Insert (m_caret, 1, key);
                    PlaceCaret (m_caret + 1 );
                    m_selection_start = m_caret;
                  }
                }
              }
            }
            else
            {
              // If we are in overwrite mode and there is already
              // a char at the caret's position, simply replace it.
              // Otherwise, we insert the char as normal.
              if ( !m_insert_mode && m_caret < (t_s32) m_textline.Length() )
              {
                m_textline[m_caret] = key;
                PlaceCaret (m_caret + 1 );
                m_selection_start = m_caret;
              }
              else
              {
                // Insert the char
                if ( m_caret <= (t_s32) m_textline.Length() )
                {
                  m_textline.Insert (m_caret, 1, key);
                  PlaceCaret (m_caret + 1 );
                  m_selection_start = m_caret;
                }
              }
            }
          }

          break;
        }
        case eIntegerNumber:
        {
          // Check if Key code is in "0123456789";
          if ( (key >= TCHAR ('0') && key <= TCHAR ('9') ) || (key == TCHAR ('-') ) || (key == TCHAR ('+') ) )
          {
            if ( m_caret != m_selection_start )
            {
              DeleteSelectionText();
            }

            // '-' and '+' can only be at position 0 of the number
            if ( (key == TCHAR ('-') ) || (key == TCHAR ('+') ) )
            {
              // If we are in overwrite mode and there is already
              // a char at the caret's position, simply replace it.
              // Otherwise, we insert the char as normal.
              if (!m_insert_mode && (m_caret < (t_s32) m_textline.Length() ) )
              {
                m_textline[m_caret] = key;
                PlaceCaret (m_caret + 1 );
                m_selection_start = m_caret;
              }
              else
              {
                // Insert the char
                if ( m_caret <= (t_s32) m_textline.Length() )
                {
                  m_textline.Insert (m_caret, 1, key);
                  PlaceCaret (m_caret + 1 );
                  m_selection_start = m_caret;
                }
              }
            }
            else
            {
              // If we are in overwrite mode and there is already
              // a char at the caret's position, simply replace it.
              // Otherwise, we insert the char as normal.
              if (!m_insert_mode && (m_caret < (t_s32) m_textline.Length() ) )
              {
                m_textline[m_caret] = key;
                PlaceCaret (m_caret + 1 );
                m_selection_start = m_caret;
              }
              else
              {
                // Insert the char
                if (m_caret <= (t_s32) m_textline.Length() )
                {
                  m_textline.Insert (m_caret, 1, key);
                  PlaceCaret (m_caret + 1 );
                  m_selection_start = m_caret;
                }
              }
            }
          }

          break;
        }

        case eHexadecimalNumber:
        {
          if ( (key >= TCHAR ('0') && key <= TCHAR ('9') ) || (key >= TCHAR ('a') && key <= TCHAR ('f') ) || (key >= TCHAR ('A') && key <= TCHAR ('F') ) || (key == TCHAR ('-') ) || (key == TCHAR ('+') ) )
            //if(strchr("0123456789abcdefABCDEF", key))
          {
            if (m_caret != m_selection_start)
            {
              DeleteSelectionText();
            }

            // If we are in overwrite mode and there is already
            // a char at the caret's position, simply replace it.
            // Otherwise, we insert the char as normal.
            if (!m_insert_mode && (m_caret < (t_s32) m_textline.Length() ) )
            {
              m_textline[m_caret] = key;
              PlaceCaret (m_caret + 1 );
              m_selection_start = m_caret;
            }
            else
            {
              // Insert the char
              if (m_caret <= (t_s32) m_textline.Length() )
              {
                m_textline.Insert (m_caret, 1, key);
                PlaceCaret (m_caret + 1 );
                m_selection_start = m_caret;
              }
            }

          }

          break;
        }

        case eBinaryNumber:
        {
          //if(strchr("01", key))
          if ( (key >= TCHAR ('0') && key <= TCHAR ('1') ) )
          {
            if ( m_caret != m_selection_start )
            {
              DeleteSelectionText();
            }

            // If we are in overwrite mode and there is already
            // a char at the caret's position, simply replace it.
            // Otherwise, we insert the char as normal.
            if (!m_insert_mode && (m_caret < (t_s32) m_textline.Length() ) )
            {
              m_textline[m_caret] = key;
              PlaceCaret (m_caret + 1 );
              m_selection_start = m_caret;
            }
            else
            {
              // Insert the char
              if (m_caret <= (t_s32) m_textline.Length() )
              {
                m_textline.Insert (m_caret, 1, key);
                PlaceCaret (m_caret + 1 );
                m_selection_start = m_caret;
              }
            }
          }

          break;
        }

        case eAlphaNumeric:
        default:
        {
          if (key > 0x1F && key < 0x7f)
          {
            if (m_caret != m_selection_start)
            {
              DeleteSelectionText();
            }

            // If we are in overwrite mode and there is already
            // a char at the caret's position, simply replace it.
            // Otherwise, we insert the char as normal.
            if (!m_insert_mode && (m_caret < (t_s32) m_textline.Length() ) )
            {
              m_textline[m_caret] = key;
              PlaceCaret (m_caret + 1 );
              m_selection_start = m_caret;
            }
            else
            {
              // Insert the char
              if (m_caret <= (t_s32) m_textline.Length() )
              {
                m_textline.Insert (m_caret, 1, key);
                PlaceCaret (m_caret + 1 );
                m_selection_start = m_caret;
              }
            }

          }

          break;
        }
      }
    }

    // CTRL+C
    if ( (virtual_code == NUX_VK_C) && (state & NUX_STATE_CTRL) )
    {
      NString s = GetSelectedText();
#if defined(NUX_OS_WINDOWS)
      inlCopyTextToClipboard (s.GetTCharPtr() );
#endif
    }

    // CTRL+V
    if ( (virtual_code == NUX_VK_V) && (state & NUX_STATE_CTRL) )
    {
#if defined(NUX_OS_WINDOWS)
      NString s = inlReadTextToClipboard();
#elif defined(NUX_OS_LINUX)
      NString s = "Paste not implemented yet";
#endif
      t_u32 start = GetTextSelectionStart();
      t_u32 end = GetTextSelectionEnd();
      m_textline.Replace (start, end - start, s.m_string);

      m_caret = start + (t_u32) s.Length();
      UnselectAllText();
    }

    if ( (virtual_code == NUX_VK_ESCAPE) )
    {
      // If Text is selected, Unselect
      UnselectAllText();
    }

    if ( (virtual_code == NUX_VK_ENTER) )
    {
      SelectAllText();
    }

    if (virtual_code == NUX_VK_BACKSPACE)
    {
      if ( m_caret != m_selection_start )
      {
        DeleteSelectionText();
      }
      else
      {
        // Deleting one character
        if ( m_caret > 0 )
        {
          m_textline.Erase (m_caret - 1, 1);
          PlaceCaret (m_caret - 1 );

          m_selection_start = m_caret;
        }
      }
    }
    else if (virtual_code == NUX_VK_DELETE)
    {
      // Check if there is a text selection.
      if ( m_caret != m_selection_start )
      {
        DeleteSelectionText();
      }
      else
      {
        // Deleting one character
        if (m_caret < (t_s32) m_textline.Length() )
        {
          m_textline.Erase (m_caret, 1);
        }
      }
    }
    else if (virtual_code == NUX_VK_LEFT)
    {
      if (IsTextSelected() && ( (state & NUX_STATE_SHIFT) == 0) )
      {
        //m_caret = Min(m_caret, m_selection_start);
        if (m_caret)
          --m_caret;

        UnselectAllText();
      }
      else
      {
        if ( state & NUX_STATE_CTRL )
        {
          // Control is down. Move the caret to a new item
          // instead of a character.
          m_caret = PrevWordPosition ( m_caret);
          PlaceCaret (m_caret );
        }
        else if ( m_caret > 0 )
          PlaceCaret (m_caret - 1 );

        if ( (state & NUX_STATE_SHIFT) == 0)
        {
          // Shift is not down. Update selection
          // start along with the caret.
          m_selection_start = m_caret;
        }
      }
    }
    else if (virtual_code == NUX_VK_RIGHT)
    {
      if (IsTextSelected() && ( (state & NUX_STATE_SHIFT) == 0) )
      {
        m_caret = Max (m_caret, m_selection_start);
        UnselectAllText();
      }
      else
      {
        if ( state & NUX_STATE_CTRL)
        {
          // Control is down. Move the caret to a new item
          // instead of a character.
          m_caret = NextWordPosition ( m_caret);
          PlaceCaret (m_caret );
        }
        else if (m_caret < (t_s32) m_textline.Length() )
          PlaceCaret (m_caret + 1 );

        if ( (state & NUX_STATE_SHIFT) == 0)
        {
          // Shift is not down. Update selection
          // start along with the caret.
          m_selection_start = m_caret;
        }
      }
    }
    else if (virtual_code == NUX_VK_HOME)
    {
      if ( (state & NUX_STATE_SHIFT) == 0)
      {
        PlaceCaret (0);
        // Shift is not down. Update selection
        // start along with the caret.
        m_selection_start = m_caret;
      }
      else
      {
        PlaceCaret (0 );
      }
    }
    else if (virtual_code == NUX_VK_END)
    {
      if ( (state & NUX_STATE_SHIFT) == 0)
      {
        PlaceCaret ( (t_u32) m_textline.Length() );
        // Shift is not down. Update selection
        // start along with the caret.
        m_selection_start = m_caret;
      }
      else
      {
        PlaceCaret ( (t_u32) m_textline.Length() );
      }
    }
    else if (virtual_code == NUX_VK_ESCAPE)
    {
      return virtual_code;
    }
    else if ( (virtual_code == NUX_VK_ENTER))
    {
      return virtual_code;
    }
    else
    {
      m_need_redraw = false;
    }

    if (virtual_code == NUX_VK_HOME)
    {
      m_text_positionx = 0;
    }
    else if (virtual_code == NUX_VK_END)
    {
      t_u32 str_width = GetFont ()->GetStringWidth (m_textline.GetTCharPtr() );

      if (str_width + s_cursor_width > (t_u32) m_clip_region.GetWidth() )
        m_text_positionx = m_clip_region.GetWidth() - (str_width + s_cursor_width);
      else
        m_text_positionx = 0;
    }
    else if (m_textline.Length() != 0)
    {
      AdjustCursorAndTextPosition();
      m_need_redraw = true;
    }
    else if (m_textline.Length() == 0)
    {
      ClearText();
      m_need_redraw = true;
    }


    return virtual_code;
  }

  void BaseKeyboardHandler::AdjustCursorAndTextPosition()
  {
    t_s32 str_width = GetFont ()->GetStringWidth (m_textline.GetTCharPtr() );
    NString temp0;

    if (m_caret > 0)
      temp0 = m_textline.GetSubString (0, m_caret - 1).GetTStringRef();
    else
      temp0 = TEXT ("");

    //      0          1         2
    //      01234567|8901234567890123456789
    //      abcdefgh|ijklmn
    //
    //      Caret pos = 8
    //      temp0 = "abcdefg"
    //      temp1 = "abcdefgh"
    //      temp2 = "abcdefghi"

    NString temp1 = m_textline.GetSubString (0, m_caret).GetTStringRef();
    NString temp2 = m_textline.GetSubString (0, m_caret + 1).GetTStringRef();
    t_s32 str_width0 = GetFont ()->GetStringWidth (temp0);
    t_s32 str_width1 = GetFont ()->GetStringWidth (temp1);
    t_s32 str_width2 = GetFont ()->GetStringWidth (temp2);


    if ( (m_text_positionx + str_width1 + s_cursor_width) > m_clip_region.GetWidth() )
    {
      // Hitting the end of the text box
      if (m_caret == (t_s32) m_textline.Length() )
      {
        m_text_positionx = - (str_width + (t_s32) s_cursor_width - m_clip_region.GetWidth() );
      }
      else
      {
        t_s32 PreviousCharWidth = str_width1 - str_width0;
        t_s32 Offset = Min<t_s32> (s_jump_offset_at_borders, str_width + s_cursor_width - str_width1);

        while (Offset > m_clip_region.GetWidth() )
        {
          Offset /= 2;
        }

        if (Offset < PreviousCharWidth)
          Offset = PreviousCharWidth;

        m_text_positionx -= Offset;

        if (m_text_positionx + str_width + s_cursor_width < (t_s32) m_clip_region.GetWidth() )
        {
          m_text_positionx = - (str_width + (t_s32) s_cursor_width - m_clip_region.GetWidth() );
        }
      }
    }
    else if ( (m_text_positionx + str_width1) <= 0)
    {
      // Hitting the start of the text box
      if (m_caret == 0)
      {
        m_text_positionx = 0;
      }
      else
      {
        t_s32 NextCharWidth = str_width2 - str_width1;
        t_s32 Offset = Min<t_s32> (s_jump_offset_at_borders, str_width1);

        while (Offset > m_clip_region.GetWidth() )
        {
          Offset /= 2;
        }

        if (Offset < NextCharWidth)
          Offset = NextCharWidth;

        if (Offset > str_width1)
        {
          m_text_positionx = 0;
          return;
        }

        m_text_positionx += Offset;

        if (m_text_positionx > 0)
        {
          m_text_positionx = 0;
        }
      }
    }
  }

  void BaseKeyboardHandler::MoveCursorAtStart()
  {
    m_previous_cursor_position = m_caret;
    m_caret  = 0;
  }
  void BaseKeyboardHandler::MoveCursorAtEnd()
  {
    t_u32 StrLength = ( t_u32) m_textline.Length();
    m_previous_cursor_position = m_caret;
    m_caret = StrLength;
  }

  void BaseKeyboardHandler::SetKeyEntryType (BaseKeyboardHandler::eKeyEntryType keytype)
  {
    m_KeyType = keytype;
  }

  BaseKeyboardHandler::eKeyEntryType BaseKeyboardHandler::GetKeyEntryType()
  {
    return m_KeyType;
  }

  void BaseKeyboardHandler::SetText (const TCHAR *str)
  {
    m_textline = str;
    // Every time we set the text, we reposition the cursor at the beginning of the text,
    // and the text position is set to zero with regard to the start of the geometry area.
    MoveCursorAtStart();
    m_selection_start = Min<t_s32> (m_selection_start, (t_s32) StringLength (str) );
    m_caret = Min<t_s32> (m_caret, (t_s32) StringLength (str) );

    if (m_caret < m_selection_start)
      m_selection_start = Max<t_s32> (m_selection_start, (t_s32) StringLength (str) );
    else if (m_caret > m_selection_start)
      m_caret = Max<t_s32> (m_caret, (t_s32) StringLength (str) );

    m_text_positionx = 0;
  }

  void BaseKeyboardHandler::SetText (const tstring &s)
  {
    SetText (s.c_str() );
  }

  void BaseKeyboardHandler::SetText (const NString &s)
  {
    SetText (s.GetTCharPtr() );
  }

  void BaseKeyboardHandler::SetClipRegion (const Geometry &g)
  {
    m_clip_region = g;
  }

  void BaseKeyboardHandler::GetTextSelection (t_s32 *start, t_s32 *end) const
  {
    *start = Min (m_selection_start, m_caret);
    *end = Max (m_selection_start, m_caret);
  }

  t_s32 BaseKeyboardHandler::GetTextSelectionStart() const
  {
    return Min (m_selection_start, m_caret);
  }

  t_s32 BaseKeyboardHandler::GetTextSelectionEnd() const
  {
    return Max (m_selection_start, m_caret);
  }

  void BaseKeyboardHandler::SelectAllText()
  {
    m_selection_start = 0;
    m_caret = (t_u32) m_textline.Length();
    AdjustCursorAndTextPosition();
  }

  void BaseKeyboardHandler::UnselectAllText()
  {
    m_selection_start = m_caret;
  }

  NString BaseKeyboardHandler::GetSelectedText() const
  {
    if (m_selection_start == m_caret)
    {
      return NString (TEXT ("") );
    }
    else
    {
      NString s = m_textline.GetSubString (GetTextSelectionStart(), GetTextSelectionEnd() - GetTextSelectionStart() ).GetTStringRef();
      return s;
    }
  }

  bool BaseKeyboardHandler::IsTextSelected()
  {
    if (m_caret != m_selection_start)
      return true;

    return false;
  }

  void BaseKeyboardHandler::SetFont (ObjectPtr<FontTexture> Font)
  {
    m_Font = Font;
  }

  ObjectPtr<FontTexture> BaseKeyboardHandler::GetFont () const
  {
    return m_Font;
  }

}
