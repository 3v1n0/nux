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
#include "IOpenGLResource.h"
#include "GLResourceManager.h"
#include "IOpenGLBaseTexture.h"
#include "GpuDevice.h"
#include "GLTextureResourceManager.h"

#include "GraphicsEngine.h"

#include "FontRenderer.h"

namespace nux
{

  extern bool USE_ARB_SHADERS;

  const int CURSOR_OFFSET = 0;
  static int CURSOR_SIZE = 2;

  NString gFontVtxShader = TEXT ("#version 110                                     \n\
                             attribute vec4 iOffset;                            \n\
                             attribute vec4 iPosition;                          \n\
                             attribute vec4 iScale;                             \n\
                             attribute vec4 iTexUV;                             \n\
                             uniform mat4 ViewProjectionMatrix;                 \n\
                             varying vec4 oTexCoord0;                           \n\
                             void main()                                        \n\
                             {                                                  \n\
                             oTexCoord0   = iTexUV;                             \n\
                             vec4 myvertex = iPosition * iScale + iOffset;      \n\
                             gl_Position  = ViewProjectionMatrix * myvertex;    \n\
                             }");

  NString gFontFragShader = TEXT ("#version 110                                            \n\
                              #extension GL_ARB_texture_rectangle : enable              \n\
                              uniform sampler2D FontTexture;                            \n\
                              uniform vec4 TextColor;                                   \n\
                              varying vec4 oTexCoord0;                                  \n\
                              void main()                                               \n\
                              {                                                         \n\
                              vec4 diffuse = texture2D(FontTexture, oTexCoord0.st);     \n\
                              gl_FragColor = vec4(TextColor.x, TextColor.y, TextColor.z, TextColor.w*diffuse.w);  \n\
                              }");

  NString FontAsmVtx = TEXT (
                         "!!ARBvp1.0                                     \n\
                        ATTRIB iScale       = vertex.attrib[9];         \n\
                        ATTRIB iOffset      = vertex.attrib[10];        \n\
                        OUTPUT oPos         = result.position;          \n\
                        OUTPUT oTexCoord0   = result.texcoord[0];       \n\
                        # Transform the vertex to clip coordinates.     \n\
                        TEMP temp;                                      \n\
                        MAD   temp, vertex.position, iScale, iOffset;   \n\
                        DP4   oPos.x, state.matrix.mvp.row[0], temp;    \n\
                        DP4   oPos.y, state.matrix.mvp.row[1], temp;    \n\
                        DP4   oPos.z, state.matrix.mvp.row[2], temp;    \n\
                        DP4   oPos.w, state.matrix.mvp.row[3], temp;    \n\
                        MOV   oTexCoord0, vertex.attrib[8];             \n\
                        END");

  NString FontAsmFrg = TEXT (
                         "!!ARBfp1.0                                       \n\
                        PARAM color = program.local[0];                   \n\
                        TEMP temp;                                        \n\
                        TEMP tex0;                                        \n\
                        TEX tex0, fragment.texcoord[0], texture[0], 2D;   \n\
                        MOV temp, color;                                  \n\
                        MUL temp.w, color, tex0;                          \n\
                        MOV result.color, temp;                           \n\
                        END");

  NString FontAsmFrgRect = TEXT (
    "!!ARBfp1.0                                       \n\
    PARAM color = program.local[0];                   \n\
    TEMP temp;                                        \n\
    TEMP tex0;                                        \n\
    TEX tex0, fragment.texcoord[0], texture[0], RECT; \n\
    MOV temp, color;                                  \n\
    MUL temp.w, color, tex0;                          \n\
    MOV result.color, temp;                           \n\
    END");


