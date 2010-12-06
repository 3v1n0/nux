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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "TextView.h"
#include "TextViewInternal.h"
#include "HScrollBar.h"
#include "VScrollBar.h"

namespace nux
{

  int ScrollDir (int counter, int dir);

//
//	WM_MOUSEACTIVATE
//
//	Grab the keyboard input focus
//
  LONG TextView::OnMouseActivate (HWND hwndTop, t_u32 nHitTest, t_u32 nMessage)
  {
    SetFocus (m_hWnd);
    return MA_ACTIVATE;
  }

//
//	WM_LBUTTONDOWN
//
//  Position caret to nearest text character under mouse
//
  void TextView::OnLButtonDown (int mx, int my, unsigned long button_flags, unsigned long key_flags /*t_u32 nFlags, int mx, int my*/)
  {
    t_u32 nLineNo;
    t_u32 nFileOff;

    mx = mx - getBorder() - GetViewContentLeftMargin();
    my = my - getTopBorder() - GetViewContentTopMargin();

    // remove any existing selection
    InvalidateRange (m_nSelectionStart, m_nSelectionEnd);

    // regular mouse input - mouse is within
    if (mx >= LeftMarginWidth() )
    {
      m_CharacterAtCursor = 0;
      m_CursorCharacterPosition = 0;
      // map the mouse-coordinates to a real file-offset-coordinate
      MouseCoordToFilePos (mx, my, &nLineNo, &nFileOff, &m_nCaretPosX, &m_CharacterAtCursor, &m_CursorCharacterPosition);
      m_nAnchorPosX = m_nCaretPosX;

      UpdateCaretXY (m_nCaretPosX, nLineNo);

      // Any key but <shift>
      if (IsKeyPressed (VK_SHIFT) == false)
      {
        // remove any existing selection
        InvalidateRange (m_nSelectionStart, m_nSelectionEnd);
        // reset cursor and selection offsets to the same location
        m_nSelectionStart	= nFileOff;
        m_nSelectionEnd		= nFileOff;
        m_nCursorOffset		= nFileOff;
      }
      else
      {
        // redraw to cursor
        InvalidateRange (m_nSelectionEnd, nFileOff);

        // extend selection to cursor
        m_nSelectionEnd		= nFileOff;
        m_nCursorOffset		= nFileOff;
      }

      if (IsKeyPressed (VK_MENU) )
      {
        m_cpBlockStart.line = nLineNo;
        m_cpBlockStart.xpos = m_nCaretPosX;
        m_nSelectionType	= SEL_BLOCK;
      }
      else
      {
        m_nSelectionType	= SEL_NORMAL;
      }

      // set capture for mouse-move selections
      m_nSelectionMode = IsKeyPressed (VK_MENU) ? SEL_BLOCK : SEL_NORMAL;
    }
    // mouse clicked within margin
    else
    {
      // remove any existing selection
      InvalidateRange (m_nSelectionStart, m_nSelectionEnd);

      nLineNo = (my / m_nLineHeight) + m_nVScrollPos;

      //
      // if we click in the margin then jump back to start of line
      //
      if (m_nHScrollPos != 0)
      {
        m_nHScrollPos = 0;
        SetupScrollbars();
        RefreshWindow();
      }

      m_pTextDoc->lineinfo_from_lineno (nLineNo, &m_nSelectionStart, &m_nSelectionEnd, 0, 0);
      m_nSelectionEnd    += m_nSelectionStart;
      m_nCursorOffset	    = m_nSelectionStart;

      m_nSelMarginOffset1 = m_nSelectionStart;
      m_nSelMarginOffset2 = m_nSelectionEnd;

      InvalidateRange (m_nSelectionStart, m_nSelectionEnd);

      UpdateCaretOffset (m_nCursorOffset, FALSE, &m_nCaretPosX, &m_nCurrentLine);
      m_nAnchorPosX = m_nCaretPosX;
      //RepositionCaret();

      // set capture for mouse-move selections
      m_nSelectionMode = SEL_MARGIN;
    }

    UpdateLine (nLineNo);

    SetCapture (GetThreadGLWindow()->GetWindowHandle() );

    StopBlinkCursor (false);
    StartBlinkCursor (false);
    NeedSoftRedraw();
  }

//
//	WM_LBUTTONUP
//
//	Release capture and cancel any mouse-scrolling
//
  void TextView::OnLButtonUp (int mx, int my, unsigned long button_flags, unsigned long key_flags /*t_u32 nFlags, int mx, int my*/)
  {
    mx = mx - getBorder() - GetViewContentLeftMargin();
    my = my - getTopBorder() - GetViewContentTopMargin();

    // shift cursor to end of selection
    if (m_nSelectionMode == SEL_MARGIN)
    {
      m_nCursorOffset = m_nSelectionEnd;
      // Invalidate the selected range just to delete the cursor that was in it.
      InvalidateRange (m_nSelectionStart, m_nSelectionEnd);
      UpdateCaretOffset (m_nCursorOffset, FALSE, &m_nCaretPosX, &m_nCurrentLine);
    }

    if (m_nSelectionMode)
    {
      // cancel the scroll-timer if it is still running
      if (MouseAutoScrollHandle.IsValid() )
      {
        GetTimer().RemoveTimerHandler (MouseAutoScrollHandle);
        MouseAutoScrollHandle = 0;
      }

      m_nSelectionMode = SEL_NONE;
      ReleaseCapture();
    }

    NeedSoftRedraw();
  }

//
//	WM_LBUTTONDBKCLK
//
//	Select the word under the mouse
//
  void TextView::OnLButtonDblClick (int mx, int my, unsigned long button_flags, unsigned long key_flags)
  {
    mx = mx - getBorder() - GetViewContentLeftMargin();
    my = my - getTopBorder() - GetViewContentTopMargin();

    // remove any existing selection
    InvalidateRange (m_nSelectionStart, m_nSelectionEnd);

    // regular mouse input - mouse is within scrolling viewport
    if (mx >= LeftMarginWidth() )
    {
      t_u32 lineno, fileoff;
      //int   xpos;

      // map the mouse-coordinates to a real file-offset-coordinate
      MouseCoordToFilePos (mx, my, &lineno, &fileoff, &m_nCaretPosX, &m_CharacterAtCursor, &m_CursorCharacterPosition);
      m_nAnchorPosX = m_nCaretPosX;

      // move selection-start to start of word
      //-->MoveWordStart();
      m_nSelectionStart = m_nCursorOffset;

      // move selection-end to end of word
      //-->MoveWordEnd();
      m_nSelectionEnd = m_nCursorOffset;

      // update caret position
      InvalidateRange (m_nSelectionStart, m_nSelectionEnd);
      UpdateCaretOffset (m_nCursorOffset, TRUE, &m_nCaretPosX, &m_nCurrentLine);
      m_nAnchorPosX = m_nCaretPosX;

      //NotifyParent(TVN_CURSOR_CHANGE);
    }
  }

//
//	WM_MOUSEMOVE
//
//	Set the selection end-point if we are dragging the mouse
//
  void TextView::RecvMouseMove (int mx, int my, int dx, int dy, unsigned long button_flags, unsigned long key_flags /*t_u32 nFlags, int mx, int my*/)
  {
    mx = mx - getBorder() - GetViewContentLeftMargin();
    my = my - getTopBorder() - GetViewContentTopMargin();

    if (m_nSelectionMode)
    {
      t_u32	nLineNo, nFileOff;
      BOOL	fCurChanged = FALSE;

      Geometry	rect;
      Point pt (mx, my);
      //int		cx;					// caret coordinates

      //
      //	First thing we must do is switch from margin-mode to normal-mode
      //	if the mouse strays into the main document area
      //
      if ( (m_nSelectionMode == SEL_MARGIN) && (mx > LeftMarginWidth() ) )
      {
        m_nSelectionMode = SEL_NORMAL;
        GetThreadGLWindow()->SetWindowCursor (LoadCursor (0, IDC_IBEAM) );
      }

      //
      //	Mouse-scrolling: detect if the mouse
      //	is inside/outside of the TextView scrolling area
      //  and stop/start a scrolling timer appropriately
      //
      //GetClientRect(m_hWnd, &rect);

      rect = GetTextAreaGeometry();

      // build the scrolling area
      rect.OffsetPosition (LeftMarginWidth(), 0);
      rect.OffsetSize (-LeftMarginWidth(), -rect.GetHeight() % m_nLineHeight);


      // If mouse is within this area, we don't need to scroll
      if (rect.IsPointInside (pt.x, pt.y) )
      {
        if (MouseAutoScrollHandle.IsValid() )
        {
          GetTimer().RemoveTimerHandler (MouseAutoScrollHandle);
          MouseAutoScrollHandle = 0;
        }
      }
      // If mouse is outside window, start a timer in
      // order to generate regular scrolling intervals
      else
      {
        if (m_nScrollTimer == 0)
        {
          m_nScrollCounter = 0;
          MouseAutoScrollHandle = GetTimer().AddTimerHandler (10, MouseAutoScrollTimer, this);
          nuxDebugMsg (TEXT ("Here") );
        }
      }

      m_CharacterAtCursor = 0;
      m_CursorCharacterPosition = 0;
      // get new cursor offset+coordinates
      MouseCoordToFilePos (mx, my, &nLineNo, &nFileOff, &m_nCaretPosX, &m_CharacterAtCursor, &m_CursorCharacterPosition);
      m_nAnchorPosX = m_nCaretPosX;

      m_cpBlockEnd.line = nLineNo;
      m_cpBlockEnd.xpos = mx + m_nHScrollPos * m_nFontWidth - LeftMarginWidth();//m_nCaretPosX;


      // redraw the old and new lines if they are different
      UpdateLine (nLineNo);

      // update the region of text that has changed selection state
      fCurChanged = m_nSelectionEnd == nFileOff ? FALSE : TRUE;
      //if(m_nSelectionEnd != nFileOff)
      {
        t_u32 linelen;
        m_pTextDoc->lineinfo_from_lineno (nLineNo, 0, &linelen, 0, 0);

        m_nCursorOffset	= nFileOff;

        if (m_nSelectionMode == SEL_MARGIN)
        {
          if (nFileOff >= m_nSelectionStart)
          {
            nFileOff += linelen;
            m_nSelectionStart = m_nSelMarginOffset1;
          }
          else
          {
            m_nSelectionStart = m_nSelMarginOffset2;
          }
        }

        // redraw from old selection-pos to new position
        InvalidateRange (m_nSelectionEnd, nFileOff);

        // adjust the cursor + selection to the new offset
        m_nSelectionEnd = nFileOff;
      }

      // always set the caret position because we might be scrolling
      UpdateCaretXY (m_nCaretPosX, nLineNo);

      NeedSoftRedraw();
    }
    // mouse isn't being used for a selection, so set the cursor instead
    else
    {

      Geometry	rect = GetTextAreaGeometry();

      if (mx >= 0 && mx < LeftMarginWidth() )
      {
        GetThreadGLWindow()->SetWindowCursor (m_hMarginCursor);
      }
      else if (rect.IsPointInside (mx + getBorder(), my + getTopBorder() ) )
        GetThreadGLWindow()->SetWindowCursor (LoadCursor (0, IDC_IBEAM) );
      else
      {
        GetThreadGLWindow()->SetWindowCursor (LoadCursor (0, IDC_ARROW) );
      }
    }
  }

