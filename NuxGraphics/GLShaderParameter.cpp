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
#include "GLShader.h"
#include "GLDeviceObjects.h"
#include "GLShaderParameter.h"

NAMESPACE_BEGIN_OGL

GLShaderParameter::GLShaderParameter(GLProgramObject* Shader,
                  const TCHAR* ParamName,
                  eShaderParameterType ParamType,
                  UBOOL IsOptional,
                  UBOOL IsStatic)
:   m_NextParameter(0)
,   m_ShaderProgram(Shader)
,   m_ShaderProgram2(0)
,   m_Name(ParamName)
,   m_ShaderParameterType(ParamType)
,   m_bIsOptional(IsOptional)
,   m_bIsStatic(IsStatic)
{
//    m_NextParameter = Shader->FirstParameter;
//    Shader->FirstParameter = this;
}

NAMESPACE_END_OGL
