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


#include "GLResource.h"
#include "GraphicsDisplay.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "IOpenGLAsmShader.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (IOpenGLAsmShader);
  NUX_IMPLEMENT_OBJECT_TYPE (IOpenGLAsmVertexShader);
  NUX_IMPLEMENT_OBJECT_TYPE (IOpenGLAsmPixelShader);
  NUX_IMPLEMENT_OBJECT_TYPE (IOpenGLAsmShaderProgram);

  bool ExtractShaderString3 (const NString &ShaderToken, const NString &ShaderSource, NString &RetSource, NString ShaderPreprocessorDefines);

  IOpenGLAsmShader::IOpenGLAsmShader (NString ShaderName, OpenGLResourceType ResourceType)
    :   IOpenGLResource (ResourceType)
    ,   _ShaderName (ShaderName)
  {

  }

  IOpenGLAsmShader::~IOpenGLAsmShader()
  {

  }

  IOpenGLAsmVertexShader::IOpenGLAsmVertexShader (NString ShaderName)
    :   IOpenGLAsmShader (ShaderName, RT_GLSL_VERTEXSHADER)
    ,   m_CompiledAndReady (false)
  {
    CHECKGL ( glGenProgramsARB (1, &_OpenGLID) );
  }

  IOpenGLAsmVertexShader::~IOpenGLAsmVertexShader()
  {
    CHECKGL ( glDeleteProgramsARB (1, &_OpenGLID) );
    _OpenGLID = 0;
    m_CompiledAndReady = false;
  }

  void IOpenGLAsmVertexShader::SetShaderCode (const TCHAR *ShaderCode)
  {
    nuxAssertMsg (ShaderCode, TEXT ("[IOpenGLAsmVertexShader::SetShaderCode] Invalid shader code.") );
    NUX_RETURN_IF_NULL (ShaderCode);
    m_CompiledAndReady = false;
    _ShaderCode = ShaderCode;
  }

  bool IOpenGLAsmVertexShader::Compile()
  {
    m_CompiledAndReady = false;
    t_size CodeSize = _ShaderCode.Size();

    if (CodeSize == 0)
    {
      nuxDebugMsg (TEXT ("[IOpenGLAsmVertexShader::Compile] Vertex shader source code is empty.") );
    }

    char *ShaderSource = new char[CodeSize+1];
    Memset (ShaderSource, 0, CodeSize + 1);
    Memcpy (ShaderSource, TCHAR_TO_ANSI (_ShaderCode.GetTCharPtr() ), CodeSize);

    CHECKGL ( glBindProgramARB (GL_VERTEX_PROGRAM_ARB, _OpenGLID) );
    glProgramStringARB (GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, CodeSize, ShaderSource);

    if ( GL_INVALID_OPERATION == glGetError() )
    {
      // Find the error position
      GLint errPos;
      glGetIntegerv ( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );
      // Print implementation-dependent program
      // errors and warnings string.
      const unsigned char *ErrorString;
      ErrorString = NUX_STATIC_CAST (const unsigned char *, glGetString (GL_PROGRAM_ERROR_STRING_ARB) );
      nuxError (TEXT ("[IOpenGLAsmVertexShader::Compile] Error in vertex shader at position: %d\n%s\n"), errPos, ErrorString );
      return m_CompiledAndReady;
    }

    delete[] ShaderSource;

    m_CompiledAndReady = true;
    return m_CompiledAndReady;
  }

  bool IOpenGLAsmVertexShader::IsValid()
  {
    return m_CompiledAndReady;
  }

  IOpenGLAsmPixelShader::IOpenGLAsmPixelShader (NString ShaderName)
    :   IOpenGLAsmShader (ShaderName, RT_GLSL_PIXELSHADER)
    ,   m_CompiledAndReady (false)
  {
    CHECKGL ( glGenProgramsARB (1, &_OpenGLID) );
  }

  IOpenGLAsmPixelShader::~IOpenGLAsmPixelShader()
  {
    CHECKGL ( glDeleteProgramsARB (1, &_OpenGLID) );
    _OpenGLID = 0;
    m_CompiledAndReady = false;
  }

  void IOpenGLAsmPixelShader::SetShaderCode (const TCHAR *ShaderCode)
  {
    nuxAssertMsg (ShaderCode, TEXT ("[IOpenGLAsmPixelShader::SetShaderCode] Invalid shader code.") );
    NUX_RETURN_IF_NULL (ShaderCode);
    m_CompiledAndReady = false;
    _ShaderCode = ShaderCode;
  }

  bool IOpenGLAsmPixelShader::Compile()
  {
    m_CompiledAndReady = false;
    t_size CodeSize = _ShaderCode.Size();

    if (CodeSize == 0)
    {
      nuxDebugMsg (TEXT ("[IOpenGLAsmPixelShader::Compile] Vertex shader source code is empty.") );
    }

    char *ShaderSource = new char[CodeSize+1];
    Memset (ShaderSource, 0, CodeSize + 1);
    Memcpy (ShaderSource, TCHAR_TO_ANSI (_ShaderCode.GetTCharPtr() ), CodeSize);

    CHECKGL ( glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, _OpenGLID) );
    glProgramStringARB (GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, CodeSize, ShaderSource);

    if ( GL_INVALID_OPERATION == glGetError() )
    {
      // Find the error position
      GLint errPos;
      glGetIntegerv ( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );
      // Print implementation-dependent program
      // errors and warnings string.
      const unsigned char *ErrorString;
      ErrorString = NUX_STATIC_CAST (const unsigned char *, glGetString (GL_PROGRAM_ERROR_STRING_ARB) );
      nuxError (TEXT ("[IOpenGLAsmPixelShader::Compile] Error in fragment shader at position: %d\n%s\n"), errPos, ErrorString );
    }

    delete[] ShaderSource;

    m_CompiledAndReady = true;
    return m_CompiledAndReady;
  }

  bool IOpenGLAsmPixelShader::IsValid()
  {
    return m_CompiledAndReady;
  }

  IOpenGLAsmShaderProgram::IOpenGLAsmShaderProgram (NString ShaderProgramName)
    :   IOpenGLResource (RT_GLSL_SHADERPROGRAM)
    ,   _ShaderProgramName (ShaderProgramName)
  {
    _OpenGLID = 0;
    m_AsmVertexProgram = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmVertexShader();
    m_AsmFragmentProgram = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmPixelShader();
  }

  IOpenGLAsmShaderProgram::~IOpenGLAsmShaderProgram()
  {
    _OpenGLID = 0;
  }

  void IOpenGLAsmShaderProgram::LoadIShaderFile (const TCHAR *ShaderFileName)
  {
    nuxAssertMsg (ShaderFileName, TEXT ("[IOpenGLAsmShaderProgram::LoadIShaderFile] Invalid shader file name.") );
    NUX_RETURN_IF_NULL (ShaderFileName);
    NString SourceCode;
    LoadFileToString (SourceCode, ShaderFileName);
    LoadIShader (&SourceCode[0]);
  }

  void IOpenGLAsmShaderProgram::LoadIShader (const TCHAR *ShaderCode)
  {
    nuxAssertMsg (ShaderCode, TEXT ("[IOpenGLAsmShaderProgram::LoadIShader] Invalid shader code.") );
    NUX_RETURN_IF_NULL (ShaderCode);
    NString VertexShaderSource;
    ExtractShaderString3 (TEXT ("[Vertex Shader]"), ShaderCode, VertexShaderSource, NString ("") );
    NString PixelShaderSource;
    ExtractShaderString3 (TEXT ("[Fragment Shader]"), ShaderCode, PixelShaderSource, NString ("") );

    m_AsmVertexProgram->SetShaderCode (&VertexShaderSource[0]);
    m_AsmFragmentProgram->SetShaderCode (&PixelShaderSource[0]);

    m_AsmVertexProgram->Compile();
    m_AsmFragmentProgram->Compile();
  }

  void IOpenGLAsmShaderProgram::LoadVertexShader (const TCHAR *glslshader)
  {
    nuxAssertMsg (glslshader, TEXT ("[IOpenGLAsmShaderProgram::LoadVertexShader] Invalid shader code.") );
    NUX_RETURN_IF_NULL (glslshader);
    m_AsmVertexProgram->SetShaderCode (glslshader);
    m_AsmVertexProgram->Compile();
  }

  void IOpenGLAsmShaderProgram::LoadPixelShader (const TCHAR *glslshader)
  {
    nuxAssertMsg (glslshader, TEXT ("[IOpenGLAsmShaderProgram::LoadPixelShader] Invalid shader code.") );
    NUX_RETURN_IF_NULL (glslshader);
    m_AsmFragmentProgram->SetShaderCode (glslshader);
    m_AsmFragmentProgram->Compile();
  }

  void IOpenGLAsmShaderProgram::Link()
  {
    m_AsmVertexProgram->Compile();
    m_AsmFragmentProgram->Compile();
  }

  bool IOpenGLAsmShaderProgram::IsValid()
  {
    if (m_AsmVertexProgram->IsValid() && m_AsmFragmentProgram->IsValid() )
      return true;

    return false;
  }

  void IOpenGLAsmShaderProgram::Begin (void)
  {
    CHECKGL ( glEnable (GL_VERTEX_PROGRAM_ARB) );
    CHECKGL ( glBindProgramARB (GL_VERTEX_PROGRAM_ARB, m_AsmVertexProgram->GetOpenGLID() ) );
    CHECKGL ( glEnable (GL_FRAGMENT_PROGRAM_ARB) );
    CHECKGL ( glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, m_AsmFragmentProgram->GetOpenGLID() ) );
  }

  void IOpenGLAsmShaderProgram::End (void)
  {
    CHECKGL ( glDisable (GL_VERTEX_PROGRAM_ARB) );
    CHECKGL ( glBindProgramARB (GL_VERTEX_PROGRAM_ARB, 0) );
    CHECKGL ( glDisable (GL_FRAGMENT_PROGRAM_ARB) );
    CHECKGL ( glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, 0) );
  }

  void IOpenGLAsmShaderProgram::SetVertexEnvParameter4dARB (t_uint32 index, double x, double y, double z, double w)
  {
    CHECKGL ( glProgramEnvParameter4dARB (GL_VERTEX_PROGRAM_ARB, index, x, y, z, w) );
  }

  void IOpenGLAsmShaderProgram::SetVertexEnvParameter4dvARB (t_uint32 index, const double *params)
  {
    CHECKGL ( glProgramEnvParameter4dvARB (GL_VERTEX_PROGRAM_ARB, index, params) );
  }

  void IOpenGLAsmShaderProgram::SetVertexEnvParameter4fARB (t_uint32 index, float x, float y, float z, float w)
  {
    CHECKGL ( glProgramEnvParameter4fARB (GL_VERTEX_PROGRAM_ARB, index, x, y, z, w) );
  }

  void IOpenGLAsmShaderProgram::SetVertexEnvParameter4fvARB (t_uint32 index, const float *params)
  {
    CHECKGL ( glProgramEnvParameter4fvARB (GL_VERTEX_PROGRAM_ARB, index, params) );
  }

  void IOpenGLAsmShaderProgram::SetVertexLocalParameter4dARB (t_uint32 index, double x, double y, double z, double w)
  {
    CHECKGL ( glProgramEnvParameter4dARB (GL_VERTEX_PROGRAM_ARB, index, x, y, z, w) );
  }

  void IOpenGLAsmShaderProgram::SetVertexLocalParameter4dvARB (t_uint32 index, const double *params)
  {
    CHECKGL ( glProgramEnvParameter4dvARB (GL_VERTEX_PROGRAM_ARB, index, params) );
  }

  void IOpenGLAsmShaderProgram::SetVertexLocalParameter4fARB (t_uint32 index, float x, float y, float z, float w)
  {
    CHECKGL ( glProgramEnvParameter4fARB (GL_VERTEX_PROGRAM_ARB, index, x, y, z, w) );
  }

  void IOpenGLAsmShaderProgram::SetVertexLocalParameter4fvARB (t_uint32 index, const float *params)
  {
    CHECKGL ( glProgramEnvParameter4fvARB (GL_VERTEX_PROGRAM_ARB, index, params) );
  }

  void IOpenGLAsmShaderProgram::SetFragmentEnvParameter4dARB (t_uint32 index, double x, double y, double z, double w)
  {
    CHECKGL ( glProgramEnvParameter4dARB (GL_FRAGMENT_PROGRAM_ARB, index, x, y, z, w) );
  }

  void IOpenGLAsmShaderProgram::SetFragmentEnvParameter4dvARB (t_uint32 index, const double *params)
  {
    CHECKGL ( glProgramEnvParameter4dvARB (GL_FRAGMENT_PROGRAM_ARB, index, params) );
  }

  void IOpenGLAsmShaderProgram::SetFragmentEnvParameter4fARB (t_uint32 index, float x, float y, float z, float w)
  {
    CHECKGL ( glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, index, x, y, z, w) );
  }

  void IOpenGLAsmShaderProgram::SetFragmentEnvParameter4fvARB (t_uint32 index, const float *params)
  {
    CHECKGL ( glProgramEnvParameter4fvARB (GL_FRAGMENT_PROGRAM_ARB, index, params) );
  }

  void IOpenGLAsmShaderProgram::SetFragmentLocalParameter4dARB (t_uint32 index, double x, double y, double z, double w)
  {
    CHECKGL ( glProgramEnvParameter4dARB (GL_FRAGMENT_PROGRAM_ARB, index, x, y, z, w) );
  }

  void IOpenGLAsmShaderProgram::SetFragmentLocalParameter4dvARB (t_uint32 index, const double *params)
  {
    CHECKGL ( glProgramEnvParameter4dvARB (GL_FRAGMENT_PROGRAM_ARB, index, params) );
  }

  void IOpenGLAsmShaderProgram::SetFragmentLocalParameter4fARB (t_uint32 index, float x, float y, float z, float w)
  {
    CHECKGL ( glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, index, x, y, z, w) );
  }

  void IOpenGLAsmShaderProgram::SetFragmentLocalParameter4fvARB (t_uint32 index, const float *params)
  {
    CHECKGL ( glProgramEnvParameter4fvARB (GL_FRAGMENT_PROGRAM_ARB, index, params) );
  }

}
