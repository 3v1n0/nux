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


#ifndef VSCROLLBAR_H
#define VSCROLLBAR_H

#include "TimerProc.h"
#include "ScrollBar.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class HLayout;
class VLayout;
class Layout;

class VScrollBar: public ScrollBar //public ValuatorAbstraction
{
public:
    VScrollBar(NUX_FILE_LINE_PROTO);
    ~VScrollBar();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    void DrawDownTriangle(GraphicsContext& GfxContext, int width, int height, const Geometry& geo, BasePainter& painter);
    void DrawUpTriangle(GraphicsContext& GfxContext, int width, int height, const Geometry& geo, BasePainter& painter);

    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
private:
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw) {};
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw) {};

    void VScrollBarHandler(void* v);
    void ScrollDown(void* v);
    void ScrollUp(void* v);
    void TrackUp(void* v);
    void TrackDown(void* v);
public:
    void SetContainerSize(int x, int y, int w, int h);
    void SetContentSize(int x, int y, int w, int h);
    void SetContentOffset(float dx, float dy);
    void ComputeScrolling();

    virtual int GetBaseWidth();
    virtual int GetBaseHeight();

    /////////////////
    //  RECEIVERS  //
    /////////////////
    void SetValue(float value);
    void SetParameterName(const char* parameter_name);
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

    smptr(VLayout) vlayout;
    smptr(CoreArea) m_SlideBar;
    smptr(CoreArea) m_TopThumb;
    smptr(CoreArea) m_BottomThumb;
    smptr(CoreArea) m_Track;

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
    TimerFunctor *up_callback;
    TimerFunctor *down_callback;
    TimerFunctor *trackup_callback;
    TimerFunctor *trackdown_callback;
    TimerHandle m_UpTimerHandler;
    TimerHandle m_DownTimerHandler;
    TimerHandle m_TrackUpTimerHandler;
    TimerHandle m_TrackDownTimerHandler;

    Point m_TrackMouseCoord;
    friend class HLayout;
    friend class VLayout;
    friend class Layout;
};


} //NUX_NAMESPACE_END

#endif // HSCROLLBAR_H


