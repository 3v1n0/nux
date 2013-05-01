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


#ifndef GLDEVICEFACTORY_H
#define GLDEVICEFACTORY_H

#include "GLResource.h"
#include "GLDeviceFrameBufferObject.h"
#include "GLDeviceObjects.h"
#include "GLTextureStates.h"
#include "GLTemplatePrimitiveBuffer.h"

namespace nux
{
  class GpuRenderStates;

  //! Brand of GPUs.
  typedef enum
  {
    GPU_VENDOR_UNKNOWN = 0,
    GPU_BRAND_AMD,
    GPU_BRAND_NVIDIA,
    GPU_BRAND_INTEL,
  } GpuBrand;

  template<typename T> class ObjectPtr;

  struct STREAMSOURCE
  {
    WORD Stream;
    ObjectPtr<IOpenGLVertexBuffer> VertexBuffer;
    unsigned short StreamOffset;
    unsigned short StreamStride;

    STREAMSOURCE()
    {
      Stream = 0;
      //VertexBuffer = 0;
      StreamOffset = 0;
      StreamStride = 0;
    }

    void ResetStreamSource()
    {
      Stream = 0;
      StreamOffset = 0;
      VertexBuffer = ObjectPtr<IOpenGLVertexBuffer> (0);
      StreamStride = 0;
    }
  };

  // GPU Graphics information.
  class GpuInfo
  {
  public:

    GpuInfo();

    bool SupportOpenGL11() const    {return _support_opengl_version_11;}
    bool SupportOpenGL12() const    {return _support_opengl_version_12;}
    bool SupportOpenGL13() const    {return _support_opengl_version_13;}
    bool SupportOpenGL14() const    {return _support_opengl_version_14;}
    bool SupportOpenGL15() const    {return _support_opengl_version_15;}
    bool SupportOpenGL20() const    {return _support_opengl_version_20;}
    bool SupportOpenGL21() const    {return _support_opengl_version_21;}
    bool SupportOpenGL30() const    {return _support_opengl_version_30;}
    bool SupportOpenGL31() const    {return _support_opengl_version_31;}
    bool SupportOpenGL32() const    {return _support_opengl_version_32;}
    bool SupportOpenGL33() const    {return _support_opengl_version_33;}
    bool SupportOpenGL40() const    {return _support_opengl_version_40;}
    bool SupportOpenGL41() const    {return _support_opengl_version_41;}

    bool Support_EXT_Swap_Control()              const    {return _support_ext_swap_control;}
    bool Support_ARB_Texture_Rectangle()         const    {return _support_arb_texture_rectangle;}
    bool Support_ARB_Vertex_Program()            const    {return _support_arb_vertex_program;}
    bool Support_ARB_Fragment_Program()          const    {return _support_arb_fragment_program;}
    bool Support_ARB_Shader_Objects()            const    {return _support_arb_shader_objects;}
    bool Support_ARB_Vertex_Shader()             const    {return _support_arb_vertex_shader;}
    bool Support_ARB_Fragment_Shader()           const    {return _support_arb_fragment_shader;}
    bool Support_ARB_Vertex_Buffer_Object()      const    {return _support_arb_vertex_buffer_object;}
    bool Support_ARB_Texture_Non_Power_Of_Two()  const    {return _support_arb_texture_non_power_of_two;}
    bool Support_EXT_Framebuffer_Object()        const    {return _support_ext_framebuffer_object;}
    bool Support_EXT_Draw_Range_Elements()       const    {return _support_ext_draw_range_elements;}
    bool Support_EXT_Stencil_Two_Side()          const    {return _support_ext_stencil_two_side;}
    bool Support_EXT_Texture_Rectangle()         const    {return _support_ext_texture_rectangle;}
    bool Support_NV_Texture_Rectangle()          const    {return _support_nv_texture_rectangle;}
    bool Support_ARB_Pixel_Buffer_Object()       const    {return _support_arb_pixel_buffer_object;}
    bool Support_EXT_Blend_Equation_Separate()   const    {return _support_ext_blend_equation_separate;}
    bool Support_Depth_Buffer()                  const    {return _support_depth_buffer;}

#ifndef NUX_OPENGLES_20
    bool Support_EXT_Texture_sRGB()              const    {return _support_ext_texture_srgb;}
    bool Support_EXT_Texture_Decode()            const    {return _support_ext_texture_srgb_decode;}
    bool Support_EXT_Framebuffer_sRGB()          const    {return _support_ext_framebuffer_srgb;}
    bool Support_ARB_Framebuffer_sRGB()          const    {return _support_arb_framebuffer_srgb;}
#endif

