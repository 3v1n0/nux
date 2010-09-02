/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "Nux.h"
#include "KeyboardHandler.h"
#include "Utils.h"
#include "NuxGraphics/OpenGLEngine.h"
#include "NuxCore/Win32Dialogs/NWin32Clipboard.h"

NAMESPACE_BEGIN_GUI

t_u32 BaseKeyboardHandler::sJumpOffsetAtBorders = 60;
t_u32 BaseKeyboardHandler::sCursorWidth = 2;

BaseKeyboardHandler::BaseKeyboardHandler()
:   m_text_positionx(0)
,   m_text_positiony(0)
,   m_previous_cursor_position(0)
,   m_clip_region(0, 0, 100, 20)
,   m_KeyType(eAlphaNumeric)
,   m_nFirstVisible(0)
,   m_bMouseDrag(false)
,   m_bMouseInsideTextArea(true)
,   m_bEnteringFocus(false)
{
    m_Caret = m_nSelStart = 0;
    m_bInsertMode = true;
}

BaseKeyboardHandler::~BaseKeyboardHandler()
{
}

void BaseKeyboardHandler::DeleteSelectionText()
{
    t_s32 nFirst = Min( m_Caret, m_nSelStart );
    t_s32 nLast = Max( m_Caret, m_nSelStart );
    // Update caret and selection
    PlaceCaret(nFirst );
    m_nSelStart = m_Caret;
    // Remove the characters
    for( t_s32 i = nFirst; i < nLast; ++i )
        m_textline.Erase( nFirst, 1 );
}

void BaseKeyboardHandler::InsertChar(t_u32 character)
{

}

void BaseKeyboardHandler::ClearText()
{
    m_text_positionx = 0;
    m_text_positiony = 0;
    m_Caret = 0;

    m_previous_cursor_position = 0;
    m_textline.Clear();
}

void BaseKeyboardHandler::PlaceCaret(t_u32 nCP)
{
    nuxAssert( (nCP >= 0) && (nCP <= (t_u32)m_textline.Length()) );
    m_previous_cursor_position = m_Caret;
    m_Caret = nCP;

//    // Obtain the X offset of the character.
//    t_s32 nX1st, nX, nX2;
//    CursorPosToX(m_nFirstVisible, FALSE, &nX1st );  // 1st visible char
//    CursorPosToX(nCP, FALSE, &nX );  // LEAD
//    // If nCP is the NULL terminator, get the leading edge instead of trailing.
//    if( nCP == m_textline.Length() )
//        nX2 = nX;
//    else
//        CursorPosToX(nCP, TRUE, &nX2 );  // TRAIL
//
//    // If the left edge of the char is smaller than the left edge of the 1st visible char,
//    // we need to scroll left until this char is visible.
//    if( nX < nX1st )
//    {
//        // Simply make the first visible character the char at the new caret position.
//        m_nFirstVisible = nCP;
//    }
//    else
//    {
//        // If the right of the character is bigger than the offset of the control's
//        // right edge, we need to scroll right to this character.
//        if( nX2 + cursor_width > nX1st + m_clip_region.GetWidth() )
//        {
//            // Compute the X of the new left-most pixel
//            t_s32 nXNewLeft = nX2 - m_clip_region.GetWidth() ;
//
//            // Compute the char position of this character
//            t_s32 nCPNew1st, nNewTrail;
//            XToCursorPosition(nXNewLeft, 0 /*m_nFirstVisible*/, &nCPNew1st, &nNewTrail );
//
//            // If this coordinate is not on a character border,
//            // start from the next character so that the caret
//            // position does not fall outside the text rectangle.
//            t_s32 nXNew1st;
//            CursorPosToX(nCPNew1st, FALSE, &nXNew1st );
//            if( nXNew1st < nXNewLeft )
//                ++nCPNew1st;
//
//            m_nFirstVisible = nCPNew1st;
//        }
//    }
}

t_u32 BaseKeyboardHandler::NextWordPosition(t_u32 cp)
{
    t_u32 num_char = (t_u32)m_textline.Length();
    if(cp == num_char)
        return cp;

    t_u32 c = cp+1;
    while(c < num_char-1)
    {
        if( ( m_textline[c] == TCHAR(' ') ) && (m_textline[c+1] != TCHAR(' ') ) )
            return c+1;
        c++;
    }
    return num_char;
}