  void TextView::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    x = x - getBorder() - GetViewContentLeftMargin();
    y = y - getTopBorder() - GetViewContentTopMargin();

    Geometry	rect = GetTextAreaGeometry();

    if (x >= 0 && x < LeftMarginWidth() )
    {
      GetThreadGLWindow()->SetWindowCursor (m_hMarginCursor);
    }
    else if (rect.IsPointInside (x + getBorder(), y + getTopBorder() ) )
      GetThreadGLWindow()->SetWindowCursor (LoadCursor (0, IDC_IBEAM) );
    else
    {
      GetThreadGLWindow()->SetWindowCursor (LoadCursor (0, IDC_ARROW) );
    }
  }

  void TextView::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    x = x - getBorder() - GetViewContentLeftMargin();
    y = y - getTopBorder() - GetViewContentTopMargin();

    GetThreadGLWindow()->SetWindowCursor (LoadCursor (0, IDC_ARROW) );
  }


  void TextView::MouseAutoScrollHandler (void *v)
  {
    MouseAutoScrollHandle = GetTimer().AddTimerHandler (10, MouseAutoScrollTimer, this);
  }

//
//	WM_TIMER handler
//
//	Used to create regular scrolling
//
  void TextView::RecvTimer (void *v)
  {
    int	  dx = 0, dy = 0;	// scrolling vectors
    Geometry  rect;

    // get the mouse's client-coordinates
    Point point = GetThreadGLWindow()->GetMouseWindowCoord();

    // find client area, but make it an even no. of lines
    rect = GetTextAreaGeometry();
    rect.OffsetPosition (LeftMarginWidth(), 0);
    rect.OffsetSize (-LeftMarginWidth(), -rect.GetHeight() % m_nLineHeight);

    //
    // scrolling up / down??
    //
    if (point.y < rect.y)
    {
      dy = ScrollDir (m_nScrollCounter, point.y - rect.y);
      ScrollUp (1, 2 /*m_nLineHeight*/);
    }
    else if (point.y >= rect.y + rect.GetHeight() )
    {
      dy = ScrollDir (m_nScrollCounter, point.y - (rect.y + rect.GetHeight() ) );
      ScrollDown (1, 2 /*m_nLineHeight*/);
    }

    //
    // scrolling left / right?
    //
    if (point.x < rect.x)
    {
      dx = ScrollDir (m_nScrollCounter, point.x - rect.x);
      ScrollLeft (1, 2);
    }
    else if (point.x > rect.x + rect.GetWidth() )
    {
      dx = ScrollDir (m_nScrollCounter, point.x - (rect.x + rect.GetWidth() ) );
      ScrollRight (1, 2);
    }

    //
    // do the redraw now that the selection offsets are all
    // pointing to the right places and the scroll positions are valid.
    //
    if ( (dy != 0) /*(m_nSelectionMode != SEL_MARGIN)*/ /*hrgnUpdate != NULL*/)
    {
      // We perform a "fake" WM_MOUSEMOVE for two reasons:
      //
      // 1. To get the cursor/caret/selection offsets set to the correct place
      //    *before* we redraw (so everything is synchronized correctly)
      //
      // 2. To invalidate any areas due to mouse-movement which won't
      //    get done until the next WM_MOUSEMOVE - and then it would
      //    be too late because we need to redraw *now*
      //
      RecvMouseMove (point.x, point.y, 0, 0, 0, 0);

    }

    // keep track of how many WM_TIMERs we process because
    // we might want to skip the next one
    m_nScrollCounter++;
  }

