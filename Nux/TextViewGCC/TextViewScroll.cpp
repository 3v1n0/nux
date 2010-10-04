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


#include "Basic/NKernel.h"
#include "TextView.h"
#include "TextViewInternal.h"
#include "HScrollBar.h"
#include "VScrollBar.h"

//
//	Set scrollbar positions and range
//
void TextView::SetupScrollbars()
{
  SCROLLINFO si = { sizeof (si) };

  si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;

  //
  //	Vertical scrollbar
  //
  si.nPos  = m_nVScrollPos;		// scrollbar thumb position
  si.nPage = m_nWindowLines;		// number of lines in a page
  si.nMin  = 0;
  si.nMax  = m_nLineCount - 1;	// total number of lines in file

  //SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
  vscrollbar->SetContainerSize (
    m_ViewX,        //GetBaseX() + m_border + m_ViewContentLeftMargin,
    m_ViewY,        //GetBaseY() + m_top_border + m_ViewContentTopMargin,
    m_ViewWidth,    //GetBaseWidth() - ScrollbarWidth - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
    m_ViewHeight    //GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin
  );
  vscrollbar->SetContentSize (
    m_ViewX,                            //m_compositionLayout->GetBaseX(),
    m_ViewY,                            //m_compositionLayout->GetBaseY(),
    m_nLongestLine * m_nFontWidth,      //m_compositionLayout->GetBaseWidth(),
    (m_nLineCount) *m_nLineHeight   //m_compositionLayout->GetBaseHeight()
  );

  //
  //	Horizontal scrollbar
  //
  si.nPos  = m_nHScrollPos;		// scrollbar thumb position
  si.nPage = m_nWindowColumns;	// number of lines in a page
  si.nMin  = 0;
  si.nMax  = m_nLongestLine - 1;	// total number of lines in file

  //SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
  hscrollbar->SetContainerSize (
    m_ViewX,        //GetBaseX() + m_border + m_ViewContentLeftMargin,
    m_ViewY,        //GetBaseY() + m_top_border + m_ViewContentTopMargin,
    m_ViewWidth - LeftMarginWidth(),    //GetBaseWidth() - ScrollbarWidth - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
    m_ViewHeight    //GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin
  );
  hscrollbar->SetContentSize (
    m_ViewX,                            //m_compositionLayout->GetBaseX(),
    m_ViewY,                            //m_compositionLayout->GetBaseY(),
    m_nLongestLine * m_nFontWidth,      //m_compositionLayout->GetBaseWidth(),
    (m_nLineCount) *m_nLineHeight   //m_compositionLayout->GetBaseHeight()
  );

  vscrollbar->SetScrollUnit (m_nLineHeight);
  hscrollbar->SetScrollUnit (m_nFontWidth);

  if ( (m_ViewX + m_ContentOffsetX +  (m_nLongestLine * m_nFontWidth) < m_ViewX + m_ViewWidth) && (m_ContentOffsetX < 0) )
  {
    m_ContentOffsetX = - ( (m_nLongestLine * m_nFontWidth) > m_ViewWidth - LeftMarginWidth() ? (m_nLongestLine * m_nFontWidth) - m_ViewWidth + LeftMarginWidth() : 0);
    m_nHScrollPos = inl::Abs (m_ContentOffsetX / m_nFontWidth) + 1;
  }
  else
  {
    m_nHScrollPos = inl::Abs (m_ContentOffsetX / m_nFontWidth);
  }

  // round to a multiple of m_nFontWidth
  //m_ContentOffsetX = (m_ContentOffsetX / m_nFontWidth) * m_nFontWidth;

  if ( (m_ViewY + m_ContentOffsetY + (m_nLineHeight * m_nLineCount) < m_ViewY + m_ViewHeight) && (m_ContentOffsetY < 0) )
  {
    m_ContentOffsetY = - ( (m_nLineHeight * m_nLineCount) > m_ViewHeight ? (m_nLineHeight * m_nLineCount) - m_ViewHeight : 0);
    m_nVScrollPos = inl::Abs (m_ContentOffsetY / m_nLineHeight);
  }
  else
  {
    m_nVScrollPos = inl::Abs (m_ContentOffsetY / m_nLineHeight);
  }

  // round to a multiple of m_nLineHeight
  //m_ContentOffsetY = (m_ContentOffsetY / m_nLineHeight) * m_nLineHeight;

  vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
  hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);

