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


#ifndef GLSH_FILL_H
#define GLSH_FILL_H

#include "IOpenGLGLSLShader.h"
#include "GLShaderParameter.h"
#include "GLDeviceObjects.h"

namespace nux
{

  class TemplateQuadBuffer;

  class GLSh_Fill
  {
  public:
    GLShaderParameter *Color;
    GLShaderParameter *ViewProjectionMatrix;
    GLShaderParameter *Scale;
    GLShaderParameter *Offset;

    GLSh_Fill();
    ~GLSh_Fill();

    void CacheShader();
    void SetColor(FLOAT R, FLOAT G, FLOAT B, FLOAT A);
    void Render(INT x, INT y, INT z, INT width, INT height);

    void SetTransformMatrix(const Matrix4 &TransformMatrix);

  private:
    FLOAT _R, _G, _B, _A;

    ObjectPtr<IOpenGLVertexShader> vs;
    ObjectPtr<IOpenGLPixelShader> ps;
    ObjectPtr<IOpenGLShaderProgram> sprog;
    Matrix4 m_TransformMatrix;
    TemplateQuadBuffer *m_QuadBuffer;
  };
}

#endif // GLSH_FILL_H
