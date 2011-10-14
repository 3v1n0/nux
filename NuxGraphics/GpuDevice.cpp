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
#include "NuxGraphics.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GLDeviceFrameBufferObject.h"
#include "GLTemplatePrimitiveBuffer.h"
#include "GraphicsEngine.h"

namespace nux
{
#if (NUX_ENABLE_CG_SHADERS)
  extern void cgErrorCallback (void);
#endif

  typedef struct
  {
    int major;
    int minor;
  } OpenGLVersion;

  static OpenGLVersion OpenGLVersionTable [] =
  {
    {1, 0},
    {1, 1},
    {1, 2},
    {1, 3},
    {1, 4},
    {2, 0},
    {2, 1},
    {3, 0},
    {3, 1},
    {3, 2},
    {3, 3},
    {4, 0},
    {4, 1},
    {0, 0}
  };

  // Pixel buffer object seems to corrupt textures. This can be seen when the window is moved
  // to the second screen in a multi-display configuration...
  // Happens on geforce 6600. Does not happens on geforce 6800.

  // ATI Radeon 4670 has problems loading textures from pixel buffer object. PBO should be deactivated if the
  // graphics card is made by AMD/ATI
  #define NUX_USE_PBO     1

  extern PixelFormatInfo GPixelFormats[];

