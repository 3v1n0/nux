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
#include "IOpenGLGLSLShader.h"

namespace nux
{
  namespace local
  {
  namespace
  {
    GLuint last_loaded_shader = 0;
    bool enable_tracking = false;
  }
  }

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLShader);
  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLVertexShader);
  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLPixelShader);
  //NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLGeometryShader);
  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLShaderProgram);


  bool ExtractShaderString3(const std::string &ShaderToken, const std::string &ShaderSource, std::string &RetSource, std::string ShaderPreprocessorDefines)
  {
    //Loop for all characters in the string
    if (ShaderToken != "")
    {
      size_t lineStart = 0;
      size_t lineCount = 1;
      bool startTokenFound = false;
      size_t shaderStringStart = 0;
      size_t shaderStartLine   = 1;
      size_t i;

      for (i = 0; i < ShaderSource.length(); i++)
      {
        //Check if the starting character '[' (open bracket) is found at the beginning of the line
        // i counts the characters in the file. lineStart is equal to i at the beginning of the line.
        std::string sub_string = ShaderSource.substr(i, std::string::npos);
        if ((TCharStringNCompare(sub_string.c_str(), "[", 1) == 0) && (lineStart == i))
        {
          if (!startTokenFound)
          {
            //Test for the start token
            if (ShaderSource.find(ShaderToken, 0) == i)
            {
              // Found the shader token
              shaderStringStart = i + ShaderToken.length();
              startTokenFound = true;

              //Set what line the shader was found on
              shaderStartLine = lineCount;
            }
          }
          else
          {
            //Break where the end token was found
            break;
          }
        }

        //If the character is equal to the new line character,
        // The next character must be on the new line
        sub_string = ShaderSource.substr(i, std::string::npos);
        if ((TCharStringNCompare(sub_string.c_str(), "\r", 1) == 0) || (TCharStringNCompare(sub_string.c_str(), "\n", 1) == 0))
        {
          lineStart = i + 1;
        }

        //Count the new lines
        if (TCharStringNCompare(sub_string.c_str(), "\n", 1) == 0)
        {
          lineCount++;
        }
      }

      //If the string was not found, return false
      if (!startTokenFound || shaderStringStart >= i)
      {
        return false;
      }

      //Assign the return string
      RetSource = ShaderSource.substr(shaderStringStart, i - shaderStringStart);

      //Add the line directive to the shader source. See the documentation for GLSL #line directive.
      // GLSL spec: The #version directive must occur in a shader before anything else, except for comments and white space.
      size_t Pos = RetSource.find("#version", 0);

      while (Pos != std::string::npos && RetSource[Pos] != '\n')
      {
        if (RetSource[Pos] == 0)
          break;

        ++Pos;
      }

      if (RetSource[Pos] != 0)
        ++Pos;

      size_t EndOfLinePosition = 0;
      size_t LinePosition = 0;

      while ((EndOfLinePosition = RetSource.find('\n', EndOfLinePosition)) < Pos - 1)
      {
        ++EndOfLinePosition;
        ++LinePosition;
      }

      std::string s = "#line ";
      s += std::to_string((unsigned long long)(LinePosition + shaderStartLine)) + "\n";
      RetSource.insert(Pos, s);

      // Insert the preprocessor definitions before the #line directive
      if (ShaderPreprocessorDefines.length())
        RetSource.insert(Pos, ShaderPreprocessorDefines + std::string("\n"));

      return true;
    }
    else
    {
      // We are not searching for a start token. Return the whole source.
      RetSource = ShaderSource;
      return true;
    }
  }

  static void InsertPreProcessorDefinitions(const std::string &ShaderSource, std::string &RetSource, std::string &ShaderPreprocessorDefines)
  {
    RetSource = ShaderSource;

    if (ShaderPreprocessorDefines.length() == 0)
      return;

    // GLSL spec: The #version directive must occur in a shader before anything else, except for comments and white space.
    size_t Pos = RetSource.find("#version", 0);

    if (Pos != tstring::npos)
    {
      Pos = RetSource.find('\n', Pos);

      if (Pos == std::string::npos)
      {
        // this is most likely an incorrect shader
        Pos = RetSource.size();
        RetSource.insert(Pos, std::string("\n"));
        Pos = RetSource.size();
      }
      else
      {
        // Skip character \n
        Pos++;
      }
    }
    else
    {
      Pos = 0;
    }

    if (ShaderPreprocessorDefines.length())
      RetSource.insert(Pos, ShaderPreprocessorDefines + std::string("\n"));
  }

  IOpenGLShader::IOpenGLShader(std::string ShaderName, OpenGLResourceType ResourceType)
    :   IOpenGLResource(ResourceType)
    ,   _ShaderName(ShaderName)
  {

  }

  IOpenGLShader::~IOpenGLShader()
  {

  }

  IOpenGLVertexShader::IOpenGLVertexShader(std::string ShaderName)
    :   IOpenGLShader(ShaderName, RT_GLSL_VERTEXSHADER)
    ,   m_CompiledAndReady(false)
  {
    _OpenGLID = glCreateShader(GL_VERTEX_SHADER_ARB);
    CHECKGL_MSG( glCreateShader(GL_VERTEX_SHADER_ARB));
  }

  IOpenGLVertexShader::~IOpenGLVertexShader()
  {
    CHECKGL(glDeleteShader(_OpenGLID));
    _OpenGLID = 0;
    m_CompiledAndReady = false;
  }

  void IOpenGLVertexShader::SetShaderCode(const char *ShaderCode, const char *VtxShaderPreprocessorDefines)
  {
    nuxAssertMsg(ShaderCode, "[IOpenGLVertexShader::SetShaderCode] Invalid shader code.");
    NUX_RETURN_IF_NULL(ShaderCode);
    std::string ProcessedShaderSource;
    std::string Defines(VtxShaderPreprocessorDefines);
    InsertPreProcessorDefinitions(ShaderCode, ProcessedShaderSource, Defines);

    m_CompiledAndReady = false;
    _ShaderCode = ProcessedShaderSource;
  }

  bool IOpenGLVertexShader::Compile()
  {
    size_t CodeSize = _ShaderCode.size();

    if (CodeSize == 0)
    {
      nuxDebugMsg("[IOpenGLVertexShader::Compile] Vertex shader source code is empty.");
    }

    char *ShaderSource = new char[CodeSize+1];
    Memset(ShaderSource, 0, CodeSize + 1);
    Memcpy(ShaderSource, _ShaderCode.c_str(), CodeSize);

    CHECKGL(glShaderSource(_OpenGLID, 1, (const GLcharARB **) &ShaderSource, NULL));
    delete [] ShaderSource;

    // compile vertex shader object
    CHECKGL(glCompileShader(_OpenGLID));

    // check if shader compiled
    m_CompiledAndReady = false;
    CHECKGL(glGetShaderiv(_OpenGLID, GL_COMPILE_STATUS, &m_CompiledAndReady));

    if (!m_CompiledAndReady)
    {
      ANSICHAR *InfoLogBuffer = 0;
      GLint InfoLogBufferSize = 0;
      GLint InfoLogReturnSize = 0;
      GLint iLog = 0;

      glGetShaderiv(_OpenGLID, GL_INFO_LOG_LENGTH, &iLog);
      InfoLogBuffer = new ANSICHAR[iLog+1];
      InfoLogBufferSize = iLog + 1;
      glGetShaderInfoLog(_OpenGLID, InfoLogBufferSize, &InfoLogReturnSize, InfoLogBuffer);

      if (InfoLogReturnSize != 0)
      {
        nuxError("[IOpenGLVertexShader::Compile] glCompileShader: %s", InfoLogBuffer);
      }

      delete[] InfoLogBuffer;
    }

    return m_CompiledAndReady;
  }

  bool IOpenGLVertexShader::IsValid()
  {
    return m_CompiledAndReady;
  }

  IOpenGLPixelShader::IOpenGLPixelShader(std::string ShaderName)
    :   IOpenGLShader(ShaderName, RT_GLSL_PIXELSHADER)
    ,   m_CompiledAndReady(false)

  {
    _OpenGLID = glCreateShader(GL_FRAGMENT_SHADER_ARB);
    CHECKGL_MSG( glCreateShader(GL_FRAGMENT_SHADER_ARB));
  }

  IOpenGLPixelShader::~IOpenGLPixelShader()
  {
    CHECKGL(glDeleteShader(_OpenGLID));
    _OpenGLID = 0;
    m_CompiledAndReady = false;
  }

  void IOpenGLPixelShader::SetShaderCode(const char *ShaderCode, const char *FrgShaderPreprocessorDefines)
  {
    nuxAssertMsg(ShaderCode, "[IOpenGLPixelShader::SetShaderCode] Invalid shader code.");
    NUX_RETURN_IF_NULL(ShaderCode);
    std::string ProcessedShaderSource;
    std::string Defines(FrgShaderPreprocessorDefines);
    InsertPreProcessorDefinitions(ShaderCode, ProcessedShaderSource, Defines);

    m_CompiledAndReady = false;
    _ShaderCode = ProcessedShaderSource;
  }

  bool IOpenGLPixelShader::Compile()
  {

    GLint CodeSize = (GLint) _ShaderCode.size();

    if (CodeSize == 0)
    {
      nuxDebugMsg("[IOpenGLPixelShader::Compile] Pixel shader source code is empty.");
    }

    char *ShaderSource = new char[CodeSize+1];
    Memset(ShaderSource, 0, CodeSize + 1);
    Memcpy(ShaderSource, _ShaderCode.c_str(), CodeSize);
    CHECKGL(glShaderSource(_OpenGLID, 1, (const GLcharARB **) &ShaderSource, &CodeSize));
    delete [] ShaderSource;

    // compile pixel shader object
    CHECKGL(glCompileShader(_OpenGLID));

    // check if shader compiled
    m_CompiledAndReady = false;
    CHECKGL(glGetShaderiv(_OpenGLID, GL_COMPILE_STATUS, &m_CompiledAndReady));

    if (!m_CompiledAndReady)
    {
      ANSICHAR *InfoLogBuffer = 0;
      GLint InfoLogBufferSize = 0;
      GLint InfoLogReturnSize = 0;
      GLint iLog = 0;

      glGetShaderiv(_OpenGLID, GL_INFO_LOG_LENGTH, &iLog);
      InfoLogBuffer = new ANSICHAR[iLog+1];
      InfoLogBufferSize = iLog + 1;
      glGetShaderInfoLog(_OpenGLID, InfoLogBufferSize, &InfoLogReturnSize, InfoLogBuffer);

      if (InfoLogReturnSize != 0)
      {
        nuxError("[IOpenGLPixelShader::Compile] glCompileShader: %s", InfoLogBuffer);
      }
      
      printf("Error: %s \n COMPILE \n", InfoLogBuffer);

      delete[] InfoLogBuffer;
    }

    return m_CompiledAndReady;
  }

  bool IOpenGLPixelShader::IsValid()
  {
    return m_CompiledAndReady;
  }