t_u32 BaseKeyboardHandler::PrevWordPosition(t_u32 cp)
{
    t_u32 num_char = (t_u32)m_textline.Length();
    if(cp == 0)
        return 0;

    t_u32 c = cp-1;
    while(c > 1)
    {
        if( ( m_textline[c] != TCHAR(' ') ) && (m_textline[c-1] == TCHAR(' ') ) )
            return c;
        c--;
    }
    return 0;
}

void BaseKeyboardHandler::MouseDown(t_s32 x, t_s32 y)
{
    ResolveCaretPosition(x, y);
    m_bMouseDrag = true;
    m_bMouseInsideTextArea = true;
    m_bEnteringFocus = false;
}

void BaseKeyboardHandler::MouseUp(t_s32 x, t_s32 y)
{
    m_bMouseDrag = false;
    m_bEnteringFocus = false;
}

void BaseKeyboardHandler::MouseDrag(t_s32 x, t_s32 y)
{
    if(m_bEnteringFocus)
        return;
    ResolveCaretPosition(x, y);
    m_bMouseInsideTextArea = true;
}

void BaseKeyboardHandler::EnterFocus()
{
    m_bEnteringFocus = true;
}

void BaseKeyboardHandler::ResolveCaretPosition(t_s32 x, t_s32 y)
{
    if(m_bEnteringFocus)
        return;

    if(m_textline.Length() == 0)
        return;

    t_u32 StrLength = (t_u32)m_textline.Length();
    t_s32 total = m_text_positionx;
    t_u32 nCP = StrLength;
    for(t_u32 i = 0; i < StrLength; i++)
    {
        t_u32 cw0 = GetFont()->GetCharWidth(m_textline[i]);
        t_u32 cw1 = GetFont()->GetCharWidth(m_textline[i+1]);
        {
            if(total == x)
            {
                nCP = i;
                break;
            }
            else if(x < total + (t_s32)cw0/2)
            {
                nCP = i;
                break;
            }
            else if(x < total + (t_s32)cw1/2)
            {
                // I don't quite understand why we need this???
                nCP = i+1;
                break;
            }
        }
        total += cw0;
    }

    PlaceCaret(nCP);
    if(!m_bMouseDrag)
    {
        m_nSelStart = m_Caret;
    }
}

void BaseKeyboardHandler::CaretAutoScroll(t_s32 x, t_s32 y, Geometry geo)
{
    if(m_bEnteringFocus)
        return;

    if(m_textline.Length() == 0)
        return;
    t_u32 StrLength = (t_u32)m_textline.Length();

    //nuxDebugMsg(TEXT("[BaseKeyboardHandler::ResolveCaretPosition]"));
    if(x < geo.x)
    {
        if(m_bMouseInsideTextArea)
        {
            while(m_Caret && (GetFont()->GetCharStringWidth(m_textline.GetTCharPtr(), m_Caret) + m_text_positionx > 0))
            {
                --m_Caret;
                //nuxDebugMsg(TEXT("Group Add: %c"), m_textline[m_Caret]);
            }
            m_bMouseInsideTextArea = false;
        }
        else if(m_Caret)
        {
            --m_Caret;
            //nuxDebugMsg(TEXT("Add Char: %c"), m_textline[m_Caret]);            
        }
        else
        {
            m_Caret = 0;
        }
    }
    else if(x > geo.x + geo.GetWidth())
    {
        if(m_bMouseInsideTextArea)
        {
            while((m_Caret != StrLength) && (GetFont()->GetCharStringWidth(m_textline.GetTCharPtr(), m_Caret) + m_text_positionx < geo.GetWidth()))
            {
                ++m_Caret;
                //nuxDebugMsg(TEXT("Group Add: %c"), m_textline[m_Caret-1]);
            }
            m_bMouseInsideTextArea = false;
        }
        else if(m_Caret < StrLength)
        {
            ++m_Caret;
            //nuxDebugMsg(TEXT("Group Add: %c"), m_textline[m_Caret-1]);
        }
        else
        {
            m_Caret = StrLength;
        }
    }

    AdjustCursorAndTextPosition();
}

