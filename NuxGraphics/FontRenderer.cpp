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
namespace
{
const int CURSOR_OFFSET = 0;
static int CURSOR_SIZE = 2;

  // On NVidia system:
  //  - declare the vertex attribute name before any other attribute.
  //  - Give the vertex attribute a name that comes before any other attribute name. For instance prefix the vertex attribute name with "_".

std::string gFontVtxShader("                  \n\
  attribute vec4 _Position;                         \n\
  attribute vec4 iOffset;                           \n\
  attribute vec4 iScale;                            \n\
  attribute vec4 iTexUV;                            \n\
  uniform mat4 ViewProjectionMatrix;                \n\
  varying vec4 oTexCoord0;                          \n\
  void main()                                       \n\
  {                                                 \n\
  oTexCoord0   = iTexUV;                            \n\
  vec4 myvertex = _Position * iScale + iOffset;     \n\
  gl_Position  = ViewProjectionMatrix * myvertex;   \n\
  }");

std::string gFontFragShader("                                         \n\
  #extension GL_ARB_texture_rectangle : enable                              \n\
  #ifdef GL_ES                                                              \n\
  precision mediump float;                                                  \n\
  #endif                                                                    \n\
  uniform vec4 TextColor;                                                   \n\
  varying vec4 oTexCoord0;                                                  \n\
  #ifdef SAMPLERTEX2D                                                       \n\
  uniform sampler2D FontTexture;                                            \n\
  vec4 SampleTexture(sampler2D TexObject, vec4 TexCoord)                    \n\
  {                                                                         \n\
    return texture2D(TexObject, TexCoord.st);                               \n\
  }                                                                         \n\
  #elif defined SAMPLERTEX2DRECT                                            \n\
  uniform sampler2DRect FontTexture;                                        \n\
  vec4 SampleTexture(sampler2DRect TexObject, vec4 TexCoord)                \n\
  {                                                                         \n\
    return texture2DRect(TexObject, TexCoord.st);                           \n\
  }                                                                         \n\
  #endif                                                                    \n\
  void main()                                                               \n\
  {                                                                         \n\
    vec4 diffuse = SampleTexture(FontTexture, oTexCoord0);                  \n\
    gl_FragColor = vec4(TextColor.x, TextColor.y, TextColor.z, diffuse.w);  \n\
  }");

std::string FontAsmVtx("!!ARBvp1.0          \n\
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

std::string FontAsmFrg("!!ARBfp1.0            \n\
  PARAM color = program.local[0];                   \n\
  TEMP temp;                                        \n\
  TEMP tex0;                                        \n\
  TEX tex0, fragment.texcoord[0], texture[0], 2D;   \n\
  MOV temp, color;                                  \n\
  MUL temp.w, color, tex0;                          \n\
  MOV result.color, temp;                           \n\
  END");

std::string FontAsmFrgRect("!!ARBfp1.0        \n\
  PARAM color = program.local[0];                   \n\
  TEMP temp;                                        \n\
  TEMP tex0;                                        \n\
  TEX tex0, fragment.texcoord[0], texture[0], RECT; \n\
  MOV temp, color;                                  \n\
  MUL temp.w, color, tex0;                          \n\
  MOV result.color, temp;                           \n\
  END");

} // anon namespace

  FontRenderer::FontRenderer(GraphicsEngine &graphics_engine)
    :   _graphics_engine(graphics_engine)
  {
    if (_graphics_engine.UsingGLSLCodePath())
    {
      _pixel_shader_prog = GetGraphicsDisplay()->GetGpuDevice()->CreatePixelShader();
      _vertex_shader_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateVertexShader();
      _shader_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateShaderProgram();

      _vertex_shader_prog->SetShaderCode(gFontVtxShader.c_str());
#ifndef NUX_OPENGLES_20
      _pixel_shader_prog->SetShaderCode(gFontFragShader.c_str(), "#define SAMPLERTEX2DRECT");
#else
      _pixel_shader_prog->SetShaderCode(gFontFragShader.c_str(), "#define SAMPLERTEX2D");
#endif

      _shader_prog->ClearShaderObjects();
      _shader_prog->AddShaderObject(_vertex_shader_prog);
      _shader_prog->AddShaderObject(_pixel_shader_prog);
      //CHECKGL(glBindAttribLocation(_shader_prog->GetOpenGLID(), 0, "_Position"));
      _shader_prog->Link();
    }
#ifndef NUX_OPENGLES_20
    else
    {
      _asm_shader_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();

      _asm_shader_prog->LoadVertexShader(FontAsmVtx.c_str());
      _asm_shader_prog->LoadPixelShader(FontAsmFrg.c_str());
      _asm_shader_prog->Link();

      _asm_font_texture_rect_prog = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmShaderProgram();
      _asm_font_texture_rect_prog->LoadVertexShader(FontAsmVtx.c_str());
      _asm_font_texture_rect_prog->LoadPixelShader(FontAsmFrgRect.c_str());
      _asm_font_texture_rect_prog->Link();
    }
#endif
  }

