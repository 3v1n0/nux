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


#include "Nux.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "HScrollBar.h"

namespace nux
{

  const int HSCROLLBAR_WIDTH = 10;
  const int HSCROLLBAR_HEIGHT = 10;

  HScrollBar::HScrollBar (NUX_FILE_LINE_DECL)
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
    m_LeftTimerHandler  = 0;
    m_RightTimerHandler = 0;

    hlayout = new HLayout (TEXT (""), NUX_TRACKER_LOCATION);
    m_LeftThumb = new CoreArea (NUX_TRACKER_LOCATION);
    m_Track = new CoreArea (NUX_TRACKER_LOCATION);
    m_RightThumb = new CoreArea (NUX_TRACKER_LOCATION);
    m_SlideBar = new CoreArea (NUX_TRACKER_LOCATION);

    // Set Original State
    SetMinimumSize (BASEOBJECT_MINWIDTH, HSCROLLBAR_HEIGHT);
    SetMaximumSize (BASEOBJECT_MAXWIDTH, HSCROLLBAR_HEIGHT);
    // Set Signals
    m_RightThumb->OnMouseDown.connect ( sigc::mem_fun (this, &HScrollBar::RecvStartScrollRight) );
    m_RightThumb->OnMouseUp.connect ( sigc::mem_fun (this, &HScrollBar::RecvEndScrollRight) );
    m_LeftThumb->OnMouseDown.connect ( sigc::mem_fun (this, &HScrollBar::RecvStartScrollLeft) );
    m_LeftThumb->OnMouseUp.connect ( sigc::mem_fun (this, &HScrollBar::RecvEndScrollLeft) );

    m_SlideBar->OnMouseDown.connect ( sigc::mem_fun (this, &HScrollBar::OnSliderMouseDown) );
    m_SlideBar->OnMouseUp.connect ( sigc::mem_fun (this, &HScrollBar::OnSliderMouseUp) );
    m_SlideBar->OnMouseDrag.connect ( sigc::mem_fun (this, &HScrollBar::OnSliderMouseDrag) );

    m_Track->OnMouseDown.connect ( sigc::mem_fun (this, &HScrollBar::RecvTrackMouseDown) );
    m_Track->OnMouseUp.connect ( sigc::mem_fun (this, &HScrollBar::RecvTrackMouseUp) );
    m_Track->OnMouseDrag.connect ( sigc::mem_fun (this, &HScrollBar::RecvTrackMouseDrag) );

