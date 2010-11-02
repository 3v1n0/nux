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


#ifndef IOPENGLFRAMEBUFFEROBJECT_H
#define IOPENGLFRAMEBUFFEROBJECT_H

namespace nux
{

  class IOpenGLResource;
  class IOpenGLFrameBufferObject: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE (IOpenGLFrameBufferObject, IOpenGLResource);
  public:
    virtual ~IOpenGLFrameBufferObject();

    int FormatFrameBufferObject (int Width, int Height, BitmapFormat PixelFormat);
    int SetRenderTarget (int ColorAttachmentIndex, IntrusiveSP<IOpenGLSurface> pRenderTargetSurface);
    int SetDepthSurface (IntrusiveSP<IOpenGLSurface> pDepthSurface);
    IntrusiveSP<IOpenGLSurface> GetRenderTarget (int ColorAttachmentIndex);
    IntrusiveSP<IOpenGLSurface> GetDepthRenderTarget();

    int Clear (DWORD Flags, FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha, FLOAT Z, DWORD Stencil);

    int Activate (bool WithClippingStack = false);
    //### Restore the original opengl back buffer as defined when creating the opengl context(color + depth + stencil).
    int Deactivate();
    bool IsActive()
    {
      return _IsActive;
    }

    // For 2D screen operations
    std::vector<Rect> _ClippingRegionStack;
    void PushClippingRegion (Rect rect);
    void PopClippingRegion();
    void EmptyClippingRegion();
    void ApplyClippingRegion();
    void SetClippingRegion (int x, int y, int width, int height);
    Rect GetClippingRegion();
    int GetWidth() const
    {
      return _Width;
    }
    int GetHeight() const
    {
      return _Height;
    }

  private:
    IOpenGLFrameBufferObject (int Width = 4, int Height = 4, BitmapFormat PixelFormat = BITFMT_R8G8B8A8);

    int             _Width;
    int             _Height;
    BitmapFormat    _PixelFormat;
    bool            _IsActive;

    IntrusiveSP<IOpenGLSurface>     _Depth_Attachment;
    IntrusiveSP<IOpenGLSurface>     _Stencil_Attachment;
    std::vector< IntrusiveSP<IOpenGLSurface> > _Color_AttachmentArray;

    GLFramebufferObject _Fbo;   // The framebuffer object used for rendering to the texture
    GLRenderbuffer      _Rbo;

    friend class GLDeviceFactory;
  };

}

#endif // IOPENGLFRAMEBUFFEROBJECT_H
