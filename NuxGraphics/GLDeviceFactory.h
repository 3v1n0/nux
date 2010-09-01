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


#ifndef GLDEVICEFACTORY_H
#define GLDEVICEFACTORY_H

#include "GLResource.h"
#include "GLDeviceFrameBufferObject.h"
#include "GLDeviceObjects.h"
#include "GLRenderStates.h"
#include "GLTextureStates.h"
#include "GLTemplatePrimitiveBuffer.h"

NAMESPACE_BEGIN_OGL

///* 
//    We create a set of classes that wraps OpenGL objects ID.
//    The equivalent of OpenGL object ID on D3D are 
//    IDirect3DResource9
//        IDirect3DSurface9, 
//        IDirect3DIndexBuffer9,
//        IDirect3DVertexBuffer9,
//        IDirect3DVolume9 
//        - IDirect3DBaseTexture9
//            IDirect3DCubeTexture9
//            IDirect3DTexture9
//            IDirect3DVolumeTexture9 
//
//    All the D3D object support Release() and AddRef() and are managed by the DirectX Api.
//    On OpenGL, we must create an object interface that support Release() and AddRef() and we define a class
//    that create opengl object and maintain a reference count of them. The creator class calls the following functions:
//        glGenQueriesARB         (occlusion query objects)
//        glGenTextures           (texture objects)
//        glGenFramebuffersEXT    (frame buffer objects)
//        glGenRenderbuffersEXT   (render buffer objects)
//        glGenBuffersARB         (index and vertex buffers)
//
//    At creation, and opengl object has a reference count equl to 1. When the object reference count reaches 0,
//    the creator classes delete it using the appropriate semantic calls:
//        glDeleteQueries
//        glDeleteTextures
//        glDeleteFramebuffersEXT
//        glDeleteRenderbuffersEXT
//        glDeleteBuffersARB
//
//    Note that all these opengl object may not have have an equivalent in OpenGL ES.
//
//*/

//
// class GLDeviceFactory;
// class IOpenGLResource;
// class IOpenGLBaseTexture;
// class IOpenGLTexture2D;
// class IOpenGLCubeTexture;
// class IOpenGLVolumeTexture;
// class IOpenGLSurface;
// class IOpenGLVolume;
// class IOpenGLQuery;
// class IOpenGLIndexBuffer;
// class IOpenGLVertexBuffer;
// class IOpenGLVertexDeclaration;
// class GraphicsContext;
template<typename T> class TRefGL;


template<class T>
struct TDeviceResourceList
{
    T _DeviceResource;
    TDeviceResourceList* _next;
    TDeviceResourceList* _previous;

    TDeviceResourceList(T DeviceResource)
    {
        Reset();
        _DeviceResource = DeviceResource;
    }
    void Reset()
    {
        //_DeviceResource = 0;
        _next = 0;
        _previous = 0;
    }
};

template<class T>
void ManageDeviceResource(T DeviceResource, TDeviceResourceList<T>** DeviceResourceList)
{
    if((*DeviceResourceList) == 0)
    {
        (*DeviceResourceList) = new TDeviceResourceList<T>(DeviceResource);
        (*DeviceResourceList)->_previous = 0;
        (*DeviceResourceList)->_next = 0;
    }
    else
    {
        TDeviceResourceList<T>* pTemp = new TDeviceResourceList<T>(DeviceResource);
        pTemp->_next = (*DeviceResourceList);
        pTemp->_previous = 0;
        (*DeviceResourceList)->_previous = pTemp;
        (*DeviceResourceList) = pTemp;

    }
}

template<class T>
bool FindDeviceResource(T DeviceResource, TDeviceResourceList<T>** DeviceResourceList)
{
    TDeviceResourceList<T>* pTemp = *DeviceResourceList;
    if(pTemp == NULL)
        return false;
    while(pTemp)
    {
        if(pTemp->_DeviceResource == DeviceResource)
        {
            return true;
        }
        pTemp = pTemp->_next;
    }
    return false;
}

template<class T>
unsigned int DeviceResourceRefCount(T DeviceResource, TDeviceResourceList<T>** DeviceResourceList)
{
    if(!DeviceResource || ((*DeviceResourceList) == 0))
        return 0;

    TDeviceResourceList<T>* pTemp = *DeviceResourceList;
    if(pTemp == NULL)
        return 0;
    while(pTemp->_DeviceResource)
    {
        if(pTemp->_DeviceResource == DeviceResource)
        {
            return DeviceResource->RefCount();
        }
        pTemp = pTemp->_next;
    }
    return 0;
}

