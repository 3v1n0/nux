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


#ifndef IOPENGLSHADER_H
#define IOPENGLSHADER_H

namespace nux
{

  class IOpenGLResource;

  bool ExtractShaderString3(const std::string &ShaderToken, const std::string &ShaderSource, std::string &RetSource, std::string ShaderPreprocessorDefines = std::string(""));

  class IOpenGLShader:  public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLShader, IOpenGLResource);
    IOpenGLShader(std::string ShaderName, OpenGLResourceType ResourceType);
    virtual ~IOpenGLShader();
  public:
    virtual void SetShaderCode(const ANSICHAR *ShaderCode, const char *ShaderPreprocessorDefines = "") = 0;
    virtual bool Compile() = 0;
    virtual bool IsValid() = 0;
    std::string _ShaderName;
    std::string _ShaderCode;
  };

  class IOpenGLVertexShader:  public IOpenGLShader
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLVertexShader, IOpenGLShader);
  public:
    virtual ~IOpenGLVertexShader();
    virtual void SetShaderCode(const ANSICHAR *ShaderCode, const char *VtxShaderPreprocessorDefines = "");
    virtual bool Compile();
    virtual bool IsValid();
  private:
    IOpenGLVertexShader(std::string ShaderName = std::string("Vertex Shader"));
    int m_CompiledAndReady;
    friend class GpuDevice;
  };

  class IOpenGLPixelShader:  public IOpenGLShader
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLPixelShader, IOpenGLShader);
  public:
    virtual ~IOpenGLPixelShader();
    virtual void SetShaderCode(const ANSICHAR *ShaderCode, const char *FrgShaderPreprocessorDefines = "");
    virtual bool Compile();
    virtual bool IsValid();
  private:
    IOpenGLPixelShader(std::string ShaderName = std::string("Fragment Shader"));
    int m_CompiledAndReady;
    friend class GpuDevice;
  };

#if 0
  class IOpenGLGeometryShader: public IOpenGLShader
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLGeometryShader, IOpenGLShader);
  public:
    virtual ~IOpenGLGeometryShader();
    virtual void SetShaderCode(const ANSICHAR *ShaderCode, const char *GeometryShaderPreprocessorDefines = "");
    virtual bool Compile();
    virtual bool IsValid();

    void SetInputPrimitiveType(GLenum type);
    void SetOutputPrimitiveType(GLenum type);
    void SetMaxVertexOutput(int max_vertex_output);

  private:
    IOpenGLGeometryShader(std::string ShaderName = std::string("Geometry Shader"));
    int m_CompiledAndReady;
    friend class GpuDevice;
  };
