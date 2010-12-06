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
#include "NuxGraphics/FontTexture.h"

namespace nux
{

  static const TCHAR *CtrlStr (DWORD ch)
  {
    static const TCHAR *reps[] =
    {
      _T ("NUL"), _T ("SOH"), _T ("STX"), _T ("ETX"), _T ("EOT"), _T ("ENQ"), _T ("ACK"), _T ("BEL"),
      _T ("BS"),  _T ("HT"),  _T ("LF"),  _T ("VT"),  _T ("FF"),  _T ("CR"),  _T ("SO"),  _T ("SI"),
      _T ("DLE"), _T ("DC1"), _T ("DC2"), _T ("DC3"), _T ("DC4"), _T ("NAK"), _T ("SYN"), _T ("ETB"),
      _T ("CAN"), _T ("EM"),  _T ("SUB"), _T ("ESC"), _T ("FS"),  _T ("GS"),  _T ("RS"),  _T ("US")
    };

    return ch < _T (' ') ? reps[ch] : _T ("???");
  }

  void PaintRect (HDC hdc, RECT *rect, COLORREF fill)
  {
    fill = SetBkColor (hdc, fill);

    ExtTextOut (hdc, 0, 0, ETO_OPAQUE, rect, 0, 0, 0);

    SetBkColor (hdc, fill);
  }

//
//	Return width of specified control-character
//
  int TextView::CtrlCharWidth (t_u32 chValue, FONT *font)
  {
    SIZE sz;
    const TCHAR *str = CtrlStr (chValue % 32);
    sz.cx = m_TextFont->GetCharStringWidth (str, _tcsclen (str) );
    return sz.cx + 4;
  }

//
//	TextView::
//
  int TextView::NeatTextYOffset (FONT *font)
  {
    return m_nMaxAscent + m_nHeightAbove - font->tm.tmAscent;
  }

//
//	Wrapper for GetTextExtentPoint32. Takes into account
//	control-characters, tabs etc.
//
  int TextView::NeatTextWidth (const TCHAR *buf, int len, int nTabOrigin)
  {
    SIZE	sz;
    int		width = 0;

    const int TABWIDTHPIXELS = TabWidth();

    if (len == -1)
      len = lstrlen (buf);

    for (int i = 0, lasti = 0; i <= len; i++)
    {
      if (i == len || buf[i] == '\t' || (TBYTE) buf[i] < 32)
      {
        sz.cx = m_TextFont->GetCharStringWidth (buf + lasti, i - lasti);
        width += sz.cx;

        if (i < len && buf[i] == '\t')
        {
          int t = (width - nTabOrigin) % TABWIDTHPIXELS;
          width += TABWIDTHPIXELS - ( (width - nTabOrigin) % TABWIDTHPIXELS);
          lasti  = i + 1;
        }
        else if (i < len && (TBYTE) buf[i] < 32)
        {
          width += CtrlCharWidth (buf[i], &m_FontAttr[0]);
          lasti  = i + 1;
        }
      }
    }

    return width;
  }


//
//	Manually calculate the internal-leading and descent
//  values for a font by parsing a small bitmap of a single letter "E"
//	and locating the top and bottom of this letter.
//
  void TextView::InitCtrlCharFontAttr()
  {
// 	// create a temporary off-screen bitmap
// 	HDC		hdcTemp = CreateCompatibleDC(hdc);
// 	HBITMAP hbmTemp = CreateBitmap(font->tm.tmAveCharWidth, font->tm.tmHeight, 1, 1, 0);
// 	HANDLE  hbmOld  = SelectObject(hdcTemp, hbmTemp);
// 	HANDLE  hfnOld	= SelectObject(hdcTemp, font->hFont);
//
// 	// black-on-white text
// 	SetTextColor(RGB(0,0,0));
// 	SetBkColor(RGB(255,255,255));
// 	SetBkMode(hdcTemp,		OPAQUE);
//
// 	TextOut(hdcTemp, 0, 0, _T("E"), 1);
//
// 	// give default values just in case the scan fails
// 	font->nInternalLeading	= font->tm.tmInternalLeading;
// 	font->nDescent			= font->tm.tmDescent;
//
// 	// scan downwards looking for the top of the letter 'E'
// 	for(int y = 0; y < font->tm.tmHeight; y++)
// 	{
// 		for(int x = 0; x < font->tm.tmAveCharWidth; x++)
// 		{
// 			COLORREF col;
//
// 			if((col = GetPixel(hdcTemp, x, y)) == RGB(0,0,0))
// 			{
// 				font->nInternalLeading = y;
// 				y = font->tm.tmHeight;
// 				break;
// 			}
// 		}
// 	}
//
// 	// scan upwards looking for the bottom of the letter 'E'
// 	for(int y = font->tm.tmHeight - 1; y >= 0; y--)
// 	{
// 		for(int x = 0; x < font->tm.tmAveCharWidth; x++)
// 		{
// 			COLORREF col;
//
// 			if((col = GetPixel(hdcTemp, x, y)) == RGB(0,0,0))
// 			{
// 				font->nDescent = font->tm.tmHeight - y - 1;
// 				y = 0;
// 				break;
// 			}
// 		}
// 	}
//
// 	// give larger fonts a thicker border
// 	if(font->nInternalLeading > 1 && font->nDescent > 1 && font->tm.tmHeight > 18)
// 	{
// 		font->nInternalLeading--;
// 		font->nDescent--;
// 	}

// 	// cleanup
// 	SelectObject(hdcTemp, hbmOld);
// 	SelectObject(hdcTemp, hfnOld);
// 	DeleteDC(hdcTemp);
// 	DeleteObject(hbmTemp);
  }

//
//	Display an ASCII control character in inverted colours
//  to what is currently set in the DC
//
  int TextView::PaintCtrlChar (GraphicsEngine &GfxContext, int xpos, int ypos, t_u32 chValue)
  {
    SIZE  sz;
    Geometry  rect;
    const TCHAR *str = CtrlStr (chValue % 32);

    Color fg = GetTextColor();
    Color bg = GetTextBkColor();

    // find out how big the text will be
    sz.cx = m_TextFont->GetCharStringWidth (str);
    sz.cy = 0;

    rect = Geometry (xpos, ypos, sz.cx + 4, m_TextFont->GetFontInfo().FontHeight);

    GetPainter().Paint2DQuadColor (GfxContext, rect, fg);
    GetPainter().PaintTextLineStatic (GfxContext, m_TextFont, rect, str, bg);
    return sz.cx + 4;
  }

//
//	Update the lineheight based on current font settings
//
  VOID TextView::RecalcLineHeight()
  {
    m_nLineHeight	= 0;
    m_nMaxAscent	= 0;

    // find the tallest font in the TextView
    for (int i = 0; i < m_nNumFonts; i++)
    {
      // always include a font's external-leading
      int fontheight = m_TextFont->GetFontInfo().FontHeight + m_TextFont->GetFontInfo().ExternalLeading;
      m_nLineHeight = std::max (m_nLineHeight, fontheight);
      m_nMaxAscent  = std::max<LONG> (m_nMaxAscent, m_TextFont->GetFontInfo().Ascent);
    }

    // add on the above+below spacings
    m_nLineHeight += m_nHeightAbove + m_nHeightBelow;
  }

//
//	Set a font for the TextView
//
  void TextView::SetFont()
  {
    m_nFontWidth = m_TextFont->GetFontInfo().AvgCharWidth;

    // pre-calc the control-characters for this font
    InitCtrlCharFontAttr();

    RecalcLineHeight();
    UpdateMarginWidth();
  }

//
//	Set spacing (in pixels) above and below each line -
//  this is in addition to the external-leading of a font
//
  LONG TextView::SetLineSpacing (int nAbove, int nBelow)
  {
    m_nHeightAbove = nAbove;
    m_nHeightBelow = nBelow;
    RecalcLineHeight();
    return TRUE;
  }

//
//
//
  int TextView::TabWidth()
  {
    return m_nTabWidthChars * m_nFontWidth;
  }
}
