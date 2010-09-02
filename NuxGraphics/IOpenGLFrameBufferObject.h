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

NAMESPACE_BEGIN_OGL

class IOpenGLResource;
class IOpenGLFrameBufferObject: public IOpenGLResource
{
    DECLARE_OBJECT_TYPE(IOpenGLFrameBufferObject, IOpenGLResource);
public:
    virtual ~IOpenGLFrameBufferObject();

    int FormatFrameBufferObject(unsigned int Width, unsigned int Height, BitmapFormat PixelFormat);
    int SetRenderTarget(unsigned int ColorAttachmentIndex, TRefGL<IOpenGLSurface> pRenderTargetSurface);
    int SetDepthSurface(TRefGL<IOpenGLSurface> pDepthSurface);
    TRefGL<IOpenGLSurface> GetRenderTarget(unsigned int ColorAttachmentIndex);
    TRefGL<IOpenGLSurface> GetDepthRenderTarget();

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
    void PushClippingRegion(Rect rect);
    void PopClippingRegion();
    void EmptyClippingRegion();
    void ApplyClippingRegion();
    void SetClippingRegion(int x, int y, unsigned int width, unsigned int height);
    Rect GetClippingRegion();
    int GetWidth() const {return _Width;}
    int GetHeight() const {return _Height;}

private:
    IOpenGLFrameBufferObject(unsigned int Width = 4, unsigned int Height = 4, BitmapFormat PixelFormat = BITFMT_R8G8B8A8);

    unsigned int	            _Width;
    unsigned int	            _Height;
    BitmapFormat    _PixelFormat;
    bool             _IsActive;

    TRefGL<IOpenGLSurface>     _Depth_Attachment;
    TRefGL<IOpenGLSurface>     _Stencil_Attachment;
    std::vector< TRefGL<IOpenGLSurface> > _Color_AttachmentArray;

    GLFramebufferObject _Fbo;   // The framebuffer object used for rendering to the texture
    GLRenderbuffer      _Rbo;

    friend class GLDeviceFactory;
};

NAMESPACE_END_OGL

#endif // IOPENGLFRAMEBUFFEROBJECT_H