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


#ifndef GLDEVICEOBJECT_H
#define GLDEVICEOBJECT_H

#include "GLResource.h"
#include "NuxGraphics.h"
#include "GLDeviceFrameBufferObject.h"
#include "GLShaderParameter.h"
#include "GLTextureStates.h"

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
#ifndef NUX_OPENGLES_20 
#include "IOpenGLAsmShader.h"
#endif
#include "IOpenGLQuery.h"
#include "IOpenGLVertexDeclaration.h"
#include "IOpenGLFrameBufferObject.h"

#if (NUX_ENABLE_CG_SHADERS)
#include "IOpenGLCgShader.h"
#endif

#endif // GLDEVICEOBJECT_H

