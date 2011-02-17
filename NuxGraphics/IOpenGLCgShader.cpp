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


#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "IOpenGLCgShader.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (ICgShader);
  NUX_IMPLEMENT_OBJECT_TYPE (ICgVertexShader);
  NUX_IMPLEMENT_OBJECT_TYPE (ICgPixelShader);

  ICgShader::ICgShader (NString ShaderName, OpenGLResourceType ResourceType)
    :   IOpenGLResource (ResourceType)
    ,   _ShaderName (ShaderName)
    ,   _CgProgram (0)
  {

  }

  ICgShader::~ICgShader()
  {

  }

  const char *ICgShader::GetProgramString()
  {
    return cgGetProgramString ( _CgProgram, CG_PROGRAM_SOURCE );
  }

  const char *ICgShader::GetProgramBinary()
  {
    return cgGetProgramString ( _CgProgram, CG_COMPILED_PROGRAM );
  }

  CGparameter ICgShader::GetNamedParameter (const TCHAR *parameter)
  {
    return cgGetNamedParameter (_CgProgram, TCHAR_TO_ANSI (parameter) );
  }

  ICgVertexShader::ICgVertexShader (NString ShaderName)
    :   _ready (0)
    ,   ICgShader (ShaderName, RT_GLSL_VERTEXSHADER)
  {

  }

  ICgVertexShader::~ICgVertexShader()
  {
    cgDestroyProgram (_CgProgram);
    _ready = 0;
  }

  void ICgVertexShader::CreateProgram (const ANSICHAR *ShaderCode, const ANSICHAR *EntryPoint)
  {
    CGprofile cg_vprofile = cgGLGetLatestProfile (CG_GL_VERTEX);
    nuxAssertMsg (ShaderCode != 0, TEXT ("[ICgVertexShader::CreateProgram] Invalid shader code.") );

    _ShaderCode = ANSI_TO_TCHAR (ShaderCode);
    _EntryPoint = ANSI_TO_TCHAR (EntryPoint);
    _CgProgram = cgCreateProgram (GetGpuDevice()->GetCgContext(),
                                  CG_SOURCE,
                                  ShaderCode,
                                  cg_vprofile, //CG_PROFILE_VP40
                                  EntryPoint,
                                  0);

    cgGLLoadProgram (_CgProgram);

    _ready = 0;
  }

  void ICgVertexShader::CreateProgramFromFileShaderCode (const TCHAR *Filename, const TCHAR *EntryPoint)
  {
    nuxAssertMsg (Filename != 0, TEXT ("[ICgVertexShader::CreateProgramFromFileShaderCode] Invalid shader file.") );

    if (Filename == 0)
      return;

    LoadFileToString (_ShaderCode, Filename);
    CreateProgram (TCHAR_TO_ANSI (&_ShaderCode[0]), TCHAR_TO_ANSI (EntryPoint) );
    _ready = 0;
  }


  bool ICgVertexShader::Compile()
  {
    return false;
  }

  void ICgVertexShader::BindProgram()
  {
    cgGLBindProgram (_CgProgram);
  }


  bool ICgVertexShader::IsValid()
  {
    return (_ready ? true : false);
  }

  ICgPixelShader::ICgPixelShader (NString ShaderName)
    :   _ready (0)
    ,   ICgShader (ShaderName, RT_GLSL_PIXELSHADER)
  {
  }

  ICgPixelShader::~ICgPixelShader()
  {
    cgDestroyProgram (_CgProgram);
    _ready = 0;
  }

  void ICgPixelShader::CreateProgram (const ANSICHAR *ShaderCode, const ANSICHAR *EntryPoint)
  {
    CGprofile cg_fprofile = cgGLGetLatestProfile (CG_GL_FRAGMENT);
    nuxAssertMsg (ShaderCode != 0, TEXT ("[ICgPixelShader::CreateProgram] Invalid shader code.") );

    _ShaderCode = ANSI_TO_TCHAR (ShaderCode);
    _EntryPoint = ANSI_TO_TCHAR (EntryPoint);
    _CgProgram = cgCreateProgram (GetGpuDevice()->GetCgContext(),
                                  CG_SOURCE,
                                  ShaderCode,
                                  cg_fprofile, //CG_PROFILE_FP40,
                                  EntryPoint,
                                  0);

    cgGLLoadProgram (_CgProgram);
    _ready = 0;
  }

  void ICgPixelShader::CreateProgramFromFileShaderCode (const TCHAR *Filename, const TCHAR *EntryPoint)
  {
    nuxAssertMsg (Filename != 0, TEXT ("[ICgPixelShader::CreateProgramFromFileShaderCode] Invalid shader file.") );

    if (Filename == 0)
      return;

    LoadFileToString (_ShaderCode, Filename);
    CreateProgram (TCHAR_TO_ANSI (&_ShaderCode[0]), TCHAR_TO_ANSI (EntryPoint) );
    _ready = 0;
  }

  bool ICgPixelShader::Compile()
  {

    return false;
  }

  void ICgPixelShader::BindProgram()
  {
    cgGLBindProgram (_CgProgram);
  }

  bool ICgPixelShader::IsValid()
  {
    return (_ready ? true : false);
  }

  void cgErrorCallback (void)
  {
    CGerror LastError = cgGetError();

    if (LastError)
    {
      nuxDebugMsg (TEXT ("%s\n\n"), cgGetErrorString (LastError) );
      nuxDebugMsg (TEXT ("%s\n"), cgGetLastListing (GetGpuDevice()->GetCgContext() ) );
      nuxDebugMsg (TEXT ("Cg error, exiting...\n") );
      nuxAssertMsg (0, TEXT ("[cgErrorCallback]: Cg Error.") );
    }
  }
}