//
//	Convert mouse(client) coordinates to a file-relative offset
//
//	Currently only uses the main font so will not support other
//	fonts introduced by syntax highlighting
//
  BOOL TextView::MouseCoordToFilePos (	int		 mx,           // [in]  mouse x-coord
                                        int		 my,            // [in]  mouse x-coord
                                        t_u32	*pnLineNo,      // [out] line number
                                        t_u32	*pnFileOffset,  // [out] zero-based file-offset (in chars)
                                        int		*psnappedX,     // [out] adjusted x coord of caret
                                        TCHAR   *Character,
                                        t_u32   *CharacterPosition
                                     )
  {
    t_u32 nLineNo;
    t_u32 off_chars;
    Geometry  rect;
    t_u32	  cp;

    // Get scrollable area
    rect = GetTextAreaGeometry();
    // Compute a rectangle that contains full lines (no partial lines)
    rect.OffsetSize (0, -rect.GetHeight() % m_nLineHeight);

    // Take left margin into account
    mx -= LeftMarginWidth();

    // Clip mouse to edge of window
    if (mx < 0)
      mx = 0;

    if (my < 0)
      my = 0;

    if (my >= rect.y + rect.GetHeight() )
      my = rect.y + rect.GetHeight() - 1;

    if (mx >= rect.x + rect.GetWidth() )
      mx = rect.x + rect.GetWidth()  - 1;

    // It's easy to find the line-number: just divide 'y' by the line-height
    nLineNo = (my / m_nLineHeight) + m_nVScrollPos;

    // make sure we don't go outside of the document
    if (nLineNo >= m_nLineCount)
    {
      nLineNo   = m_nLineCount ? m_nLineCount - 1 : 0;
      off_chars = m_pTextDoc->size();
    }

    mx += m_nHScrollPos * m_nFontWidth;

    // get the USPDATA object for the selected line!!
    //USPDATA *uspData = GetUspData(0, nLineNo);

    // convert mouse-x coordinate to a character-offset relative to start of line
    UspSnapXToOffset (nLineNo, mx, &mx, &cp, 0);

    // return coords!
    TextIterator itor = m_pTextDoc->iterate_line (nLineNo, &off_chars);

    if (pnLineNo)
      *pnLineNo		= nLineNo;

    if (pnLineNo)
      *pnFileOffset	= cp + off_chars;

    if (pnLineNo)
      *psnappedX		= mx;// - m_nHScrollPos * m_nFontWidth;

    //*psnappedX		+= LeftMarginWidth();

    if (CharacterPosition)
      *CharacterPosition = cp;

    // now find the x-coordinate on the specified line
    if (Character)
    {
      TCHAR buf[TEXTBUFSIZE];
      t_u32 len = 0;
      int offset = 0;

      while ( (len += itor.gettext (buf, TEXTBUFSIZE) ) > 0)
      {
        if (offset + len > *CharacterPosition)
        {
          *Character       = * (buf + *CharacterPosition);

          if (*Character < TEXT (' ') )
            *Character = 0;

          break;
        }

        offset += len;
      }
    }

    return 0;
  }

  LONG TextView::InvalidateLine (t_u32 nLineNo)
  {
    if ( (nLineNo >= m_nVScrollPos) && (nLineNo <= m_nVScrollPos + m_nWindowLines) )
    {
      RECT rect;

      rect.top    = (nLineNo - m_nVScrollPos) * m_nLineHeight;
      rect.bottom = rect.top + m_nLineHeight;
    }

    return 0;
  }

