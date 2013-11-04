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


#ifndef __PBUFFERS_H__
#define __PBUFFERS_H__

#ifndef NUX_OPENGLES_20

#if defined(WIN32)
#include "GLResource.h"

#  pragma warning (disable : 4786)
#elif defined(UNIX)
#  include <GL/glx.h>
#  include <GL/glxext.h>
#elif defined(__APPLE__)
#  include <AGL/agl.h>
#endif

#include <string>
#include <vector>

// The pixel format for the pbuffer is controlled by the mode string passed
// into the PBuffer constructor. This string can have the following attributes:
//
// r			- r pixel format (for float buffer).
// rg			- rg pixel format (for float buffer).
// rgb          - rgb pixel format. 8 bit or 16/32 bit in float buffer mode
// rgba         - same as "rgb alpha" string
// alpha        - must have alpha channel
// depth        - must have a depth buffer
// depth=n      - must have n-bit depth buffer
// stencil      - must have a stencil buffer
// double       - must support double buffered rendering
// samples=n    - must support n-sample antialiasing (n can be 2 or 4)
// float=n      - must support n-bit per channel floating point
//
// texture2D
// textureRECT
// textureCUBE  - must support binding pbuffer as texture to specified target
//              - binding the depth buffer is also supporting by specifying
//                '=depth' like so: texture2D=depth or textureRECT=depth
//              - the internal format of the texture will be rgba by default or
//                float if pbuffer is floating point
//

namespace nux
{

  class PBuffer
  {
  public:
    // see above for documentation on strMode format
    // set managed to true if you want the class to cleanup OpenGL objects in destructor
    PBuffer (const char *strMode, bool managed = false);
    ~PBuffer();

    bool Initialize (int iWidth, int iHeight, bool bShareContexts, bool bShareObjects);
    void Destroy();

    void Activate (PBuffer *current = NULL); // to switch between pbuffers, pass active pbuffer as argument
    void Deactivate();

#if defined(WIN32)
    int Bind (int iBuffer);
    int Release (int iBuffer);

    void HandleModeSwitch();
#endif

    unsigned int GetSizeInBytes();
    unsigned int CopyToBuffer (void *ptr, int w = -1, int h = -1);

    inline int GetNumComponents()
    {
      return m_iNComponents;
    }

    inline int GetBitsPerComponent()
    {
      return m_iBitsPerComponent;
    }

    inline int GetWidth()
    {
      return m_iWidth;
    }

    inline int GetHeight()
    {
      return m_iHeight;
    }

    inline bool IsSharedContext()
    {
      return m_bSharedContext;
    }

#if defined(WIN32)
    inline bool IsTexture()
    {
      return m_bIsTexture;
    }
#endif

  protected:
#if defined(NUX_OS_WINDOWS)
    HDC         m_hDC;     ///< Handle to a device context.
    HGLRC       m_hGLRC;   ///< Handle to a GL context.
    HPBUFFERARB m_hPBuffer;///< Handle to a pbuffer.

    HGLRC       m_hOldGLRC;
    HDC         m_hOldDC;

    std::vector<int> m_pfAttribList;
    std::vector<int> m_pbAttribList;

    bool m_bIsTexture;
#elif defined(NUX_OS_LINUX)
    Display    *m_pDisplay;
    GLXPbuffer  m_glxPbuffer;
    GLXContext  m_glxContext;

    Display    *m_pOldDisplay;
    GLXPbuffer  m_glxOldDrawable;
    GLXContext  m_glxOldContext;

    std::vector<int> m_pfAttribList;
    std::vector<int> m_pbAttribList;
#elif defined(NUX_OS_MACOSX)
    AGLContext  m_context;
    WindowPtr   m_window;
    std::vector<int> m_pfAttribList;
#endif

    int m_iWidth;
    int m_iHeight;
    int m_iNComponents;
    int m_iBitsPerComponent;

    const char *m_strMode;
    bool m_bSharedContext;
    bool m_bShareObjects;

  private:
    std::string getStringValue (std::string token);
    int getIntegerValue (std::string token);
#if defined(NUX_OS_WINDOWS) || defined(NUX_OS_LINUX)
    void parseModeString (const char *modeString, std::vector<int> *pfAttribList, std::vector<int> *pbAttribList);

    bool m_bIsActive;
    bool m_bManaged;
#endif
  };
}

#endif // NUX_OPENGLES_20

#endif // __PBUFFERS_H__
