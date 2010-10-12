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


#include "GLResource.h"
#include "GLDeviceFactory.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GLDeviceFrameBufferObject.h"
#include "GLTemplatePrimitiveBuffer.h"
#include "OpenGLEngine.h"

namespace nux
{

#define MANAGEDEVICERESOURCE    0

  extern void cgErrorCallback (void);

// Pixel buffer object seems to corrupt textures. This can be seen when the window is moved
// to the second screen in a multi-display configuration...
// Happens on geforce 6600. Does not happens on geforce 6800.

// ATI Radeon 4670 has problems loading textures from pixel buffer object. PBO should be deactivated if the
// graphics card is made by AMD/ATI
#define NUX_USE_PBO     1

#define NUX_MISSING_GL_EXTENSION_MESSAGE_BOX(message) {MessageBox(NULL, TEXT("Missing extension: " #message), TEXT("ERROR"), MB_OK|MB_ICONERROR); exit(-1);}

// float Log2(float f)
// {
//     return logf(f) / logf(2);
// }

  extern PixelFormatInfo GPixelFormats[];

  struct ReqExtension
  {
    const char *extension0;
    const char *extension1;
    const char *extension2;
    int supported0;
    int supported1;
    int supported2;
  };

  struct ReqExtension ReqNVidiaExtension[] =
  {
    {"GL_VERSION_1_5",              "",                         "",                         0,  0,  0},
    {"GL_ARB_texture_rectangle",    "GL_NV_texture_rectangle",  "GL_EXT_texture_rectangle", 0,  0,  0},
    {"GL_ARB_vertex_program",       "",                         "",                         0,  0,  0},
    {"GL_ARB_fragment_program",     "",                         "",                         0,  0,  0},
    {"GL_ARB_shader_objects",       "",                         "",                         0,  0,  0},
    {"GL_ARB_vertex_shader",        "",                         "",                         0,  0,  0},
    {"GL_ARB_fragment_shader",      "",                         "",                         0,  0,  0},
    {"GL_ARB_vertex_buffer_object", "",                         "",                         0,  0,  0},
    {"GL_EXT_framebuffer_object",   "",                         "",                         0,  0,  0},
    {"GL_EXT_draw_range_elements",  "",                         "",                         0,  0,  0},
    {"GL_EXT_stencil_two_side",     "",                         "",                         0,  0,  0},
  };

  struct ReqExtension ReqNVidiaWGLExtension[] =
  {
    {"WGL_ARB_pbuffer",             "",                         "",                         0,  0,  0},
    {"WGL_ARB_pixel_format",        "",                         "",                         0,  0,  0},
    {"WGL_ARB_render_texture",      "",                         "",                         0,  0,  0},
    {"WGL_ARB_render_texture",      "",                         "",                         0,  0,  0},
  };

  static void InitializeExtension()
  {

    int NumReqExtension = sizeof (ReqNVidiaExtension) / sizeof (ReqNVidiaExtension[0]);

    for (int index = 0; index < NumReqExtension; index++)
    {
      ReqNVidiaExtension[index].supported0 = 0;
      ReqNVidiaExtension[index].supported1 = 0;
      ReqNVidiaExtension[index].supported2 = 0;

      if (ReqNVidiaExtension[index].extension0)
      {
        ReqNVidiaExtension[index].supported0 = glewIsSupported (ReqNVidiaExtension[index].extension0);
      }

      if (ReqNVidiaExtension[index].extension1)
      {
        ReqNVidiaExtension[index].supported1 = glewIsSupported (ReqNVidiaExtension[index].extension1);
      }

      if (ReqNVidiaExtension[index].extension2)
      {
        ReqNVidiaExtension[index].supported2 = glewIsSupported (ReqNVidiaExtension[index].extension2);
      }

      if ( (!ReqNVidiaExtension[index].supported0) &&
           (!ReqNVidiaExtension[index].supported1) &&
           (!ReqNVidiaExtension[index].supported2) )
      {
//            char error[512];
//            sprintf(error, "Error - required extensions were not supported: \n %s \n %s \n %s \n"
//                ,ReqNVidiaExtension[index].extension0
//                ,ReqNVidiaExtension[index].extension1
//                ,ReqNVidiaExtension[index].extension2);
//            MessageBox(NULL,error,"ERROR",MB_OK|MB_ICONERROR);
//            exit(-1);
      }

//        else
//        {
//            if(ReqNVidiaExtension[index].supported0)
//                glh_init_extensions(ReqNVidiaExtension[index].extension0);
//            if(ReqNVidiaExtension[index].supported1)
//                glh_init_extensions(ReqNVidiaExtension[index].extension1);
//            if(ReqNVidiaExtension[index].supported2)
//                glh_init_extensions(ReqNVidiaExtension[index].extension2);
//        }
    }

//
//     if (!glewIsSupported("GL_ARB_multitexture "
//         "GL_ARB_vertex_program "
//         "GL_ARB_fragment_program "
//         "GL_ARB_shader_objects "
//         "GL_ARB_vertex_shader "
//         "GL_ARB_fragment_shader "
//         //"GL_NV_float_buffer "
//         "GL_ARB_vertex_buffer_object "
//         //"GL_EXT_pixel_buffer_object "
//         //"GL_ARB_texture_non_power_of_two "
//         "GL_EXT_framebuffer_object "
//         "GL_ARB_texture_rectangle "
//         "GL_EXT_pixel_buffer_object"
//         ))
//     {
// //        char error[512];
// //        sprintf(error, "Error - required extensions were not supported: %s", "XXXX");
// //        MessageBox(NULL,error,"ERROR",MB_OK|MB_ICONERROR);
// //        //fprintf(stderr, "Error - required extensions were not supported: %s", glh_get_unsupported_extensions());
// //        exit(-1);
//     }
//
//     if (!wglewIsSupported(
//         "WGL_ARB_pbuffer "
//         "WGL_ARB_pixel_format "
//         "WGL_ARB_render_texture "
//         ))
//     {
// //        char error[512];
// //        sprintf(error, "Error - required extensions were not supported: %s", "XXXX");
// //        MessageBox(NULL,error,"ERROR",MB_OK|MB_ICONERROR);
// //        //fprintf(stderr, "Error - required extensions were not supported: %s", glh_get_unsupported_extensions());
// //        exit(-1);
//     }

  }