  FontRenderer::FontRenderer (GraphicsEngine &OpenGLEngine)
    :   m_OpenGLEngine (OpenGLEngine)
  {
    //m_QuadBuffer = new TemplateQuadBuffer(GetGpuDevice(), SHADER_TYPE_GLSL);
    if (!USE_ARB_SHADERS)
    {
      m_PixelShaderProg   = GetGpuDevice()->CreatePixelShader();
      m_VertexShaderProg  = GetGpuDevice()->CreateVertexShader();
      m_ShaderProg        = GetGpuDevice()->CreateShaderProgram();

      m_VertexShaderProg->SetShaderCode (TCHAR_TO_ANSI (*gFontVtxShader) );
      m_PixelShaderProg->SetShaderCode (TCHAR_TO_ANSI (*gFontFragShader) );

      m_ShaderProg->ClearShaderObjects();
      m_ShaderProg->AddShaderObject (m_VertexShaderProg);
      m_ShaderProg->AddShaderObject (m_PixelShaderProg);
      m_ShaderProg->Link();
    }
    else
    {
      //m_AsmVertexShaderProg  = GetGpuDevice()->CreateAsmVertexShader();
      //m_AsmPixelShaderProg   = GetGpuDevice()->CreateAsmPixelShader();
      m_AsmShaderProg        = GetGpuDevice()->CreateAsmShaderProgram();

      m_AsmShaderProg->LoadVertexShader (TCHAR_TO_ANSI (*FontAsmVtx) );
      m_AsmShaderProg->LoadPixelShader (TCHAR_TO_ANSI (*FontAsmFrg) );
      m_AsmShaderProg->Link();

      _asm_font_texture_rect_prog = GetGpuDevice()->CreateAsmShaderProgram();
      _asm_font_texture_rect_prog->LoadVertexShader (TCHAR_TO_ANSI (*FontAsmVtx) );
      _asm_font_texture_rect_prog->LoadPixelShader (TCHAR_TO_ANSI (*FontAsmFrgRect) );
      _asm_font_texture_rect_prog->Link();
    }
  }

  FontRenderer::~FontRenderer()
  {
    //delete m_QuadBuffer;
  }

  int FontRenderer::DrawColorString (ObjectPtr<FontTexture> Font, int x, int y, const NString &str, const Color &color, bool WriteAlphaChannel, int SkipFirstNCharacters, int NumCharacter)
  {
    return RenderText (Font, x, y, str, color, WriteAlphaChannel, SkipFirstNCharacters, NumCharacter);
  }

  void FontRenderer::PositionString (ObjectPtr<FontTexture> Font, const NString &str, const PageBBox &pageBBox, StringBBox &strBBox, TextAlignment alignment, int NumCharacter)
  {
    int x, y;
    int xmin, ymin, xmax, ymax;
    xmin = pageBBox.xmin + pageBBox.x_margin;
    xmax = pageBBox.xmax - pageBBox.x_margin;
    ymin = pageBBox.ymin + pageBBox.y_margin;
    ymax = pageBBox.ymax - pageBBox.y_margin;

    int NumChar = 0;

    if (NumCharacter == 0)
      NumChar = str.Size();
    else
      NumChar = Min ( (int) str.Size(), NumCharacter);

    strBBox.width = Font->GetStringWidth (str, NumChar);
    strBBox.height = Font->GetLineHeight();

    switch (alignment)
    {
      case eAlignTextCenter:

        if (strBBox.width > xmax - xmin)
          x = xmin; // the text is larger than the box: default to eAlignTextLeft for x.
        else
          x = xmin + ( (float) (xmax - xmin) - (float) (strBBox.width) ) / 2.0f;

        y = ymin + ( (float) (ymax - ymin) - (float) (strBBox.height) ) / 2.0f;
        break;

      case eAlignTextRight:
        x = xmin + ( (float) (xmax - xmin) - (float) (strBBox.width) );
        y = ymin + ( (float) (ymax - ymin) - (float) (strBBox.height) ) / 2.0f;
        break;

      case eAlignTextLeft:
      default:
        x = xmin;
        y = ymin + ( (float) (ymax - ymin) - (float) (strBBox.height) ) / 2.0f;
        break;
    }

    strBBox.x = x;
    strBBox.y = y;
  }

  int FontRenderer::RenderColorText (ObjectPtr<FontTexture> Font, int x, int y, const NString &Str, const Color &color,
                                     bool WriteAlphaChannel, int NumCharacter)
  {
    int off = DrawColorString (Font, x, y, Str, color, WriteAlphaChannel, 0, NumCharacter);
    return off;
  }

