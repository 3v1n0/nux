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
#include "IOpenGLResource.h"
#include "IOpenGLBaseTexture.h"
#include "IOpenGLIndexBuffer.h"
#include "IOpenGLVertexBuffer.h"
#include "RunTimeStats.h"


namespace nux
{

  NUX_IMPLEMENT_GLOBAL_OBJECT (RenderingStats);

  t_u32 RenderingStats::m_NumTex2D = 0;
  t_u32 RenderingStats::m_GPUSizeTex2D = 0;
  t_u32 RenderingStats::m_NumTexRectangle = 0;
  t_u32 RenderingStats::m_GPUSizeTexRectangle = 0;
  t_u32 RenderingStats::m_NumTexCube = 0;
  t_u32 RenderingStats::m_GPUSizeTexCube = 0;
  t_u32 RenderingStats::m_NumTexVolume = 0;
  t_u32 RenderingStats::m_GPUSizeTexVolume = 0;

  t_u32 RenderingStats::m_NumTexAnimated = 0;
  t_u32 RenderingStats::m_GPUSizeTexAnimated = 0;

  t_u32 RenderingStats::m_NumPBO = 0; // Counted as Vertex Buffer
  t_u32 RenderingStats::m_GPUSizePBO = 0; // Counted as Vertex Buffer
  t_u32 RenderingStats::m_NumQueryObject = 0;
  t_u32 RenderingStats::m_NumFrameBufferObject = 0;
  t_u32 RenderingStats::m_NumVertexBuffer = 0;
  t_u32 RenderingStats::m_GPUSizeVertexBuffer = 0;
  t_u32 RenderingStats::m_NumIndexBuffer = 0;
  t_u32 RenderingStats::m_GPUSizeIndexBuffer = 0;
  t_u32 RenderingStats::m_NumVertexShader = 0;
  t_u32 RenderingStats::m_NumPixelShader = 0;
  t_u32 RenderingStats::m_NumShaderProgram = 0;

  t_u32 RenderingStats::m_TotalGPUSize = 0;

  void RenderingStats::Constructor()
  {

  }

  void RenderingStats::Destructor()
  {
    if (m_NumIndexBuffer != 0)
    {
      nuxDebugMsg (TEXT ("[RenderingStats::Destructor] Some index buffers have not been released.") );
    }

    if (m_NumTex2D != 0)
    {
      nuxDebugMsg (TEXT ("[RenderingStats::Destructor] Some 2D textures have not been released.") );
      std::vector<int>::iterator it;

      for (it = _texture_2d_array.begin(); it != _texture_2d_array.end(); it++)
      {
        nuxDebugMsg (TEXT ("[RenderingStats::Destructor] Remaining 2D textures: %d."), *it);
      }
    }

    if (m_NumTexRectangle != 0)
    {
      nuxDebugMsg (TEXT ("[RenderingStats::Destructor] Some rectangle textures buffers have not been released.") );
      std::vector<int>::iterator it;

      for (it = _texture_rect_array.begin(); it != _texture_rect_array.end(); it++)
      {
        nuxDebugMsg (TEXT ("[RenderingStats::Destructor] Remaining Rectangle textures: %d."), *it);
      }
    }
  }

