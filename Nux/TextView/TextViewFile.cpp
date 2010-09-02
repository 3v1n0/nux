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

NAMESPACE_BEGIN_GUI

LONG TextView::OpenFile(TCHAR *szFileName)
{
	ClearFile();

	if(m_pTextDoc->init(szFileName))
	{
		m_nLineCount   = m_pTextDoc->linecount();
		m_nLongestLine = m_pTextDoc->longestline(4);

		m_nVScrollPos  = 0;
		m_nHScrollPos  = 0;

		m_nSelectionStart	= 0;
		m_nSelectionEnd		= 0;
		m_nCursorOffset		= 0;

		UpdateMetrics();
		UpdateMarginWidth();
		return TRUE;
	}

	return FALSE;
}

LONG TextView::ClearFile()
{
	if(m_pTextDoc)
		m_pTextDoc->clear();

	m_nLineCount   = m_pTextDoc->linecount();
	m_nLongestLine = m_pTextDoc->longestline(4);

	m_nVScrollPos  = 0;
	m_nHScrollPos  = 0;

	UpdateMetrics();

	return TRUE;
}

 NAMESPACE_END_GUI
