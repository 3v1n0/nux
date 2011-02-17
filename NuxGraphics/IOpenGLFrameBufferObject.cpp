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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "GLResource.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "IOpenGLFrameBufferObject.h"
#include "GraphicsEngine.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (IOpenGLFrameBufferObject);

  IOpenGLFrameBufferObject::IOpenGLFrameBufferObject (NUX_FILE_LINE_DECL)
    :   IOpenGLResource (RTFRAMEBUFFEROBJECT, NUX_FILE_LINE_PARAM)
  {
    _Width = 1;
    _Height = 1;
    _PixelFormat = BITFMT_R8G8B8A8;
    _IsActive = false;

    for (int i = 0; i < GetGpuDevice()->GetGpuInfo().GetMaxFboAttachment(); i++)
    {
      _Color_AttachmentArray.push_back (ObjectPtr<IOpenGLSurface> (0) );
    }

    FormatFrameBufferObject (_Width, _Height, _PixelFormat);
    GRunTimeStats.Register (this);
  }

  IOpenGLFrameBufferObject::~IOpenGLFrameBufferObject()
  {
    // IOpenGLFrameBufferObject is an abstraction. Is does not have an opengl id.
    // _Fbo has an opengl id that is destroyed when the destructor is called.
    _OpenGLID = 0;
    GRunTimeStats.UnRegister (this);
  }

  int IOpenGLFrameBufferObject::FormatFrameBufferObject (int Width, int Height, BitmapFormat PixelFormat)
  {
    Deactivate();

    for (int i = 0; i < GetGpuDevice()->GetGpuInfo().GetMaxFboAttachment(); i++)
    {
      _Color_AttachmentArray[i] = ObjectPtr<IOpenGLSurface> (0);
    }

    _Depth_Attachment = ObjectPtr<IOpenGLSurface> (0);
    _Stencil_Attachment = ObjectPtr<IOpenGLSurface> (0);

    if ( (_Width == Width) && (_Height == Height) && (_PixelFormat == PixelFormat) )
      return 1;

#ifndef NUX_OPENGLES_20
    _Rbo.Set (GL_DEPTH_COMPONENT, Width, Height);
#endif

    // Clear clipping region stack
    _Width  = Width;
    _Height = Height;
    _PixelFormat = PixelFormat;
    EmptyClippingRegion();
    
    return 1;
  }

  int IOpenGLFrameBufferObject::SetRenderTarget (int ColorAttachmentIndex, ObjectPtr<IOpenGLSurface> pRenderTargetSurface)
  {
    nuxAssert (ColorAttachmentIndex < GetGpuDevice()->GetGpuInfo().GetMaxFboAttachment());

    if (pRenderTargetSurface.IsNull() )
    {
      _Color_AttachmentArray[ColorAttachmentIndex] = ObjectPtr<IOpenGLSurface> (0);
      return 1;
    }

    if (_Color_AttachmentArray[ColorAttachmentIndex] == pRenderTargetSurface)
    {
      return 1;
    }

    if (! (_Width == pRenderTargetSurface->GetWidth() && _Height == pRenderTargetSurface->GetHeight() ) )
    {
      nuxAssertMsg (0, TEXT ("[IOpenGLFrameBufferObject::SetRenderTarget] Invalid surface size.") );
      return 0;
    }

    _Color_AttachmentArray[ColorAttachmentIndex] = pRenderTargetSurface;

    if (_IsActive)
    {
      Activate();
    }

    return 1;
  }

  int IOpenGLFrameBufferObject::SetDepthSurface (ObjectPtr<IOpenGLSurface> pDepthSurface)
  {
    //nuxAssert(pDepthSurface.IsValid());

    if (pDepthSurface.IsNull() )
    {
      _Depth_Attachment = ObjectPtr<IOpenGLSurface> (0);
      _Stencil_Attachment = ObjectPtr<IOpenGLSurface> (0);
      return 1;
    }

    if (! (_Width == pDepthSurface->GetWidth() && _Height == pDepthSurface->GetHeight() ) )
    {
      nuxAssertMsg (0, TEXT ("The depth surface size is not compatible with the frame buffer size.") );
      return 0;
    }

    if (_Depth_Attachment == pDepthSurface)
      return 1;

    // We rely on the fact that the depth texture is actually a D24_S8 texture.
    // That is, the surface for the depth and stencil attachment is the same. When we bound, the surface,
    // we explicitly bind the depth attachment and the stencil attachment with the same surface.
    _Depth_Attachment = pDepthSurface;
    _Stencil_Attachment = pDepthSurface;

    if (_IsActive)
    {
      Activate();
    }

    return 1;
  }

  ObjectPtr<IOpenGLSurface> IOpenGLFrameBufferObject::GetRenderTarget (int ColorAttachmentIndex)
  {
    nuxAssert (ColorAttachmentIndex < GetGpuDevice()->GetGpuInfo().GetMaxFboAttachment());
    return _Color_AttachmentArray[ColorAttachmentIndex];
  }

  ObjectPtr<IOpenGLSurface> IOpenGLFrameBufferObject::GetDepthRenderTarget()
  {
    return _Depth_Attachment;
  }

  int IOpenGLFrameBufferObject::Activate (bool WithClippingStack)
  {
    GLuint NumBuffers = 0;
    _Fbo.Bind();

    if (GetGpuDevice() )
      GetGpuDevice()->SetCurrentFrameBufferObject (ObjectPtr<IOpenGLFrameBufferObject> (this));

    for (int i = 0; i < GetGpuDevice()->GetGpuInfo().GetMaxFboAttachment(); i++)
    {
      if (_Color_AttachmentArray[i].IsValid() )
      {
        GLenum target   = _Color_AttachmentArray[i]->GetSurfaceTarget();
        GLenum glID     = _Color_AttachmentArray[i]->GetOpenGLID();
        GLint level     = _Color_AttachmentArray[i]->GetMipLevel();
        CHECKGL ( glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, target, glID, level) );

#ifndef NUX_OPENGLES_20
        CHECKGL ( glDrawBuffer (GL_COLOR_ATTACHMENT0 + i) );
#endif
        NumBuffers++;
      }
      else
      {
        CHECKGL ( glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0) );
      }
    }