  static void InitTextureFormats()
  {
#ifndef NUX_OPENGLES_20
    GPixelFormats[ BITFMT_UNKNOWN		].PlatformFormat	= GL_NONE;							// Not supported for rendering.

    // Data in PC system memory: R(LSB) G B A(MSB) ---> GL Format:GL_RGBA - GL Type:GL_UNSIGNED_INT_8_8_8_8_REV
    GPixelFormats[BITFMT_R8G8B8A8].PlatformFormat	    = GL_RGBA8;
    GPixelFormats[BITFMT_R8G8B8A8].Format	            = GL_RGBA;
    GPixelFormats[BITFMT_R8G8B8A8].type	              = GL_UNSIGNED_INT_8_8_8_8_REV;

    // Data in PC system memory: A(LSB) B G R(MSB) ---> GL Format:GL_RGBA - GL Type:GL_UNSIGNED_INT_8_8_8_8
    GPixelFormats[BITFMT_A8B8G8R8].PlatformFormat	    = GL_RGBA8;
    GPixelFormats[BITFMT_A8B8G8R8].Format	            = GL_RGBA;
    GPixelFormats[BITFMT_A8B8G8R8].type	              = GL_UNSIGNED_INT_8_8_8_8;

    // Data in PC system memory: B(LSB) G R A(MSB) ---> GL Format:GL_BGRA - GL Type:GL_UNSIGNED_INT_8_8_8_8_REV
    GPixelFormats[BITFMT_B8G8R8A8].PlatformFormat	    = GL_RGBA8;
    GPixelFormats[BITFMT_B8G8R8A8].Format	            = GL_BGRA;
    GPixelFormats[BITFMT_B8G8R8A8].type	              = GL_UNSIGNED_INT_8_8_8_8_REV;

    // Data in PC system memory: A(LSB) R G B(MSB) ---> GL Format:GL_BGRA - GL Type:GL_UNSIGNED_INT_8_8_8_8
    GPixelFormats[BITFMT_A8R8G8B8].PlatformFormat	    = GL_RGBA8;
    GPixelFormats[BITFMT_A8R8G8B8].Format	            = GL_BGRA;
    GPixelFormats[BITFMT_A8R8G8B8].type	              = GL_UNSIGNED_INT_8_8_8_8;

    // Data in PC system memory: R(LSB) G B(MSB) ---> GL Format:GL_RGB - GL Type:GL_UNSIGNED
    GPixelFormats[BITFMT_R8G8B8].PlatformFormat	      = GL_RGB8;
    GPixelFormats[BITFMT_R8G8B8].Format	              = GL_RGB;
    GPixelFormats[BITFMT_R8G8B8].type	                = GL_UNSIGNED_BYTE;

    GPixelFormats[BITFMT_B8G8R8].PlatformFormat	      = GL_RGB8;
    GPixelFormats[BITFMT_B8G8R8].Format	              = GL_BGR;
    GPixelFormats[BITFMT_B8G8R8].type	                = GL_UNSIGNED_BYTE;

    GPixelFormats[BITFMT_R5G6B5].PlatformFormat	      = GL_RGB5;
    GPixelFormats[BITFMT_R5G6B5].Format	              = GL_RGB;
    GPixelFormats[BITFMT_R5G6B5].type	                = GL_UNSIGNED_SHORT_5_6_5;

    GPixelFormats[BITFMT_RGBA16F].PlatformFormat	    = GL_RGBA16F_ARB;
    GPixelFormats[BITFMT_RGBA16F].Format	            = GL_RGBA;
    GPixelFormats[BITFMT_RGBA16F].type	              = GL_HALF_FLOAT_ARB;

    GPixelFormats[BITFMT_RGB32F].PlatformFormat	      = GL_RGB;
    GPixelFormats[BITFMT_RGB32F].Format	              = GL_RGB;
    GPixelFormats[BITFMT_RGB32F].type	                = GL_FLOAT;

    GPixelFormats[BITFMT_RGBA32F].PlatformFormat	    = GL_RGBA32F_ARB;
    GPixelFormats[BITFMT_RGBA32F].Format	            = GL_RGBA;
    GPixelFormats[BITFMT_RGBA32F].type	              = GL_FLOAT;

    // Note: Using GL_DEPTH_COMPONENT24 or GL_DEPTH_COMPONENT for PlatformFormat generate error GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT.
    GPixelFormats[BITFMT_D24S8].PlatformFormat	      = GL_DEPTH24_STENCIL8_EXT;
    GPixelFormats[BITFMT_D24S8].Format	              = GL_DEPTH_STENCIL_EXT;     // or GL_DEPTH_STENCIL_NV;
    GPixelFormats[BITFMT_D24S8].type	                = GL_UNSIGNED_INT_24_8_EXT; // or GL_UNSIGNED_INT_24_8_NV;

    GPixelFormats[BITFMT_DXT1].PlatformFormat	        = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    GPixelFormats[BITFMT_DXT2].PlatformFormat	        = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    GPixelFormats[BITFMT_DXT3].PlatformFormat	        = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    GPixelFormats[BITFMT_DXT4].PlatformFormat	        = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    GPixelFormats[BITFMT_DXT5].PlatformFormat	        = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    GPixelFormats[BITFMT_R10G10B10A2].PlatformFormat	= GL_RGB10_A2;
    GPixelFormats[BITFMT_R10G10B10A2].Format	        = GL_RGBA;
    GPixelFormats[BITFMT_R10G10B10A2].type	          = GL_UNSIGNED_INT_10_10_10_2;

    GPixelFormats[BITFMT_A2B10G10R10].PlatformFormat	= GL_RGB10_A2;
    GPixelFormats[BITFMT_A2B10G10R10].Format	        = GL_RGBA;
    GPixelFormats[BITFMT_A2B10G10R10].type	          = GL_UNSIGNED_INT_2_10_10_10_REV;

    GPixelFormats[BITFMT_B10G10R10A2].PlatformFormat	= GL_RGB10_A2;
    GPixelFormats[BITFMT_B10G10R10A2].Format	        = GL_BGRA;
    GPixelFormats[BITFMT_B10G10R10A2].type	          = GL_UNSIGNED_INT_10_10_10_2;

    GPixelFormats[BITFMT_A2R10G10B10].PlatformFormat	= GL_RGB10_A2;
    GPixelFormats[BITFMT_A2R10G10B10].Format	        = GL_BGRA;
    GPixelFormats[BITFMT_A2R10G10B10].type	          = GL_UNSIGNED_INT_2_10_10_10_REV;

    GPixelFormats[BITFMT_A8].PlatformFormat	          = GL_RGBA8;
    GPixelFormats[BITFMT_A8].Format	                  = GL_LUMINANCE;
    GPixelFormats[BITFMT_A8].type	                    = GL_UNSIGNED_BYTE;
#else
    GPixelFormats[ BITFMT_UNKNOWN		].PlatformFormat	= GL_NONE;							// Not supported for rendering.

    // Data in PC system memory: R(LSB) G B A(MSB) ---> GL Format:GL_RGBA - GL Type:GL_UNSIGNED_INT_8_8_8_8_REV
    GPixelFormats[BITFMT_R8G8B8A8].PlatformFormat	    = GL_RGBA;
    GPixelFormats[BITFMT_R8G8B8A8].Format	            = GL_RGBA;
    GPixelFormats[BITFMT_R8G8B8A8].type	              = GL_UNSIGNED_BYTE;

    // Data in PC system memory: B(LSB) G R A(MSB) ---> GL Format:GL_BGRA - GL Type:GL_UNSIGNED_INT_8_8_8_8_REV
    GPixelFormats[BITFMT_B8G8R8A8].PlatformFormat	    = GL_BGRA_EXT;
    GPixelFormats[BITFMT_B8G8R8A8].Format	            = GL_BGRA_EXT;
    GPixelFormats[BITFMT_B8G8R8A8].type	              = GL_UNSIGNED_BYTE;

    // Data in PC system memory: R(LSB) G B(MSB) ---> GL Format:GL_RGB - GL Type:GL_UNSIGNED
    GPixelFormats[BITFMT_R8G8B8].PlatformFormat	      = GL_RGB;
    GPixelFormats[BITFMT_R8G8B8].Format	              = GL_RGB;
    GPixelFormats[BITFMT_R8G8B8].type	                = GL_UNSIGNED_BYTE;

    GPixelFormats[BITFMT_R5G6B5].PlatformFormat	      = GL_RGB;
    GPixelFormats[BITFMT_R5G6B5].Format	              = GL_RGB;
    GPixelFormats[BITFMT_R5G6B5].type	                = GL_UNSIGNED_SHORT_5_6_5;

    // Note: Using GL_DEPTH_COMPONENT24 or GL_DEPTH_COMPONENT for PlatformFormat generate error GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT.
    GPixelFormats[BITFMT_D24S8].PlatformFormat	      = GL_DEPTH_STENCIL_OES;
    GPixelFormats[BITFMT_D24S8].Format	              = GL_DEPTH_STENCIL_OES;
    GPixelFormats[BITFMT_D24S8].type	                = GL_UNSIGNED_INT_24_8_OES;

    GPixelFormats[BITFMT_A8].PlatformFormat	          = GL_ALPHA;
    GPixelFormats[BITFMT_A8].Format	                  = GL_ALPHA;
    GPixelFormats[BITFMT_A8].type	                    = GL_UNSIGNED_BYTE;
#endif
  }

  STREAMSOURCE GpuDevice::_StreamSource[MAX_NUM_STREAM];

  GpuInfo::GpuInfo ()
  {
    _support_opengl_version_11 = false;
    _support_opengl_version_12 = false;
    _support_opengl_version_13 = false;
    _support_opengl_version_14 = false;
    _support_opengl_version_15 = false;
    _support_opengl_version_20 = false;
    _support_opengl_version_21 = false;
    _support_opengl_version_30 = false;
    _support_opengl_version_31 = false;
    _support_opengl_version_32 = false;
    _support_opengl_version_33 = false;
    _support_opengl_version_40 = false;
    _support_opengl_version_41 = false;
  }