#endif

  class IOpenGLShaderProgram:  public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLShaderProgram, IOpenGLResource);
  public:
    virtual ~IOpenGLShaderProgram();

    // Load an inalogic shader.
    void LoadVertexShader(const char *glslshader, const char *VtxShaderPreprocessorDefines = "");
    void LoadPixelShader(const char *glslshader, const char *FrgShaderPreprocessorDefines = "");
    void LoadIShaderFile(const char *ShaderFileName, const char *VtxShaderPreprocessorDefines = "", const char *FrgShaderPreprocessorDefines = "");
    void LoadIShader(const char *ShaderCode, const char *VtxShaderPreprocessorDefines = "", const char *FrgShaderPreprocessorDefines = "");
    void AddShaderParameter(GLShaderParameter *Parameter);
    void AddShaderObject(ObjectPtr<IOpenGLShader> ShaderObject);
    void RemoveShaderObject(ObjectPtr<IOpenGLShader> ShaderObject);
    void ClearShaderObjects();
    bool Link();

    void Begin();
    void End();

    static void SetShaderTracking(bool enabled);

  public:

    bool SetUniform1f(char *varname, GLfloat v0);
    bool SetUniform2f(char *varname, GLfloat v0, GLfloat v1);
    bool SetUniform3f(char *varname, GLfloat v0, GLfloat v1, GLfloat v2);
    bool SetUniform4f(char *varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

    bool SetUniform1i(char *varname, GLint v0);
    bool SetUniform2i(char *varname, GLint v0, GLint v1);
    bool SetUniform3i(char *varname, GLint v0, GLint v1, GLint v2);
    bool SetUniform4i(char *varname, GLint v0, GLint v1, GLint v2, GLint v3);

    bool SetUniform1fv(char *varname, GLsizei count, GLfloat *value);
    bool SetUniform2fv(char *varname, GLsizei count, GLfloat *value);
    bool SetUniform3fv(char *varname, GLsizei count, GLfloat *value);
    bool SetUniform4fv(char *varname, GLsizei count, GLfloat *value);

    bool SetUniform1iv(char *varname, GLsizei count, GLint *value);
    bool SetUniform2iv(char *varname, GLsizei count, GLint *value);
    bool SetUniform3iv(char *varname, GLsizei count, GLint *value);
    bool SetUniform4iv(char *varname, GLsizei count, GLint *value);

    bool SetUniformMatrix2fv(char *varname, GLsizei count, GLboolean transpose, GLfloat *value);
    bool SetUniformMatrix3fv(char *varname, GLsizei count, GLboolean transpose, GLfloat *value);
    bool SetUniformMatrix4fv(char *varname, GLsizei count, GLboolean transpose, GLfloat *value);


    bool SetUniform1f(GLint loc, GLfloat v0);
    bool SetUniform2f(GLint loc, GLfloat v0, GLfloat v1);
    bool SetUniform3f(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2);
    bool SetUniform4f(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

    bool SetUniform1i(GLint loc, GLint v0);
    bool SetUniform2i(GLint loc, GLint v0, GLint v1);
    bool SetUniform3i(GLint loc, GLint v0, GLint v1, GLint v2);
    bool SetUniform4i(GLint loc, GLint v0, GLint v1, GLint v2, GLint v3);

    bool SetUniform1fv(GLint loc, GLsizei count, GLfloat *value);
    bool SetUniform2fv(GLint loc, GLsizei count, GLfloat *value);
    bool SetUniform3fv(GLint loc, GLsizei count, GLfloat *value);
    bool SetUniform4fv(GLint loc, GLsizei count, GLfloat *value);

    bool SetUniform1iv(GLint loc, GLsizei count, GLint *value);
    bool SetUniform2iv(GLint loc, GLsizei count, GLint *value);
    bool SetUniform3iv(GLint loc, GLsizei count, GLint *value);
    bool SetUniform4iv(GLint loc, GLsizei count, GLint *value);

    bool SetUniformLocMatrix2fv(GLint loc, GLsizei count, GLboolean transpose, GLfloat *value);
    bool SetUniformLocMatrix3fv(GLint loc, GLsizei count, GLboolean transpose, GLfloat *value);
    bool SetUniformLocMatrix4fv(GLint loc, GLsizei count, GLboolean transpose, GLfloat *value);

    void GetUniformfv(char *name, GLfloat *values);
    void GetUniformiv(char *name, GLint *values);
    int GetUniformLocationARB(const GLchar *name);
    void GetActiveUniformARB(GLuint index,
                              GLsizei maxLength,
                              GLsizei *length,
                              GLint *size,
                              GLenum *type,
                              GLchar *name);

    //    glGetObjectParameterARBfv Parameters
    //        object
    //        Specifies the handle of the object to be queried
    //
    //        pname
    //        Specifies the an object parameter. Accepted symbolic names are
    //        GL_OBJECT_TYPE_ARB ,
    //        GL_OBJECT_SUBTYPE_ARB,
    //        GL_OBJECT_DELETE_STATUS_ARB,
    //        GL_OBJECT_LINK_STATUS_ARB,
    //        GL_OBJECT_VALIDATE_STATUS_ARB,
    //        GL_OBJECT_COMPILE_STATUS_ARB,
    //        GL_OBJECT_INFO_LOG_LENGTH_ARB,
    //        GL_OBJECT_ATTACHED_OBJECTS_ARB,
    //        GL_OBJECT_ACTIVE_ATTRIBUTES_ARB,
    //        GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB,
    //        GL_OBJECT_ACTIVE_UNIFORMS_ARB,
    //        GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB,
    //        GL_OBJECT_SHADER_SOURCE_LENGTH_ARB
    //
    //        params
    //        Returns the requested object parameter
    //
    void GetObjectParameterfvARB(GLenum pname, GLfloat *params);
    bool SetSampler(char *name, int exture_unit);

    GLShaderParameter *_FirstParameter;
    void CheckAttributeLocation();
    void CheckUniformLocation();
    int GetAttributeLocation(const char *AttributeName);

  private:
    IOpenGLShaderProgram(std::string ShaderProgramName = std::string("ShaderProgram"));
    ShaderAttributeDefinition m_ProgramAttributeDefinition[16/*NUM_VERTEX_SHADER_INPUT_ATTRIBUTE*/];
    std::vector<ObjectPtr<IOpenGLShader> > ShaderObjectList;
    bool m_CompiledAndReady;
    std::string _ShaderProgramName;
    friend class GpuDevice;
  };

}

#endif // IOPENGLSHADER_H
