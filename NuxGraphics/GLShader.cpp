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
#include "GLResource.h"
#include "GLShader.h"
#include "GLShaderParameter.h"

namespace nux
{
// //-----------------------------------------------------------------------------
// void AddShaderDefinition(std::vector<ShaderDefinition>& Definitions,const TCHAR* Name,const TCHAR* Format,...)
// {
//     TCHAR	DefinitionText[1024];
//     GET_VARARGS(DefinitionText, NUX_ARRAY_COUNT(DefinitionText), NUX_ARRAY_COUNT(DefinitionText)-1,Format);
//
//     ShaderDefinition	Definition;
//     Definition.Name = Name;
//     Definition.Value = DefinitionText;
//     Definitions.push_back(Definition);
// }
//
// //-----------------------------------------------------------------------------
// bool ExtractShaderString3(const NString &ShaderToken, const NString &ShaderSource, NString &RetSource, NString& ShaderPreprocessorDefines)
// {
//     unsigned int lineStart = 0;
//     unsigned int lineCount = 1;
//     bool startTokenFound = false;
//     int shaderStringStart =0;
//     int shaderStartLine   =1;
//
//
//     //Loop for all characters in the string
//     if(ShaderToken != TEXT(""))
//     {
//         int i;
//         for(i = 0; i< ShaderSource.Length(); i++)
//         {
//             //Check if the starting character '[' (open bracket) is found at the beginning of the line
//             // i counts the characters in the file. lineStart is equal to i at the beginning of the line.
//             if((TCharStringNCompare(&ShaderSource[i],TEXT("["), 1) == 0) && (lineStart == i))
//             {
//                 if(!startTokenFound)
//                 {
//                     //Test for the start token
//                     if(ShaderSource.FindFirstOccurence(ShaderToken) == i)
//                     {
//                         // Found the shader token
//                         shaderStringStart = i + ShaderToken.Length();
//                         startTokenFound = true;
//
//                         //Set what line the shader was found on
//                         shaderStartLine = lineCount;
//                     }
//                 }
//                 else
//                 {
//                     //Break where the end token was found
//                     break;
//                 }
//             }
//
//             //If the character is equal to the new line character,
//             // The next character must be on the new line
//             if((TCharStringNCompare(&ShaderSource[i], TEXT("\r"), 1) == 0) || (TCharStringNCompare(&ShaderSource[i], TEXT("\n"), 1) == 0))
//             {
//                 lineStart = i+1;
//             }
//
//             //Count the new lines
//             if(TCharStringNCompare(&ShaderSource[i], TEXT("\n"), 1) == 0)
//             {
//                 lineCount++;
//             }
//         }
//
//         //If the string was not found, return false
//         if(!startTokenFound || shaderStringStart >= i)
//         {
//             return false;
//         }
//
//         //Assign the return string
//         RetSource = ShaderSource.GetSubString(shaderStringStart, i - shaderStringStart);
//
//         //Add the line directive to the shader source. See the documentation for GLSL #line directive.
//         // GLSL spec: The #version directive must occur in a shader before anything else, except for comments and white space.
//         int Pos = RetSource.FindFirstOccurence(TEXT("#version"));
//         while(RetSource[Pos] != TEXT('\n'))
//         {
//             if(RetSource[Pos] == 0)
//                 break;
//             ++Pos;
//         }
//         if(RetSource[Pos] != 0)
//             ++Pos;
//
//         int EndOfLinePosition = 0;
//         unsigned int LinePosition = 0;
//         while((EndOfLinePosition = RetSource.FindNextOccurence(TEXT('\n'), EndOfLinePosition)) < Pos-1)
//         {
//             ++EndOfLinePosition;
//             ++LinePosition;
//         }
//
//         RetSource.Insert(Pos, NString::Printf(TEXT("#line %u\n"), LinePosition + shaderStartLine));
//
//         // Insert the preprocessor definitions before the #line directive
//         if(ShaderPreprocessorDefines.Length())
//             RetSource.Insert(Pos, ShaderPreprocessorDefines + NString(TEXT('\n')));
//
//         return true;
//     }
//     else
//     {
//         // We are not searching for a start token. Return the whole source.
//         RetSource = ShaderSource;
//         return true;
//     }
// }
//
// void InsertPreProcessorDefinitions(const NString &ShaderSource, NString &RetSource, NString& ShaderPreprocessorDefines)
// {
//     RetSource = ShaderSource;
//
//     if(ShaderPreprocessorDefines.Length() == 0)
//         return;
//
//     // GLSL spec: The #version directive must occur in a shader before anything else, except for comments and white space.
//     int Pos = RetSource.FindFirstOccurence(TEXT("#version"));
//     if(Pos != -1)
//     {
//         Pos = RetSource.FindNextOccurence(TEXT('\n'), Pos);
//         if(Pos == -1)
//         {
//             // this is most likely an incorrect shader
//             Pos = RetSource.Size();
//             RetSource.Insert(Pos, NString(TEXT('\n')));
//             Pos = RetSource.Size();
//         }
//         else
//         {
//             // Skip character \n
//             Pos++;
//         }
//     }
//     else
//     {
//         Pos = 0;
//     }
//
//     if(ShaderPreprocessorDefines.Length())
//         RetSource.Insert(Pos, ShaderPreprocessorDefines + NString(TEXT('\n')));
// }
}