  void GpuInfo::Setup()
  {
#ifndef NUX_OPENGLES_20
    _support_opengl_version_11 = GLEW_VERSION_1_1 ? true : false;
    _support_opengl_version_12 = GLEW_VERSION_1_2 ? true : false;
    _support_opengl_version_13 = GLEW_VERSION_1_3 ? true : false;
    _support_opengl_version_14 = GLEW_VERSION_1_4 ? true : false;
    _support_opengl_version_15 = GLEW_VERSION_1_5 ? true : false;
    _support_opengl_version_20 = GLEW_VERSION_2_0 ? true : false;
    _support_opengl_version_21 = GLEW_VERSION_2_1 ? true : false;
    _support_opengl_version_30 = GLEW_VERSION_3_0 ? true : false;
    _support_opengl_version_31 = GLEW_VERSION_3_1 ? true : false;
    _support_opengl_version_32 = GLEW_VERSION_3_2 ? true : false;
//     _support_opengl_version_33 = GLEW_VERSION_3_3 ? true : false;
//     _support_opengl_version_40 = GLEW_VERSION_4_0 ? true : false;
//     _support_opengl_version_41 = GLEW_VERSION_4_1 ? true : false;

    // See: http://developer.nvidia.com/object/General_FAQ.html
    // The value of GL_MAX_TEXTURE_UNITS is 4 for GeForce FX and GeForce 6 Series GPUs. Why is that, since those GPUs have 16 texture units?
    CHECKGL (glGetIntegerv (GL_MAX_TEXTURE_UNITS, &_opengl_max_texture_units));
    CHECKGL (glGetIntegerv (GL_MAX_TEXTURE_COORDS, &_opengl_max_texture_coords));
    CHECKGL (glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &_opengl_max_texture_image_units));
    CHECKGL (glGetIntegerv (GL_MAX_VERTEX_ATTRIBS, &_opengl_max_vertex_attributes));
    CHECKGL (glGetIntegerv (GL_MAX_COLOR_ATTACHMENTS_EXT, &_opengl_max_fb_attachment));
#else
    _opengl_max_fb_attachment = 1;
#endif

#if defined(NUX_OS_WINDOWS)
    _support_ext_swap_control                 = WGLEW_EXT_swap_control ? true : false;
#elif defined(NUX_OS_LINUX) && !defined(NUX_OPENGLES_20)
    _support_ext_swap_control                 = GLXEW_SGI_swap_control ? true : false;
#endif

#ifndef NUX_OPENGLES_20
    _support_arb_vertex_program               = GLEW_ARB_vertex_program ? true : false;
    _support_arb_fragment_program             = GLEW_ARB_fragment_program ? true : false;
    _support_ext_framebuffer_object           = GLEW_EXT_framebuffer_object ? true : false;
    _support_arb_shader_objects               = GLEW_ARB_shader_objects ? true : false;
    _support_arb_vertex_shader                = GLEW_ARB_vertex_shader ? true : false;
    _support_arb_fragment_shader              = GLEW_ARB_fragment_shader ? true : false;
    _support_arb_vertex_buffer_object         = GLEW_ARB_vertex_buffer_object ? true : false;
    _support_arb_texture_non_power_of_two     = GLEW_ARB_texture_non_power_of_two ? true : false;
    _support_ext_draw_range_elements          = GLEW_EXT_draw_range_elements ? true : false;
    _support_ext_stencil_two_side             = GLEW_EXT_stencil_two_side ? true : false;
    _support_ext_texture_rectangle            = GLEW_EXT_texture_rectangle ? true : false;
    _support_arb_texture_rectangle            = GLEW_ARB_texture_rectangle ? true : false;
    _support_nv_texture_rectangle             = GLEW_NV_texture_rectangle ? true : false;
    _support_arb_pixel_buffer_object          = GLEW_ARB_pixel_buffer_object ? true : false;
    _support_ext_blend_equation_separate      = GLEW_EXT_blend_equation_separate ? true : false;
    _support_ext_texture_srgb                 = GLEW_EXT_texture_sRGB ? true : false;
    _support_ext_texture_srgb_decode          = false; //GLEW_EXT_texture_sRGB_decode ? true : false;
    _support_ext_framebuffer_srgb             = GLEW_EXT_framebuffer_sRGB ? true : false;
    _support_arb_framebuffer_srgb             = GLEW_ARB_framebuffer_sRGB ? true : false;
#else
    _support_arb_vertex_program               = false;
    _support_arb_fragment_program             = false;
    _support_arb_shader_objects               = true;
    _support_arb_vertex_shader                = true;
    _support_arb_fragment_shader              = true;
    _support_arb_vertex_buffer_object         = true;
    _support_arb_texture_non_power_of_two     = true;
    _support_ext_framebuffer_object           = true;
    _support_ext_draw_range_elements          = false;
    _support_ext_stencil_two_side             = false;
    _support_ext_texture_rectangle            = false;
    _support_arb_texture_rectangle            = false;
    _support_nv_texture_rectangle             = false;
    _support_arb_pixel_buffer_object          = false;
    _support_ext_blend_equation_separate      = true;
#endif
  }

#if defined (NUX_OS_WINDOWS)
  GpuDevice::GpuDevice (t_u32 DeviceWidth, t_u32 DeviceHeight, BitmapFormat DeviceFormat,
    HDC device_context,
    HGLRC &opengl_rendering_context,
    int req_opengl_major,
    int req_opengl_minor,
    bool opengl_es_20)
