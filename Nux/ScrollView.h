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


#ifndef SCROLLVIEW_H
#define SCROLLVIEW_H

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
    virtual bool SetLayout(Layout *layout);

    /////////////////
    //  EMITERS    //
    /////////////////
    void OnSizeGrigMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnSizeGrigMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void EmitMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    //void EmitInternalResize(int x, int y, int w, int h);


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

    // Geometry of the layout that encompass the child layouts.
    //! X Position of the content
    int m_ViewContentX;
    //! Y Position of the content
    int m_ViewContentY;
    //! Width of content
    int m_ViewContentWidth;
    //! Height of content
    int m_ViewContentHeight;

    //Client View Area
    //! X position of the scroll view content area
    int m_ViewX;
    //! Y position of the scroll view content area
    int m_ViewY;
    //! Width of the scroll view content area
    int m_ViewWidth;
    //! Height of the scroll view content area
    int m_ViewHeight;

    Geometry m_ViewGeometry;
    Geometry m_ContentGeometry;

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

    //! Change Vertical Scrollbar in the ScrollView.
    /*!
        For styling purpose, allow the classes that inherit from ScrollView to
        change the vertical scrollbar.
    */
    void SetVScrollBar(VScrollBar* newVScrollBar);

    // Backup texture to speed up scrolling
    ObjectPtr<IOpenGLFrameBufferObject> m_FrameBufferObject;

    void SwapTextureIndex()
    {
      m_TextureIndex = (m_TextureIndex == 0) ? 1 : 0;
    }
    void SetTextureIndex(int index)
    {
      m_TextureIndex = index;
    }
    int GetTextureIndex()
    {
      return m_TextureIndex;
    }
    int m_TextureIndex;
    bool m_ReformatTexture;

    // ScrollBars
    HScrollBar     *_hscrollbar;
    VScrollBar     *_vscrollbar;
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
  };
}

#endif // SCROLLVIEW_H
