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


#include "GLDeviceObjects.h"
#include "IOpenGLRectangleTexture.h"

NAMESPACE_BEGIN_OGL

IMPLEMENT_OBJECT_TYPE(IOpenGLRectangleTexture);

IOpenGLRectangleTexture::IOpenGLRectangleTexture(
    unsigned int Width
    , unsigned int Height
    , unsigned int Levels
    , BitmapFormat PixelFormat, bool Dummy)
    : IOpenGLBaseTexture(RTTEXTURERECTANGLE, Width, Height, 1, Levels, PixelFormat)
{
    if(Dummy == false)
    {
        glGenTextures(1, &_OpenGLID);
        CHECKGL( glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _OpenGLID) );
    }

    //_SurfaceArray.Empty(Levels);
    for(unsigned int l = 0; l < Levels; l++)
    {
        IOpenGLSurface* surface = new IOpenGLSurface(this, _OpenGLID, GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_RECTANGLE_ARB, l);
        if(Dummy == false) surface->InitializeLevel();
        _SurfaceArray.push_back( surface );
    }

    SetFiltering(GL_NEAREST, GL_NEAREST);
    SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    SetRenderStates();
    GRunTimeStats.Register(this);
}

IOpenGLRectangleTexture::~IOpenGLRectangleTexture()
{
    for(int l = 0; l < _NumMipLevel; l++)
    {
        _SurfaceArray[l] = 0;
    }
    _SurfaceArray.clear();
    CHECKGL( glDeleteTextures(1, &_OpenGLID) );
    _OpenGLID = 0;
    GRunTimeStats.UnRegister(this);
}

TRefGL<IOpenGLSurface> IOpenGLRectangleTexture::GetSurfaceLevel(int Level)
{
    if(Level < _NumMipLevel)
    {
        return _SurfaceArray[Level];
    }
    else
    {
        nuxAssertMsg(0, TEXT("[IOpenGLRectangleTexture::GetSurfaceLevel] Invalid surface level"));
    }
    return TRefGL<IOpenGLSurface>(0);
}

void IOpenGLRectangleTexture::GetSurfaceLevel(int Level, TRefGL<IOpenGLSurface>& surface)
{
    surface = 0;
    surface = GetSurfaceLevel(Level);
}

int IOpenGLRectangleTexture::LockRect(
                                      int Level,
                                      SURFACE_LOCKED_RECT * pLockedRect,
                                      const SURFACE_RECT * pRect)
{
    nuxAssertMsg(pLockedRect, TEXT("[IOpenGLRectangleTexture::LockRect] Invalid parameter 'pLockedRect'."));
    nuxAssertMsg(Level >= 0, TEXT("[IOpenGLRectangleTexture::LockRect] Invalid mipmap level."));
    nuxAssertMsg(Level < _NumMipLevel, TEXT("[IOpenGLRectangleTexture::LockRect] Invalid mipmap level."));


    if(Level < _NumMipLevel)
    {
        TRefGL<IOpenGLSurface> pSurfaceLevel = _SurfaceArray[Level];
        return pSurfaceLevel->LockRect(pLockedRect, pRect);
    }
    else
    {
        pLockedRect->pBits = 0;
        pLockedRect->Pitch = 0;
        return OGL_INVALID_SURFACE_LEVEL;
    }
    return OGL_OK;
}

int IOpenGLRectangleTexture::UnlockRect(
                                        int Level
                                        )
{
    nuxAssertMsg(Level >= 0, TEXT("[IOpenGLRectangleTexture::LockRect] Invalid mipmap level."));
    nuxAssertMsg(Level < _NumMipLevel, TEXT("[IOpenGLRectangleTexture::LockRect] Invalid mipmap level."));

    if(Level < _NumMipLevel)
    {
        TRefGL<IOpenGLSurface> pSurfaceLevel = _SurfaceArray[Level];
        return pSurfaceLevel->UnlockRect();
    }
    else
    {
        return OGL_INVALID_SURFACE_LEVEL;
    }
    return OGL_OK;
}

unsigned int IOpenGLRectangleTexture::EnableGammaCorrection(bool b)
{
    nuxAssert(_OpenGLID);
    return OGL_OK;
}

NAMESPACE_END_OGL
