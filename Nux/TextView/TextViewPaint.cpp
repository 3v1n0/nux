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
#include "TextView.h"
#include "TextViewInternal.h"
#include "NuxGraphics/FontRenderer.h"

extern "C" COLORREF MixRGB (COLORREF, COLORREF);

namespace nux
{

//
//	Perform a full redraw of the entire window
//
  VOID TextView::RefreshWindow()
  {
    //InvalidateRect(m_hWnd, NULL, FALSE);
  }

//
//	Painting procedure for TextView objects
//
  LONG TextView::OnPaint (GraphicsEngine &GfxContext)
  {
    t_u32 i, first, last;

    // figure out which lines to redraw
    first = m_nVScrollPos + m_ViewY    / m_nLineHeight;
    last  = m_nVScrollPos + m_ViewHeight / m_nLineHeight;

    // make sure we never wrap around the 4gb boundary
    if (last < first) last = -1;

    // draw the display line-by-line
    GfxContext.PushClippingRectangle (Geometry (m_ViewX /*+ LeftMarginWidth()*/, m_ViewY, m_ViewWidth /*- LeftMarginWidth()*/, m_ViewHeight) );

    for (i = first; i <= last; i++)
    {
      PaintLine (GfxContext, i);
    }

    GfxContext.PopClippingRectangle();

    return 0;
  }