long BaseKeyboardHandler::ProcessKey(        
    unsigned long    eventType    /*event type*/,
    unsigned long    keysym       /*event keysym*/,
    unsigned long    state        /*event state*/,
    const TCHAR*      character    /*character*/,
    const Geometry& g)
{
    //if(ievent.event_type != I_AsciiKey && ievent.event_type != I_KeyPress && ievent.event_type != I_KeyRelease)
    if(eventType != INL_KEYDOWN)
        return 0;

    m_need_redraw = true;
    //unsigned key = ievent.ascii_code;
    unsigned key = 0;
    if(character != 0)
        key = (unsigned) *character;

    long virtual_code = keysym;

    if(key != 0)
    {
        switch(m_KeyType)
        {
        case eDecimalNumber:
            {
                // Check if Key code is in ".0123456789";
                // Be careful because WM_KEYDOWN and WM_KEYUP send a key value 
                // that gets here. If key pad 0 is press/released, WM_KEYDOWN/WM_KEYUP will send key = key_kp0.
                // It is WM_CHAR who send key = 0x30 = '0'. Therefore, do not use strchr(".0123456789", key) to test 
                // if key is a digit character. When using strchr(".0123456789", key_kp0) we get a positive result 
                // and the result is unexpected.
                if((key >= TCHAR('0') && key <= TCHAR('9')) || (key == TCHAR('.')) || (key == TCHAR('-')) || (key == TCHAR('+')))
                {
                    if( m_Caret != m_nSelStart )
                    {
                        DeleteSelectionText();
                    }

                    // '-' and '+' can only be at position 0 of the number
                    if((key == TCHAR('-')) || (key == TCHAR('+')))
                    {
                        if((m_Caret == 0) && (m_textline[0] != TCHAR('+')) && (m_textline[0] != TCHAR('-')))
                        {
                            // If we are in overwrite mode and there is already
                            // a char at the caret's position, simply replace it.
                            // Otherwise, we insert the char as normal.
                            if( !m_bInsertMode && m_Caret < (t_u32)m_textline.Length() )
                            {
                                m_textline[m_Caret] = key;
                                PlaceCaret(m_Caret + 1 );
                                m_nSelStart = m_Caret;
                            } 
                            else
                            {
                                // Insert the char
                                if( m_Caret <= m_textline.Length() )
                                {
                                    m_textline.Insert(m_Caret, 1, key);
                                    PlaceCaret(m_Caret + 1 );
                                    m_nSelStart = m_Caret;
                                }
                            }
                        }
                    }
                    else
                    {
                        // If we are in overwrite mode and there is already
                        // a char at the caret's position, simply replace it.
                        // Otherwise, we insert the char as normal.
                        if( !m_bInsertMode && m_Caret < m_textline.Length() )
                        {
                            m_textline[m_Caret] = key;
                            PlaceCaret(m_Caret + 1 );
                            m_nSelStart = m_Caret;
                        } 
                        else
                        {
                            // Insert the char
                            if( m_Caret <= m_textline.Length() )
                            {
                                m_textline.Insert(m_Caret, 1, key);
                                PlaceCaret(m_Caret + 1 );
                                m_nSelStart = m_Caret;
                            }
                        }
                    }
                }
                break;
            }
        case eIntegerNumber:
            {
                // Check if Key code is in "0123456789";
                if((key >= TCHAR('0') && key <= TCHAR('9')) || (key == TCHAR('-')) || (key == TCHAR('+')))
                {
                    if( m_Caret != m_nSelStart )
                    {
                        DeleteSelectionText();
                    }

                    // '-' and '+' can only be at position 0 of the number
                    if((key == TCHAR('-')) || (key == TCHAR('+')))
                    {
                        // If we are in overwrite mode and there is already
                        // a char at the caret's position, simply replace it.
                        // Otherwise, we insert the char as normal.
                        if( !m_bInsertMode && m_Caret < m_textline.Length() )
                        {
                            m_textline[m_Caret] = key;
                            PlaceCaret(m_Caret + 1 );
                            m_nSelStart = m_Caret;
                        } 
                        else
                        {
                            // Insert the char
                            if( m_Caret <= m_textline.Length() )
                            {
                                m_textline.Insert(m_Caret, 1, key);
                                PlaceCaret(m_Caret + 1 );
                                m_nSelStart = m_Caret;
                            }
                        }
                    }
                    else
                    {
                        // If we are in overwrite mode and there is already
                        // a char at the caret's position, simply replace it.
                        // Otherwise, we insert the char as normal.
                        if( !m_bInsertMode && m_Caret < m_textline.Length() )
                        {
                            m_textline[m_Caret] = key;
                            PlaceCaret(m_Caret + 1 );
                            m_nSelStart = m_Caret;
                        } 
                        else
                        {
                            // Insert the char
                            if( m_Caret <= m_textline.Length() )
                            {
                                m_textline.Insert(m_Caret, 1, key);
                                PlaceCaret(m_Caret + 1 );
                                m_nSelStart = m_Caret;
                            }
                        }
                    }
                }
                break;
            }
        
        case eHexadecimalNumber:
            {
                if((key >= TCHAR('0') && key <= TCHAR('9')) || (key >= TCHAR('a') && key <= TCHAR('f')) || (key >= TCHAR('A') && key <= TCHAR('F')) || (key == TCHAR('-')) || (key == TCHAR('+')))
                //if(strchr("0123456789abcdefABCDEF", key))
                {
                    if( m_Caret != m_nSelStart )
                    {
                        DeleteSelectionText();
                    }

                    // If we are in overwrite mode and there is already
                    // a char at the caret's position, simply replace it.
                    // Otherwise, we insert the char as normal.
                    if( !m_bInsertMode && m_Caret < m_textline.Length() )
                    {
                        m_textline[m_Caret] = key;
                        PlaceCaret(m_Caret + 1 );
                        m_nSelStart = m_Caret;
                    } 
                    else
                    {
                        // Insert the char
                        if( m_Caret <= m_textline.Length() )
                        {
                            m_textline.Insert(m_Caret, 1, key);
                            PlaceCaret(m_Caret + 1 );
                            m_nSelStart = m_Caret;
                        }
                    }

                }
                break;
            }

        case eBinaryNumber:
            {
                //if(strchr("01", key))
                if((key >= TCHAR('0') && key <= TCHAR('1')))
                {
                    if( m_Caret != m_nSelStart )
                    {
                        DeleteSelectionText();
                    }

                    // If we are in overwrite mode and there is already
                    // a char at the caret's position, simply replace it.
                    // Otherwise, we insert the char as normal.
                    if( !m_bInsertMode && m_Caret < m_textline.Length() )
                    {
                        m_textline[m_Caret] = key;
                        PlaceCaret(m_Caret + 1 );
                        m_nSelStart = m_Caret;
                    } 
                    else
                    {
                        // Insert the char
                        if( m_Caret <= m_textline.Length() )
                        {
                            m_textline.Insert(m_Caret, 1, key);
                            PlaceCaret(m_Caret + 1 );
                            m_nSelStart = m_Caret;
                        }
                    }
                }
                break;
            }

        case eAlphaNumeric:
        default:
            {
                if(key > 0x1F && key < 0x7f)
                {
                    if( m_Caret != m_nSelStart )
                    {
                        DeleteSelectionText();
                    }

                    // If we are in overwrite mode and there is already
                    // a char at the caret's position, simply replace it.
                    // Otherwise, we insert the char as normal.
                    if( !m_bInsertMode && m_Caret < m_textline.Length() )
                    {
                        m_textline[m_Caret] = key;
                        PlaceCaret(m_Caret + 1 );
                        m_nSelStart = m_Caret;
                    } 
                    else
                    {
                        // Insert the char
                        if( m_Caret <= m_textline.Length() )
                        {
                            m_textline.Insert(m_Caret, 1, key);
                            PlaceCaret(m_Caret + 1 );
                            m_nSelStart = m_Caret;
                        }
                    }

                }
                break;
            }
        }
    }

    // CTRL+C
    if((virtual_code == INL_VK_C) && (state & INL_STATE_CTRL))
    {
        NString s = GetSelectedText();
#if defined(INL_OS_WINDOWS)
        inlCopyTextToClipboard(s.GetTCharPtr());
#endif
    }

    // CTRL+V
    if((virtual_code == INL_VK_V) && (state & INL_STATE_CTRL))
    {
#if defined(INL_OS_WINDOWS)
        NString s = inlReadTextToClipboard();
#elif defined(INL_OS_LINUX)
        NString s = "Paste not implemented yet";
#endif
        t_u32 start = GetTextSelectionStart();
        t_u32 end = GetTextSelectionEnd();
        m_textline.Replace(start, end - start, s.m_string);
        
        m_Caret = start + (t_u32)s.Length();
        UnselectAllText();
    }

    if((virtual_code == INL_VK_ESCAPE))
    {
        // If Text is selected, Unselect
       UnselectAllText();
    }

    if((virtual_code == INL_VK_ENTER) ||
        (virtual_code == INL_KP_ENTER))
    {
        SelectAllText();
    }

    if(virtual_code == INL_VK_BACKSPACE)
    {
        if( m_Caret != m_nSelStart )
        {
            DeleteSelectionText();
        }
        else
        {
            // Deleting one character
            if( m_Caret > 0 )
            {
                m_textline.Erase(m_Caret-1, 1);
                PlaceCaret(m_Caret - 1 );

                m_nSelStart = m_Caret;
            }
        }
    }
    else if(virtual_code == INL_VK_DELETE)
    {
        // Check if there is a text selection.
        if( m_Caret != m_nSelStart )
        {
            DeleteSelectionText();
        }
        else
        {
            // Deleting one character
            if( m_Caret < m_textline.Length() )
            {
                m_textline.Erase(m_Caret, 1);
            }
        }
    }
    else if(virtual_code == INL_KP_LEFT || virtual_code == INL_VK_LEFT)
    {
        if(IsTextSelected() && ((state & INL_STATE_SHIFT) == 0))
        {
            //m_Caret = Min(m_Caret, m_nSelStart);
            if(m_Caret)
                --m_Caret;
            UnselectAllText();
        }
        else
        {
            if( state & INL_STATE_CTRL )
            {
                // Control is down. Move the caret to a new item
                // instead of a character.
                m_Caret = PrevWordPosition( m_Caret);
                PlaceCaret(m_Caret );
            }
            else if( m_Caret > 0 )
                PlaceCaret(m_Caret - 1 );

            if((state & INL_STATE_SHIFT) == 0)
            {
                // Shift is not down. Update selection
                // start along with the caret.
                m_nSelStart = m_Caret;
            }
        }
    }
    else if(virtual_code == INL_KP_RIGHT || virtual_code == INL_VK_RIGHT)
    {
        if(IsTextSelected() && ((state & INL_STATE_SHIFT) == 0))
        {
            m_Caret = Max(m_Caret, m_nSelStart);
            UnselectAllText();
        }
        else
        {
            if( state & INL_STATE_CTRL)
            {
                // Control is down. Move the caret to a new item
                // instead of a character.
                m_Caret = NextWordPosition( m_Caret);
                PlaceCaret(m_Caret );
            }
            else if( m_Caret < m_textline.Length() )
                PlaceCaret(m_Caret + 1 );

            if( (state & INL_STATE_SHIFT) == 0 )
            {
                // Shift is not down. Update selection
                // start along with the caret.
                m_nSelStart = m_Caret;
            }
        }
    }
    else if(virtual_code == INL_KP_HOME || virtual_code == INL_VK_HOME)
    {
        if((state & INL_STATE_SHIFT) == 0)
        {
            PlaceCaret(0 );
            // Shift is not down. Update selection
            // start along with the caret.
            m_nSelStart = m_Caret;
        }
        else
        {
            PlaceCaret(0 );
        }
    }
    else if(virtual_code == INL_KP_END || virtual_code == INL_VK_END)
    {
        if((state & INL_STATE_SHIFT) == 0)
        {
            PlaceCaret((t_u32)m_textline.Length() );
            // Shift is not down. Update selection
            // start along with the caret.
            m_nSelStart = m_Caret;
        }
        else
        {
            PlaceCaret((t_u32)m_textline.Length() );
        }
    }
    else if(virtual_code == INL_VK_ESCAPE)
    {
        return virtual_code;
    }
    else if(virtual_code == INL_VK_ENTER ||
        virtual_code == INL_KP_ENTER)
    {
        return virtual_code;
    }
    else
    {
        m_need_redraw = false;
    }


    t_s32 offset= 0;
    if(virtual_code == INL_KP_HOME)
    {
        m_text_positionx = 0;
    }
    else if(virtual_code == INL_KP_END)
    {
        t_u32 str_width = GetFont()->GetStringWidth(m_textline.GetTCharPtr());
        if(str_width + sCursorWidth > (t_u32)m_clip_region.GetWidth())
            m_text_positionx = m_clip_region.GetWidth() - (str_width + sCursorWidth);
        else
            m_text_positionx = 0;
    }
    else if(m_textline.Length() != 0)
    {
        AdjustCursorAndTextPosition();
        m_need_redraw = true;
    }
    else if(m_textline.Length() == 0)
    {
        ClearText();
        m_need_redraw = true;
    }

        
    return virtual_code;
}