template<class T>
unsigned int RemoveDeviceResource(T DeviceResource, TDeviceResourceList<T>** DeviceResourceList)
{
    bool CanRemove = false;
    TDeviceResourceList<T>* pTemp = *DeviceResourceList;
    if(pTemp == NULL)
        return 0;

    while(pTemp)
    {
        if(pTemp->_DeviceResource == DeviceResource)
        {
            CanRemove = true;
            break;
        }
        pTemp = pTemp->_next;
    }
    
    if(CanRemove)
    {
        TDeviceResourceList<T>* pMarkedForDelete = pTemp;
        if(pMarkedForDelete->_previous)
        {
            pTemp = pMarkedForDelete->_previous;
            pTemp->_next = pMarkedForDelete->_next;
            if(pMarkedForDelete->_next)
                pMarkedForDelete->_next->_previous = pTemp;
        }
        else if(pMarkedForDelete->_next)
        {
            // removing the first element
            pTemp = pMarkedForDelete->_next;
            pTemp->_previous = 0;
            (*DeviceResourceList) = pTemp;
        }
        else
        {
            // removing the only element in the list
            (*DeviceResourceList) = 0;
        }

        delete pMarkedForDelete;
        return 1;
    }
    return 0;
}

struct STREAMSOURCE
{
    WORD Stream;
    TRefGL<IOpenGLVertexBuffer> VertexBuffer;
    t_u16 StreamOffset;
    t_u16 StreamStride;

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
        VertexBuffer = 0;
        StreamStride = 0;
    }
};

class GLDeviceFactory
{
public:
    static void Initialize();
private:
    static STREAMSOURCE _StreamSource[MAX_NUM_STREAM];

    int CreateTexture(
        unsigned int Width
        , unsigned int Height
        , unsigned int Levels
        //, DWORD Usage    // no use
        , BitmapFormat PixelFormat
        //, D3DPOOL Pool       // no use
        , IOpenGLTexture2D** ppTexture
        //, HANDLE* pSharedHandle       // no use
        );

    int CreateRectangleTexture(
        unsigned int Width
        , unsigned int Height
        , unsigned int Levels
        //, DWORD Usage    // no use
        , BitmapFormat PixelFormat
        //, D3DPOOL Pool       // no use
        , IOpenGLRectangleTexture** ppTexture
        //, HANDLE* pSharedHandle       // no use
        );

    int CreateCubeTexture(
        unsigned int EdgeLength
        , unsigned int Levels
        //, DWORD Usage    // no use
        , BitmapFormat PixelFormat
        //, D3DPOOL Pool    // no use
        , IOpenGLCubeTexture ** ppCubeTexture
        //, HANDLE* pSharedHandle    // no use
        );

    int CreateVolumeTexture(
        unsigned int Width
        , unsigned int Height
        , unsigned int Depth
        , unsigned int Levels
        //, DWORD Usage        // no use
        , BitmapFormat PixelFormat
        //, D3DPOOL Pool       // no use
        , IOpenGLVolumeTexture** ppVolumeTexture
        //, HANDLE* pSharedHandle       // no use
        );

    int CreateAnimatedTexture(
        unsigned int Width
        , unsigned int Height
        , unsigned int Depth
        , BitmapFormat PixelFormat
        , IOpenGLAnimatedTexture** ppAnimatedTexture
        );

    int CreateVertexBuffer(
        unsigned int Length
        , VBO_USAGE Usage    // Dynamic or WriteOnly
        //, DWORD FVF             // Not Used
        //, D3DPOOL Pool    // no use
        , IOpenGLVertexBuffer** ppVertexBuffer
        //, HANDLE* pSharedHandle    // no use
        );

    int CreateIndexBuffer(
        unsigned int Length
        , VBO_USAGE Usage    // Dynamic or WriteOnly
        , INDEX_FORMAT Format
        //, D3DPOOL Pool    // no use
        , IOpenGLIndexBuffer** ppIndexBuffer
        //, HANDLE* pSharedHandle    // no use
        );

    int CreatePixelBufferObject(int Size, VBO_USAGE Usage,    // Dynamic or WriteOnly
        IOpenGLPixelBufferObject** ppPixelBufferObject
        );

