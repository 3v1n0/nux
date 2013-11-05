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
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GLDeviceFrameBufferObject.h"
#include "GLTemplatePrimitiveBuffer.h"
#include "GraphicsEngine.h"

namespace nux
{

  GLenum AttachmentBuffer[] =
  {
    GL_COLOR_ATTACHMENT0_EXT
#ifndef NUX_OPENGLES_20 // GLES 2.0 only supports one color attachment
    , GL_COLOR_ATTACHMENT1_EXT
    , GL_COLOR_ATTACHMENT2_EXT
    , GL_COLOR_ATTACHMENT3_EXT
#endif
  };

//////////////////////////////////////////////////////////////////////////
// GLFramebufferObject
//////////////////////////////////////////////////////////////////////////

  GLFramebufferObject::GLFramebufferObject()
    :   m_fboId(0)
    ,   m_savedFboId(0)
  {
    m_fboId = _GenerateFboId();
    // Bind this FBO so that it actually gets created now
    _GuardedBind();
    _GuardedUnbind();
  }

  GLFramebufferObject::~GLFramebufferObject()
  {
    CHECKGL(glDeleteFramebuffersEXT(1, (const GLuint *) &m_fboId));
  }

  void GLFramebufferObject::Bind()
  {
#ifdef NUX_OPENGLES_20
    GLenum binding = GL_FRAMEBUFFER;
#else
    GLenum binding = GL_DRAW_FRAMEBUFFER_EXT;
#endif
    CHECKGL(glBindFramebufferEXT(binding, m_fboId));
  }

  void GLFramebufferObject::Disable()
  {
#ifdef NUX_OPENGLES_20
    GLenum binding = GL_FRAMEBUFFER;
#else
    GLenum binding = GL_DRAW_FRAMEBUFFER_EXT;
#endif
    CHECKGL(glBindFramebufferEXT(binding, 0));
  }

  void
  GLFramebufferObject::AttachTexture( GLenum attachment, GLenum texType,
                                       GLuint texId, int mipLevel, int zSlice)
  {
    _GuardedBind();

    if ( GetAttachedId(attachment) != texId )
    {
      _FramebufferTextureND( attachment, texType,
                              texId, mipLevel, zSlice );
    }
    else
    {
//        nuxError("GLFramebufferObject::AttachTexture PERFORMANCE WARNING:\n
//            \tRedundant bind of texture(id = %d).\n"), texId);
    }

    _GuardedUnbind();
  }
  void
  GLFramebufferObject::AttachTextures( int numTextures, GLenum texTarget[], GLuint texId[],
                                        GLenum attachment[], int mipLevel[], int zSlice[] )
  {
    for (int i = 0; i < numTextures; ++i)
    {
      AttachTexture( texTarget[i], texId[i],
                      attachment ? attachment[i] : (GL_COLOR_ATTACHMENT0_EXT + i),
                      mipLevel ? mipLevel[i] : 0,
                      zSlice ? zSlice[i] : 0 );
    }
  }


  void
  GLFramebufferObject::AttachRenderBuffer( GLenum attachment, GLuint buffId )
  {
    _GuardedBind();

    if ( GetAttachedId(attachment) != buffId )
    {
      CHECKGL(glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, attachment,
                GL_RENDERBUFFER_EXT, buffId));
    }
    else
    {
//        nuxError(TEXT("GLFramebufferObject::AttachRenderBuffer PERFORMANCE WARNING:\n
//            \tRedundant bind of GLRenderbuffer(id = %d).\n"), buffId);
    }

    _GuardedUnbind();
  }

  void
  GLFramebufferObject::Unattach( GLenum attachment )
  {
    _GuardedBind();
    GLenum type = GetAttachedType(attachment);

    switch(type)
    {
      case GL_NONE:
        break;
      case GL_RENDERBUFFER_EXT:
        AttachRenderBuffer( attachment, 0 );
        break;
      case GL_TEXTURE:
        AttachTexture( attachment, GL_TEXTURE_2D, 0 );
        break;
      default:
        std::cout << "GLFramebufferObject::unbind_attachment ERROR: Unknown attached resource type\n";
    }

    _GuardedUnbind();
  }

  GLint GLFramebufferObject::GetMaxColorAttachments()
  {
#ifndef NUX_OPENGLES_20
    GLint maxAttach = 0;
    CHECKGL(glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS_EXT, &maxAttach ));
    return maxAttach;
#else
    return 1;
#endif
  }

  GLuint GLFramebufferObject::_GenerateFboId()
  {
    GLuint id = 0;
    CHECKGL(glGenFramebuffersEXT(1, &id));
    return id;
  }

  void GLFramebufferObject::_GuardedBind()
  {
#ifndef NUX_OPENGLES_20
    // Only binds if m_fboId is different than the currently bound FBO
    CHECKGL(glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING_EXT, &m_savedFboId ));

    if (m_fboId != m_savedFboId)
    {
      CHECKGL(glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, m_fboId));
    }
#else
    // Only binds if m_fboId is different than the currently bound FBO
    CHECKGL(glGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &m_savedFboId ));

    if (m_fboId != m_savedFboId)
    {
      CHECKGL(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId));
    }
#endif
  }

  void GLFramebufferObject::_GuardedUnbind()
  {
#ifdef NUX_OPENGLES_20
    GLenum binding = GL_FRAMEBUFFER;
#else
    GLenum binding = GL_DRAW_FRAMEBUFFER_EXT;
#endif
    // Returns FBO binding to the previously enabled FBO
    if (m_savedFboId != m_fboId)
    {
      CHECKGL(glBindFramebufferEXT(binding, (GLuint) m_savedFboId));
    }
  }

  void
  GLFramebufferObject::_FramebufferTextureND( GLenum attachment, GLenum texType,
      GLuint texId, int mipLevel,
      int zSlice )
  {
    if (texType == GL_TEXTURE_2D)
    {
      // Default is GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB, or cube faces
      CHECKGL(glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, attachment,
                                            texType, texId, mipLevel ));
    }