    int GetMaxFboAttachment()                    const    {return _opengl_max_fb_attachment;}
    int GetMaxTextureSize()                      const    {return _opengl_max_texture_size;}


  private:
    void Setup();

    bool _support_opengl_version_11;
    bool _support_opengl_version_12;
    bool _support_opengl_version_13;
    bool _support_opengl_version_14;
    bool _support_opengl_version_15;
    bool _support_opengl_version_20;
    bool _support_opengl_version_21;
    bool _support_opengl_version_30;
    bool _support_opengl_version_31;
    bool _support_opengl_version_32;
    bool _support_opengl_version_33;
    bool _support_opengl_version_40;
    bool _support_opengl_version_41;

    int _opengl_max_texture_size;
    int _opengl_max_texture_units;
    int _opengl_max_texture_coords;
    int _opengl_max_texture_image_units;
    int _opengl_max_fb_attachment;
    int _opengl_max_vertex_attributes;

    bool _support_ext_swap_control;
    bool _support_arb_vertex_program;
    bool _support_arb_fragment_program;
    bool _support_arb_shader_objects;
    bool _support_arb_vertex_shader;
    bool _support_arb_fragment_shader;
    bool _support_arb_vertex_buffer_object;
    bool _support_arb_texture_non_power_of_two;
    bool _support_ext_framebuffer_object;
    bool _support_ext_draw_range_elements;
    bool _support_ext_stencil_two_side;
    bool _support_ext_texture_rectangle;
    bool _support_arb_texture_rectangle; //!< Promoted from GL_EXT_TEXTURE_RECTANGLE to ARB.
    bool _support_nv_texture_rectangle;
    bool _support_arb_pixel_buffer_object;
    bool _support_ext_blend_equation_separate;
    bool _support_depth_buffer;

#ifndef NUX_OPENGLES_20
    bool _support_ext_texture_srgb;
    bool _support_ext_texture_srgb_decode;
    bool _support_ext_framebuffer_srgb;
    bool _support_arb_framebuffer_srgb;
#endif

    friend class GpuDevice;
  };

  //! The interface to the GPU.
  /*!
      This is the object that serves as the interface between the program and the GPU device.
      The GpuDevice creates the opengl primitives used for rendering.
  */
  class GpuDevice
  {
  private:
    static STREAMSOURCE _StreamSource[MAX_NUM_STREAM];

  public:
    ObjectPtr<IOpenGLTexture2D> CreateTexture(
      int Width,
      int Height,
      int Levels,
      BitmapFormat PixelFormat, NUX_FILE_LINE_PROTO);

    ObjectPtr<IOpenGLTexture2D> CreateTexture2DFromID(int id,
      int Width,
      int Height,
      int Levels,
      BitmapFormat PixelFormat, NUX_FILE_LINE_PROTO);

    ObjectPtr<IOpenGLRectangleTexture> CreateRectangleTexture(
      int Width
      , int Height
      , int Levels
      , BitmapFormat PixelFormat, NUX_FILE_LINE_PROTO);

    ObjectPtr<IOpenGLCubeTexture> CreateCubeTexture(
      int EdgeLength
      , int Levels
      , BitmapFormat PixelFormat, NUX_FILE_LINE_PROTO);