  FontRenderer::~FontRenderer()
  {
  }


  void FontRenderer::PositionString(ObjectPtr<FontTexture> const& Font,
                                    std::string const& str,
                                    PageBBox const& pageBBox,
                                    StringBBox& strBBox,
                                    TextAlignment alignment,
                                    int NumCharacter)
  {
    int xmin = pageBBox.xmin + pageBBox.x_margin;
    int xmax = pageBBox.xmax - pageBBox.x_margin;
    int ymin = pageBBox.ymin + pageBBox.y_margin;
    int ymax = pageBBox.ymax - pageBBox.y_margin;

    int NumChar = 0;
    int str_len = str.size();

    if (NumCharacter == 0)
      NumChar = str_len;
    else
      NumChar = std::min<int>(str_len, NumCharacter);

    strBBox.width = Font->GetStringWidth(str, NumChar);
    strBBox.height = Font->GetLineHeight();

    int x = 0;
    int y = 0;
    switch(alignment)
    {
      case eAlignTextCenter:

        if (strBBox.width > xmax - xmin)
          x = xmin; // the text is larger than the box: default to eAlignTextLeft for x.
        else
          x = xmin + ((float) (xmax - xmin) - (float) (strBBox.width)) / 2.0f;

        y = ymin + ((float) (ymax - ymin) - (float) (strBBox.height)) / 2.0f;
        break;

      case eAlignTextRight:
        x = xmin + ((float) (xmax - xmin) - (float) (strBBox.width));
        y = ymin + ((float) (ymax - ymin) - (float) (strBBox.height)) / 2.0f;
        break;

      case eAlignTextLeft:
      default:
        x = xmin;
        y = ymin + ((float) (ymax - ymin) - (float) (strBBox.height)) / 2.0f;
        break;
    }

    strBBox.x = x;
    strBBox.y = y;
  }

  int FontRenderer::RenderColorText(ObjectPtr<FontTexture> const& Font,
                                    int x, int y,
                                    std::string const& Str,
                                    Color const& color,
                                    bool WriteAlphaChannel,
                                    int NumCharacter)
  {
    return RenderText(Font, x, y, Str, color, WriteAlphaChannel, 0, NumCharacter);
  }

  int FontRenderer::RenderColorTextLineStatic(ObjectPtr<FontTexture> const& Font,
                                              PageBBox const& pageSize,
                                              std::string const& Str,
                                              Color const& color,
                                              bool WriteAlphaChannel,
                                              TextAlignment alignment)
  {
    StringBBox stringBBox;

    _graphics_engine.PushClippingRectangle(Rect(pageSize.xmin,
                                                pageSize.ymin,
                                                pageSize.xmax - pageSize.xmin,
                                                pageSize.ymax - pageSize.ymin));
    PositionString(Font, Str, pageSize, stringBBox, alignment);
    int off = RenderText(Font, stringBBox.x, stringBBox.y, Str, color, WriteAlphaChannel, 0, Str.size());

    _graphics_engine.PopClippingRectangle();
    return off;
  }