//     if(_Depth_Attachment.IsValid())
//     {
//         GLenum target   = _Depth_Attachment->GetSurfaceTarget();
//         GLenum glID     = _Depth_Attachment->GetOpenGLID();
//         GLint level     = _Depth_Attachment->GetMipLevel();
//         CHECKGL( glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, target, glID, level) );
//     }
//     else
//     {
//         CHECKGL( glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0) );
//         // On the PC you need to bing the same D24S8 surface to the depth and the stencil attachment.
//     }

#ifndef NUX_OPENGLES_20
    _Rbo.Set (GL_DEPTH_COMPONENT, _Width, _Height);
    CHECKGL ( glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT,
                                            GL_DEPTH_ATTACHMENT_EXT,
                                            GL_RENDERBUFFER_EXT,
                                            _Rbo.GetId() ) );
#endif

    nuxAssert ( _Fbo.IsValid() == true );

    if (GetThreadGraphicsContext() )
      GetThreadGraphicsContext()->SetViewport (0, 0, _Width, _Height);

    if (WithClippingStack)
      ApplyClippingRegion();

    _IsActive = true;
    return 1;
  }

// Restore the original opengl back buffer as defined when creating the opengl context(color + depth + stencil).
  int IOpenGLFrameBufferObject::Deactivate()
  {
    CHECKGL ( glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT, 0 ) );

#ifndef NUX_OPENGLES_20
    CHECKGL ( glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, 0) );
#endif

    if (GetGpuDevice() )
      GetGpuDevice()->SetCurrentFrameBufferObject (ObjectPtr<IOpenGLFrameBufferObject> (0));

    if (GetThreadGraphicsContext() )
      GetThreadGraphicsContext()->SetScissor (0, 0, _Width, _Height);

    _IsActive = false;
    return 1;
  }

  void IOpenGLFrameBufferObject::PushClippingRegion (Rect clip_rect)
  {
    clip_rect.OffsetPosition (0, 0);

    Rect current_clip_rect;
    unsigned int stacksize = (unsigned int) _ClippingRegionStack.size();

    if (stacksize == 0)
    {
      current_clip_rect = Rect (0, 0, _Width, _Height);
    }
    else
    {
      current_clip_rect = _ClippingRegionStack[stacksize-1];
    }

    Rect rect;

    if (GetThreadGraphicsContext() )
      rect = GetThreadGraphicsContext()->GetViewportRect();

    clip_rect.OffsetPosition (rect.x, _Height - (rect.y + rect.GetHeight() ) );

    Rect Intersection = current_clip_rect.Intersect (clip_rect);

    if (!Intersection.IsNull() )
    {
      _ClippingRegionStack.push_back (Intersection);

      SetClippingRegion (Intersection.x + GetThreadGraphicsContext()->GetContextX(),
                         _Height - Intersection.y - Intersection.GetHeight() - GetThreadGraphicsContext()->GetContextY(),
                         Intersection.GetWidth(), Intersection.GetHeight() );
    }
    else
    {
      _ClippingRegionStack.push_back (Rect (0, 0, 0, 0) );
      SetClippingRegion (0, 0, 0, 0);
    }
  }

  void IOpenGLFrameBufferObject::PopClippingRegion()
  {
    _ClippingRegionStack.pop_back();
    int stacksize = (int) _ClippingRegionStack.size();

    if (stacksize == 0)
    {
      SetClippingRegion (0, 0, _Width, _Height);
    }
    else
    {
      Rect current_clip_rect = _ClippingRegionStack[stacksize-1];
      SetClippingRegion (current_clip_rect.x, _Height - current_clip_rect.y - current_clip_rect.GetHeight(), current_clip_rect.GetWidth(), current_clip_rect.GetHeight() );
    }
  }

  void IOpenGLFrameBufferObject::EmptyClippingRegion()
  {
    _ClippingRegionStack.clear();
    {
      SetClippingRegion (0, 0, _Width, _Height);
    }
  }

  void IOpenGLFrameBufferObject::ApplyClippingRegion()
  {
    int stacksize = (int) _ClippingRegionStack.size();

    if (stacksize == 0)
    {
      SetClippingRegion (0, 0, _Width, _Height);
    }
    else
    {
      Rect current_clip_rect = _ClippingRegionStack[stacksize-1];
      SetClippingRegion (current_clip_rect.x, _Height - current_clip_rect.y - current_clip_rect.GetHeight(), current_clip_rect.GetWidth(), current_clip_rect.GetHeight() );
    }
  }

  void IOpenGLFrameBufferObject::SetClippingRegion (int x, int y, int width, int height)
  {
    if (GetThreadGraphicsContext() )
      GetThreadGraphicsContext()->SetScissor (x, y, width, height);
  }

  Rect IOpenGLFrameBufferObject::GetClippingRegion()
  {
    unsigned int stacksize = (unsigned int) _ClippingRegionStack.size();

    if (stacksize == 0)
    {
      return Rect (0, 0, _Width, _Height);
    }
    else
    {
      Rect r = _ClippingRegionStack [stacksize-1];
      return r;
    }
  }

  int IOpenGLFrameBufferObject::GetNumberOfClippingRegions () const
  {
    return _ClippingRegionStack.size ();
  }
}
