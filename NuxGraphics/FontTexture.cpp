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


#include "GLResource.h"
#include "GLResourceManager.h"
#include "GLTextureResourceManager.h"
#include "OpenGLEngine.h"
#include "FontTexture.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (FontTexture);

  const int CURSOR_OFFSET = 0;

  FontTexture::FontTexture (const TCHAR *FontFile, NUX_FILE_LINE_DECL)
    :   Object (true, NUX_FILE_LINE_PARAM)
  {
    NString FontPath = GNuxGraphicsResources.FindResourceLocation (FontFile);

    std::filebuf fb;
    fb.open (FontPath.GetTCharPtr(), std::ios::in);
    std::istream is (&fb);

    BMFontParseFNT (is);
  }

  FontTexture::FontTexture (int width, int height, BYTE *Texture)
  {

  }

  FontTexture::~FontTexture()
  {
  }

  int FontTexture::GetCharWidth (const TCHAR &c) const
  {
    int ascii = c & 0xff;
    nuxAssert (ascii < m_Charset.NumChar);

    if (ascii >= m_Charset.NumChar)
      return 0;

    // XAdvance = abcA + abcB + abcC
    return m_Charset.Chars[ascii].XAdvance;
  }

  int FontTexture::GetStringWidth (const NString &str) const
  {
//     unsigned int total = 0;
//     for (unsigned int i = 0; i != (unsigned int)str.size(); ++i)
//     {
//         total += GetCharWidth(str[i]);
//     }
//     return total;
    return GetCharStringWidth (str.GetTCharPtr() );
  }

  int FontTexture::GetCharStringWidth (const TCHAR *str) const
  {
    if ( (str == 0) || (NString (str) == NString (TEXT ("") ) ) )
      return 0;

    unsigned int total = 0;

    for (int i = 0; ; ++i)
    {
      if (str[i] == 0)
        return total;

      total += GetCharWidth (str[i]);
    }

    return total;
  }

  int FontTexture::GetStringWidth (const NString &str, int num_char_to_compute) const
  {
    return GetCharStringWidth (str.GetTCharPtr(), num_char_to_compute);
  }

  int FontTexture::GetCharStringWidth (const TCHAR *str, int num_char_to_compute) const
  {
    if ( (str == 0) || (NString (str) == NString (TEXT ("") ) ) )
      return 0;

    int num_chars = num_char_to_compute;

    if (num_chars <= 0)
    {
      return 0;
    }

    int total = 0;

    for (int i = 0; i < num_chars; ++i)
    {
      if (str[i] == 0)
        return total;

      total += GetCharWidth (str[i]);
    }

    return total;
  }

  int FontTexture::GetFontHeight()
  {
    return m_Charset.FontHeight;
  }

  bool FontTexture::BMFontParseFNT ( std::istream &Stream )
  {
    std::string Line;
    int KerningIndex = 0;

    while ( !Stream.eof() )
    {
      std::getline ( Stream, Line );

      unsigned int line_size = (unsigned int) Line.length();
      TCHAR *tc = new TCHAR[line_size+1];
      const TCHAR *Stream = tc;
      Memcpy (tc, Line.c_str(), line_size + 1);
      tc[line_size] = 0;

      if ( ParseCommand (&Stream, TEXT ("common") ) /*Read == "common"*/)
      {
        Parse_bool (tc, TEXT ("Bold="),        m_Charset.bold);
        Parse_bool (tc, TEXT ("Italic="),      m_Charset.italic);
        Parse_u16 (tc, TEXT ("base="),        m_Charset.Base);
        Parse_u16 (tc, TEXT ("scaleW="),      m_Charset.Width);
        Parse_u16 (tc, TEXT ("scaleH="),      m_Charset.Height);
        Parse_u16 (tc, TEXT ("NumPages="),    m_Charset.Pages);
        Parse_u16 (tc, TEXT ("FontHeight="),  m_Charset.FontHeight);
        Parse_u16 (tc, TEXT ("Ascent="),      m_Charset.Ascent);
        Parse_u16 (tc, TEXT ("Descent="),     m_Charset.Descent);
        Parse_int (tc, TEXT ("AvgCharWidth="),     m_Charset.AvgCharWidth);
        Parse_int (tc, TEXT ("MaxCharWidth="),     m_Charset.MaxCharWidth);
        Parse_int (tc, TEXT ("InternalLeading="),     m_Charset.InternalLeading);
        Parse_int (tc, TEXT ("ExternalLeading="),     m_Charset.ExternalLeading);
        // Constant for now... Should be read from the font file
        m_Charset.NumChar = 256;
      }
      else if (ParseCommand (&Stream, TEXT ("char") ) )
      {

        unsigned short CharID = 0;

        Parse_u16 (tc, TEXT ("id="), CharID);
        Parse_u16 (tc, TEXT ("x="), m_Charset.Chars[CharID].x);
        Parse_u16 (tc, TEXT ("y="), m_Charset.Chars[CharID].y);
        Parse_u16 (tc, TEXT ("width="), m_Charset.Chars[CharID].Width);
        Parse_u16 (tc, TEXT ("height="), m_Charset.Chars[CharID].Height);
        Parse_s16 (tc, TEXT ("xoffset="), m_Charset.Chars[CharID].XOffset);
        Parse_s16 (tc, TEXT ("yoffset="), m_Charset.Chars[CharID].YOffset);
        Parse_s16 (tc, TEXT ("xadvance="), m_Charset.Chars[CharID].XAdvance);
        Parse_s16 (tc, TEXT ("abcA="), m_Charset.Chars[CharID].abcA);
        Parse_s16 (tc, TEXT ("abcB="), m_Charset.Chars[CharID].abcB);
        Parse_s16 (tc, TEXT ("abcC="), m_Charset.Chars[CharID].abcC);
        Parse_u16 (tc, TEXT ("page="), m_Charset.Chars[CharID].page);
      }
      else if ( ParseCommand (&Stream, TEXT ("Kerning") ) )
      {
        Parse_u16 (tc, "count=", m_Charset.NumKerningPairs);

        if (m_Charset.NumKerningPairs > 0)
          m_Charset.Kerning = new KerningPair[m_Charset.NumKerningPairs];
      }
      else if ( ParseCommand (&Stream, TEXT ("KerningPair") ) )
      {
        if (KerningIndex < m_Charset.NumKerningPairs)
        {
          Parse_u16 (tc, "first=", m_Charset.Kerning[KerningIndex].first);
          Parse_u16 (tc, "second=", m_Charset.Kerning[KerningIndex].second);
          Parse_s16 (tc, "amount=", m_Charset.Kerning[KerningIndex].amount);
          KerningIndex++;
        }
      }
      else if ( ParseCommand (&Stream, TEXT ("Texture") ) )
      {
        TCHAR texture[256];

        if (ParseLine (&Stream, texture, 256) )
        {
//                 FilePath FontPath;
//                 FontPath.AddSearchPath(""); // for case where fully qualified path is given
//                 FontPath.AddSearchPath(".");
//                 FontPath.AddSearchPath("../Fonts");

#ifdef UNICODE
          NString font_texture_file = GNuxGraphicsResources.FindResourceLocation (texture);
#else
          NString font_texture_file = GNuxGraphicsResources.FindResourceLocation (texture);
#endif

          NTexture2D *Texture = new NTexture2D;
          NBitmapData *BitmapData = LoadImageFile (font_texture_file.GetTCharPtr() );

          if (BitmapData)
            Texture->Update (BitmapData, false);

          NUX_SAFE_DELETE (BitmapData);
          TextureArray.push_back (Texture);
        }
      }

      delete [] tc;
    }

    return true;
  }