  int FontRenderer::RenderColorTextLineEdit(ObjectPtr<FontTexture> const& Font,
                                            PageBBox const& pageSize,
                                            std::string const& Str,
                                            Color const& TextColor,
                                            bool WriteAlphaChannel,
                                            Color const& SelectedTextColor,
                                            Color const& SelectedTextBackgroundColor,
                                            Color const& TextBlinkColor,
                                            Color const& CursorColor,
                                            bool ShowCursor, unsigned int CursorPosition,
                                            int offset, int selection_start, int selection_end)
  {
    StringBBox stringBBox;
    Color selection_color(0xFF888888);

    // TODO: sanity checking on selection_start and end
    std::string substring = Str.substr(selection_start, selection_end - selection_start);
    unsigned int substring_width = Font->GetStringWidth(substring);
    int substring_pos = Font->GetStringWidth(Str, selection_start);

    _graphics_engine.PushClippingRectangle(Rect(pageSize.xmin, pageSize.ymin, pageSize.xmax - pageSize.xmin, pageSize.ymax - pageSize.ymin));

    if (substring_width > 0)
      _graphics_engine.QRP_Color(pageSize.xmin + offset + substring_pos, pageSize.ymin, substring_width, pageSize.ymax - pageSize.ymin, SelectedTextBackgroundColor);

    _graphics_engine.PopClippingRectangle();

    // TODO: double check this, pushing exactly sthe same rect as above
    _graphics_engine.PushClippingRectangle(Rect(pageSize.xmin, pageSize.ymin, pageSize.xmax - pageSize.xmin, pageSize.ymax - pageSize.ymin));

    PositionString(Font, Str, pageSize, stringBBox, eAlignTextLeft);

    // Part before selected text
    int off = RenderText(Font, stringBBox.x + offset, stringBBox.y, Str, TextColor, WriteAlphaChannel, 0, selection_start);
    // Selection part
    off = RenderText(Font, stringBBox.x + offset, stringBBox.y, Str, SelectedTextColor, WriteAlphaChannel, selection_start, selection_end - selection_start);
    // Part after selected text
    off = RenderText(Font, stringBBox.x + offset, stringBBox.y, Str, TextColor, WriteAlphaChannel, selection_end, Str.size() - selection_end);

    _graphics_engine.PopClippingRectangle();

    if (ShowCursor)
    {
      // Render Cursor
      std::string temp = Str.substr(0, CursorPosition);
      int w = Font->GetStringWidth(temp);

      int x = pageSize.xmin + w + offset + CURSOR_OFFSET;
      x = (x >= pageSize.xmax) ? pageSize.xmax - 1 : x;
      _graphics_engine.PushClippingRectangle(Rect(x, pageSize.ymin, CURSOR_SIZE, pageSize.ymax - pageSize.ymin));

      _graphics_engine.QRP_Color(x, pageSize.ymin, CURSOR_SIZE, pageSize.ymax - pageSize.ymin, CursorColor);

      RenderText(Font, stringBBox.x + offset, stringBBox.y, Str, TextBlinkColor, WriteAlphaChannel, CursorPosition, 1);
      _graphics_engine.PopClippingRectangle();
    }

    return off;
  }

  int FontRenderer::RenderText(ObjectPtr<FontTexture> const& Font,
                               int x, int y,
                               std::string const& str,
                               Color const& color,
                               bool WriteAlphaChannel,
                               int StartCharacter,
                               int NumCharacters)
  {
    // !WARNING This call works if all the glyph of the font are in a single texture.

    int StrLength = str.size();

    if (StrLength <= 0)
      return 0;

    nuxAssertMsg(NumCharacters >= 0, "[FontRenderer::RenderText] Incorrect value for NumCharacters.");
    nuxAssertMsg(StartCharacter >= 0, "[FontRenderer::RenderText] Incorrect value for StartCharacter.");
    nuxAssertMsg(StartCharacter <= StrLength, "[FontRenderer::RenderText] Incorrect value for StartCharacter.");

    int NumCharToDraw = std::min<int>(StrLength - StartCharacter, NumCharacters);

    if (NumCharToDraw <= 0)
      return 0;

    CHECKGL(glDisable(GL_CULL_FACE));
    int CurX = x;
    int CurY = y;
    _graphics_engine.GetRenderStates().SetBlend(TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _graphics_engine.GetRenderStates().SetColorMask(TRUE, TRUE, TRUE, WriteAlphaChannel); // Do not write the alpha of characters

    GLshort *Index = new GLshort[StrLength*6];
    Vector4 *Position = new Vector4[StrLength*4];
    Vector4 *UV = new Vector4[StrLength*4];
    Vector4 *Offset = new Vector4[StrLength*4];
    Vector4 *Scale = new Vector4[StrLength*4];

    ObjectPtr<CachedBaseTexture> glTexture = _graphics_engine.ResourceCache.GetCachedResource(Font->TextureArray[0]);

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

      if ((i >= StartCharacter) && (i < StartCharacter + NumCharToDraw))
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

        if (glTexture->m_Texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
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

      // Set up element array indices
      Index[i*6 + 0] = i*4;
      Index[i*6 + 1] = i*4 + 2;
      Index[i*6 + 2] = i*4 + 3;

      Index[i*6 + 3] = i*4;
      Index[i*6 + 4] = i*4 + 1;
      Index[i*6 + 5] = i*4 + 2;

      CurX += abcA + abcB + abcC;
    }

    CHECKGL(glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0));
    CHECKGL(glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0));