    ObjectPtr<IOpenGLVolumeTexture> CreateVolumeTexture(
      int Width
      , int Height
      , int Depth
      , int Levels
      , BitmapFormat PixelFormat, NUX_FILE_LINE_PROTO);

    ObjectPtr<IOpenGLAnimatedTexture> CreateAnimatedTexture(
      int Width
      , int Height
      , int Depth
      , BitmapFormat PixelFormat);

    ObjectPtr<IOpenGLVertexBuffer> CreateVertexBuffer(
      int Length
      , VBO_USAGE Usage);

    ObjectPtr<IOpenGLIndexBuffer> CreateIndexBuffer(
      int Length
      , VBO_USAGE Usage    // Dynamic or WriteOnly
      , INDEX_FORMAT Format);

    ObjectPtr<IOpenGLPixelBufferObject> CreatePixelBufferObject(int Size, VBO_USAGE Usage);

    ObjectPtr<IOpenGLQuery> CreateQuery(
      QUERY_TYPE Type);

    ObjectPtr<IOpenGLVertexDeclaration> CreateVertexDeclaration(
      const VERTEXELEMENT *pVertexElements);

    //! Create an return an ObjectPtr that encapsulates OpenGL framebuffer object.
    /*!
        Create an return an ObjectPtr that encapsulates OpenGL framebuffer object.
        This function has side effects. It will change the framebuffer object that 
        is set in the GPU. 
    */
    ObjectPtr<IOpenGLFrameBufferObject> CreateFrameBufferObject();

    ObjectPtr<IOpenGLShaderProgram> CreateShaderProgram();
    ObjectPtr<IOpenGLVertexShader> CreateVertexShader();
    ObjectPtr<IOpenGLPixelShader> CreatePixelShader();

#ifndef NUX_OPENGLES_20    
    ObjectPtr<IOpenGLAsmShaderProgram> CreateAsmShaderProgram();
    ObjectPtr<IOpenGLAsmVertexShader> CreateAsmVertexShader();
    ObjectPtr<IOpenGLAsmPixelShader> CreateAsmPixelShader();
#endif

#if (NUX_ENABLE_CG_SHADERS)
    ObjectPtr<ICgVertexShader> CreateCGVertexShader();
    ObjectPtr<ICgPixelShader> CreateCGPixelShader();
#endif

    // This is for the fixed pipeline
    // When using shaders see how a shader sampler parameter links to a texture with a call to setTexture.
    int SetTexture(unsigned int TextureUnit, IOpenGLBaseTexture *texture);

    int DrawIndexedPrimitive(
      ObjectPtr<IOpenGLIndexBuffer> IndexBuffer,
      ObjectPtr<IOpenGLVertexDeclaration> VertexDeclaration,
      PRIMITIVE_TYPE PrimitiveType,
      int PrimitiveCount
    );

    // Draw Primitive without index buffer
    int DrawPrimitive(
      ObjectPtr<IOpenGLVertexDeclaration> VertexDeclaration,
      PRIMITIVE_TYPE pt_,
      unsigned vtx_start_,
      unsigned num_prims_);

    // Draw Primitive without index buffer, and use a user pointer for the source of the stream.
    int DrawPrimitiveUP(
      ObjectPtr<IOpenGLVertexDeclaration> VertexDeclaration,
      PRIMITIVE_TYPE PrimitiveType,
      unsigned int PrimitiveCount,
      const void *pVertexStreamZeroData,
      unsigned int VertexStreamZeroStride
    );

    int SetStreamSource(
      unsigned int StreamNumber,
      ObjectPtr<IOpenGLVertexBuffer> pStreamData,
      unsigned int OffsetInBytes,
      unsigned int Stride);

    //! Setup a NULL vertex buffer
    void InvalidateVertexBuffer();
    //! Setup a NULL index buffer
    void InvalidateIndexBuffer();
    //! Setup a NULL texture
    void InvalidateTextureUnit(int TextureUnitIndex);

