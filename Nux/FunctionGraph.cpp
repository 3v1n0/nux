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


#include "Nux.h"
#include "NuxGraphics/GLSh_DrawFunction.h"
#include "FunctionGraph.h"

namespace nux
{

  FunctionGraph::FunctionGraph (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_minX (0.0f)
    ,   m_minY (0.0f)
    ,   m_maxX (1.0f)
    ,   m_maxY (1.0f)
    ,   m_FunctionCallback (0)
  {
    InitializeLayout();
    InitializeWidgets();
  }

  FunctionGraph::~FunctionGraph()
  {
    DestroyLayout();
  }

  void FunctionGraph::InitializeWidgets()
  {
    SetMinimumSize (200, 100);
    SetBaseSize (200, 100);

    Texture = GetGpuDevice()->CreateSystemCapableDeviceTexture (256, 4, 0, BITFMT_R8G8B8A8);
    m_DrawFunctionShader = new GLSh_DrawFunction();

    NString Path = NUX_FINDRESOURCELOCATION (TEXT ("UITextures/FunctionGraphBackground.tga") );
    BaseTexture* BackgroundTexture = GetGpuDevice()->CreateSystemCapableTexture ();
    BackgroundTexture->Update (Path.GetTCharPtr() );

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_BackgroundLayer = new TextureLayer (BackgroundTexture->GetDeviceTexture(), texxform, color::White);

    BackgroundTexture->UnReference ();
  }

  void FunctionGraph::InitializeLayout()
  {

  }

  void FunctionGraph::DestroyLayout()
  {
    NUX_SAFE_DELETE (m_DrawFunctionShader);
    NUX_SAFE_DELETE (m_BackgroundLayer);
  }

  long FunctionGraph::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return TraverseInfo;
  }

  void FunctionGraph::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground (GfxContext, base);
    GetPainter().Paint2DQuadWireframe (GfxContext, base, Color (COLOR_BACKGROUND_SECONDARY) );

    int W = GetBaseWidth() - 2;
    int H = GetBaseHeight() - 2;
    int X = GetBaseX() + 1;
    int Y = GetBaseY() + 1;

    float dX = (m_maxX - m_minX) / W;

    float x0, y0;
    x0 = m_minX;
    y0 = EvalFunction (x0);

    //GetPainter().Draw2DLine(X, Y, X+W, Y+H, Color(0xFFFF0000));

    base.OffsetPosition (1, 1);
    base.OffsetSize (-2, -2);

    GfxContext.PushClippingRectangle (base);

    float tex_dx = (m_maxX - m_minX) / Texture->GetWidth();
    SURFACE_LOCKED_RECT lockrect;
    Texture->LockRect (0, &lockrect, 0);
    BYTE *dest = (BYTE *) lockrect.pBits;

    for (t_s32 i = 0; i < Texture->GetWidth(); i++)
    {
      float y = EvalFunction (m_minX + i * tex_dx);
      y = (y - m_minY) / (m_maxY - m_minY);

      for (t_s32 j = 0; j < Texture->GetHeight(); j++)
      {
        dest[4*i + 0 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
        dest[4*i + 1 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
        dest[4*i + 2 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
        dest[4*i + 3 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
      }
    }

    Texture->UnlockRect (0);

    m_BackgroundLayer->SetGeometry (base);
    m_BackgroundLayer->Renderlayer (GfxContext);

    GfxContext.GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_DrawFunctionShader->SetTextureFunction (Texture);
    m_DrawFunctionShader->SetBackgroundColor(Color(0.1f, 0.1f, 0.1f, 0.6f));
    m_DrawFunctionShader->Render (X, Y, 0, W, H, GfxContext.GetWindowWidth(), GfxContext.GetWindowHeight() );

    GfxContext.GetRenderStates().EnableLineSmooth (TRUE, 1, GL_FASTEST);   //You need this blending formula to get anti-aliased lines

    for (int i = 1; i < GetBaseWidth(); i++)
    {
      float x1, y1;
      x1 = x0 + dX;
      y1 = EvalFunction (x1);

      int X0, Y0, X1, Y1;
      X0 = X + W * (x0 - m_minX) / (m_maxX - m_minX);
      Y0 = Y + H * ( 1 - (y0 - m_minY) / (m_maxY - m_minY) );
      X1 = X + W * (x1 - m_minX) / (m_maxX - m_minX);
      Y1 = Y + H * ( 1 - (y1 - m_minY) / (m_maxY - m_minY) );
      GetPainter().Draw2DLine (GfxContext, X0, Y0, X1, Y1, Color (0xFFFFFFFF) );

      x0 = x1;
      y0 = y1;
    }

    GfxContext.GetRenderStates().EnableLineSmooth (FALSE);
    GfxContext.GetRenderStates().SetBlend (GL_FALSE);

    GfxContext.PopClippingRectangle();
  }

  void FunctionGraph::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void FunctionGraph::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void FunctionGraph::SetXAxisBounds (float minX, float maxX)
  {
    m_minX = minX;
    m_maxX = maxX;
    NeedRedraw();
  }

  void FunctionGraph::SetYAxisBounds (float minY, float maxY)
  {
    m_minY = minY;
    m_maxY = maxY;
    NeedRedraw();
  }

  void FunctionGraph::SetFunctionCallback (FunctionCallback f)
  {
    m_FunctionCallback = f;
    NeedRedraw();
  }

  float FunctionGraph::EvalFunction (float x)
  {
    if (m_FunctionCallback != 0)
      return (*m_FunctionCallback) (x);

    return 0;
  }

  void FunctionGraph::UpdateGraph()
  {
    NeedRedraw();
  }

}