  static void InitTextureFormats()
  {
    GPixelFormats[ BITFMT_UNKNOWN		].PlatformFormat	= GL_NONE;							// Not supported for rendering.

    // Data in PC system memory: R(LSB) G B A(MSB) ---> GL Format:GL_RGBA - GL Type:GL_UNSIGNED_INT_8_8_8_8_REV
    GPixelFormats[ BITFMT_R8G8B8A8     ].PlatformFormat	= GL_RGBA8;
    GPixelFormats[ BITFMT_R8G8B8A8     ].Format	        = GL_RGBA;
    GPixelFormats[ BITFMT_R8G8B8A8     ].type	            = GL_UNSIGNED_INT_8_8_8_8_REV;

    // Data in PC system memory: A(LSB) B G R(MSB) ---> GL Format:GL_RGBA - GL Type:GL_UNSIGNED_INT_8_8_8_8
    GPixelFormats[ BITFMT_A8B8G8R8     ].PlatformFormat	= GL_RGBA8;
    GPixelFormats[ BITFMT_A8B8G8R8     ].Format	        = GL_RGBA;
    GPixelFormats[ BITFMT_A8B8G8R8     ].type	            = GL_UNSIGNED_INT_8_8_8_8;

    // Data in PC system memory: B(LSB) G R A(MSB) ---> GL Format:GL_BGRA - GL Type:GL_UNSIGNED_INT_8_8_8_8_REV
    GPixelFormats[ BITFMT_B8G8R8A8     ].PlatformFormat	= GL_RGBA8;
    GPixelFormats[ BITFMT_B8G8R8A8     ].Format	        = GL_BGRA;
    GPixelFormats[ BITFMT_B8G8R8A8     ].type	            = GL_UNSIGNED_INT_8_8_8_8_REV;

    // Data in PC system memory: A(LSB) R G B(MSB) ---> GL Format:GL_BGRA - GL Type:GL_UNSIGNED_INT_8_8_8_8
    GPixelFormats[ BITFMT_A8R8G8B8     ].PlatformFormat	= GL_RGBA8;
    GPixelFormats[ BITFMT_A8R8G8B8     ].Format	        = GL_BGRA;
    GPixelFormats[ BITFMT_A8R8G8B8     ].type	            = GL_UNSIGNED_INT_8_8_8_8;

    // Data in PC system memory: R(LSB) G B(MSB) ---> GL Format:GL_RGB - GL Type:GL_UNSIGNED
    GPixelFormats[ BITFMT_R8G8B8		].PlatformFormat	= GL_RGB8;
    GPixelFormats[ BITFMT_R8G8B8		].Format	        = GL_RGB;
    GPixelFormats[ BITFMT_R8G8B8		].type	            = GL_UNSIGNED_BYTE;

    GPixelFormats[ BITFMT_B8G8R8		].PlatformFormat	= GL_RGB8;
    GPixelFormats[ BITFMT_B8G8R8		].Format	        = GL_BGR;
    GPixelFormats[ BITFMT_B8G8R8		].type	            = GL_UNSIGNED_BYTE;

    GPixelFormats[ BITFMT_R5G6B5       ].PlatformFormat	= GL_RGB5;
    GPixelFormats[ BITFMT_R5G6B5       ].Format	        = GL_RGB;
    GPixelFormats[ BITFMT_R5G6B5       ].type	            = GL_UNSIGNED_SHORT_5_6_5;

    GPixelFormats[ BITFMT_RGBA16F		].PlatformFormat	= GL_RGBA16F_ARB;
    GPixelFormats[ BITFMT_RGBA16F		].Format	        = GL_RGBA;
    GPixelFormats[ BITFMT_RGBA16F		].type	            = GL_HALF_FLOAT_ARB;

    GPixelFormats[ BITFMT_RGB32F		].PlatformFormat	= GL_RGB;
    GPixelFormats[ BITFMT_RGB32F		].Format	        = GL_RGB;
    GPixelFormats[ BITFMT_RGB32F		].type	            = GL_FLOAT;

    GPixelFormats[ BITFMT_RGBA32F		].PlatformFormat	= GL_RGBA32F_ARB;
    GPixelFormats[ BITFMT_RGBA32F		].Format	        = GL_RGBA;
    GPixelFormats[ BITFMT_RGBA32F		].type	            = GL_FLOAT;

    // Note: Using GL_DEPTH_COMPONENT24 or GL_DEPTH_COMPONENT for PlatformFormat generate error GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT.
    GPixelFormats[ BITFMT_D24S8		].PlatformFormat	= GL_DEPTH24_STENCIL8_EXT;
    GPixelFormats[ BITFMT_D24S8		].Format	        = GL_DEPTH_STENCIL_EXT;     // or GL_DEPTH_STENCIL_NV;
    GPixelFormats[ BITFMT_D24S8		].type	            = GL_UNSIGNED_INT_24_8_EXT; // or GL_UNSIGNED_INT_24_8_NV;

    GPixelFormats[ BITFMT_DXT1			].PlatformFormat	= GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    GPixelFormats[ BITFMT_DXT2			].PlatformFormat	= GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    GPixelFormats[ BITFMT_DXT3			].PlatformFormat	= GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    GPixelFormats[ BITFMT_DXT4			].PlatformFormat	= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    GPixelFormats[ BITFMT_DXT5			].PlatformFormat	= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    GPixelFormats[ BITFMT_R10G10B10A2  ].PlatformFormat	= GL_RGB10_A2;
    GPixelFormats[ BITFMT_R10G10B10A2  ].Format	        = GL_RGBA;
    GPixelFormats[ BITFMT_R10G10B10A2  ].type	            = GL_UNSIGNED_INT_10_10_10_2;

    GPixelFormats[ BITFMT_A2B10G10R10  ].PlatformFormat	= GL_RGB10_A2;
    GPixelFormats[ BITFMT_A2B10G10R10  ].Format	        = GL_RGBA;
    GPixelFormats[ BITFMT_A2B10G10R10  ].type	            = GL_UNSIGNED_INT_2_10_10_10_REV;

    GPixelFormats[ BITFMT_B10G10R10A2  ].PlatformFormat	= GL_RGB10_A2;
    GPixelFormats[ BITFMT_B10G10R10A2  ].Format	        = GL_BGRA;
    GPixelFormats[ BITFMT_B10G10R10A2  ].type	            = GL_UNSIGNED_INT_10_10_10_2;

    GPixelFormats[ BITFMT_A2R10G10B10  ].PlatformFormat	= GL_RGB10_A2;
    GPixelFormats[ BITFMT_A2R10G10B10  ].Format	        = GL_BGRA;
    GPixelFormats[ BITFMT_A2R10G10B10  ].type	            = GL_UNSIGNED_INT_2_10_10_10_REV;

    GPixelFormats[ BITFMT_A8           ].PlatformFormat	= GL_RGBA8;
    GPixelFormats[ BITFMT_A8           ].Format	        = GL_LUMINANCE;
    GPixelFormats[ BITFMT_A8           ].type	            = GL_UNSIGNED_BYTE;
  }

  TRefGL<IOpenGLTexture2D> GLDeviceFactory::CreateTexture (
    int Width
    , int Height
    , int Levels
    , BitmapFormat PixelFormat)
  {
    IOpenGLTexture2D *ptr;
    CreateTexture (Width, Height, Levels, PixelFormat, (IOpenGLTexture2D **) &ptr);
    TRefGL<IOpenGLTexture2D> h = ptr;
    return h;
  }

  TRefGL<IOpenGLRectangleTexture> GLDeviceFactory::CreateRectangleTexture (
    int Width
    , int Height
    , int Levels
    , BitmapFormat PixelFormat)
  {
    IOpenGLRectangleTexture *ptr;
    CreateRectangleTexture (Width, Height, Levels, PixelFormat, (IOpenGLRectangleTexture **) &ptr);
    TRefGL<IOpenGLRectangleTexture> h = ptr;
    return h;
  }

  TRefGL<IOpenGLCubeTexture> GLDeviceFactory::CreateCubeTexture (
    int EdgeLength
    , int Levels
    , BitmapFormat PixelFormat)
  {
    IOpenGLCubeTexture *ptr;
    CreateCubeTexture (EdgeLength, Levels, PixelFormat, (IOpenGLCubeTexture **) &ptr);
    TRefGL<IOpenGLCubeTexture> h = ptr;
    return h;
  }

  TRefGL<IOpenGLVolumeTexture> GLDeviceFactory::CreateVolumeTexture (
    int Width
    , int Height
    , int Depth
    , int Levels
    , BitmapFormat PixelFormat)
  {
    IOpenGLVolumeTexture *ptr;
    CreateVolumeTexture (Width, Height, Depth, Levels, PixelFormat, (IOpenGLVolumeTexture **) &ptr);
    TRefGL<IOpenGLVolumeTexture> h = ptr;
    return h;
  }

  TRefGL<IOpenGLAnimatedTexture> GLDeviceFactory::CreateAnimatedTexture (
    int Width
    , int Height
    , int Depth
    , BitmapFormat PixelFormat)
  {
    IOpenGLAnimatedTexture *prt;
    CreateAnimatedTexture (Width, Height, Depth, PixelFormat, (IOpenGLAnimatedTexture **) &prt);
    TRefGL<IOpenGLAnimatedTexture> h = prt;
    return h;
  }