    // Set Geometry
    m_RightThumb->SetMinimumSize (HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
    m_RightThumb->SetMaximumSize (HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
    m_RightThumb->SetGeometry (Geometry (0, 0, HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT) );
    m_LeftThumb->SetMinimumSize (HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
    m_LeftThumb->SetMaximumSize (HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
    m_LeftThumb->SetGeometry (Geometry (0, 0, HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT) );


    hlayout->AddView (m_LeftThumb, 0, eCenter, eFix);
    hlayout->AddView (m_Track, 1, eCenter, eFull);
    hlayout->AddView (m_RightThumb, 0, eCenter, eFix);

    callback = new TimerFunctor;
    callback->OnTimerExpired.connect (sigc::mem_fun (this, &HScrollBar::HScrollBarHandler) );
    left_callback = new TimerFunctor;
    left_callback->OnTimerExpired.connect (sigc::mem_fun (this, &HScrollBar::ScrollLeft) );
    right_callback = new TimerFunctor;
    right_callback->OnTimerExpired.connect (sigc::mem_fun (this, &HScrollBar::ScrollRight) );
    trackleft_callback = new TimerFunctor;
    trackleft_callback->OnTimerExpired.connect (sigc::mem_fun (this, &HScrollBar::TrackLeft) );
    trackright_callback = new TimerFunctor;
    trackright_callback->OnTimerExpired.connect (sigc::mem_fun (this, &HScrollBar::TrackRight) );

    SetCompositionLayout (hlayout);
  }

  HScrollBar::~HScrollBar()
  {
    delete callback;
    delete left_callback;
    delete trackleft_callback;
    delete right_callback;
  }

  void HScrollBar::HScrollBarHandler (void *v)
  {
    HScrollBar *scrollbar = static_cast<HScrollBar *> (v);

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
  }

  void HScrollBar::ScrollRight (void *v)
  {
    OnScrollRight.emit (m_ScrollUnit, 1);

    if (AtMaximum() )
      RecvEndScrollRight (0, 0, 0, 0);
    else
      m_RightTimerHandler = GetTimer().AddTimerHandler (10, right_callback, this);

    NeedRedraw();
  }

  void HScrollBar::ScrollLeft (void *v)
  {
    OnScrollLeft.emit (m_ScrollUnit, 1);

    if (AtMaximum() )
      RecvEndScrollLeft (0, 0, 0, 0);
    else
      m_LeftTimerHandler = GetTimer().AddTimerHandler (10, left_callback, this);

    NeedRedraw();
  }

  void HScrollBar::TrackLeft (void *v)
  {
    if (m_TrackMouseCoord.x < m_SlideBar->GetBaseX() - m_Track->GetBaseX() )
    {
      OnScrollLeft.emit (m_containerWidth, 1);
      m_TrackLeftTimerHandler  = GetTimer().AddTimerHandler (10, trackleft_callback, this);
      NeedRedraw();
    }
  }

  void HScrollBar::TrackRight (void *v)
  {
    if (m_TrackMouseCoord.x > m_SlideBar->GetBaseX() + m_SlideBar->GetBaseWidth() - m_Track->GetBaseX() )
    {
      OnScrollRight.emit (m_containerWidth, 1);
      m_TrackRightTimerHandler  = GetTimer().AddTimerHandler (10, trackright_callback, this);
      NeedRedraw();
    }
  }

  void HScrollBar::RecvStartScrollLeft (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (!AtMinimum() )
      ScrollLeft (this);
  }

  void HScrollBar::RecvStartScrollRight (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (!AtMaximum() )
      ScrollRight (this);
  }

  void HScrollBar::RecvEndScrollRight (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_RightTimerHandler.IsValid() )
      GetTimer().RemoveTimerHandler (m_RightTimerHandler);

    m_RightTimerHandler = 0;
  }

  void HScrollBar::RecvEndScrollLeft (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_LeftTimerHandler.IsValid() )
      GetTimer().RemoveTimerHandler (m_LeftTimerHandler);

    m_LeftTimerHandler = 0;
  }

  void HScrollBar::RecvTrackMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    m_TrackMouseCoord = Point (x, y);

    int X = m_SlideBar->GetBaseX() - m_Track->GetBaseX();

    if (x < X)
    {
      // move the slide bar up
      TrackLeft (this);
    }
    else
    {
      TrackRight (this);
      // move the slide bar down
    }
  }