  int FontRenderer::RenderColorTextLineStatic (ObjectPtr<FontTexture> Font, const PageBBox &pageSize, const NString &Str, const Color &color,
      bool WriteAlphaChannel, TextAlignment alignment)
  {
    StringBBox stringBBox;

    m_OpenGLEngine.PushClippingRectangle (Rect (pageSize.xmin, pageSize.ymin, pageSize.xmax - pageSize.xmin, pageSize.ymax - pageSize.ymin) );
    PositionString (Font, Str, pageSize, stringBBox, alignment);
    int off = DrawColorString (Font, stringBBox.x, stringBBox.y, Str, color, WriteAlphaChannel, 0, Str.Size() );

    m_OpenGLEngine.PopClippingRectangle();
    return off;
  }

  int FontRenderer::RenderColorTextLineEdit (ObjectPtr<FontTexture> Font, const PageBBox &pageSize, const NString &Str,
      const Color &TextColor,
      bool WriteAlphaChannel,
      const Color &SelectedTextColor,
      const Color &SelectedTextBackgroundColor,
      const Color &TextBlinkColor,
      const Color &CursorColor,
      bool ShowCursor, unsigned int CursorPosition, int offset, int selection_start, int selection_end)
  {
    StringBBox stringBBox;
    Color selection_color (0xFF888888);

    NString substring = Str.GetSubString (selection_start, selection_end - selection_start);
    unsigned int substring_width = Font->GetStringWidth (substring);
    int substring_pos = Font->GetStringWidth (Str, selection_start);

    m_OpenGLEngine.PushClippingRectangle (Rect (pageSize.xmin, pageSize.ymin, pageSize.xmax - pageSize.xmin, pageSize.ymax - pageSize.ymin) );

    if (substring_width > 0)
      m_OpenGLEngine.QRP_Color (pageSize.xmin + offset + substring_pos, pageSize.ymin, substring_width, pageSize.ymax - pageSize.ymin, SelectedTextBackgroundColor);

    m_OpenGLEngine.PopClippingRectangle();

    m_OpenGLEngine.PushClippingRectangle (Rect (pageSize.xmin, pageSize.ymin, pageSize.xmax - pageSize.xmin, pageSize.ymax - pageSize.ymin) );

    PositionString (Font, Str, pageSize, stringBBox, eAlignTextLeft);
    //ComputeGlyphString(stringBBox.x + offset, stringBBox.y, Str.c_str());

    // Part before selected text
    int off = DrawColorString (Font, stringBBox.x + offset, stringBBox.y, Str, TextColor, WriteAlphaChannel, 0, selection_start);
    // Selection part
    off = DrawColorString (Font, stringBBox.x + offset, stringBBox.y, Str, SelectedTextColor, WriteAlphaChannel, selection_start, selection_end - selection_start);
    // Part after selected text
    off = DrawColorString (Font, stringBBox.x + offset, stringBBox.y, Str, TextColor, WriteAlphaChannel, selection_end, Str.Size() - selection_end);

    m_OpenGLEngine.PopClippingRectangle();

    // Render Cursor
    NString temp = Str.GetSubString (0, CursorPosition);
    int w = Font->GetStringWidth (temp.GetTCharPtr() );


    if (ShowCursor)
    {
      int x = pageSize.xmin + w + offset + CURSOR_OFFSET;
      x = (x >= pageSize.xmax) ? pageSize.xmax - 1 : x;
      m_OpenGLEngine.PushClippingRectangle (Rect (x, pageSize.ymin, CURSOR_SIZE, pageSize.ymax - pageSize.ymin) );

      m_OpenGLEngine.QRP_Color (x, pageSize.ymin, CURSOR_SIZE, pageSize.ymax - pageSize.ymin, CursorColor);
      
      DrawColorString (Font, stringBBox.x + offset, stringBBox.y, Str, TextBlinkColor, WriteAlphaChannel, CursorPosition, 1);
      m_OpenGLEngine.PopClippingRectangle();
    }

    return off;
  }

