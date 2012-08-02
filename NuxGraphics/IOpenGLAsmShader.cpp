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
#include "NuxCore/Logger.h"

namespace nux
{

namespace
{
  logging::Logger logger("nux.graphics");
}
  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLAsmShader);
  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLAsmVertexShader);
  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLAsmPixelShader);
  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLAsmShaderProgram);

  bool ExtractShaderString3(const NString &ShaderToken, const NString &ShaderSource, NString &RetSource, NString ShaderPreprocessorDefines);

  IOpenGLAsmShader::IOpenGLAsmShader(std::string const& shader_name, OpenGLResourceType ResourceType)
    : IOpenGLResource(ResourceType)
    , shader_name_(shader_name)
    , compiled_and_ready_(false)
  {
  }

  IOpenGLAsmShader::~IOpenGLAsmShader()
  {
  }

  IOpenGLAsmVertexShader::IOpenGLAsmVertexShader()
    : IOpenGLAsmShader("VertexProgram", RT_GLSL_VERTEXSHADER)
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glGenProgramsARB(1, &_OpenGLID));
#endif
  }

  IOpenGLAsmVertexShader::~IOpenGLAsmVertexShader()
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glDeleteProgramsARB(1, &_OpenGLID));
#endif
  }

  void IOpenGLAsmVertexShader::SetShaderCode(std::string const& shader_code)
  {
    compiled_and_ready_ = false;
    shader_code_ = shader_code;
  }

  bool IOpenGLAsmVertexShader::Compile()
  {
    if (compiled_and_ready_)
    {
      return true;
    }
#ifndef NUX_OPENGLES_20
    if (shader_code_.empty())
    {
      LOG_DEBUG(logger) << "Vertex shader source code is empty.";
    }

    CHECKGL(glBindProgramARB(GL_VERTEX_PROGRAM_ARB, _OpenGLID));
    glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, shader_code_.size(), shader_code_.c_str());

    if ( GL_INVALID_OPERATION == glGetError())
    {
      // Find the error position
      GLint errPos;
      glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );
      // Print implementation-dependent program
      // errors and warnings string.
      LOG_ERROR(logger) << "Error in vertex shader at position: " << errPos << "\n\t" << glGetString(GL_PROGRAM_ERROR_STRING_ARB);
      
      return false;
    }

    compiled_and_ready_ = true;
#endif
    return compiled_and_ready_;
  }

  bool IOpenGLAsmVertexShader::IsValid()
  {
    return compiled_and_ready_;
  }

  IOpenGLAsmPixelShader::IOpenGLAsmPixelShader()
    : IOpenGLAsmShader("PixelProgram", RT_GLSL_PIXELSHADER)
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glGenProgramsARB(1, &_OpenGLID));
#endif
  }

  IOpenGLAsmPixelShader::~IOpenGLAsmPixelShader()
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glDeleteProgramsARB(1, &_OpenGLID));
#endif
  }

  void IOpenGLAsmPixelShader::SetShaderCode(std::string const& shader_code)
  {
    compiled_and_ready_ = false;
    shader_code_ = shader_code;
  }

  bool IOpenGLAsmPixelShader::Compile()
  {
    if (compiled_and_ready_)
    {
      return true;
    }
#ifndef NUX_OPENGLES_20
    if (shader_code_.empty())
    {
      LOG_DEBUG(logger) << "Pixel shader source code is empty.";
    }

    CHECKGL(glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, _OpenGLID));
    glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, shader_code_.size(), shader_code_.c_str());

    if (GL_INVALID_OPERATION == glGetError())
    {
      // Find the error position
      GLint errPos;
      glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );
      // Print implementation-dependent program
      // errors and warnings string.

      LOG_ERROR(logger) << "Error in pixel shader at position: " << errPos << "\n\t" << glGetString(GL_PROGRAM_ERROR_STRING_ARB);
      
      return false;
    }

    compiled_and_ready_ = true;