    int in_attrib_position = 0;
    int in_attrib_tex_uv = 0;
    int in_attrib_scale = 0;
    int in_attrib_offset = 0;

#ifndef NUX_OPENGLES_20
    ObjectPtr<IOpenGLAsmShaderProgram> shader_program;
#endif
    if (_graphics_engine.UsingGLSLCodePath())
    {
      _shader_prog->Begin();
      int ViewProjectionMatrix = _shader_prog->GetUniformLocationARB("ViewProjectionMatrix");
      Matrix4 mat = _graphics_engine.GetOpenGLModelViewProjectionMatrix();

      _shader_prog->SetUniformLocMatrix4fv(ViewProjectionMatrix, 1, false, (float *) &mat);

      in_attrib_position = _shader_prog->GetAttributeLocation("_Position");
      in_attrib_tex_uv   = _shader_prog->GetAttributeLocation("iTexUV");
      in_attrib_scale    = _shader_prog->GetAttributeLocation("iScale");
      in_attrib_offset   = _shader_prog->GetAttributeLocation("iOffset");

      int FontTexture    = _shader_prog->GetUniformLocationARB("FontTexture");
      int TextColor      = _shader_prog->GetUniformLocationARB("TextColor");

      _graphics_engine.SetTexture(GL_TEXTURE0, glTexture->m_Texture);

      if (FontTexture != -1)
      {
        CHECKGL(glUniform1iARB(FontTexture, 0));
      }

      if (TextColor != -1)
      {
        CHECKGL(glUniform4fARB(TextColor, color.red, color.green, color.blue, color.alpha));
      }
    }
#ifndef NUX_OPENGLES_20
    else
    {
      shader_program = _asm_shader_prog;
      if (glTexture->m_Texture->Type().IsDerivedFromType(IOpenGLRectangleTexture::StaticObjectType))
      {
        shader_program = _asm_font_texture_rect_prog;
      }
      shader_program->Begin();

      CHECKGL(glMatrixMode(GL_MODELVIEW));
      CHECKGL(glLoadIdentity());
      Matrix4 model_view_matrix = _graphics_engine.GetModelViewMatrix();
      model_view_matrix.Transpose();
      CHECKGL(glLoadMatrixf((float *) model_view_matrix.m));

      CHECKGL(glMatrixMode(GL_PROJECTION));
      CHECKGL(glLoadIdentity());
      Matrix4 projection_matrix = GetGraphicsDisplay()->GetGraphicsEngine()->GetProjectionMatrix();
      projection_matrix.Transpose();
      CHECKGL(glLoadMatrixf((float *) projection_matrix.m));

      in_attrib_position   = VTXATTRIB_POSITION;
      in_attrib_tex_uv      = VTXATTRIB_TEXCOORD0;
      in_attrib_scale      = VTXATTRIB_TEXCOORD1;
      in_attrib_offset     = VTXATTRIB_TEXCOORD2;

      CHECKGL(glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, color.red, color.green, color.blue, color.alpha ));