//    CursorPosToX (similar to ScriptStringCPtoX from Microsoft UniScript)
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
  bool FontTexture::CursorPosToX (const NString &Str,
                                  int icp,
                                  bool fTrailing,
                                  int *pX)
  {
    if (icp > (int) Str.Size() )
      return false;

    if (fTrailing)
      // get pX at the right of the character at position icp
      *pX = GetStringWidth (Str, icp + 1);
    else
      // get pX at the left of the character at position icp
      *pX = GetStringWidth (Str, icp);

    return true;
  }

//    XToCursorPosition (similar to ScriptStringXtoCP from Microsoft UniScript)
//        The XToCursorPosition function converts an x-coordinate to a character position.
//
//    Parameters
//        iX
//          [in] Specifies the x coordinate.
//        FirstVisibleCharIndex,
//          [in] Index of the first visible character in the text box
//        piCh
//          [out] Pointer to a variable that receives the character position corresponding to iX.
//        piTrailing
//          [out] Pointer to a variable that receives an indicator whether the position is the leading or trailing edge of the character.
//
//        Return Values
//          If the function is successful, it returns S_OK.
//          If the function fails, it returns an HRESULT.
//          The return value can be tested with the SUCCEEDED and FAILED macros.
  bool FontTexture::XToCursorPosition (const NString &Str,
                                       int iX,
                                       unsigned int FirstVisibleCharIndex,
                                       int *piCh,
                                       int *piTrailing)
  {
    unsigned int num_chars;
    num_chars = (unsigned int) Str.Size();
    nuxAssert (FirstVisibleCharIndex < num_chars);

    *piCh = 0;
    *piTrailing = 0;

    unsigned int total = 0;

    if (iX == 0)
    {
      *piCh = 0;
      *piTrailing = 0;
      return true;
    }


    unsigned int X = iX;

    for (unsigned int i = 0; i < FirstVisibleCharIndex; ++i)
    {
      X += GetCharWidth (Str[i]);
    }

    for (unsigned int i = 0; i < num_chars; ++i)
    {
      unsigned int s = GetCharWidth (Str[i]);

      if (i >= FirstVisibleCharIndex)
      {
        if (total == X)
        {
          *piCh = i;
          *piTrailing = 0;
          return true;
        }
        else if (total + s / 2 > X)
        {
          *piCh = i;
          *piTrailing = 0;
          return true;
        }

        else if (total + GetCharWidth (Str[i+1]) / 2 > X)
        {
          *piCh = i + 1;
          *piTrailing = 0;
          return true;
        }
      }

      total += s;
    }

    return false;
  }

  const Charset &FontTexture::GetFontInfo() const
  {
    return m_Charset;
  }

}
