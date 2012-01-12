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


#ifndef GLDEVICEFRAMEBUFFEROBJECT_H
#define GLDEVICEFRAMEBUFFEROBJECT_H

/*!
FramebufferObject Class. This class encapsulates the FramebufferObject
(FBO) OpenGL spec. See the official spec at:
http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt

for details.

A framebuffer object(FBO) is conceptually a structure containing pointers
to GPU memory. The memory pointed to is either an OpenGL texture or an
OpenGL RenderBuffer. FBOs can be used to render to one or more textures,
share depth buffers between multiple sets of color buffers/textures and
are a complete replacement for pbuffers.

Performance Notes:
1) It is more efficient(but not required) to call Bind()
on an FBO before making multiple method calls. For example:

FramebufferObject fbo;
fbo.Bind();
fbo.AttachTexture(GL_COLOR_ATTACHMENT0_EXT, texId0);
fbo.AttachTexture(GL_COLOR_ATTACHMENT1_EXT, texId1);
fbo.IsValid();

To provide a complete encapsulation, the following usage
pattern works correctly but is less efficient:

FramebufferObject fbo;
// NOTE : No Bind() call
fbo.AttachTexture(GL_COLOR_ATTACHMENT0_EXT, texId0);
fbo.AttachTexture(GL_COLOR_ATTACHMENT1_EXT, texId1);
fbo.IsValid();

The first usage pattern binds the FBO only once, whereas
the second usage binds/unbinds the FBO for each method call.

2) Use FramebufferObject::Disable() sparingly. We have intentionally
left out an "Unbind()" method because it is largely unnecessary
and encourages rendundant Bind/Unbind coding. Binding an FBO is
usually much faster than enabling/disabling a pbuffer, but is
still a costly operation. When switching between multiple FBOs
and a visible OpenGL framebuffer, the following usage pattern
is recommended:

FramebufferObject fbo1, fbo2;
fbo1.Bind();
... Render ...
// NOTE : No Unbind/Disable here...

fbo2.Bind();
... Render ...

// Disable FBO rendering and return to visible window
// OpenGL framebuffer.
FramebufferObject::Disable();
*/

namespace nux
{

  class GLFramebufferObject
  {
  public:
    /// Ctor/Dtor
    GLFramebufferObject();
    virtual ~GLFramebufferObject();

    /// Bind this FBO as current render target
    void Bind();

    /// Bind a texture to the "attachment" point of this FBO
    virtual void AttachTexture( GLenum attachment, GLenum texType, GLuint texId,
                                 int mipLevel = 0, int zSlice = 0);

    /// Bind an array of textures to multiple "attachment" points of this FBO
    ///  - By default, the first 'numTextures' attachments are used,
    ///    starting with GL_COLOR_ATTACHMENT0_EXT
    virtual void AttachTextures( int numTextures,
                                  GLenum texTarget[],
                                  GLuint texId[],
                                  GLenum attachment[] = NULL,
                                  int mipLevel[]      = NULL,
                                  int zSlice[]        = NULL );

    /// Bind a render buffer to the "attachment" point of this FBO
    virtual void AttachRenderBuffer( GLenum attachment, GLuint buffId );

    /// Free any resource bound to the "attachment" point of this FBO
    void Unattach( GLenum attachment );

    /// Is this FBO currently a valid render target?
    ///  - Sends output to std::cerr by default but can
    ///    be a user-defined C++ stream
    ///
    /// NOTE : This function works correctly in debug build
    ///        mode but always returns "true" if NDEBUG is
    ///        is defined(optimized builds)
    bool IsValid();


    /// BEGIN : Accessors
    /// Is attached type GL_RENDERBUFFER_EXT or GL_TEXTURE?
    GLenum GetAttachedType( GLenum attachment );

    /// What is the Id of Renderbuffer/texture currently
    /// attached to "attachement?"
    GLuint GetAttachedId( GLenum attachment );

    /// Which mipmap level is currently attached to "attachement?"
    GLint  GetAttachedMipLevel( GLenum attachment );

    /// Which cube face is currently attached to "attachment?"
    GLint  GetAttachedCubeFace( GLenum attachment );

    /// Which z-slice is currently attached to "attachment?"
    GLint  GetAttachedZSlice( GLenum attachment );
    /// END : Accessors


    /// BEGIN : Static methods global to all FBOs
    /// Return number of color attachments permitted
    static int GetMaxColorAttachments();

    /// Disable all FBO rendering and return to traditional,
    /// windowing-system controlled framebuffer
    ///  NOTE:
    ///     This is NOT an "unbind" for this specific FBO, but rather
    ///     disables all FBO rendering. This call is intentionally "static"
    ///     and named "Disable" instead of "Unbind" for this reason. The
    ///     motivation for this strange semantic is performance. Providing "Unbind"
    ///     would likely lead to a large number of unnecessary FBO enablings/disabling.
    static void Disable();
    /// END : Static methods global to all FBOs

  private:
    GLint m_fboId;
    GLint  m_savedFboId;

    void  _GuardedBind();
    void  _GuardedUnbind();
    void  _FramebufferTextureND( GLenum attachment, GLenum texType,
                                  GLuint texId, int mipLevel, int zSlice );
    static GLuint _GenerateFboId();
  };

  /*!
  Renderbuffer Class. This class encapsulates the Renderbuffer OpenGL
  object described in the FramebufferObject(FBO) OpenGL spec.
  See the official spec at:
  http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
  for complete details.

  A "Renderbuffer" is a chunk of GPU memory used by FramebufferObjects to
  represent "traditional" framebuffer memory(depth, stencil, and color buffers).
  By "traditional," we mean that the memory cannot be bound as a texture.
  With respect to GPU shaders, Renderbuffer memory is "write-only." Framebuffer
  operations such as alpha blending, depth test, alpha test, stencil test, etc.
  read from this memory in post-fragement-shader(ROP) operations.

  The most common use of Renderbuffers is to create depth and stencil buffers.
  Note that as of 7/1/05, NVIDIA drivers to do not support stencil Renderbuffers.

  Usage Notes:
  1) "internalFormat" can be any of the following:
  Valid OpenGL internal formats beginning with:
  RGB, RGBA, DEPTH_COMPONENT

  or a stencil buffer format(not currently supported
  in NVIDIA drivers as of 7/1/05).
  STENCIL_INDEX1_EXT
  STENCIL_INDEX4_EXT
  STENCIL_INDEX8_EXT
  STENCIL_INDEX16_EXT

  */
  class GLRenderbuffer
  {
  public:
    /// Ctors/Dtors
    GLRenderbuffer();
    GLRenderbuffer(GLenum internalFormat, int width, int height);
    ~GLRenderbuffer();

    void   Bind();
    void   Unbind();
    void   Set(GLenum internalFormat, int width, int height);
    GLuint GetId() const;

    static GLint GetMaxSize();

  private:
    GLuint m_bufId;

    static GLuint _CreateBufferId();
  };

}

#endif // GLDEVICEFRAMEBUFFEROBJECT_H


