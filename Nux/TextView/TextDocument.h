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


#ifndef TEXTDOCUMENT_H
#define TEXTDOCUMENT_H

#include "codepages.h"
#include "sequence.h"

NAMESPACE_BEGIN_GUI

class TextIterator;

class TextDocument
{
	friend class TextIterator;
	friend class TextView;

public:
	TextDocument();
	~TextDocument();

	bool  init(HANDLE hFile);
	bool  init(TCHAR *filename);
	
	bool  clear();
	bool EmptyDoc();

	bool	Undo(t_u32 *offset_start, t_u32 *offset_end);
	bool	Redo(t_u32 *offset_start, t_u32 *offset_end);

	// UTF-16 text-editing interface
	t_u32	insert_text(t_u32 offset_chars, TCHAR *text, t_u32 length);
	t_u32	replace_text(t_u32 offset_chars, TCHAR *text, t_u32 length, t_u32 erase_len);
	t_u32	erase_text(t_u32 offset_chars, t_u32 length);

	t_u32 lineno_from_offset(t_u32 offset);
	t_u32 offset_from_lineno(t_u32 lineno);

	bool  lineinfo_from_offset(t_u32 offset_chars, t_u32 *lineno, t_u32 *lineoff_chars,  t_u32 *linelen_chars, t_u32 *lineoff_bytes, t_u32 *linelen_bytes);
	bool  lineinfo_from_lineno(t_u32 lineno, t_u32 *lineoff_chars,  t_u32 *linelen_chars, t_u32 *lineoff_bytes, t_u32 *linelen_bytes);	

	TextIterator iterate(t_u32 offset);
	TextIterator iterate_line(t_u32 lineno, t_u32 *linestart = 0, t_u32 *linelen = 0);
	TextIterator iterate_line_offset(t_u32 offset_chars, t_u32 *lineno, t_u32 *linestart = 0);

	t_u32 getdata(t_u32 offset, BYTE *buf, size_t len);
	t_size getline(t_size nLineNo, TCHAR *buf, t_size buflen, t_size *off_chars);

	int   getformat();
	t_u32 linecount();
	t_u32 longestline(int tabwidth);
	t_u32 size();

private:
	
	bool init_linebuffer();

	t_u32 charoffset_to_byteoffset(t_u32 offset_chars);
	t_u32 byteoffset_to_charoffset(t_u32 offset_bytes);

	t_u32 count_chars(t_u32 offset_bytes, t_u32 length_chars);

	size_t utf16_to_rawdata(TCHAR *utf16str, size_t utf16len, BYTE *rawdata, size_t *rawlen);
    size_t rawdata_to_utf16(BYTE *rawdata, size_t rawlen, TCHAR *utf16str, size_t *utf16len);
    size_t rawdata_to_utf8(BYTE *rawdata, size_t rawlen, BYTE *utf8str, size_t *utf8len);

	int   detect_file_format(int *headersize);
	t_size gettext(t_u32 offset, t_u32 lenbytes, TCHAR *buf, t_size *len);
	int   getchar(t_u32 offset, t_u32 lenbytes, t_u32 *pch32);

	// UTF-16 text-editing interface
	t_u32	insert_raw(t_u32 offset_bytes, TCHAR *text, t_u32 length);
	t_u32	replace_raw(t_u32 offset_bytes, TCHAR *text, t_u32 length, t_u32 erase_len);
	t_u32	erase_raw(t_u32 offset_bytes, t_u32 length);


	sequence m_seq;

	t_u32  m_nDocLength_chars;
	t_u32  m_nDocLength_bytes;

	t_u32 *m_pLineBuf_byte;
	t_u32 *m_pLineBuf_char;
	t_u32  m_nNumLines;
	
	int	   m_nFileFormat;
	int    m_nHeaderSize;
};

class TextIterator
{
public:
	// default constructor sets all members to zero
	TextIterator()
		: text_doc(0), off_bytes(0), len_bytes(0)
	{
	}

	TextIterator(t_u32 off, t_u32 len, TextDocument *td)
		: text_doc(td), off_bytes(off), len_bytes(len)
	{
		
	}

	// default copy-constructor
	TextIterator(const TextIterator &ti) 
		: text_doc(ti.text_doc), off_bytes(ti.off_bytes), len_bytes(ti.len_bytes)
	{
	}

	// assignment operator
	TextIterator & operator= (TextIterator &ti)
	{
		text_doc  = ti.text_doc;
		off_bytes = ti.off_bytes;
		len_bytes = ti.len_bytes;
		return *this;
	}

	t_size gettext(TCHAR *buf, t_size buflen)
	{
		if(text_doc)
		{
			// get text from the TextDocument at the specified byte-offset
			t_u32 len = text_doc->gettext(off_bytes, len_bytes, buf, &buflen);

			// adjust the iterator's internal position
			off_bytes += len;
			len_bytes -= len;

			return buflen;
		}
		else
		{
			return 0;
		}
	}

	/*int insert_text(TCHAR *buf, int buflen)
	{
		if(text_doc)
		{
			// get text from the TextDocument at the specified byte-offset
			int len = text_doc->insert(off_bytes, buf, buflen);

			// adjust the iterator's internal position
			off_bytes += len;
			return buflen;
		}
		else
		{
			return 0;
		}
	}

	int replace_text(TCHAR *buf, int buflen)
	{
		if(text_doc)
		{
			// get text from the TextDocument at the specified byte-offset
			int len = text_doc->replace(off_bytes, buf, buflen);

			// adjust the iterator's internal position
			off_bytes += len;
			return buflen;
		}
		else
		{
			return 0;
		}
	}

	int erase_text(int length)
	{
		if(text_doc)
		{
			// get text from the TextDocument at the specified byte-offset
			int len = text_doc->erase(off_bytes, length);

			// adjust the iterator's internal position
			off_bytes += len;
			return len;
		}
		else
		{
			return 0;
		}
	}*/


	operator bool()
	{
		return text_doc ? true : false;
	}

private:

	TextDocument *text_doc;
	
	t_u32 off_bytes;
	t_u32 len_bytes;
};

class LineIterator
{
public:
	LineIterator();

private:
	TextDocument *m_pTextDoc;
};

struct _BOM_LOOKUP
{
	DWORD  bom;
	t_u32  len;
	int    type;
};

NAMESPACE_END_GUI

#endif // TEXTDOCUMENT_H
