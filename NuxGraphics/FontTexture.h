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


#ifndef FONT_H
#define FONT_H

#include "GLDeviceObjects.h"
#include <boost/shared_ptr.hpp>

NAMESPACE_BEGIN_OGL

class GraphicsContext;
class NRectangleTexture;
class ICgPixelShader;
class ICgVertexShader;

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
    StringBBox() {
        x = 0;
        y = 0;
        width = 0;
        height = 0;
        ybearing = 0;
        downline = 0;
    };
    ~StringBBox(){};

    int x;
    int y;
    int width;
    int height;
    int ybearing; // max ybearing of the string
    int downline; // max downline of the string (max space below the baseline)
};

class PageBBox 
{
public:
    PageBBox() {
        xmin = 0;
        ymin = 0;
        xmax = 0;
        ymax = 0;
        x_margin = 0;    
        y_margin = 0;    
    };
    ~PageBBox(){};
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
        , XAdvance( 0 )
        , abcA( 0 )
        , abcB( 0 )
        , abcC( 0 )
        , page( 0 )
    { }
};

struct KerningPair
{
    unsigned short first;
    unsigned short second;
    short          amount;
};

struct Charset
{
    BOOL italic;
    BOOL bold;
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
    KerningPair* Kerning;
};

// Information about a glyph. Tex_y2 can be calculated from tex_y1
// and _tex_line_height (see below). Advance is the width of the
// glyph in screen space.
struct Glyph
{
    float tex_x1, tex_y1, tex_x2;
    int advance;
};

class FontRenderer;
class TemplateQuadBuffer;


////////////////////////////////////////////////////////////////////////////////////////////////////

// This font system loads in a custom file containing a gray scale
// texture (used as alpha texture) with all the letters on it, and
// information about what glyph is where.
class FontTexture
{
public:
    FontTexture(const TCHAR* FontFile);
    FontTexture(INT width, INT height, BYTE *Texture);
    ~FontTexture();

    // The line height is a constant;
    int GetLineHeight() const{return m_Charset.FontHeight;}
    // Knowing the width of a character or a string can be useful if you
    // want your UI to look good at all.
    int GetCharWidth(const TCHAR& c) const;
    int GetStringWidth(const NString& str) const;
    int GetCharStringWidth(const TCHAR* str) const;
    int GetStringWidth(const NString& str, int num_char_to_compute) const;
    int GetCharStringWidth(const TCHAR* str, int num_char_to_compute) const;
    int GetFontHeight();

    //    CursorPosToX (similar to ScriptStringCPtoX from microsoft UniScript)
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
    bool CursorPosToX(const NString& Str,
        int icp, 
        bool fTrailing, 
        int *pX);

    //    XToCursorPosition (similar to ScriptStringXtoCP from microsoft UniScript)
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
    bool XToCursorPosition(const NString& Str,
        int iX,
        t_u32 FirstVisibleCharIndex,
        int *piCh, 
        int *piTrailing);

    bool BMFontParseFNT( std::istream& Stream);
    
    const Charset& GetFontInfo() const;

    std::vector<NTexture2D*> TextureArray;

private:
    INT _RefCount;
    INT _textureBMF;
    std::vector<t_u32> m_gl_texture_id;
    Charset m_Charset;
    
    FilePath m_FontSearchPath;
    friend class FontRenderer;
};



typedef boost::shared_ptr<FontTexture>  NFontPtr;

class FontRenderer
{
public:
    FontRenderer(GraphicsContext& OpenGLEngine);
    ~FontRenderer();

    GraphicsContext& m_OpenGLEngine;

    int DrawColorString(const NFontPtr& Font, int x, int y, const NString& str, const Color& color, bool WriteAlphaChannel, int NumCharacter = 0, int SkipFirstNCharacters = 0);
    
    void PositionString(const NFontPtr& Font, const NString& str, const PageBBox&, StringBBox&, TextAlignment align = eAlignTextCenter, int NumCharacter = 0);
    int RenderColorText(const NFontPtr& Font, int x, int y, const NString& Str, const Color& color, bool WriteAlphaChannel, int NumCharacter);
    int RenderColorTextLineStatic(const NFontPtr& Font, const PageBBox& pageSize, const NString& Str, const Color& color,
        bool WriteAlphaChannel, TextAlignment alignment);
    int RenderColorTextLineEdit(const NFontPtr& Font, const PageBBox& pageSize, const NString& Str,
        const Color& TextColor,
        bool WriteAlphaChannel,
        const Color& SelectedTextColor,
        const Color& SelectedTextBackgroundColor,
        const Color& TextBlinkColor,
        const Color& CursorColor,
        bool ShowCursor, unsigned int CursorPosition,
        int offset = 0, int selection_start = 0, int selection_end = 0);

    int RenderText(const NFontPtr& Font, int x, int y, const NString& str, const Color& color, bool WriteAlphaChannel, int StartCharacter = 0, int NumCharacters = 0);
    int RenderTextToBuffer(
        float* VertexBuffer, int VBSize,
        const NFontPtr& Font, Rect geo, const NString& str, const Color& color, TextAlignment alignment = eAlignTextCenter, int NumCharacter = 0);

private:
    TemplateQuadBuffer* m_QuadBuffer;
    TRefGL<IOpenGLPixelShader> m_PixelShaderProg;
    TRefGL<IOpenGLVertexShader> m_VertexShaderProg;
    TRefGL<IOpenGLShaderProgram> m_ShaderProg;

    TRefGL<IOpenGLAsmPixelShader> m_AsmPixelShaderProg;
    TRefGL<IOpenGLAsmVertexShader> m_AsmVertexShaderProg;
    TRefGL<IOpenGLAsmShaderProgram> m_AsmShaderProg;
};

NAMESPACE_END_OGL

#endif //FONT_H