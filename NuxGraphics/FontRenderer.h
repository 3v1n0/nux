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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef FONTRENDERER_H
#define FONTRENDERER_H

#include "GLResource.h"

class IOpenGLPixelShader;

namespace nux
{

  class GraphicsEngine;
  class TextureRectangle;
  class ICgPixelShader;
  class ICgVertexShader;
  class TemplateQuadBuffer;
  class FontTexture;

  class FontRenderer
  {
  public:
    FontRenderer (GraphicsEngine &OpenGLEngine);
    ~FontRenderer();

    GraphicsEngine &m_OpenGLEngine;

    int DrawColorString (ObjectPtr<FontTexture> Font, int x, int y, const NString &str, const Color &color, bool WriteAlphaChannel, int NumCharacter = 0, int SkipFirstNCharacters = 0);

    void PositionString (ObjectPtr<FontTexture> Font, const NString &str, const PageBBox &, StringBBox &, TextAlignment align = eAlignTextCenter, int NumCharacter = 0);
    int RenderColorText (ObjectPtr<FontTexture> Font, int x, int y, const NString &Str, const Color &color, bool WriteAlphaChannel, int NumCharacter);
    int RenderColorTextLineStatic (ObjectPtr<FontTexture> Font, const PageBBox &pageSize, const NString &Str, const Color &color,
                                   bool WriteAlphaChannel, TextAlignment alignment);
    int RenderColorTextLineEdit (ObjectPtr<FontTexture> Font, const PageBBox &pageSize, const NString &Str,
                                 const Color &TextColor,
                                 bool WriteAlphaChannel,
                                 const Color &SelectedTextColor,
                                 const Color &SelectedTextBackgroundColor,
                                 const Color &TextBlinkColor,
                                 const Color &CursorColor,
                                 bool ShowCursor, unsigned int CursorPosition,
                                 int offset = 0, int selection_start = 0, int selection_end = 0);

    int RenderText (ObjectPtr<FontTexture> Font, int x, int y, const NString &str, const Color &color, bool WriteAlphaChannel, int StartCharacter = 0, int NumCharacters = 0);
    int RenderTextToBuffer (
      float *VertexBuffer, int VBSize,
      ObjectPtr<FontTexture> Font, Rect geo, const NString &str, const Color &color, TextAlignment alignment = eAlignTextCenter, int NumCharacter = 0);

  private:
    TemplateQuadBuffer *m_QuadBuffer;
    ObjectPtr<IOpenGLPixelShader> m_PixelShaderProg;
    ObjectPtr<IOpenGLVertexShader> m_VertexShaderProg;
    ObjectPtr<IOpenGLShaderProgram> m_ShaderProg;

    //ObjectPtr<IOpenGLAsmPixelShader> m_AsmPixelShaderProg;
    //ObjectPtr<IOpenGLAsmVertexShader> m_AsmVertexShaderProg;
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmShaderProg;

    ObjectPtr<IOpenGLAsmShaderProgram> _asm_font_texture_rect_prog;
  };

}

#endif //FONTRENDERER_H