  LONG TextView::OnPaintLine (GraphicsEngine &GfxContext, unsigned int LigneNumber)
  {
    t_u32 first, last;

    // figure out which lines to redraw
    first = m_nVScrollPos + m_ViewY    / m_nLineHeight;
    last  = m_nVScrollPos + m_ViewHeight / m_nLineHeight;

    // make sure we never wrap around the 4gb boundary
    if (last < first) last = -1;

    // draw the display line-by-line
    GfxContext.PushClippingRectangle (Geometry (m_ViewX /*+ LeftMarginWidth()*/, m_ViewY, m_ViewWidth /*- LeftMarginWidth()*/, m_ViewHeight) );
    PaintLine (GfxContext, LigneNumber);
    GfxContext.PopClippingRectangle();

    return 0;
  }

//
//	Draw the specified line (including margins etc)
//
  void TextView::PaintLine (GraphicsEngine &GfxContext, t_u32 nLineNo)
  {
    Geometry  rect;

    // Get the area we want to update
    //GetClientRect(m_hWnd, &rect);

    // calculate rectangle for entire length of line in window
//    rect = Geometry(m_ViewX -m_nHScrollPos * m_nFontWidth, m_ViewY + (nLineNo - m_nVScrollPos) * m_nLineHeight,
//                m_ViewX + m_ViewWidth + (m_nHScrollPos * m_nFontWidth), m_nLineHeight);
    rect = GetTextAreaGeometry();
    rect.OffsetPosition (0, (nLineNo - m_nVScrollPos) * m_nLineHeight);
    rect.SetHeight (m_nLineHeight);
    //rect.OffsetPosition()


// 	rect.left       = (long)(-m_nHScrollPos * m_nFontWidth);
// 	rect.top		= (long)((nLineNo - m_nVScrollPos) * m_nLineHeight);
// 	rect.right		= (long)(rect.right);
// 	rect.bottom		= (long)(rect.top + m_nLineHeight);

    if (!b_RenderToVertexBuffer)
    {
      GetPainter().Paint2DQuadColor (GfxContext, rect, GetTextBkColor() );
    }
    else
    {
      if (m_ColorQuadBuffer0.size() - 2 * m_NumVBColorQuad0 * 16 < 2 * 16)
      {
        // Enlarge the array and preserve the data that it contains.
        m_ColorQuadBuffer0.resize (2 * 16);
      }

      RenderColorQuadToBuffer (
        &m_ColorQuadBuffer0[0] + 2 * m_NumVBColorQuad0 * 16, m_ColorQuadBuffer0.size() - 2 * m_NumVBColorQuad0 * 16,
        rect, GetTextBkColor() );
      ++m_NumVBColorQuad0;
    }

    //
    //	do things like margins, line numbers etc. here
    //
    if (LeftMarginWidth() > 0)
    {
      Geometry margin = rect;
      margin.SetWidth (LeftMarginWidth() );
      PaintMargin (GfxContext, nLineNo, margin);

      rect.OffsetPosition (LeftMarginWidth(), 0);
      rect.OffsetSize (-LeftMarginWidth(), 0);

    }

    rect.OffsetPosition (-m_nHScrollPos * m_nFontWidth, 0);
    rect.OffsetSize ( (m_nHScrollPos * m_nFontWidth), 0);

    //GetPainter().Paint2DQuadColor(GfxContext, rect, GetTextBkColor());

    //
    //	check we have data to draw on this line
    //
    if (nLineNo >= m_nLineCount)
    {
      return;
    }

    //
    //	paint the line's text
    //

    GfxContext.PushClippingRectangle (Geometry (m_ViewX + LeftMarginWidth(), rect.y, m_ViewWidth - LeftMarginWidth(), m_ViewHeight) );
    PaintText (GfxContext, nLineNo, rect);
    GfxContext.PopClippingRectangle();
  }

//
//	Return width of margin
//
  int TextView::LeftMarginWidth()
  {
    int width	= 0;
    int cx		= 20;
    int cy		= 0;

    // get dimensions of imagelist icons
//texttodo:
//  if(m_hImageList)
// 		ImageList_GetIconSize(m_hImageList, &cx, &cy);

    if (CheckStyle (TXS_LINENUMBERS) )
    {
      width += m_nLinenoWidth;

      if (CheckStyle (TXS_SELMARGIN) && cx > 0)
        width += cx;

      return width;
    }
    // selection margin by itself
    else if (CheckStyle (TXS_SELMARGIN) )
    {
      width += cx;
      return width;
    }

    return 0;
  }

//
//	This must be called whenever the number of lines changes
//  (probably easier to call it when the file-size changes)
//
  void TextView::UpdateMarginWidth()
  {
    NString linestr = inlPrintf (LINENO_FMT, m_nLineCount);
    m_nLinenoWidth = NeatTextWidth (linestr.GetTCharPtr(), linestr.Length(), 0);
  }

//
//	Draw the specified line's margin into the area described by *margin*
//
  void TextView::PaintMargin (GraphicsEngine &GfxContext, t_u32 nLineNo, Geometry &margin)
  {
    Geometry rect = margin;
    TCHAR ach[32];

    int imgWidth, imgHeight;
    int imgX, imgY;
    int selwidth = CheckStyle (TXS_SELMARGIN) ? 20 : 0;

    //int nummaxwidth = 60;
    int curx = margin.x;
    int cury = margin.y + margin.GetWidth();

// 	if(m_hImageList && selwidth > 0)
// 	{
// 		// selection margin must include imagelists
//         // texttodo:
//         //ImageList_GetIconSize(m_hImageList, &imgWidth, &imgHeight);
//
// 		imgX = rect.x + (selwidth - imgWidth) / 2;
// 		imgY = rect.y  + (margin.GetHeight() - imgHeight) / 2;
// 	}

    if (!b_RenderToVertexBuffer)
      GetPainter().Paint2DQuadColor (GfxContext, rect, Color (0xFF222222) );
    else
    {
      if (m_ColorQuadBuffer1.size() - 2 * m_NumVBColorQuad1 * 16 < 2 * 16)
      {
        // Enlarge the array and preserve the data that it contains.
        m_ColorQuadBuffer1.resize (2 * 16);
      }

      RenderColorQuadToBuffer (
        &m_ColorQuadBuffer1[0] + 2 * m_NumVBColorQuad1 * 16, m_ColorQuadBuffer1.size() - 2 * m_NumVBColorQuad1 * 16,
        rect, Color (0xFF222222) );
      ++m_NumVBColorQuad1;
    }

    if (CheckStyle (TXS_LINENUMBERS) )
    {
      rect  = margin;

      //HANDLE hOldFont = SelectObject(hdc, m_FontAttr[0].hFont);

      int  len   = wsprintf (ach, LINENO_FMT, nLineNo + 1);
      int	 width = NeatTextWidth (ach, len, 0);

      // only draw line number if in-range
      if (nLineNo >= m_nLineCount)
        len = 0;

      //rect.OffsetPosition(m_nLinenoWidth - width, 0);
      rect.SetWidth (m_nLinenoWidth);

      if (!b_RenderToVertexBuffer)
      {
        GetPainter().Paint2DQuadColor (GfxContext, rect, Color (0xFF111111) );
        GetPainter().PaintTextLineStatic (GfxContext, m_TextFont, rect, ach, Color (0xFFFFFFFF), true, eAlignTextRight);
      }
      else
      {
        int num_char = StringLength (ach);

        if (m_TextVertexBuffer1.capacity() < 3 * num_char * 16)
        {
          // Enlarge the array and preserve the data that it contains.
          m_TextVertexBuffer1.resize (3 * num_char * 16);
        }

        int n = m_TextFontRenderer->RenderTextToBuffer (
                  &m_TextVertexBuffer1[0] + 3 * m_NumVBQuad1 * 16, m_TextVertexBuffer1.size() - 3 * m_NumVBQuad1 * 16,
                  m_TextFont, rect, ach, Color (0xFFFFFFFF), eAlignTextRight);
        m_NumVBQuad1 += n;


        if (m_ColorQuadBuffer1.size() - 2 * m_NumVBColorQuad1 * 16 < 2 * 16)
        {
          // Enlarge the array and preserve the data that it contains.
          m_ColorQuadBuffer1.resize (2 * 16);
        }

        RenderColorQuadToBuffer (
          &m_ColorQuadBuffer1[0] + 2 * m_NumVBColorQuad1 * 16, m_ColorQuadBuffer1.size() - 2 * m_NumVBColorQuad1 * 16,
          rect, Color (0xFF111111) );
        ++m_NumVBColorQuad1;
      }

// 		if(CheckStyle(TXS_SELMARGIN) && m_hImageList)
// 		{
// 			imgX = rect.right;
// 			rect.right += imgWidth + 4;
// 		}

// 		SetTextColor(0xFFFFFFFF /*GetColour(TXC_LINENUMBERTEXT)*/);
// 		SetBkColor(GetColour(TXC_LINENUMBER));
//
// 		ExtTextOut(	hdc,
// 					rect.left + m_nLinenoWidth - width,
// 					rect.top  + NeatTextYOffset(&m_FontAttr[0]),
// 					ETO_OPAQUE | ETO_CLIPPED,
// 					&rect,
// 					ach,
// 					len,
// 					0);

// 		// vertical line
// 		rect.left   = rect.right;
// 		rect.right += 1;
// 		//PaintRect(hdc, &rect, MixRGB(GetSysColor(COLOR_3DFACE), 0xffffff));
// 		PaintRect(hdc, &rect, GetColour(TXC_BACKGROUND));

      // bleed area - use this to draw "folding" arrows
      /*	rect.left   = rect.right;
      	rect.right += 5;
      	PaintRect(hdc, &rect, GetColour(TXC_BACKGROUND));*/

      //SelectObject(hdc, hOldFont);
    }
    else
    {
      //DrawCheckedRect(hdc, &rect, GetColour(TXC_SELMARGIN1), GetColour(TXC_SELMARGIN2));
    }

    //
    //	Retrieve information about this specific line
    //
    LINEINFO *linfo = GetLineInfo (nLineNo);

// 	if(m_hImageList && linfo)
// 	{
//         // texttodo:
// // 		ImageList_DrawEx(
// // 					  m_hImageList,
// // 					  linfo->nImageIdx,
// // 					  hdc,
// // 					  imgX,
// // 					  imgY,
// // 					  imgWidth,
// // 					  imgHeight,
// // 					  CLR_NONE,
// // 					  CLR_NONE,
// // 					  ILD_TRANSPARENT
// // 					  );
// 	}
//
// 	return rect.right-rect.left;
    //return 0;
  }

//
//	Draw a line of text into the TextView window
//
  void TextView::PaintText (GraphicsEngine &GfxContext, t_u32 nLineNo, Geometry &rect)
  {
    TCHAR		buff[TEXTBUFSIZE];
    ATTR		attr[TEXTBUFSIZE];

    t_u32		off_chars;

    int			xpos  = rect.x;
    int			ypos  = rect.y;
    int			xtab  = rect.x;

    t_u32		colno = 0;
    int			len;

    //
    //	TODO: Clip text to left side of window
    //

    TextIterator itor = m_pTextDoc->iterate_line (nLineNo, &off_chars);

    //
    //	Keep drawing until we reach the edge of the window
    //
    while ( (len = itor.gettext (buff, TEXTBUFSIZE) ) > 0)
    {
      //
      //	Apply text attributes -
      //	i.e. syntax highlighting, mouse selection colours etc.
      //
      len = ApplyTextAttributes (nLineNo, off_chars, colno, buff, len, attr);

      if (len == 0)
        Sleep (0);

      //
      //	Display the text by breaking it into spans of colour/style
      //
      for (int i = 0, lasti = 0; i <= len; i++)
      {
        // if the colour or font changes, then need to output
        if (i == len ||
            attr[i].fg		!= attr[lasti].fg		||
            attr[i].bg		!= attr[lasti].bg		||
            attr[i].style	!= attr[lasti].style)
        {
          xpos += NeatTextOut (GfxContext, xpos, ypos, buff + lasti, i - lasti, xtab, &attr[lasti]);

          lasti = i;
        }
      }

      off_chars += len;
    }

    //
    // Erase to the end of the line
    //
    //rect.OffsetPosition(xpos, 0);
    //rect.OffsetSize(xpos, 0);
    //GetPainter().Paint2DQuadColor(GfxContext, rect, GetTextBkColor());
  }

//
//	Apply visual-styles to the text by returning colour and font
//	information into the supplied TEXT_ATTR structure
//
//	nLineNo	- line number
//	nOffset	- actual offset of line within file
//
//	Returns new length of buffer if text has been modified
//
  int TextView::ApplyTextAttributes (t_u32 nLineNo, t_u32 nOffset, t_u32 &nColumn, TCHAR *szText, int nTextLen, ATTR *attr)
  {
    int	font	= nLineNo % m_nNumFonts;
    int i;

    t_u32 selstart = std::min (m_nSelectionStart, m_nSelectionEnd);
    t_u32 selend   = std::max (m_nSelectionStart, m_nSelectionEnd);

    //
    //	STEP 1. Apply the "base coat"
    //
    for (i = 0; i < nTextLen; i++)
    {
      // change the background if the line is too long
      if (nColumn >= (t_u32) m_nLongLineLimit && CheckStyle (TXS_LONGLINES) )
      {
        attr[i].fg = m_rgbColourList[TXC_FOREGROUND];
        attr[i].bg = LongColour (nLineNo);
      }
      else
      {
        attr[i].fg = m_rgbColourList[TXC_FOREGROUND];
        attr[i].bg = LineColour (nLineNo); //GetColour(TXC_BACKGROUND);
      }

      // keep track of how many columns we have processed
      if (szText[i] == '\t')
        nColumn += m_nTabWidthChars - (nColumn % m_nTabWidthChars);
      else
        nColumn += 1;
    }

    //
    //	TODO: 1. Apply syntax colouring first of all
    //

    //
    //	TODO: 2. Apply bookmarks, line highlighting etc (override syntax colouring)
    //

    //
    //	STEP 3:  Now apply text-selection (overrides everything else)
    //
    for (i = 0; i < nTextLen; i++)
    {
      // apply highlight colours
      if (nOffset + i >= selstart && nOffset + i < selend)
      {
        if (GetFocus() == m_hWnd)
        {
          attr[i].fg = m_rgbColourList[TXC_HIGHLIGHTTEXT];
          attr[i].bg = m_rgbColourList[TXC_HIGHLIGHT];
        }
        else
        {
          attr[i].fg = m_rgbColourList[TXC_HIGHLIGHTTEXT2];
          attr[i].bg = m_rgbColourList[TXC_HIGHLIGHT2];
        }
      }

      // just an example of how to set the font
      if (szText[i] == ' ')
        font = (font + 1) % m_nNumFonts;

      attr[i].style = font;
    }

    //
    //	Turn any CR/LF at the end of a line into a single 'space' character
    //
    return StripCRLF (szText, nTextLen, false);
  }

//
//	Draw tabbed text using specified colours and font, return width of output text
//
//	We could just use TabbedTextOut - however, we need to parse the text because
//  it might contain ascii-control characters which must be output separately.
//  because of this we'll just handle the tabs at the same time.
//
  int TextView::NeatTextOut (GraphicsEngine &GfxContext, int xpos, int ypos, TCHAR *szText, int nLen, int nTabOrigin, ATTR *attr)
  {
    int   i;
    int   xold = xpos;
    int   lasti = 0;
    SIZE  sz;

    const int TABWIDTHPIXELS = m_nTabWidthChars * m_nFontWidth;

    FONT *font = &m_FontAttr[attr->style];

    DWORD flag;


    // loop over each character
    for (i = 0; i <= nLen; )
    {
      int  yoff = NeatTextYOffset (font);

      // output any "deferred" text before handling tab/control chars
      if (i == nLen || szText[i] == '\t' || (TBYTE) szText[i] < 32)
      {
        Geometry rect;

        // get size of text
        sz.cx = m_TextFont->GetCharStringWidth (szText + lasti, i - lasti);
        rect = Geometry (xpos, ypos, sz.cx, m_TextFont->GetFontInfo().FontHeight);

        // draw the text and erase it's background at the same time
        NString text = szText + lasti;
        text = text.GetSubString (0, i - lasti);

        if (!b_RenderToVertexBuffer)
        {
          GetPainter().Paint2DQuadColor (GfxContext, rect, Color (attr->bg) );
          GetPainter().PaintTextLineStatic (GfxContext, m_TextFont, rect, text.GetTCharPtr(), GetTextColor() );
        }
        else
        {
          int num_char = text.Size();

          if (m_TextVertexBuffer0.capacity() < 3 * num_char * 16)
          {
            // Enlarge the array and preserve the data that it contains.
            m_TextVertexBuffer0.resize (3 * num_char * 16);
          }

          int n = m_TextFontRenderer->RenderTextToBuffer (
                    &m_TextVertexBuffer0[0] + 3 * m_NumVBQuad0 * 16, m_TextVertexBuffer0.size() - 3 * m_NumVBQuad0 * 16,
                    m_TextFont, rect, text.GetTCharPtr(), GetTextColor(), eAlignTextRight);
          m_NumVBQuad0 += n;

          if (m_ColorQuadBuffer0.size() - 2 * m_NumVBColorQuad0 * 16 < 2 * 16)
          {
            // Enlarge the array and preserve the data that it contains.
            m_ColorQuadBuffer0.resize (2 * 16);
          }

          RenderColorQuadToBuffer (
            &m_ColorQuadBuffer0[0] + 2 * m_NumVBColorQuad0 * 16, m_ColorQuadBuffer0.size() - 2 * m_NumVBColorQuad0 * 16,
            rect, Color (attr->bg) );
          ++m_NumVBColorQuad0;

        }

        xpos += sz.cx;
      }

      // special case for TAB and Control characters
      if (i < nLen)
      {
        // TAB characters
        if (szText[i] == '\t')
        {
          // calculate distance in pixels to the next tab-stop
          int width = TABWIDTHPIXELS - ( (xpos - nTabOrigin) % TABWIDTHPIXELS);

          // draw a blank space
          GetPainter().Paint2DQuadColor (GfxContext, xpos, ypos, width, m_TextFont->GetFontInfo().FontHeight, GetTextBkColor() );

          xpos += width;
          lasti = i + 1;
        }
        // ASCII-CONTROL characters
        else if ( (TBYTE) szText[i] < 32)
        {
          xpos += PaintCtrlChar (GfxContext, xpos, ypos, szText[i]);
          lasti = i + 1;
        }
      }

      i++;
    }

    // return the width of text drawn
    return xpos - xold;
  }

