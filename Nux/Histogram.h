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


#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "NuxGraphics/GLDeviceFactory.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "NuxGraphics/GLSh_DrawFunction.h"

namespace nux
{

  class Histogram : public View
  {
  public:
    Histogram (NUX_FILE_LINE_PROTO);
    ~Histogram();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsContext &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw);

    void setXAxisBounds (int minX, int maxX);
    void setYAxisBounds (int minY, int maxY);

    void SetHistogram (std::vector<int>& HistogramData);

  private:
    int m_minX, m_minY, m_maxX, m_maxY;

    std::vector<int> m_HistogramData;
    IntrusiveSP<IOpenGLBaseTexture> Texture;
    TextureLayer *m_BackgroundLayer;
    GLSh_DrawFunction *m_DrawFunctionShader;
  };

}

#endif // HISTOGRAM_H
