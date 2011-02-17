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


#ifndef HSCROLLBAR_H
#define HSCROLLBAR_H

#include "TimerProc.h"
#include "ScrollBar.h"

namespace nux
{

  class HLayout;
  class VLayout;
  class Layout;

  class HScrollBar : public ScrollBar //public ValuatorAbstraction
  {
  public:
    HScrollBar (NUX_FILE_LINE_PROTO);
    ~HScrollBar();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    void DrawLeftTriangle (GraphicsEngine &GfxContext, int width, int height, const Geometry &geo, BasePainter &painter);
    void DrawRightTriangle (GraphicsEngine &GfxContext, int width, int height, const Geometry &geo, BasePainter &painter);

    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
  private:
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw) {};
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw) {};

    void HScrollBarHandler (void *v);
    void ScrollRight (void *v);
    void ScrollLeft (void *v);
    void TrackLeft (void *v);
    void TrackRight (void *v);

  public:
    void SetContainerSize (int x, int y, int w, int h);
    void SetContentSize (int x, int y, int w, int h);
    void SetContentOffset (float dx, float dy);
    void ComputeScrolling();

    /////////////////
    //  RECEIVERS  //
    /////////////////
    void SetValue (float value);
    void SetParameterName (const char *parameter_name);
    ////////////////
    //  EMITTERS  //
    ////////////////
    void RecvStartScrollRight (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvStartScrollLeft (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvEndScrollRight (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvEndScrollLeft (int x, int y, unsigned long button_flags, unsigned long key_flags);

    void RecvTrackMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTrackMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTrackMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void OnSliderMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnSliderMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnSliderMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    ///////////////
    //  SIGNALS  //
    ///////////////
    sigc::signal<void> sigClick;
    sigc::signal<void, float, int> OnScrollLeft;
    sigc::signal<void, float, int> OnScrollRight;
    sigc::signal<void> sigHScrollBarSliderMouseDown;

    bool b_MouseDownTimer;
    bool b_MouseUpTimer;
    float m_color_factor;

  protected:
    // When the Scrollbar is used standalone, it is necessary to call ComputeScrolling at the end of the layout.
    virtual long PostLayoutManagement (long LayoutResult);

    bool AtMinimum();
    bool AtMaximum();

    HLayout *hlayout;
    InputArea *m_SlideBar;
    InputArea *m_LeftThumb;
    InputArea *m_RightThumb;
    InputArea *m_Track;

    int m_contentWidth;
    int m_contentHeight;
    float m_contentOffsetX;
    float m_contentOffsetY;

    int m_containerWidth;
    int m_containerHeight;

    int m_TrackWidth;
    int m_TrackHeight;

    int m_SlideBarOffsetX;
    int m_SlideBarOffsetY;

    float stepX;
    float stepY;

    int m_SliderDragPositionX;
    int m_SliderDragPositionY;

    TimerFunctor *callback;
    TimerFunctor *left_callback;
    TimerFunctor *right_callback;
    TimerFunctor *trackleft_callback;
    TimerFunctor *trackright_callback;
    TimerHandle m_LeftTimerHandler;
    TimerHandle m_RightTimerHandler;
    TimerHandle m_TrackLeftTimerHandler;
    TimerHandle m_TrackRightTimerHandler;

    Point m_TrackMouseCoord;
    friend class HLayout;
    friend class VLayout;
    friend class Layout;
  };

}

#endif // HSCROLLBAR_H


