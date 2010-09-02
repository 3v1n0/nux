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


#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <string>
#include "Utils.h"
#include "NuxGraphics/FontTexture.h"
#if defined(INL_OS_WINDOWS)
    #include "NuxGraphics/Gfx_Events.h"
#elif defined(INL_OS_LINUX)
    #include "NuxGraphics/GfxEventsX11.h"
#endif
#include "NuxCore/Color.h"

NAMESPACE_BEGIN_GUI

class GraphicsContext;

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
        const TCHAR*      character    /*character*/,
        const Geometry& g);
    
    NString GetTextLine() const {return m_textline;}
    t_u32 GetLength() const {return (t_u32)m_textline.Length();}

    t_u32 GetCursorPosition() const {return m_Caret;}
    bool NeedRedraw() const {return m_need_redraw;}
    t_s32 GetPositionX() const {return m_text_positionx;}
    t_s32 GetPositionY() const {return m_text_positiony;}

    void SetText(const TCHAR* str);
    void SetText(const tstring& s);
    void SetText(const NString& s);

    void ClearText();
    void PlaceCaret(t_u32 cp);

    void MoveCursorAtStart();
    void MoveCursorAtEnd();

    void SetKeyEntryType(eKeyEntryType keytype);
    eKeyEntryType GetKeyEntryType();

    void SetClipRegion(const Geometry& g);
    void GetTextSelection(t_s32 *start, t_s32 *end) const;
    t_s32 GetTextSelectionStart() const;
    t_s32 GetTextSelectionEnd() const;
    NString GetSelectedText() const;
    void SelectAllText();
    void UnselectAllText();
    bool IsTextSelected();

    void SetPrefix(NString);  // 0x865, 25 rad, 25 degree...
    void SetPostfix(NString); 

    void EnterFocus();

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
    bool CursorPosToX(t_s32 icp, 
        bool fTrailing, 
        t_s32 *pX);

    //    XToCursorPosition (similar to ScriptStringXtoCP from microsoft UniScript)
    //        The XToCursorPosition function converts an x-coordinate to a character position.
    //
    //    Parameters
    //        iX
    //          [in] Specifies the x coordinate.
    //        piCh
    //          [out] Pointer to a variable that receives the character position corresponding to iX.
    //        piTrailing
    //          [out] Pointer to a variable that receives an indicator whether the position is the leading or trailing edge of the character. 
    //
    //        Return Values
    //          If the function is successful, it returns S_OK.
    //          If the function fails, it returns an HRESULT.
    //          The return value can be tested with the SUCCEEDED and FAILED macros.
    bool XToCursorPosition(t_s32 iX,
        UINT FirstVisibleCharIndex,
        t_s32 *piCh, 
        t_s32 *piTrailing);

    t_u32 NextWordPosition(t_u32 cp);
    t_u32 PrevWordPosition(t_u32 cp);
    void ResolveCaretPosition(t_s32 x, t_s32 y);
    void CaretAutoScroll(t_s32 x, t_s32 y, Geometry geo);
    void MouseDown(t_s32 x, t_s32 y);
    void MouseUp(t_s32 x, t_s32 y);
    void MouseDrag(t_s32 x, t_s32 y);
    //bool IsMouseDrag();

    void SetFont(const NFontPtr& Font);
    const NFontPtr& GetFont() const;

protected:
    NFontPtr m_Font;
    NString m_textline;
    t_s32 m_previous_cursor_position;
    bool m_need_redraw;
    t_s32 m_text_positionx;
    t_s32 m_text_positiony;
    Geometry m_clip_region;

    eKeyEntryType m_KeyType;

    t_u32   m_Caret;           // Caret position, in characters
    bool    m_bInsertMode;     // If true, control is in insert mode. Else, overwrite mode.
    t_u32   m_nSelStart;       // Starting position of the selection. The caret marks the end.
    t_u32   m_nFirstVisible;   // First visible character in the edit control
    
    void DeleteSelectionText();
    void InsertChar(t_u32 character);
    void AdjustCursorAndTextPosition();


    bool m_bEnteringFocus;
    bool m_bMouseDrag;
    // m_bMouseInsideTextArea Detects the fast transition of the mouse from inside the text area to outside
    bool m_bMouseInsideTextArea;

    static t_u32 sJumpOffsetAtBorders;
    static t_u32 sCursorWidth;
};

NAMESPACE_END_GUI

#endif // KEYBOARDHANDLER_H