    int AllocateUnpackPixelBufferIndex(int *index);
    int FreeUnpackPixelBufferIndex(const int index);
    int BindUnpackPixelBufferIndex(const int index);
    int BindPackPixelBufferIndex(const int index);
    void *LockUnpackPixelBufferIndex(const int index, const int Size);
    void *LockPackPixelBufferIndex(const int index, const int Size);

    void UnlockUnpackPixelBufferIndex(const int index);
    void UnlockPackPixelBufferIndex(const int index);

    // All these operations are done on the default frame buffer object: _FrameBufferObject.
    int FormatFrameBufferObject(unsigned int Width, unsigned int Height, BitmapFormat PixelFormat);
    int SetColorRenderTargetSurface(unsigned int ColorAttachmentIndex, ObjectPtr<IOpenGLSurface> pRenderTargetSurface);
    int SetDepthRenderTargetSurface(ObjectPtr<IOpenGLSurface> pDepthSurface);
    ObjectPtr<IOpenGLSurface> GetColorRenderTargetSurface(unsigned int ColorAttachmentIndex);
    ObjectPtr<IOpenGLSurface> GetDepthRenderTargetSurface();
    // Activate and Deactivate the default framebuffer: _FrameBufferObject.
    void ActivateFrameBuffer();

    //! Restore the backbuffer as the render target.
    void DeactivateFrameBuffer();

    unsigned int GetPixelStoreAlignment() const;

  public:
    void SetCurrentFrameBufferObject(ObjectPtr<IOpenGLFrameBufferObject> fbo);
    ObjectPtr<IOpenGLFrameBufferObject> GetFrameBufferObject() const;
    ObjectPtr<IOpenGLFrameBufferObject> GetCurrentFrameBufferObject();
    ObjectPtr<IOpenGLBaseTexture> ActiveFboTextureAttachment(int color_attachment_index);
    ObjectPtr<IOpenGLBaseTexture> ActiveFboDepthTextureAttachment();


    int GetOpenGLMajorVersion() const;
    int GetOpenGLMinorVersion() const;
  private:
    // Default FrameBufferobject
    ObjectPtr<IOpenGLFrameBufferObject> _FrameBufferObject;
    ObjectPtr<IOpenGLFrameBufferObject> active_framebuffer_object_;

    struct PixelBufferObject
    {
      ObjectPtr<IOpenGLPixelBufferObject> PBO;
      bool   IsReserved;
    };

  public:

#if (NUX_ENABLE_CG_SHADERS)
    CGcontext GetCgContext()
    {
      return m_Cgcontext;
    }
    CGcontext m_Cgcontext;
#endif

    bool UsePixelBufferObjects() const;

    GpuBrand GetGPUBrand() const;

    GpuRenderStates& GetRenderStates();

    const GpuInfo& GetGpuInfo() const;

    void ResetRenderStates();

    void VerifyRenderStates();

    //! Create a texture that the system supports. Rectangle texture or 2D texture.
    /*!
      @Width        Texture width.
      @Height       Texture height.
      @Levels       Texture number of mipmaps. If 0, all the mipmaps levels are created
      @PixelFormat  Texture format.
      @return       A device texture. Depending on the system capabilities returns a ObjectPtr<IOpenGLTexture2D> or ObjectPtr<IOpenGLRectangleTexture>.
    */
    ObjectPtr<IOpenGLBaseTexture> CreateSystemCapableDeviceTexture(
      int Width
      , int Height
      , int Levels
      , BitmapFormat PixelFormat, NUX_FILE_LINE_PROTO);

    //! Created a cached texture
    /*!
      @return A cached texture. Depending on the system capabilities, returns a Texture2D or TextureRectangle.
    */
    BaseTexture* CreateSystemCapableTexture(NUX_FILE_LINE_PROTO);

    bool SUPPORT_GL_ARB_TEXTURE_NON_POWER_OF_TWO()  const
    {
      return gpu_info_->Support_ARB_Texture_Non_Power_Of_Two();
    }

