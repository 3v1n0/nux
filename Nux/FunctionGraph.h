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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef FUNCTIONGRAPH_H
#define FUNCTIONGRAPH_H

#include "NuxGraphics/GpuDevice.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "NuxGraphics/GLSh_DrawFunction.h"

namespace nux
{

  typedef float (*FunctionCallback) (float);

  class FunctionGraph : public View
  {
  public:
    FunctionGraph (NUX_FILE_LINE_PROTO);
    ~FunctionGraph();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void SetXAxisBounds (float minX, float maxX);
    void SetYAxisBounds (float minY, float maxY);
    void SetFunctionCallback (FunctionCallback f);
    float EvalFunction (float x);
    void UpdateGraph();

  protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

  private:
    float m_minX, m_minY, m_maxX, m_maxY;
    FunctionCallback m_FunctionCallback;

    ObjectPtr<IOpenGLBaseTexture> Texture;
    TextureLayer *m_BackgroundLayer;
    GLSh_DrawFunction *m_DrawFunctionShader;
  };


}

#endif // FUNCTIONGRAPH_H
