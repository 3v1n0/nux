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


#ifndef GLDEVICEOBJECT_H
#define GLDEVICEOBJECT_H

#include "GLResource.h"
#include "GLThread.h"
#include "GLDeviceFrameBufferObject.h"
#include "GLShaderParameter.h"
#include "GLTextureStates.h"

// #include "CG/cg.h"
// #include "CG/cgGL.h"
// #ifdef WIN32
//     #pragma comment( lib, "cg.lib" )
//     #pragma comment( lib, "cgGL.lib" )
// #endif

/*
We create a set of classes that wraps OpenGL objects ID.
The equivalent of OpenGL object ID on D3D are
IDirect3DResource9
IDirect3DSurface9,
IDirect3DIndexBuffer9,
IDirect3DVertexBuffer9,
IDirect3DVolume9
- IDirect3DBaseTexture9
IDirect3DCubeTexture9
IDirect3DTexture9
IDirect3DVolumeTexture9

All the D3D object support Release() and AddRef() and are managed by the DirectX Api.
On OpenGL, we must create an object interface that support Release() and AddRef() and we define a class
that create opengl object and maintain a reference count of them. The creator class calls the following functions:
glGenQueriesARB         (occlusion query objects)
glGenTextures           (texture objects)
glGenFramebuffersEXT    (frame buffer objects)
glGenRenderbuffersEXT   (render buffer objects)
glGenBuffersARB         (index and vertex buffers)

At creation, and opengl object has a reference count equal to 1. When the object reference count reaches 0,
the creator classes delete it using the appropriate semantic calls:
glDeleteQueries
glDeleteTextures
glDeleteFramebuffersEXT
glDeleteRenderbuffersEXT
glDeleteBuffersARB

Note that all these opengl object may not have have an equivalent in OpenGL ES.

*/


#include "IOpenGLResource.h"
#include "IOpenGLVertexBuffer.h"
#include "IOpenGLIndexBuffer.h"
#include "IOpenGLPixelBufferOject.h"

#include "IOpenGLBaseTexture.h"
#include "IOpenGLSurface.h"
#include "IOpenGLVolume.h"
#include "IOpenGLVolumeTexture.h"
#include "IOpenGLCubeTexture.h"
#include "IOpenGLRectangleTexture.h"
#include "IOpenGLTexture2D.h"
#include "IOpenGLAnimatedTexture.h"
#include "IOpenGLGLSLShader.h"
#include "IOpenGLAsmShader.h"
#include "IOpenGLQuery.h"
#include "IOpenGLVertexDeclaration.h"
#include "IOpenGLFrameBufferObject.h"

#if (NUX_ENABLE_CG_SHADERS)
#include "IOpenGLCgShader.h"
#endif

#endif // GLDEVICEOBJECT_H

