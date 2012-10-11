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


#ifndef FONTTEXTURE_H
#define FONTTEXTURE_H

class IOpenGLPixelShader;

namespace nux
{

  typedef enum _TextAlignment
  {
    eAlignTextNone       = 0,
    eAlignTextLeft       = 1,
    eAlignTextRight      = 2,
    eAlignTextCenter     = 3,

  } TextAlignment;

  class StringBBox
  {
  public:
    StringBBox()
    {
      x = 0;
      y = 0;
      width = 0;
      height = 0;
      ybearing = 0;
      downline = 0;
    };
    ~StringBBox() {};

    int x;
    int y;
    int width;
    int height;
    int ybearing; // max ybearing of the string
    int downline; // max downline of the string(max space below the baseline)
  };

  class PageBBox
  {
  public:
    PageBBox()
    {
      xmin = 0;
      ymin = 0;
      xmax = 0;
      ymax = 0;
      x_margin = 0;
      y_margin = 0;
    };
    ~PageBBox() {};
    INT xmin;
    INT ymin;
    INT xmax;
    INT ymax;
    INT x_margin;
    INT y_margin;
  };

///////////////////////////////////////////////////////////////////////////////////////////////////////
  struct CharDescriptor
  {
    //clean 16 bytes
    unsigned short x;
    unsigned short y;
    unsigned short Width;
    unsigned short Height;
    short XOffset;
    short YOffset;
    unsigned short page;
    short XAdvance;
    short abcA;
    short abcB;
    short abcC;


    CharDescriptor()
      : x( 0 )
      , y( 0 )
      , Width( 0 )
      , Height( 0 )
      , XOffset( 0 )
      , YOffset( 0 )
      , page( 0 )
      , XAdvance( 0 )
      , abcA( 0 )
      , abcB( 0 )
      , abcC( 0 )
    { }
  };

  struct KerningPair
  {
    unsigned short first;
    unsigned short second;
    short          amount;

    KerningPair()
      : first(0), second(0), amount(0)
    {}
  };

  struct Charset
  {
    bool italic;
    bool bold;
    unsigned short LineHeight;
    unsigned short Base;
    unsigned short Width, Height;
    unsigned short Pages;
    unsigned short FontHeight;
    unsigned short Ascent;
    unsigned short Descent;
    int AvgCharWidth;
    int MaxCharWidth;
    int InternalLeading;
    int ExternalLeading;
    unsigned short NumChar;
    CharDescriptor Chars[256];
    unsigned short NumKerningPairs;
    KerningPair *Kerning;

    Charset()
    : italic(false), bold(false), LineHeight(0), Base(0), Width(0), Height(0)
        , Pages(0), FontHeight(0), Ascent(0), Descent(0), AvgCharWidth(0)
        , MaxCharWidth(0), InternalLeading(0), ExternalLeading(0)
        , NumChar(0), NumKerningPairs(0), Kerning(NULL)
    {}

    ~Charset()
    {
      delete [] Kerning;
    }
  };

// Information about a glyph. Tex_y2 can be calculated from tex_y1
// and _tex_line_height(see below). Advance is the width of the
// glyph in screen space.
  struct Glyph
  {
    float tex_x1, tex_y1, tex_x2;
    int advance;
  };

  class FontRenderer;


////////////////////////////////////////////////////////////////////////////////////////////////////

// This font system loads in a custom file containing a gray scale
// texture(used as alpha texture) with all the letters on it, and
// information about what glyph is where.
  class FontTexture: public Object
  {
  public:
    NUX_DECLARE_OBJECT_TYPE(FontTexture, Object);

    FontTexture(const char *FontFile, NUX_FILE_LINE_PROTO);
    FontTexture(INT width, INT height, BYTE *Texture);
    ~FontTexture();

    // The line height is a constant;
    int GetLineHeight() const
    {
      return m_Charset.FontHeight;
    }
    // Knowing the width of a character or a string can be useful if you
    // want your UI to look good at all.
    int GetCharWidth(const char &c) const;
    int GetStringWidth(const std::string &str) const;
    int GetCharStringWidth(const char *str) const;
    int GetStringWidth(const std::string &str, int num_char_to_compute) const;
    int GetCharStringWidth(const char *str, int num_char_to_compute) const;
    int GetFontHeight();

    //    CursorPosToX(similar to ScriptStringCPtoX from microsoft UniScript)
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
    bool CursorPosToX(const std::string &Str,
                       int icp,
                       bool fTrailing,
                       int *pX);

    //    XToCursorPosition(similar to ScriptStringXtoCP from microsoft UniScript)
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
    bool XToCursorPosition(const std::string &Str,
                            int iX,
                            unsigned int FirstVisibleCharIndex,
                            int *piCh,
                            int *piTrailing);

    bool BMFontParseFNT( std::istream &Stream);

    const Charset &GetFontInfo() const;

    std::vector<BaseTexture*> TextureArray;

  private:
    INT _RefCount;
    INT _textureBMF;
    std::vector<unsigned int> m_gl_texture_id;
    Charset m_Charset;

    friend class FontRenderer;
  };

}

#endif //FONTTEXTURE_H