void BaseKeyboardHandler::AdjustCursorAndTextPosition()
{
    t_s32 str_width = GetFont()->GetStringWidth(m_textline.GetTCharPtr());
    NString temp0;
    if(m_Caret > 0)
        temp0 = m_textline.GetSubString(0, m_Caret - 1).GetTStringRef();
    else
        temp0 = TEXT("");

    //      0          1         2
    //      01234567|8901234567890123456789
    //      abcdefgh|ijklmn
    //              
    //      Caret pos = 8
    //      temp0 = "abcdefg"
    //      temp1 = "abcdefgh"
    //      temp2 = "abcdefghi"

    NString temp1 = m_textline.GetSubString(0, m_Caret).GetTStringRef();
    NString temp2 = m_textline.GetSubString(0, m_Caret + 1).GetTStringRef();
    t_s32 str_width0 = GetFont()->GetStringWidth(temp0);
    t_s32 str_width1 = GetFont()->GetStringWidth(temp1);
    t_s32 str_width2 = GetFont()->GetStringWidth(temp2);


    if((m_text_positionx + str_width1 + sCursorWidth) > (t_u32)m_clip_region.GetWidth())
    {
        // Hitting the end of the text box
        if(m_Caret == m_textline.Length())
        {
            m_text_positionx = -(str_width + (t_s32)sCursorWidth - m_clip_region.GetWidth());
        }
        else
        {
            t_s32 PreviousCharWidth = str_width1 - str_width0;
            t_s32 Offset = Min<t_s32>(sJumpOffsetAtBorders, str_width + sCursorWidth - str_width1);
            while(Offset > m_clip_region.GetWidth())
            {
                Offset /= 2;
            }
            if(Offset < PreviousCharWidth)
                Offset = PreviousCharWidth;

            m_text_positionx -= Offset;
            if(m_text_positionx + str_width + sCursorWidth < (t_u32)m_clip_region.GetWidth())
            {
                m_text_positionx = -(str_width + (t_s32)sCursorWidth - m_clip_region.GetWidth());
            }
        }
    }
    else if((m_text_positionx + str_width1) <= 0)
    {
        // Hitting the start of the text box
        if(m_Caret == 0)
        {
            m_text_positionx = 0;
        }
        else
        {
            t_s32 NextCharWidth = str_width2 - str_width1;
            t_s32 Offset = Min<t_s32>(sJumpOffsetAtBorders, str_width1);
            while(Offset > m_clip_region.GetWidth())
            {
                Offset /= 2;
            }
            if(Offset < NextCharWidth)
                Offset = NextCharWidth;

            if(Offset > str_width1)
            {
                m_text_positionx = 0;
                return;
            }                

            m_text_positionx += Offset;
            if(m_text_positionx > 0)
            {
                m_text_positionx = 0;
            }
        }
    }
}