    bool SUPPORT_GL_EXT_TEXTURE_RECTANGLE()    const
    {
      return gpu_info_->Support_EXT_Texture_Rectangle();
    }

    bool SUPPORT_GL_ARB_TEXTURE_RECTANGLE()  const
    {
      return gpu_info_->Support_ARB_Texture_Rectangle();
    }

  private:
    //
    int _glsl_version_major;  //!< GLSL major version.
    int _glsl_version_minor;  //!< GLSL major version.
    int opengl_major_;  //!< OpenGL major version.
    int opengl_minor_;  //!< OpenGL minor version.

    std::string _board_vendor_string;     //!< GPU vendor sting.
    std::string _board_renderer_string;   //!< GPU renderer sting.
    std::string _openGL_version_string;   //!< OpenGL version string.
    GpuBrand gpu_brand_;                  //!< GPU brand.

    bool use_pixel_buffer_object_;
    unsigned int pixel_store_alignment_;
    std::vector<PixelBufferObject> _PixelBufferArray;

    bool OGL_EXT_SWAP_CONTROL;
    bool GL_ARB_VERTEX_PROGRAM;
    bool GL_ARB_FRAGMENT_PROGRAM;
    bool GL_ARB_SHADER_OBJECTS;
    bool GL_ARB_VERTEX_SHADER;
    bool GL_ARB_FRAGMENT_SHADER;
    bool GL_ARB_VERTEX_BUFFER_OBJECT;
    bool GL_ARB_TEXTURE_NON_POWER_OF_TWO;
    bool GL_EXT_FRAMEBUFFER_OBJECT;
    bool GL_EXT_DRAW_RANGE_ELEMENTS;
    bool GL_EXT_STENCIL_TWO_SIDE;
    bool GL_EXT_TEXTURE_RECTANGLE;
    bool GL_ARB_TEXTURE_RECTANGLE; //!< Promoted from GL_EXT_TEXTURE_RECTANGLE to ARB.
    bool GL_NV_TEXTURE_RECTANGLE;

    GpuRenderStates* gpu_render_states_;
    GpuInfo* gpu_info_;

  public:

    ObjectPtr<IOpenGLTexture2D> backup_texture0_;

#if defined(NUX_OS_WINDOWS)
    GpuDevice(unsigned int DeviceWidth, unsigned int DeviceHeight, BitmapFormat DeviceFormat,
      HDC device_context,
      HGLRC &opengl_rendering_context,
      int req_opengl_major = 1,   // requested opengl major version.
      int req_opengl_minor = 0,   // requested opengl minor version.
      bool opengl_es_20 = false);

#elif defined(USE_X11)
    #ifdef NUX_OPENGLES_20
        GpuDevice(unsigned int DeviceWidth, unsigned int DeviceHeight,
          BitmapFormat DeviceFormat,
          Display *display,
          Window window,
          bool has_new_glx_support,
          EGLConfig fb_config,
          EGLContext &opengl_rendering_context,
          int req_opengl_major = 2,
          int req_opengl_minor = 0,
          bool opengl_es_20 = true);
    #else
        GpuDevice(unsigned int DeviceWidth, unsigned int DeviceHeight,
          BitmapFormat DeviceFormat,
          Display *display,
          Window window,
          bool has_new_glx_support,
          GLXFBConfig fb_config,
          GLXContext &opengl_rendering_context,
          int req_opengl_major = 1,   // requested opengl major version.
          int req_opengl_minor = 0,   // requested opengl minor version.
          bool opengl_es_20 = false);
    #endif
#elif defined(NO_X11)
    GpuDevice(unsigned int DeviceWidth, unsigned int DeviceHeight, BitmapFormat DeviceFormat,
      EGLDisplay display,
      EGLConfig fb_config,
      EGLContext &opengl_rendering_context,
      int req_opengl_major,
      int req_opengl_minor);

#endif
    ~GpuDevice();
    friend class IOpenGLSurface;
    friend class GraphicsEngine;
  };

}

#endif // GLDEVICEFACTORY_H
