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


#include "NuxCore/NKernel.h"
#include "GLResource.h"
#include "GLDeviceFactory.h"
#include "GLDeviceObjects.h"
#include "GLSh_Fill.h"
#include "GLShaderParameter.h"
#include "GLDeviceFactory.h"
#include "GLTemplatePrimitiveBuffer.h"
#include "NuxCore/Math/Matrix4.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  GLSh_Fill::GLSh_Fill()
  {
//     Color                 = new GLShaderParameter(0,  TEXT("ColorFill"),                  eFRAGMENTUNIFORMTYPE, TRUE);
//     ViewProjectionMatrix  = new GLShaderParameter(0,  TEXT("ViewProjectionMatrix"),       eVERTEXUNIFORMTYPE, TRUE);
//     Scale                 = new GLShaderParameter(0,  TEXT("Scale"),                      eVERTEXUNIFORMTYPE, TRUE);
//     Offset                = new GLShaderParameter(0,  TEXT("Offset"),                     eVERTEXUNIFORMTYPE, TRUE);
//
//     vs = GetThreadGLDeviceFactory()->CreateVertexShader();
//     ps = GetThreadGLDeviceFactory()->CreatePixelShader();
//     sprog = GetThreadGLDeviceFactory()->CreateShaderProgram();
//
//     sprog->AddShaderParameter(Color);
//     sprog->AddShaderParameter(ViewProjectionMatrix);
//     sprog->AddShaderParameter(Scale);
//     sprog->AddShaderParameter(Offset);
//
//     NString SourceCode;
//     LoadFileToString(SourceCode, TEXT("..//Shaders//Fill.glsl"));
//     NString VertexShaderSource;
//     ExtractShaderString3(TEXT("[Vertex Shader]"), SourceCode, VertexShaderSource);
//     NString PixelShaderSource;
//     ExtractShaderString3(TEXT("[Fragment Shader]"), SourceCode, PixelShaderSource);
//
//     vs->SetShaderCode(VertexShaderSource.GetTCharPtr());
//     ps->SetShaderCode(PixelShaderSource.GetTCharPtr());
//     vs->Compile();
//     ps->Compile();
//
//     sprog->AddShaderObject(vs);
//     sprog->AddShaderObject(ps);
//     sprog->Link();
//
//     m_QuadBuffer = new TemplateQuadBuffer(GetThreadGLDeviceFactory());
// //     Vector4 v[] =
// //     {
// //         Vector4(1, 0, 0, 0),
// //         Vector4(1, 0, 0, 0),
// //         Vector4(0, 0, 1, 0),
// //         Vector4(0, 0, 1, 0),
// //     };
// //     m_QuadBuffer->SetPerVertexAttribute(1, 4, v);
  }

  GLSh_Fill::~GLSh_Fill()
  {
    NUX_SAFE_DELETE (m_QuadBuffer);
    NUX_SAFE_DELETE (Color);
    NUX_SAFE_DELETE (ViewProjectionMatrix);
    NUX_SAFE_DELETE (Scale);
    NUX_SAFE_DELETE (Offset);
  }

  void GLSh_Fill::SetColor (FLOAT R, FLOAT G, FLOAT B, FLOAT A)
  {
    _R = R;
    _G = G;
    _B = B;
    _A = A;
  }

  void GLSh_Fill::SetTransformMatrix (const Matrix4 &TransformMatrix)
  {
    m_TransformMatrix = TransformMatrix;
  }

  void GLSh_Fill::Render (INT x, INT y, INT z, INT width, INT height)
  {
    return;
    CHECKGL ( glDisable (GL_CULL_FACE) );
    sprog->Begin();

    //Matrix4 ProjectionMatrix;
    //ProjectionMatrix.Orthographic(0, 640, 480, 0, 0.0, 10000.0);
    //ProjectionMatrix.Transpose();
    ViewProjectionMatrix->SetUniformMatrix4fv (1, (GLfloat *) m_TransformMatrix.m, GL_FALSE);

    Scale->SetUniform4f (width, height, 1, 1);
    Offset->SetUniform4f (x, y, 0, 0);
    Color->SetUniform4f (_R, _G, _B, _A);

    INT VertexLocation = sprog->GetAttributeLocation (TEXT ("Vertex") );
    m_QuadBuffer->BindAttribute (VertexLocation, 0);

//     INT VertexColorLocation = sprog->GetAttributeLocation(TEXT("VertexColor"));
//     m_QuadBuffer->BindAttribute(VertexColorLocation, 1);

    m_QuadBuffer->Render (1);

    m_QuadBuffer->UnBindAttribute (VertexLocation);
//     m_QuadBuffer->UnBindAttribute(VertexColorLocation);
    m_QuadBuffer->UnBind(); // Unbind the vertex and index buffer.

    sprog->End();
  }

  void GLSh_Fill::CacheShader()
  {

  }
} //NUX_NAMESPACE_END