  void TextView::PaintRect (GraphicsEngine &GfxContext, int x, int y, int width, int height, COLORREF fill)
  {
    GetPainter().Paint2DQuadColor (GfxContext, x, y, width, height/*, m_TextFont->GetFontInfo().FontHeight*/, GetTextBkColor() );
  }

//
//	Strip CR/LF combinations from the end of a line and
//  replace with a single space character (for drawing purposes)
//
  int TextView::StripCRLF (TCHAR *szText, int nLength, bool fAllow)
  {
    if (nLength >= 2)
    {
      if (szText[nLength-2] == '\r' && szText[nLength-1] == '\n')
      {
        if (m_nCRLFMode & TXL_CRLF)
        {
          // convert CRLF to a single space
          szText[nLength-2] = ' ';
          return nLength - 1 - (int) fAllow;
        }
        else
        {
          return nLength;
        }
      }
    }

    if (nLength >= 1)
    {
      if (szText[nLength-1] == '\r' && (m_nCRLFMode & TXL_CR) ||
          szText[nLength-1] == '\n' && (m_nCRLFMode & TXL_LF)   )
      {
        // convert CR or LF to a single space
        szText[nLength-1] = ' ';
        return nLength - (int) fAllow;
      }
    }

    return nLength;
  }

//
//
//
  COLORREF TextView::LineColour (t_u32 nLineNo)
  {
    if (m_nCurrentLine == nLineNo && CheckStyle (TXS_HIGHLIGHTCURLINE) )
      return m_rgbColourList[TXC_CURRENTLINE];
    else
      return m_rgbColourList[TXC_BACKGROUND];
  }