      _graphics_engine.SetTexture(GL_TEXTURE0, glTexture->m_Texture);
    }
#endif

    if (in_attrib_offset != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(in_attrib_offset));
      CHECKGL(glVertexAttribPointerARB(in_attrib_offset, 4, GL_FLOAT, GL_FALSE, 16, Offset));
    }

    if (in_attrib_position != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(in_attrib_position));
      CHECKGL(glVertexAttribPointerARB(in_attrib_position, 4, GL_FLOAT, GL_FALSE, 16, Position));
    }

    if (in_attrib_scale != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(in_attrib_scale));
      CHECKGL(glVertexAttribPointerARB(in_attrib_scale, 4, GL_FLOAT, GL_FALSE, 16, Scale));
    }

    if (in_attrib_tex_uv != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(in_attrib_tex_uv));
      CHECKGL(glVertexAttribPointerARB(in_attrib_tex_uv, 4, GL_FLOAT, GL_FALSE, 16, UV));
    }

    if (NumCharToDraw > 0)
      CHECKGL(glDrawElements( GL_TRIANGLES, NumCharToDraw * 6, GL_UNSIGNED_SHORT, Index ));

    if (in_attrib_position != -1)
      CHECKGL(glDisableVertexAttribArrayARB(in_attrib_position));

    if (in_attrib_offset != -1)
      CHECKGL(glDisableVertexAttribArrayARB(in_attrib_offset));

    if (in_attrib_scale != -1)
      CHECKGL(glDisableVertexAttribArrayARB(in_attrib_scale));

    if (in_attrib_tex_uv != -1)
      CHECKGL(glDisableVertexAttribArrayARB(in_attrib_tex_uv));

    if (_graphics_engine.UsingGLSLCodePath())
    {
      _shader_prog->End();
    }
    else
    {
#ifndef NUX_OPENGLES_20
      shader_program->End();
#endif
    }

    _graphics_engine.GetRenderStates().SetColorMask(TRUE, TRUE, TRUE, TRUE);
    _graphics_engine.GetRenderStates().SetBlend(FALSE);

    CurX -= x + CURSOR_OFFSET;

    delete [] Index;
    delete [] Position;
    delete [] UV;
    delete [] Scale;
    delete [] Offset;

    return CurX; // number of pixel to offset before writing the next string.
  }

  int FontRenderer::RenderTextToBuffer(float *VertexBuffer, int VBSize,
                                       ObjectPtr<FontTexture> const& Font,
                                       Rect const& geo,
                                       std::string const& str,
                                       Color const& color,
                                       TextAlignment alignment,
                                       int NumCharacter)
  {
    nuxAssertMsg(NumCharacter >= 0, "[FontRenderer::RenderTextToBuffer] Number of char to draw must be positive.");
    int NumCharToDraw = 0;

    if (NumCharacter == 0)
      NumCharToDraw = str.size();
    else
      NumCharToDraw = Min((int) str.size(), NumCharacter);

    nuxAssertMsg(3 * NumCharToDraw * 16 <= VBSize, "[FontRenderer::RenderTextToBuffer] VertexBuffer not large enough.");

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

    PositionString(Font, str, pageBox, stringBBox, alignment);

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
      Position[i*12 + 2].x = color.red;
      Position[i*12 + 2].y = color.green;
      Position[i*12 + 2].z = color.blue;
      Position[i*12 + 2].w = color.alpha;

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
      Position[i*12 + 11].x = color.red;
      Position[i*12 + 11].y = color.green;
      Position[i*12 + 11].z = color.blue;
      Position[i*12 + 11].w = color.alpha;

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
      Position[i*12 + 8].x = color.red;
      Position[i*12 + 8].y = color.green;
      Position[i*12 + 8].z = color.blue;
      Position[i*12 + 8].w = color.alpha;

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
      Position[i*12 + 5].x = color.red;
      Position[i*12 + 5].y = color.green;
      Position[i*12 + 5].z = color.blue;
      Position[i*12 + 5].w = color.alpha;

      CurX += abcA + abcB + abcC;
    }

    CurX -= stringBBox.x + CURSOR_OFFSET;
    return NumCharToDraw;
  }

}
