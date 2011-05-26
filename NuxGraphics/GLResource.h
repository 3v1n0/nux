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


#ifndef GLRESOURCE_H
#define GLRESOURCE_H

#include "NuxCore/NuxCore.h"
#include "NuxCore/SystemTypes.h"
#include "NuxCore/Error.h"
#include "NuxCore/FilePath.h"
#include "NuxCore/Color.h"
#include "NuxCore/Rect.h"
#include "NuxCore/Point.h"
#include "NuxCore/Size.h"
#include "NuxImage/BitmapFormats.h"
#include "NuxCore/Parsing.h"
#include "NuxCore/Object.h"
#include "NuxCore/SmartPtr/GenericSmartPointer.h"
#include "NuxCore/ObjectPtr.h"

#include "NuxCore/Math/MathUtility.h"
#include "NuxCore/Math/Constants.h"
#include "NuxCore/Math/Vector3.h"
#include "NuxCore/Math/Vector4.h"
#include "NuxCore/Math/Matrix3.h"
#include "NuxCore/Math/Matrix4.h"
#include "NuxCore/Math/Spline.h"
#include "NuxCore/Math/MathFunctions.h"

#include "NuxImage/ImageSurface.h"

namespace nux
{
  class IOpenGLResource;

  class BaseTexture;
  class Texture2D;
  class TextureRectangle;
  class TextureCube;
  class TextureVolume;
  class TextureFrameAnimation;

  class NVertexBuffer;
  class NIndexBuffer;

  class CachedTexture2D;
  class CachedTextureRectangle;
  class CachedTextureCube;
  class CachedTextureVolume;
  class CachedTextureFrameAnimation;
  class FontTexture;

}

#define NUX_ENABLE_CG_SHADERS 0

//#define NUX_OPENGLES_20

#if defined(NUX_OS_WINDOWS)
  #include "GL/glew.h"
  #include "GL/wglew.h"

  GLEWContext *glewGetContext();
  WGLEWContext *wglewGetContext();

  #if (NUX_ENABLE_CG_SHADERS)
  #include "CG/cg.h"
  #include "CG/cgGL.h"
  #pragma comment( lib, "cg.lib" )
  #pragma comment( lib, "cgGL.lib"  )
  #endif

#elif defined(NUX_OS_LINUX)

  #ifdef NUX_OPENGLES_20
    #define GLEW_MX
    #include "EGL/egl.h"
    #include "GLES2/gl2.h"
  #else
    #define GLEW_MX
    #include "GL/glew.h"
    #include "GL/glxew.h"
    
    GLEWContext *glewGetContext();
    GLXEWContext *glxewGetContext();

    #if (NUX_ENABLE_CG_SHADERS)
      #include "Cg/cg.h"
      #include "Cg/cgGL.h"
    #endif
  #endif

#endif

#include "RunTimeStats.h"
#include "NuxGraphicsResources.h"
#include "FontTexture.h"
#include "GlobalGraphicsInitializer.h"

#include "GLError.h"

#define CG_FRAGMENT_PROFILE CG_PROFILE_FP30
#define CG_VERTEX_PROFILE CG_PROFILE_VP30

#define NUX_BUFFER_OFFSET(i) ((BYTE *)NULL + (i))


namespace nux
{

  enum
  {
    OGL_OK = 0,
    OGL_ERROR,
    OGL_ERROR_UNKNOWN,
    OGL_INVALID_SURFACE_LEVEL,
    OGL_INVALID_CALL,
    OGL_INVALID_LOCK,
    OGL_INVALID_UNLOCK,
    OGL_INVALID_TEXTURE,
    OGL_FORCE_DWORD            = 0x7fffffff /* force 32-bit size enum */
  };