  TRefGL<IOpenGLQuery> GLDeviceFactory::CreateQuery (QUERY_TYPE Type)
  {
    IOpenGLQuery *prt;
    CreateQuery (Type, (IOpenGLQuery **) &prt);
    TRefGL<IOpenGLQuery> h = prt;
    return h;
  }

  TRefGL<IOpenGLFrameBufferObject> GLDeviceFactory::CreateFrameBufferObject()
  {
    IOpenGLFrameBufferObject *ptr;
    CreateFrameBufferObject ( (IOpenGLFrameBufferObject **) &ptr);
    TRefGL<IOpenGLFrameBufferObject> h = ptr;
    return h;
  }

  TRefGL<IOpenGLShaderProgram> GLDeviceFactory::CreateShaderProgram()
  {
    IOpenGLShaderProgram *ptr;
    CreateShaderProgram ( (IOpenGLShaderProgram **) &ptr);
    TRefGL<IOpenGLShaderProgram> h = ptr;
    return h;
  }

  TRefGL<IOpenGLVertexShader> GLDeviceFactory::CreateVertexShader()
  {
    IOpenGLVertexShader *ptr;
    CreateVertexShader ( (IOpenGLVertexShader **) &ptr);
    TRefGL<IOpenGLVertexShader> h = ptr;
    return h;
  }

  TRefGL<IOpenGLPixelShader> GLDeviceFactory::CreatePixelShader()
  {
    IOpenGLPixelShader *ptr;
    CreatePixelShader ( (IOpenGLPixelShader **) &ptr);
    TRefGL<IOpenGLPixelShader> h = ptr;
    return h;
  }

  TRefGL<IOpenGLAsmShaderProgram> GLDeviceFactory::CreateAsmShaderProgram()
  {
    IOpenGLAsmShaderProgram *ptr;
    CreateAsmShaderProgram ( (IOpenGLAsmShaderProgram **) &ptr);
    TRefGL<IOpenGLAsmShaderProgram> h = ptr;
    return h;
  }

  TRefGL<IOpenGLAsmVertexShader> GLDeviceFactory::CreateAsmVertexShader()
  {
    IOpenGLAsmVertexShader *ptr;
    CreateAsmVertexShader ( (IOpenGLAsmVertexShader **) &ptr);
    TRefGL<IOpenGLAsmVertexShader> h = ptr;
    return h;
  }

  TRefGL<IOpenGLAsmPixelShader> GLDeviceFactory::CreateAsmPixelShader()
  {
    IOpenGLAsmPixelShader *ptr;
    CreateAsmPixelShader ( (IOpenGLAsmPixelShader **) &ptr);
    TRefGL<IOpenGLAsmPixelShader> h = ptr;
    return h;
  }

#if (NUX_ENABLE_CG_SHADERS)
  TRefGL<ICgVertexShader> GLDeviceFactory::CreateCGVertexShader()
  {
    ICgVertexShader *ptr;
    CreateCGVertexShader ( (ICgVertexShader **) &ptr);
    TRefGL<ICgVertexShader> h = ptr;
    return h;
  }

  TRefGL<ICgPixelShader> GLDeviceFactory::CreateCGPixelShader()
  {
    ICgPixelShader *ptr;
    CreateCGPixelShader ( (ICgPixelShader **) &ptr);
    TRefGL<ICgPixelShader> h = ptr;
    return h;
  }
#endif

  void GLDeviceFactory::Initialize()
  {
    GLenum Glew_Ok = glewInit();

    if (Glew_Ok != GLEW_OK)
    {
      nuxAssertMsg (0, TEXT ("[GLDeviceFactory::Initialize] Failed glewInit.") );
    }

    InitializeExtension();
  }

  STREAMSOURCE GLDeviceFactory::_StreamSource[MAX_NUM_STREAM];
// TRefGL<IOpenGLIndexBuffer> GLDeviceFactory::_CurrentIndexBuffer = 0;
// TRefGL<IOpenGLVertexBuffer> GLDeviceFactory::_CurrentVertexBuffer = 0;
// TRefGL<IOpenGLVertexDeclaration> GLDeviceFactory::_CurrentVertexDeclaration = 0;

//void TestARBShaders()
//{
//    int OPENGL_PROGRAM_LENGTH_ARB;
//    int OPENGL_PROGRAM_FORMAT_ARB;
//    int OPENGL_PROGRAM_BINDING_ARB;
//    int OPENGL_PROGRAM_INSTRUCTIONS_ARB;
//    int OPENGL_MAX_PROGRAM_INSTRUCTIONS_ARB;
//    int OPENGL_PROGRAM_NATIVE_INSTRUCTIONS_ARB;
//    int OPENGL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB;
//    int OPENGL_PROGRAM_TEMPORARIES_ARB;
//    int OPENGL_MAX_PROGRAM_TEMPORARIES_ARB;
//    int OPENGL_PROGRAM_NATIVE_TEMPORARIES_ARB;
//    int OPENGL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB;
//    int OPENGL_PROGRAM_PARAMETERS_ARB;
//    int OPENGL_MAX_PROGRAM_PARAMETERS_ARB;
//    int OPENGL_PROGRAM_NATIVE_PARAMETERS_ARB;
//    int OPENGL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB;
//    int OPENGL_PROGRAM_ATTRIBS_ARB;
//    int OPENGL_MAX_PROGRAM_ATTRIBS_ARB;
//    int OPENGL_PROGRAM_NATIVE_ATTRIBS_ARB;
//    int OPENGL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB;
//    int OPENGL_PROGRAM_ADDRESS_REGISTERS_ARB;
//    int OPENGL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB;
//    int OPENGL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB;
//    int OPENGL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB;
//    int OPENGL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB;
//    int OPENGL_MAX_PROGRAM_ENV_PARAMETERS_ARB;
//    int OPENGL_PROGRAM_UNDER_NATIVE_LIMITS_ARB;
//
//    // BEWARE glGetProgramiv != glGetProgramivARB (ARB fragment vertex program)
//
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_LENGTH_ARB,                          &OPENGL_PROGRAM_LENGTH_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ARB,                          &OPENGL_PROGRAM_FORMAT_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_BINDING_ARB,                         &OPENGL_PROGRAM_BINDING_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_INSTRUCTIONS_ARB,                    &OPENGL_PROGRAM_INSTRUCTIONS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_INSTRUCTIONS_ARB,                &OPENGL_MAX_PROGRAM_INSTRUCTIONS_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB,             &OPENGL_PROGRAM_NATIVE_INSTRUCTIONS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB,         &OPENGL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_TEMPORARIES_ARB,                     &OPENGL_PROGRAM_TEMPORARIES_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_TEMPORARIES_ARB,                 &OPENGL_MAX_PROGRAM_TEMPORARIES_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEMPORARIES_ARB,              &OPENGL_PROGRAM_NATIVE_TEMPORARIES_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB,          &OPENGL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_PARAMETERS_ARB,                      &OPENGL_PROGRAM_PARAMETERS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB,                  &OPENGL_MAX_PROGRAM_PARAMETERS_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_PARAMETERS_ARB,               &OPENGL_PROGRAM_NATIVE_PARAMETERS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB,           &OPENGL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_ATTRIBS_ARB,                         &OPENGL_PROGRAM_ATTRIBS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ATTRIBS_ARB,                     &OPENGL_MAX_PROGRAM_ATTRIBS_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ATTRIBS_ARB,                  &OPENGL_PROGRAM_NATIVE_ATTRIBS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB,              &OPENGL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_ADDRESS_REGISTERS_ARB,               &OPENGL_PROGRAM_ADDRESS_REGISTERS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB,           &OPENGL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB,        &OPENGL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB,    &OPENGL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,            &OPENGL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB) );
//    CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,              &OPENGL_MAX_PROGRAM_ENV_PARAMETERS_ARB) );
//    //CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB,             &OPENGL_PROGRAM_UNDER_NATIVE_LIMITS_ARB) );
//}
//

  GLDeviceFactory::GLDeviceFactory (t_u32 DeviceWidth, t_u32 DeviceHeight, BitmapFormat DeviceFormat)
    :   _FrameBufferObject (0)
    ,   _PixelStoreAlignment (4)
    ,   _CachedPixelBufferObjectList (0)
    ,   _CachedVertexBufferList (0)
    ,   _CachedIndexBufferList (0)
    ,   _CachedTextureList (0)
    ,   _CachedTextureRectangleList (0)
    ,   _CachedCubeTextureList (0)
    ,   _CachedVolumeTextureList (0)
    ,   _CachedAnimatedTextureList (0)
    ,   _CachedQueryList (0)
    ,   _CachedVertexDeclarationList (0)
    ,   _CachedFrameBufferList (0)
    ,   _CachedVertexShaderList (0)
    ,   _CachedPixelShaderList (0)
    ,   _CachedShaderProgramList (0)
    ,   _CachedAsmVertexShaderList (0)
    ,   _CachedAsmPixelShaderList (0)
    ,   _CachedAsmShaderProgramList (0)
    ,   m_isINTELBoard (false)
    ,   m_isATIBoard (false)
    ,   m_isNVIDIABoard (false)
    ,   m_UsePixelBufferObject (false)
    ,   m_GraphicsBoardVendor (BOARD_UNKNOWN)