    int CreateQuery(
        QUERY_TYPE Type,
        IOpenGLQuery** ppQuery);

    int CreateVertexDeclaration(
        const VERTEXELEMENT* pVertexElements,
        IOpenGLVertexDeclaration** ppDecl);

    int CreateFrameBufferObject(
        IOpenGLFrameBufferObject** ppFrameBufferObject);

    int CreateShaderProgram(
        IOpenGLShaderProgram** ppShaderProgram);

    int CreateVertexShader(
        IOpenGLVertexShader** ppVertexShader);

    int CreatePixelShader(
        IOpenGLPixelShader** ppPixelShader);

    int CreateAsmShaderProgram(
        IOpenGLAsmShaderProgram** ppAsmShaderProgram);

    int CreateAsmVertexShader(
        IOpenGLAsmVertexShader** ppAsmVertexShader);

    int CreateAsmPixelShader(
        IOpenGLAsmPixelShader** ppAsmPixelShader);

    #if (NUX_ENABLE_CG_SHADERS)
    int CreateCGVertexShader(
        ICgVertexShader** ppCgVertexShader);

    int CreateCGPixelShader(
        ICgPixelShader** ppCgPixelShader);
    #endif

public:
/////////////////////////////////////////////////////
    TRefGL<IOpenGLTexture2D> CreateTexture(
        int Width
        , int Height
        , int Levels
        , BitmapFormat PixelFormat);

    TRefGL<IOpenGLRectangleTexture> CreateRectangleTexture(
        int Width
        , int Height
        , int Levels
        , BitmapFormat PixelFormat);

    TRefGL<IOpenGLCubeTexture> CreateCubeTexture(
        int EdgeLength
        , int Levels
        , BitmapFormat PixelFormat);

    TRefGL<IOpenGLVolumeTexture> CreateVolumeTexture(
        int Width
        , int Height
        , int Depth
        , int Levels
        , BitmapFormat PixelFormat);

    TRefGL<IOpenGLAnimatedTexture> CreateAnimatedTexture(
        int Width
        , int Height
        , int Depth
        , BitmapFormat PixelFormat);

    TRefGL<IOpenGLVertexBuffer> CreateVertexBuffer(
        int Length
        , VBO_USAGE Usage);

    TRefGL<IOpenGLIndexBuffer> CreateIndexBuffer(
        int Length
        , VBO_USAGE Usage    // Dynamic or WriteOnly
        , INDEX_FORMAT Format);

    TRefGL<IOpenGLPixelBufferObject> CreatePixelBufferObject(int Size, VBO_USAGE Usage);

    TRefGL<IOpenGLQuery> CreateQuery(
        QUERY_TYPE Type);

    TRefGL<IOpenGLVertexDeclaration> CreateVertexDeclaration(
        const VERTEXELEMENT* pVertexElements);

    TRefGL<IOpenGLFrameBufferObject> CreateFrameBufferObject();

    TRefGL<IOpenGLShaderProgram> CreateShaderProgram();
    TRefGL<IOpenGLVertexShader> CreateVertexShader();
    TRefGL<IOpenGLPixelShader> CreatePixelShader();
    TRefGL<IOpenGLAsmShaderProgram> CreateAsmShaderProgram();
    TRefGL<IOpenGLAsmVertexShader> CreateAsmVertexShader();
    TRefGL<IOpenGLAsmPixelShader> CreateAsmPixelShader();

#if (NUX_ENABLE_CG_SHADERS)
    TRefGL<ICgVertexShader> CreateCGVertexShader();
    TRefGL<ICgPixelShader> CreateCGPixelShader();
#endif



    static TRefGL<IOpenGLTexture2D>    pDefaultRenderTargetTexture;
    static TRefGL<IOpenGLSurface>      pDefaultRenderTargetSurface;
    
    // This is for the fixed pipeline
    // When using shaders see how a shader sampler parameter links to a texture with a call to setTexture.
    int SetTexture(unsigned int TextureUnit, IOpenGLBaseTexture* texture);


    void DrawQuad_FixPipe(int x, int y, int width, int height,
        FLOAT R, FLOAT G, FLOAT B, FLOAT A);

    void DrawTextureQuad_FixPipe(TRefGL<IOpenGLTexture2D> texture, int x, int y, int width, int height,
        FLOAT u0, FLOAT v0, FLOAT u1, FLOAT v1);