  extern const TCHAR *OGLDeviceErrorMessages[];

#define OGL_OK 0
#define OGL_CALL(call)              \
    {                               \
        int Result = call;          \
        if(Result != OGL_OK)          \
        {nuxError(TEXT("OGL Object Error: Error # %d - (%s) "), Result,  *OGLDeviceErrorMessages[Result] );}   \
    }

//if(Result!=OGL_OK) {nuxError(TEXT("OGL Object Error: Error # %d - %s"), Result, OGLDeviceErrorMessages[Result]);}

  enum TEXTURE_FORMAT
  {
    TEXTURE_FMT_UNKNOWN              = 0,
    TEXTURE_FMT_ALPHA                = GL_ALPHA,
    TEXTURE_FMT_ALPHA8               = GL_ALPHA8,
    TEXTURE_FMT_ALPHA16              = GL_ALPHA16,

    TEXTURE_FMT_LUMINANCE            = GL_LUMINANCE,
    TEXTURE_FMT_LUMINANCE8           = GL_LUMINANCE8,
    TEXTURE_FMT_LUMINANCE16          = GL_LUMINANCE16,

    TEXTURE_FMT_LUMINANCE_ALPHA      = GL_LUMINANCE_ALPHA,
    TEXTURE_FMT_LUMINANCE8_ALPHA8    = GL_LUMINANCE8_ALPHA8,
    TEXTURE_FMT_LUMINANCE16_ALPHA16  = GL_LUMINANCE16_ALPHA16,

    TEXTURE_FMT_INTENSITY            = GL_INTENSITY,
    TEXTURE_FMT_INTENSITY8           = GL_INTENSITY8,
    TEXTURE_FMT_INTENSITY16          = GL_INTENSITY16,

    TEXTURE_FMT_GL_DEPTH_COMPONENT   = GL_DEPTH_COMPONENT,
    TEXTURE_FMT_GL_DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24,

    TEXTURE_FMT_RGBA                 = GL_RGBA,
    TEXTURE_FMT_RGBA8                = GL_RGBA8,
    TEXTURE_FMT_RGBA16               = GL_RGBA16,

    TEXTURE_FMT_RGBA16F_ARB          = GL_RGBA16F_ARB,
    TEXTURE_FMT_RGBA32F_ARB          = GL_RGBA32F_ARB,
    TEXTURE_FMT_RGB                  = GL_RGB,
    TEXTURE_FMT_RGB8                 = GL_RGB8,
    TEXTURE_FMT_RGB16                = GL_RGB16,
    TEXTURE_FMT_RGB16F_ARB           = GL_RGB16F_ARB,
    TEXTURE_FMT_RGB32F_ARB           = GL_RGB32F_ARB,

    TEXTURE_FMT_COMPRESSED_RGB_S3TC_DXT1_EXT   = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
    TEXTURE_FMT_COMPRESSED_RGBA_S3TC_DXT1_EXT  = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    TEXTURE_FMT_COMPRESSED_RGBA_S3TC_DXT3_EXT  = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    TEXTURE_FMT_COMPRESSED_RGBA_S3TC_DXT5_EXT  = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
    TEXTURE_FMT_FORCE_DWORD                    = 0x7fffffff /* force 32-bit size enum */
  };

  struct PixelFormatReadInfo
  {
    const TCHAR	*Name;
    GLenum          Format;     // format use for glReadPixels
    GLenum          type;       // type use for glReadPixels
    // Format specific internal flags, e.g. whether SRGB is supported with this format
    DWORD			Flags;
    bool			Supported;
  };

  enum eCUBEMAP_FACES
  {
    CUBEMAP_FACE_POSITIVE_X     = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    CUBEMAP_FACE_NEGATIVE_X     = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    CUBEMAP_FACE_POSITIVE_Y     = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    CUBEMAP_FACE_NEGATIVE_Y     = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    CUBEMAP_FACE_POSITIVE_Z     = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    CUBEMAP_FACE_NEGATIVE_Z     = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,

    CUBEMAP_FACE_FORCE_DWORD    = 0x7fffffff
  };