#elif defined (NUX_OS_LINUX)
#ifdef NUX_OPENGLES_20
  GpuDevice::GpuDevice (t_u32 DeviceWidth, t_u32 DeviceHeight, BitmapFormat DeviceFormat,
    Display *display,
    Window window,
    bool has_glx_13_support,
    EGLConfig fb_config,
    EGLContext &opengl_rendering_context,
    int req_opengl_major,
    int req_opengl_minor,
    bool opengl_es_20)
#else
  GpuDevice::GpuDevice (t_u32 DeviceWidth, t_u32 DeviceHeight, BitmapFormat DeviceFormat,
    Display *display,
    Window window,
    bool has_glx_13_support,
    GLXFBConfig fb_config,
    GLXContext &opengl_rendering_context,
    int req_opengl_major,
    int req_opengl_minor,
    bool opengl_es_20)
#endif
#endif
  {
    _PixelStoreAlignment  = 4;
    _UsePixelBufferObject = false;
    _gpu_info             = NULL;
    _gpu_brand            = GPU_VENDOR_UNKNOWN;

#ifndef NUX_OPENGLES_20    
    // OpenGL extension initialization
    GLenum Glew_Ok = 0;
#ifdef GLEW_MX
    Glew_Ok = glewContextInit (glewGetContext() );
    nuxAssertMsg (Glew_Ok == GLEW_OK, TEXT ("[GpuDevice::GpuDevice] GL Extensions failed to initialize."));

#if defined(NUX_OS_WINDOWS)
    Glew_Ok = wglewContextInit (wglewGetContext() );
#elif defined(NUX_OS_LINUX)
    Glew_Ok = glxewContextInit (glxewGetContext() );
#elif defined(NUX_OS_MACOSX)
    Glew_Ok = glxewContextInit (glxewGetContext() );
#endif

    nuxAssertMsg (Glew_Ok == GLEW_OK, TEXT ("[GpuDevice::GpuDevice] OpenGL Extensions failed to initialize."));
#else
    Glew_Ok = glewInit();
#endif
#endif

#ifndef NUX_OPENGLES_20
    CHECKGL (glGetIntegerv (GL_MAJOR_VERSION, &_opengl_major));
    CHECKGL (glGetIntegerv (GL_MINOR_VERSION, &_opengl_minor));
#else
    _opengl_major = 2;
    _opengl_minor = 0;
#endif

#if defined (NUX_OS_WINDOWS)
    bool opengl_es_context_created = false;
    if (((_opengl_major >= 3) && (req_opengl_major >= 3)) || (_opengl_major >= 3) || opengl_es_20)
#elif defined (NUX_OS_LINUX)
    //bool opengl_es_context_created = false;
    if (has_glx_13_support && (((_opengl_major >= 3) && (req_opengl_major >= 3)) || ((_opengl_major >= 3) && opengl_es_20)))
#endif
    {
      // Create a new Opengl Rendering Context
      bool requested_profile_is_supported = false;
      int index = 0;
      for (index = 0; OpenGLVersionTable [index].major != 0; index++)
      {
        if ((OpenGLVersionTable[index].major == req_opengl_major) &&
          (OpenGLVersionTable[index].minor == req_opengl_minor))
        {
          if (_opengl_major == 1)
          {
            if ((req_opengl_major == 1) && (req_opengl_minor >= 0) && (req_opengl_minor <= 5))
              requested_profile_is_supported = true;
          }
          else if (_opengl_major == 2)
          {
            if ((req_opengl_major == 2) && (req_opengl_minor >= 0) && (req_opengl_minor <= 1))
              requested_profile_is_supported = true;
          }
          else if (_opengl_major == 3)
          {
            if ((req_opengl_major == 3) && (req_opengl_minor >= 0) && (req_opengl_minor <= 3))
              requested_profile_is_supported = true;
          }
          else if (_opengl_major == 4)
          {
            if ((req_opengl_major == 4) && (req_opengl_minor >= 0) && (req_opengl_minor <= 1))
              requested_profile_is_supported = true;
          }
          break;
        }
      }

      if (opengl_es_20)
      {
#if defined (NUX_OS_WINDOWS)
        int attribs[] =
        {
          WGL_CONTEXT_MAJOR_VERSION_ARB,  2,
          WGL_CONTEXT_MINOR_VERSION_ARB,  0,
          WGL_CONTEXT_PROFILE_MASK_ARB,   //WGL_CONTEXT_ES2_PROFILE_BIT_EXT,
          0
        };

        HGLRC new_opengl_rendering_context = wglCreateContextAttribsARB(device_context,0, attribs);

        if (new_opengl_rendering_context == 0)
        {
          nuxDebugMsg (TEXT("[GpuDevice::GpuDevice] OpenGL ES 2.0 context creation has failed."));
        }
        else
        {
          wglMakeCurrent (NULL, NULL);
          wglDeleteContext (opengl_rendering_context);
          opengl_rendering_context = new_opengl_rendering_context;
          wglMakeCurrent (device_context, opengl_rendering_context);
          opengl_es_context_created = true;
        }
#elif defined (NUX_OS_LINUX)
/*        int attribs[] =
        {
          GLX_CONTEXT_MAJOR_VERSION_ARB,  2,
          GLX_CONTEXT_MINOR_VERSION_ARB,  0,
          //GLX_CONTEXT_PROFILE_MASK_ARB,   GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
          0
        };

        GLXContext new_opengl_rendering_context = glXCreateContextAttribsARB(display, fb_config, 0, true, attribs);

        if (new_opengl_rendering_context == 0)
        {
          nuxDebugMsg (TEXT("[GpuDevice::GpuDevice] OpenGL ES 2.0 context creation has failed."));
        }
        else
        {
          opengl_rendering_context = new_opengl_rendering_context;
          glXMakeCurrent (display, window, opengl_rendering_context);
          opengl_es_context_created = true;
        }*/
#endif
      }
      else if (requested_profile_is_supported)
      {
        int profile_mask = 0;
        int profile_value = 0;
        int flag_mask = 0;
        int flag_value = 0;

#if defined (NUX_OS_WINDOWS)
        if (((req_opengl_major == 3) && (req_opengl_minor >= 3)) || (req_opengl_major >= 4))
        {
          profile_mask = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
          profile_value = WGL_CONTEXT_PROFILE_MASK_ARB;
          flag_mask = WGL_CONTEXT_FLAGS_ARB;
          flag_value = 0;
        }

        int attribs[] =
        {
          WGL_CONTEXT_MAJOR_VERSION_ARB,  req_opengl_major,
          WGL_CONTEXT_MINOR_VERSION_ARB,  req_opengl_minor,
          profile_mask,                   profile_value,
          flag_mask,                      flag_value,
          0
        };

        HGLRC new_opengl_rendering_context = wglCreateContextAttribsARB(device_context,0, attribs);

        if (new_opengl_rendering_context == 0)
        {
          nuxDebugMsg (TEXT("[GpuDevice::GpuDevice] OpenGL version %d.%d context creation has failed."), req_opengl_major, req_opengl_minor);
        }
        else
        {
          wglMakeCurrent (NULL, NULL);
          wglDeleteContext (opengl_rendering_context);
          opengl_rendering_context = new_opengl_rendering_context;
          wglMakeCurrent (device_context, opengl_rendering_context);
        }
#elif defined(NUX_OS_LINUX) && !defined(NUX_OPENGLES_20)
        if (((req_opengl_major == 3) && (req_opengl_minor >= 3)) || (req_opengl_major >= 4))
        {
          profile_mask  = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
          profile_value = GLX_CONTEXT_PROFILE_MASK_ARB;
          flag_mask     = GLX_CONTEXT_FLAGS_ARB;
          flag_value    = 0;
        }

        int attribs[] =
        {
          GLX_CONTEXT_MAJOR_VERSION_ARB,  req_opengl_major,
          GLX_CONTEXT_MINOR_VERSION_ARB,  req_opengl_minor,
          profile_mask,                   profile_value,
          flag_mask,                      flag_value,
          0
        };

        GLXContext new_opengl_rendering_context = glXCreateContextAttribsARB(display, fb_config, 0, true, attribs);

        if (new_opengl_rendering_context == 0)
        {
          nuxDebugMsg (TEXT("[GpuDevice::GpuDevice] OpenGL version %d.%d context creation has failed."), req_opengl_major, req_opengl_minor);
          attribs[0] = 1; // major version
          attribs[1] = 0; // minor version
          attribs[2] = 0;
          new_opengl_rendering_context = glXCreateContextAttribsARB(display, fb_config, 0, true, attribs);

          opengl_rendering_context = new_opengl_rendering_context;
          glXMakeCurrent (display, window, opengl_rendering_context);
        }
        else
        {
          opengl_rendering_context = new_opengl_rendering_context;
          glXMakeCurrent (display, window, opengl_rendering_context);
        }
#endif
      }
      else
      {
        nuxDebugMsg (TEXT("[GpuDevice::GpuDevice] Using highest default OpenGL version."));
      }
    }
    else
    {
      opengl_rendering_context = 0;
    }

    _board_vendor_string = ANSI_TO_TCHAR (NUX_REINTERPRET_CAST (const char *, glGetString (GL_VENDOR) ) );
    CHECKGL_MSG (glGetString (GL_VENDOR) );
    _board_renderer_string = ANSI_TO_TCHAR (NUX_REINTERPRET_CAST (const char *, glGetString (GL_RENDERER) ) );
    CHECKGL_MSG (glGetString (GL_RENDERER) );
    _openGL_version_string = ANSI_TO_TCHAR (NUX_REINTERPRET_CAST (const char *, glGetString (GL_VERSION) ) );
    CHECKGL_MSG (glGetString (GL_VERSION) );
#ifndef NUX_OPENGLES_20
    if (GLEW_VERSION_2_0)
#else
    if (1)
#endif
    {
      _glsl_version_string = ANSI_TO_TCHAR (NUX_REINTERPRET_CAST (const char *, glGetString (GL_SHADING_LANGUAGE_VERSION) ) );
      CHECKGL_MSG (glGetString (GL_SHADING_LANGUAGE_VERSION) );

      NString glsl_major;
      NString glsl_minor;
      TCHAR split = TEXT('.');
      _glsl_version_string.SplitAtFirstOccurenceOf(split, glsl_major, glsl_minor);

    }

    nuxDebugMsg (TEXT ("Gpu Vendor: %s"), _board_vendor_string.GetTCharPtr() );
    nuxDebugMsg (TEXT ("Gpu Renderer: %s"), _board_renderer_string.GetTCharPtr() );
    nuxDebugMsg (TEXT ("Gpu OpenGL Version: %s"), _openGL_version_string.GetTCharPtr() );
    nuxDebugMsg (TEXT ("Gpu GLSL Version: %s"), _glsl_version_string.GetTCharPtr() );

#ifndef NUX_OPENGLES_20
    // Get the version supported by the context that was set.
    int new_opengl_major;
    int new_opengl_minor;
    CHECKGL (glGetIntegerv (GL_MAJOR_VERSION, &new_opengl_major));
    CHECKGL (glGetIntegerv (GL_MINOR_VERSION, &new_opengl_minor));

    if ((new_opengl_major != _opengl_major) || (new_opengl_minor != _opengl_minor))
    {
      nuxDebugMsg (TEXT ("The Gpu supports OpenGL %d.%d but version %d.%d has been requested."), _opengl_major, _opengl_minor, new_opengl_major, new_opengl_minor);
    }
#endif

    NString TempStr = (const TCHAR *) TCharToUpperCase (_board_vendor_string.GetTCharPtr() );

    if (TempStr.FindFirstOccurence (TEXT ("NVIDIA") ) != tstring::npos)
    {
      _gpu_brand = GPU_BRAND_NVIDIA;
    }
    else if (TempStr.FindFirstOccurence (TEXT ("ATI") ) != tstring::npos)
    {
      _gpu_brand = GPU_BRAND_AMD;
    }
    else if (TempStr.FindFirstOccurence (TEXT ("TUNGSTEN") ) != tstring::npos)
    {
      _gpu_brand = GPU_BRAND_INTEL;
    }

    if (0)
    {
      if (GetGPUBrand() == GPU_BRAND_AMD)
        _UsePixelBufferObject = false;
      else
        _UsePixelBufferObject = true;
    }
    else
    {
      _UsePixelBufferObject = false;
    }

    _gpu_info = new GpuInfo ();
    _gpu_info->Setup ();
    _gpu_render_states = new GpuRenderStates (_gpu_brand, _gpu_info);

#if defined(NUX_OS_WINDOWS)
    OGL_EXT_SWAP_CONTROL                = WGLEW_EXT_swap_control ? true : false;
#elif defined(NUX_OS_LINUX) && !defined(NUX_OPENGLES_20)
    OGL_EXT_SWAP_CONTROL                = GLXEW_SGI_swap_control ? true : false;
#endif

    InitTextureFormats();

    // See Avoiding 16 Common OpenGL Pitfalls
    // 7. Watch Your Pixel Store Alignment
    // http://www.opengl.org/resources/features/KilgardTechniques/oglpitfall/
    // We use a pack /unpack alignment to 1 so we don't have any padding at the end of row.

    CHECKGL (glPixelStorei (GL_UNPACK_ALIGNMENT, _PixelStoreAlignment));
    CHECKGL (glPixelStorei (GL_PACK_ALIGNMENT, _PixelStoreAlignment));

//     _DeviceWidth = DeviceWidth;
//     _DeviceHeight = DeviceHeight;
// 
//     _ViewportX = 0;
//     _ViewportY = 0;
//     _ViewportWidth = DeviceWidth;
//     _ViewportHeight = DeviceHeight;

    for (int i = 0; i < MAX_NUM_STREAM; i++)
    {
      _StreamSource[i].ResetStreamSource();
    }

    // Configure NVidia CG
#if (NUX_ENABLE_CG_SHADERS)
    {
      m_Cgcontext = 0;

      // Create Cg context and set profile.
      CHECKGL ( cgSetErrorCallback ( cgErrorCallback ) );
      m_Cgcontext = cgCreateContext();
      nuxAssert (m_Cgcontext);
      //CHECKGL( cgGLEnableProfile( CG_PROFILE_VP40 ) );
      //CHECKGL( cgGLEnableProfile( CG_PROFILE_FP40 ) );
      CHECKGL ( cgGLSetManageTextureParameters ( m_Cgcontext, CG_FALSE ) );
    }
#endif

    if (GetGpuInfo ().Support_EXT_Framebuffer_Object ())
    {
      _FrameBufferObject = CreateFrameBufferObject ();
      _FrameBufferObject->Deactivate ();
    }
  }

  GpuDevice::~GpuDevice()
  {
    NUX_SAFE_DELETE (_gpu_info);
    NUX_SAFE_DELETE (_gpu_render_states);

    _FrameBufferObject.Release ();
    _CurrentFrameBufferObject.Release ();

    _PixelBufferArray.clear ();

    for (int i = 0; i < MAX_NUM_STREAM; i++)
    {
      _StreamSource[i].ResetStreamSource();
    }
    // NVidia CG
#if (NUX_ENABLE_CG_SHADERS)
    cgDestroyContext (m_Cgcontext);
#endif

  }

  ObjectPtr<IOpenGLFrameBufferObject> GpuDevice::CreateFrameBufferObject()
  {
    IOpenGLFrameBufferObject *ptr;
    CreateFrameBufferObject ( (IOpenGLFrameBufferObject **) &ptr);
    ObjectPtr<IOpenGLFrameBufferObject> h = ObjectPtr<IOpenGLFrameBufferObject> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateFrameBufferObject (IOpenGLFrameBufferObject **ppFrameBufferObject)
  {
    *ppFrameBufferObject = new IOpenGLFrameBufferObject(NUX_TRACKER_LOCATION);

    return OGL_OK;
  }

  int GpuDevice::GetOpenGLMajorVersion () const
  {
    return _opengl_major;
  }

  int GpuDevice::GetOpenGLMinorVersion () const
  {
    return _opengl_minor;
  }

  GpuBrand GpuDevice::GetGPUBrand() const
  {
    return _gpu_brand;
  }

  GpuRenderStates &GpuDevice::GetRenderStates()
  {
    return *_gpu_render_states;
  }

  GpuInfo &GpuDevice::GetGpuInfo()
  {
    return *_gpu_info;
  }

  void GpuDevice::ResetRenderStates()
  {
    _gpu_render_states->ResetStateChangeToDefault();
  }

  void GpuDevice::VerifyRenderStates()
  {
    _gpu_render_states->CheckStateChange();
  }

  void GpuDevice::InvalidateTextureUnit (int TextureUnitIndex)
  {
    CHECKGL (glActiveTextureARB (TextureUnitIndex) );

    CHECKGL (glBindTexture (GL_TEXTURE_2D, 0) );
#ifndef NUX_OPENGLES_20
    CHECKGL (glBindTexture (GL_TEXTURE_1D, 0) );
    CHECKGL (glBindTexture (GL_TEXTURE_CUBE_MAP, 0) );
    CHECKGL (glBindTexture (GL_TEXTURE_3D, 0) );
    CHECKGL (glBindTexture (GL_TEXTURE_RECTANGLE_ARB, 0) );
#endif

    // From lowest priority to highest priority:
    //      GL_TEXTURE_1D,
    //      GL_TEXTURE_2D,
    //      GL_TEXTURE_RECTANGLE_ARB,
    //      GL_TEXTURE_3D,
    //      GL_TEXTURE_CUBE_MAP.

#ifndef NUX_OPENGLES_20
    CHECKGL (glDisable (GL_TEXTURE_2D) );
    CHECKGL (glDisable (GL_TEXTURE_1D) );
    CHECKGL (glDisable (GL_TEXTURE_RECTANGLE_ARB) );
    CHECKGL (glDisable (GL_TEXTURE_3D) );
    CHECKGL (glDisable (GL_TEXTURE_CUBE_MAP) );
#endif
  }

  int GpuDevice::AllocateUnpackPixelBufferIndex (int *index)
  {
    t_u32 num = (t_u32) _PixelBufferArray.size();

    for (t_u32 i = 0; i < num; i++)
    {
      if (_PixelBufferArray[i].IsReserved == FALSE)
      {
        _PixelBufferArray[i].IsReserved = TRUE;
        *index = i;
        return OGL_OK;
      }
    }

    // Not enough free pbo
    PixelBufferObject pbo;
    pbo.PBO = CreatePixelBufferObject (4, (VBO_USAGE) GL_STATIC_DRAW);
    pbo.IsReserved = TRUE;
    _PixelBufferArray.push_back (pbo);
    *index = (int) _PixelBufferArray.size() - 1;
    return OGL_OK;
  }

  int GpuDevice::FreeUnpackPixelBufferIndex (const int index)
  {
    t_s32 num = (t_s32) _PixelBufferArray.size();
    nuxAssertMsg ( (index >= 0) && (index < num), TEXT ("[GpuDevice::FreeUnpackPixelBufferIndex] Trying to Free a pixel buffer index that does not exist.") );

    if ( (index < 0) || (index >= num) )
    {
      return OGL_ERROR;
    }

    _PixelBufferArray[index].IsReserved = false;

// //     if(0)
// //     {
// //         // Can we realloc the memory used by the buffer with much less memory (4x4bytes)???
// //         CHECKGL( glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, OpenGLID) );
// //         CHECKGL( glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, 4*4, NULL, GL_STREAM_DRAW_ARB) );
// //     }
    return OGL_OK;
  }

  void *GpuDevice::LockUnpackPixelBufferIndex (const int index, int Size)
  {
#ifndef NUX_OPENGLES_20
    BindUnpackPixelBufferIndex (index);
    CHECKGL ( glBufferDataARB (GL_PIXEL_UNPACK_BUFFER_ARB, Size, NULL, GL_STREAM_DRAW) );
    void *pBits = glMapBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    CHECKGL_MSG (glMapBufferARB );
    CHECKGL ( glBindBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB, 0) );
    return pBits;
#else
	return NULL;
#endif
  }

  void* GpuDevice::LockPackPixelBufferIndex (const int index, int Size)
  {
#ifndef NUX_OPENGLES_20
    BindPackPixelBufferIndex (index);
    CHECKGL ( glBufferDataARB (GL_PIXEL_PACK_BUFFER_ARB, Size, NULL, GL_STREAM_DRAW) );
    void *pBits = glMapBufferARB (GL_PIXEL_PACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    CHECKGL_MSG (glMapBufferARB );
    CHECKGL ( glBindBufferARB (GL_PIXEL_PACK_BUFFER_ARB, 0) );
    return pBits;
#else
	return NULL;
#endif
  }

  void GpuDevice::UnlockUnpackPixelBufferIndex (const int index)
  {
#ifndef NUX_OPENGLES_20
    BindUnpackPixelBufferIndex (index);
    CHECKGL ( glUnmapBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB) );
    CHECKGL ( glBindBufferARB (GL_PIXEL_PACK_BUFFER_ARB, 0) );
#endif
  }

  void GpuDevice::UnlockPackPixelBufferIndex (const int index)
  {
#ifndef NUX_OPENGLES_20
    BindPackPixelBufferIndex (index);
    CHECKGL ( glUnmapBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB) );
    CHECKGL ( glBindBufferARB (GL_PIXEL_PACK_BUFFER_ARB, 0) );
