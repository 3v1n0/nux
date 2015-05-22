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
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "GLResource.h"
#include "GLResourceManager.h"
#include "GLTextureResourceManager.h"
#include "GraphicsEngine.h"
#include "FontTexture.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(FontTexture);

  FontTexture::FontTexture(const char *FontFile, NUX_FILE_LINE_DECL)
    :   Object(true, NUX_FILE_LINE_PARAM)
  {
    std::string FontPath = GNuxGraphicsResources.FindResourceLocation(FontFile);

    std::filebuf fb;
    fb.open(FontPath.c_str(), std::ios::in);
    std::istream is(&fb);

    BMFontParseFNT(is);
  }

  FontTexture::FontTexture(int /* width */, int /* height */, BYTE * /* Texture */)
  {

  }

  FontTexture::~FontTexture()
  {
    std::vector<BaseTexture*>::iterator it;
    for (it = TextureArray.begin(); it != TextureArray.end(); ++it)
    {
      (*it)->UnReference();
    }
    TextureArray.clear();
  }

  int FontTexture::GetCharWidth(const char &c) const
  {
    int ascii = c & 0xff;
    nuxAssert(ascii < m_Charset.NumChar);

    if (ascii >= m_Charset.NumChar)
      return 0;

    // XAdvance = abcA + abcB + abcC
    return m_Charset.Chars[ascii].XAdvance;
  }

  int FontTexture::GetStringWidth(const std::string &str) const
  {
//     unsigned int total = 0;
//     for (unsigned int i = 0; i != (unsigned int)str.size(); ++i)
//     {
//         total += GetCharWidth(str[i]);
//     }
//     return total;
    return GetCharStringWidth(str.c_str());
  }

  int FontTexture::GetCharStringWidth(const char *str) const
  {
    if (str == 0 || *str == '\0')
      return 0;

    unsigned int total = 0;

    for (int i = 0; ; ++i)
    {
      if (str[i] == 0)
        return total;

      total += GetCharWidth(str[i]);
    }

    return total;
  }

  int FontTexture::GetStringWidth(const std::string &str, int num_char_to_compute) const
  {
    return GetCharStringWidth(str.c_str(), num_char_to_compute);
  }

  int FontTexture::GetCharStringWidth(const char *str, int num_char_to_compute) const
  {
    if (str == 0 || *str == '\0')
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

      total += GetCharWidth(str[i]);
    }

    return total;
  }

  int FontTexture::GetFontHeight()
  {
    return m_Charset.FontHeight;
  }

  bool FontTexture::BMFontParseFNT( std::istream &Stream )
  {
    std::string Line;
    int KerningIndex = 0;

    while ( !Stream.eof())
    {
      std::getline( Stream, Line );

      unsigned int line_size = (unsigned int) Line.length();
      char *tc = new char[line_size+1];
      const char *stream = tc;
      Memcpy(tc, Line.c_str(), line_size + 1);
      tc[line_size] = 0;

      if ( ParseCommand(&stream, "common") /*Read == "common"*/)
      {
        Parse_bool(tc, "Bold=",        m_Charset.bold);
        Parse_bool(tc, "Italic=",      m_Charset.italic);
        Parse_u16(tc, "base=",        m_Charset.Base);
        Parse_u16(tc, "scaleW=",      m_Charset.Width);
        Parse_u16(tc, "scaleH=",      m_Charset.Height);
        Parse_u16(tc, "NumPages=",    m_Charset.Pages);
        Parse_u16(tc, "FontHeight=",  m_Charset.FontHeight);
        Parse_u16(tc, "Ascent=",      m_Charset.Ascent);
        Parse_u16(tc, "Descent=",     m_Charset.Descent);
        Parse_int(tc, "AvgCharWidth=",     m_Charset.AvgCharWidth);
        Parse_int(tc, "MaxCharWidth=",     m_Charset.MaxCharWidth);
        Parse_int(tc, "InternalLeading=",     m_Charset.InternalLeading);
        Parse_int(tc, "ExternalLeading=",     m_Charset.ExternalLeading);
        // Constant for now... Should be read from the font file
        m_Charset.NumChar = 256;
      }
      else if (ParseCommand(&stream, "char"))
      {

        unsigned short CharID = 0;

        Parse_u16(tc, "id=", CharID);
        Parse_u16(tc, "x=", m_Charset.Chars[CharID].x);
        Parse_u16(tc, "y=", m_Charset.Chars[CharID].y);
        Parse_u16(tc, "width=", m_Charset.Chars[CharID].Width);
        Parse_u16(tc, "height=", m_Charset.Chars[CharID].Height);
        Parse_s16(tc, "xoffset=", m_Charset.Chars[CharID].XOffset);
        Parse_s16(tc, "yoffset=", m_Charset.Chars[CharID].YOffset);
        Parse_s16(tc, "xadvance=", m_Charset.Chars[CharID].XAdvance);
        Parse_s16(tc, "abcA=", m_Charset.Chars[CharID].abcA);
        Parse_s16(tc, "abcB=", m_Charset.Chars[CharID].abcB);
        Parse_s16(tc, "abcC=", m_Charset.Chars[CharID].abcC);
        Parse_u16(tc, "page=", m_Charset.Chars[CharID].page);
      }
      else if ( ParseCommand(&stream, "Kerning"))
      {
        Parse_u16(tc, "count=", m_Charset.NumKerningPairs);

        if (m_Charset.NumKerningPairs > 0)
          m_Charset.Kerning = new KerningPair[m_Charset.NumKerningPairs];
      }
      else if ( ParseCommand(&stream, "KerningPair"))
      {
        if (KerningIndex < m_Charset.NumKerningPairs)
        {
          Parse_u16(tc, "first=", m_Charset.Kerning[KerningIndex].first);
          Parse_u16(tc, "second=", m_Charset.Kerning[KerningIndex].second);
          Parse_s16(tc, "amount=", m_Charset.Kerning[KerningIndex].amount);
          KerningIndex++;
        }
      }
      else if ( ParseCommand(&stream, "Texture"))
      {
        char texture[256];

        if (ParseLine(&stream, texture, 256))
        {
//                 FilePath FontPath;
//                 FontPath.AddSearchPath(""); // for case where fully qualified path is given
//                 FontPath.AddSearchPath(".");
//                 FontPath.AddSearchPath("../Fonts");

          std::string font_texture_file = GNuxGraphicsResources.FindResourceLocation(texture);

#ifdef NUX_OPENGLES_20
          Texture2D *Texture = new Texture2D(NUX_TRACKER_LOCATION);
#else
          TextureRectangle *Texture = new TextureRectangle(NUX_TRACKER_LOCATION);
#endif

          NBitmapData* bitmap_data = LoadImageFile(font_texture_file.c_str());

          if (bitmap_data)
            Texture->Update(bitmap_data, false);

          delete bitmap_data;
          TextureArray.push_back(Texture);
        }
      }

      delete [] tc;
    }

    return true;
  }

//    CursorPosToX(similar to ScriptStringCPtoX from Microsoft UniScript)
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
  bool FontTexture::CursorPosToX(const std::string &Str,
                                  int icp,
                                  bool fTrailing,
                                  int *pX)
  {
    if (icp > (int) Str.size())
      return false;

    if (fTrailing)
      // get pX at the right of the character at position icp
      *pX = GetStringWidth(Str, icp + 1);
    else
      // get pX at the left of the character at position icp
      *pX = GetStringWidth(Str, icp);

    return true;
  }

//    XToCursorPosition(similar to ScriptStringXtoCP from Microsoft UniScript)
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
  bool FontTexture::XToCursorPosition(const std::string &Str,
                                       int iX,
                                       unsigned int FirstVisibleCharIndex,
                                       int *piCh,
                                       int *piTrailing)
  {
    unsigned int num_chars;
    num_chars = (unsigned int) Str.size();
    nuxAssert(FirstVisibleCharIndex < num_chars);

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
      X += GetCharWidth(Str[i]);
    }

    for (unsigned int i = 0; i < num_chars; ++i)
    {
      unsigned int s = GetCharWidth(Str[i]);

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

        else if (total + GetCharWidth(Str[i+1]) / 2 > X)
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
