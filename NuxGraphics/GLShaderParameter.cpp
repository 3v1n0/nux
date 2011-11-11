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


#include "NuxCore/NuxCore.h"
#include "GLShader.h"
#include "GLDeviceObjects.h"
#include "GLShaderParameter.h"

namespace nux
{

  GLShaderParameter::GLShaderParameter(GLProgramObject *Shader,
                                        const char *ParamName,
                                        eShaderParameterType ParamType,
                                        bool IsOptional,
                                        bool IsStatic)
    :   m_ShaderParameterType(ParamType)
    ,   m_Name(ParamName)
    ,   m_bIsOptional(IsOptional)
    ,   m_bIsStatic(IsStatic)
    ,   m_ShaderProgram(Shader)
    ,   m_ShaderProgram2(0)
    ,   m_NextParameter(0)
  {
    m_Index = -1;
  }

}