#endif
  }

  int GpuDevice::BindUnpackPixelBufferIndex (const int index)
  {
    t_s32 num = (t_s32) _PixelBufferArray.size();
    nuxAssertMsg ( (index >= 0) && (index < num), TEXT ("[GpuDevice::BindUnpackPixelBufferIndex] Trying to bind an invalid pixel buffer index.") );

    if ( (index < 0) || (index >= num) )
    {
      return OGL_ERROR;
    }

    nuxAssertMsg (_PixelBufferArray[index].IsReserved == true, TEXT ("[GpuDevice::BindUnpackPixelBufferIndex] Trying to reserved pixel buffer index.") );

    if (_PixelBufferArray[index].IsReserved == false)
    {
      return OGL_ERROR;
    }

    _PixelBufferArray[index].PBO->BindUnpackPixelBufferObject();
    return OGL_OK;
  }

  int GpuDevice::BindPackPixelBufferIndex (const int index)
  {
    t_s32 num = (t_s32) _PixelBufferArray.size();
    nuxAssertMsg ( (index >= 0) && (index < num), TEXT ("[GpuDevice::BindPackPixelBufferIndex] Trying to bind an invalid pixel buffer index.") );

    if ( (index < 0) || (index >= num) )
    {
      return OGL_ERROR;
    }

    nuxAssertMsg (_PixelBufferArray[index].IsReserved == true, TEXT ("[GpuDevice::BindPackPixelBufferIndex] Trying to reserved pixel buffer index.") );

    if (_PixelBufferArray[index].IsReserved == false)
    {
      return OGL_ERROR;
    }

    _PixelBufferArray[index].PBO->BindPackPixelBufferObject();
    return OGL_OK;
  }

  int GpuDevice::FormatFrameBufferObject (t_u32 Width, t_u32 Height, BitmapFormat PixelFormat)
  {
    if (!GetGpuInfo ().Support_EXT_Framebuffer_Object ())
    {
      nuxDebugMsg (TEXT ("[GpuDevice::FormatFrameBufferObject] No support for OpenGL framebuffer extension.") );
      return 0;
    }

    return _FrameBufferObject->FormatFrameBufferObject (Width, Height, PixelFormat);
  }

  int GpuDevice::SetColorRenderTargetSurface (t_u32 ColorAttachmentIndex, ObjectPtr<IOpenGLSurface> pRenderTargetSurface)
  {
    if (!GetGpuInfo ().Support_EXT_Framebuffer_Object ())
    {
      nuxDebugMsg (TEXT ("[GpuDevice::SetColorRenderTargetSurface] No support for OpenGL framebuffer extension.") );
      return 0;
    }

    return _FrameBufferObject->SetRenderTarget (ColorAttachmentIndex, pRenderTargetSurface);
  }

  int GpuDevice::SetDepthRenderTargetSurface (ObjectPtr<IOpenGLSurface> pDepthSurface)
  {
    if (!GetGpuInfo ().Support_EXT_Framebuffer_Object ())
    {
      nuxDebugMsg (TEXT ("[GpuDevice::SetDepthRenderTargetSurface] No support for OpenGL framebuffer extension.") );
      return 0;
    }

    return _FrameBufferObject->SetDepthSurface (pDepthSurface);
  }

  ObjectPtr<IOpenGLSurface> GpuDevice::GetColorRenderTargetSurface (t_u32 ColorAttachmentIndex)
  {
    if (!GetGpuInfo ().Support_EXT_Framebuffer_Object ())
    {
      nuxDebugMsg (TEXT ("[GpuDevice::GetColorRenderTargetSurface] No support for OpenGL framebuffer extension.") );
      return ObjectPtr<IOpenGLSurface> (0);
    }

    return _FrameBufferObject->GetRenderTarget (ColorAttachmentIndex);
  }

  ObjectPtr<IOpenGLSurface> GpuDevice::GetDepthRenderTargetSurface()
  {
    if (!GetGpuInfo ().Support_EXT_Framebuffer_Object ())
    {
      nuxDebugMsg (TEXT ("[GpuDevice::GetDepthRenderTargetSurface] No support for OpenGL framebuffer extension.") );
      return ObjectPtr<IOpenGLSurface> (0);
    }

    return _FrameBufferObject->GetDepthRenderTarget();
  }

  void GpuDevice::ActivateFrameBuffer()
  {
    if (!GetGpuInfo ().Support_EXT_Framebuffer_Object ())
    {
      nuxDebugMsg (TEXT ("[GpuDevice::ActivateFrameBuffer] No support for OpenGL framebuffer extension.") );
      return;
    }

    _FrameBufferObject->Activate();
  }

  void GpuDevice::SetCurrentFrameBufferObject (ObjectPtr<IOpenGLFrameBufferObject> fbo)
  {
    _CurrentFrameBufferObject = fbo;
  }

  ObjectPtr<IOpenGLFrameBufferObject> GpuDevice::GetCurrentFrameBufferObject()
  {
    return _CurrentFrameBufferObject;
  }

  void GpuDevice::DeactivateFrameBuffer ()
  {
    if (!GetGpuInfo ().Support_EXT_Framebuffer_Object () )
    {
      nuxDebugMsg (TEXT ("[GpuDevice::DeactivateFrameBuffer] No support for OpenGL framebuffer extension.") );
      return;
    }

    _CurrentFrameBufferObject.Release ();
    CHECKGL ( glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0));
    CHECKGL ( glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, 0));
  }

  ObjectPtr<IOpenGLBaseTexture> GpuDevice::CreateSystemCapableDeviceTexture (
    int Width
    , int Height
    , int Levels
    , BitmapFormat PixelFormat)
  {
    if(GetGpuInfo().Support_ARB_Texture_Non_Power_Of_Two())
    {
      return CreateTexture (Width, Height, Levels, PixelFormat);
    }

    if(GetGpuInfo().Support_EXT_Texture_Rectangle () || GetGpuInfo().Support_ARB_Texture_Rectangle ())
    {
      return CreateRectangleTexture (Width, Height, Levels, PixelFormat);
    }

    nuxAssertMsg(0, TEXT("[NuxGraphicsResources::CreateSystemCapableDeviceTexture] No support for non power of two textures or rectangle textures"));

    return ObjectPtr<IOpenGLBaseTexture>();
  }

  BaseTexture* GpuDevice::CreateSystemCapableTexture ()
  {
    if(GetGpuInfo().Support_ARB_Texture_Non_Power_Of_Two())
    {
      return new Texture2D (NUX_TRACKER_LOCATION);
    }

    if(GetGpuInfo().Support_EXT_Texture_Rectangle () || GetGpuInfo().Support_ARB_Texture_Rectangle ())
    {
      return new TextureRectangle (NUX_TRACKER_LOCATION);
    }

    nuxAssertMsg(0, TEXT("[NuxGraphicsResources::CreateSystemCapableTexture] No support for non power of two textures or rectangle textures"));

    return 0;
  }
}
