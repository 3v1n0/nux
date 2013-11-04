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


#ifndef GLSH_COLORPICKER_H
#define GLSH_COLORPICKER_H

#include "IOpenGLGLSLShader.h"
#include "GLShaderParameter.h"
#include "GLDeviceObjects.h"

namespace nux
{
  class GLSh_ColorPicker
  {
  public:
    GLSh_ColorPicker(color::Channel cc);
    ~GLSh_ColorPicker();

    void SetColor(float R, float G, float B, float A);
    void SetScreenPositionOffset(float x, float y);
    void Render(int x, int y, int z, int width, int height, int WindowWidth, int WindowHeight);


  private:
    float _R, _G, _B, _A;
    float _ScreenOffsetX, _ScreenOffsetY;

    ObjectPtr<IOpenGLPixelShader> GlobalPixelShader;
    ObjectPtr<IOpenGLShaderProgram> sprog;
#ifndef NUX_OPENGLES_20
    ObjectPtr<IOpenGLAsmShaderProgram> m_AsmProg;
#endif
  };

}

#endif // GLSH_COLORPICKER_H
