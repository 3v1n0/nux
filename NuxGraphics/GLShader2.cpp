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


#include "NuxCore/NuxCore.h"
#include "GLResource.h"
#include "GLShader.h"
#include "GLDeviceObjects.h"

namespace nux
{
// //-----------------------------------------------------------------------------
// bool IOpenGLShaderProgram::SetUniform1f(char* varname, GLfloat v0)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform1fARB(loc, v0);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform1f(GLint loc, GLfloat v0)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//     glUniform1fARB(loc, v0);
//     return true;
// }
//
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform2f(char* varname, GLfloat v0, GLfloat v1)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform2fARB(loc, v0, v1);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform2f(GLint loc, GLfloat v0, GLfloat v1)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//     glUniform2fARB(loc, v0, v1);
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform3f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform3fARB(loc, v0, v1, v2);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform3f(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform3fARB(loc, v0, v1, v2);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform4f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform4fARB(loc, v0, v1, v2, v3);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform4f(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform4fARB(loc, v0, v1, v2, v3);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform1i(char* varname, GLint v0)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform1iARB(loc, v0);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform1i(GLint loc, GLint v0)
// {
//     //if (!useGLSL) return false; // GLSL not available
//     //if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform1iARB(loc, v0);
//
//     return true;
// }
//
// bool IOpenGLShaderProgram::SetUniform2i(char* varname, GLint v0, GLint v1)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform2iARB(loc, v0, v1);
//
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform2i(GLint loc, GLint v0, GLint v1)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform2iARB(loc, v0, v1);
//
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform3i(char* varname, GLint v0, GLint v1, GLint v2)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform3iARB(loc, v0, v1, v2);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform3i(GLint loc, GLint v0, GLint v1, GLint v2)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform3iARB(loc, v0, v1, v2);
//
//     return true;
// }
//
// bool IOpenGLShaderProgram::SetUniform4i(char* varname, GLint v0, GLint v1, GLint v2, GLint v3)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform4iARB(loc, v0, v1, v2, v3);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform4i(GLint loc, GLint v0, GLint v1, GLint v2, GLint v3)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform4iARB(loc, v0, v1, v2, v3);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform1fv(char* varname, GLsizei count, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform1fvARB(loc, count, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform1fv(GLint loc, GLsizei count, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform1fvARB(loc, count, value);
//
//     return true;
// }
//
// bool IOpenGLShaderProgram::SetUniform2fv(char* varname, GLsizei count, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform2fvARB(loc, count, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform2fv(GLint loc, GLsizei count, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform2fvARB(loc, count, value);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform3fv(char* varname, GLsizei count, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform3fvARB(loc, count, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform3fv(GLint loc, GLsizei count, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform3fvARB(loc, count, value);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform4fv(char* varname, GLsizei count, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform4fvARB(loc, count, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform4fv(GLint loc, GLsizei count, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform4fvARB(loc, count, value);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform1iv(char* varname, GLsizei count, GLint *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform1ivARB(loc, count, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform1iv(GLint loc, GLsizei count, GLint *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform1ivARB(loc, count, value);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform2iv(char* varname, GLsizei count, GLint *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform2ivARB(loc, count, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform2iv(GLint loc, GLsizei count, GLint *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform2ivARB(loc, count, value);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform3iv(char* varname, GLsizei count, GLint *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform3ivARB(loc, count, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform3iv(GLint loc, GLsizei count, GLint *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform3ivARB(loc, count, value);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniform4iv(char* varname, GLsizei count, GLint *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform4ivARB(loc, count, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniform4iv(GLint loc, GLsizei count, GLint *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform4ivARB(loc, count, value);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniformMatrix2fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniformMatrix2fvARB(loc, count, transpose, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniformLocMatrix2fv(GLint loc, GLsizei count, GLboolean transpose, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniformMatrix2fvARB(loc, count, transpose, value);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniformMatrix3fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniformMatrix3fvARB(loc, count, transpose, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniformLocMatrix3fv(GLint loc, GLsizei count, GLboolean transpose, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniformMatrix3fvARB(loc, count, transpose, value);
//
//     return true;
// }
// //-----------------------------------------------------------------------------
//
// bool IOpenGLShaderProgram::SetUniformMatrix4fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     GLint loc = GetUniformLocationARB(varname);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniformMatrix4fvARB(loc, count, transpose, value);
//
//     return true;
// }
// bool IOpenGLShaderProgram::SetUniformLocMatrix4fv(GLint loc, GLsizei count, GLboolean transpose, GLfloat *value)
// {
//     //    if (!useGLSL) return false; // GLSL not available
//     //    if (!_noshader) return true;
//
//     if (loc==-1) return false;  // can't find variable
//
//     glUniformMatrix4fvARB(loc, count, transpose, value);
//
//     return true;
// }
//
// //-----------------------------------------------------------------------------
//
// void IOpenGLShaderProgram::GetUniformfv(char* name, GLfloat* values)
// {
//     //     if (!useGLSL) return;
//     GLint loc;
//
//     loc = glGetUniformLocationARB(_OpenGLID, name);
//     if (loc == -1)
//     {
//         std::cout << "Error: can't find uniform variable \"" << name << "\"\n";
//     }
//     glGetUniformfvARB(_OpenGLID, loc, values);
// }
//
// //-----------------------------------------------------------------------------
//
// void IOpenGLShaderProgram::GetUniformiv(char* name, GLint* values)
// {
//     //if (!useGLSL) return;
//     GLint loc;
//     loc = glGetUniformLocationARB(_OpenGLID, name);
//     if (loc == -1)
//     {
//         std::cout << "Error: can't find uniform variable \"" << name << "\"\n";
//     }
//     glGetUniformivARB(_OpenGLID, loc, values);
// }
// //-----------------------------------------------------------------------------
//
// int IOpenGLShaderProgram::GetUniformLocationARB(const GLcharARB* name)
// {
//     GLint loc;
//     loc = glGetUniformLocationARB(_OpenGLID, name);
//     return loc;
// }
//
// //-----------------------------------------------------------------------------
// void IOpenGLShaderProgram::GetActiveUniformARB(
//     GLuint index,
//     GLsizei maxLength,
//     GLsizei *length,
//     GLint *size,
//     GLenum *type,
//     GLcharARB *name)
// {
//
//     glGetActiveUniformARB(_OpenGLID,
//         index,
//         maxLength,
//         length,
//         size,
//         type,
//         name);
//     CHECKGL_MSG(glGetActiveUniformARB);
// }
//
// //-----------------------------------------------------------------------------
//
// void IOpenGLShaderProgram::GetObjectParameterfvARB(GLenum pname,
//                                                    GLfloat *params)
// {
//     glGetObjectParameterfvARB(_OpenGLID,
//         pname,
//         params);
//     CHECKGL_MSG(glGetObjectParameterfvARB);
// }
//
// //-----------------------------------------------------------------------------
//
// BOOL IOpenGLShaderProgram::SetSampler(char* name, int texture_unit)
// {
//     GLint loc = GetUniformLocationARB(name);
//     if (loc==-1) return false;  // can't find variable
//
//     glUniform1iARB(loc, texture_unit);
//     return true;
// }
//
}
