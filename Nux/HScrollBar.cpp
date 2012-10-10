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
#include "HScrollBar.h"

namespace nux
{

  const int HSCROLLBAR_WIDTH = 10;
  const int HSCROLLBAR_HEIGHT = 10;

  HScrollBar::HScrollBar(NUX_FILE_LINE_DECL)
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
    m_LeftTimerHandler  = 0;
    m_RightTimerHandler = 0;

    hlayout = new HLayout(NUX_TRACKER_LOCATION);
    _scroll_left_button = new BasicView(NUX_TRACKER_LOCATION);
    _track = new BasicView(NUX_TRACKER_LOCATION);
    _scroll_right_button = new BasicView(NUX_TRACKER_LOCATION);
    _slider = new BasicView(NUX_TRACKER_LOCATION);
    _slider->SetParentObject(this);

    // Set Original State
    SetMinimumSize(AREA_MIN_WIDTH, HSCROLLBAR_HEIGHT);
    SetMaximumSize(AREA_MAX_WIDTH, HSCROLLBAR_HEIGHT);

    // Set Signals
    _scroll_right_button->mouse_down.connect(sigc::mem_fun(this, &HScrollBar::RecvStartScrollRight));
    _scroll_right_button->mouse_up.connect(sigc::mem_fun(this, &HScrollBar::RecvEndScrollRight));
    _scroll_left_button->mouse_down.connect(sigc::mem_fun(this, &HScrollBar::RecvStartScrollLeft));
    _scroll_left_button->mouse_up.connect(sigc::mem_fun(this, &HScrollBar::RecvEndScrollLeft));

    _slider->mouse_down.connect(sigc::mem_fun(this, &HScrollBar::OnSliderMouseDown));
    _slider->mouse_up.connect(sigc::mem_fun(this, &HScrollBar::OnSliderMouseUp));
    _slider->mouse_drag.connect(sigc::mem_fun(this, &HScrollBar::OnSliderMouseDrag));

    _track->mouse_down.connect(sigc::mem_fun(this, &HScrollBar::RecvTrackMouseDown));
    _track->mouse_up.connect(sigc::mem_fun(this, &HScrollBar::RecvTrackMouseUp));
    _track->mouse_drag.connect(sigc::mem_fun(this, &HScrollBar::RecvTrackMouseDrag));

