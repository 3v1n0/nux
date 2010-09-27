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


#ifndef GLSH_FILL_H
#define GLSH_FILL_H

#include "IOpenGLGLSLShader.h"
#include "GLShaderParameter.h"
#include "GLDeviceObjects.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class TemplateQuadBuffer;

class GLSh_Fill
{
public:
    GLShaderParameter* Color;
    GLShaderParameter* ViewProjectionMatrix;
    GLShaderParameter* Scale;
    GLShaderParameter* Offset;

    GLSh_Fill();
    ~GLSh_Fill();

    void CacheShader();
    void SetColor(FLOAT R, FLOAT G, FLOAT B, FLOAT A);
    void Render(INT x, INT y, INT z, INT width, INT height);

    void SetTransformMatrix(const Matrix4& TransformMatrix);

private:
    FLOAT _R, _G, _B, _A;

    TRefGL<IOpenGLVertexShader> vs;
    TRefGL<IOpenGLPixelShader> ps;
    TRefGL<IOpenGLShaderProgram> sprog;
    Matrix4 m_TransformMatrix;
    TemplateQuadBuffer* m_QuadBuffer;
};
} //NUX_NAMESPACE_END

#endif // GLSH_FILL_H