#endif
    return compiled_and_ready_;
  }

  bool IOpenGLAsmPixelShader::IsValid()
  {
    return compiled_and_ready_;
  }

  IOpenGLAsmShaderProgram::IOpenGLAsmShaderProgram(NString ShaderProgramName)
    :   IOpenGLResource(RT_GLSL_SHADERPROGRAM)
    ,   _ShaderProgramName(ShaderProgramName)
  {
    _OpenGLID = 0;
    m_AsmVertexProgram = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmVertexShader();
    m_AsmFragmentProgram = GetGraphicsDisplay()->GetGpuDevice()->CreateAsmPixelShader();
  }

  IOpenGLAsmShaderProgram::~IOpenGLAsmShaderProgram()
  {
    _OpenGLID = 0;
  }

  void IOpenGLAsmShaderProgram::LoadIShaderFile(const char *ShaderFileName)
  {
    nuxAssertMsg(ShaderFileName, "[IOpenGLAsmShaderProgram::LoadIShaderFile] Invalid shader file name.");
    NUX_RETURN_IF_NULL(ShaderFileName);
    NString SourceCode;
    LoadFileToString(SourceCode, ShaderFileName);
    LoadIShader(&SourceCode[0]);
  }

  void IOpenGLAsmShaderProgram::LoadIShader(const char *ShaderCode)
  {
    nuxAssertMsg(ShaderCode, "[IOpenGLAsmShaderProgram::LoadIShader] Invalid shader code.");
    NUX_RETURN_IF_NULL(ShaderCode);
    NString VertexShaderSource;
    ExtractShaderString3("[Vertex Shader]", ShaderCode, VertexShaderSource, NString(""));
    NString PixelShaderSource;
    ExtractShaderString3("[Fragment Shader]", ShaderCode, PixelShaderSource, NString(""));

    m_AsmVertexProgram->SetShaderCode(&VertexShaderSource[0]);
    m_AsmFragmentProgram->SetShaderCode(&PixelShaderSource[0]);

    m_AsmVertexProgram->Compile();
    m_AsmFragmentProgram->Compile();
  }

  void IOpenGLAsmShaderProgram::LoadVertexShader(const char *glslshader)
  {
    nuxAssertMsg(glslshader, "[IOpenGLAsmShaderProgram::LoadVertexShader] Invalid shader code.");
    NUX_RETURN_IF_NULL(glslshader);
    m_AsmVertexProgram->SetShaderCode(glslshader);
    m_AsmVertexProgram->Compile();
  }

  void IOpenGLAsmShaderProgram::LoadPixelShader(const char *glslshader)
  {
    nuxAssertMsg(glslshader, "[IOpenGLAsmShaderProgram::LoadPixelShader] Invalid shader code.");
    NUX_RETURN_IF_NULL(glslshader);
    m_AsmFragmentProgram->SetShaderCode(glslshader);
    m_AsmFragmentProgram->Compile();
  }

  void IOpenGLAsmShaderProgram::Link()
  {
    m_AsmVertexProgram->Compile();
    m_AsmFragmentProgram->Compile();
  }

  bool IOpenGLAsmShaderProgram::IsValid()
  {
    if (m_AsmVertexProgram->IsValid() && m_AsmFragmentProgram->IsValid())
      return true;

    return false;
  }

  void IOpenGLAsmShaderProgram::Begin(void)
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glEnable(GL_VERTEX_PROGRAM_ARB));
    CHECKGL(glBindProgramARB(GL_VERTEX_PROGRAM_ARB, m_AsmVertexProgram->GetOpenGLID()));
    CHECKGL(glEnable(GL_FRAGMENT_PROGRAM_ARB));
    CHECKGL(glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_AsmFragmentProgram->GetOpenGLID()));
#endif
  }

  void IOpenGLAsmShaderProgram::End(void)
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glDisable(GL_VERTEX_PROGRAM_ARB));
    CHECKGL(glBindProgramARB(GL_VERTEX_PROGRAM_ARB, 0));
    CHECKGL(glDisable(GL_FRAGMENT_PROGRAM_ARB));
    CHECKGL(glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, 0));
