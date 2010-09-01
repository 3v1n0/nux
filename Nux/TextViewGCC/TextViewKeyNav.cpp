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

//
//	Move caret up specified number of lines
//
void TextView::MoveLineUp(int numLines)
{
//     USPDATA			* uspData;
//     ULONG			  lineOffset;
// 
//     int				  charPos;
//     BOOL			  trailing;
// 
//     m_nCurrentLine -= min(m_nCurrentLine, (unsigned)numLines);
// 
//     // get Uniscribe data for prev line
//     uspData = GetUspData(0, m_nCurrentLine, &lineOffset);
// 
//     // move up to character nearest the caret-anchor positions
//     UspXToOffset(uspData, m_nAnchorPosX, &charPos, &trailing, 0);
// 
//     m_nCursorOffset = lineOffset + charPos + trailing;
}

//
//	Move caret down specified number of lines
//
// void TextView::MoveLineDown(int numLines)
// {
//     USPDATA			* uspData;
//     ULONG			  lineOffset;
// 
//     int				  charPos;
//     BOOL			  trailing;
// 
//     m_nCurrentLine += min(m_nLineCount-m_nCurrentLine-1, (unsigned)numLines);
// 
//     // get Uniscribe data for prev line
//     uspData = GetUspData(0, m_nCurrentLine, &lineOffset);
// 
//     // move down to character nearest the caret-anchor position
//     UspXToOffset(uspData, m_nAnchorPosX, &charPos, &trailing, 0);
// 
//     m_nCursorOffset = lineOffset + charPos + trailing;
// }

