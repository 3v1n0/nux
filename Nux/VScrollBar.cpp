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
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "VScrollBar.h"

namespace nux
{

  const int VSCROLLBAR_WIDTH = 5;
  const int VSCROLLBAR_HEIGHT = 10;

  VScrollBar::VScrollBar(NUX_FILE_LINE_DECL)
    :   ScrollBar(NUX_FILE_LINE_PARAM)
  {
    content_width_      = 0;
    content_height_     = 0;
    container_width_    = 0;
    container_height_   = 0;
    m_TrackWidth        = 0;
    m_TrackHeight       = 0;
    m_SlideBarOffsetX   = 0;
    m_SlideBarOffsetY   = 0;
    content_offset_x_    = 0;
    content_offset_y_    = 0;
    b_MouseUpTimer      = false;
    b_MouseDownTimer    = false;
    m_color_factor      = 1.0f;
    m_UpTimerHandler    = 0;
    m_DownTimerHandler  = 0;

    vlayout         = new VLayout(NUX_TRACKER_LOCATION);
    _scroll_up_button      = new BasicView(NUX_TRACKER_LOCATION);
    _track         = new BasicView(NUX_TRACKER_LOCATION);
    _scroll_down_button   = new BasicView(NUX_TRACKER_LOCATION);
    _slider      = new BasicView(NUX_TRACKER_LOCATION);
    _slider->SetParentObject(this);

    // Set Original State
    SetMinimumSize(VSCROLLBAR_WIDTH, AREA_MIN_HEIGHT);
    SetMaximumSize(VSCROLLBAR_WIDTH, AREA_MAX_HEIGHT);

    // Set Signals
    //_scroll_down_button->mouse_down.connect(sigc::mem_fun(this, &VScrollBar::RecvStartScrollDown));
    //_scroll_down_button->mouse_up.connect(sigc::mem_fun(this, &VScrollBar::RecvEndScrollDown));
    //_scroll_up_button->mouse_down.connect(sigc::mem_fun(this, &VScrollBar::RecvStartScrollUp));
    //_scroll_up_button->mouse_up.connect(sigc::mem_fun(this, &VScrollBar::RecvEndScrollUp));

    _slider->mouse_down.connect(sigc::mem_fun(this, &VScrollBar::OnSliderMouseDown));
    _slider->mouse_up.connect(sigc::mem_fun(this, &VScrollBar::OnSliderMouseUp));
    _slider->mouse_drag.connect(sigc::mem_fun(this, &VScrollBar::OnSliderMouseDrag));

    _track->mouse_down.connect(sigc::mem_fun(this, &VScrollBar::RecvTrackMouseDown));
    _track->mouse_up.connect(sigc::mem_fun(this, &VScrollBar::RecvTrackMouseUp));
    _track->mouse_drag.connect(sigc::mem_fun(this, &VScrollBar::RecvTrackMouseDrag));

    //_track->mouse_down.connect(sigc::mem_fun(this, &VScrollBar::OnSliderMouseDown));

    // Set Geometry
    _scroll_down_button->SetMinimumSize(VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
    _scroll_down_button->SetMaximumSize(VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
    _scroll_down_button->SetGeometry(Geometry(0, 0, VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT));
    _scroll_up_button->SetMinimumSize(VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
    _scroll_up_button->SetMaximumSize(VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
    _scroll_up_button->SetGeometry(Geometry(0, 0, VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT));

    vlayout->AddView(_scroll_up_button, 0, eCenter, eFix);
    vlayout->AddView(_track, 1, eCenter, eFull);
    vlayout->AddView(_scroll_down_button, 0, eCenter, eFix);

    callback = new TimerFunctor;
    callback->tick.connect(sigc::mem_fun(this, &VScrollBar::VScrollBarHandler));
    up_callback = new TimerFunctor;
    up_callback->tick.connect(sigc::mem_fun(this, &VScrollBar::ScrollUp));
    down_callback = new TimerFunctor;
    down_callback->tick.connect(sigc::mem_fun(this, &VScrollBar::ScrollDown));
    trackup_callback = new TimerFunctor;
    trackup_callback->tick.connect(sigc::mem_fun(this, &VScrollBar::TrackUp));
    trackdown_callback = new TimerFunctor;
    trackdown_callback->tick.connect(sigc::mem_fun(this, &VScrollBar::TrackDown));

    SetLayout(vlayout);
    SetAcceptMouseWheelEvent(true);
  }


  VScrollBar::~VScrollBar()
  {
    _slider->UnReference();
    delete callback;
    delete up_callback;
    delete trackup_callback;
    delete down_callback;
    delete trackdown_callback;
  }

  void VScrollBar::VScrollBarHandler(void *v)
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
        scrollbar->QueueDraw();
        GetTimer().AddOneShotTimer(10, callback, scrollbar);
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
        scrollbar->QueueDraw();
        GetTimer().AddOneShotTimer(10, callback, scrollbar);
      }
    }

    QueueDraw();
  }

  void VScrollBar::ScrollDown(void * /* v */)
  {
    OnScrollDown.emit(m_ScrollUnit, 1);

    if (AtMaximum())
      RecvEndScrollDown(0, 0, 0, 0);
    else
      m_DownTimerHandler = GetTimer().AddOneShotTimer(10, down_callback, this);

    QueueDraw();
  }

  void VScrollBar::ScrollUp(void * /* v */)
  {
    OnScrollUp.emit(m_ScrollUnit, 1);

    if (AtMaximum())
      RecvEndScrollUp(0, 0, 0, 0);
    else
      m_UpTimerHandler = GetTimer().AddOneShotTimer(10, up_callback, this);

    QueueDraw();
  }

  void VScrollBar::TrackUp(void * /* v */)
  {
    if (m_TrackMouseCoord.y < _slider->GetBaseY() - _track->GetBaseY())
    {
      OnScrollUp.emit(container_height_, 1);
      m_TrackUpTimerHandler  = GetTimer().AddOneShotTimer(10, trackup_callback, this);
      QueueDraw();
    }
  }

  void VScrollBar::TrackDown(void * /* v */)
  {
    if (m_TrackMouseCoord.y > _slider->GetBaseY() + _slider->GetBaseHeight() - _track->GetBaseY())
    {
      OnScrollDown.emit(container_height_, 1);
      m_TrackDownTimerHandler  = GetTimer().AddOneShotTimer(10, trackdown_callback, this);
      QueueDraw();
    }
  }

  void VScrollBar::RecvStartScrollUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (!AtMinimum())
      ScrollUp(this);
  }

