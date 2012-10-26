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


#ifndef VSCROLLBAR_H
#define VSCROLLBAR_H

#include "TimerProc.h"
#include "ScrollBar.h"

namespace nux
{
  class VLayout;

  class VScrollBar: public ScrollBar
  {
  public:
    VScrollBar(NUX_FILE_LINE_PROTO);
    ~VScrollBar();

    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);
    void DrawDownTriangle(GraphicsEngine &graphics_engine, int width, int height, const Geometry &geo, BasePainter &painter);
    void DrawUpTriangle(GraphicsEngine &graphics_engine, int width, int height, const Geometry &geo, BasePainter &painter);

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
  private:
    virtual void DrawContent(GraphicsEngine & /* graphics_engine */, bool /* force_draw */) {};

    void VScrollBarHandler(void *v);
    void ScrollDown(void *v);
    void ScrollUp(void *v);
    void TrackUp(void *v);
    void TrackDown(void *v);
  public:
    void SetContainerSize(int x, int y, int w, int h);
    void SetContentSize(int x, int y, int w, int h);
    void SetContentOffset(float dx, float dy);
    void ComputeScrolling();

    /////////////////
    //  RECEIVERS  //
    /////////////////
    void SetValue(float value);
    void SetParameterName(const char *parameter_name);
    ////////////////
    //  EMITTERS  //
    ////////////////
    void RecvStartScrollDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvStartScrollUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvEndScrollDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvEndScrollUp(int x, int y, unsigned long button_flags, unsigned long key_flags);

    void RecvTrackMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTrackMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTrackMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void OnSliderMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnSliderMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnSliderMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    ///////////////
    //  SIGNALS  //
    ///////////////
    sigc::signal<void> sigClick;
    sigc::signal<void, float, int> OnScrollUp;
    sigc::signal<void, float, int> OnScrollDown;
    sigc::signal<void> sigVScrollBarSliderMouseDown;
    

    bool b_MouseDownTimer;
    bool b_MouseUpTimer;
    float m_color_factor;

  protected:
    // When the Scrollbar is used standalone, it is necessary to call ComputeScrolling at the end of the layout.
    virtual long PostLayoutManagement(long LayoutResult);

    bool AtMinimum();
    bool AtMaximum();

    VLayout *vlayout;
    BasicView *_slider;
    BasicView *_scroll_up_button;
    BasicView *_scroll_down_button;
    BasicView *_track;

    int content_width_;
    int content_height_;
    float content_offset_x_;
    float content_offset_y_;

    int container_width_;
    int container_height_;

    int m_TrackWidth;
    int m_TrackHeight;

    int m_SlideBarOffsetX;
    int m_SlideBarOffsetY;

    float stepX;
    float stepY;

    int m_SliderDragPositionX;
    int m_SliderDragPositionY;

    TimerFunctor *callback;
    TimerFunctor *up_callback;
    TimerFunctor *down_callback;
    TimerFunctor *trackup_callback;
    TimerFunctor *trackdown_callback;
    TimerHandle m_UpTimerHandler;
    TimerHandle m_DownTimerHandler;
    TimerHandle m_TrackUpTimerHandler;
    TimerHandle m_TrackDownTimerHandler;

    Point m_TrackMouseCoord;
    friend class VLayout;
    friend class ScrollView;
  };


}

#endif // HSCROLLBAR_H


