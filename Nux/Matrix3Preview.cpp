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
#include "Theme.h"
#include "Matrix3Preview.h"
#include "StaticTextBox.h"

namespace nux
{

  static const int GRAPH_MARGIN = 1;

  Matrix3Preview::Matrix3Preview (Matrix3 matrix, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    m_DialogThreadProxy = new Matrix3DialogProxy (true);

    SetMinMaxSize (30, 30);

    OnMouseClick.connect (sigc::mem_fun (this, &Matrix3Preview::RecvClick) );

    m_ChangeDetectionTimer = new TimerFunctor();
    m_ChangeDetectionTimer->OnTimerExpired.connect (sigc::mem_fun (this, &Matrix3Preview::RecvTimer) );
    m_ChangeTimerHandler = 0;
  }

  Matrix3Preview::~Matrix3Preview()
  {
    if (m_ChangeTimerHandler.IsValid() )
      GetTimer().RemoveTimerHandler (m_ChangeTimerHandler);

    NUX_SAFE_DELETE (m_DialogThreadProxy);
  }

  long Matrix3Preview::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void Matrix3Preview::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground (GfxContext, base);
    //GetPainter().PaintShape(GfxContext, base, 0xFF4D4D4D, eSHAPE_CORNER_ROUND4, false);

    GeometryPositioning gp (eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning (base, GetTheme().GetImageGeometry (eMATRIX3PREVIEW), gp);
    GetPainter().PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eMATRIX3PREVIEW);

    GetPainter().Paint2DQuadWireframe (GfxContext, base, Color (COLOR_BACKGROUND_SECONDARY) );
    //GetPainter().Paint2DQuadWireframe(GfxContext, base, Color(COLOR_BACKGROUND_SECONDARY));
  }

  void Matrix3Preview::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
  }

  void Matrix3Preview::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void Matrix3Preview::RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    m_DialogThreadProxy->Start();

    m_ChangeTimerHandler = GetTimer().AddTimerHandler (33, m_ChangeDetectionTimer, this);
  }

  void Matrix3Preview::RecvTimer (void *v)
  {
    if (m_DialogThreadProxy->m_bDialogChange && m_DialogThreadProxy->m_bDialogRunning)
    {
      m_DialogThreadProxy->m_bDialogChange = false;
      m_Matrix = m_DialogThreadProxy->GetMatrix();
    }

    if (m_DialogThreadProxy->IsActive() )
    {
      m_ChangeTimerHandler = GetTimer().AddTimerHandler (33, m_ChangeDetectionTimer, this);
    }
    else
    {
      if (m_ChangeTimerHandler.IsValid() )
        GetTimer().RemoveTimerHandler (m_ChangeTimerHandler);

      m_ChangeTimerHandler = 0;

      m_Matrix = m_DialogThreadProxy->GetMatrix();
    }
  }

  void Matrix3Preview::RecvDialogChange (Matrix3Editor *matrixeditor)
  {
  }

  void Matrix3Preview::SetMatrix (Matrix3 matrix)
  {
    m_Matrix = matrix;
  }

  Matrix3 Matrix3Preview::GetMatrix() const
  {
    return m_Matrix;
  }

}
