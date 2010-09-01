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


#ifndef ICGSHADER_H
#define ICGSHADER_H

NAMESPACE_BEGIN_OGL

class IOpenGLResource;

class ICgShader:  public IOpenGLResource
{
    DECLARE_OBJECT_TYPE(ICgShader, IOpenGLResource);

public:
    ICgShader(NString ShaderName, OpenGLResourceType ResourceType);
    virtual ~ICgShader();

    virtual void CreateProgram(const ANSICHAR* ShaderCode, const ANSICHAR* EntryPoint) = 0;
    virtual void CreateProgramFromFileShaderCode(const TCHAR* Filename, const TCHAR* EntryPoint) = 0;
    virtual void BindProgram() = 0;
    const char* GetProgramString();
    const char* GetProgramBinary();
    virtual bool Compile() = 0;
    virtual bool IsValid() = 0;
    CGparameter GetNamedParameter(const TCHAR* parameter);

    NString _ShaderName;
    NString _ShaderCode;
    NString _EntryPoint;
    CGprogram _CgProgram;
};

class ICgVertexShader:  public ICgShader
{
    DECLARE_OBJECT_TYPE(ICgVertexShader, ICgShader);
public:
    virtual ~ICgVertexShader();
    virtual void CreateProgram(const ANSICHAR* ShaderCode, const ANSICHAR* EntryPoint);
    virtual void CreateProgramFromFileShaderCode(const TCHAR* Filename, const TCHAR* EntryPoint);
    virtual void BindProgram();
    virtual bool Compile();
    virtual bool IsValid();
private:
    ICgVertexShader(NString ShaderName=NString("VertexProgram"));
    int _ready;
    friend class GLDeviceFactory;
};

class ICgPixelShader:  public ICgShader
{
    DECLARE_OBJECT_TYPE(ICgPixelShader, ICgShader);
public:
    virtual ~ICgPixelShader();
    virtual void CreateProgram(const ANSICHAR* ShaderCode, const ANSICHAR* EntryPoint);
    virtual void CreateProgramFromFileShaderCode(const TCHAR* Filename, const TCHAR* EntryPoint);
    virtual void BindProgram();
    virtual bool Compile();
    virtual bool IsValid();
private:
    ICgPixelShader(NString ShaderName=NString("PixelProgram"));
    int _ready;
    friend class GLDeviceFactory;
};

NAMESPACE_END_OGL

#endif // ICGSHADER_H
