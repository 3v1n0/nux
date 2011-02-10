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


#include "Nux.h"
#include "Layout.h"
#include "GroupBox.h"

namespace nux
{

  GroupBox::GroupBox (const TCHAR *Caption, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   bCaptionAvailable (false)
    ,   m_layout (0)
  {
    m_CaptionArea.SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    m_CaptionArea.SetBaseSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    SetMinimumSize (DEFAULT_WIDGET_WIDTH + 5, PRACTICAL_WIDGET_HEIGHT + 5);
    SetBaseSize (DEFAULT_WIDGET_WIDTH + 5, 2 * PRACTICAL_WIDGET_HEIGHT);
    SetCaption (TEXT ("") );
  }

  GroupBox::~GroupBox()
  {

  }

  long GroupBox::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    long ProcEvInfo = 0;

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      if (!m_Geometry.IsPointInside (ievent.e_x, ievent.e_y) )
      {
        ProcEvInfo = eDoNotProcess;
      }
    }

    if (m_layout != 0)
    {
      ret = m_layout->ProcessEvent (ievent, ret, ProcEvInfo);
    }

    ret = PostProcessEvent2 (ievent, ret, 0);
    return ret;
  }


  void GroupBox::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry() );

    Geometry wireborder_geo = GetGeometry();

    //if(bCaptionAvailable)
    {
      wireborder_geo.OffsetPosition (0, 10);
      wireborder_geo.OffsetSize (0, -10);
    }
//    else
//    {
//        wireborder_geo.OffsetPosition(0, 0);
//        wireborder_geo.OffsetSize(0, 0);
//    }

    //if(bCaptionAvailable)
    {
      //GetPainter().Paint2DQuadColor(m_CaptionArea.GetGeometry(), COLOR_BACKGROUND_PRIMARY);
      //GetPainter().PaintTextLineStatic(m_CaptionArea.GetGeometry(), m_CaptionArea.GetCaptionString(), eAlignTextCenter);
      GetPainter().PaintTextLineStatic (GfxContext, GetSysBoldFont(), m_CaptionArea.GetGeometry(), m_CaptionArea.GetBaseString().GetTCharPtr(), GetTextColor(),
                                    true, eAlignTextCenter);
    }

    if (m_layout != 0)
    {
      m_layout->NeedRedraw();
    }

    GfxContext.PopClippingRectangle();
  }

  void GroupBox::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry() );

    if (m_layout)
    {
      GfxContext.PushClippingRectangle (m_layout->GetGeometry() );
      m_layout->ProcessDraw (GfxContext, force_draw);
      GfxContext.PopClippingRectangle();
    }

    GfxContext.PopClippingRectangle();
  }

  void GroupBox::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  bool GroupBox::SetLayout (Layout *layout)
  {
    if(View::SetLayout(layout) == false)
    {
      return false;
    }

    m_layout = layout;

    return true;

//    Geometry geo = GetGeometry();
//    Geometry layout_geo = Geometry(geo.x + m_border, geo.y + m_top_border,
//        geo.GetWidth() - 2*m_border, geo.GetHeight() - m_border - m_top_border);
//    m_layout->SetGeometry(layout_geo);
  }

  void GroupBox::PreLayoutManagement()
  {
    // Give the managed layout appropriate size and position..
    if (m_CompositionLayout)
    {
      Geometry layout_geo = GetGeometry();
      //if(bCaptionAvailable)
      {
        layout_geo.OffsetPosition (2, 20);
        layout_geo.OffsetSize (-4, -22);
      }
//        else
//        {
//            layout_geo.OffsetPosition(2, 2);
//            layout_geo.OffsetSize(-4, -4);
//        }
      m_CompositionLayout->SetGeometry (layout_geo);
    }
  }

  long GroupBox::PostLayoutManagement (long LayoutResult)
  {
    // A Group box must tightly group its children.
    // So it must embrace the size that was compute for the composition layout.
    // Only the size is change is important here of the GroupBox is important here.

    long ret = 0;
    Geometry old_geo = Area::GetGeometry();

    if (m_CompositionLayout)
    {
      Geometry base = m_CompositionLayout->GetGeometry();
      //if(bCaptionAvailable)
      {
        base.OffsetPosition (-2, -20);
        base.OffsetSize (4, 22);
      }
//        else
//        {
//            base.OffsetPosition(-2, -2);
//            base.OffsetSize(4, 4);
//        }
      Area::SetGeometry (base);
    }

    Geometry base = GetGeometry();
    m_CaptionArea.SetBaseXY (base.x + 6, base.y);

    if (old_geo.GetWidth() > base.GetWidth() )
      ret |= eLargerWidth;
    else if (old_geo.GetWidth() < base.GetWidth() )
      ret |= eSmallerWidth;
    else
      ret |= eCompliantWidth;

    if (old_geo.GetHeight() > base.GetHeight() )
      ret |= eLargerHeight;
    else if (old_geo.GetHeight() < base.GetHeight() )
      ret |= eSmallerHeight;
    else
      ret |= eCompliantHeight;

    return ret;
  }

  void GroupBox::PositionChildLayout (float offsetX, float offsetY)
  {
    if (m_CompositionLayout)
    {
      //if(bCaptionAvailable)
      {
        m_CompositionLayout->SetBaseX (GetBaseX() + 2);
        m_CompositionLayout->SetBaseY (GetBaseY() + 20);
      }
//        else
//        {
//            m_CompositionLayout->SetX(GetX() + 2);
//            m_CompositionLayout->SetY(GetY() + 2);
//        }
      m_CompositionLayout->ComputePosition2 (offsetX, offsetY);
    }

    Geometry base = GetGeometry();
    m_CaptionArea.SetBaseXY (base.x + 6, base.y);
  }

  void GroupBox::SetCaption (const char *name)
  {
    if ( (name == 0) || strlen (name) == 0)
    {
      //bCaptionAvailable = false;
      m_CaptionArea.SetBaseString (TEXT ("") );
      m_CaptionArea.SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
      m_CaptionArea.SetBaseSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    }
    else
    {
      //bCaptionAvailable = true;
      m_CaptionArea.SetBaseString (name);
      m_CaptionArea.SetMinimumSize (4 + GetSysBoldFont()->GetStringWidth (name), PRACTICAL_WIDGET_HEIGHT);
      m_CaptionArea.SetBaseSize (4 + GetSysBoldFont()->GetStringWidth (name), PRACTICAL_WIDGET_HEIGHT);
    }
  }

}
