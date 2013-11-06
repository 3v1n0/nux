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


#ifndef IOPENGLFRAMEBUFFEROBJECT_H
#define IOPENGLFRAMEBUFFEROBJECT_H

namespace nux
{

  class IOpenGLResource;
  class IOpenGLFrameBufferObject: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLFrameBufferObject, IOpenGLResource);
  public:
    virtual ~IOpenGLFrameBufferObject();

    void SetupFrameBufferObject();
    int FormatFrameBufferObject(int Width, int Height, BitmapFormat PixelFormat);
    int SetRenderTarget(int color_attachment_index, ObjectPtr<IOpenGLSurface> pRenderTargetSurface);
    int SetTextureAttachment(int color_attachment_index, ObjectPtr<IOpenGLBaseTexture> texture, int surface_level);
    int SetDepthSurface(ObjectPtr<IOpenGLSurface> pDepthSurface);
    int SetDepthTextureAttachment(ObjectPtr<IOpenGLBaseTexture> depth_texture, int mip_level);
    ObjectPtr<IOpenGLSurface> GetRenderTarget(int color_attachment_index);
    ObjectPtr<IOpenGLSurface> GetDepthRenderTarget();

    ObjectPtr<IOpenGLBaseTexture> TextureAttachment(int color_attachment_index);
    ObjectPtr<IOpenGLBaseTexture> DepthTextureAttachment();


    int Clear(DWORD Flags, FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha, FLOAT Z, DWORD Stencil);

    int Activate(bool WithClippingStack = false);
    //### Restore the original opengl back buffer as defined when creating the opengl context(color + depth + stencil).
    int Deactivate();
    bool IsActive()
    {
      return _IsActive;
    }

    // For 2D screen operations
    std::vector<Rect> _ClippingRegionStack;
    void PushClippingRegion(Rect const& rect);
    void PopClippingRegion();
    void EmptyClippingRegion();
    void ApplyClippingRegion();

    //! Bypass the clipping rectangle stack and set a different clipping rectangle region.
    /*!
        You may restore the clipping rectangle stack with ApplyClippingRectangle.
    */
    void SetClippingRectangle(const Rect &rect);

    void SetOpenGLClippingRectangle(int x, int y, int width, int height);
    Rect GetClippingRegion();
    int GetNumberOfClippingRegions() const;

    int GetWidth() const
    {
      return attachment_width_;
    }
    int GetHeight() const
    {
      return attachment_height_;
    }

  private:
    struct Attachment
    {
      ObjectPtr<IOpenGLBaseTexture> texture;
      int level;
    };

    IOpenGLFrameBufferObject(NUX_FILE_LINE_PROTO);

    int             attachment_width_;
    int             attachment_height_;
    BitmapFormat    _PixelFormat;
    bool            _IsActive;
    Rect            _clipping_rect;

    ObjectPtr<IOpenGLSurface>     depth_surface_attachment_;
    ObjectPtr<IOpenGLSurface>     stencil_surface_attachment_;
    ObjectPtr<IOpenGLBaseTexture> depth_texture_attachment_;
    std::vector< ObjectPtr<IOpenGLSurface> > surface_attachment_array_;
    std::vector< ObjectPtr<IOpenGLBaseTexture> > texture_attachment_array_;


    GLFramebufferObject _Fbo;   // The framebuffer object used for rendering to the texture
    GLRenderbuffer      _Rbo;
    bool platform_support_for_depth_texture_;

    friend class GpuDevice;
  };

}

#endif // IOPENGLFRAMEBUFFEROBJECT_H