//     m_nVScrollPos = inl::Abs(m_ContentOffsetY / m_nLineHeight);
//     m_nHScrollPos = inl::Abs(m_ContentOffsetX / m_nFontWidth);

  //inlDebugMsg(TEXT("HScrollPos = %d"), m_nHScrollPos);
  //inlDebugMsg(TEXT("VScrollPos = %d"), m_nVScrollPos);

  // adjust our interpretation of the max scrollbar range to make
  // range-checking easier. The scrollbars don't use these values, they
  // are for our own use.
  m_nVScrollMax = m_nLineCount   - m_nWindowLines;
  m_nHScrollMax = m_nLongestLine - m_nWindowColumns;
}

//
//	Ensure that we never scroll off the end of the file
//
bool TextView::PinToBottomCorner()
{
  bool repos = false;

  if (m_nHScrollPos + m_nWindowColumns > m_nLongestLine)
  {
    m_nHScrollPos = m_nLongestLine - m_nWindowColumns;
    repos = true;
  }

  if (m_nVScrollPos + m_nWindowLines > m_nLineCount)
  {
    m_nVScrollPos = m_nLineCount - m_nWindowLines;
    repos = true;
  }

  return repos;
}

//
//	The window has changed size - update the scrollbars
//
long TextView::OnSize (UINT nFlags, int width, int height)
{
  int margin = LeftMarginWidth();

  m_nWindowLines   = std::min<int> ( (unsigned) height		/ m_nLineHeight, m_nLineCount);
  m_nWindowColumns = std::min<int> ( (width - margin)		/ m_nFontWidth,  m_nLongestLine);

  if (PinToBottomCorner() )
  {
    RefreshWindow();
    RepositionCaret();
  }

  SetupScrollbars();

  return 0;
}

// //
// //	ScrollRgn
// //
// //	Scrolls the viewport in specified direction. If fReturnUpdateRgn is true,
// //	then a HRGN is returned which holds the client-region that must be redrawn
// //	manually. This region must be deleted by the caller using DeleteObject.
// //
// //  Otherwise ScrollRgn returns NULL and updates the entire window
// //
// HRGN TextView::ScrollRgn(int dx, int dy, bool fReturnUpdateRgn)
// {
// 	RECT clip;
//
// 	GetClientRect(m_hWnd, &clip);
//
// 	//
// 	// make sure that dx,dy don't scroll us past the edge of the document!
// 	//
//
// 	// scroll up
// 	if(dy < 0)
// 	{
// 		dy = -(int)std::min(-dy, m_nVScrollPos);
// 		clip.top = -dy * m_nLineHeight;
// 	}
// 	// scroll down
// 	else if(dy > 0)
// 	{
// 		dy = std::min(dy, m_nVScrollMax-m_nVScrollPos);
// 		clip.bottom = (m_nWindowLines -dy) * m_nLineHeight;
// 	}
//
//
// 	// scroll left
// 	if(dx < 0)
// 	{
// 		dx = -(int)std::min(-dx, m_nHScrollPos);
// 		clip.left = -dx * m_nFontWidth * 4;
// 	}
// 	// scroll right
// 	else if(dx > 0)
// 	{
// 		dx = std::min(dx, m_nHScrollMax-m_nHScrollPos);
// 		clip.right = (m_nWindowColumns - dx - 4) * m_nFontWidth ;
// 	}
//
// 	// adjust the scrollbar thumb position
// 	m_nHScrollPos += dx;
// 	m_nVScrollPos += dy;
//
// 	// ignore clipping rectangle if its a whole-window scroll
// 	if(fReturnUpdateRgn == false)
// 		GetClientRect(m_hWnd, &clip);
//
// 	// take margin into account
// 	clip.left += LeftMarginWidth();
//
// 	// perform the scroll
// 	if(dx != 0 || dy != 0)
// 	{
// 		// do the scroll!
// 		ScrollWindowEx(
// 			m_hWnd,
// 			-dx * m_nFontWidth,					// scale up to pixel coords
// 			-dy * m_nLineHeight,
// 			NULL,								// scroll entire window
// 			&clip,								// clip the non-scrolling part
// 			0,
// 			0,
// 			SW_INVALIDATE
// 			);
//
// 		SetupScrollbars();
// 	}
//
// 	if(dy != 0)
// 	{
// 		GetClientRect(m_hWnd, &clip);
// 		clip.right = LeftMarginWidth();
// 		//ScrollWindow(m_hWnd, 0, -dy * m_nLineHeight, 0, &clip);
// 		InvalidateRect(m_hWnd, &clip, 0);
// 	}
//
// 	return NULL;
// }