  COLORREF TextView::LongColour (t_u32 nLineNo)
  {
    if (m_nCurrentLine == nLineNo && CheckStyle (TXS_HIGHLIGHTCURLINE) )
      return m_rgbColourList[TXC_CURRENTLINE];
    else
      return m_rgbColourList[TXC_LONGLINE];
  }

  COLORREF MixRGB (COLORREF rgbCol1, COLORREF rgbCol2)
  {
    return RGB (
             (GetRValue (rgbCol1) + GetRValue (rgbCol2) ) / 2,
             (GetGValue (rgbCol1) + GetGValue (rgbCol2) ) / 2,
             (GetBValue (rgbCol1) + GetBValue (rgbCol2) ) / 2
           );
  }

  COLORREF RealizeColour (COLORREF col)
  {
    COLORREF result = col;

    if (col & 0x80000000)
      result = GetSysColor (col & 0xff);

    if (col & 0x40000000)
      result = MixRGB (GetSysColor ( (col & 0xff00) >> 8), result);

    if (col & 0x20000000)
      result = MixRGB (GetSysColor ( (col & 0xff00) >> 8), result);

    return result;
  }

//
//	Return an RGB value corresponding to the specified HVC_xxx index
//
//	If the RGB value has the top bit set (0x80000000) then it is
//  not a real RGB value - instead the low 29bits specify one
//  of the GetSysColor COLOR_xxx indices. This allows us to use
//	system colours without worrying about colour-scheme changes etc.
//
  COLORREF TextView::GetColour (UINT idx)
  {
    if (idx >= TXC_MAX_COLOURS)
      return 0;

    return REALIZE_SYSCOL (m_rgbColourList[idx]);
  }

