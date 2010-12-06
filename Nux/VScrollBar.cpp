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
#include "HLayout.h"
#include "VLayout.h"
#include "VScrollBar.h"

namespace nux
{

  const int VSCROLLBAR_WIDTH = 10;
  const int VSCROLLBAR_HEIGHT = 10;

  VScrollBar::VScrollBar (NUX_FILE_LINE_DECL)
    :   ScrollBar (NUX_FILE_LINE_PARAM)
  {
    m_contentWidth      = 0;
    m_contentHeight     = 0;
    m_containerWidth    = 0;
    m_containerHeight   = 0;
    m_TrackWidth        = 0;
    m_TrackHeight       = 0;
    m_SlideBarOffsetX   = 0;
    m_SlideBarOffsetY   = 0;
    m_contentOffsetX    = 0;
    m_contentOffsetY    = 0;
    b_MouseUpTimer      = false;
    b_MouseDownTimer    = false;
    m_color_factor      = 1.0f;
    m_UpTimerHandler    = 0;
    m_DownTimerHandler  = 0;

    vlayout         = new VLayout (TEXT (""), NUX_TRACKER_LOCATION);
    m_TopThumb      = new CoreArea (NUX_TRACKER_LOCATION);
    m_Track         = new CoreArea (NUX_TRACKER_LOCATION);
    m_BottomThumb   = new CoreArea (NUX_TRACKER_LOCATION);
    m_SlideBar      = new CoreArea (NUX_TRACKER_LOCATION);

    // Set Original State
    SetMinimumSize (VSCROLLBAR_WIDTH, BASEOBJECT_MINHEIGHT);
    SetMaximumSize (VSCROLLBAR_WIDTH, BASEOBJECT_MAXHEIGHT);

    // Set Signals
    m_BottomThumb->OnMouseDown.connect ( sigc::mem_fun (this, &VScrollBar::RecvStartScrollDown) );
    m_BottomThumb->OnMouseUp.connect ( sigc::mem_fun (this, &VScrollBar::RecvEndScrollDown) );
    m_TopThumb->OnMouseDown.connect ( sigc::mem_fun (this, &VScrollBar::RecvStartScrollUp) );
    m_TopThumb->OnMouseUp.connect ( sigc::mem_fun (this, &VScrollBar::RecvEndScrollUp) );

    m_SlideBar->OnMouseDown.connect ( sigc::mem_fun (this, &VScrollBar::OnSliderMouseDown) );
    m_SlideBar->OnMouseUp.connect ( sigc::mem_fun (this, &VScrollBar::OnSliderMouseUp) );
    m_SlideBar->OnMouseDrag.connect ( sigc::mem_fun (this, &VScrollBar::OnSliderMouseDrag) );

    m_Track->OnMouseDown.connect ( sigc::mem_fun (this, &VScrollBar::RecvTrackMouseDown) );
    m_Track->OnMouseUp.connect ( sigc::mem_fun (this, &VScrollBar::RecvTrackMouseUp) );
    m_Track->OnMouseDrag.connect ( sigc::mem_fun (this, &VScrollBar::RecvTrackMouseDrag) );

    //m_Track->OnMouseDown.connect( sigc::mem_fun(this, &VScrollBar::OnSliderMouseDown));

    // Set Geometry
    m_BottomThumb->SetMinimumSize (VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
    m_BottomThumb->SetMaximumSize (VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
    m_BottomThumb->SetGeometry (Geometry (0, 0, VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT) );
    m_TopThumb->SetMinimumSize (VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
    m_TopThumb->SetMaximumSize (VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
    m_TopThumb->SetGeometry (Geometry (0, 0, VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT) );

    vlayout->AddView (m_TopThumb, 0, eCenter, eFix);
    vlayout->AddView (m_Track, 1, eCenter, eFull);
    vlayout->AddView (m_BottomThumb, 0, eCenter, eFix);

    callback = new TimerFunctor;
    callback->OnTimerExpired.connect (sigc::mem_fun (this, &VScrollBar::VScrollBarHandler) );
    up_callback = new TimerFunctor;
    up_callback->OnTimerExpired.connect (sigc::mem_fun (this, &VScrollBar::ScrollUp) );
    down_callback = new TimerFunctor;
    down_callback->OnTimerExpired.connect (sigc::mem_fun (this, &VScrollBar::ScrollDown) );
    trackup_callback = new TimerFunctor;
    trackup_callback->OnTimerExpired.connect (sigc::mem_fun (this, &VScrollBar::TrackUp) );
    trackdown_callback = new TimerFunctor;
    trackdown_callback->OnTimerExpired.connect (sigc::mem_fun (this, &VScrollBar::TrackDown) );

    SetCompositionLayout (vlayout);
  }


  VScrollBar::~VScrollBar()
  {
    delete callback;
    delete up_callback;
    delete trackup_callback;
    delete down_callback;
  }

  void VScrollBar::VScrollBarHandler (void *v)
  {
    VScrollBar *scrollbar = static_cast<VScrollBar *> (v);

    if (scrollbar->b_MouseUpTimer && scrollbar->m_color_factor < 1)
    {
      scrollbar->m_color_factor += 0.1f;

      if (scrollbar->m_color_factor >= 1)
      {
        scrollbar->m_color_factor = 1;
        scrollbar->b_MouseUpTimer = false;
      }
      else
      {
        scrollbar->NeedRedraw();
        GetTimer().AddTimerHandler (10, callback, scrollbar);
      }
    }

    if (scrollbar->b_MouseDownTimer && scrollbar->m_color_factor > 0)
    {
      scrollbar->m_color_factor -= 0.09f;

      if (scrollbar->m_color_factor <= 0)
      {
        scrollbar->m_color_factor = 0;
        scrollbar->b_MouseUpTimer = false;
      }
      else
      {
        scrollbar->NeedRedraw();
        GetTimer().AddTimerHandler (10, callback, scrollbar);
      }
    }

    NeedRedraw();
  }

  void VScrollBar::ScrollDown (void *v)
  {
    OnScrollDown.emit (m_ScrollUnit, 1);

    if (AtMaximum() )
      RecvEndScrollDown (0, 0, 0, 0);
    else
      m_DownTimerHandler = GetTimer().AddTimerHandler (10, down_callback, this);

    NeedRedraw();
  }

  void VScrollBar::ScrollUp (void *v)
  {
    OnScrollUp.emit (m_ScrollUnit, 1);

    if (AtMaximum() )
      RecvEndScrollUp (0, 0, 0, 0);
    else
      m_UpTimerHandler = GetTimer().AddTimerHandler (10, up_callback, this);

    NeedRedraw();
  }

  void VScrollBar::TrackUp (void *v)
  {
    if (m_TrackMouseCoord.y < m_SlideBar->GetBaseY() - m_Track->GetBaseY() )
    {
      OnScrollUp.emit (m_containerHeight, 1);
      m_TrackUpTimerHandler  = GetTimer().AddTimerHandler (10, trackup_callback, this);
      NeedRedraw();
    }
  }

  void VScrollBar::TrackDown (void *v)
  {
    if (m_TrackMouseCoord.y > m_SlideBar->GetBaseY() + m_SlideBar->GetBaseHeight() - m_Track->GetBaseY() )
    {
      OnScrollDown.emit (m_containerHeight, 1);
      m_TrackDownTimerHandler  = GetTimer().AddTimerHandler (10, trackdown_callback, this);
      NeedRedraw();
    }
  }

  void VScrollBar::RecvStartScrollUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (!AtMinimum() )
      ScrollUp (this);
  }

  void VScrollBar::RecvEndScrollUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_UpTimerHandler.IsValid() )
    {
      GetTimer().RemoveTimerHandler (m_UpTimerHandler);
      m_UpTimerHandler = 0;
    }
  }

  void VScrollBar::RecvStartScrollDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (!AtMaximum() )
      ScrollDown (this);
  }