    void DrawTextureQuad_FixPipe(TRefGL<IOpenGLSurface> surface, int x, int y, int width, int height,
        FLOAT u0, FLOAT v0, FLOAT u1, FLOAT v1);

    int DrawIndexedPrimitive(
        TRefGL<IOpenGLIndexBuffer> IndexBuffer,
        TRefGL<IOpenGLVertexDeclaration> VertexDeclaration,
        PRIMITIVE_TYPE PrimitiveType,
        int BaseVertexIndex,
        int MinIndex,
        int NumVertices,
        int StartIndex,
        int PrimitiveCount
        );

    // Draw Primitive without index buffer
    int DrawPrimitive(
        TRefGL<IOpenGLVertexDeclaration> VertexDeclaration,
        PRIMITIVE_TYPE pt_,
        unsigned vtx_start_,
        unsigned num_prims_);

    // Draw Primitive without index buffer, and use a user pointer for the source of the stream.
    int DrawPrimitiveUP(
        TRefGL<IOpenGLVertexDeclaration> VertexDeclaration,
        PRIMITIVE_TYPE PrimitiveType,
        unsigned int PrimitiveCount,
        const void* pVertexStreamZeroData,
        unsigned int VertexStreamZeroStride
        );

    int SetStreamSource(
        unsigned int StreamNumber,
        TRefGL<IOpenGLVertexBuffer> pStreamData,
        unsigned int OffsetInBytes,
        unsigned int Stride,
        unsigned int NumComponent = 0,
        unsigned int ComponentFormat = 0);

    //! Setup a NULL vertex buffer
    void InvalidateVertexBuffer();
    //! Setup a NULL index buffer
    void InvalidateIndexBuffer();
    //! Setup a NULL texture
    void InvalidateTextureUnit(int TextureUnitIndex);

    unsigned int GetPixelStoreAlignment()
    {
        return _PixelStoreAlignment;
    }
    
    int AllocateUnpackPixelBufferIndex(int* index);
    int FreeUnpackPixelBufferIndex(const int index);
    int BindUnpackPixelBufferIndex(const int index);
    int BindPackPixelBufferIndex(const int index);
    void* LockUnpackPixelBufferIndex(const int index, const int Size);
    void* LockPackPixelBufferIndex(const int index, const int Size);

    void UnlockUnpackPixelBufferIndex(const int index);
    void UnlockPackPixelBufferIndex(const int index);

    // Operations on the current render target.
    void Clear(FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha, FLOAT depth, int stencil);
    void ClearColorRT(FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha);
    void ClearDepthRT(FLOAT depth);
    void ClearStencilRT(int stencil);
    void ClearFloatingPointColorRT(int x, int y, int width, int height,
        FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha); // use a Quad.
    void ClearSurfaceWithColor(TRefGL<IOpenGLSurface> s_, const SURFACE_RECT *rect_, float r, float g, float b, float a);

    // All these operations are done on the default frame buffer object: _FrameBufferObject.
    int FormatFrameBufferObject(unsigned int Width, unsigned int Height, BitmapFormat PixelFormat);
    int SetColorRenderTargetSurface(unsigned int ColorAttachmentIndex, TRefGL<IOpenGLSurface> pRenderTargetSurface);
    int SetDepthRenderTargetSurface(TRefGL<IOpenGLSurface> pDepthSurface);
    TRefGL<IOpenGLSurface> GetColorRenderTargetSurface(unsigned int ColorAttachmentIndex);
    TRefGL<IOpenGLSurface> GetDepthRenderTargetSurface();
    // Activate and Deactivate the default framebuffer: _FrameBufferObject.
    void ActivateFrameBuffer();
    void DeactivateFrameBuffer();

public:
    void SetCurrentFrameBufferObject(TRefGL<IOpenGLFrameBufferObject> fbo); // {_CurrentFrameBufferObject = fbo;}
    TRefGL<IOpenGLFrameBufferObject> GetCurrentFrameBufferObject(); // {return _CurrentFrameBufferObject;}

    void CollectDeviceResource();

private:
    // Default FrameBufferobject
    TRefGL<IOpenGLFrameBufferObject> _FrameBufferObject;
    TRefGL<IOpenGLFrameBufferObject> _CurrentFrameBufferObject;

    struct PixelBufferObject
    {
        TRefGL<IOpenGLPixelBufferObject> PBO;
        bool   IsReserved;
    };

    unsigned int _PixelStoreAlignment;

    std::vector<PixelBufferObject> _PixelBufferArray;