//
//	Redraw the specified range of text/data in the control
//
  LONG TextView::InvalidateRange (t_u32 nStart, t_u32 nFinish)
  {
    t_u32 start  = std::min (nStart, nFinish);
    t_u32 finish = std::max (nStart, nFinish);

    int   ypos;
    Geometry  rect;
    Geometry  client;
    TextIterator itor;

    // information about current line:
    t_u32 lineno;
    t_u32 off_chars;
    t_u32 len_chars;
    client = GetTextAreaGeometry();

    // nothing to do?
    if (start == finish)
    {
      // We still need to clean the line in case the cursor was on it.
    }

    //
    //	Find the start-of-line information from specified file-offset
    //
    lineno = m_pTextDoc->lineno_from_offset (start);

    // clip to top of window
    if (lineno < m_nVScrollPos)
    {
      lineno = m_nVScrollPos;
      itor   = m_pTextDoc->iterate_line (lineno, &off_chars, &len_chars);
      start  = off_chars;
      AddDirtyLine (lineno);
      NeedSoftRedraw();
    }
    else
    {
      itor   = m_pTextDoc->iterate_line (lineno, &off_chars, &len_chars);
      AddDirtyLine (lineno);
      NeedSoftRedraw();
    }

    if (!itor || start >= finish)
      return 0;

    ypos = (lineno - m_nVScrollPos) * m_nLineHeight;


    // invalidate *whole* lines. don't care about flickering anymore because
    // all output is double-buffered now, and this method is much simpler
    while (itor && off_chars < finish)
    {
      // jump down to next line
      itor = m_pTextDoc->iterate_line (++lineno, &off_chars, &len_chars);
      ypos   += m_nLineHeight;
      AddDirtyLine (lineno);
      NeedSoftRedraw();
    }

    return 0;
  }