#if (NUX_ENABLE_CG_SHADERS)
    ,   _CachedCGVertexShaderList (0)
    ,   _CachedCGPixelShaderList (0)
    ,   m_Cgcontext (0)
#endif
  {
    inlSetThreadLocalStorage (ThreadLocal_GLDeviceFactory, this);

    GLenum Glew_Ok = 0;
#ifdef GLEW_MX
    Glew_Ok = glewContextInit (glewGetContext() );
    nuxAssertMsg (Glew_Ok == GLEW_OK, TEXT ("[GLDeviceFactory::GLDeviceFactory] GL Extensions failed to initialize.") );

#if defined(NUX_OS_WINDOWS)
    Glew_Ok = wglewContextInit (wglewGetContext() );
#elif defined(NUX_OS_LINUX)
    Glew_Ok = glxewContextInit (glxewGetContext() );
#elif defined(NUX_OS_MACOSX)
    Glew_Ok = glxewContextInit (glxewGetContext() );
#endif
    nuxAssertMsg (Glew_Ok == GLEW_OK, TEXT ("[GLDeviceFactory::GLDeviceFactory] WGL Extensions failed to initialize.") );
#else
    Glew_Ok = glewInit();
#endif

    InitializeExtension();

    //m_BoardVendorString = "aaaa";
    //std::string str = (const char*) glGetString(GL_VENDOR);
    m_BoardVendorString = ANSI_TO_TCHAR (NUX_REINTERPRET_CAST (const char *, glGetString (GL_VENDOR) ) );
    CHECKGL_MSG (glGetString (GL_VENDOR) );
    m_BoardRendererString = ANSI_TO_TCHAR (NUX_REINTERPRET_CAST (const char *, glGetString (GL_RENDERER) ) );
    CHECKGL_MSG (glGetString (GL_RENDERER) );
    m_OpenGLVersionString = ANSI_TO_TCHAR (NUX_REINTERPRET_CAST (const char *, glGetString (GL_VERSION) ) );
    CHECKGL_MSG (glGetString (GL_VERSION) );
    m_GLSLVersionString = ANSI_TO_TCHAR (NUX_REINTERPRET_CAST (const char *, glGetString (GL_SHADING_LANGUAGE_VERSION) ) );
    CHECKGL_MSG (glGetString (GL_SHADING_LANGUAGE_VERSION) );

    nuxDebugMsg (TEXT ("Board Vendor: %s"), m_BoardVendorString.GetTCharPtr() );
    nuxDebugMsg (TEXT ("Board Renderer: %s"), m_BoardRendererString.GetTCharPtr() );
    nuxDebugMsg (TEXT ("Board OpenGL Version: %s"), m_OpenGLVersionString.GetTCharPtr() );
    nuxDebugMsg (TEXT ("Board GLSL Version: %s"), m_GLSLVersionString.GetTCharPtr() );

    // See: http://developer.nvidia.com/object/General_FAQ.html
    // The value of GL_MAX_TEXTURE_UNITS is 4 for GeForce FX and GeForce 6 Series GPUs. Why is that, since those GPUs have 16 texture units?
    glGetIntegerv (GL_MAX_TEXTURE_UNITS, &OPENGL_MAX_TEXTURE_UNITS);
    glGetIntegerv (GL_MAX_TEXTURE_COORDS, &OPENGL_MAX_TEXTURE_COORDS);
    glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &OPENGL_MAX_TEXTURE_IMAGE_UNITS);

    glGetIntegerv (GL_MAX_VERTEX_ATTRIBS, &OPENGL_MAX_VERTEX_ATTRIBUTES);

//     // ARB shaders
//     int OPENGL_MAX_VERTEX_ATTRIBS_ARB;
//     int OPENGL_MAX_PROGRAM_MATRIX_STACK_DEPTH;
//
//     CHECKGL(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &OPENGL_MAX_VERTEX_ATTRIBS_ARB));
//     CHECKGL(glGetIntegerv(GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB, &OPENGL_MAX_PROGRAM_MATRIX_STACK_DEPTH));

    //TestARBShaders();

    OPENGL_MAX_FB_ATTACHMENT = 4;

    NString TempStr = (const TCHAR *) TCharToUpperCase (m_BoardVendorString.GetTCharPtr() );

    if (TempStr.FindFirstOccurence (TEXT ("NVIDIA") ) != tstring::npos)
    {
      m_isNVIDIABoard = true;
      m_GraphicsBoardVendor = BOARD_NVIDIA;
    }
    else if (TempStr.FindFirstOccurence (TEXT ("ATI") ) != tstring::npos)
    {
      m_isATIBoard = true;
      m_GraphicsBoardVendor = BOARD_ATI;
    }
    else if (TempStr.FindFirstOccurence (TEXT ("TUNGSTEN") ) != tstring::npos)
    {
      m_isINTELBoard = true;
      m_GraphicsBoardVendor = BOARD_INTEL;
    }

    if (NUX_USE_PBO)
    {
      if (isATIBoard() )
        m_UsePixelBufferObject = false;
      else
        m_UsePixelBufferObject = true;
    }
    else
    {
      m_UsePixelBufferObject = false;
    }

    m_RenderStates = new GLRenderStates (m_GraphicsBoardVendor);

    OPENGL_VERSION_1_1 = GLEW_VERSION_1_1 ? true : false;
    OPENGL_VERSION_1_2 = GLEW_VERSION_1_2 ? true : false;
    OPENGL_VERSION_1_3 = GLEW_VERSION_1_3 ? true : false;
    OPENGL_VERSION_1_4 = GLEW_VERSION_1_4 ? true : false;
    OPENGL_VERSION_1_5 = GLEW_VERSION_1_5 ? true : false;
    OPENGL_VERSION_2_0 = GLEW_VERSION_2_0 ? true : false;
    OPENGL_VERSION_2_1 = GLEW_VERSION_2_1 ? true : false;
    GLSL_VERSION_1_0 = 1;
    GLSL_VERSION_1_1 = 1;

    if (OPENGL_VERSION_2_1 == false)
    {
      //nuxDebugMsg(TEXT("[GLDeviceFactory::GLDeviceFactory] No support for OpenGL 2.1"));
//         inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal,
//             TEXT("No Support for OpenGL 2.1.\nThe program will exit."));
//         exit(-1);
    }

    if (GLEW_EXT_framebuffer_object == false)
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::GLDeviceFactory] No support for Framebuffer Objects.") );
//         inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal,
//             TEXT("No support for Framebuffer Objects.\nThe program will exit."));
//         exit(-1);
    }

    if ( (GLEW_ARB_texture_rectangle == false) && (GLEW_EXT_texture_rectangle == false) )
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::GLDeviceFactory] No support for rectangle textures.") );
//         inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal,
//             TEXT("No support for rectangle textures.\nThe program will exit."));
//         exit(-1);
    }

