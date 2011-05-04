/*
 * Copyright 2011 Inalogic® Inc.
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
 * Authored by: Travis Watkins <travis.watkins@linaro.org>
 *
 */

// A series of defines to map GL functions to GLES functions

#ifndef OPENGLMAPPING_H
#define OPENGLMAPPING_H

#define GL_GLEXT_PROTOTYPES

#define GL_COLOR_ATTACHMENT0_EXT GL_COLOR_ATTACHMENT0
#define GL_FRAMEBUFFER_EXT GL_FRAMEBUFFER
#define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT GL_FRAMEBUFFER_UNSUPPORTED
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL

#define GL_RENDERBUFFER_EXT GL_RENDERBUFFER
#define GL_RENDERBUFFER_BINDING_EXT GL_RENDERBUFFER_BINDING
#define GL_MAX_RENDERBUFFER_SIZE_EXT GL_MAX_RENDERBUFFER_SIZE

#define GL_ARRAY_BUFFER_ARB GL_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER_ARB GL_ELEMENT_ARRAY_BUFFER

#define GL_CLAMP GL_CLAMP_TO_EDGE

#define GL_WRITE_ONLY_ARB GL_WRITE_ONLY_OES


#define glActiveTextureARB glActiveTexture

#define glGenBuffersARB glGenBuffers
#define glDeleteBuffersARB glDeleteBuffers
#define glBindBufferARB glBindBuffer
#define glBufferDataARB glBufferData
#define glMapBufferARB glMapBufferOES
#define glUnmapBufferARB glUnmapBufferOES

#define glBindBufferARB glBindBuffer
#define glEnableVertexAttribArrayARB glEnableVertexAttribArray
#define glDisableVertexAttribArrayARB glDisableVertexAttribArray
#define glVertexAttribPointerARB glVertexAttribPointer

#define glDeleteFramebuffersEXT glDeleteFramebuffers
#define glBindFramebufferEXT glBindFramebuffer
#define glGenFramebuffersEXT glGenFramebuffers
#define glFramebufferTexture2DEXT glFramebufferTexture2D
#define glCheckFramebufferStatusEXT glCheckFramebufferStatus
#define glGetFramebufferAttachmentParameterivEXT glGetFramebufferAttachmentParameteriv
#define glFramebufferRenderbufferEXT glFramebufferRenderbuffer

#define glBindRenderbufferEXT glBindRenderbuffer
#define glRenderbufferStorageEXT glRenderbufferStorage
#define glGenRenderbuffersEXT glGenRenderbuffers
#define glDeleteRenderbuffersEXT glDeleteRenderbuffers


#define glUniform1iARB glUniform1i

#define glUniform1fARB glUniform1f
#define glUniform2fARB glUniform2f
#define glUniform3fARB glUniform3f
#define glUniform4fARB glUniform4f

#define glUniform1fvARB glUniform1fv
#define glUniform2fvARB glUniform2fv
#define glUniform3fvARB glUniform3fv
#define glUniform4fvARB glUniform4fv

#define glUniformMatrix2fvARB glUniformMatrix2fv
#define glUniformMatrix3fvARB glUniformMatrix3fv
#define glUniformMatrix4fvARB glUniformMatrix4fv

#define glDepthRange glDepthRangef

#endif // OPENGLMAPPING_H