  enum VBO_USAGE
  {
    VBO_USAGE_UNKNOWN   = 0,
    VBO_USAGE_STATIC    = GL_STATIC_DRAW,
    VBO_USAGE_DYNAMIC   = GL_DYNAMIC_DRAW,
    VBO_USAGE_STREAM    = GL_STREAM_DRAW,
    VBO_USAGE_FORCE_DWORD    = 0x7fffffff /* force 32-bit size enum */
  };

  enum INDEX_FORMAT
  {
    INDEX_FORMAT_UNKNOWN   = 0,
    INDEX_FORMAT_USHORT    = GL_UNSIGNED_SHORT,
    INDEX_FORMAT_UINT      = GL_UNSIGNED_INT,
    INDEX_FORMAT_FORCE_DWORD    = 0x7fffffff /* force 32-bit size enum */
  };

// Primitives supported by draw-primitive API
  typedef enum
  {
    PRIMITIVE_TYPE_POINTLIST             = GL_POINTS,
    PRIMITIVE_TYPE_LINELIST              = GL_LINES,
    PRIMITIVE_TYPE_LINESTRIP             = GL_LINE_STRIP,
    PRIMITIVE_TYPE_TRIANGLELIST          = GL_TRIANGLES,
    PRIMITIVE_TYPE_TRIANGLESTRIP         = GL_TRIANGLE_STRIP,
    PRIMITIVE_TYPE_TRIANGLEFAN           = GL_TRIANGLE_FAN,
    PRIMITIVE_TYPE_QUADLIST              = GL_QUADS,
    PRIMITIVE_TYPE_QUADSTRIP             = GL_QUAD_STRIP,
    PRIMITIVE_TYPE_FORCE_DWORD           = 0x7fffffff /* force 32-bit size enum */
  } PRIMITIVE_TYPE;

  typedef enum
  {
    RTINDEXBUFFER,
    RTVERTEXBUFFER,
    RTSURFACE,
    RTVOLUME,
    RTBASETEXTURE,
    RTTEXTURE,
    RTTEXTURERECTANGLE,
    RTCUBETEXTURE,
    RTVOLUMETEXTURE,
    RTANIMATEDTEXTURE,
    RTVERTEXDECLARATION,
    RTQUERY,
    RTFRAMEBUFFEROBJECT,
    RT_GLSL_VERTEXSHADER,
    RT_GLSL_PIXELSHADER,
    RT_GLSL_SHADERPROGRAM,
    RT_CG_VERTEXSHADER,
    RT_CG_PIXELSHADER,
    RT_FORCE_DWORD           = 0x7fffffff /* force 32-bit size enum */
  } OpenGLResourceType;

  /* Multi-Sample buffer types */
  typedef enum
  {
    MULTISAMPLE_TYPE_NONE            =  0,
    MULTISAMPLE_TYPE_NONMASKABLE     =  1,
    MULTISAMPLE_TYPE_2_SAMPLES       =  2,
    MULTISAMPLE_TYPE_3_SAMPLES       =  3,
    MULTISAMPLE_TYPE_4_SAMPLES       =  4,
    MULTISAMPLE_TYPE_5_SAMPLES       =  5,
    MULTISAMPLE_TYPE_6_SAMPLES       =  6,
    MULTISAMPLE_TYPE_7_SAMPLES       =  7,
    MULTISAMPLE_TYPE_8_SAMPLES       =  8,
    MULTISAMPLE_TYPE_9_SAMPLES       =  9,
    MULTISAMPLE_TYPE_10_SAMPLES      = 10,
    MULTISAMPLE_TYPE_11_SAMPLES      = 11,
    MULTISAMPLE_TYPE_12_SAMPLES      = 12,
    MULTISAMPLE_TYPE_13_SAMPLES      = 13,
    MULTISAMPLE_TYPE_14_SAMPLES      = 14,
    MULTISAMPLE_TYPE_15_SAMPLES      = 15,
    MULTISAMPLE_TYPE_16_SAMPLES      = 16,
    MULTISAMPLE_TYPE_FORCE_DWORD     = 0x7fffffff /* force 32-bit size enum */
  } MULTISAMPLE_TYPE;