#if defined(NUX_OS_WINDOWS)
    OGL_EXT_SWAP_CONTROL                = WGLEW_EXT_swap_control ? true : false;
#elif defined(NUX_OS_LINUX)
    OGL_EXT_SWAP_CONTROL                = GLXEW_SGI_swap_control ? true : false;
#elif defined(NUX_OS_LINUX)
    OGL_EXT_SWAP_CONTROL                = GLXEW_SGI_swap_control ? true : false;
#endif

    
    GL_ARB_VERTEX_PROGRAM               = GLEW_ARB_vertex_program ? true : false;
    GL_ARB_FRAGMENT_PROGRAM             = GLEW_ARB_fragment_program ? true : false;
    GL_ARB_SHADER_OBJECTS               = GLEW_ARB_shader_objects ? true : false;
    GL_ARB_VERTEX_SHADER                = GLEW_ARB_vertex_shader ? true : false;
    GL_ARB_FRAGMENT_SHADER              = GLEW_ARB_fragment_shader ? true : false;
    GL_ARB_VERTEX_BUFFER_OBJECT         = GLEW_ARB_vertex_buffer_object ? true : false;
    GL_ARB_TEXTURE_NON_POWER_OF_TWO     = GLEW_ARB_texture_non_power_of_two ? true : false;
    GL_EXT_FRAMEBUFFER_OBJECT           = GLEW_EXT_framebuffer_object ? true : false;
    GL_EXT_DRAW_RANGE_ELEMENTS          = GLEW_EXT_draw_range_elements ? true : false;
    GL_EXT_STENCIL_TWO_SIDE             = GLEW_EXT_stencil_two_side ? true : false;
    GL_EXT_TEXTURE_RECTANGLE            = GLEW_EXT_texture_rectangle ? true : false;
    GL_ARB_TEXTURE_RECTANGLE            = GLEW_ARB_texture_rectangle ? true : false;
    GL_NV_TEXTURE_RECTANGLE             = GLEW_NV_texture_rectangle ? true : false;

    InitTextureFormats();

    // See Avoiding 16 Common OpenGL Pitfalls
    // 7. Watch Your Pixel Store Alignment
    // http://www.opengl.org/resources/features/KilgardTechniques/oglpitfall/
    // We use a pack /unpack alignment to 1 so we don't have any padding at the end of row.

    glPixelStorei (GL_UNPACK_ALIGNMENT, _PixelStoreAlignment);
    glPixelStorei (GL_PACK_ALIGNMENT, _PixelStoreAlignment);

    _DeviceWidth = DeviceWidth;
    _DeviceHeight = DeviceHeight;

    _ViewportX = 0;
    _ViewportY = 0;
    _ViewportWidth = DeviceWidth;
    _ViewportHeight = DeviceHeight;

    for (int i = 0; i < MAX_NUM_STREAM; i++)
    {
      _StreamSource[i].ResetStreamSource();
    }

    // Configure NVidia CG
#if (NUX_ENABLE_CG_SHADERS)
    {
      // Create Cg context and set profile.
      CHECKGL ( cgSetErrorCallback ( cgErrorCallback ) );
      m_Cgcontext = cgCreateContext();
      nuxAssert (m_Cgcontext);
      //CHECKGL( cgGLEnableProfile( CG_PROFILE_VP40 ) );
      //CHECKGL( cgGLEnableProfile( CG_PROFILE_FP40 ) );
      CHECKGL ( cgGLSetManageTextureParameters ( m_Cgcontext, CG_FALSE ) );
    }