  COLORREF TextView::SetColour (UINT idx, COLORREF rgbColour)
  {
    COLORREF rgbOld;

    if (idx >= TXC_MAX_COLOURS)
      return 0;

    //if(idx == 13)
    //	__asm int 3;

    rgbOld				 = m_rgbColourList[idx];
    m_rgbColourList[idx] = rgbColour;

    return rgbOld;
  }

//
//	Paint a checkered rectangle, with each alternate
//	pixel being assigned a different colour
//
  void TextView::DrawCheckedRect (GraphicsEngine &GfxContext, RECT *rect, COLORREF fg, COLORREF bg)
  {
// 	static WORD wCheckPat[8] =
// 	{
// 		0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555
// 	};
//
// 	HBITMAP hbmp;
// 	HBRUSH  hbr, hbrold;
// 	COLORREF fgold, bgold;
//
// 	hbmp = CreateBitmap(8, 8, 1, 1, wCheckPat);
// 	hbr  = CreatePatternBrush(hbmp);
//
// 	SetBrushOrgEx(hdc, rect->left, 0, 0);
// 	hbrold = (HBRUSH)SelectObject(hdc, hbr);
//
// 	fgold = SetTextColor(hdc, fg);
// 	bgold = SetBkColor(hdc, bg);
//
// 	PatBlt(hdc, rect->left, rect->top,
// 				rect->right - rect->left,
// 				rect->bottom - rect->top,
// 				PATCOPY);
//
// 	SetBkColor(hdc, bgold);
// 	SetTextColor(hdc, fgold);
//
// 	SelectObject(hdc, hbrold);
// 	DeleteObject(hbr);
// 	DeleteObject(hbmp);
  }