  void VScrollBar::RecvEndScrollDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_DownTimerHandler.IsValid() )
    {
      GetTimer().RemoveTimerHandler (m_DownTimerHandler);
      m_DownTimerHandler = 0;
    }
  }

  void VScrollBar::RecvTrackMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    m_TrackMouseCoord = Point (x, y);

    int Y = m_SlideBar->GetBaseY() - m_Track->GetBaseY();

    if (y < Y)
    {
      // move the slide bar up
      TrackUp (this);
    }
    else
    {
      TrackDown (this);
      // move the slide bar down
    }
  }

  void VScrollBar::RecvTrackMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_TrackUpTimerHandler.IsValid() )
      GetTimer().RemoveTimerHandler (m_TrackUpTimerHandler);

    if (m_TrackDownTimerHandler.IsValid() )
      GetTimer().RemoveTimerHandler (m_TrackDownTimerHandler);

    m_TrackUpTimerHandler = 0;
    m_TrackDownTimerHandler = 0;
  }

  void VScrollBar::RecvTrackMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {

  }

  long VScrollBar::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret;
    ret = m_BottomThumb->OnEvent (ievent, TraverseInfo, ProcessEventInfo);
    ret = m_TopThumb->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_SlideBar->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_Track->OnEvent (ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void VScrollBar::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    GetPainter().PaintBackground (GfxContext, base);

    base.OffsetPosition (0, VSCROLLBAR_HEIGHT);
    base.OffsetSize (0, -2 * VSCROLLBAR_HEIGHT);
    GetPainter().PaintShape (GfxContext, base,
                         Color (COLOR_SCROLLBAR_TRACK), eVSCROLLBAR, false);

    GetPainter().PaintShape (GfxContext, m_TopThumb->GetGeometry(), Color (0xFFFFFFFF), eSCROLLBAR_TRIANGLE_UP);
    GetPainter().PaintShape (GfxContext, m_BottomThumb->GetGeometry(), Color (0xFFFFFFFF), eSCROLLBAR_TRIANGLE_DOWN);

    GetPainter().PaintShape (GfxContext, m_SlideBar->GetGeometry(),
                         Color (0.2156 * m_color_factor, 0.2156 * m_color_factor, 0.2156 * m_color_factor, 1.0f),
                         eVSCROLLBAR, true);
  }

  void VScrollBar::SetContainerSize (int x, int y, int w, int h)
  {
    m_containerWidth = w;
    m_containerHeight = h;
    ComputeScrolling();
  }

  void VScrollBar::SetContentSize (int x, int y, int w, int h)
  {
    // x and y are not needed
    m_contentWidth = w;
    m_contentHeight = h;
    ComputeScrolling();
  }

  void VScrollBar::SetContentOffset (float dx, float dy)
  {
    m_contentOffsetX = dx;
    m_contentOffsetY = dy;
    ComputeScrolling();
  }

  void VScrollBar::ComputeScrolling()
  {
    float percentage = 0;

    if (m_contentHeight == 0)
    {
      percentage = 100.0f;
    }
    else
    {
      percentage = 100.0f * (float) m_containerHeight / (float) m_contentHeight;
    }

    if (percentage > 100.0f)
    {
      percentage = 100.0f;
    }

    if (percentage < 0.0f)
    {
      percentage = 0.0f;
    }

    m_TrackHeight = m_Track->GetBaseHeight();

    int slider_width = m_TopThumb->GetBaseWidth();
    int slider_height = m_TrackHeight * percentage / 100.0f;

    if (slider_height < 15)
    {
      slider_height = 15;
    }

    m_SlideBar->SetBaseWidth (slider_width);
    m_SlideBar->SetBaseHeight (slider_height);
    m_SlideBar->SetBaseX (m_TopThumb->GetBaseX() );

    float pct;

    if (m_contentHeight - m_containerHeight > 0)
      pct = - (float) m_contentOffsetY / (float) (m_contentHeight - m_containerHeight);
    else
      pct = 0;

    int y = m_Track->GetBaseY() + pct * (m_TrackHeight - slider_height);
    m_SlideBar->SetBaseY (y);
  }