void BaseKeyboardHandler::MoveCursorAtStart()
{
    m_previous_cursor_position = m_Caret;
    m_Caret  = 0;
}
void BaseKeyboardHandler::MoveCursorAtEnd()
{
    t_u32 StrLength = ( t_u32)m_textline.Length();
    m_previous_cursor_position = m_Caret;
    m_Caret = StrLength;
}

void BaseKeyboardHandler::SetKeyEntryType(BaseKeyboardHandler::eKeyEntryType keytype)
{
    m_KeyType = keytype;
}

BaseKeyboardHandler::eKeyEntryType BaseKeyboardHandler::GetKeyEntryType()
{
    return m_KeyType;
}

void BaseKeyboardHandler::SetText(const TCHAR* str)
{
    m_textline = str;
    // Every time we set the text, we reposition the cursor at the beginning of the text,
    // and the text position is set to zero with regard to the start of the geometry area.
    MoveCursorAtStart();
    m_nSelStart = Min<t_s32>(m_nSelStart, (t_s32)StringLength(str));
    m_Caret = Min<t_s32>(m_Caret, (t_s32)StringLength(str));
    if(m_Caret < m_nSelStart)
        m_nSelStart = Max<t_s32>(m_nSelStart, (t_s32)StringLength(str));
    else if(m_Caret > m_nSelStart)
        m_Caret = Max<t_s32>(m_Caret, (t_s32)StringLength(str));

    m_text_positionx = 0;
}