  typedef enum
  {
    MEM_POOL_DEFAULT = 0,
    MEM_POOL_MANAGED = 1,
    MEM_POOL_SYSTEMMEM = 2,
    MEM_POOL_SCRATCH = 3,
    MEM_POOL_FORCE_DWORD = 0x7fffffff /* force 32-bit size enum */
  } MEM_POOL;


  typedef struct
  {
    OpenGLResourceType ResourceType;
    unsigned int Width;
    unsigned int Height;
    unsigned int Depth;
    BitmapFormat PixelFormat;
    unsigned int RowPitch;
    unsigned int SlicePitch;
    unsigned int BitsPerPixel;
    unsigned int WidthInBlocks;
    unsigned int HeightInBlocks;
    unsigned int DepthInBlocks;
    unsigned int BytesPerBlock;
    int ExpBias;
    DWORD Flags;
    MULTISAMPLE_TYPE MultiSampleType;
  } TEXTURE_DESC;

  typedef struct _SURFACE_DESC
  {
    BitmapFormat    PixelFormat;
    OpenGLResourceType     Type;
    DWORD               Usage;
    MEM_POOL             Pool;

    MULTISAMPLE_TYPE MultiSampleType;
    DWORD               MultiSampleQuality;
    unsigned int                Width;
    unsigned int                Height;
  } SURFACE_DESC;

  typedef struct _VOLUME_DESC
  {
    BitmapFormat PixelFormat;
    OpenGLResourceType Type;
    DWORD Usage;
    MEM_POOL Pool;
    unsigned int Width;
    unsigned int Height;
    unsigned int Depth;
  } VOLUME_DESC;

  typedef struct _ANIMATEDTEXTURE_DESC
  {
    BitmapFormat PixelFormat;
    OpenGLResourceType Type;
    DWORD Usage;
    MEM_POOL Pool;
    unsigned int Width;
    unsigned int Height;
    unsigned int Depth;
  } ANIMATEDTEXTURE_DESC;

  typedef struct _VERTEXBUFFER_DESC
  {
    VBO_USAGE Usage;
    unsigned int Size;
  } VERTEXBUFFER_DESC;

  typedef struct _INDEXBUFFER_DESC
  {
    INDEX_FORMAT Format;
    VBO_USAGE Usage;
    unsigned int Size;
  } INDEXBUFFER_DESC;

  typedef struct _LOCKED_RECT
  {
    int Pitch;
    void *pBits;
  } SURFACE_LOCKED_RECT;

  typedef struct _SURFACE_RECT
  {
    long left;      //Specifies the x-coordinate of the lower-left corner of the rectangle.
    long top;       //Specifies the y-coordinate of the lower-left corner of the rectangle.
    long right;     //Specifies the x-coordinate of the upper-right corner of the rectangle.
    long bottom;    //Specifies the y-coordinate of the upper-right corner of the rectangle.
  } SURFACE_RECT;


  /* Structures for LockBox */
  typedef struct _VOLUME_BOX
  {
    int                Left;
    int                Top;
    int                Right;
    int                Bottom;
    int                Front;
    int                Back;
  } VOLUME_BOX;

  typedef struct _VOLUME_LOCKED_BOX
  {
    int                 RowPitch;
    int                 SlicePitch;
    void               *pBits;
  } VOLUME_LOCKED_BOX;

