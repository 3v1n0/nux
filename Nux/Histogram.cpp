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
#include "Histogram.h"

namespace nux
{

  Histogram::Histogram (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_minX (0)
    ,   m_minY (0)
    ,   m_maxX (100)
    ,   m_maxY (20)
  {

    SetMinimumSize (100, 100);
    //SetMinMaxSize(100, 100);
    SetBaseSize (200, 100);

    for (int i = 0; i < m_maxX - m_minX; i++)
      m_HistogramData.push_back (/*5*pow(i * Pi/(300.0f)/1.3f, 0.9f)*/ + cos (i * Const::Pi / (100.0f) / 1.7f) * (15 + RandomUInt (3 + 7 * i / 100.0f) ) );

    m_DrawFunctionShader = new GLSh_DrawFunction();

    NString Path = NUX_FINDRESOURCELOCATION (TEXT ("Data/UITextures/FunctionGraphBackground.tga"));
    BaseTexture* BackgroundTexture = GetGpuDevice()->CreateSystemCapableTexture ();
    BackgroundTexture->Update (Path.GetTCharPtr());

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_BackgroundLayer = new TextureLayer (BackgroundTexture->GetDeviceTexture(), texxform, Colors::White);

    BackgroundTexture->UnReference ();
  }

  Histogram::~Histogram()
  {
    NUX_SAFE_DELETE (m_DrawFunctionShader);
    NUX_SAFE_DELETE (m_BackgroundLayer);
  }

  long Histogram::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return TraverseInfo;
  }

  void Histogram::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground (GfxContext, base);
    GetPainter().Paint2DQuadWireframe (GfxContext, base, Color (COLOR_BACKGROUND_SECONDARY) );

    int W = GetBaseWidth() - 2;
    int H = GetBaseHeight() - 2;
    int X = GetBaseX() + 1;
    int Y = GetBaseY() + 1;

    float dX = float (m_maxX - m_minX) / W;

    float x0, y0;
    x0 = m_minX;
    y0 = m_HistogramData[0];

    base.OffsetPosition (1, 1);
    base.OffsetSize (-2, -2);

    GfxContext.PushClippingRectangle (base);

    if (Texture.IsNull() || (Texture->GetWidth() != (t_s32) m_HistogramData.size() ) )
      Texture = GetGpuDevice()->CreateSystemCapableDeviceTexture ( (t_s32) m_HistogramData.size(), 1, 0, BITFMT_A8);

    float tex_dx = (m_maxX - m_minX) / Texture->GetWidth();

    SURFACE_LOCKED_RECT lockrect;

    Texture->LockRect (0, &lockrect, 0);
    BYTE *dest = (BYTE *) lockrect.pBits;

    for (t_s32 i = 0; i < Texture->GetWidth(); i++)
    {
      float y = 0;

      if (m_HistogramData.size() == 0)
        y = 0;
      else
        y = m_HistogramData[Clamp<int> (int (m_minX + i*tex_dx), 0, (t_s32) m_HistogramData.size()-1) ];

      y = float (y - m_minY) / float (m_maxY - m_minY);

      for (t_s32 j = 0; j < Texture->GetHeight(); j++)
      {
        dest[1*i + 0 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
//            dest[4*i + 1 + j * lockrect.Pitch] = 255 * Clamp<float>(y, 0.0f, 1.0f);
//            dest[4*i + 2 + j * lockrect.Pitch] = 255 * Clamp<float>(y, 0.0f, 1.0f);
//            dest[4*i + 3 + j * lockrect.Pitch] = 255 * Clamp<float>(y, 0.0f, 1.0f);
      }
    }

    Texture->UnlockRect (0);

    m_BackgroundLayer->SetGeometry (base);
    m_BackgroundLayer->Renderlayer (GfxContext);

    GfxContext.GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_DrawFunctionShader->SetTextureFunction (Texture);
    m_DrawFunctionShader->SetBackgroundColor (0.1f, 0.1f, 0.1f, 0.6f);
    m_DrawFunctionShader->Render (X, Y, 0, W, H, GfxContext.GetWindowWidth(), GfxContext.GetWindowHeight() );

    GfxContext.GetRenderStates().EnableLineSmooth (TRUE, 1, GL_FASTEST);   //You need this blending formula to get anti-aliased lines

    for (int i = 1; i < GetBaseWidth(); i++)
    {
      float x1, y1;
      x1 = x0 + dX;

      if (m_HistogramData.size() == 0)
        y1 = 0;
      else
        y1 = m_HistogramData[Clamp<int> (int (x1), 0, (t_s32) m_HistogramData.size()-1) ];

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

  void Histogram::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void Histogram::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void Histogram::setXAxisBounds (int minX, int maxX)
  {
    //m_minX = minX;
    //m_maxX = maxX;

    //Texture = GetGpuDevice()->CreateSystemCapableDeviceTexture(m_maxX - m_minX, 4, 0, BITFMT_R8G8B8A8);
    NeedRedraw();
  }

  void Histogram::setYAxisBounds (int minY, int maxY)
  {
    m_minY = minY;
    m_maxY = maxY;
    NeedRedraw();
  }

  void Histogram::SetHistogram (std::vector<int>& HistogramData)
  {
    if (HistogramData.size() == 0)
      return;

    m_HistogramData = HistogramData;
    m_minX = 0;
    m_maxX = (t_s32) m_HistogramData.size();
    NeedRedraw();
  }



}