//
//	Wrapper around SetCaretPos, hides the caret when it goes
//  off-screen (in case x/y wrap around)
//
  VOID TextView::MoveCaret (int xpos, t_u32 lineno)
  {
    bool visible = false;

    // convert x-coord to window-relative
    // xpos -= m_nHScrollPos * m_nFontWidth;
    // xpos += LeftMarginWidth();

    if (lineno >= m_nVScrollPos && lineno <= m_nVScrollPos + m_nWindowLines)
    {
      if (xpos >= LeftMarginWidth() )
        visible = true;
    }

    // hide caret if it was previously visible
    if (visible == false && m_fHideCaret == false)
    {
      m_fHideCaret = true;
      //HideCaret(m_hWnd);
    }
    // show caret if it was previously hidden
    else if (visible == true && m_fHideCaret == true)
    {
      m_fHideCaret = false;
      //ShowCaret(m_hWnd);
    }

    if (m_fHideCaret == false)
      m_CaretPosition.Set (xpos + getBorder() + GetViewContentLeftMargin(), (lineno - m_nVScrollPos) * m_nLineHeight + getTopBorder() + GetViewContentTopMargin() );

    m_RedrawCaret = true;
  }

//
//	x		- x-coord relative to start of line
//	lineno	- line-number
//
  void TextView::UpdateCaretXY (int xpos, t_u32 lineno)
  {
    bool visible = false;

    // convert x-coord to window-relative
    xpos -= m_nHScrollPos * m_nFontWidth;
    xpos += LeftMarginWidth();

    // only show caret if it is visible within viewport
    if (lineno >= m_nVScrollPos && lineno <= m_nVScrollPos + m_nWindowLines)
    {
      if (xpos >= LeftMarginWidth() /* && (GetThreadGLWindow()->GetCurrentEvent().e_event != NUX_MOUSE_PRESSED)*/)
        visible = true;
    }

    // hide caret if it was previously visible
    if (visible == false && m_fHideCaret == false)
    {
      m_fHideCaret = true;
      //HideCaret(m_hWnd);
    }
    // show caret if it was previously hidden
    else if (visible == true && m_fHideCaret == true)
    {
      m_fHideCaret = false;
      //ShowCaret(m_hWnd);
    }

    // set caret position if within window viewport
    if (m_fHideCaret == false)
    {
      m_CaretPosition.Set (xpos + getBorder() + GetViewContentLeftMargin(), (lineno - m_nVScrollPos) * m_nLineHeight + getTopBorder() + GetViewContentTopMargin() );
    }

    nuxDebugMsg (TEXT ("Caret x: %d"), xpos + getBorder() + GetViewContentLeftMargin() );
    m_RedrawCaret = true;
  }