    TDeviceResourceList< TRefGL<IOpenGLPixelBufferObject> >   *_CachedPixelBufferObjectList;
    TDeviceResourceList< TRefGL<IOpenGLVertexBuffer> >        *_CachedVertexBufferList;
    TDeviceResourceList< TRefGL<IOpenGLIndexBuffer> >         *_CachedIndexBufferList;
    TDeviceResourceList< TRefGL<IOpenGLTexture2D> >           *_CachedTextureList;
    TDeviceResourceList< TRefGL<IOpenGLRectangleTexture> >    *_CachedTextureRectangleList;
    TDeviceResourceList< TRefGL<IOpenGLCubeTexture> >         *_CachedCubeTextureList;
    TDeviceResourceList< TRefGL<IOpenGLVolumeTexture> >       *_CachedVolumeTextureList;
    TDeviceResourceList< TRefGL<IOpenGLAnimatedTexture> >     *_CachedAnimatedTextureList;
    TDeviceResourceList< TRefGL<IOpenGLQuery> >               *_CachedQueryList;
    TDeviceResourceList< TRefGL<IOpenGLVertexDeclaration> >   *_CachedVertexDeclarationList;
    TDeviceResourceList< TRefGL<IOpenGLFrameBufferObject> >   *_CachedFrameBufferList;
    TDeviceResourceList< TRefGL<IOpenGLVertexShader> >        *_CachedVertexShaderList;
    TDeviceResourceList< TRefGL<IOpenGLPixelShader> >         *_CachedPixelShaderList;
    TDeviceResourceList< TRefGL<IOpenGLShaderProgram> >       *_CachedShaderProgramList;
    TDeviceResourceList< TRefGL<IOpenGLAsmVertexShader> >     *_CachedAsmVertexShaderList;
    TDeviceResourceList< TRefGL<IOpenGLAsmPixelShader> >      *_CachedAsmPixelShaderList;
    TDeviceResourceList< TRefGL<IOpenGLAsmShaderProgram> >    *_CachedAsmShaderProgramList;

#if (NUX_ENABLE_CG_SHADERS)
    TDeviceResourceList< TRefGL<ICgVertexShader> >            *_CachedCGVertexShaderList;
    TDeviceResourceList< TRefGL<ICgPixelShader> >             *_CachedCGPixelShaderList;
#endif

private:
    int _DeviceWidth;
    int _DeviceHeight;
    int _ViewportX;
    int _ViewportY;
    int _ViewportWidth;
    int _ViewportHeight;

    // OPENGL capabilities
public:
    bool SUPPORT_OPENGL_VERSION_1_1() const {return OPENGL_VERSION_1_1;}
    bool SUPPORT_OPENGL_VERSION_1_2() const {return OPENGL_VERSION_1_2;}
    bool SUPPORT_OPENGL_VERSION_1_3() const {return OPENGL_VERSION_1_3;}
    bool SUPPORT_OPENGL_VERSION_1_4() const {return OPENGL_VERSION_1_4;}
    bool SUPPORT_OPENGL_VERSION_1_5() const {return OPENGL_VERSION_1_5;}
    bool SUPPORT_OPENGL_VERSION_2_0() const {return OPENGL_VERSION_2_0;}
    bool SUPPORT_OPENGL_VERSION_2_1() const {return OPENGL_VERSION_2_1;}
    bool SUPPORT_GLSL_VERSION_1_0() const {return GLSL_VERSION_1_0;}
    bool SUPPORT_GLSL_VERSION_1_1() const {return GLSL_VERSION_1_1;}

