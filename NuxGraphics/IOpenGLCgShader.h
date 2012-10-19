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


#ifndef ICGSHADER_H
#define ICGSHADER_H

namespace nux
{

  class IOpenGLResource;

  class ICgShader:  public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(ICgShader, IOpenGLResource);

  public:
    ICgShader(std::string ShaderName, OpenGLResourceType ResourceType);
    virtual ~ICgShader();

    virtual void CreateProgram(const ANSICHAR *ShaderCode, const ANSICHAR *EntryPoint) = 0;
    virtual void CreateProgramFromFileShaderCode(const char *Filename, const char *EntryPoint) = 0;
    virtual void BindProgram() = 0;
    const char *GetProgramString();
    const char *GetProgramBinary();
    virtual bool Compile() = 0;
    virtual bool IsValid() = 0;
    CGparameter GetNamedParameter(const char *parameter);

    std::string _ShaderName;
    std::string _ShaderCode;
    std::string _EntryPoint;
    CGprogram _CgProgram;
  };

  class ICgVertexShader:  public ICgShader
  {
    NUX_DECLARE_OBJECT_TYPE(ICgVertexShader, ICgShader);
  public:
    virtual ~ICgVertexShader();
    virtual void CreateProgram(const ANSICHAR *ShaderCode, const ANSICHAR *EntryPoint);
    virtual void CreateProgramFromFileShaderCode(const char *Filename, const char *EntryPoint);
    virtual void BindProgram();
    virtual bool Compile();
    virtual bool IsValid();
  private:
    ICgVertexShader(std::string ShaderName = std::string("VertexProgram"));
    int _ready;
    friend class GpuDevice;
  };

  class ICgPixelShader:  public ICgShader
  {
    NUX_DECLARE_OBJECT_TYPE(ICgPixelShader, ICgShader);
  public:
    virtual ~ICgPixelShader();
    virtual void CreateProgram(const ANSICHAR *ShaderCode, const ANSICHAR *EntryPoint);
    virtual void CreateProgramFromFileShaderCode(const char *Filename, const char *EntryPoint);
    virtual void BindProgram();
    virtual bool Compile();
    virtual bool IsValid();
  private:
    ICgPixelShader(std::string ShaderName = std::string("PixelProgram"));
    int _ready;
    friend class GpuDevice;
  };

}

#endif // ICGSHADER_H
