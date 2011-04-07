/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef TEXTDOC_INCLUDED
#define TEXTDOC_INCLUDED

#include "codepages.h"
#include "Sequence.h"

class TextIterator;

class TextDocument
{
  friend class TextIterator;
  friend class TextView;

public:
  TextDocument();
  ~TextDocument();

#if defined(INL_OS_WINDOWS)
  bool init (HANDLE hFile);
#elif defined(INL_OS_LINUX)
  bool init (int hFile);
#else
#error TextDocument::init is not implemented for this platform.
#endif

  bool init (const TCHAR *filename);

  bool clear();
  bool EmptyDoc();

  bool Undo (inl::t_u32 *offset_start, inl::t_u32 *offset_end);
  bool Redo (inl::t_u32 *offset_start, inl::t_u32 *offset_end);

  // UTF-16 text-editing interface
  inl::t_u32 insert_text (inl::t_u32 offset_chars, TCHAR *text, inl::t_u32 length);
  inl::t_u32 replace_text (inl::t_u32 offset_chars, TCHAR *text, inl::t_u32 length, inl::t_u32 erase_len);
  inl::t_u32 erase_text (inl::t_u32 offset_chars, inl::t_u32 length);

  inl::t_u32 lineno_from_offset (inl::t_u32 offset);
  inl::t_u32 offset_from_lineno (inl::t_u32 lineno);

  bool  lineinfo_from_offset (inl::t_u32 offset_chars, inl::t_u32 *lineno, inl::t_u32 *lineoff_chars,  inl::t_u32 *linelen_chars, inl::t_u32 *lineoff_bytes, inl::t_u32 *linelen_bytes);
  bool  lineinfo_from_lineno (inl::t_u32 lineno,                      inl::t_u32 *lineoff_chars,  inl::t_u32 *linelen_chars, inl::t_u32 *lineoff_bytes, inl::t_u32 *linelen_bytes);

  TextIterator iterate (inl::t_u32 offset);
  TextIterator iterate_line (inl::t_u32 lineno, inl::t_u32 *linestart = 0, inl::t_u32 *linelen = 0);
  TextIterator iterate_line_offset (inl::t_u32 offset_chars, inl::t_u32 *lineno, inl::t_u32 *linestart = 0);

  inl::t_u32 getdata (inl::t_u32 offset, BYTE *buf, size_t len);
  inl::t_u32 getline (inl::t_u32 nLineNo, TCHAR *buf, inl::t_u32 buflen, inl::t_u32 *off_chars);

  int   getformat();
  inl::t_u32 linecount();
  inl::t_u32 longestline (int tabwidth);
  inl::t_u32 size();

private:

  bool init_linebuffer();

  inl::t_u32 charoffset_to_byteoffset (inl::t_u32 offset_chars);
  inl::t_u32 byteoffset_to_charoffset (inl::t_u32 offset_bytes);

  inl::t_u32 count_chars (inl::t_u32 offset_bytes, inl::t_u32 length_chars);

  size_t utf16_to_rawdata (TCHAR *utf16str, size_t utf16len, BYTE *rawdata, size_t *rawlen);
  size_t rawdata_to_utf16 (BYTE *rawdata, size_t rawlen, TCHAR *utf16str, size_t *utf16len);
  size_t rawdata_to_utf8 (BYTE *rawdata, size_t rawlen, BYTE *utf8str, size_t *utf8len);

  int   detect_file_format (int *headersize);
  inl::t_u32	  gettext (inl::t_u32 offset, inl::t_u32 lenbytes, TCHAR *buf, inl::t_u32 *len);
  int   getchar (inl::t_u32 offset, inl::t_u32 lenbytes, inl::t_u32 *pch32);

  // UTF-16 text-editing interface
  inl::t_u32	insert_raw (inl::t_u32 offset_bytes, TCHAR *text, inl::t_u32 length);
  inl::t_u32	replace_raw (inl::t_u32 offset_bytes, TCHAR *text, inl::t_u32 length, inl::t_u32 erase_len);
  inl::t_u32	erase_raw (inl::t_u32 offset_bytes, inl::t_u32 length);


  sequence m_seq;

  inl::t_u32  m_nDocLength_chars;
  inl::t_u32  m_nDocLength_bytes;

  inl::t_u32 *m_pLineBuf_byte;
  inl::t_u32 *m_pLineBuf_char;
  inl::t_u32  m_nNumLines;

  int	   m_nFileFormat;
  int    m_nHeaderSize;
};

class TextIterator
{
public:
  // default constructor sets all members to zero
  TextIterator()
    : text_doc (0), off_bytes (0), len_bytes (0)
  {
  }

  TextIterator (inl::t_u32 off, inl::t_u32 len, TextDocument *td)
    : text_doc (td), off_bytes (off), len_bytes (len)
  {

  }

  // default copy-constructor
  TextIterator (const TextIterator &ti)
    : text_doc (ti.text_doc), off_bytes (ti.off_bytes), len_bytes (ti.len_bytes)
  {
  }

  // assignment operator
  TextIterator &operator= (TextIterator &ti)
  {
    text_doc  = ti.text_doc;
    off_bytes = ti.off_bytes;
    len_bytes = ti.len_bytes;
    return *this;
  }

  inl::t_u32 gettext (TCHAR *buf, inl::t_u32 buflen)
  {
    if (text_doc)
    {
      // get text from the TextDocument at the specified byte-offset
      inl::t_u32 len = text_doc->gettext (off_bytes, len_bytes, buf, &buflen);

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

  inl::t_u32 off_bytes;
  inl::t_u32 len_bytes;
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
  inl::t_u32  len;
  int    type;
};

#endif