#if 0
  IOpenGLGeometryShader::IOpenGLGeometryShader(std::string ShaderName)
    :   IOpenGLShader(ShaderName, RT_GLSL_GEOMETRYSHADER)
    ,   m_CompiledAndReady(false)

  {
    _OpenGLID = glCreateShader(GL_GEOMETRY_SHADER);
    CHECKGL_MSG(glCreateShader(GL_GEOMETRY_SHADER));
  }

  IOpenGLGeometryShader::~IOpenGLGeometryShader()
  {
    CHECKGL(glDeleteShader(_OpenGLID));
    _OpenGLID = 0;
    m_CompiledAndReady = false;
  }

  void IOpenGLGeometryShader::SetShaderCode(const char *ShaderCode, const char *GeometryShaderPreprocessorDefines)
  {
    nuxAssertMsg(ShaderCode, "[IOpenGLGeometryShader::SetShaderCode] Invalid shader code.");
    NUX_RETURN_IF_NULL(ShaderCode);
    std::string ProcessedShaderSource;
    std::string Defines(GeometryShaderPreprocessorDefines);
    InsertPreProcessorDefinitions(ShaderCode, ProcessedShaderSource, Defines);

    m_CompiledAndReady = false;
    _ShaderCode = ProcessedShaderSource;
  }

  bool IOpenGLGeometryShader::Compile()
  {

    GLint CodeSize = (GLint) _ShaderCode.Size();

    if (CodeSize == 0)
    {
      nuxDebugMsg("[IOpenGLGeometryShader::Compile] Pixel shader source code is empty.");
    }

    char *ShaderSource = new char[CodeSize+1];
    Memset(ShaderSource, 0, CodeSize + 1);
    Memcpy(ShaderSource, _ShaderCode.m_string.c_str(), CodeSize);
    CHECKGL(glShaderSource(_OpenGLID, 1, (const GLcharARB **) &ShaderSource, &CodeSize));
    delete [] ShaderSource;

    // compile pixel shader object
    CHECKGL(glCompileShader(_OpenGLID));

    // check if shader compiled
    m_CompiledAndReady = false;
    CHECKGL(glGetShaderiv(_OpenGLID, GL_COMPILE_STATUS, &m_CompiledAndReady));

    if (!m_CompiledAndReady)
    {
      ANSICHAR *InfoLogBuffer = 0;
      GLint InfoLogBufferSize = 0;
      GLint InfoLogReturnSize = 0;
      GLint iLog = 0;

      glGetShaderiv(_OpenGLID, GL_INFO_LOG_LENGTH, &iLog);
      InfoLogBuffer = new ANSICHAR[iLog+1];
      InfoLogBufferSize = iLog + 1;
      glGetShaderInfoLog(_OpenGLID, InfoLogBufferSize, &InfoLogReturnSize, InfoLogBuffer);

      if (InfoLogReturnSize != 0)
      {
        nuxError("[IOpenGLGeometryShader::Compile] glCompileShader: %s", InfoLogBuffer);
      }

      delete InfoLogBuffer;
    }

    return m_CompiledAndReady;
  }

  bool IOpenGLGeometryShader::IsValid()
  {
    return m_CompiledAndReady;
  }

  void IOpenGLGeometryShader::SetInputPrimitiveType(GLenum type)
  {
    CHECKGL(glProgramParameteri(_OpenGLID, GL_GEOMETRY_INPUT_TYPE_EXT, type));
  }

  void IOpenGLGeometryShader::SetOutputPrimitiveType(GLenum type)
  {
    CHECKGL(glProgramParameteri(_OpenGLID, GL_GEOMETRY_OUTPUT_TYPE_EXT, type));
  }

  void IOpenGLGeometryShader::SetMaxVertexOutput(int max_vertex_output)
  {
    CHECKGL(glProgramParameteri(_OpenGLID, GL_GEOMETRY_VERTICES_OUT_EXT, max_vertex_output));
  }