    // Set Geometry
    _scroll_right_button->SetMinimumSize(HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
    _scroll_right_button->SetMaximumSize(HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
    _scroll_right_button->SetGeometry(Geometry(0, 0, HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT));
    _scroll_left_button->SetMinimumSize(HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
    _scroll_left_button->SetMaximumSize(HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
    _scroll_left_button->SetGeometry(Geometry(0, 0, HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT));


    hlayout->AddView(_scroll_left_button, 0, eCenter, eFix);
    hlayout->AddView(_track, 1, eCenter, eFull);
    hlayout->AddView(_scroll_right_button, 0, eCenter, eFix);

    callback = new TimerFunctor;
    callback->tick.connect(sigc::mem_fun(this, &HScrollBar::HScrollBarHandler));
    left_callback = new TimerFunctor;
    left_callback->tick.connect(sigc::mem_fun(this, &HScrollBar::ScrollLeft));
    right_callback = new TimerFunctor;
    right_callback->tick.connect(sigc::mem_fun(this, &HScrollBar::ScrollRight));
    trackleft_callback = new TimerFunctor;
    trackleft_callback->tick.connect(sigc::mem_fun(this, &HScrollBar::TrackLeft));
    trackright_callback = new TimerFunctor;
    trackright_callback->tick.connect(sigc::mem_fun(this, &HScrollBar::TrackRight));

    SetLayout(hlayout);
    SetAcceptMouseWheelEvent(true);
  }

  HScrollBar::~HScrollBar()
  {
    _slider->UnReference();
    delete callback;
    delete left_callback;
    delete trackleft_callback;
    delete right_callback;
    delete trackright_callback;
  }

  void HScrollBar::HScrollBarHandler(void *v)
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
  }

  void HScrollBar::ScrollRight(void * /* v */)
  {
    OnScrollRight.emit(m_ScrollUnit, 1);

    if (AtMaximum())
      RecvEndScrollRight(0, 0, 0, 0);
    else
      m_RightTimerHandler = GetTimer().AddOneShotTimer(10, right_callback, this);

    QueueDraw();
  }

  void HScrollBar::ScrollLeft(void * /* v */)
  {
    OnScrollLeft.emit(m_ScrollUnit, 1);

    if (AtMaximum())
      RecvEndScrollLeft(0, 0, 0, 0);
    else
      m_LeftTimerHandler = GetTimer().AddOneShotTimer(10, left_callback, this);

    QueueDraw();
  }

  void HScrollBar::TrackLeft(void * /* v */)
  {
    if (m_TrackMouseCoord.x < _slider->GetBaseX() - _track->GetBaseX())
    {
      OnScrollLeft.emit(container_width_, 1);
      m_TrackLeftTimerHandler  = GetTimer().AddOneShotTimer(10, trackleft_callback, this);
      QueueDraw();
    }
  }

  void HScrollBar::TrackRight(void * /* v */)
  {
    if (m_TrackMouseCoord.x > _slider->GetBaseX() + _slider->GetBaseWidth() - _track->GetBaseX())
    {
      OnScrollRight.emit(container_width_, 1);
      m_TrackRightTimerHandler  = GetTimer().AddOneShotTimer(10, trackright_callback, this);
      QueueDraw();
    }
  }

  void HScrollBar::RecvStartScrollLeft(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (!AtMinimum())
      ScrollLeft(this);
  }

  void HScrollBar::RecvStartScrollRight(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (!AtMaximum())
      ScrollRight(this);
  }

  void HScrollBar::RecvEndScrollRight(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_RightTimerHandler.IsValid())
      GetTimer().RemoveTimerHandler(m_RightTimerHandler);

    m_RightTimerHandler = 0;
  }

  void HScrollBar::RecvEndScrollLeft(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_LeftTimerHandler.IsValid())
      GetTimer().RemoveTimerHandler(m_LeftTimerHandler);

    m_LeftTimerHandler = 0;
  }

  void HScrollBar::RecvTrackMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    m_TrackMouseCoord = Point(x, y);

    int X = _slider->GetBaseX() - _track->GetBaseX();