    bool SUPPORT_WGL_EXT_SWAP_CONTROL()        const {return OGL_EXT_SWAP_CONTROL;}
    bool SUPPORT_GL_ARB_TEXTURE_RECTANGLE()    const {return GL_ARB_TEXTURE_RECTANGLE;}
    bool SUPPORT_GL_ARB_VERTEX_PROGRAM()       const {return GL_ARB_VERTEX_PROGRAM;}
    bool SUPPORT_GL_ARB_FRAGMENT_PROGRAM()     const {return GL_ARB_FRAGMENT_PROGRAM;}
    bool SUPPORT_GL_ARB_SHADER_OBJECTS()       const {return GL_ARB_SHADER_OBJECTS;}
    bool SUPPORT_GL_ARB_VERTEX_SHADER()        const {return GL_ARB_VERTEX_SHADER;}
    bool SUPPORT_GL_ARB_FRAGMENT_SHADER()      const {return GL_ARB_FRAGMENT_SHADER;}
    bool SUPPORT_GL_ARB_VERTEX_BUFFER_OBJECT() const {return GL_ARB_VERTEX_BUFFER_OBJECT;}
    bool SUPPORT_GL_ARB_TEXTURE_NON_POWER_OF_TWO() const {return GL_ARB_TEXTURE_NON_POWER_OF_TWO;}
    bool SUPPORT_GL_EXT_FRAMEBUFFER_OBJECT()   const {return GL_EXT_FRAMEBUFFER_OBJECT;}
    bool SUPPORT_GL_EXT_DRAW_RANGE_ELEMENTS()  const {return GL_EXT_DRAW_RANGE_ELEMENTS;}
    bool SUPPORT_GL_EXT_STENCIL_TWO_SIDE()     const {return GL_EXT_STENCIL_TWO_SIDE;}
    bool SUPPORT_GL_EXT_TEXTURE_RECTANGLE()    const {return GL_EXT_TEXTURE_RECTANGLE;}
    bool SUPPORT_NV_TEXTURE_RECTANGLE()        const {return GL_NV_TEXTURE_RECTANGLE;}

    int GetOpenGLMaxTextureUnits() const {return OPENGL_MAX_TEXTURE_UNITS;}
    int GetOpenGLMaxTextureCoords() const {return OPENGL_MAX_TEXTURE_COORDS;}
    int GetOpenGLMaxTextureImageUnits() const {return OPENGL_MAX_TEXTURE_IMAGE_UNITS;}
    int GetOpenGLMaxFrameBufferAttachment() const {return OPENGL_MAX_FB_ATTACHMENT;}
    int GetOpenGLMaxVertexAttributes() const {return OPENGL_MAX_VERTEX_ATTRIBUTES;}

    // NVidia Cg
    #if (NUX_ENABLE_CG_SHADERS)
    CGcontext GetCgContext() {return m_Cgcontext;}
    CGcontext m_Cgcontext;
    #endif

    inline bool isATIBoard() const {return m_isATIBoard;}
    inline bool isNVIDIABoard() const {return m_isNVIDIABoard;}
    inline bool UsePixelBufferObjects() const {return m_UsePixelBufferObject;}
    eGraphicsBoardVendor GetGraphicsBoardVendor() const {return m_GraphicsBoardVendor;}

    GLRenderStates& GetRenderStates() {return *m_RenderStates;}
    void ResetRenderStates() {m_RenderStates->ResetStateChangeToDefault();}
    void VerifyRenderStates() {m_RenderStates->CheckStateChange();}

private:
    NString m_BoardVendorString;
    NString m_BoardRendererString;
    NString m_OpenGLVersionString;
    NString m_GLSLVersionString;

    bool m_isINTELBoard;
    bool m_isATIBoard;
    bool m_isNVIDIABoard;
    bool m_UsePixelBufferObject;
    eGraphicsBoardVendor m_GraphicsBoardVendor;

    bool OPENGL_VERSION_1_1;
    bool OPENGL_VERSION_1_2;
    bool OPENGL_VERSION_1_3;
    bool OPENGL_VERSION_1_4;
    bool OPENGL_VERSION_1_5;
    bool OPENGL_VERSION_2_0;
    bool OPENGL_VERSION_2_1;

    int OPENGL_MAX_TEXTURE_UNITS;
    int OPENGL_MAX_TEXTURE_COORDS;
    int OPENGL_MAX_TEXTURE_IMAGE_UNITS;
    int OPENGL_MAX_FB_ATTACHMENT;
    int OPENGL_MAX_VERTEX_ATTRIBUTES;

    bool GLSL_VERSION_1_0;
    bool GLSL_VERSION_1_1;

    bool OGL_EXT_SWAP_CONTROL;
    bool GL_ARB_TEXTURE_RECTANGLE ;
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
    bool GL_NV_TEXTURE_RECTANGLE;

    GLRenderStates* m_RenderStates;

public:
    GLDeviceFactory(unsigned int DeviceWidth, unsigned int DeviceHeight, BitmapFormat DeviceFormat);
    ~GLDeviceFactory();

    friend class IOpenGLSurface;
    friend class GraphicsContext;
};

NAMESPACE_END_OGL

#endif // GLDEVICEFACTORY_H

