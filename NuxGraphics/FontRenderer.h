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


#ifndef FONTRENDERER_H
#define FONTRENDERER_H

#include <string>
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
    FontRenderer(GraphicsEngine &OpenGLEngine);
    ~FontRenderer();

    void PositionString(ObjectPtr<FontTexture> const& Font,
                        std::string const& str,
                        PageBBox const&,
                        StringBBox&,
                        TextAlignment align = eAlignTextCenter,
                        int NumCharacter = 0);

    int RenderColorText(ObjectPtr<FontTexture> const& Font,
                        int x, int y,
                        std::string const& Str,
                        Color const& color,
                        bool WriteAlphaChannel,
                        int NumCharacter);

    int RenderColorTextLineStatic(ObjectPtr<FontTexture> const& Font,
                                  PageBBox const& pageSize,
                                  std::string const& Str,
                                  Color const& color,
                                  bool WriteAlphaChannel,
                                  TextAlignment alignment);

    int RenderColorTextLineEdit(ObjectPtr<FontTexture> const& Font,
                                PageBBox const& pageSize,
                                std::string const& Str,
                                Color const& TextColor,
                                bool WriteAlphaChannel,
                                Color const& SelectedTextColor,
                                Color const& SelectedTextBackgroundColor,
                                Color const& TextBlinkColor,
                                Color const& CursorColor,
                                bool ShowCursor,
                                unsigned int CursorPosition,
                                int offset = 0,
                                int selection_start = 0,
                                int selection_end = 0);

  private:

    int RenderText(ObjectPtr<FontTexture> const& Font,
                   int x, int y,
                   std::string const& str,
                   Color const& color,
                   bool WriteAlphaChannel,
                   int StartCharacter = 0,
                   int NumCharacters = 0);

    // TODO: delete this
    int RenderTextToBuffer(float *VertexBuffer, int VBSize,
                           ObjectPtr<FontTexture> const& Font,
                           Rect const& geo,
                           std::string const& str,
                           Color const& color,
                           TextAlignment alignment = eAlignTextCenter,
                           int NumCharacter = 0);

  private:
    GraphicsEngine& _graphics_engine;
    ObjectPtr<IOpenGLPixelShader> _pixel_shader_prog;
    ObjectPtr<IOpenGLVertexShader> _vertex_shader_prog;
    ObjectPtr<IOpenGLShaderProgram> _shader_prog;

#ifndef NUX_OPENGLES_20
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_shader_prog;
    ObjectPtr<IOpenGLAsmShaderProgram> _asm_font_texture_rect_prog;
#endif
  };

}

#endif //FONTRENDERER_H