//
//	Reposition the caret based on cursor-offset
//	return the resulting x-coord and line#
//
  void TextView::UpdateCaretOffset (t_u32 offset, BOOL fTrailing, int *outx, t_u32 *outlineno)
  {
    t_u32		lineno = 0;
    int			xpos = 0;
    t_u32		off_chars;
    //USPDATA	  * uspData;

    // get line information from cursor-offset
    if (m_pTextDoc->lineinfo_from_offset (offset, &lineno, &off_chars, 0, 0, 0) )
    {
      // locate the USPDATA for this line
      //if((uspData = GetUspData(NULL, lineno)) != 0)
      {
        // convert character-offset to x-coordinate
        off_chars = m_nCursorOffset - off_chars;

        if (fTrailing && off_chars > 0)
          UspOffsetToX (/*uspData,*/ lineno, off_chars - 1, TRUE, &xpos);
        else
          UspOffsetToX (/*uspData,*/ lineno, off_chars, FALSE, &xpos);

        // update caret position
        UpdateCaretXY (xpos, lineno);
      }
    }

    if (outx)	  *outx = xpos;

    if (outlineno) *outlineno = lineno;
  }

  void TextView::RepositionCaret()
  {
    UpdateCaretXY (m_nCaretPosX, m_nCurrentLine);
  }

  void TextView::UpdateLine (t_u32 nLineNo)
  {
    m_nPreviousLine = m_nCurrentLine;
    AddDirtyLine (m_nCurrentLine);

    // redraw the old and new lines if they are different
    if (m_nCurrentLine != nLineNo)
    {
      if (CheckStyle (TXS_HIGHLIGHTCURLINE) )
        InvalidateLine (m_nCurrentLine);

      AddDirtyLine (nLineNo);
      m_nCurrentLine = nLineNo;

      if (CheckStyle (TXS_HIGHLIGHTCURLINE) )
        InvalidateLine (m_nCurrentLine);
    }
  }

  void TextView::AddDirtyLine (t_u32 nLineNo)
  {
    static int n = 0;
    std::vector<int>::iterator it;
    it = find (m_DirtyLines.begin(), m_DirtyLines.end(), nLineNo);

    if (it == m_DirtyLines.end() )
    {
      //nuxDebugMsg(TEXT("Set Dirty Line (%d): %d"), n, nLineNo);
      ++n;
      m_DirtyLines.push_back (nLineNo);
    }
  }

//
//	return direction to scroll (+ve, -ve or 0) based on
//  distance of mouse from window edge
//
//	note: counter now redundant, we scroll multiple lines at
//  a time (with a slower timer than before) to achieve
//	variable-speed scrolling
//
  int ScrollDir (int counter, int distance)
  {
    if (distance > 48)		return 5;

    if (distance > 16)		return 2;

    if (distance > 3)		return 1;

    if (distance > 0)		return counter % 5 == 0 ? 1 : 0;

    if (distance < -48)		return -5;

    if (distance < -16)		return -2;

    if (distance < -3)		return -1;

    if (distance < 0)		return counter % 5 == 0 ? -1 : 0;

    return 0;
  }




}
