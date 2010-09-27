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


#ifndef GLSH_DRAWFUNCTION_H
#define GLSH_DRAWFUNCTION_H

#include "GLShader.h"
#include "GLShaderParameter.h"
#include "NuxCore/Math/Matrix4.h"
#include "GLDeviceObjects.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class TemplateQuadBuffer;

class GLSh_DrawFunction
{
public:
    GLSh_DrawFunction();
    ~GLSh_DrawFunction();

    void CacheShader();
    void SetBackgroundColor(float R, float G, float B, float A);
    void SetBackgroundColor(Color color);
    void SetTextureFunction(TRefGL<IOpenGLTexture2D> device_texture);
    void SetTransformMatrix(const Matrix4& TransformMatrix)
    {
        m_TransformMatrix = TransformMatrix;
    }
    void Render(int x, int y, int z, int width, int height, int WindowWidth, int WindowHeight);
    void SetScreenPositionOffset(float x, float y);

private:
    float _R, _G, _B, _A;
    TRefGL<IOpenGLShaderProgram> sprog;
    TRefGL<IOpenGLAsmShaderProgram> m_AsmProg;

    float _ScreenOffsetX, _ScreenOffsetY;
    UINT ViewPortX;
    UINT ViewPortY;
    UINT ViewPortWidth;
    UINT ViewPortHeight;
    Matrix4 m_TransformMatrix;
    TRefGL<IOpenGLTexture2D> m_device_texture;

private:
    TemplateQuadBuffer* m_QuadBuffer;
};
} //NUX_NAMESPACE_END

#endif // GLSH_DRAWFUNCTION_H