/////////////////
//  RECEIVERS  //
/////////////////
  void VScrollBar::SetValue (float value)
  {
    //m_ValueString.setCaption(value);
  }
  void VScrollBar::SetParameterName (const char *parameter_name)
  {
    //m_ParameterName.setCaption(parameter_name);
  }

////////////////
//  EMITTERS  //
////////////////
//void VScrollBar::EmitScrollUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
//{
//    OnScrollUp.emit(m_ScrollUnit, 1);
//}
//void VScrollBar::EmitScrollDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
//{
//    OnScrollDown.emit(m_ScrollUnit, 1);
//}

  void VScrollBar::OnSliderMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    m_SliderDragPositionX = x;
    m_SliderDragPositionY = y;
    //sigVScrollBarSliderMouseDown.emit();
    b_MouseDownTimer = true;
    b_MouseUpTimer = false;
    GetTimer().AddTimerHandler (10, callback, this);
  }

  void VScrollBar::OnSliderMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    b_MouseDownTimer = false;
    b_MouseUpTimer = true;
    GetTimer().AddTimerHandler (10, callback, this);
  }

  void VScrollBar::OnSliderMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_Track->GetBaseHeight() - m_SlideBar->GetBaseHeight() > 0)
    {
      stepY = (float) (m_contentHeight - m_containerHeight) / (float) (m_Track->GetBaseHeight() - m_SlideBar->GetBaseHeight() );
    }
    else
    {
      return;
    }

    if ( (dy > 0) && (y > m_SliderDragPositionY) )
    {
      OnScrollDown.emit (stepY, y - m_SliderDragPositionY);
    }

    if ( (dy < 0) && (y < m_SliderDragPositionY) )
    {
      OnScrollUp.emit (stepY, m_SliderDragPositionY - y);
    }

    NeedRedraw();
  }

  bool VScrollBar::AtMaximum()
  {
    if (m_SlideBar->GetBaseY() + m_SlideBar->GetBaseHeight() == m_Track->GetBaseY() + m_Track->GetBaseHeight() )
      return TRUE;

    return FALSE;
  }

  bool VScrollBar::AtMinimum()
  {
    if (m_SlideBar->GetBaseY() == m_Track->GetBaseY() )
      return TRUE;

    return FALSE;
  }

  long VScrollBar::PostLayoutManagement (long LayoutResult)
  {
    long ret = ScrollBar::PostLayoutManagement (LayoutResult);
    ComputeScrolling();
    return ret;
  }


}
