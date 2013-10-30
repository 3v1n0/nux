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
#include "GraphicsDisplay.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "IOpenGLFrameBufferObject.h"
#include "GraphicsEngine.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLFrameBufferObject);

  IOpenGLFrameBufferObject::IOpenGLFrameBufferObject(NUX_FILE_LINE_DECL)
  : IOpenGLResource(RTFRAMEBUFFEROBJECT, NUX_FILE_LINE_PARAM)
  , platform_support_for_depth_texture_(false)
  {
    attachment_width_ = 1;
    attachment_height_ = 1;
    _PixelFormat = BITFMT_R8G8B8A8;
    _IsActive = false;

    SetupFrameBufferObject();
    GRunTimeStats.Register(this);
  }

  void IOpenGLFrameBufferObject::SetupFrameBufferObject()
  {
    if (!GetGraphicsDisplay()->GetGpuDevice())
      return;

    auto const& gpu_info = GetGraphicsDisplay()->GetGpuDevice()->GetGpuInfo();
    texture_attachment_array_.resize(gpu_info.GetMaxFboAttachment());
    surface_attachment_array_.resize(gpu_info.GetMaxFboAttachment());
    platform_support_for_depth_texture_ = gpu_info.Support_Depth_Buffer();

    FormatFrameBufferObject(attachment_width_, attachment_height_, _PixelFormat);
  }

  IOpenGLFrameBufferObject::~IOpenGLFrameBufferObject()
  {
    // IOpenGLFrameBufferObject is an abstraction. Is does not have an opengl id.
    // _Fbo has an opengl id that is destroyed when the destructor is called.
    _OpenGLID = 0;
    GRunTimeStats.UnRegister(this);
  }

  int IOpenGLFrameBufferObject::FormatFrameBufferObject(int Width, int Height, BitmapFormat PixelFormat)
  {
    Deactivate();

    texture_attachment_array_.clear();
    surface_attachment_array_.clear();

    depth_surface_attachment_ = ObjectPtr<IOpenGLSurface> (0);
    stencil_surface_attachment_ = ObjectPtr<IOpenGLSurface> (0);

    if ((attachment_width_ == Width) && (attachment_height_ == Height) && (_PixelFormat == PixelFormat))
      return 1;

// #ifndef NUX_OPENGLES_20
//     _Rbo.Set(GL_DEPTH_COMPONENT, Width, Height);
// #endif

    nuxAssertMsg(Width>0, "[IOpenGLFrameBufferObject::FormatFrameBufferObject] Invalid surface size.");
    nuxAssertMsg(Height>0, "[IOpenGLFrameBufferObject::FormatFrameBufferObject] Invalid surface size.");
    attachment_width_  = Width;
    attachment_height_ = Height;
    _PixelFormat = PixelFormat;

    // Clear clipping region stack
    EmptyClippingRegion();
    
    return 1;
  }

  int IOpenGLFrameBufferObject::SetRenderTarget(int color_attachment_index, ObjectPtr<IOpenGLSurface> pRenderTargetSurface)
  {
    nuxAssert(color_attachment_index < GetGraphicsDisplay()->GetGpuDevice()->GetGpuInfo().GetMaxFboAttachment());

    if (pRenderTargetSurface.IsNull())
    {
      // Invalid.
      texture_attachment_array_[color_attachment_index] = ObjectPtr<IOpenGLBaseTexture> (0);
      surface_attachment_array_[color_attachment_index] = ObjectPtr<IOpenGLSurface> (0);
      return 0;
    }

    if (surface_attachment_array_[color_attachment_index] == pRenderTargetSurface)
    {
      // Already set. Return.
      return 1;
    }

    if (! (attachment_width_ == pRenderTargetSurface->GetWidth() && attachment_height_ == pRenderTargetSurface->GetHeight()))
    {
      nuxAssertMsg(0, "[IOpenGLFrameBufferObject::SetRenderTarget] Invalid surface size.");
      return 0;
    }

    texture_attachment_array_[color_attachment_index] = ObjectPtr<IOpenGLBaseTexture> (0);
    surface_attachment_array_[color_attachment_index] = pRenderTargetSurface;

    if (_IsActive)
    {
      Activate();
    }

    return 1;
  }

  int IOpenGLFrameBufferObject::SetTextureAttachment(int color_attachment_index, ObjectPtr<IOpenGLBaseTexture> texture, int mip_level)
  {
    nuxAssert(color_attachment_index < GetGraphicsDisplay()->GetGpuDevice()->GetGpuInfo().GetMaxFboAttachment());

    if (texture.IsNull())
    {
      // Invalid.
      texture_attachment_array_[color_attachment_index] = ObjectPtr<IOpenGLBaseTexture> (0);
      surface_attachment_array_[color_attachment_index] = ObjectPtr<IOpenGLSurface> (0);
      return 0;
    }

    if (!surface_attachment_array_.empty())
    {
      if ((texture_attachment_array_[color_attachment_index] == texture) &&
      (surface_attachment_array_[color_attachment_index] == texture->GetSurfaceLevel(mip_level)))
      {
        // Already set. Return.
        return 1;
      }
    }

    ObjectPtr<IOpenGLSurface> surface = texture->GetSurfaceLevel(mip_level);
    if ((attachment_width_ != surface->GetWidth()) || (attachment_height_ != surface->GetHeight()))
    {
      nuxAssertMsg(0, "[IOpenGLFrameBufferObject::SetRenderTarget] Invalid surface size.");
      return 0;
    }

    texture_attachment_array_[color_attachment_index] = texture;
    surface_attachment_array_[color_attachment_index] = texture->GetSurfaceLevel(mip_level);

    if (_IsActive)
    {
      Activate();
    }

    return 1;
  }

  ObjectPtr<IOpenGLBaseTexture> IOpenGLFrameBufferObject::TextureAttachment(int color_attachment_index)
  {
    return texture_attachment_array_[color_attachment_index];
  }

  int IOpenGLFrameBufferObject::SetDepthSurface(ObjectPtr<IOpenGLSurface> pDepthSurface)
  {
    if (!platform_support_for_depth_texture_)
    {
      return 0;
    }

    if (pDepthSurface.IsNull())
    {
      depth_surface_attachment_ = ObjectPtr<IOpenGLSurface> (0);
      stencil_surface_attachment_ = ObjectPtr<IOpenGLSurface> (0);
      return 1;
    }

    if (! (attachment_width_ == pDepthSurface->GetWidth() && attachment_height_ == pDepthSurface->GetHeight()))
    {
      nuxAssertMsg(0, "The depth surface size is not compatible with the frame buffer size.");
      return 0;
    }

    if (depth_surface_attachment_ == pDepthSurface)
      return 1;

    // We rely on the fact that the depth texture is actually a D24_S8 texture.
    // That is, the surface for the depth and stencil attachment is the same. When we bound, the surface,
    // we explicitly bind the depth attachment and the stencil attachment with the same surface.
    depth_surface_attachment_ = pDepthSurface;
    stencil_surface_attachment_ = pDepthSurface;

    if (_IsActive)
    {
      Activate();
    }

    return 1;
  }

  int IOpenGLFrameBufferObject::SetDepthTextureAttachment(ObjectPtr<IOpenGLBaseTexture> depth_texture, int mip_level)
  {
    if (!platform_support_for_depth_texture_)
    {
      return 0;
    }

    if (depth_texture.IsNull())
    {
      depth_surface_attachment_ = ObjectPtr<IOpenGLSurface> (0);
      stencil_surface_attachment_ = ObjectPtr<IOpenGLSurface> (0);
      depth_texture_attachment_ = ObjectPtr<IOpenGLBaseTexture> (0);
      return 1;
    }

    if (depth_texture_attachment_ == depth_texture &&
      depth_surface_attachment_ == depth_texture->GetSurfaceLevel(mip_level))
    {
      return 1;
    }

    ObjectPtr<IOpenGLSurface> depth_surface = depth_texture->GetSurfaceLevel(mip_level);

    if ((attachment_width_ != depth_surface->GetWidth()) || (attachment_height_ != depth_surface->GetHeight()))
    {
      nuxAssertMsg(0, "The depth surface size is not compatible with the frame buffer size.");
      return 0;
    }

    // We rely on the fact that the depth texture is actually a D24_S8 texture.
    // That is, the surface for the depth and stencil attachment is the same. When we bound, the surface,
    // we explicitly bind the depth attachment and the stencil attachment with the same surface.
    depth_surface_attachment_ = depth_surface;
    stencil_surface_attachment_ = depth_surface;
    depth_texture_attachment_ = depth_texture;

    if (_IsActive)
    {
      Activate();
    }

    return 1;
  }

  ObjectPtr<IOpenGLBaseTexture> IOpenGLFrameBufferObject::DepthTextureAttachment()
  {
    return depth_texture_attachment_;
  }

  ObjectPtr<IOpenGLSurface> IOpenGLFrameBufferObject::GetRenderTarget(int ColorAttachmentIndex)
  {
    nuxAssert(ColorAttachmentIndex < GetGraphicsDisplay()->GetGpuDevice()->GetGpuInfo().GetMaxFboAttachment());
    return surface_attachment_array_[ColorAttachmentIndex];
  }

  ObjectPtr<IOpenGLSurface> IOpenGLFrameBufferObject::GetDepthRenderTarget()
  {
    return depth_surface_attachment_;
  }

  int IOpenGLFrameBufferObject::Activate(bool WithClippingStack)
  {
    GLuint NumBuffers = 0;
    _Fbo.Bind();

    if (!GetGraphicsDisplay()->GetGpuDevice())
      return 0;

    GetGraphicsDisplay()->GetGpuDevice()->SetCurrentFrameBufferObject(ObjectPtr<IOpenGLFrameBufferObject> (this));

    for (int i = 0; i < GetGraphicsDisplay()->GetGpuDevice()->GetGpuInfo().GetMaxFboAttachment(); ++i)
    {
      if (surface_attachment_array_[i].IsValid())
      {
        GLenum target   = surface_attachment_array_[i]->GetSurfaceTarget();
        GLenum glID     = surface_attachment_array_[i]->GetOpenGLID();
        GLint level     = surface_attachment_array_[i]->GetMipLevel();
        CHECKGL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, target, glID, level));

#ifndef NUX_OPENGLES_20
        CHECKGL(glDrawBuffer(GL_COLOR_ATTACHMENT0 + i));
#endif
        NumBuffers++;
      }
      else
      {
        CHECKGL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0));
      }
    }

    if (platform_support_for_depth_texture_)
    {
      if (depth_surface_attachment_.IsValid())
      {
          GLenum target   = depth_surface_attachment_->GetSurfaceTarget();
          GLenum glID     = depth_surface_attachment_->GetOpenGLID();
          GLint level     = depth_surface_attachment_->GetMipLevel();
          CHECKGL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, target, glID, level));
      }
      else
      {
          CHECKGL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0));
          // On the PC you need to bing the same D24S8 surface to the depth and the stencil attachment.
      }
    }
    else
    {
      CHECKGL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0));
    }

    nuxAssert( _Fbo.IsValid() == true );

    if (GetGraphicsDisplay()->GetGraphicsEngine())
      GetGraphicsDisplay()->GetGraphicsEngine()->SetViewport(0, 0, attachment_width_, attachment_height_);

    if (WithClippingStack)
      ApplyClippingRegion();

    _IsActive = true;
    return 1;
  }