  typedef enum _ATTRIB_DECL_TYPE
  {
    ATTRIB_DECLTYPE_UNKNOWN = 0,
    ATTRIB_DECLTYPE_FLOAT1,
    ATTRIB_DECLTYPE_FLOAT2,
    ATTRIB_DECLTYPE_FLOAT3,
    ATTRIB_DECLTYPE_FLOAT4,
    ATTRIB_DECLTYPE_COLOR,
    ATTRIB_DECLTYPE_UBYTE4,
    ATTRIB_DECLTYPE_SHORT2,
    ATTRIB_DECLTYPE_SHORT4,
    ATTRIB_DECLTYPE_UBYTE4N,
    ATTRIB_DECLTYPE_SHORT2N,
    ATTRIB_DECLTYPE_SHORT4N,
    ATTRIB_DECLTYPE_USHORT2N,
    ATTRIB_DECLTYPE_USHORT4N,
    ATTRIB_DECLTYPE_UDEC3,
    ATTRIB_DECLTYPE_DEC3N,
    ATTRIB_DECLTYPE_FLOAT16_2,
    ATTRIB_DECLTYPE_FLOAT16_4,
    ATTRIB_DECLTYPE_UNUSED,
    ATTRIB_DECLTYPE_FORCE_DWORD           = 0x7fffffff /* force 32-bit size enum */
  } ATTRIB_DECL_TYPE;

  typedef enum _ATTRIB_COMPONENT_TYPE
  {
    ATTRIB_CT_UNKNOWN           =   0,
    ATTRIB_CT_BYTE              =   GL_BYTE,
    ATTRIB_CT_UNSIGNED_BYTE     =   GL_UNSIGNED_BYTE,
    ATTRIB_CT_SHORT             =   GL_SHORT,
    ATTRIB_CT_UNSIGNED_SHORT    =   GL_UNSIGNED_SHORT,
    ATTRIB_CT_INT               =   GL_INT,
    ATTRIB_CT_UNSIGNED_INT      =   GL_UNSIGNED_INT,
    ATTRIB_CT_FLOAT             =   GL_FLOAT,
    ATTRIB_CT_HALF_FLOAT        =   GL_HALF_FLOAT_ARB,
//    ATTRIB_CT_2_BYTES           =   GL_2_BYTES,
//    ATTRIB_CT_3_BYTES           =   GL_3_BYTES,
//    ATTRIB_CT_4_BYTES           =   GL_4_BYTES,
    ATTRIB_CT_DOUBLE            =   GL_DOUBLE,
// Type can be GL_UNSIGNED_BYTE, GL_SHORT, GL_INT, GL_FLOAT, GL_DOUBLE
    ATTRIB_CT_FORCE_DWORD           = 0x7fffffff /* force 32-bit size enum */
  } ATTRIB_COMPONENT_TYPE;

// Binding Semantics
  typedef enum
  {
    ATTRIB_USAGE_DECL_POSITION       = 0,
    ATTRIB_USAGE_DECL_BLENDWEIGHT    = 1,
    ATTRIB_USAGE_DECL_NORMAL         = 2,
    ATTRIB_USAGE_DECL_COLOR          = 3,
    ATTRIB_USAGE_DECL_COLOR1         = 4,
    ATTRIB_USAGE_DECL_FOGCOORD       = 5,
    ATTRIB_USAGE_DECL_PSIZE          = 6,
    ATTRIB_USAGE_DECL_BLENDINDICES   = 7,
    ATTRIB_USAGE_DECL_TEXCOORD       = 8,
    ATTRIB_USAGE_DECL_TEXCOORD0      = 8,
    ATTRIB_USAGE_DECL_TEXCOORD1      = 9,
    ATTRIB_USAGE_DECL_TEXCOORD2      = 10,
    ATTRIB_USAGE_DECL_TEXCOORD3      = 11,
    ATTRIB_USAGE_DECL_TEXCOORD4      = 12,
    ATTRIB_USAGE_DECL_TEXCOORD5      = 13,
    ATTRIB_USAGE_DECL_TEXCOORD6      = 14,
    ATTRIB_USAGE_DECL_TEXCOORD7      = 15,
    ATTRIB_USAGE_DECL_TANGENT        = 14,
    ATTRIB_USAGE_DECL_BINORMAL       = 15,
    ATTRIB_USAGE_DECL_FORCE_DWORD    = 0x7fffffff /* force 32-bit size enum */
  } ATTRIB_USAGE_DECL;