  void RenderingStats::Register (IOpenGLResource *GraphicsObject)
  {
    switch (GraphicsObject->GetResourceType() )
    {
      case RTINDEXBUFFER:
      {
        m_NumIndexBuffer++;
        m_GPUSizeIndexBuffer += NUX_REINTERPRET_CAST (IOpenGLIndexBuffer *, GraphicsObject)->GetSize();
        m_TotalGPUSize += m_GPUSizeIndexBuffer;
        break;
      }
      case RTVERTEXBUFFER:
      {
        m_NumVertexBuffer++;
        m_GPUSizeVertexBuffer += NUX_REINTERPRET_CAST (IOpenGLVertexBuffer *, GraphicsObject)->GetSize();
        m_TotalGPUSize += m_GPUSizeVertexBuffer;
        break;
      }
      case RTTEXTURE:
      {
        _texture_2d_array.push_back (NUX_STATIC_CAST (IOpenGLBaseTexture *, GraphicsObject)->GetOpenGLID() );
        m_NumTex2D++;
        m_GPUSizeTex2D += GetTextureSize (NUX_REINTERPRET_CAST (IOpenGLBaseTexture *, GraphicsObject) );
        m_TotalGPUSize += m_GPUSizeTex2D;
        break;
      }
      case RTTEXTURERECTANGLE:
      {
        _texture_rect_array.push_back (NUX_STATIC_CAST (IOpenGLBaseTexture *, GraphicsObject)->GetOpenGLID() );
        m_NumTexRectangle++;
        m_GPUSizeTexRectangle += GetTextureSize (NUX_REINTERPRET_CAST (IOpenGLBaseTexture *, GraphicsObject) );
        m_TotalGPUSize += m_GPUSizeTexRectangle;
        break;
      }
      case RTCUBETEXTURE:
      {
        m_NumTexCube++;
        m_GPUSizeTexCube += GetTextureSize (NUX_STATIC_CAST (IOpenGLBaseTexture *, GraphicsObject) );
        m_TotalGPUSize += m_GPUSizeTexCube;
        break;
      }
      case RTVOLUMETEXTURE:
      {
        m_NumTexVolume++;
        m_GPUSizeTexVolume += GetTextureSize (NUX_STATIC_CAST (IOpenGLBaseTexture *, GraphicsObject) );
        m_TotalGPUSize += m_GPUSizeTexVolume;
        break;
      }
      case RTANIMATEDTEXTURE:
      {
        // Animated texture size is compted as rectangle textures;
        m_NumTexAnimated++;
        break;
      }
      case RTQUERY:
      {
        m_NumQueryObject++;
        break;
      }
      case RTFRAMEBUFFEROBJECT:
      {
        m_NumFrameBufferObject++;
        break;
      }
      default:
        break;
    }

  }
  void RenderingStats::UnRegister (IOpenGLResource *GraphicsObject)
  {
    switch (GraphicsObject->GetResourceType() )
    {
      case RTINDEXBUFFER:
      {
        m_NumIndexBuffer--;
        m_GPUSizeIndexBuffer -= NUX_REINTERPRET_CAST (IOpenGLIndexBuffer *, GraphicsObject)->GetSize();
        m_TotalGPUSize -= m_GPUSizeIndexBuffer;
        break;
      }
      case RTVERTEXBUFFER:
      {
        m_NumVertexBuffer--;
        m_GPUSizeVertexBuffer -= NUX_REINTERPRET_CAST (IOpenGLVertexBuffer *, GraphicsObject)->GetSize();
        m_TotalGPUSize -= m_GPUSizeVertexBuffer;
        break;
      }
      case RTTEXTURE:
      {
        std::vector<int>::iterator it;
        it = std::find (_texture_2d_array.begin(), _texture_2d_array.end(), NUX_STATIC_CAST (IOpenGLBaseTexture *, GraphicsObject)->GetOpenGLID() );

        if (it != _texture_2d_array.end() )
          _texture_2d_array.erase (it);

        m_NumTex2D--;
        m_GPUSizeTex2D -= GetTextureSize (NUX_REINTERPRET_CAST (IOpenGLBaseTexture *, GraphicsObject) );
        m_TotalGPUSize -= m_GPUSizeTex2D;
        break;
      }
      case RTTEXTURERECTANGLE:
      {
        std::vector<int>::iterator it;
        it = std::find (_texture_rect_array.begin(), _texture_rect_array.end(), NUX_STATIC_CAST (IOpenGLBaseTexture *, GraphicsObject)->GetOpenGLID() );

        if (it != _texture_rect_array.end() )
          _texture_rect_array.erase (it);

        m_NumTexRectangle--;
        m_GPUSizeTexRectangle -= GetTextureSize (NUX_REINTERPRET_CAST (IOpenGLBaseTexture *, GraphicsObject) );
        m_TotalGPUSize -= m_GPUSizeTexRectangle;
        break;
      }
      case RTCUBETEXTURE:
      {
        m_NumTexCube--;
        m_GPUSizeTexCube -= GetTextureSize (NUX_STATIC_CAST (IOpenGLBaseTexture *, GraphicsObject) );
        m_TotalGPUSize -= m_GPUSizeTexCube;
        break;
      }
      case RTVOLUMETEXTURE:
      {
        m_NumTexVolume--;
        m_GPUSizeTexVolume -= GetTextureSize (NUX_STATIC_CAST (IOpenGLBaseTexture *, GraphicsObject) );
        m_TotalGPUSize -= m_GPUSizeTexVolume;
        break;
      }
      case RTANIMATEDTEXTURE:
      {
        // Animated texture size is compted as rectangle textures;
        m_NumTexAnimated--;
        break;
      }
      case RTQUERY:
      {
        m_NumQueryObject--;
        break;
      }
      case RTFRAMEBUFFEROBJECT:
      {
        m_NumFrameBufferObject--;
        break;
      }
      default:
        break;
    }

  }

}