  void VScrollBar::RecvEndScrollUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_UpTimerHandler.IsValid())
    {
      GetTimer().RemoveTimerHandler(m_UpTimerHandler);
      m_UpTimerHandler = 0;
    }
  }

  void VScrollBar::RecvStartScrollDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (!AtMaximum())
      ScrollDown(this);
  }

  void VScrollBar::RecvEndScrollDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_DownTimerHandler.IsValid())
    {
      GetTimer().RemoveTimerHandler(m_DownTimerHandler);
      m_DownTimerHandler = 0;
    }
  }

  void VScrollBar::RecvTrackMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    m_TrackMouseCoord = Point(x, y);

    int Y = _slider->GetBaseY() - _track->GetBaseY();

    if (y < Y)
    {
      // move the slide bar up
      TrackUp(this);
    }
    else
    {
      TrackDown(this);
      // move the slide bar down
    }
  }

  void VScrollBar::RecvTrackMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_TrackUpTimerHandler.IsValid())
      GetTimer().RemoveTimerHandler(m_TrackUpTimerHandler);

    if (m_TrackDownTimerHandler.IsValid())
      GetTimer().RemoveTimerHandler(m_TrackDownTimerHandler);

    m_TrackUpTimerHandler = 0;
    m_TrackDownTimerHandler = 0;
  }

  void VScrollBar::RecvTrackMouseDrag(int /* x */, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {

  }

  Area* VScrollBar::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    NUX_RETURN_VALUE_IF_TRUE(_scroll_down_button->TestMousePointerInclusion(mouse_position, event_type), _scroll_down_button);
    NUX_RETURN_VALUE_IF_TRUE(_scroll_up_button->TestMousePointerInclusion(mouse_position, event_type), _scroll_up_button);
    NUX_RETURN_VALUE_IF_TRUE(_slider->TestMousePointerInclusion(mouse_position, event_type), _slider);
    NUX_RETURN_VALUE_IF_TRUE(_track->TestMousePointerInclusion(mouse_position, event_type), _track);

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  void VScrollBar::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    GetPainter().PaintBackground(graphics_engine, base);

    base.OffsetPosition(0, VSCROLLBAR_HEIGHT);
    base.OffsetSize(0, -2 * VSCROLLBAR_HEIGHT);
    //GetPainter().PaintShape(graphics_engine, base, Color(COLOR_SCROLLBAR_TRACK), eVSCROLLBAR, false);
    //graphics_engine.QRP_Color(base.x, base.y, base.width, base.height, Color(COLOR_SCROLLBAR_TRACK));

    //GetPainter().PaintShape(graphics_engine, _scroll_up_button->GetGeometry(), Color(0xFFFFFFFF), eSCROLLBAR_TRIANGLE_UP);
    //GetPainter().PaintShape(graphics_engine, _scroll_down_button->GetGeometry(), Color(0xFFFFFFFF), eSCROLLBAR_TRIANGLE_DOWN);

    /*GetPainter().PaintShape(graphics_engine, _slider->GetGeometry(),
                         Color(0.2156 * m_color_factor, 0.2156 * m_color_factor, 0.2156 * m_color_factor, 1.0f),
                         eVSCROLLBAR, true);*/

    if (content_height_ > container_height_)
    {
      Geometry slider_geo = _slider->GetGeometry();
      graphics_engine.QRP_Color(slider_geo.x, slider_geo.y, slider_geo.width, slider_geo.height,
          Color(1.0f, 1.0f, 1.0f, 0.8f));
    }
      //Color(0.2156 * m_color_factor, 0.2156 * m_color_factor, 0.2156 * m_color_factor, 1.0f));
  }

  void VScrollBar::SetContainerSize(int /* x */, int /* y */, int w, int h)
  {
    container_width_ = w;
    container_height_ = h;
    ComputeScrolling();
  }

  void VScrollBar::SetContentSize(int /* x */, int /* y */, int w, int h)
  {
    // x and y are not needed
    content_width_ = w;
    content_height_ = h;
    ComputeScrolling();
  }

  void VScrollBar::SetContentOffset(float dx, float dy)
  {
    content_offset_x_ = dx;
    content_offset_y_ = dy;
    ComputeScrolling();
  }

  void VScrollBar::ComputeScrolling()
  {
    if (content_height_ == 0)
    {
      visibility_percentage_ = 100.0f;
    }
    else
    {
      visibility_percentage_ = Clamp<float>(100.0f * (float) container_height_ / (float) content_height_, 0.0f, 100.0f);
    }

    m_TrackHeight = _track->GetBaseHeight();

    int slider_width = _scroll_up_button->GetBaseWidth();
    int slider_height = m_TrackHeight * visibility_percentage_ / 100.0f;

    if (slider_height < 15)
    {
      slider_height = 15;
    }

    _slider->SetBaseWidth(slider_width);
    _slider->SetBaseHeight(slider_height);
    _slider->SetBaseX(_scroll_up_button->GetBaseX());

    float pct;

    if (content_height_ - container_height_ > 0)
      pct = - (float) content_offset_y_ / (float) (content_height_ - container_height_);
    else
      pct = 0;

    int y = _track->GetBaseY() + pct * (m_TrackHeight - slider_height);
    _slider->SetBaseY(y);
  }

