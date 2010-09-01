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


#ifndef IOPENGLSHADER_H
#define IOPENGLSHADER_H

NAMESPACE_BEGIN_OGL

class IOpenGLResource;

bool ExtractShaderString3(const NString &ShaderToken, const NString &ShaderSource, NString &RetSource, NString ShaderPreprocessorDefines = NString(TEXT("")));

class IOpenGLShader:  public IOpenGLResource
{
    DECLARE_OBJECT_TYPE(IOpenGLShader, IOpenGLResource);
    IOpenGLShader(NString ShaderName, OpenGLResourceType ResourceType);
    virtual ~IOpenGLShader();
public:
    virtual void SetShaderCode(const ANSICHAR* ShaderCode, const TCHAR* ShaderPreprocessorDefines = TEXT("")) = 0;
    virtual bool Compile() = 0;
    virtual bool IsValid() = 0;
    NString _ShaderName;
    NString _ShaderCode;
};

class IOpenGLVertexShader:  public IOpenGLShader
{
    DECLARE_OBJECT_TYPE(IOpenGLVertexShader, IOpenGLShader);
public:
    virtual ~IOpenGLVertexShader();
    virtual void SetShaderCode(const ANSICHAR* ShaderCode, const TCHAR* VtxShaderPreprocessorDefines = TEXT(""));
    virtual bool Compile();
    virtual bool IsValid();
private:
    IOpenGLVertexShader(NString ShaderName=NString("VertexProgram"));
    int m_CompiledAndReady;
    friend class GLDeviceFactory;
};

class IOpenGLPixelShader:  public IOpenGLShader
{
    DECLARE_OBJECT_TYPE(IOpenGLPixelShader, IOpenGLShader);
public:
    virtual ~IOpenGLPixelShader();
    virtual void SetShaderCode(const ANSICHAR* ShaderCode, const TCHAR* FrgShaderPreprocessorDefines = TEXT(""));
    virtual bool Compile();
    virtual bool IsValid();
private:
    IOpenGLPixelShader(NString ShaderName=NString("PixelProgram"));
    int m_CompiledAndReady;
    friend class GLDeviceFactory;
};


class IOpenGLShaderProgram:  public IOpenGLResource
{
    DECLARE_OBJECT_TYPE(IOpenGLShaderProgram, IOpenGLResource);
public:
    virtual ~IOpenGLShaderProgram();

    // Load an inalogic shader.
    void LoadVertexShader(const TCHAR* glslshader, const TCHAR* VtxShaderPreprocessorDefines = TEXT(""));
    void LoadPixelShader(const TCHAR* glslshader, const TCHAR* FrgShaderPreprocessorDefines = TEXT(""));
    void LoadIShaderFile(const TCHAR* ShaderFileName, const TCHAR* VtxShaderPreprocessorDefines = TEXT(""), const TCHAR* FrgShaderPreprocessorDefines = TEXT(""));
    void LoadIShader(const TCHAR* ShaderCode, const TCHAR* VtxShaderPreprocessorDefines = TEXT(""), const TCHAR* FrgShaderPreprocessorDefines = TEXT(""));
    void AddShaderParameter(GLShaderParameter* Parameter);
    void AddShaderObject(TRefGL<IOpenGLShader> ShaderObject);
    void RemoveShaderObject(TRefGL<IOpenGLShader> ShaderObject);
    void ClearShaderObjects();
    bool Link();

    void Begin();
    void End();

public:

    bool SetUniform1f(char* varname, GLfloat v0);
    bool SetUniform2f(char* varname, GLfloat v0, GLfloat v1);
    bool SetUniform3f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2);
    bool SetUniform4f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

    bool SetUniform1i(char* varname, GLint v0);
    bool SetUniform2i(char* varname, GLint v0, GLint v1);
    bool SetUniform3i(char* varname, GLint v0, GLint v1, GLint v2);
    bool SetUniform4i(char* varname, GLint v0, GLint v1, GLint v2, GLint v3);

    bool SetUniform1fv(char* varname, GLsizei count, GLfloat *value);
    bool SetUniform2fv(char* varname, GLsizei count, GLfloat *value);
    bool SetUniform3fv(char* varname, GLsizei count, GLfloat *value);
    bool SetUniform4fv(char* varname, GLsizei count, GLfloat *value);

    bool SetUniform1iv(char* varname, GLsizei count, GLint *value);
    bool SetUniform2iv(char* varname, GLsizei count, GLint *value);
    bool SetUniform3iv(char* varname, GLsizei count, GLint *value);
    bool SetUniform4iv(char* varname, GLsizei count, GLint *value);

    bool SetUniformMatrix2fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
    bool SetUniformMatrix3fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
    bool SetUniformMatrix4fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);


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

    void GetUniformfv(char* name, GLfloat* values);
    void GetUniformiv(char* name, GLint* values); 
    int GetUniformLocationARB(const GLcharARB* name);
    void GetActiveUniformARB(GLuint index,
        GLsizei maxLength,
        GLsizei *length,
        GLint *size,
        GLenum *type,
        GLcharARB *name);

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
    BOOL SetSampler(char* name, int exture_unit);

    GLShaderParameter* _FirstParameter;
    void CheckAttributeLocation();
    void CheckUniformLocation();
    int GetAttributeLocation(const TCHAR* AttributeName);

private:
    IOpenGLShaderProgram(NString ShaderProgramName=NString("ShaderProgram"));
    ShaderAttributeDefinition m_ProgramAttributeDefinition[16/*NUM_VERTEX_SHADER_INPUT_ATTRIBUTE*/];
    std::vector<TRefGL<IOpenGLShader> > ShaderObjectList;
    bool m_CompiledAndReady;
    NString _ShaderProgramName;
    friend class GLDeviceFactory;
};

NAMESPACE_END_OGL

#endif // IOPENGLSHADER_H
