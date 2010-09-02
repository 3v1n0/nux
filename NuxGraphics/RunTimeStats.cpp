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


#include "GLResource.h"
#include "IOpenGLResource.h"
#include "IOpenGLBaseTexture.h"
#include "IOpenGLIndexBuffer.h"
#include "IOpenGLVertexBuffer.h"
#include "RunTimeStats.h"


NAMESPACE_BEGIN_OGL

INL_IMPLEMENT_GLOBAL_OBJECT(RenderingStats);

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
    if(m_NumIndexBuffer != 0)
    {
        nuxDebugMsg(TEXT("[RenderingStats::Destructor] Some index buffers have not been released."));
    }

    if(m_NumTex2D != 0)
    {
        nuxDebugMsg(TEXT("[RenderingStats::Destructor] Some 2D textures have not been released."));
        std::vector<int>::iterator it;
        for(it = v.begin(); it != v.end(); it++)
        {
            nuxDebugMsg(TEXT("[RenderingStats::Destructor] Remaining 2D textures: %d."), *it);
        }
    }

    if(m_NumTexRectangle != 0)
    {
        nuxDebugMsg(TEXT("[RenderingStats::Destructor] Some rectangle textures buffers have not been released."));
    }
}

void RenderingStats::Register(IOpenGLResource* GraphicsObject)
{
    switch (GraphicsObject->GetResourceType())
    {
    case RTINDEXBUFFER:
        {
            m_NumIndexBuffer++;
            m_GPUSizeIndexBuffer += INL_REINTERPRET_CAST(IOpenGLIndexBuffer*, GraphicsObject)->GetSize();
            m_TotalGPUSize += m_GPUSizeIndexBuffer;
            break;
        }
    case RTVERTEXBUFFER:
        {
            m_NumVertexBuffer++;
            m_GPUSizeVertexBuffer += INL_REINTERPRET_CAST(IOpenGLVertexBuffer*, GraphicsObject)->GetSize();
            m_TotalGPUSize += m_GPUSizeVertexBuffer;
            break;
        }
    case RTTEXTURE:
        {
            v.push_back(INL_STATIC_CAST(IOpenGLBaseTexture*, GraphicsObject)->GetOpenGLID());
            m_NumTex2D++;
            m_GPUSizeTex2D += GetTextureSize(INL_REINTERPRET_CAST(IOpenGLBaseTexture*, GraphicsObject));
            m_TotalGPUSize += m_GPUSizeTex2D;
            break;
        }
    case RTTEXTURERECTANGLE:
        {
            m_NumTexRectangle++;
            m_GPUSizeTexRectangle += GetTextureSize(INL_REINTERPRET_CAST(IOpenGLBaseTexture*, GraphicsObject));
            m_TotalGPUSize += m_GPUSizeTexRectangle;
            break;
        }
    case RTCUBETEXTURE:
        {
            m_NumTexCube++;
            m_GPUSizeTexCube += GetTextureSize(INL_STATIC_CAST(IOpenGLBaseTexture*, GraphicsObject));
            m_TotalGPUSize += m_GPUSizeTexCube;
            break;
        }
    case RTVOLUMETEXTURE:
        {
            m_NumTexVolume++;
            m_GPUSizeTexVolume += GetTextureSize(INL_STATIC_CAST(IOpenGLBaseTexture*, GraphicsObject));
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
    }
}
void RenderingStats::UnRegister(IOpenGLResource* GraphicsObject)
{
    switch (GraphicsObject->GetResourceType())
    {
    case RTINDEXBUFFER:
        {
            m_NumIndexBuffer--;
            m_GPUSizeIndexBuffer -= INL_REINTERPRET_CAST(IOpenGLIndexBuffer*, GraphicsObject)->GetSize();
            m_TotalGPUSize -= m_GPUSizeIndexBuffer;
            break;
        }
    case RTVERTEXBUFFER:
        {
            m_NumVertexBuffer--;
            m_GPUSizeVertexBuffer -= INL_REINTERPRET_CAST(IOpenGLVertexBuffer*, GraphicsObject)->GetSize();
            m_TotalGPUSize -= m_GPUSizeVertexBuffer;
            break;
        }
    case RTTEXTURE:
        {
            std::vector<int>::iterator it;
            it = std::find(v.begin(), v.end(), INL_STATIC_CAST(IOpenGLBaseTexture*, GraphicsObject)->GetOpenGLID());
            if(it != v.end())
                v.erase(it);
            m_NumTex2D--;
            m_GPUSizeTex2D -= GetTextureSize(INL_REINTERPRET_CAST(IOpenGLBaseTexture*, GraphicsObject));
            m_TotalGPUSize -= m_GPUSizeTex2D;
            break;
        }
    case RTTEXTURERECTANGLE:
        {
            m_NumTexRectangle--;
            m_GPUSizeTexRectangle -= GetTextureSize(INL_REINTERPRET_CAST(IOpenGLBaseTexture*, GraphicsObject));
            m_TotalGPUSize -= m_GPUSizeTexRectangle;
            break;
        }
    case RTCUBETEXTURE:
        {
            m_NumTexCube--;
            m_GPUSizeTexCube -= GetTextureSize(INL_STATIC_CAST(IOpenGLBaseTexture*, GraphicsObject));
            m_TotalGPUSize -= m_GPUSizeTexCube;
            break;
        }
    case RTVOLUMETEXTURE:
        {
            m_NumTexVolume--;
            m_GPUSizeTexVolume -= GetTextureSize(INL_STATIC_CAST(IOpenGLBaseTexture*, GraphicsObject));
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
    }
}

NAMESPACE_END_OGL