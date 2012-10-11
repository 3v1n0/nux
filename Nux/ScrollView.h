// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2010-2011 Inalogic® Inc.
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

#ifndef NUX_SCROLLVIEW_H
#define NUX_SCROLLVIEW_H

#include "Nux.h"
#include "View.h"

namespace nux
{
  class HScrollBar;
  class VScrollBar;

// Rename it to ScrollArea or ScrollWindow
  class ScrollView: public View
  {
    NUX_DECLARE_OBJECT_TYPE(ScrollView, View);
  public:
    ScrollView(NUX_FILE_LINE_PROTO);
    virtual ~ScrollView();

    // API
    void EnableVerticalScrollBar(bool b);
    void EnableHorizontalScrollBar(bool b);

    /////////////////
    //  RECEIVERS  //
    /////////////////
    virtual void ScrollLeft(float stepx, int mousedx);
    virtual void ScrollRight(float stepx, int mousedx);
    virtual void ScrollUp(float stepy, int mousedy);
    virtual void ScrollDown(float stepy, int mousedy);

    virtual void ResetScrollToLeft();
    virtual void ResetScrollToRight();
    virtual void ResetScrollToUp();
    virtual void ResetScrollToDown();

    // amount to scroll by for each mouse wheel event
    int m_MouseWheelScrollSize;

    //Client View Area
    Geometry view_geo_;

    // Geometry of the layout that encompass the child layouts.
    Geometry content_geo_;

    // signals
    /*!
        Emitted when scrolling happens. the parameters to the signals are:
        int: value of the horizontal translation of the layout.
        int: value of the vertical translation of the layout.
        This signal is emitted only if the scroll view has a layout.
    */
    sigc::signal<void, int, int> scrolling;

  public:
    void    SetViewContentLeftMargin(int margin)
    {
      m_ViewContentLeftMargin = margin;
    }
    int     GetViewContentLeftMargin() const
    {
      return m_ViewContentLeftMargin;
    }
    void    SetViewContentRightMargin(int margin)
    {
      m_ViewContentRightMargin = margin;
    }
    int     GetViewContentRightMargin() const
    {
      return m_ViewContentRightMargin;
    }
    void    SetViewContentTopMargin(int margin)
    {
      m_ViewContentTopMargin = margin;
    }
    int     GetViewContentTopMargin() const
    {
      return m_ViewContentTopMargin;
    }
    void    SetViewContentBottomMargin(int margin)
    {
      m_ViewContentBottomMargin = margin;
    }
    int     GetViewContentBottomMargin() const
    {
      return m_ViewContentBottomMargin;
    }

  protected:

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);
    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);

    void RecvMouseWheel(int x, int y, int wheel_delta,  long button_flags, unsigned long key_flags);

    //! Change Horizontal Scrollbar in the ScrollView.
    /*!
        For styling purpose, allow the classes that inherit from ScrollView to
        change the horizontal scrollbar.
    */
    void SetHScrollBar(HScrollBar* hscrollbar);

    //! Change Vertical Scrollbar in the ScrollView.
    /*!
        For styling purpose, allow the classes that inherit from ScrollView to
        change the vertical scrollbar.
    */
    void SetVScrollBar(VScrollBar* vscrollbar);

    // ScrollBars
    ObjectPtr<HScrollBar> _hscrollbar;
    ObjectPtr<VScrollBar> _vscrollbar;
    bool m_horizontal_scrollbar_enable;
    bool m_vertical_scrollbar_enable;

    int m_top_border;
    int m_border;

    //! Horizontal scrollbar offsets.
    int _delta_x;

    //! Vertical scrollbar offsets.
    int _delta_y;

    void FormatContent();
    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement(long LayoutResult);
    virtual void ComputeContentPosition(float offsetX, float offsetY);

  private:

    virtual bool AcceptKeyNavFocus();
    /**
        If True, the scrollbar size will be adjusted to match the size of the content.
        This is useful for the ComboBoxComplex widget.
    */
    bool m_bSizeMatchContent;

    int m_ViewContentLeftMargin;
    int m_ViewContentRightMargin;
    int m_ViewContentTopMargin;
    int m_ViewContentBottomMargin;

    sigc::connection scroll_up_connection_;
    sigc::connection scroll_down_connection_;
    sigc::connection vmouse_whell_connection_;
    sigc::connection scroll_left_connection_;
    sigc::connection scroll_right_connection_;
    sigc::connection hmouse_whell_connection_;
  };
}

#endif