#endif

  IOpenGLShaderProgram::IOpenGLShaderProgram(std::string ShaderProgramName)
    :   IOpenGLResource(RT_GLSL_SHADERPROGRAM)
    ,   _FirstParameter(0)
    ,   m_CompiledAndReady(false)
    ,   _ShaderProgramName(ShaderProgramName)
  {
    _OpenGLID = glCreateProgram();
    CHECKGL_MSG( glCreateProgram());
  }

  IOpenGLShaderProgram::~IOpenGLShaderProgram()
  {
    if (local::last_loaded_shader == _OpenGLID)
    {
      CHECKGL(glUseProgramObjectARB(0));
      local::last_loaded_shader = 0;
    }
      
    CHECKGL(glDeleteProgram(_OpenGLID));
    _OpenGLID = 0;
    m_CompiledAndReady = false;
  }

  void IOpenGLShaderProgram::LoadIShaderFile(const char *ShaderFileName, const char *VtxShaderPreprocessorDefines, const char *FrgShaderPreprocessorDefines)
  {
    nuxAssertMsg(ShaderFileName, "[IOpenGLShaderProgram::LoadIShaderFile] Invalid shader file name.");
    NUX_RETURN_IF_NULL(ShaderFileName);
    std::string SourceCode;
    LoadFileToString(SourceCode, ShaderFileName);
    LoadIShader(&SourceCode[0], VtxShaderPreprocessorDefines, FrgShaderPreprocessorDefines);
  }

  void IOpenGLShaderProgram::LoadIShader(const char *ShaderCode, const char *VtxShaderPreprocessorDefines, const char *FrgShaderPreprocessorDefines)
  {
    nuxAssertMsg(ShaderCode, "[IOpenGLShaderProgram::LoadIShader] Invalid shader code.");
    NUX_RETURN_IF_NULL(ShaderCode);
    std::string VertexShaderSource;
    ExtractShaderString3("[Vertex Shader]", ShaderCode, VertexShaderSource, std::string(VtxShaderPreprocessorDefines));
    std::string PixelShaderSource;
    ExtractShaderString3("[Fragment Shader]", ShaderCode, PixelShaderSource, std::string(FrgShaderPreprocessorDefines));

    ObjectPtr<IOpenGLVertexShader> vs = GetGraphicsDisplay()->GetGpuDevice()->CreateVertexShader(); //new IOpenGLVertexShader;
    ObjectPtr<IOpenGLPixelShader> ps = GetGraphicsDisplay()->GetGpuDevice()->CreatePixelShader(); //new IOpenGLPixelShader;

    vs->SetShaderCode(VertexShaderSource.c_str());
    ps->SetShaderCode(PixelShaderSource.c_str());
    vs->Compile();
    ps->Compile();

    ShaderObjectList.clear();

    AddShaderObject(vs);
    AddShaderObject(ps);
  }

  void IOpenGLShaderProgram::LoadVertexShader(const char *glslshader, const char *VtxShaderPreprocessorDefines)
  {
    nuxAssertMsg(glslshader, "[IOpenGLShaderProgram::LoadVertexShader] Invalid shader code.");
    NUX_RETURN_IF_NULL(glslshader);
    ObjectPtr<IOpenGLVertexShader> vs = GetGraphicsDisplay()->GetGpuDevice()->CreateVertexShader(); //new IOpenGLVertexShader;

    std::string ProcessedShaderSource;

    if(!VtxShaderPreprocessorDefines)
    {
      VtxShaderPreprocessorDefines = "";
    }
    std::string Defines(VtxShaderPreprocessorDefines);
    InsertPreProcessorDefinitions(glslshader, ProcessedShaderSource, Defines);

    vs->SetShaderCode(glslshader);
    vs->Compile();
    AddShaderObject(vs);
  }

  void IOpenGLShaderProgram::LoadPixelShader(const char *glslshader, const char *FrgShaderPreprocessorDefines)
  {
    nuxAssertMsg(glslshader, "[IOpenGLShaderProgram::LoadPixelShader] Invalid shader code.");
    NUX_RETURN_IF_NULL(glslshader);
    ObjectPtr<IOpenGLPixelShader> ps = GetGraphicsDisplay()->GetGpuDevice()->CreatePixelShader(); //new IOpenGLPixelShader;

    std::string ProcessedShaderSource;

    if (!FrgShaderPreprocessorDefines)
    {
      FrgShaderPreprocessorDefines = "";
    }
    std::string Defines(FrgShaderPreprocessorDefines);
    InsertPreProcessorDefinitions(glslshader, ProcessedShaderSource, Defines);

    ps->SetShaderCode(glslshader);
    ps->Compile();
    AddShaderObject(ps);
  }

  void IOpenGLShaderProgram::AddShaderObject(ObjectPtr<IOpenGLShader> ShaderObject)
  {
    ShaderObjectList.push_back(ShaderObject);
  }

  void IOpenGLShaderProgram::AddShaderParameter(GLShaderParameter* parameter)
  {
    GLShaderParameter* temp = _FirstParameter;

    while (temp)
    {
      if (temp == parameter)
      {
        // Parameter already added
        return;
      }
      temp = temp->m_NextParameter;
    }

    parameter->m_NextParameter = _FirstParameter;
    _FirstParameter = parameter;

    // If we add shader parameters after the program is linked, we need to call CheckUniformLocation().
    CheckUniformLocation();
  }

  void IOpenGLShaderProgram::RemoveShaderObject(ObjectPtr<IOpenGLShader> ShaderObject)
  {
    std::vector< ObjectPtr<IOpenGLShader> >::iterator it = find(ShaderObjectList.begin(), ShaderObjectList.end(), ShaderObject);

    if (it != ShaderObjectList.end())
    {
      ShaderObjectList.erase(it);
    }
  }

  void IOpenGLShaderProgram::ClearShaderObjects()
  {
    ShaderObjectList.clear();
  }

  bool IOpenGLShaderProgram::Link()
  {
    // Get the number of attached shaders.
    GLint NumAttachedShaders;
    CHECKGL(glGetProgramiv(_OpenGLID, GL_ATTACHED_SHADERS, &NumAttachedShaders));
    GLuint *ShaderObjects = 0;

    if (NumAttachedShaders)
    {
      ShaderObjects = new GLuint[NumAttachedShaders];
    }

    CHECKGL(glGetAttachedShaders(_OpenGLID, NumAttachedShaders, NULL, ShaderObjects));

    // Detach everything first
    for (int i = 0; i < (int) NumAttachedShaders; i++)
    {
      unsigned int obj = ShaderObjects[i];
      CHECKGL(glDetachShader(_OpenGLID, obj));
    }

    if (NumAttachedShaders)
    {
      delete[] ShaderObjects;
    }

    for (int i = 0; i < (int) ShaderObjectList.size(); i++)
    {
      if (!ShaderObjectList[i]->IsValid())
      {
        if (ShaderObjectList[i]->Compile() == false)
        {
          nuxDebugMsg("[IOpenGLShaderProgram::Link] Attached shader %s does not compile with program: %s.", ShaderObjectList[i]->_ShaderName.c_str(), _ShaderProgramName.c_str());
        }
      }

      unsigned int obj = ShaderObjectList[i]->GetOpenGLID();
      CHECKGL(glAttachShader(_OpenGLID, obj));
    }

    GLint linked;
    CHECKGL(glLinkProgram(_OpenGLID));
    CHECKGL(glGetProgramiv(_OpenGLID, GL_LINK_STATUS, &linked));

    if (linked == GL_FALSE)
    {
      ANSICHAR *InfoLogBuffer = 0;
      GLint InfoLogBufferSize = 0;
      GLint InfoLogReturnSize = 0;
      GLint iLog = 0;
      glGetProgramiv(_OpenGLID, GL_INFO_LOG_LENGTH, &iLog);
      InfoLogBuffer = new ANSICHAR[iLog+1];
      InfoLogBufferSize = iLog + 1;

      glGetProgramInfoLog(_OpenGLID, InfoLogBufferSize, &InfoLogReturnSize, InfoLogBuffer);

      if (InfoLogReturnSize != 0)
      {
        nuxError("[IOpenGLShaderProgram::Link] glLinkProgram: %s", InfoLogBuffer);
      }

      delete[] InfoLogBuffer;
      m_CompiledAndReady = false;
      return m_CompiledAndReady;
    }

    GLint validated;
    // glValidateProgram checks to see whether the executables contained in program can execute given the current OpenGL state.
    CHECKGL(glValidateProgram(_OpenGLID));
    CHECKGL(glGetProgramiv(_OpenGLID, GL_VALIDATE_STATUS, &validated));

    if (validated == GL_FALSE)
    {
      ANSICHAR *InfoLogBuffer = 0;
      GLint InfoLogBufferSize = 0;
      GLint InfoLogReturnSize = 0;
      GLint iLog = 0;
      glGetProgramiv(_OpenGLID, GL_INFO_LOG_LENGTH, &iLog);
      InfoLogBuffer = new ANSICHAR[iLog+1];
      InfoLogBufferSize = iLog + 1;

      glGetProgramInfoLog(_OpenGLID, InfoLogBufferSize, &InfoLogReturnSize, InfoLogBuffer);

      if (InfoLogReturnSize != 0)
      {
        nuxError("[IOpenGLShaderProgram::Link] glValidateProgram: %s", InfoLogBuffer);
      }

      delete [] InfoLogBuffer;
    }

    m_CompiledAndReady = true;

    Begin();
    CheckUniformLocation();
    CheckAttributeLocation();
    End();

    return m_CompiledAndReady;
  }

  void IOpenGLShaderProgram::SetShaderTracking(bool enabled)
  {
    local::enable_tracking = enabled;
    local::last_loaded_shader = 0;
    CHECKGL(glUseProgramObjectARB(0));
  }

  void IOpenGLShaderProgram::Begin(void)
  {
    if (local::last_loaded_shader == _OpenGLID && local::enable_tracking)
      return;
    
    local::last_loaded_shader = _OpenGLID;
    CHECKGL(glUseProgramObjectARB(_OpenGLID));
  }

  void IOpenGLShaderProgram::End(void)
  {
    if (!local::enable_tracking)
      CHECKGL(glUseProgramObjectARB(0));
  }

  void IOpenGLShaderProgram::CheckAttributeLocation()
  {
    //ResetAttributeVariable(m_ProgramAttributeDefinition);
    for (int i = 0; i < NUM_VERTEX_SHADER_INPUT_ATTRIBUTE; i++)
    {
      m_ProgramAttributeDefinition[i].attribute_index = -1;
      m_ProgramAttributeDefinition[i].attribute_name = "";
      m_ProgramAttributeDefinition[i].type = VAT_UNDEFINED;
      m_ProgramAttributeDefinition[i].valid = false;
    }

    char active_attribute_name[256];
    GLsizei length;
    GLint size;
    GLenum type;

    GLint num_active_attributes;
    CHECKGL(glGetObjectParameterivARB(_OpenGLID, GL_OBJECT_ACTIVE_ATTRIBUTES_ARB, &num_active_attributes));


    //         Vertex Attribute Aliasing
    //         GLSL attempts to eliminate aliasing of vertex attributes but this is integral to NVIDIA's hardware
    //         approach and necessary for maintaining compatibility with existing OpenGL applications that NVIDIA customers rely on.
    //         NVIDIA's GLSL implementation therefore does not allow built-in vertex attributes to collide with a
    //         generic vertex attributes that is assigned to a particular vertex attribute index with glBindAttribLocation.
    //         For example, you should not use gl_Normal(a built-in vertex attribute) and also use glBindAttribLocation to
    //         bind a generic vertex attribute named "whatever" to vertex attribute index 2 because gl_Normal aliases to index 2.
    //
    //         Built-in vertex attribute name      Incompatible aliased vertex attribute index
    //         gl_Vertex                           0
    //         gl_Normal                           2
    //         gl_Color                            3
    //         gl_SecondaryColor                   4
    //         gl_FogCoord                         5
    //         gl_MultiTexCoord0                   8
    //         gl_MultiTexCoord1                   9
    //         gl_MultiTexCoord2                   10
    //         gl_MultiTexCoord3                   11
    //         gl_MultiTexCoord4                   12
    //         gl_MultiTexCoord5                   13
    //         gl_MultiTexCoord6                   14
    //         gl_MultiTexCoord7                   15
    //         The compiler will automatically assign vertex shader attribute variables not pre-assigned
    //         by glBindAttribLocation to locations that do not collide with any built-in attribute variables
    //         used by the vertex shader. The assigned locations can be queries with glGetAttribLocation.
    //         This means that a developer only needs to worry about collisions when they are explicitly requesting
    //         an attribute to be bound to a specific location.

    for (int index = 0; index < num_active_attributes; index++)
    {
      glGetActiveAttribARB(_OpenGLID,
                            index,
                            256,
                            &length,
                            &size,
                            &type,
                            active_attribute_name);
      CHECKGL_MSG( glGetActiveAttribARB );
      m_ProgramAttributeDefinition[index].attribute_index = glGetAttribLocationARB(_OpenGLID, active_attribute_name);
      CHECKGL_MSG( glGetAttribLocationARB );
      m_ProgramAttributeDefinition[index].attribute_name = active_attribute_name;
      m_ProgramAttributeDefinition[index].valid = true;

      switch(type)
      {
        case GL_FLOAT:
          m_ProgramAttributeDefinition[index].type = VAT_FLOAT;
          break;
        case GL_FLOAT_VEC2:
          m_ProgramAttributeDefinition[index].type = VAT_FLOAT2;
          break;
        case GL_FLOAT_VEC3:
          m_ProgramAttributeDefinition[index].type = VAT_FLOAT3;
          break;
        case GL_FLOAT_VEC4:
          m_ProgramAttributeDefinition[index].type = VAT_FLOAT4;
          break;

        case GL_FLOAT_MAT2:
        case GL_FLOAT_MAT3:
        case GL_FLOAT_MAT4:
        default:
          //todo
          nuxAssert(0);
      }
    }
  }

  void IOpenGLShaderProgram::CheckUniformLocation()
  {
    GLShaderParameter *parameter = _FirstParameter;

    while (m_CompiledAndReady && parameter)
    {
      int location = glGetUniformLocationARB(_OpenGLID, parameter->m_Name.c_str());
      CHECKGL_MSG(glGetUniformLocationARB(_OpenGLID, parameter->m_Name.c_str()));

      //nuxDebugMsg("[IOpenGLShaderProgram::CheckUniformLocation] prog: %d, name: %s, location index: %d", _OpenGLID, parameter->m_Name.c_str(), location);
      if ((location == -1) && (!parameter->m_bIsOptional))
      {
        nuxDebugMsg("[IOpenGLShaderProgram::CheckUniformLocation] Couldn't find shader program parameter %s \n", parameter->m_Name.c_str());
        nuxAssert(0);
      }

      parameter->m_Index = location;
      parameter = parameter->m_NextParameter;
    }
  }

  int IOpenGLShaderProgram::GetAttributeLocation(const char *AttributeName)
  {
    for (int i = 0; i < 16 /*NUM_VERTEX_SHADER_INPUT_ATTRIBUTE*/; i++)
    {
      if (m_ProgramAttributeDefinition[i].attribute_name == AttributeName)
        return m_ProgramAttributeDefinition[i].attribute_index;
    }

    return -1;
  }

  bool IOpenGLShaderProgram::SetUniform1f(char *varname, GLfloat v0)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform1fARB(loc, v0);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform1f(GLint loc, GLfloat v0)
  {
    if (loc == -1) return false; // can't find variable

    glUniform1fARB(loc, v0);
    return true;
  }


  bool IOpenGLShaderProgram::SetUniform2f(char *varname, GLfloat v0, GLfloat v1)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform2fARB(loc, v0, v1);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform2f(GLint loc, GLfloat v0, GLfloat v1)
  {
    if (loc == -1) return false; // can't find variable

    glUniform2fARB(loc, v0, v1);
    return true;
  }

  bool IOpenGLShaderProgram::SetUniform3f(char *varname, GLfloat v0, GLfloat v1, GLfloat v2)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform3fARB(loc, v0, v1, v2);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform3f(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2)
  {
    if (loc == -1) return false; // can't find variable

    glUniform3fARB(loc, v0, v1, v2);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform4f(char *varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform4fARB(loc, v0, v1, v2, v3);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform4f(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
  {
    if (loc == -1) return false; // can't find variable

    glUniform4fARB(loc, v0, v1, v2, v3);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform1i(char *varname, GLint v0)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform1iARB(loc, v0);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform1i(GLint loc, GLint v0)
  {
    if (loc == -1) return false; // can't find variable

    glUniform1iARB(loc, v0);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform2i(char *varname, GLint v0, GLint v1)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform2iARB(loc, v0, v1);


    return true;
  }
  bool IOpenGLShaderProgram::SetUniform2i(GLint loc, GLint v0, GLint v1)
  {
    if (loc == -1) return false; // can't find variable

    glUniform2iARB(loc, v0, v1);


    return true;
  }

  bool IOpenGLShaderProgram::SetUniform3i(char *varname, GLint v0, GLint v1, GLint v2)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform3iARB(loc, v0, v1, v2);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform3i(GLint loc, GLint v0, GLint v1, GLint v2)
  {
    if (loc == -1) return false; // can't find variable

    glUniform3iARB(loc, v0, v1, v2);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform4i(char *varname, GLint v0, GLint v1, GLint v2, GLint v3)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform4iARB(loc, v0, v1, v2, v3);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform4i(GLint loc, GLint v0, GLint v1, GLint v2, GLint v3)
  {
    if (loc == -1) return false; // can't find variable

    glUniform4iARB(loc, v0, v1, v2, v3);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform1fv(char *varname, GLsizei count, GLfloat *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform1fvARB(loc, count, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform1fv(GLint loc, GLsizei count, GLfloat *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniform1fvARB(loc, count, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform2fv(char *varname, GLsizei count, GLfloat *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform2fvARB(loc, count, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform2fv(GLint loc, GLsizei count, GLfloat *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniform2fvARB(loc, count, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform3fv(char *varname, GLsizei count, GLfloat *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform3fvARB(loc, count, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform3fv(GLint loc, GLsizei count, GLfloat *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniform3fvARB(loc, count, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform4fv(char *varname, GLsizei count, GLfloat *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform4fvARB(loc, count, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform4fv(GLint loc, GLsizei count, GLfloat *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniform4fvARB(loc, count, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform1iv(char *varname, GLsizei count, GLint *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform1ivARB(loc, count, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform1iv(GLint loc, GLsizei count, GLint *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniform1ivARB(loc, count, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform2iv(char *varname, GLsizei count, GLint *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform2ivARB(loc, count, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform2iv(GLint loc, GLsizei count, GLint *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniform2ivARB(loc, count, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform3iv(char *varname, GLsizei count, GLint *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform3ivARB(loc, count, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform3iv(GLint loc, GLsizei count, GLint *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniform3ivARB(loc, count, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniform4iv(char *varname, GLsizei count, GLint *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniform4ivARB(loc, count, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniform4iv(GLint loc, GLsizei count, GLint *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniform4ivARB(loc, count, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniformMatrix2fv(char *varname, GLsizei count, GLboolean transpose, GLfloat *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniformMatrix2fvARB(loc, count, transpose, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniformLocMatrix2fv(GLint loc, GLsizei count, GLboolean transpose, GLfloat *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniformMatrix2fvARB(loc, count, transpose, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniformMatrix3fv(char *varname, GLsizei count, GLboolean transpose, GLfloat *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniformMatrix3fvARB(loc, count, transpose, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniformLocMatrix3fv(GLint loc, GLsizei count, GLboolean transpose, GLfloat *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniformMatrix3fvARB(loc, count, transpose, value);

    return true;
  }

  bool IOpenGLShaderProgram::SetUniformMatrix4fv(char *varname, GLsizei count, GLboolean transpose, GLfloat *value)
  {
    GLint loc = GetUniformLocationARB(varname);

    if (loc == -1) return false; // can't find variable

    glUniformMatrix4fvARB(loc, count, transpose, value);

    return true;
  }
  bool IOpenGLShaderProgram::SetUniformLocMatrix4fv(GLint loc, GLsizei count, GLboolean transpose, GLfloat *value)
  {
    if (loc == -1) return false; // can't find variable

    glUniformMatrix4fvARB(loc, count, transpose, value);

    return true;
  }


  void IOpenGLShaderProgram::GetUniformfv(char *name, GLfloat *values)
  {
    GLint loc;

    loc = glGetUniformLocationARB(_OpenGLID, name);
    CHECKGL_MSG( glGetUniformLocationARB );

    if (loc == -1)
    {
      std::cout << "Error: can't find uniform variable \"" << name << "\"\n";
    }

    CHECKGL(glGetUniformfvARB(_OpenGLID, loc, values));
  }


  void IOpenGLShaderProgram::GetUniformiv(char *name, GLint *values)
  {
    GLint loc;
    loc = glGetUniformLocationARB(_OpenGLID, name);
    CHECKGL_MSG( glGetUniformLocationARB );

    if (loc == -1)
    {
      std::cout << "Error: can't find uniform variable \"" << name << "\"\n";
    }

    CHECKGL(glGetUniformivARB(_OpenGLID, loc, values));
  }

  int IOpenGLShaderProgram::GetUniformLocationARB(const GLcharARB *name)
  {
    GLint loc;
    loc = glGetUniformLocationARB(_OpenGLID, name);
    CHECKGL_MSG( glGetUniformLocationARB );
    return loc;
  }

  void IOpenGLShaderProgram::GetActiveUniformARB(
    GLuint index,
    GLsizei maxLength,
    GLsizei *length,
    GLint *size,
    GLenum *type,
    GLcharARB *name)
  {

    glGetActiveUniformARB(_OpenGLID,
                           index,
                           maxLength,
                           length,
                           size,
                           type,
                           name);
    CHECKGL_MSG(glGetActiveUniformARB);
  }


  void IOpenGLShaderProgram::GetObjectParameterfvARB(GLenum pname,
      GLfloat *params)
  {
#ifndef NUX_OPENGLES_20
    glGetObjectParameterfvARB(_OpenGLID,
                               pname,
                               params);
    CHECKGL_MSG(glGetObjectParameterfvARB);
#endif
  }

  bool IOpenGLShaderProgram::SetSampler(char *name, int texture_unit)
  {
    GLint loc = GetUniformLocationARB(name);

    if (loc == -1) return false; // can't find variable

    glUniform1iARB(loc, texture_unit);
    return true;
  }

}