  typedef enum
  {
    QUERY_TYPE_VCACHE                 = 4,
    QUERY_TYPE_RESOURCEMANAGER        = 5,
    QUERY_TYPE_VERTEXSTATS            = 6,
    QUERY_TYPE_EVENT                  = 8,
    QUERY_TYPE_OCCLUSION              = 9,
    QUERY_TYPE_SCREENEXTENT           = 10,
    QUERY_TYPE_FORCE_DWORD            = 0x7fffffff /* force 32-bit size enum */
  } QUERY_TYPE;

// Flags field for Issue
#define ISSUE_END (1 << 0) // Tells the runtime to issue the end of a query, changing it's state to "non-signaled".
#define ISSUE_BEGIN (1 << 1) // Tells the runtime to issue the begining of a query.

  struct VERTEXELEMENT
  {
    VERTEXELEMENT()
    {
      Stream = 0;
      Offset = 0;
      Type = ATTRIB_CT_UNKNOWN;
      NumComponent = 0;
      Usage = ATTRIB_USAGE_DECL_POSITION;
      UsageIndex = 0;
      Method = 0;
    }

    VERTEXELEMENT (WORD stream, WORD offset, ATTRIB_COMPONENT_TYPE type, BYTE numcomponents, ATTRIB_USAGE_DECL usage, BYTE usageindex, BYTE method = 0)
    {
      Stream = stream;
      Offset = offset;
      Type = type;
      NumComponent = numcomponents;
      Usage = usage;
      UsageIndex = usageindex;
      Method = method;
    }

    WORD Stream;
    WORD Offset;

    // Type can be GL_UNSIGNED_BYTE, GL_SHORT, GL_INT, GL_FLOAT, GL_DOUBLE ...
    ATTRIB_COMPONENT_TYPE Type;
    //DWORD Stride;

    // This can be 1, 2, 3 or 4; For a position (xyzw), it will be 4. For a texture coordinate (uv) it will be 2.
    BYTE NumComponent;

    ATTRIB_USAGE_DECL Usage;

    // Use this index in association with TEXCOORD or COLOR to identify the texture
    // coordinate set or the color set.
    BYTE UsageIndex;

    BYTE Method; // Should be always 0 for D3DDECLMETHOD_DEFAULT = 0

    // The vertex attribute Cg parameter that will be bound to the vertex element;
//    CGparameter CgParameter;
//    VERTEXELEMENT()
//    {
//        CgParameter = 0;
//    }
//    void setCgParameter(CGparameter parameter)
//    {
//        cgParameter = parameter;
//    }
  };

#define DECL_END VERTEXELEMENT(0xFF/*Stream*/,0/*Offset*/,ATTRIB_CT_UNKNOWN/*Type*/,0/*NumComponent*/,ATTRIB_USAGE_DECL_POSITION/*Usage*/,0/*UsageIndex*/, 0 /*CgParameter*/)

  unsigned int GetVertexElementSize (VERTEXELEMENT vtxelement);

#define MAXDECLLENGTH    64
#define MAX_NUM_STREAM  8

  void DecomposeTypeDeclaraction (ATTRIB_DECL_TYPE Type, BYTE &NumComponent, ATTRIB_COMPONENT_TYPE &ComponentType);

  void AddVertexElement (std::vector<VERTEXELEMENT>& Elements,
                         WORD Stream,
                         WORD Offset,
                         //ubiS16 Stride,
                         ATTRIB_DECL_TYPE Type,
                         ATTRIB_USAGE_DECL Usage,
                         BYTE UsageIndex);