#endif

    if (GL_EXT_FRAMEBUFFER_OBJECT)
    {
      _FrameBufferObject = CreateFrameBufferObject();
      _FrameBufferObject->Deactivate();
    }
  }

  GLDeviceFactory::~GLDeviceFactory()
  {
    NUX_SAFE_DELETE (m_RenderStates);

    _FrameBufferObject = 0;
    _CurrentFrameBufferObject = 0;
    CollectDeviceResource();

    // NVidia CG
#if (NUX_ENABLE_CG_SHADERS)
    cgDestroyContext (m_Cgcontext);
    inlSetThreadLocalStorage (ThreadLocal_GLDeviceFactory, 0);
#endif
  }

  int GLDeviceFactory::CreateTexture (
    t_u32 Width
    , t_u32 Height
    , t_u32 Levels
    //, DWORD Usage    // no use
    , BitmapFormat PixelFormat
    //, D3DPOOL Pool       // no use
    , IOpenGLTexture2D **ppTexture
    //, HANDLE* pSharedHandle       // no use
  )
  {
// From : http://oss.sgi.com/projects/ogl-sample/registry/ARB/texture_non_power_of_two.txt
//    The "floor" convention has a relatively straightforward way to
//        evaluate (with integer math) means to determine how many mipmap
//        levels are required for a complete pyramid:
//    numLevels = 1 + floor(log2(max(w, h, d)))
    t_u32 NumTotalMipLevel    = 1 + floorf (Log2 (Max (Width, Height) ) );

//    Levels
//        [in] Number of levels in the texture. If this is zero, generate all texture sublevels
//        down to 1 by 1 pixels for hardware that supports mip-maps textures. Call GetNumMipLevel to see the
//        number of levels generated.
    t_u32 NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else if (Levels > NumTotalMipLevel)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else
    {
      NumMipLevel = Levels;
    }


//    The "floor" convention can be evaluated incrementally with the
//        following recursion:
//
//    nextLODdim = max(1, currentLODdim >> 1)
//
//        where currentLODdim is the dimension of a level N and nextLODdim
//        is the dimension of level N+1.  The recursion stops when level
//        numLevels-1 is reached.

    *ppTexture = new IOpenGLTexture2D (Width, Height, NumMipLevel, PixelFormat);

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLTexture2D> > (*ppTexture, &_CachedTextureList);

    return 1;
  }

  int GLDeviceFactory::CreateRectangleTexture (
    t_u32 Width
    , t_u32 Height
    , t_u32 Levels
    //, DWORD Usage    // no use
    , BitmapFormat PixelFormat
    //, D3DPOOL Pool       // no use
    , IOpenGLRectangleTexture **ppTexture
    //, HANDLE* pSharedHandle       // no use
  )
  {

    // From : http://oss.sgi.com/projects/ogl-sample/registry/ARB/texture_non_power_of_two.txt
    //    The "floor" convention has a relatively straightforward way to
    //        evaluate (with integer math) means to determine how many mipmap
    //        levels are required for a complete pyramid:
    //    numLevels = 1 + floor(log2(max(w, h, d)))
    t_u32 NumTotalMipLevel    = 1 + floorf (Log2 (Max (Width, Height) ) );

    //    Levels
    //        [in] Number of levels in the texture. If this is zero, generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mip-maps textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    t_u32 NumMipLevel = 0;

    if (Levels == 0)
    {
      //Rectangle texture texture don't support mipmaps
      NumMipLevel = 1;
    }
    else if (Levels > NumTotalMipLevel)
    {
      NumMipLevel = 1;
    }
    else
    {
      NumMipLevel = 1;
    }


    //    The "floor" convention can be evaluated incrementally with the
    //        following recursion:
    //
    //    nextLODdim = max(1, currentLODdim >> 1)
    //
    //        where currentLODdim is the dimension of a level N and nextLODdim
    //        is the dimension of level N+1.  The recursion stops when level
    //        numLevels-1 is reached.

    *ppTexture = new IOpenGLRectangleTexture (Width, Height, NumMipLevel, PixelFormat);

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLRectangleTexture> > (*ppTexture, &_CachedTextureRectangleList);

    return 1;
  }


  int GLDeviceFactory::CreateCubeTexture (
    t_u32 EdgeLength
    , t_u32 Levels
    //, DWORD Usage    // no use
    , BitmapFormat PixelFormat
    //, D3DPOOL Pool    // no use
    , IOpenGLCubeTexture **ppCubeTexture
    //, HANDLE* pSharedHandle    // no use
  )
  {
    t_u32 NumTotalMipLevel    = 1 + floorf (Log2 (EdgeLength) );
    //    Levels
    //        [in] Number of levels in the texture. If this is zero, Direct3D will generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mipmapped textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    t_u32 NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else if (Levels > NumTotalMipLevel)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else
    {
      NumMipLevel = Levels;
    }

    *ppCubeTexture = new IOpenGLCubeTexture (EdgeLength, NumMipLevel, PixelFormat);

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLCubeTexture> > (*ppCubeTexture, &_CachedCubeTextureList);

    return 1;
  }

  int GLDeviceFactory::CreateVolumeTexture (
    t_u32 Width
    , t_u32 Height
    , t_u32 Depth
    , t_u32 Levels
    //, DWORD Usage        // no use
    , BitmapFormat PixelFormat
    //, D3DPOOL Pool       // no use
    , IOpenGLVolumeTexture **ppVolumeTexture
    //, HANDLE* pSharedHandle       // no use
  )
  {
    t_u32 NumTotalMipLevel = 1 + floorf (Log2 (Max (Max (Width, Height), Depth) ) );
    //    Levels
    //        [in] Number of levels in the texture. If this is zero, Direct3D will generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mipmapped textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    t_u32 NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else if (Levels > NumTotalMipLevel)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else
    {
      NumMipLevel = Levels;
    }

    *ppVolumeTexture = new IOpenGLVolumeTexture (Width, Height, Depth, NumMipLevel, PixelFormat);

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLVolumeTexture> > (*ppVolumeTexture, &_CachedVolumeTextureList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreateAnimatedTexture (t_u32 Width,
      t_u32 Height,
      t_u32 Depth,
      BitmapFormat PixelFormat,
      IOpenGLAnimatedTexture **ppAnimatedTexture)
  {
    *ppAnimatedTexture = new IOpenGLAnimatedTexture (Width, Height, Depth, PixelFormat);

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLAnimatedTexture> > (*ppAnimatedTexture, &_CachedAnimatedTextureList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreateQuery (QUERY_TYPE Type, IOpenGLQuery **ppQuery)
  {
    *ppQuery = new IOpenGLQuery (Type);

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLQuery> > (*ppQuery, &_CachedQueryList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreateFrameBufferObject (IOpenGLFrameBufferObject **ppFrameBufferObject)
  {
    *ppFrameBufferObject = new IOpenGLFrameBufferObject();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLFrameBufferObject> > (*ppFrameBufferObject, &_CachedFrameBufferList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreateShaderProgram (IOpenGLShaderProgram **ppShaderProgram)
  {
    *ppShaderProgram = new IOpenGLShaderProgram();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLShaderProgram> > (*ppShaderProgram, &_CachedShaderProgramList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreateVertexShader (IOpenGLVertexShader **ppVertexShader)
  {
    *ppVertexShader = new IOpenGLVertexShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLVertexShader> > (*ppVertexShader, &_CachedVertexShaderList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreatePixelShader (IOpenGLPixelShader **ppPixelShader)
  {
    *ppPixelShader = new IOpenGLPixelShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLPixelShader> > (*ppPixelShader, &_CachedPixelShaderList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreateAsmShaderProgram (IOpenGLAsmShaderProgram **ppAsmShaderProgram)
  {
    *ppAsmShaderProgram = new IOpenGLAsmShaderProgram();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLAsmShaderProgram> > (*ppAsmShaderProgram, &_CachedAsmShaderProgramList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreateAsmVertexShader (IOpenGLAsmVertexShader **ppAsmVertexShader)
  {
    *ppAsmVertexShader = new IOpenGLAsmVertexShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLAsmVertexShader> > (*ppAsmVertexShader, &_CachedAsmVertexShaderList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreateAsmPixelShader (IOpenGLAsmPixelShader **ppAsmPixelShader)
  {
    *ppAsmPixelShader = new IOpenGLAsmPixelShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<IOpenGLAsmPixelShader> > (*ppAsmPixelShader, &_CachedAsmPixelShaderList);

    return OGL_OK;
  }

#if (NUX_ENABLE_CG_SHADERS)
  int GLDeviceFactory::CreateCGVertexShader (ICgVertexShader **ppCgVertexShader)
  {
    *ppCgVertexShader = new ICgVertexShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<ICgVertexShader> > (*ppCgVertexShader, &_CachedCGVertexShaderList);

    return OGL_OK;
  }

  int GLDeviceFactory::CreateCGPixelShader (ICgPixelShader **ppCgPixelShader)
  {
    *ppCgPixelShader = new ICgPixelShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< TRefGL<ICgPixelShader> > (*ppCgPixelShader, &_CachedCGPixelShaderList);

    return OGL_OK;
  }
#endif

  void GLDeviceFactory::CollectDeviceResource()
  {
    TDeviceResourceList< TRefGL<IOpenGLVertexBuffer> >* pTempVertexBuffer = _CachedVertexBufferList;

    while (pTempVertexBuffer)
    {
      if (pTempVertexBuffer->_DeviceResource->RefCount() == 1)
      {
        pTempVertexBuffer->_DeviceResource = 0;
        pTempVertexBuffer = pTempVertexBuffer->_next;
      }
      else
        pTempVertexBuffer = pTempVertexBuffer->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLIndexBuffer> >* pTempIndexBuffer = _CachedIndexBufferList;

    while (pTempIndexBuffer)
    {
      if (pTempIndexBuffer->_DeviceResource->RefCount() == 1)
      {
        pTempIndexBuffer->_DeviceResource = 0;
        pTempIndexBuffer = pTempIndexBuffer->_next;
      }
      else
        pTempIndexBuffer = pTempIndexBuffer->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLVertexDeclaration> >* pTempVertexDeclaration = _CachedVertexDeclarationList;

    while (pTempVertexDeclaration)
    {
      if (pTempVertexDeclaration->_DeviceResource->RefCount() == 1)
      {
        pTempVertexDeclaration->_DeviceResource = 0;
        pTempVertexDeclaration = pTempVertexDeclaration->_next;
      }
      else
        pTempVertexDeclaration = pTempVertexDeclaration->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLTexture2D> >* pTempTexture2D = _CachedTextureList;

    while (pTempTexture2D)
    {
      if (pTempTexture2D->_DeviceResource->RefCount() == 1)
      {
        pTempTexture2D->_DeviceResource = 0;
        pTempTexture2D = pTempTexture2D->_next;
      }
      else
        pTempTexture2D = pTempTexture2D->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLCubeTexture> >* pTempCubeTexture = _CachedCubeTextureList;

    while (pTempCubeTexture)
    {
      if (pTempCubeTexture->_DeviceResource->RefCount() == 1)
      {
        pTempCubeTexture->_DeviceResource = 0;
        pTempCubeTexture = pTempCubeTexture->_next;
      }
      else
        pTempCubeTexture = pTempCubeTexture->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLVolumeTexture> >* pTempVolumeTexture = _CachedVolumeTextureList;

    while (pTempVolumeTexture)
    {
      if (pTempVolumeTexture->_DeviceResource->RefCount() == 1)
      {
        pTempVolumeTexture->_DeviceResource = 0;
        pTempVolumeTexture = pTempVolumeTexture->_next;
      }
      else
        pTempVolumeTexture = pTempVolumeTexture->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLAnimatedTexture> >* pTempAnimatedTexture = _CachedAnimatedTextureList;

    while (pTempAnimatedTexture)
    {
      if (pTempAnimatedTexture->_DeviceResource->RefCount() == 1)
      {
        pTempAnimatedTexture->_DeviceResource = 0;
        pTempAnimatedTexture = pTempAnimatedTexture->_next;
      }
      else
        pTempAnimatedTexture = pTempAnimatedTexture->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLFrameBufferObject> >* pTempFrameBufferObject = _CachedFrameBufferList;

    while (pTempFrameBufferObject)
    {
      if (pTempFrameBufferObject->_DeviceResource->RefCount() == 1)
      {
        pTempFrameBufferObject->_DeviceResource = 0;
        pTempFrameBufferObject = pTempFrameBufferObject->_next;
      }
      else
        pTempFrameBufferObject = pTempFrameBufferObject->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLQuery> >* pTempQuery = _CachedQueryList;

    while (pTempQuery)
    {
      if (pTempQuery->_DeviceResource->RefCount() == 1)
      {
        pTempQuery->_DeviceResource = 0;
        pTempQuery = pTempQuery->_next;
      }
      else
        pTempQuery = pTempQuery->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLVertexShader> >* pTempVertexShader = _CachedVertexShaderList;

    while (pTempVertexShader)
    {
      if (pTempVertexShader->_DeviceResource->RefCount() == 1)
      {
        pTempVertexShader->_DeviceResource = 0;
        pTempVertexShader = pTempVertexShader->_next;
      }
      else
        pTempVertexShader = pTempVertexShader->_next;
    }

    TDeviceResourceList< TRefGL<IOpenGLPixelShader> >* pTempPixelShader = _CachedPixelShaderList;

    while (pTempPixelShader)
    {
      if (pTempPixelShader->_DeviceResource->RefCount() == 1)
      {
        pTempPixelShader->_DeviceResource = 0;
        pTempPixelShader = pTempPixelShader->_next;
      }
      else
        pTempPixelShader = pTempPixelShader->_next;
    }

#if (NUX_ENABLE_CG_SHADERS)
    TDeviceResourceList< TRefGL<ICgVertexShader> >* pTempCgVertexShader = _CachedCGVertexShaderList;

    while (pTempCgVertexShader)
    {
      if (pTempCgVertexShader->_DeviceResource->RefCount() == 1)
      {
        pTempCgVertexShader->_DeviceResource = 0;
        pTempCgVertexShader = pTempCgVertexShader->_next;
      }
      else
        pTempCgVertexShader = pTempCgVertexShader->_next;
    }

    TDeviceResourceList< TRefGL<ICgPixelShader> >* pTempCgPixelShader = _CachedCGPixelShaderList;

    while (pTempCgPixelShader)
    {
      if (pTempCgPixelShader->_DeviceResource->RefCount() == 1)
      {
        pTempCgPixelShader->_DeviceResource = 0;
        pTempCgPixelShader = pTempCgPixelShader->_next;
      }
      else
        pTempCgPixelShader = pTempCgPixelShader->_next;
    }

#endif
  }

  void GLDeviceFactory::InvalidateTextureUnit (int TextureUnitIndex)
  {
    CHECKGL (glActiveTextureARB (TextureUnitIndex) );

    CHECKGL (glBindTexture (GL_TEXTURE_1D, 0) );
    CHECKGL (glBindTexture (GL_TEXTURE_2D, 0) );
    CHECKGL (glBindTexture (GL_TEXTURE_CUBE_MAP, 0) );
    CHECKGL (glBindTexture (GL_TEXTURE_3D, 0) );
    CHECKGL (glBindTexture (GL_TEXTURE_RECTANGLE_ARB, 0) );

    // From lowest priority to highest priority:
    //      GL_TEXTURE_1D,
    //      GL_TEXTURE_2D,
    //      GL_TEXTURE_RECTANGLE_ARB,
    //      GL_TEXTURE_3D,
    //      GL_TEXTURE_CUBE_MAP.

    CHECKGL (glDisable (GL_TEXTURE_1D) );
    CHECKGL (glDisable (GL_TEXTURE_2D) );
    CHECKGL (glDisable (GL_TEXTURE_RECTANGLE_ARB) );
    CHECKGL (glDisable (GL_TEXTURE_3D) );
    CHECKGL (glDisable (GL_TEXTURE_CUBE_MAP) );
  }

  int GLDeviceFactory::AllocateUnpackPixelBufferIndex (int *index)
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

  int GLDeviceFactory::FreeUnpackPixelBufferIndex (const int index)
  {
    t_s32 num = (t_s32) _PixelBufferArray.size();
    nuxAssertMsg ( (index >= 0) && (index < num), TEXT ("[GLDeviceFactory::FreeUnpackPixelBufferIndex] Trying to Free a pixel buffer index that does not exist.") );

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

  void *GLDeviceFactory::LockUnpackPixelBufferIndex (const int index, int Size)
  {
    GetThreadGLDeviceFactory()->BindUnpackPixelBufferIndex (index);
    CHECKGL ( glBufferDataARB (GL_PIXEL_UNPACK_BUFFER_ARB, Size, NULL, GL_STREAM_DRAW) );
    void *pBits = glMapBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    CHECKGL_MSG (glMapBufferARB );
    CHECKGL ( glBindBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB, 0) );
    return pBits;
  }

  void *GLDeviceFactory::LockPackPixelBufferIndex (const int index, int Size)
  {
    GetThreadGLDeviceFactory()->BindPackPixelBufferIndex (index);
    CHECKGL ( glBufferDataARB (GL_PIXEL_PACK_BUFFER_ARB, Size, NULL, GL_STREAM_DRAW) );
    void *pBits = glMapBufferARB (GL_PIXEL_PACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    CHECKGL_MSG (glMapBufferARB );
    CHECKGL ( glBindBufferARB (GL_PIXEL_PACK_BUFFER_ARB, 0) );
    return pBits;
  }

  void GLDeviceFactory::UnlockUnpackPixelBufferIndex (const int index)
  {
    GetThreadGLDeviceFactory()->BindUnpackPixelBufferIndex (index);
    CHECKGL ( glUnmapBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB) );
    CHECKGL ( glBindBufferARB (GL_PIXEL_PACK_BUFFER_ARB, 0) );
  }

  void GLDeviceFactory::UnlockPackPixelBufferIndex (const int index)
  {
    GetThreadGLDeviceFactory()->BindPackPixelBufferIndex (index);
    CHECKGL ( glUnmapBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB) );
    CHECKGL ( glBindBufferARB (GL_PIXEL_PACK_BUFFER_ARB, 0) );
  }

  int GLDeviceFactory::BindUnpackPixelBufferIndex (const int index)
  {
    t_s32 num = (t_s32) _PixelBufferArray.size();
    nuxAssertMsg ( (index >= 0) && (index < num), TEXT ("[GLDeviceFactory::BindUnpackPixelBufferIndex] Trying to bind an invalid pixel buffer index.") );

    if ( (index < 0) || (index >= num) )
    {
      return OGL_ERROR;
    }

    nuxAssertMsg (_PixelBufferArray[index].IsReserved == true, TEXT ("[GLDeviceFactory::BindUnpackPixelBufferIndex] Trying to reserved pixel buffer index.") );

    if (_PixelBufferArray[index].IsReserved == false)
    {
      return OGL_ERROR;
    }

    _PixelBufferArray[index].PBO->BindUnpackPixelBufferObject();
    return OGL_OK;
  }

  int GLDeviceFactory::BindPackPixelBufferIndex (const int index)
  {
    t_s32 num = (t_s32) _PixelBufferArray.size();
    nuxAssertMsg ( (index >= 0) && (index < num), TEXT ("[GLDeviceFactory::BindPackPixelBufferIndex] Trying to bind an invalid pixel buffer index.") );

    if ( (index < 0) || (index >= num) )
    {
      return OGL_ERROR;
    }

    nuxAssertMsg (_PixelBufferArray[index].IsReserved == true, TEXT ("[GLDeviceFactory::BindPackPixelBufferIndex] Trying to reserved pixel buffer index.") );

    if (_PixelBufferArray[index].IsReserved == false)
    {
      return OGL_ERROR;
    }

    _PixelBufferArray[index].PBO->BindPackPixelBufferObject();
    return OGL_OK;
  }

  int GLDeviceFactory::FormatFrameBufferObject (t_u32 Width, t_u32 Height, BitmapFormat PixelFormat)
  {
    if (!GL_EXT_FRAMEBUFFER_OBJECT)
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::FormatFrameBufferObject] No support for OpenGL framebuffer extension.") );
      return 0;
    }

    return _FrameBufferObject->FormatFrameBufferObject (Width, Height, PixelFormat);
  }

  int GLDeviceFactory::SetColorRenderTargetSurface (t_u32 ColorAttachmentIndex, TRefGL<IOpenGLSurface> pRenderTargetSurface)
  {
    if (!GL_EXT_FRAMEBUFFER_OBJECT)
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::SetColorRenderTargetSurface] No support for OpenGL framebuffer extension.") );
      return 0;
    }

    return _FrameBufferObject->SetRenderTarget (ColorAttachmentIndex, pRenderTargetSurface);
  }

  int GLDeviceFactory::SetDepthRenderTargetSurface (TRefGL<IOpenGLSurface> pDepthSurface)
  {
    if (!GL_EXT_FRAMEBUFFER_OBJECT)
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::SetDepthRenderTargetSurface] No support for OpenGL framebuffer extension.") );
      return 0;
    }

    return _FrameBufferObject->SetDepthSurface (pDepthSurface);
  }

  TRefGL<IOpenGLSurface> GLDeviceFactory::GetColorRenderTargetSurface (t_u32 ColorAttachmentIndex)
  {
    if (!GL_EXT_FRAMEBUFFER_OBJECT)
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::GetColorRenderTargetSurface] No support for OpenGL framebuffer extension.") );
      return 0;
    }

    return _FrameBufferObject->GetRenderTarget (ColorAttachmentIndex);
  }

  TRefGL<IOpenGLSurface> GLDeviceFactory::GetDepthRenderTargetSurface()
  {
    if (!GL_EXT_FRAMEBUFFER_OBJECT)
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::GetDepthRenderTargetSurface] No support for OpenGL framebuffer extension.") );
      return 0;
    }

    return _FrameBufferObject->GetDepthRenderTarget();
  }

  void GLDeviceFactory::ActivateFrameBuffer()
  {
    if (!GL_EXT_FRAMEBUFFER_OBJECT)
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::ActivateFrameBuffer] No support for OpenGL framebuffer extension.") );
      return;
    }

    _FrameBufferObject->Activate();
  }

  void GLDeviceFactory::DeactivateFrameBuffer()
  {
    if (!GL_EXT_FRAMEBUFFER_OBJECT)
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::DeactivateFrameBuffer] No support for OpenGL framebuffer extension.") );
      return;
    }

    CHECKGL ( glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT, 0 ) );
    CHECKGL ( glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, 0) );
  }

  void GLDeviceFactory::Clear (FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha, FLOAT depth, int stencil)
  {
    CHECKGL ( glClearColor (red, green, blue, alpha) );
    CHECKGL ( glClearDepth (depth) );
    CHECKGL ( glClearStencil (stencil) );
    CHECKGL ( glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) );
  }

  void GLDeviceFactory::ClearColorRT (FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha)
  {
    CHECKGL ( glClearColor (red, green, blue, alpha) );
    CHECKGL ( glClear (GL_COLOR_BUFFER_BIT) );
  }

  void GLDeviceFactory::ClearDepthRT (FLOAT depth)
  {
    CHECKGL ( glClearDepth (depth) );
    CHECKGL ( glClear (GL_DEPTH_BUFFER_BIT) );
  }
  void GLDeviceFactory::ClearStencilRT (int stencil)
  {
    CHECKGL ( glClearStencil (stencil) );
    CHECKGL ( glClear (GL_STENCIL_BUFFER_BIT) );
  }

  void GLDeviceFactory::ClearFloatingPointColorRT (int x, int y, int width, int height,
      FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha) // use a Quad.
  {
    DrawQuad_FixPipe (x, y, width, height, red, green, blue, alpha);
  }

  void GLDeviceFactory::ClearSurfaceWithColor (TRefGL<IOpenGLSurface> s_, const SURFACE_RECT *rect_, float r, float g, float b, float a)
  {
    if (!GL_EXT_FRAMEBUFFER_OBJECT)
    {
      nuxDebugMsg (TEXT ("[GLDeviceFactory::ClearSurfaceWithColor] No support for OpenGL framebuffer extension.") );
    }

    FormatFrameBufferObject (s_->GetWidth(), s_->GetHeight(), s_->GetPixelFormat() );
    SetColorRenderTargetSurface (0, s_);
    SetDepthRenderTargetSurface (0);
    ActivateFrameBuffer();
    ClearFloatingPointColorRT (rect_->left,
                               rect_->top,
                               rect_->right - rect_->left,
                               rect_->bottom - rect_->top,
                               r, g, b, a);
  }

  void GLDeviceFactory::SetCurrentFrameBufferObject (TRefGL<IOpenGLFrameBufferObject> fbo)
  {
    _CurrentFrameBufferObject = fbo;
  }

  TRefGL<IOpenGLFrameBufferObject> GLDeviceFactory::GetCurrentFrameBufferObject()
  {
    return _CurrentFrameBufferObject;
  }

  TRefGL<IOpenGLBaseTexture> GLDeviceFactory::CreateSystemCapableDeviceTexture (
    int Width
    , int Height
    , int Levels
    , BitmapFormat PixelFormat)
  {
    if(GetThreadGLDeviceFactory()->SUPPORT_GL_ARB_TEXTURE_NON_POWER_OF_TWO ())
    {
      return GetThreadGLDeviceFactory ()->CreateTexture (Width, Height, Levels, PixelFormat);
    }

    if(SUPPORT_GL_EXT_TEXTURE_RECTANGLE () || SUPPORT_GL_ARB_TEXTURE_RECTANGLE ())
    {
      return GetThreadGLDeviceFactory ()->CreateRectangleTexture (Width, Height, Levels, PixelFormat);
    }

    nuxAssertMsg(0, TEXT("[NuxGraphicsResources::CreateSystemCapableDeviceTexture] No support for non power of two textures or rectangle textures"));

    return TRefGL<IOpenGLBaseTexture>();
  }

  NTexture* GLDeviceFactory::CreateSystemCapableTexture ()
  {
    if(SUPPORT_GL_ARB_TEXTURE_NON_POWER_OF_TWO ())
    {
      return new NTexture2D ();
    }

    if(SUPPORT_GL_EXT_TEXTURE_RECTANGLE () || SUPPORT_GL_ARB_TEXTURE_RECTANGLE ())
    {
      return new NRectangleTexture ();
    }

    nuxAssertMsg(0, TEXT("[NuxGraphicsResources::CreateSystemCapableTexture] No support for non power of two textures or rectangle textures"));

    return 0;
  }
}