#ifndef NUX_OPENGLES_20
    else if (texType == GL_TEXTURE_1D)
    {
      CHECKGL(glFramebufferTexture1DEXT( GL_FRAMEBUFFER_EXT, attachment,
                                            GL_TEXTURE_1D, texId, mipLevel ));
    }
    else if (texType == GL_TEXTURE_3D)
    {
      CHECKGL(glFramebufferTexture3DEXT( GL_FRAMEBUFFER_EXT, attachment,
                                            GL_TEXTURE_3D, texId, mipLevel, zSlice ));
    }
#endif
  }

  bool GLFramebufferObject::IsValid()
  {
    _GuardedBind();

    bool isOK = false;

    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    CHECKGL_MSG(glCheckFramebufferStatusEXT);

    switch(status)
    {
      case GL_FRAMEBUFFER_COMPLETE_EXT: // Everything's OK
        isOK = true;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT");
        isOK = false;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
        isOK = false;
        break;
// See issue(87) of http://www.opengl.org/registry/specs/EXT/framebuffer_object.txt
//  case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
//      nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT");
//      isOK = false;
//      break;
      case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
        isOK = false;
        break;
#ifndef NUX_OPENGLES_20
      case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
        isOK = false;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT");
        isOK = false;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT");
        isOK = false;
        break;
#endif
//  case GL_FRAMEBUFFER_STATUS_ERROR_EXT:
//      nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_STATUS_ERROR_EXT");
//      isOK = false;
//      break;
      case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_UNSUPPORTED_EXT");
        isOK = false;
        break;
      default:
        nuxError("[GLFramebufferObject::IsValid] Unknown ERROR");
        isOK = false;
    }

    _GuardedUnbind();
    return isOK;
  }

/// Accessors
  GLenum GLFramebufferObject::GetAttachedType( GLenum attachment )
  {
    // Returns GL_RENDERBUFFER_EXT or GL_TEXTURE
    _GuardedBind();
    GLint type = 0;
    CHECKGL(glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
              &type));
    _GuardedUnbind();
    return GLenum(type);
  }

  GLuint GLFramebufferObject::GetAttachedId( GLenum attachment )
  {
    _GuardedBind();
    GLint id = 0;
    CHECKGL(glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
              &id));
    _GuardedUnbind();
    return GLuint(id);
  }

  GLint GLFramebufferObject::GetAttachedMipLevel( GLenum attachment )
  {
    _GuardedBind();
    GLint level = 0;
    CHECKGL(glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
              GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT,
              &level));
    _GuardedUnbind();
    return level;
  }

  GLint GLFramebufferObject::GetAttachedCubeFace( GLenum attachment )
  {
    _GuardedBind();
    GLint level = 0;
#ifndef NUX_OPENGLES_20
    CHECKGL(glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
              GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT,
              &level));
#endif
    _GuardedUnbind();
    return level;
  }

  GLint GLFramebufferObject::GetAttachedZSlice( GLenum attachment )
  {
    _GuardedBind();
    GLint slice = 0;
#ifndef NUX_OPENGLES_20
    CHECKGL(glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
              GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT,
              &slice));
#endif
    _GuardedUnbind();
    return slice;
  }

//////////////////////////////////////////////////////////////////////////
// GLRenderbuffer
//////////////////////////////////////////////////////////////////////////


  GLRenderbuffer::GLRenderbuffer()
    :   m_bufId(0)
  {
    m_bufId = _CreateBufferId();
  }

  GLRenderbuffer::GLRenderbuffer(GLenum internalFormat, int width, int height)
    : m_bufId(_CreateBufferId())
  {
    Set(internalFormat, width, height);
  }

  GLRenderbuffer::~GLRenderbuffer()
  {
    CHECKGL(glDeleteRenderbuffersEXT(1, &m_bufId));
  }

  void GLRenderbuffer::Bind()
  {
    CHECKGL(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_bufId));
  }

  void GLRenderbuffer::Unbind()
  {
    CHECKGL(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0));
  }

  void GLRenderbuffer::Set(GLenum internalFormat, int width, int height)
  {
    int maxSize = GLRenderbuffer::GetMaxSize();

    if (width > maxSize || height > maxSize )
    {
      std::cout << "GLRenderbuffer::GLRenderbuffer() ERROR: Size too big width=" << width << "height=" << height << "\n";
      return;
    }

    // Guarded bind
    GLint savedId = 0;
    CHECKGL(glGetIntegerv( GL_RENDERBUFFER_BINDING_EXT, &savedId ));

    if (savedId != (GLint) m_bufId)
    {
      Bind();
    }

    // Allocate memory for renderBuffer
    CHECKGL(glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, internalFormat, width, height ));

    // Guarded unbind
    if (savedId != (GLint) m_bufId)
    {
      Unbind();
    }
  }

  GLuint GLRenderbuffer::GetId() const
  {
    return m_bufId;
  }

  GLint GLRenderbuffer::GetMaxSize()
  {
    GLint maxAttach = 0;
    CHECKGL(glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE_EXT, &maxAttach ));
    return maxAttach;
  }

  GLuint GLRenderbuffer::_CreateBufferId()
  {
    GLuint id = 0;
    CHECKGL(glGenRenderbuffersEXT(1, &id));
    return id;
  }

}
