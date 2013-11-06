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


#ifndef GLSH_DRAWFUNCTION_H
#define GLSH_DRAWFUNCTION_H

#include "GLShader.h"
#include "GLShaderParameter.h"
#include "NuxCore/Color.h"
#include "NuxCore/Math/Matrix4.h"
#include "GLDeviceObjects.h"

namespace nux
{

  class TemplateQuadBuffer;

  class GLSh_DrawFunction
  {
  public:
    GLSh_DrawFunction();
    ~GLSh_DrawFunction();

    void SetBackgroundColor(Color const& color);
    void SetTextureFunction(ObjectPtr<IOpenGLBaseTexture> device_texture);
    void Render(int x, int y, int z, int width, int height, int WindowWidth, int WindowHeight);
    void SetScreenPositionOffset(float x, float y);

  private:
    Color background_color_;
    ObjectPtr<IOpenGLShaderProgram> sprog;
#ifndef NUX_OPENGLES_20
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmProg;
#endif

    float _ScreenOffsetX, _ScreenOffsetY;
    ObjectPtr<IOpenGLBaseTexture> m_device_texture;
  };
}

#endif // GLSH_DRAWFUNCTION_H