    if (x < X)
    {
      // move the slide bar up
      TrackLeft(this);
    }
    else
    {
      TrackRight(this);
      // move the slide bar down
    }
  }

  void HScrollBar::RecvTrackMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_TrackLeftTimerHandler.IsValid())
      GetTimer().RemoveTimerHandler(m_TrackLeftTimerHandler);

    if (m_TrackRightTimerHandler.IsValid())
      GetTimer().RemoveTimerHandler(m_TrackRightTimerHandler);

    m_TrackLeftTimerHandler = 0;
    m_TrackRightTimerHandler = 0;
  }

  void HScrollBar::RecvTrackMouseDrag(int /* x */, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {

  }

  Area* HScrollBar::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    NUX_RETURN_VALUE_IF_TRUE(_scroll_right_button->TestMousePointerInclusion(mouse_position, event_type), _scroll_right_button);
    NUX_RETURN_VALUE_IF_TRUE(_scroll_left_button->TestMousePointerInclusion(mouse_position, event_type), _scroll_left_button);
    NUX_RETURN_VALUE_IF_TRUE(_slider->TestMousePointerInclusion(mouse_position, event_type), _slider);
    NUX_RETURN_VALUE_IF_TRUE(_track->TestMousePointerInclusion(mouse_position, event_type), _track);

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  void HScrollBar::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    GetPainter().PaintBackground(graphics_engine, base);
    base.OffsetPosition(HSCROLLBAR_WIDTH, 0);
    base.OffsetSize(-2 * HSCROLLBAR_WIDTH, 0);
    GetPainter().PaintShape(graphics_engine, base,
                         Color(COLOR_SCROLLBAR_TRACK), eHSCROLLBAR, false);

    GetPainter().PaintShape(graphics_engine, _scroll_left_button->GetGeometry(), Color(0xFFFFFFFF), eSCROLLBAR_TRIANGLE_LEFT);
    GetPainter().PaintShape(graphics_engine, _scroll_right_button->GetGeometry(), Color(0xFFFFFFFF), eSCROLLBAR_TRIANGLE_RIGHT);

    GetPainter().PaintShape(graphics_engine, _slider->GetGeometry(),
                         Color(0.2156 * m_color_factor, 0.2156 * m_color_factor, 0.2156 * m_color_factor, 1.0f),
                         eHSCROLLBAR, true);
  };

  void HScrollBar::SetContainerSize(int /* x */, int /* y */, int w, int h)
  {
    // x and y are not needed
    container_width_ = w;
    container_height_ = h;
    ComputeScrolling();
  }

  void HScrollBar::SetContentSize(int /* x */, int /* y */, int w, int h)
  {
    // x and y are not needed
    content_width_ = w;
    content_height_ = h;
    ComputeScrolling();
  }

  void HScrollBar::SetContentOffset(float dx, float dy)
  {
    content_offset_x_ = dx;
    content_offset_y_ = dy;
    ComputeScrolling();
  }

  void HScrollBar::ComputeScrolling()
  {
    if (content_width_ == 0)
    {
      visibility_percentage_ = 100.0f;
    }
    else
    {
      visibility_percentage_ = Clamp<float>(100.0f * (float) container_width_ / (float) content_width_, 0.0f, 100.0f);
    }

    m_TrackWidth = _track->GetBaseWidth();

    int slider_height = _scroll_left_button->GetBaseHeight();
    int slider_width = m_TrackWidth * visibility_percentage_ / 100.0f;

    if (slider_width < 15)
    {
      slider_width = 15;
    }

    _slider->SetBaseWidth(slider_width);
    _slider->SetBaseHeight(slider_height);
    _slider->SetBaseY(_scroll_left_button->GetBaseY());


    float pct;

    if (content_width_ - container_width_ > 0)
      pct = - (float) content_offset_x_ / (float) (content_width_ - container_width_);
    else
      pct = 0;

    int x = _track->GetBaseX() + pct * (m_TrackWidth - slider_width);
    _slider->SetBaseX(x);
  }


/////////////////
//  RECEIVERS  //
/////////////////
  void HScrollBar::SetValue(float /* value */)
  {
    //m_ValueString.setCaption(value);
  }

  void HScrollBar::SetParameterName(const char * /* parameter_name */)
  {
    //m_ParameterName.setCaption(parameter_name);
  }

////////////////
//  EMITTERS  //
////////////////

  void HScrollBar::OnSliderMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    m_SliderDragPositionX = x;
    m_SliderDragPositionY = y;
    //sigVScrollBarSliderMouseDown.emit();
    b_MouseDownTimer = true;
    b_MouseUpTimer = false;
    GetTimer().AddOneShotTimer(10, callback, this);
  }

  void HScrollBar::OnSliderMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    b_MouseDownTimer = false;
    b_MouseUpTimer = true;
    GetTimer().AddOneShotTimer(10, callback, this);
  }

  void HScrollBar::OnSliderMouseDrag(int x, int /* y */, int dx, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (_track->GetBaseWidth() - _slider->GetBaseWidth() > 0)
    {
      stepX = (float) (content_width_ - container_width_) / (float) (_track->GetBaseWidth() - _slider->GetBaseWidth());
    }
    else
    {
      return;
    }

    if ((dx > 0) && (x > m_SliderDragPositionX))
    {
      OnScrollRight.emit(stepX, x - m_SliderDragPositionX);
    }

    if ((dx < 0) && (x < m_SliderDragPositionX))
    {
      OnScrollLeft.emit(stepX, m_SliderDragPositionX - x);
    }
  }

  bool HScrollBar::AtMaximum()
  {
    if (_slider->GetBaseX() + _slider->GetBaseWidth() == _track->GetBaseX() + _track->GetBaseWidth())
      return TRUE;

    return FALSE;
  }

  bool HScrollBar::AtMinimum()
  {
    if (_slider->GetBaseX() == _track->GetBaseX())
      return TRUE;

    return FALSE;
  }

  long HScrollBar::PostLayoutManagement(long LayoutResult)
  {
    long ret = ScrollBar::PostLayoutManagement(LayoutResult);
    ComputeScrolling();
    return ret;
  }

}