void BaseKeyboardHandler::SetText(const tstring& s)
{
    SetText(s.c_str());
}

void BaseKeyboardHandler::SetText(const NString& s)
{
    SetText(s.GetTCharPtr());
}

//    CursorPosToX (similar to ScriptStringCPtoX from microsoft UniScript)
//        The CursorPosToX function returns the x-coordinate for the leading or trailing edge of a character position.

//        Parameters
//        icp
//          [in] Character position in the string.
//        fTrailing
//          [in] Indicates the edge of the icp that corresponds to the x coordinate. If TRUE, it indicates the trailing edge. If FALSE, it indicates the leading edge. 
//        pX
//          [out] Pointer to a variable that receives the corresponding x coordinate for the icp. 
//
//        Return Values
//          If the function succeeds, it returns S_OK.
//          If the function fails, it returns an HRESULT.
//          The return value can be tested with the SUCCEEDED and FAILED macros.
bool BaseKeyboardHandler::CursorPosToX(
                                       t_s32 icp,
                                       bool fTrailing,
                                       t_s32 *pX)
{
    return GetFont()->CursorPosToX(m_textline.GetTStringRef(), icp, fTrailing, pX);
}

//    XToCursorPosition (similar to ScriptStringXtoCP from microsoft UniScript)
//        The XToCursorPosition function converts an x-coordinate to a character position.
//
//    Parameters
//        iX
//          [in] Specifies the x coordinate.
//        FirstVisibleCharIndex,
//          [in] Index of the first visible character in the text box
//        piCh
//          [out] Pointer to a variable that receives the character position corresponding to iX.
//        piTrailing
//          [out] Pointer to a variable that receives an indicator whether the position is the leading or trailing edge of the character. 
//
//        Return Values
//          If the function is successful, it returns S_OK.
//          If the function fails, it returns an HRESULT.
//          The return value can be tested with the SUCCEEDED and FAILED macros.
bool BaseKeyboardHandler::XToCursorPosition(
    t_s32 iX,
    UINT FirstVisibleCharIndex,
    t_s32 *piCh,
    t_s32 *piTrailing)
{
    return GetFont()->XToCursorPosition(m_textline.GetTStringRef(), iX, FirstVisibleCharIndex, piCh, piTrailing);
}


