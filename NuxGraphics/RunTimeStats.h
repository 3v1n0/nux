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


#ifndef RUNTIMESTATS_H
#define RUNTIMESTATS_H

namespace nux
{

  class RenderingStats
  {
    NUX_DECLARE_GLOBAL_OBJECT (RenderingStats, NuxGraphicsGlobalSingletonInitializer);
  public:
    static t_u32 m_NumTex2D;
    static t_u32 m_GPUSizeTex2D;
    static t_u32 m_NumTexRectangle;
    static t_u32 m_GPUSizeTexRectangle;
    static t_u32 m_NumTexCube;
    static t_u32 m_GPUSizeTexCube;
    static t_u32 m_NumTexVolume;
    static t_u32 m_GPUSizeTexVolume;
    static t_u32 m_NumTexAnimated;
    static t_u32 m_GPUSizeTexAnimated;

    static t_u32 m_NumPBO;
    static t_u32 m_GPUSizePBO;

    static t_u32 m_NumFrameBufferObject;

    static t_u32 m_NumQueryObject;

    static t_u32 m_NumVertexBuffer;
    static t_u32 m_GPUSizeVertexBuffer;

    static t_u32 m_NumIndexBuffer;
    static t_u32 m_GPUSizeIndexBuffer;

    static t_u32 m_NumVertexShader;
    static t_u32 m_NumPixelShader;
    static t_u32 m_NumShaderProgram;

    static t_u32 m_TotalGPUSize;
    void Register (IOpenGLResource *GraphicsObject);
    void UnRegister (IOpenGLResource *GraphicsObject);

    std::vector<int> _texture_2d_array;
    std::vector<int> _texture_rect_array;
  };

}

#define GRunTimeStats NUX_GLOBAL_OBJECT_INSTANCE(nux::RenderingStats)

#endif // RUNTIMESTATS_H