  void TextView::RenderColorQuadToBuffer (float *VertexBuffer, int VBSize, Rect geo, Color color)
  {
    Vector4 *Position = (Vector4 *) VertexBuffer;

    {

      // Position upper left
      //                            Scale       Offset
      Position[0].x = 0.0f  * geo.GetWidth()     + geo.x;    // x
      Position[0].y = 0.0f  * geo.GetHeight()    + geo.y;           // y
      Position[0].z = 0.0f  * 1.0f      + 0.0f;             // z
      Position[0].w = 1.0f  * 1.0f      + 0.0f;             // w
      // color
      Position[1].x = color.R();
      Position[1].y = color.G();
      Position[1].z = color.B();
      Position[1].w = color.A();

      // Position lower left
      Position[2].x = 0.0f  * geo.GetWidth()     + geo.x;    // x
      Position[2].y = 1.0f  * geo.GetHeight()    + geo.y;           // y
      Position[2].z = 0.0f  * 1.0f      + 0.0f;             // z
      Position[2].w = 1.0f  * 1.0f      + 0.0f;             // w
      // color
      Position[3].x = color.R();
      Position[3].y = color.G();
      Position[3].z = color.B();
      Position[3].w = color.A();

      // Position lower right
      Position[4].x = 1.0f  * geo.GetWidth()     + geo.x;    // x
      Position[4].y = 1.0f  * geo.GetHeight()    + geo.y;           // y
      Position[4].z = 0.0f  * 1.0f      + 0.0f;             // z
      Position[4].w = 1.0f  * 1.0f      + 0.0f;             // w
      // color
      Position[5].x = color.R();
      Position[5].y = color.G();
      Position[5].z = color.B();
      Position[5].w = color.A();

      // Position upper right
      Position[6].x = 1.0f  * geo.GetWidth()     + geo.x;    // x
      Position[6].y = 0.0f  * geo.GetHeight()    + geo.y;           // y
      Position[6].z = 0.0f  * 1.0f      + 0.0f;             // z
      Position[6].w = 1.0f  * 1.0f      + 0.0f;             // w
      // color
      Position[7].x = color.R();
      Position[7].y = color.G();
      Position[7].z = color.B();
      Position[7].w = color.A();
    }
  }

}