void BaseKeyboardHandler::SetClipRegion(const Geometry& g)
{
    m_clip_region = g;
}

void BaseKeyboardHandler::GetTextSelection(t_s32 *start, t_s32 *end) const
{
    *start = Min(m_nSelStart, m_Caret);
    *end = Max(m_nSelStart, m_Caret);
}

t_s32 BaseKeyboardHandler::GetTextSelectionStart() const
{
    return Min(m_nSelStart, m_Caret);
}

t_s32 BaseKeyboardHandler::GetTextSelectionEnd() const
{
    return Max(m_nSelStart, m_Caret);
}

void BaseKeyboardHandler::SelectAllText()
{
    m_nSelStart = 0;
    m_Caret = (t_u32)m_textline.Length();
    AdjustCursorAndTextPosition();
}

void BaseKeyboardHandler::UnselectAllText()
{
    m_nSelStart = m_Caret;
}

NString BaseKeyboardHandler::GetSelectedText() const
{
    if(m_nSelStart == m_Caret)
    {
        return NString(TEXT(""));
    }
    else
    {
        NString s = m_textline.GetSubString(GetTextSelectionStart(), GetTextSelectionEnd() - GetTextSelectionStart()).GetTStringRef();
        return s;
    }
}

bool BaseKeyboardHandler::IsTextSelected()
{
    if(m_Caret != m_nSelStart)
        return true;
    return false;
}

void BaseKeyboardHandler::SetFont(const NFontPtr& Font)
{
    m_Font = Font;
}

const NFontPtr& BaseKeyboardHandler::GetFont() const 
{
    if(m_Font)
        return m_Font;
    else
        return GFont;
}
NAMESPACE_END_GUI