  int FontRenderer::RenderText (ObjectPtr<FontTexture> Font, int x, int y, const NString &str, const Color &color, bool WriteAlphaChannel, int StartCharacter, int NumCharacter)
  {
    // !WARNING This call works if all the glyph of the font are in a single texture.

    int StrLength = str.Size();

    if (StrLength <= 0)
      return 0;

    nuxAssertMsg (NumCharacter >= 0, TEXT ("[FontRenderer::RenderText] Incorrect value for NumCharacter.") );
    nuxAssertMsg (StartCharacter >= 0, TEXT ("[FontRenderer::RenderText] Incorrect value for StartCharacter.") );
    nuxAssertMsg (StartCharacter <= StrLength, TEXT ("[FontRenderer::RenderText] Incorrect value for StartCharacter.") );

    //     if(NumCharacter == 0)
    //         NumCharacter = str.Size();

    int NumCharToDraw = Min<int> (StrLength - StartCharacter, NumCharacter);

    //nuxAssertMsg(NumCharToDraw > 0, TEXT("[FontRenderer::RenderText] Incorrect value for NumCharToDraw."));
    if (NumCharToDraw <= 0)
      return 0;

    CHECKGL ( glDisable (GL_CULL_FACE) );
    int CurX = x;
    int CurY = y;
    GetThreadGraphicsContext()->GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetThreadGraphicsContext()->GetRenderStates().SetColorMask (TRUE, TRUE, TRUE, WriteAlphaChannel); // Do not write the alpha of characters

    Vector4 *Position = new Vector4[StrLength*4];
    Vector4 *UV = new Vector4[StrLength*4];
    Vector4 *Offset = new Vector4[StrLength*4];
    Vector4 *Scale = new Vector4[StrLength*4];

    ObjectPtr<CachedBaseTexture> glTexture = m_OpenGLEngine.ResourceCache.GetCachedResource (Font->TextureArray[0]);

    float tex_width = (float) glTexture->m_Texture->GetWidth();
    float tex_height = (float) glTexture->m_Texture->GetHeight();

    for (int i = 0; i < StrLength; ++i)
    {
      unsigned char c = static_cast<unsigned char> (str[i]);
      unsigned int CharX = Font->m_Charset.Chars[c /*Str[i]*/].x;
      unsigned int CharY = Font->m_Charset.Chars[c /*Str[i]*/].y;
      unsigned int Width = Font->m_Charset.Chars[c /*Str[i]*/].Width;
      unsigned int Height = Font->m_Charset.Chars[c /*Str[i]*/].Height;
      //int OffsetX = Font->m_Charset.Chars[c /*Str[i]*/].XOffset;
      //int OffsetY = Font->m_Charset.Chars[c /*Str[i]*/].YOffset;
      int abcA = Font->m_Charset.Chars[c /*Str[i]*/].abcA;
      int abcB = Font->m_Charset.Chars[c /*Str[i]*/].abcB;
      int abcC = Font->m_Charset.Chars[c /*Str[i]*/].abcC;
      //int page = Font->m_Charset.Chars[c /*Str[i]*/].page;

      if ( (i >= StartCharacter) && (i < StartCharacter + NumCharToDraw) )
      {
        int II = i - StartCharacter;
        Position[II*4 + 0].x = 0;      // x
        Position[II*4 + 0].y = 0;      // y
        Position[II*4 + 0].z = 0;      // z
        Position[II*4 + 0].w = 1.0f;   // w

        Position[II*4 + 1].x = 1.0f;   // x
        Position[II*4 + 1].y = 0;      // y
        Position[II*4 + 1].z = 0;      // z
        Position[II*4 + 1].w = 1;      // w

        Position[II*4 + 2].x = 1.0f;   // x
        Position[II*4 + 2].y = 1.0f;   // y
        Position[II*4 + 2].z = 0;      // z
        Position[II*4 + 2].w = 1;      // w

        Position[II*4 + 3].x = 0;      // x
        Position[II*4 + 3].y = 1.0f;   // y
        Position[II*4 + 3].z = 0;      // z
        Position[II*4 + 3].w = 1;      // w

        for (int j = 0; j < 4; j++)
        {
          Offset[II*4 + j].x = CurX + abcA;
          Offset[II*4 + j].y = CurY;
          Offset[II*4 + j].z = 0.0f;
          Offset[II*4 + j].w = 0.0f;

          Scale[II*4 + j].x = Width;
          Scale[II*4 + j].y = Height;
          Scale[II*4 + j].z = 1.0f;
          Scale[II*4 + j].w = 1.0f;
        }

        if(glTexture->m_Texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
        {
          //upper left
          UV[II*4 + 0].x = CharX;
          UV[II*4 + 0].y = CharY;
          UV[II*4 + 0].z = 0.0f;
          UV[II*4 + 0].w = 0.0f;

          //upper right
          UV[II*4 + 1].x = (CharX + Width);
          UV[II*4 + 1].y = CharY;
          UV[II*4 + 1].z = 0.0f;
          UV[II*4 + 1].w = 0.0f;

          //lower right
          UV[II*4 + 2].x = (CharX + Width);
          UV[II*4 + 2].y = (CharY + Height);
          UV[II*4 + 2].z = 0.0f;
          UV[II*4 + 2].w = 0.0f;

          //lower left
          UV[II*4 + 3].x = CharX;
          UV[II*4 + 3].y = (CharY + Height);
          UV[II*4 + 3].z = 0.0f;
          UV[II*4 + 3].w = 0.0f;
        }
        else
        {
          //upper left
          UV[II*4 + 0].x = CharX / tex_width;
          UV[II*4 + 0].y = CharY / tex_height;
          UV[II*4 + 0].z = 0.0f;
          UV[II*4 + 0].w = 0.0f;

          //upper right
          UV[II*4 + 1].x = (CharX + Width) / tex_width;
          UV[II*4 + 1].y = CharY / tex_height;
          UV[II*4 + 1].z = 0.0f;
          UV[II*4 + 1].w = 0.0f;

          //lower right
          UV[II*4 + 2].x = (CharX + Width) / tex_width;
          UV[II*4 + 2].y = (CharY + Height) / tex_height;
          UV[II*4 + 2].z = 0.0f;
          UV[II*4 + 2].w = 0.0f;

          //lower left
          UV[II*4 + 3].x = CharX / tex_width;
          UV[II*4 + 3].y = (CharY + Height) / tex_height;
          UV[II*4 + 3].z = 0.0f;
          UV[II*4 + 3].w = 0.0f;
        }
      }

      CurX += abcA + abcB + abcC;
    }

    CHECKGL (glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0) );
    CHECKGL (glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0) );