  GLenum GetGLPrimitiveType (PRIMITIVE_TYPE InPrimitiveType);
  unsigned int   GetGLElementCount (PRIMITIVE_TYPE InPrimitiveType,
                                    unsigned int        InPrimitiveCount);

//   template<typename T> class ObjectPtr
//   {
//   public:
//     T	*Handle;
// 
//   private:
//     void CheckReferenceObject()
//     {
//       if (Handle)
//       {
//         if (Handle->GetValue() == 0)
//         {
//           delete Handle;
//           //GetGpuDevice()->DestroyDeviceResource<T>(Handle);
//         }
//       }
// 
//       Handle = 0;
//     }
// 
//     //     // Do not allow access to the managed pointer. This also avoid compiler confusion between
//     //     //  int BindTexture(IOpenGLBaseTexture* texture);
//     //     // and
//     //     //  int BindTexture(ObjectPtr<IOpenGLBaseTexture> texture);
//     //
//     //     // Access operators.
//     //     typedef	T*	PtrT;
//     //     operator T*()
//     //     {
//     //         return Handle;
//     //     }
// 
//   public:
//     T *operator -> ()
//     {
//       nuxAssert (Handle);
//       return Handle;
//     }
// 
//     const T *operator -> () const
//     {
//       nuxAssert (Handle);
//       return Handle;
//     }
// 
//     T &operator*()
//     {
//       nuxAssert (Handle);
//       return *Handle;
//     }
// 
//     const T &operator *() const
//     {
//       nuxAssert (Handle);
//       return *Handle;
//     }
// 
//     // Constructor/destructor.
//     ObjectPtr (T *InHandle = 0)
//       :   Handle (InHandle)
//     {
//       if (Handle)
//         Handle->Increment();
//     }
// 
//     template <typename U>
//     ObjectPtr (U *InHandle)
//       :   Handle (0)
//     {
//       if (InHandle == 0)
//         return;
// 
//       if (InHandle->Type().IsDerivedFromType (T::StaticObjectType) )
//       {
//         Handle = (T *) InHandle;
//       }
// 
//       if (Handle)
//         Handle->Increment();
//     }
// 
//     ObjectPtr (const ObjectPtr<T>& Copy)
//       :   Handle (0)
//     {
//       Handle = Copy.Handle;
// 
//       if (Handle)
//         Handle->Increment();
//     }
// 
//     template <typename U>
//     ObjectPtr (const ObjectPtr<U>& Copy)
//       :   Handle (0)
//     {
//       // Check if type U is derived from type T
//       // Type() is virtual. Even if we have a ObjectPtr<IOpenGLBaseTexture> but its internal pointer is a IOpenGLCubeTexture,
//       // Type() returns the static object type inside IOpenGLCubeTexture.
//       // This make this possible
//       //
//       //      ObjectPtr<IOpenGLBaseTexture> basetex;            // IOpenGLBaseTexture is an abstract base class for IOpenGLTexture2D
//       //      basetex = gGLDeviceFactory->CreateTexture(.....);
//       //      ObjectPtr<IOpenGLVertexBuffer> vtx(basetex);   // vtx Handle will be null
//       //      ObjectPtr<IOpenGLTexture2D> tex(basetex);      // tex Handle will be the same as basetex
//       //
// 
//       if (Copy.Handle->Type().IsDerivedFromType (T::StaticObjectType) )
//       {
//         Handle = (T *) Copy.Handle;
//       }
// 
//       if (Handle)
//         Handle->Increment();
//     }
// 
//     ~ObjectPtr()
//     {
//       if (Handle)
//       {
//         Handle->Decrement();
//         CheckReferenceObject();
//       }
//     }
// 
//     // Assignment operator.
// 
//     ObjectPtr<T>& operator = (T *InHandle)
//     {
//       if (Handle != InHandle)
//       {
//         if (Handle)
//         {
//           Handle->Decrement();
//           CheckReferenceObject();
//         }
// 
//         Handle = InHandle;
// 
//         if (Handle)
//           Handle->Increment();
//       }
// 
//       return *this;
//     }
// 
//     ObjectPtr<T>& operator = (const ObjectPtr<T>& Other)
//     {
//       // Avoid self assignment
//       if (Handle == Other.Handle)
//       {
//         return *this;
//       }
// 
//       if (Handle)
//       {
//         Handle->Decrement();
//         CheckReferenceObject();
//       }
// 
//       Handle = Other.Handle;
// 
//       if (Handle)
//         Handle->Increment();
// 
//       return *this;
//     }
// 
//     bool operator == (const ObjectPtr<T>& Other) const
//     {
//       if (Handle == Other.Handle)
//       {
//         return true;
//       }
// 
//       return false;
//     }
// 
//     bool operator == (const T *Ptr) const
//     {
//       if (Handle == Ptr)
//       {
//         return true;
//       }
// 
//       return false;
//     }
// 
//     template <typename U>
//     bool operator == (const U *Ptr) const
//     {
//       if (Handle == Ptr)
//       {
//         return true;
//       }
// 
//       return false;
//     }
// 
//     template <typename U>
//     bool operator == (ObjectPtr<U>& Other) const
//     {
//       if (Handle == Other.Handle)
//       {
//         return true;
//       }
// 
//       return false;
//     }
// 
//     bool operator != (const ObjectPtr<T>& Other) const
//     {
//       if (Handle != Other.Handle)
//       {
//         return true;
//       }
// 
//       return false;
//     }
// 
//     bool operator != (const T *Ptr) const
//     {
//       if (Handle != Ptr)
//       {
//         return true;
//       }
// 
//       return false;
//     }
// 
//     template <typename U>
//     bool operator != (const U *Ptr) const
//     {
//       if (Handle != Ptr)
//       {
//         return true;
//       }
// 
//       return false;
//     }
// 
//     template <typename U>
//     bool operator != (ObjectPtr<U>& Other) const
//     {
//       if (Handle != Other.Handle)
//       {
//         return true;
//       }
// 
//       return false;
//     }
// 
//     //    // This isn't safe. An assignment like the following is wrong, but it will no flag an error at compile time.
//     //    // ObjectPtr<IOpenGLVertexBuffer> vtx;
//     //    // ObjectPtr<IOpenGLTexture2D> tex;
//     //    // vtx = tex;         // WRONG
//     //
//     //    template<typename U>
//     //        ObjectPtr<T>& operator=(const ObjectPtr<U>& Other)
//     //    {
//     //        Handle = (T*)(Other.Handle);
//     //        return *this;
//     //    }
// 
//     // Doing vtx = tex.Castref<IOpenGLVertexBuffer> will return a ObjectPtr<IOpenGLVertexBuffer> with a null Handle.
//     template<typename U>
//     ObjectPtr<U> CastRef()
//     {
//       ObjectPtr<U> t;
// 
//       // Check if type U is derived from type T
//       if (U::StaticObjectType.IsDerivedFromType (T::StaticObjectType) )
//       {
//         t = (U *) Handle;
//       }
// 
//       return t;
//     }
// 
//     void Release()
//     {
//       if (Handle)
//       {
//         Handle->Decrement();
//         CheckReferenceObject();
//       }
// 
//       Handle = 0;
//     }
//   private:
//     T &GetHandle()
//     {
//       return *Handle;
//     }
// 
//   public:
//     bool IsValid()
//     {
//       if (Handle)
//       {
//         return true;
//       }
// 
//       return false;
//     }
// 
//     bool IsNull()
//     {
//       if (Handle)
//       {
//         return false;
//       }
// 
//       return true;
//     }
//   };

}

#endif // GLRESOURCE_H

