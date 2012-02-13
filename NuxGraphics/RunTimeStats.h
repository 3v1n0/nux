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


#ifndef RUNTIMESTATS_H
#define RUNTIMESTATS_H

namespace nux
{

  class RenderingStats
  {
    NUX_DECLARE_GLOBAL_OBJECT(RenderingStats, NuxGraphicsGlobalSingletonInitializer);
  public:
    static unsigned int m_NumTex2D;
    static unsigned int m_GPUSizeTex2D;
    static unsigned int m_NumTexRectangle;
    static unsigned int m_GPUSizeTexRectangle;
    static unsigned int m_NumTexCube;
    static unsigned int m_GPUSizeTexCube;
    static unsigned int m_NumTexVolume;
    static unsigned int m_GPUSizeTexVolume;
    static unsigned int m_NumTexAnimated;
    static unsigned int m_GPUSizeTexAnimated;

    static unsigned int m_NumPBO;
    static unsigned int m_GPUSizePBO;

    static unsigned int m_NumFrameBufferObject;

    static unsigned int m_NumQueryObject;

    static unsigned int m_NumVertexBuffer;
    static unsigned int m_GPUSizeVertexBuffer;

    static unsigned int m_NumIndexBuffer;
    static unsigned int m_GPUSizeIndexBuffer;

    static unsigned int m_NumVertexShader;
    static unsigned int m_NumPixelShader;
    static unsigned int m_NumShaderProgram;

    static unsigned int m_TotalGPUSize;
    void Register(IOpenGLResource *GraphicsObject);
    void UnRegister(IOpenGLResource *GraphicsObject);

    std::vector<int> _texture_2d_array;
    std::vector<int> _texture_rect_array;
  };

}

#define GRunTimeStats NUX_GLOBAL_OBJECT_INSTANCE(nux::RenderingStats)

#endif // RUNTIMESTATS_H