    int iPosition = 0;
    int iTexUV = 0;
    int iScale = 0;
    int iOffset = 0;

    ObjectPtr<IOpenGLAsmShaderProgram> shader_program;
    if (!USE_ARB_SHADERS)
    {
      m_ShaderProg->Begin();
      int ViewProjectionMatrix = m_ShaderProg->GetUniformLocationARB ("ViewProjectionMatrix");
      Matrix4 mat = GetThreadGraphicsContext()->GetOpenGLModelViewProjectionMatrix();

      m_ShaderProg->SetUniformLocMatrix4fv (ViewProjectionMatrix, 1, false, (float *) &mat);

      iPosition       = m_ShaderProg->GetAttributeLocation ("iPosition");
      iTexUV          = m_ShaderProg->GetAttributeLocation ("iTexUV");
      iScale          = m_ShaderProg->GetAttributeLocation ("iScale");
      iOffset         = m_ShaderProg->GetAttributeLocation ("iOffset");

      int FontTexture     = m_ShaderProg->GetUniformLocationARB ("FontTexture");
      int TextColor       = m_ShaderProg->GetUniformLocationARB ("TextColor");

      GetThreadGraphicsContext()->SetTexture (GL_TEXTURE0, glTexture->m_Texture);

      if (FontTexture != -1)
      {
        CHECKGL ( glUniform1iARB (FontTexture, 0) );
      }

      if (TextColor != -1)
      {
        CHECKGL ( glUniform4fARB (TextColor, color.R(), color.G(), color.B(), color.A() ) );
      }
    }
    else
    {
      shader_program = m_AsmShaderProg;
      if(glTexture->m_Texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
      {
        shader_program = _asm_font_texture_rect_prog;
      }
      shader_program->Begin();

      CHECKGL (glMatrixMode (GL_MODELVIEW));
      CHECKGL (glLoadIdentity ());
      Matrix4 model_view_matrix = GetThreadGraphicsContext()->GetModelViewMatrix ();
      model_view_matrix.Transpose ();
      CHECKGL (glLoadMatrixf ((float *) model_view_matrix.m));

      CHECKGL (glMatrixMode (GL_PROJECTION) );
      CHECKGL (glLoadIdentity ());
      Matrix4 projection_matrix = GetThreadGraphicsContext ()->GetProjectionMatrix ();
      projection_matrix.Transpose ();
      CHECKGL (glLoadMatrixf ((float *) projection_matrix.m));

      iPosition   = VTXATTRIB_POSITION;
      iTexUV      = VTXATTRIB_TEXCOORD0;
      iScale      = VTXATTRIB_TEXCOORD1;
      iOffset     = VTXATTRIB_TEXCOORD2;

      CHECKGL ( glProgramLocalParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 0, color.R(), color.G(), color.B(), color.A() ) );

      GetThreadGraphicsContext()->SetTexture (GL_TEXTURE0, glTexture->m_Texture);
    }

