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


#ifndef IOPENGLRESOURCE_H
#define IOPENGLRESOURCE_H

namespace nux
{

  class GpuDevice;
  class IOpenGLBaseTexture;
  class IOpenGLTexture2D;
  class IOpenGLRectangleTexture;
  class IOpenGLCubeTexture;
  class IOpenGLSurface;
  class IOpenGLVolumeTexture;
  class IOpenGLVolume;
  class IOpenGLQuery;
  class IOpenGLShader;
  class IOpenGLVertexShader;
  class IOpenGLPixelShader;

  template<typename T> class ObjectPtr;

#define NUM_VERTEX_SHADER_INPUT_ATTRIBUTE      16
  enum VertexAttributeType
  {
    VAT_UNDEFINED = 0,
    VAT_FLOAT = 1,
    VAT_FLOAT2,
    VAT_FLOAT3,
    VAT_FLOAT4,
    VAT_FLOAT_MAT2,
    VAT_FLOAT_MAT3,
    VAT_FLOAT_MAT4,
  };

  enum ShaderType
  {
    SHADER_TYPE_GLSL = 0,
    SHADER_TYPE_CG,
    SHADER_TYPE_FORCE_DWORD = 0x7fffffff /* force 32-bit size enum */
  };
  struct ShaderAttributeDefinition
  {
    int attribute_index;    // 0 to 15
    std::string attribute_name;
    int type;          // 1, 2, 3 or 4 component,
    bool valid;             // true is the attribute is used
  };


//////////////////////////////////////////////////////////////////////////
  class IOpenGLResource: public Object
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLResource, Object);

  public:
    IOpenGLResource(OpenGLResourceType ResourceType, NUX_FILE_LINE_PROTO)
      : Object(true, NUX_FILE_LINE_PARAM)
      , _OpenGLID(0)
      , _RefCount(0)
      , _ResourceType(ResourceType)
    {
    }

    virtual ~IOpenGLResource()
    {

    }

    virtual int RefCount() const
    {
      return GetReferenceCount();
    }

    OpenGLResourceType GetResourceType() const
    {
      return _ResourceType;
    }

    GLuint GetOpenGLID() const
    {
      return _OpenGLID;
    }

    GLuint* GetOpenGLIDAsInParam()
    {
      return &_OpenGLID;
    }
  private:
    GLuint _OpenGLID;
    int _RefCount;
    OpenGLResourceType _ResourceType;

    friend class IOpenGLFrameBufferObject;
    friend class IOpenGLSurface;
    friend class IOpenGLBaseTexture;
    friend class IOpenGLCubeTexture;
    friend class IOpenGLVolumeTexture;
    friend class IOpenGLAnimatedTexture;
    friend class IOpenGLTexture2D;
    friend class IOpenGLRectangleTexture;
    friend class IOpenGLVolume;
    friend class IOpenGLQuery;
    friend class GpuDevice;
    friend class IOpenGLIndexBuffer;
    friend class IOpenGLVertexBuffer;
    friend class IOpenGLVertexDeclaration;
    friend class IOpenGLShader;
    friend class IOpenGLVertexShader;
    friend class IOpenGLPixelShader;
    friend class IOpenGLGeometryShader;
    friend class IOpenGLShaderProgram;
    friend class IOpenGLAsmShaderProgram;
    friend class IOpenGLPixelBufferObject;
  };

}

#endif // IOPENGLRESOURCE_H
