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


void TextView::UspSnapXToOffset(
                                int     lineno,
                                int     xpos,			
                                int     *snappedX,		// out, optional
                                inl::t_u32 *CharacterColumn,		// out
                                bool    *fRTL			// out, optional
                              )
{
    TCHAR Character;
    inl::t_u32 CharFileOffset;
    bool  Trailing;

    UspXToOffset(
        lineno,
        xpos,
        snappedX,
        CharacterColumn,
        &CharFileOffset,
        &Character,
        &Trailing
        );

}

void TextView::UspXToOffset(
                            int     lineno,
                            int     xpos,
                            int     *px,			    // [out] adjusted x coord of caret
                            inl::t_u32   *CharacterColumn,   // out
                            inl::t_u32	*pnFileOffset,      // [out] zero-based file-offset (in chars)
                            TCHAR   *Character,     
                            bool    *trailing           // out
                            )
{
    inl::t_u32 off_chars;
    TextIterator itor = m_pTextDoc->iterate_line(lineno, &off_chars);

    int len;
    int	curx = 0;
    TCHAR TextLineBuffer[TEXTBUFSIZE];
    // character offset within the line is more complicated. 
    // We have to parse the text to work out the exact character which
    // falls under the mouse x-coordinate
    while((len = itor.gettext(TextLineBuffer, TEXTBUFSIZE)) > 0)
    {
        len = StripCRLF(TextLineBuffer, len, true);

        // find its width
        int width = NeatTextWidth(TextLineBuffer, len, -(curx % TabWidth()));

        // does cursor fall within this segment?
        if((xpos >= curx) && (xpos < curx + width))
        { 
            //
            //	We have a block of text, with the mouse 
            //  somewhere in the middle. Perform a "binary chop" to
            //  locate the exact character that the mouse is positioned over
            //
            int low   = 0;
            int high  = len;
            int lowx  = 0;
            int highx = width;

            while(low < high - 1)
            {
                int newlen   = (high - low) / 2;

                width = NeatTextWidth(TextLineBuffer + low, newlen, -lowx-curx);

                if(xpos-curx < width + lowx)
                {
                    high  = low + newlen;
                    highx = lowx + width;
                }
                else
                {
                    low   = low + newlen;
                    lowx  = lowx + width;
                }
            }

            // base coordinates on center of characters, not the edges
            if(xpos - curx > highx - m_nFontWidth/2)
            {
                curx       += highx;
                off_chars  += high;
            }
            else
            {
                curx       += lowx;
                off_chars  += low;
            }

            break;
        }
        else
        {
            curx	  += width;
            off_chars += len;
        }
    }

    *pnFileOffset       = off_chars;
    *px                 = curx; // - m_nHScrollPos * m_nFontWidth;
    *CharacterColumn    = *px / m_nFontWidth;
    *Character          = *(TextLineBuffer + *CharacterColumn);
    *trailing           = false;
}

bool TextView::UspOffsetToX(
                            int     lineno,
                            int     charPos, 
                            bool    trailing,
                            int     *px
                           )
{
    *px = charPos * m_nFontWidth;
    return true;
}