    if (iOffset != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (iOffset) );
      CHECKGL ( glVertexAttribPointerARB (iOffset, 4, GL_FLOAT, GL_FALSE, 16, Offset) );
    }

    if (iPosition != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (iPosition) );
      CHECKGL ( glVertexAttribPointerARB (iPosition, 4, GL_FLOAT, GL_FALSE, 16, Position) );
    }

    if (iScale != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (iScale) );
      CHECKGL ( glVertexAttribPointerARB (iScale, 4, GL_FLOAT, GL_FALSE, 16, Scale) );
    }

    if (iTexUV != -1)
    {
      CHECKGL ( glEnableVertexAttribArrayARB (iTexUV) );
      CHECKGL ( glVertexAttribPointerARB (iTexUV, 4, GL_FLOAT, GL_FALSE, 16, UV) );
    }

    if (NumCharToDraw > 0)
      CHECKGL ( glDrawArrays ( GL_QUADS, 0, NumCharToDraw * 4 ) );

    if (iPosition != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (iPosition) );

    if (iOffset != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (iOffset) );

    if (iScale != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (iScale) );

    if (iTexUV != -1)
      CHECKGL ( glDisableVertexAttribArrayARB (iTexUV) );

    if (!USE_ARB_SHADERS)
    {
      m_ShaderProg->End();
    }
    else
    {
      shader_program->End();
    }

    GetThreadGraphicsContext()->GetRenderStates().SetColorMask (TRUE, TRUE, TRUE, TRUE);
    GetThreadGraphicsContext()->GetRenderStates().SetBlend (FALSE);

    CurX -= x + CURSOR_OFFSET;

    delete [] Position;
    delete [] UV;
    delete [] Scale;
    delete [] Offset;

    return CurX; // number of pixel to offset before writing the next string.
  }

  int FontRenderer::RenderTextToBuffer (float *VertexBuffer, int VBSize,
                                        ObjectPtr<FontTexture> Font, Rect geo, const NString &str, const Color &color, TextAlignment alignment, int NumCharacter)
  {
    nuxAssertMsg (NumCharacter >= 0, TEXT ("[FontRenderer::RenderTextToBuffer] Number of char to draw must be positive.") );
    int NumCharToDraw = 0;

    if (NumCharacter == 0)
      NumCharToDraw = str.Size();
    else
      NumCharToDraw = Min ( (int) str.Size(), NumCharacter);

    nuxAssertMsg (3 * NumCharToDraw * 16 <= VBSize, TEXT ("[FontRenderer::RenderTextToBuffer] VertexBuffer not large enough.") );

    if (3 * NumCharToDraw * 16 > VBSize)
      return 0;

    StringBBox stringBBox;
    PageBBox pageBox;
    pageBox.xmin = geo.x;
    pageBox.xmax = geo.x + geo.GetWidth();
    pageBox.ymin = geo.y;
    pageBox.ymax = geo.y + geo.GetHeight();
    pageBox.x_margin = 0;
    pageBox.y_margin = 0;

    PositionString (Font, str, pageBox, stringBBox, alignment);

    int CurX = stringBBox.x;
    int CurY = stringBBox.y;

    Vector4 *Position = (Vector4 *) VertexBuffer;

    for (int i = 0; i < NumCharToDraw; ++i)
    {
      unsigned char c = static_cast<unsigned char> (str[i]);
      unsigned int CharX = Font->m_Charset.Chars[c /*Str[i]*/].x;
      unsigned int CharY = Font->m_Charset.Chars[c /*Str[i]*/].y;
      unsigned int Width = Font->m_Charset.Chars[c /*Str[i]*/].Width;
      unsigned int Height = Font->m_Charset.Chars[c /*Str[i]*/].Height;
      //int OffsetX = Font->m_Charset.Chars[c /*Str[i]*/].XOffset;
      //int OffsetY = Font->m_Charset.Chars[c /*Str[i]*/].YOffset;
      int abcA = Font->m_Charset.Chars[c /*Str[i]*/].abcA;
      int abcB = Font->m_Charset.Chars[c /*Str[i]*/].abcB;
      int abcC = Font->m_Charset.Chars[c /*Str[i]*/].abcC;
      //int page = Font->m_Charset.Chars[c /*Str[i]*/].page;

      // Position upper left
      //                            Scale       Offset
      Position[i*12 + 0].x = 0.0f  * Width     + (CurX + abcA);    // x
      Position[i*12 + 0].y = 0.0f  * Height    + (CurY);           // y
      Position[i*12 + 0].z = 0.0f  * 1.0f      + 0.0f;             // z
      Position[i*12 + 0].w = 1.0f  * 1.0f      + 0.0f;             // w
      // texture coord
      Position[i*12 + 1].x = CharX;
      Position[i*12 + 1].y = CharY;
      Position[i*12 + 1].z = 0.0f;
      Position[i*12 + 1].w = 1.0f;
      // color
      Position[i*12 + 2].x = color.R();
      Position[i*12 + 2].y = color.G();
      Position[i*12 + 2].z = color.B();
      Position[i*12 + 2].w = color.A();

      // Position lower left
      Position[i*12 + 9].x = 0.0f  * Width     + (CurX + abcA);    // x
      Position[i*12 + 9].y = 1.0f  * Height    + (CurY);           // y
      Position[i*12 + 9].z = 0.0f  * 1.0f      + 0.0f;             // z
      Position[i*12 + 9].w = 1.0f  * 1.0f      + 0.0f;             // w
      // texture coord
      Position[i*12 + 10].x = CharX;
      Position[i*12 + 10].y = CharY + Height;
      Position[i*12 + 10].z = 0.0f;
      Position[i*12 + 10].w = 1.0f;
      // color
      Position[i*12 + 11].x = color.R();
      Position[i*12 + 11].y = color.G();
      Position[i*12 + 11].z = color.B();
      Position[i*12 + 11].w = color.A();

      // Position lower right
      Position[i*12 + 6].x = 1.0f  * Width     + (CurX + abcA);    // x
      Position[i*12 + 6].y = 1.0f  * Height    + (CurY);           // y
      Position[i*12 + 6].z = 0.0f  * 1.0f      + 0.0f;             // z
      Position[i*12 + 6].w = 1.0f  * 1.0f      + 0.0f;             // w
      // texture coord
      Position[i*12 + 7].x = CharX + Width;
      Position[i*12 + 7].y = CharY + Height;
      Position[i*12 + 7].z = 0.0f;
      Position[i*12 + 7].w = 1.0f;
      // color
      Position[i*12 + 8].x = color.R();
      Position[i*12 + 8].y = color.G();
      Position[i*12 + 8].z = color.B();
      Position[i*12 + 8].w = color.A();

      // Position upper right
      Position[i*12 + 3].x = 1.0f  * Width     + (CurX + abcA);    // x
      Position[i*12 + 3].y = 0.0f  * Height    + (CurY);           // y
      Position[i*12 + 3].z = 0.0f  * 1.0f      + 0.0f;             // z
      Position[i*12 + 3].w = 1.0f  * 1.0f      + 0.0f;             // w
      // texture coord
      Position[i*12 + 4].x = CharX + Width;
      Position[i*12 + 4].y = CharY;
      Position[i*12 + 4].z = 0.0f;
      Position[i*12 + 4].w = 1.0f;
      // color
      Position[i*12 + 5].x = color.R();
      Position[i*12 + 5].y = color.G();
      Position[i*12 + 5].z = color.B();
      Position[i*12 + 5].w = color.A();

      CurX += abcA + abcB + abcC;
    }

    CurX -= stringBBox.x + CURSOR_OFFSET;
    return NumCharToDraw;
  }

}