/////////////////
//  RECEIVERS  //
/////////////////
  void VScrollBar::SetValue(float /* value */)
  {
    //m_ValueString.setCaption(value);
  }
  void VScrollBar::SetParameterName(const char * /* parameter_name */)
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

  void VScrollBar::OnSliderMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    m_SliderDragPositionX = x;
    m_SliderDragPositionY = y;
    //sigVScrollBarSliderMouseDown.emit();
    b_MouseDownTimer = true;
    b_MouseUpTimer = false;
    GetTimer().AddOneShotTimer(10, callback, this);
  }

  void VScrollBar::OnSliderMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    b_MouseDownTimer = false;
    b_MouseUpTimer = true;
    GetTimer().AddOneShotTimer(10, callback, this);
  }

  void VScrollBar::OnSliderMouseDrag(int /* x */, int y, int /* dx */, int dy, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (_track->GetBaseHeight() - _slider->GetBaseHeight() > 0)
    {
      stepY = (float) (content_height_ - container_height_) / (float) (_track->GetBaseHeight() - _slider->GetBaseHeight());
    }
    else
    {
      return;
    }

    if ((dy > 0) && (y > m_SliderDragPositionY))
    {
      OnScrollDown.emit(stepY, y - m_SliderDragPositionY);
    }

    if ((dy < 0) && (y < m_SliderDragPositionY))
    {
      OnScrollUp.emit(stepY, m_SliderDragPositionY - y);
    }

    QueueDraw();
  }

  bool VScrollBar::AtMaximum()
  {
    if (_slider->GetBaseY() + _slider->GetBaseHeight() == _track->GetBaseY() + _track->GetBaseHeight())
      return TRUE;

    return FALSE;
  }

  bool VScrollBar::AtMinimum()
  {
    if (_slider->GetBaseY() == _track->GetBaseY())
      return TRUE;

    return FALSE;
  }

  long VScrollBar::PostLayoutManagement(long LayoutResult)
  {
    long ret = ScrollBar::PostLayoutManagement(LayoutResult);
    ComputeScrolling();
    return ret;
  }


}
