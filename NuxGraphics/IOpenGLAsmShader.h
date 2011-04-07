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


#ifndef IOPENGLASMSHADER_H
#define IOPENGLASMSHADER_H

namespace nux
{

  typedef enum
  {
    VTXATTRIB_INVALID           = -1,
    VTXATTRIB_POSITION          = 0,
    VTXATTRIB_WEIGHT            = 1,
    VTXATTRIB_NORMAL            = 2,
    VTXATTRIB_COLOR             = 3,
    VTXATTRIB_COLOR_PRIMARY     = 3,
    VTXATTRIB_COLOR_SECONDARY   = 4,
    VTXATTRIB_FOGCOORD          = 5,
    VTXATTRIB_TEXCOORD          = 8,
    VTXATTRIB_TEXCOORD0         = 8,
    VTXATTRIB_TEXCOORD1         = 9,
    VTXATTRIB_TEXCOORD2         = 10,
    VTXATTRIB_TEXCOORD3         = 11,
    VTXATTRIB_TEXCOORD4         = 12,
    VTXATTRIB_TEXCOORD5         = 13,
    VTXATTRIB_TEXCOORD6         = 14,
    VTXATTRIB_TEXCOORD7         = 15,
  } VtxAttributeIndex;

  class IOpenGLResource;

  class IOpenGLAsmShader:  public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE (IOpenGLAsmShader, IOpenGLResource);
    IOpenGLAsmShader (NString ShaderName, OpenGLResourceType ResourceType);
    virtual ~IOpenGLAsmShader();
  public:
    virtual void SetShaderCode (const ANSICHAR *ShaderCode) = 0;
    virtual bool Compile() = 0;
    virtual bool IsValid() = 0;
    NString _ShaderName;
    NString _ShaderCode;
  };

  class IOpenGLAsmVertexShader:  public IOpenGLAsmShader
  {
    NUX_DECLARE_OBJECT_TYPE (IOpenGLAsmVertexShader, IOpenGLAsmShader);
  public:
    virtual ~IOpenGLAsmVertexShader();
    virtual void SetShaderCode (const ANSICHAR *ShaderCode);
    virtual bool Compile();
    virtual bool IsValid();
  private:
    IOpenGLAsmVertexShader (NString ShaderName = NString ("VertexProgram") );
    bool m_CompiledAndReady;
    friend class GpuDevice;
  };

  class IOpenGLAsmPixelShader:  public IOpenGLAsmShader
  {
    NUX_DECLARE_OBJECT_TYPE (IOpenGLAsmPixelShader, IOpenGLAsmShader);
  public:
    virtual ~IOpenGLAsmPixelShader();
    virtual void SetShaderCode (const ANSICHAR *ShaderCode);
    virtual bool Compile();
    virtual bool IsValid();
  private:
    IOpenGLAsmPixelShader (NString ShaderName = NString ("PixelProgram") );
    bool m_CompiledAndReady;
    friend class GpuDevice;
  };


  class IOpenGLAsmShaderProgram:  public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE (IOpenGLAsmShaderProgram, IOpenGLResource);
  public:
    virtual ~IOpenGLAsmShaderProgram();

    void LoadVertexShader (const TCHAR *glslshader);
    void LoadPixelShader (const TCHAR *glslshader);
    void LoadIShaderFile (const TCHAR *ShaderFileName);
    void LoadIShader (const TCHAR *ShaderCode);
    void Link();

    bool IsValid();

    void Begin();
    void End();

    void SetVertexEnvParameter4dARB         (t_uint32 index, double x, double y, double z, double w);
    void SetVertexEnvParameter4dvARB        (t_uint32 index, const double *params);
    void SetVertexEnvParameter4fARB         (t_uint32 index, float x, float y, float z, float w);
    void SetVertexEnvParameter4fvARB        (t_uint32 index, const float *params);
    void SetVertexLocalParameter4dARB       (t_uint32 index, double x, double y, double z, double w);
    void SetVertexLocalParameter4dvARB      (t_uint32 index, const double *params);
    void SetVertexLocalParameter4fARB       (t_uint32 index, float x, float y, float z, float w);
    void SetVertexLocalParameter4fvARB      (t_uint32 index, const float *params);

    void SetFragmentEnvParameter4dARB       (t_uint32 index, double x, double y, double z, double w);
    void SetFragmentEnvParameter4dvARB      (t_uint32 index, const double *params);
    void SetFragmentEnvParameter4fARB       (t_uint32 index, float x, float y, float z, float w);
    void SetFragmentEnvParameter4fvARB      (t_uint32 index, const float *params);
    void SetFragmentLocalParameter4dARB     (t_uint32 index, double x, double y, double z, double w);
    void SetFragmentLocalParameter4dvARB    (t_uint32 index, const double *params);
    void SetFragmentLocalParameter4fARB     (t_uint32 index, float x, float y, float z, float w);
    void SetFragmentLocalParameter4fvARB    (t_uint32 index, const float *params);

  private:
    IOpenGLAsmShaderProgram (NString ShaderProgramName = NString ("AsmShaderProgram") );
    std::vector<ObjectPtr<IOpenGLAsmShader> > ShaderObjectList;
    NString _ShaderProgramName;

    ObjectPtr<IOpenGLAsmVertexShader> m_AsmVertexProgram;
    ObjectPtr<IOpenGLAsmPixelShader> m_AsmFragmentProgram;

    friend class GpuDevice;
  };

}

#endif // IOPENGLASMSHADER_H