// Restore the original opengl back buffer as defined when creating the opengl context(color + depth + stencil).
  int IOpenGLFrameBufferObject::Deactivate()
  {
#ifdef NUX_OPENGLES_20
    GLenum binding = GL_FRAMEBUFFER;
#else
    GLenum binding = GL_DRAW_FRAMEBUFFER_EXT;
#endif

    CHECKGL(glBindFramebufferEXT( binding, 0 ));

#ifndef NUX_OPENGLES_20
    CHECKGL(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0));
#endif

    if (GetGraphicsDisplay()->GetGpuDevice())
      GetGraphicsDisplay()->GetGpuDevice()->SetCurrentFrameBufferObject(ObjectPtr<IOpenGLFrameBufferObject> (0));

    if (GetGraphicsDisplay()->GetGraphicsEngine())
      GetGraphicsDisplay()->GetGraphicsEngine()->SetScissor(0, 0, attachment_width_, attachment_height_);

    _IsActive = false;
    return 1;
  }

  void IOpenGLFrameBufferObject::PushClippingRegion(Rect const& rect)
  {
    Rect r0;
    if (GetGraphicsDisplay()->GetGraphicsEngine())
    {
      r0 = GetGraphicsDisplay()->GetGraphicsEngine()->ModelViewXFormRect(rect);
    }

    Rect current_clip_rect;
    unsigned int stacksize = (unsigned int) _ClippingRegionStack.size();

    if (stacksize == 0)
    {
      current_clip_rect.Set(0, 0, attachment_width_, attachment_height_);
    }
    else
    {
      current_clip_rect = _ClippingRegionStack[stacksize-1];
    }

    Rect r1;

    if (GetGraphicsDisplay()->GetGraphicsEngine())
      r1 = GetGraphicsDisplay()->GetGraphicsEngine()->GetViewportRect();

    r0.OffsetPosition(r1.x, attachment_height_ - (r1.y + r1.GetHeight()));

    Rect Intersection = current_clip_rect.Intersect(r0);

    if (!Intersection.IsNull())
    {
      _clipping_rect = Intersection;
      _ClippingRegionStack.push_back(Intersection);

      SetOpenGLClippingRectangle(Intersection.x + GetGraphicsDisplay()->GetGraphicsEngine()->GetViewportX(),
                         attachment_height_ - Intersection.y - Intersection.GetHeight() - GetGraphicsDisplay()->GetGraphicsEngine()->GetViewportY(),
                         Intersection.GetWidth(), Intersection.GetHeight());
    }
    else
    {
      _clipping_rect.Set(0, 0, 0, 0);
      _ClippingRegionStack.push_back(Rect(0, 0, 0, 0));
      SetOpenGLClippingRectangle(0, 0, 0, 0);
    }
  }

  void IOpenGLFrameBufferObject::PopClippingRegion()
  {
    _ClippingRegionStack.pop_back();
    int stacksize = (int) _ClippingRegionStack.size();

    if (stacksize == 0)
    {
      _clipping_rect.Set(0, 0, attachment_width_, attachment_height_);
      SetOpenGLClippingRectangle(0, 0, attachment_width_, attachment_height_);
    }
    else
    {
      _clipping_rect = _ClippingRegionStack [stacksize-1];
      Rect current_clip_rect = _ClippingRegionStack [stacksize-1];
      SetOpenGLClippingRectangle(current_clip_rect.x, attachment_height_ - current_clip_rect.y - current_clip_rect.GetHeight(), current_clip_rect.GetWidth(), current_clip_rect.GetHeight());
    }
  }

  void IOpenGLFrameBufferObject::EmptyClippingRegion()
  {
    _ClippingRegionStack.clear();
    {
      _clipping_rect.Set(0, 0, attachment_width_, attachment_height_);
      SetOpenGLClippingRectangle(0, 0, attachment_width_, attachment_height_);
    }
  }

  void IOpenGLFrameBufferObject::ApplyClippingRegion()
  {
    int stacksize = (int) _ClippingRegionStack.size();

    if (stacksize == 0)
    {
      _clipping_rect.Set(0, 0, attachment_width_, attachment_height_);
      SetOpenGLClippingRectangle(0, 0, attachment_width_, attachment_height_);
    }
    else
    {
      _clipping_rect = _ClippingRegionStack [stacksize-1];
      Rect current_clip_rect = _ClippingRegionStack [stacksize-1];
      SetOpenGLClippingRectangle(current_clip_rect.x, attachment_height_ - current_clip_rect.y - current_clip_rect.GetHeight(), current_clip_rect.GetWidth(), current_clip_rect.GetHeight());
    }
  }

  void IOpenGLFrameBufferObject::SetClippingRectangle(const Rect &rect)
  {
    if (GetGraphicsDisplay()->GetGraphicsEngine())
    {
      _clipping_rect = rect;
      GetGraphicsDisplay()->GetGraphicsEngine()->SetScissor(rect.x, attachment_height_ - rect.y - rect.height, rect.width, rect.height);
    }
  }

  void IOpenGLFrameBufferObject::SetOpenGLClippingRectangle(int x, int y, int width, int height)
  {
    if (GetGraphicsDisplay()->GetGraphicsEngine())
    {
      _clipping_rect.Set(x, y, width, height);
      GetGraphicsDisplay()->GetGraphicsEngine()->SetScissor(x, y, width, height);
    }
  }

  Rect IOpenGLFrameBufferObject::GetClippingRegion()
  {
    return _clipping_rect;

//     unsigned int stacksize = (unsigned int) _ClippingRegionStack.size();
// 
//     if (stacksize == 0)
//     {
//       return Rect(0, 0, attachment_width_, attachment_height_);
//     }
//     else
//     {
//       Rect r = _ClippingRegionStack [stacksize-1];
//       return r;
//     }
  }

  int IOpenGLFrameBufferObject::GetNumberOfClippingRegions() const
  {
    return _ClippingRegionStack.size();
  }
}

