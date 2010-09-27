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


#ifndef GLSHADER_H
#define GLSHADER_H

#include "NuxCore/NKernel.h"
#include "NuxCore/Error.h"
#include "NuxCore/NOutputDevice.h"

#include "GLResource.h"

namespace nux { //NUX_NAMESPACE_BEGIN
// struct ShaderDefinition
// {
//     NString	    Name;
//     NString     Value;
// };
// 
// void AddShaderDefinition(std::vector<ShaderDefinition>& Definitions,const TCHAR* Name,const TCHAR* Format,...);
// 
// void InsertPreProcessorDefinitions(const NString &ShaderSource, NString &RetSource, NString& ShaderPreprocessorDefines = NString(TEXT("")));
} //NUX_NAMESPACE_END

#endif //GLSHADER_H