#endif
  }

#ifndef NUX_OPENGLES_20
  void IOpenGLAsmShaderProgram::SetVertexEnvParameter4dARB(unsigned int index, double x, double y, double z, double w)
  {
    CHECKGL(glProgramEnvParameter4dARB(GL_VERTEX_PROGRAM_ARB, index, x, y, z, w));
  }

  void IOpenGLAsmShaderProgram::SetVertexEnvParameter4dvARB(unsigned int index, const double *params)
  {
    CHECKGL(glProgramEnvParameter4dvARB(GL_VERTEX_PROGRAM_ARB, index, params));
  }

  void IOpenGLAsmShaderProgram::SetVertexEnvParameter4fARB(unsigned int index, float x, float y, float z, float w)
  {
    CHECKGL(glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, index, x, y, z, w));
  }

  void IOpenGLAsmShaderProgram::SetVertexEnvParameter4fvARB(unsigned int index, const float *params)
  {
    CHECKGL(glProgramEnvParameter4fvARB(GL_VERTEX_PROGRAM_ARB, index, params));
  }

  void IOpenGLAsmShaderProgram::SetVertexLocalParameter4dARB(unsigned int index, double x, double y, double z, double w)
  {
    CHECKGL(glProgramEnvParameter4dARB(GL_VERTEX_PROGRAM_ARB, index, x, y, z, w));
  }

  void IOpenGLAsmShaderProgram::SetVertexLocalParameter4dvARB(unsigned int index, const double *params)
  {
    CHECKGL(glProgramEnvParameter4dvARB(GL_VERTEX_PROGRAM_ARB, index, params));
  }

  void IOpenGLAsmShaderProgram::SetVertexLocalParameter4fARB(unsigned int index, float x, float y, float z, float w)
  {
    CHECKGL(glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, index, x, y, z, w));
  }

  void IOpenGLAsmShaderProgram::SetVertexLocalParameter4fvARB(unsigned int index, const float *params)
  {
    CHECKGL(glProgramEnvParameter4fvARB(GL_VERTEX_PROGRAM_ARB, index, params));
  }

  void IOpenGLAsmShaderProgram::SetFragmentEnvParameter4dARB(unsigned int index, double x, double y, double z, double w)
  {
    CHECKGL(glProgramEnvParameter4dARB(GL_FRAGMENT_PROGRAM_ARB, index, x, y, z, w));
  }

  void IOpenGLAsmShaderProgram::SetFragmentEnvParameter4dvARB(unsigned int index, const double *params)
  {
    CHECKGL(glProgramEnvParameter4dvARB(GL_FRAGMENT_PROGRAM_ARB, index, params));
  }

  void IOpenGLAsmShaderProgram::SetFragmentEnvParameter4fARB(unsigned int index, float x, float y, float z, float w)
  {
    CHECKGL(glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, index, x, y, z, w));
  }

  void IOpenGLAsmShaderProgram::SetFragmentEnvParameter4fvARB(unsigned int index, const float *params)
  {
    CHECKGL(glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, index, params));
  }

  void IOpenGLAsmShaderProgram::SetFragmentLocalParameter4dARB(unsigned int index, double x, double y, double z, double w)
  {
    CHECKGL(glProgramEnvParameter4dARB(GL_FRAGMENT_PROGRAM_ARB, index, x, y, z, w));
  }

  void IOpenGLAsmShaderProgram::SetFragmentLocalParameter4dvARB(unsigned int index, const double *params)
  {
    CHECKGL(glProgramEnvParameter4dvARB(GL_FRAGMENT_PROGRAM_ARB, index, params));
  }

  void IOpenGLAsmShaderProgram::SetFragmentLocalParameter4fARB(unsigned int index, float x, float y, float z, float w)
  {
    CHECKGL(glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, index, x, y, z, w));
  }

  void IOpenGLAsmShaderProgram::SetFragmentLocalParameter4fvARB(unsigned int index, const float *params)
  {
    CHECKGL(glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, index, params));
  }
#endif
}