//
//	Scroll viewport in specified direction
//
// void TextView::Scroll(int dx, int dy)
// {
// 	// do a "normal" scroll - don't worry about invalid regions,
// 	// just scroll the whole window
// 	ScrollRgn(dx, dy, false);
// }
//
// inl::t_u32 GetTrackPos32(HWND hwnd, int nBar)
// {
// 	SCROLLINFO si = { sizeof(si), SIF_TRACKPOS };
// 	GetScrollInfo(hwnd, nBar, &si);
// 	return si.nTrackPos;
// }

// //
// //	Vertical scrollbar support
// //
// long TextView::OnVScroll(UINT nSBCode, UINT nPos)
// {
// 	inl::t_u32 oldpos = m_nVScrollPos;
//
// 	switch(nSBCode)
// 	{
// 	case SB_TOP:
// 		m_nVScrollPos = 0;
// 		RefreshWindow();
// 		break;
//
// 	case SB_BOTTOM:
// 		m_nVScrollPos = m_nVScrollMax;
// 		RefreshWindow();
// 		break;
//
// 	case SB_LINEUP:
// 		Scroll(0, -1);
// 		break;
//
// 	case SB_LINEDOWN:
// 		Scroll(0, 1);
// 		break;
//
// 	case SB_PAGEDOWN:
// 		Scroll(0, m_nWindowLines);
// 		break;
//
// 	case SB_PAGEUP:
// 		Scroll(0, -m_nWindowLines);
// 		break;
//
// 	case SB_THUMBPOSITION:
// 	case SB_THUMBTRACK:
//
// 		m_nVScrollPos = GetTrackPos32(m_hWnd, SB_VERT);
// 		RefreshWindow();
//
// 		break;
// 	}
//
// 	if(oldpos != m_nVScrollPos)
// 	{
// 		SetupScrollbars();
// 		RepositionCaret();
// 	}
//
//
// 	return 0;
// }

// //
// //	Horizontal scrollbar support
// //
// long TextView::OnHScroll(UINT nSBCode, UINT nPos)
// {
// 	int oldpos = m_nHScrollPos;
//
// 	switch(nSBCode)
// 	{
// 	case SB_LEFT:
// 		m_nHScrollPos = 0;
// 		RefreshWindow();
// 		break;
//
// 	case SB_RIGHT:
// 		m_nHScrollPos = m_nHScrollMax;
// 		RefreshWindow();
// 		break;
//
// 	case SB_LINELEFT:
// 		Scroll(-1, 0);
// 		break;
//
// 	case SB_LINERIGHT:
// 		Scroll(1, 0);
// 		break;
//
// 	case SB_PAGELEFT:
// 		Scroll(-m_nWindowColumns, 0);
// 		break;
//
// 	case SB_PAGERIGHT:
// 		Scroll(m_nWindowColumns, 0);
// 		break;
//
// 	case SB_THUMBPOSITION:
// 	case SB_THUMBTRACK:
//
// 		m_nHScrollPos = GetTrackPos32(m_hWnd, SB_HORZ);
// 		RefreshWindow();
// 		break;
// 	}
//
// 	if(oldpos != m_nHScrollPos)
// 	{
// 		SetupScrollbars();
// 		RepositionCaret();
// 	}
//
// 	return 0;
// }

long TextView::OnMouseWheelFunc (int nDelta)
{
#ifndef	SPI_GETWHEELSCROLLLINES
#define SPI_GETWHEELSCROLLLINES   104
#endif

  int nScrollLines;

  SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);

  if (nScrollLines <= 1)
    nScrollLines = 3;

  Scroll (0, (-nDelta / 120) * nScrollLines);
  RepositionCaret();

  return 0;
}