  void HScrollBar::RecvTrackMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_TrackLeftTimerHandler.IsValid() )
      GetTimer().RemoveTimerHandler (m_TrackLeftTimerHandler);

    if (m_TrackRightTimerHandler.IsValid() )
      GetTimer().RemoveTimerHandler (m_TrackRightTimerHandler);

    m_TrackLeftTimerHandler = 0;
    m_TrackRightTimerHandler = 0;
  }

  void HScrollBar::RecvTrackMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {

  }

  long HScrollBar::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = m_RightThumb->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_LeftThumb->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_SlideBar->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_Track->OnEvent (ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);

    return ret;
  }

  void HScrollBar::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    GetPainter().PaintBackground (GfxContext, base);
    base.OffsetPosition (HSCROLLBAR_WIDTH, 0);
    base.OffsetSize (-2 * HSCROLLBAR_WIDTH, 0);
    GetPainter().PaintShape (GfxContext, base,
                         Color (COLOR_SCROLLBAR_TRACK), eHSCROLLBAR, false);

    GetPainter().PaintShape (GfxContext, m_LeftThumb->GetGeometry(), Color (0xFFFFFFFF), eSCROLLBAR_TRIANGLE_LEFT);
    GetPainter().PaintShape (GfxContext, m_RightThumb->GetGeometry(), Color (0xFFFFFFFF), eSCROLLBAR_TRIANGLE_RIGHT);

    GetPainter().PaintShape (GfxContext, m_SlideBar->GetGeometry(),
                         Color (0.2156 * m_color_factor, 0.2156 * m_color_factor, 0.2156 * m_color_factor, 1.0f),
                         eHSCROLLBAR, true);
  };

  void HScrollBar::SetContainerSize (int x, int y, int w, int h)
  {
    // x and y are not needed
    m_containerWidth = w;
    m_containerHeight = h;
    ComputeScrolling();
  }

  void HScrollBar::SetContentSize (int x, int y, int w, int h)
  {
    // x and y are not needed
    m_contentWidth = w;
    m_contentHeight = h;
    ComputeScrolling();
  }

  void HScrollBar::SetContentOffset (float dx, float dy)
  {
    m_contentOffsetX = dx;
    m_contentOffsetY = dy;
    ComputeScrolling();
  }

  void HScrollBar::ComputeScrolling()
  {
    float percentage = 0;

    if (m_contentWidth == 0)
    {
      percentage = 100.0f;
    }
    else
    {
      percentage = 100.0f * (float) m_containerWidth / (float) m_contentWidth;
    }

    if (percentage > 100.0f)
    {
      percentage = 100.0f;
    }

    if (percentage < 0.0f)
    {
      percentage = 0.0f;
    }

    m_TrackWidth = m_Track->GetBaseWidth();

    int slider_height = m_LeftThumb->GetBaseHeight();
    int slider_width = m_TrackWidth * percentage / 100.0f;

    if (slider_width < 15)
    {
      slider_width = 15;
    }

    m_SlideBar->SetBaseWidth (slider_width);
    m_SlideBar->SetBaseHeight (slider_height);
    m_SlideBar->SetBaseY (m_LeftThumb->GetBaseY() );


    float pct;

    if (m_contentWidth - m_containerWidth > 0)
      pct = - (float) m_contentOffsetX / (float) (m_contentWidth - m_containerWidth);
    else
      pct = 0;

    int x = m_Track->GetBaseX() + pct * (m_TrackWidth - slider_width);
    m_SlideBar->SetBaseX (x);
  }


/////////////////
//  RECEIVERS  //
/////////////////
  void HScrollBar::SetValue (float value)
  {
    //m_ValueString.setCaption(value);
  }

  void HScrollBar::SetParameterName (const char *parameter_name)
  {
    //m_ParameterName.setCaption(parameter_name);
  }

////////////////
//  EMITTERS  //
////////////////

  void HScrollBar::OnSliderMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    m_SliderDragPositionX = x;
    m_SliderDragPositionY = y;
    //sigVScrollBarSliderMouseDown.emit();
    b_MouseDownTimer = true;
    b_MouseUpTimer = false;
    GetTimer().AddTimerHandler (10, callback, this);
  }

  void HScrollBar::OnSliderMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    b_MouseDownTimer = false;
    b_MouseUpTimer = true;
    GetTimer().AddTimerHandler (10, callback, this);
  }

  void HScrollBar::OnSliderMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_Track->GetBaseWidth() - m_SlideBar->GetBaseWidth() > 0)
    {
      stepX = (float) (m_contentWidth - m_containerWidth) / (float) (m_Track->GetBaseWidth() - m_SlideBar->GetBaseWidth() );
    }
    else
    {
      return;
    }

    if ( (dx > 0) && (x > m_SliderDragPositionX) )
    {
      OnScrollRight.emit (stepX, x - m_SliderDragPositionX);
    }

    if ( (dx < 0) && (x < m_SliderDragPositionX) )
    {
      OnScrollLeft.emit (stepX, m_SliderDragPositionX - x);
    }
  }

  bool HScrollBar::AtMaximum()
  {
    if (m_SlideBar->GetBaseX() + m_SlideBar->GetBaseWidth() == m_Track->GetBaseX() + m_Track->GetBaseWidth() )
      return TRUE;

    return FALSE;
  }

  bool HScrollBar::AtMinimum()
  {
    if (m_SlideBar->GetBaseX() == m_Track->GetBaseX() )
      return TRUE;

    return FALSE;
  }

  long HScrollBar::PostLayoutManagement (long LayoutResult)
  {
    long ret = ScrollBar::PostLayoutManagement (LayoutResult);
    ComputeScrolling();
    return ret;
  }

}
