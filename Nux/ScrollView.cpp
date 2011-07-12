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
#include "HScrollBar.h"
#include "VScrollBar.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "ScrollView.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(ScrollView);

  ScrollView::ScrollView (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    m_vertical_scrollbar_enable     = true;
    m_horizontal_scrollbar_enable   = true;
    m_bSizeMatchContent             = false;
    m_TextureIndex                  = 0;
    m_ReformatTexture               = true;

    _delta_x                        = 0;
    _delta_y                        = 0;

    //GetPainter().CreateBackgroundTexture(m_BackgroundTexture);
    _hscrollbar = new HScrollBar (NUX_TRACKER_LOCATION);
    _vscrollbar = new VScrollBar (NUX_TRACKER_LOCATION);
    // _hscrollbar and _vscrollbar have to be parented so they are correctly
    // rendered and so that GetRootGeometry/GetAbsoluteGeometry returns the
    // correct Geometry. This is necessary in embedded mode.
    _hscrollbar->SetParentObject (this);
    _vscrollbar->SetParentObject (this);

    SetMinimumSize (30, 30);
    SetGeometry (Geometry (0, 0, 400, 200) );

    _hscrollbar->OnScrollLeft.connect ( sigc::mem_fun (this, &ScrollView::ScrollLeft) );
    _hscrollbar->OnScrollRight.connect ( sigc::mem_fun (this, &ScrollView::ScrollRight) );
    _vscrollbar->OnScrollUp.connect ( sigc::mem_fun (this, &ScrollView::ScrollUp) );
    _vscrollbar->OnScrollDown.connect ( sigc::mem_fun (this, &ScrollView::ScrollDown) );

    OnMouseWheel.connect(sigc::mem_fun(this, &ScrollView::RecvMouseWheel));

    setTopBorder (4);
    setBorder (4);

    m_MouseWheelScrollSize = 32;

    m_ViewContentLeftMargin      = 0;
    m_ViewContentRightMargin     = 0;
    m_ViewContentTopMargin       = 0;
    m_ViewContentBottomMargin    = 0;
    FormatContent();

    ChildFocusChanged.connect (sigc::mem_fun (this, &ScrollView::OnChildFocusChanged));

    SetAcceptMouseWheelEvent(true);
  }

  // customization for Unity
  void ScrollView::SetVScrollBar (VScrollBar* newVScrollBar)
  {
    if(_vscrollbar)
    {
      // disconnect old _vscrollbar
      _vscrollbar->OnScrollUp.connect (sigc::mem_fun (this,
                                                     &ScrollView::ScrollUp));
      _vscrollbar->OnScrollDown.connect (sigc::mem_fun (this,
                                                       &ScrollView::ScrollDown));
      _vscrollbar->UnReference ();
    }

    _vscrollbar = newVScrollBar;

    _vscrollbar->Reference ();
    _vscrollbar->SetParentObject (this);

    // connect new _vscrollbar
    _vscrollbar->OnScrollUp.connect (sigc::mem_fun (this,
                                                   &ScrollView::ScrollUp));
    _vscrollbar->OnScrollDown.connect (sigc::mem_fun (this,
                                                     &ScrollView::ScrollDown));
  }

  ScrollView::~ScrollView()
  {
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
    _hscrollbar->UnReference ();
    _vscrollbar->UnReference ();
  }

  void ScrollView::OnChildFocusChanged (Area *parent, Area *child)
  {
    if (child->IsView ())
    {
      View *view = (View*)child;
      if (view->HasPassiveFocus ())
      {
        return;
      }
    }
    if (child->IsLayout ())
      return;
    
    int child_y = child->GetGeometry ().y - GetGeometry ().y;
    int child_y_diff = child_y - abs (_delta_y);

    
    if (child_y_diff + child->GetGeometry ().height < GetGeometry ().height && child_y_diff >= 0)
    {
      return;
    }
    
    if (child_y_diff < 0)
    {
      ScrollUp (1, abs (child_y_diff));
    }
    else
    {
      int size = child_y_diff - GetGeometry ().height;

      // always keeps the top of a view on the screen
      size += (child->GetGeometry ().height, GetGeometry ().height) ? child->GetGeometry ().height : GetGeometry ().height;
      
      ScrollDown (1, size);
    }

  }

  long ScrollView::ProcessEvent (Event &event, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    long ProcEvInfo = 0;

    if (event.e_event == NUX_MOUSE_PRESSED)
    {
      // Verify that the mouse down event falls inside the of the ScrollView.
      if (!GetGeometry ().IsPointInside (event.e_x - event.e_x_root, event.e_y - event.e_y_root))
      {
        ProcEvInfo = eDoNotProcess;
      }
    }

    if (event.e_event == NUX_MOUSE_WHEEL)
    {
      // nux can't tell the difference between horizontal and vertical mouse wheel events
      // so we are only going to support vertical
      if (event.e_wheeldelta > 0)
      {
        ScrollDown (abs (event.e_wheeldelta / NUX_MOUSEWHEEL_DELTA), m_MouseWheelScrollSize);
      }
      else
      {
        ScrollUp (abs (event.e_wheeldelta / NUX_MOUSEWHEEL_DELTA), m_MouseWheelScrollSize);
      }
    }

    if (m_vertical_scrollbar_enable)
      ret = _vscrollbar->ProcessEvent (event, ret, ProcEvInfo);

    if (m_horizontal_scrollbar_enable)
      ret = _hscrollbar->ProcessEvent (event, ret, ProcEvInfo);

    // The child layout get the Mouse down button only if the MouseDown happened inside the client view Area
    Geometry viewGeometry = Geometry (m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight);
    bool traverse = true;

    if ((event.e_event == NUX_MOUSE_PRESSED) ||
     (event.e_event == NUX_MOUSE_RELEASED) ||
     (event.e_event == NUX_MOUSE_MOVE))
    {
      if (!viewGeometry.IsPointInside (event.e_x - event.e_x_root, event.e_y - event.e_y_root) )
      {
        ProcEvInfo = eDoNotProcess;
        traverse = false;
      }
    }

    if (m_CompositionLayout)
    {
      // The ScrollView layout position is fixed. The ScrollView keeps track of the delta offset in x and y of the layout it manages.
      // Modify the event to account for this offset;
      Event mod_event = event;
      ret = m_CompositionLayout->ProcessEvent (mod_event, ret, ProcEvInfo);
    }

    ret = PostProcessEvent2 (event, ret, 0);
    return ret;
  }

  Area* ScrollView::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    // Test if the mouse is inside the ScrollView.
    // The last parameter of TestMousePointerInclusion is a boolean used to test if the case
    // of mouse wheel events. If that boolean value is true, then TestMousePointerInclusion 
    // returns true only if the mouse pointer is over this area and the the area accepts
    // mouse wheel events (see Area::SetAcceptMouseWheelEvent)
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if(mouse_inside == false)
    {
      // The mouse pointer is not over this Area. return NULL.
      return NULL;
    }

    Area* found_area;

    // Recursively go over the ui element that are managed by this ScrollView and look
    // for the area that is below the mouse.

    // Test the vertical scrollbar
    found_area = _vscrollbar->FindAreaUnderMouse(mouse_position, event_type);
    NUX_RETURN_VALUE_IF_NOTNULL(found_area, found_area);

    // Test the horizontal scrollbar
    found_area = _vscrollbar->FindAreaUnderMouse(mouse_position, event_type);
    NUX_RETURN_VALUE_IF_NOTNULL(found_area, found_area);

    // If the code gets here, it means that no area has been found yet.
    // Test the layout of the ScrollView
    return View::FindAreaUnderMouse(mouse_position, event_type);
  }

  bool ScrollView::SetLayout (Layout *layout)
  {
    if(View::SetLayout(layout) == false)
    {
      return false;
    }

    FormatContent();

    return true;
  }

  void ScrollView::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry ());

    Geometry base = GetGeometry ();

    if (m_CompositionLayout)
      m_CompositionLayout->NeedRedraw();

    GetPainter().PaintBackground (GfxContext, base);

    if (m_vertical_scrollbar_enable)
    {
      _vscrollbar->NeedRedraw();
    }

    if (m_horizontal_scrollbar_enable)
    {
      _hscrollbar->NeedRedraw();
    }

    GfxContext.PopClippingRectangle();
  }

  void ScrollView::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry ());

    GfxContext.PushClippingRectangle (Rect (m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight));

    if (m_CompositionLayout)
    {
//       GfxContext.PushClipOffset (_delta_x, _delta_y);
//       GfxContext.PushClippingRectangle (m_CompositionLayout->GetGeometry ());
//       GfxContext.Push2DTranslationModelViewMatrix (_delta_x, _delta_y, 0.0f);
      m_CompositionLayout->ProcessDraw (GfxContext, force_draw);
//       GfxContext.PopModelViewMatrix ();
//       GfxContext.PopClippingRectangle ();
//       GfxContext.PopClipOffset ();
    }

    GfxContext.PopClippingRectangle();

    if (m_vertical_scrollbar_enable)
    {
      _vscrollbar->ProcessDraw (GfxContext, force_draw);
    }

    if (m_horizontal_scrollbar_enable)
    {
      _hscrollbar->ProcessDraw (GfxContext, force_draw);
    }

    GfxContext.PopClippingRectangle();
  }

  void ScrollView::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

/////////
// API //
/////////
  void ScrollView::EnableVerticalScrollBar (bool b)
  {
    m_vertical_scrollbar_enable = b;
    _delta_y = 0;
    ComputeChildLayout();
  }

  void ScrollView::EnableHorizontalScrollBar (bool b)
  {
    m_horizontal_scrollbar_enable = b;
    _delta_x = 0;
    ComputeChildLayout();
  }

///////////////////////
// Internal function //
///////////////////////
  void ScrollView::setBorder (int border)
  {
    m_border = border;
  }

  void ScrollView::setTopBorder (int top_border)
  {
    m_top_border = top_border;
  }

  int ScrollView::getBorder() const
  {
    return m_border;
  }

  int ScrollView::getTopBorder() const
  {
    return m_top_border;
  }

  void ScrollView::SetGeometry (const Geometry &geo)
  {
    Area::SetGeometry (geo);
    //ComputeChildLayout();
  }

  void ScrollView::FormatContent()
  {
    Geometry geo;
    geo = GetGeometry();

    ComputeChildLayout();
  }

  void ScrollView::PreLayoutManagement()
  {
    // Give the managed layout the same size and position as the Control.

    Geometry geo = GetGeometry();
    int ScrollBarWidth = _vscrollbar->GetBaseWidth();
    int ScrollBarHeight = _hscrollbar->GetBaseHeight();

    nuxAssertMsg (ScrollBarWidth > 0, TEXT ("[ScrollView::PreLayoutManagement] Invalid scrollbar width.") );
    nuxAssertMsg (ScrollBarHeight > 0, TEXT ("[ScrollView::PreLayoutManagement] Invalid scrollbar height.") );

    m_ViewX = GetBaseX() + m_border + m_ViewContentLeftMargin;
    m_ViewY = GetBaseY() + m_top_border + m_ViewContentTopMargin;

    if (m_vertical_scrollbar_enable == false)
      m_ViewWidth = GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    else
      m_ViewWidth = GetBaseWidth() - ScrollBarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;

    nuxAssertMsg (m_ViewWidth > 0, TEXT ("[ScrollView::PreLayoutManagement] Invalid view width.") );

    if (m_horizontal_scrollbar_enable == false)
      m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;
    else
      m_ViewHeight = GetBaseHeight() - ScrollBarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;

    nuxAssertMsg (m_ViewWidth > 0, TEXT ("[ScrollView::PreLayoutManagement] Invalid view height.") );

    if (m_ViewX + _delta_x +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute _delta_x so the end of the content match exactly the view right border position
      _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      nuxAssert (_delta_x <= 0);
    }

    if (m_ViewY + _delta_y + m_ViewContentHeight < m_ViewY + m_ViewHeight)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute _delta_y so the end of the content match exactly the view right border position
      _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      nuxAssert (_delta_y <= 0);
    }

    if (m_CompositionLayout)
    {
      // Set the composition layout to the size of the view area and offset it by (_delta_x, _delta_y)

      if (m_CompositionLayout->GetStretchFactor () != 0)
      {
        m_CompositionLayout->SetGeometry (
                m_ViewX,
                m_ViewY,
                m_ViewWidth,
                m_ViewHeight);
      }

      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      if (m_vertical_scrollbar_enable == false)
      {
        // If there is no vertical scrollbar, take all the width available.
        _hscrollbar->SetBaseWidth (GetBaseWidth() - 2 * m_border);
      }
      else
        _hscrollbar->SetBaseWidth (GetBaseWidth() - ScrollBarWidth - 2 * m_border);

      _hscrollbar->SetBaseX (geo.x + m_border);
      _hscrollbar->SetBaseY (geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeChildLayout();
    }
    else
    {
      // The horizontal scrollbar won't be visible but give it a proper size anyway.
      _hscrollbar->SetBaseWidth (GetBaseWidth() - ScrollBarWidth - 2 * m_border);
      _hscrollbar->SetBaseX (geo.x + m_border);
      _hscrollbar->SetBaseY (geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeChildLayout();
    }


    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      if (m_horizontal_scrollbar_enable == false)
      {
        // If there is no horizontal scrollbar, take all the width available.
        _vscrollbar->SetBaseHeight (GetBaseHeight() - m_top_border - m_border);
      }
      else
        _vscrollbar->SetBaseHeight (GetBaseHeight() - ScrollBarHeight - m_top_border - m_border);

      _vscrollbar->SetBaseX (geo.x + geo.GetWidth() - ScrollBarWidth - m_border);
      _vscrollbar->SetBaseY (geo.y + m_top_border);
      _vscrollbar->ComputeChildLayout();
    }
    else
    {
      // The vertical scrollbar won't be visible but give it a proper size anyway.
      _vscrollbar->SetBaseHeight (GetBaseHeight() - ScrollBarHeight - m_top_border - m_border);
      _vscrollbar->SetBaseX (geo.x + geo.GetWidth() - ScrollBarWidth - m_border);
      _vscrollbar->SetBaseY (geo.y + m_top_border);
      _vscrollbar->ComputeChildLayout();
    }
  }

  long ScrollView::PostLayoutManagement (long LayoutResult)
  {
    if (IsSizeMatchContent ())
      return PostLayoutManagement2 (LayoutResult);

    int ScrollBarWidth = 0;
    int ScrollBarHeight = 0;

    if (m_CompositionLayout)
    {
      m_ViewContentX = m_CompositionLayout->GetBaseX();
      m_ViewContentY = m_CompositionLayout->GetBaseY();
      m_ViewContentWidth = m_CompositionLayout->GetBaseWidth();
      m_ViewContentHeight = m_CompositionLayout->GetBaseHeight();
    }

    if (m_horizontal_scrollbar_enable)
      ScrollBarHeight = _hscrollbar->GetBaseHeight();

    if (m_vertical_scrollbar_enable)
      ScrollBarWidth = _vscrollbar->GetBaseWidth();

    _hscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                  GetBaseWidth() - ScrollBarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                  GetBaseHeight() - ScrollBarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

    if (m_horizontal_scrollbar_enable)
    {
      if (m_CompositionLayout)
      {
        _hscrollbar->SetContentSize (m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                                    m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight() );
      }
      else
      {
        _hscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      _hscrollbar->SetContentOffset (_delta_x, _delta_y);
    }
    else
    {
      _hscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      _hscrollbar->SetContentOffset (0, 0);
    }

    _vscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                  GetBaseWidth() - ScrollBarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                  GetBaseHeight() - ScrollBarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

    if (m_vertical_scrollbar_enable)
    {
      if (m_CompositionLayout)
      {
        _vscrollbar->SetContentSize (m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                                    m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight() );
      }
      else
      {
        _vscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      _vscrollbar->SetContentOffset (_delta_x, _delta_y);
    }
    else
    {
      _vscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      _vscrollbar->SetContentOffset (0, 0);
    }

    // I may not be necessary to call this function here since ComputePosition2 was called on ComputePosition2
    // during the layout process.
    if (m_CompositionLayout)
      m_CompositionLayout->ComputePosition2 (0, 0);

    // The ScrollView always returns complient width and height to its parent layout.
    return (eCompliantHeight | eCompliantWidth);
  }

  long ScrollView::PostLayoutManagement2 (long LayoutResult)
  {
    // In case IsSizeMatchContent returns True, The scroll view is resized to match its content.
    int ScrollbarWidth = 0;
    int ScrollbarHeight = 0;

    if (m_horizontal_scrollbar_enable)
      ScrollbarHeight = _hscrollbar->GetBaseHeight();

    if (m_vertical_scrollbar_enable)
      ScrollbarWidth = _vscrollbar->GetBaseWidth();

    // We want the controller to match the size of the content as defined in:
    //      m_ViewContentX
    //      m_ViewContentY
    //      m_ViewContentWidth
    //      m_ViewContentHeight
    // So we make the composition layout the same size as the content
    // Note that classes that inherits from ScrollView are responsible for setting the dimension of the ViewContent

    if (m_CompositionLayout)
    {
      m_CompositionLayout->SetBaseX (m_ViewContentX);
      m_CompositionLayout->SetBaseY (m_ViewContentY);
      m_CompositionLayout->SetBaseWidth (m_ViewContentWidth);
      m_CompositionLayout->SetBaseHeight (m_ViewContentHeight);
    }

    Geometry base;
    // Given the (m_ViewContentWidth, m_ViewContentHeight) compute the size of the ScrollView.
    // It is possible that the ScrollView size be limited by its min/Max dimension. If this happens, then the scrollbar will reflect that.
    base.SetX (m_ViewContentX - m_border - m_ViewContentLeftMargin);
    base.SetY (m_ViewContentY - m_top_border - m_ViewContentTopMargin);

    if (m_horizontal_scrollbar_enable)
      base.SetHeight (m_top_border + m_ViewContentTopMargin + m_ViewContentHeight + m_ViewContentBottomMargin + ScrollbarHeight + m_border);
    else
      base.SetHeight (m_top_border + m_ViewContentTopMargin + m_ViewContentHeight + m_ViewContentBottomMargin + m_border);

    if (m_vertical_scrollbar_enable)
      base.SetWidth (m_border + m_ViewContentLeftMargin + m_ViewContentWidth + m_ViewContentRightMargin + ScrollbarWidth + m_border);
    else
      base.SetWidth (m_border + m_ViewContentLeftMargin + m_ViewContentWidth + m_ViewContentRightMargin + m_border);

    // Set the size so that is is equal to the visible content.
    Area::SetBaseWidth (base.GetWidth() );
    Area::SetBaseHeight (base.GetHeight() );
    Geometry geo = GetGeometry();

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      if (m_vertical_scrollbar_enable == false)
        _hscrollbar->SetBaseWidth (GetBaseWidth() - 2 * m_border);
      else
        _hscrollbar->SetBaseWidth (GetBaseWidth() - ScrollbarWidth - 2 * m_border);

      _hscrollbar->SetBaseX (geo.x + m_border);
      _hscrollbar->SetBaseY (geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeChildLayout();

      //---
      _hscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                    GetBaseWidth() - ScrollbarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

      if (m_CompositionLayout)
      {
        _hscrollbar->SetContentSize (m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                                    m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight() );
      }
      else
      {
        _hscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      _hscrollbar->SetContentOffset (_delta_x, _delta_y);
    }
    else
    {
      _hscrollbar->SetBaseWidth (GetBaseWidth() - ScrollbarWidth - 2 * m_border);
      _hscrollbar->SetBaseX (geo.x + m_border);
      _hscrollbar->SetBaseY (geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeChildLayout();

      //---
      _hscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                    GetBaseWidth() - ScrollbarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
      _hscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      _hscrollbar->SetContentOffset (0, 0);
    }


    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      if (m_horizontal_scrollbar_enable == false)
        _vscrollbar->SetBaseHeight (GetBaseHeight() - m_top_border - m_border);
      else
        _vscrollbar->SetBaseHeight (GetBaseHeight() - ScrollbarHeight - m_top_border - m_border);

      _vscrollbar->SetBaseX (geo.x + geo.GetWidth() - ScrollbarWidth - m_border);
      _vscrollbar->SetBaseY (geo.y + m_top_border);
      _vscrollbar->ComputeChildLayout();

      //---
      _vscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                    GetBaseWidth() - ScrollbarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

      if (m_CompositionLayout)
      {
        _vscrollbar->SetContentSize (m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                                    m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight() );
      }
      else
      {
        _vscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      _vscrollbar->SetContentOffset (_delta_x, _delta_y);
    }
    else
    {
      _vscrollbar->SetBaseHeight (GetBaseHeight() - ScrollbarHeight - m_top_border - m_border);
      _vscrollbar->SetBaseX (geo.x + geo.GetWidth() - ScrollbarWidth - m_border);
      _vscrollbar->SetBaseY (geo.y + m_top_border);
      _vscrollbar->ComputeChildLayout();

      //---
      _vscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                    GetBaseWidth() - ScrollbarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
      _vscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      _vscrollbar->SetContentOffset (0, 0);
    }

    if (m_CompositionLayout)
      m_CompositionLayout->ComputePosition2 (0, 0);

    return (eCompliantHeight | eCompliantWidth);
  }

// When the ScrollView is in a Layout object, and that layout call View::PositionChildLayout
// the ScrollView must call its own PositionChildLayout so it can properly do the positioning of the inner object.
// Otherwise, m_CompositionLayout->ComputePosition2 is called but it doesn't know that it may not contain all the
// object of the ScrollView. Which result in incorrect positioning.
// Here we touch only the position. Do not touch the width or height of object.
// This function is called when the ScrollView is embedded within a Layout.
  void ScrollView::PositionChildLayout (float offsetX, float offsetY)
  {
    Geometry geo = GetGeometry();
    int w = 0;
    int h = 0;

    w = _vscrollbar->GetBaseWidth();
    h = _hscrollbar->GetBaseHeight();

//    m_ViewX = GetX() + m_border;
//    m_ViewY = GetY() + m_top_border;
//
//    if(m_vertical_scrollbar_enable == false)
//        m_ViewWidth = GetWidth() - 2*m_border - m_ViewContentRightMargin;
//    else
//        m_ViewWidth = GetWidth() - w - 2*m_border - m_ViewContentRightMargin;
//
//    if(m_horizontal_scrollbar_enable == false)
//        m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin;
//    else
//        m_ViewHeight = GetBaseHeight() - h - m_top_border - m_border - m_ViewContentBottomMargin;

    m_ViewX = GetBaseX() + m_border + m_ViewContentLeftMargin;
    m_ViewY = GetBaseY() + m_top_border + m_ViewContentTopMargin;

    if (m_vertical_scrollbar_enable == false)
      m_ViewWidth = GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    else
      m_ViewWidth = GetBaseWidth() - w - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;

    if (m_horizontal_scrollbar_enable == false)
      m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;
    else
      m_ViewHeight = GetBaseHeight() - h - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;


    if (m_ViewX + _delta_x +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
    {
      _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
    }

    if (m_ViewY + _delta_y + m_ViewContentHeight < m_ViewY + m_ViewHeight)
    {
      _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
    }

    if (m_CompositionLayout)
    {
      m_CompositionLayout->SetBaseX (m_ViewX);
      m_CompositionLayout->SetBaseY (m_ViewY);
    }

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      if (m_vertical_scrollbar_enable == false)
        _hscrollbar->SetBaseWidth (GetBaseWidth() - 2 * m_border);
      else
        _hscrollbar->SetBaseWidth (GetBaseWidth() - w - 2 * m_border);

      _hscrollbar->SetBaseX (geo.x + m_border);
      _hscrollbar->SetBaseY (geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeChildLayout();
    }

    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      if (m_horizontal_scrollbar_enable == false)
        _vscrollbar->SetBaseHeight (GetBaseHeight() - m_top_border - m_border);
      else
        _vscrollbar->SetBaseHeight (GetBaseHeight() - h - m_top_border - m_border);

      _vscrollbar->SetBaseX (geo.x + geo.GetWidth() - w - m_border);
      _vscrollbar->SetBaseY (geo.y + m_top_border);
      _vscrollbar->ComputeChildLayout();
    }

    if (m_CompositionLayout)
    {
      m_ViewContentX = m_CompositionLayout->GetBaseX();
      m_ViewContentY = m_CompositionLayout->GetBaseY();
    }
    else
    {
      m_ViewContentX = m_ViewX;
      m_ViewContentY = m_ViewY;
    }

    int hor_scrollbar_height = 0;
    int ver_scrollbar_width = 0;

    if (m_horizontal_scrollbar_enable == true)
      hor_scrollbar_height = _hscrollbar->GetBaseHeight();

    if (m_vertical_scrollbar_enable == true)
      ver_scrollbar_width = _vscrollbar->GetBaseWidth();

    _vscrollbar->SetContentOffset (_delta_x, _delta_y);
    _hscrollbar->SetContentOffset (_delta_x, _delta_y);

    if (m_CompositionLayout)
    {
      m_CompositionLayout->ComputePosition2 (0, 0);
    }
  }


  void ScrollView::ScrollLeft (float stepx, int mousedx)
  {
    if (m_CompositionLayout)
    {
      _delta_x += (float) stepx * (float) mousedx;;

      if (_delta_x > 0)
      {
        _delta_x = 0;
      }
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    if (m_CompositionLayout)
    {
      _hscrollbar->SetContentOffset (_delta_x, _delta_y);
      _hscrollbar->NeedRedraw();
    }

    NeedRedraw();
  }

  void ScrollView::ScrollRight (float stepx, int mousedx)
  {
    if (m_CompositionLayout)
    {
      _delta_x -= (float) stepx * (float) mousedx;

      if (m_ViewX + _delta_x +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
      {
        _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      }
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    if (m_CompositionLayout)
    {
      _hscrollbar->SetContentOffset (_delta_x, _delta_y);
      _hscrollbar->NeedRedraw();
    }

    NeedRedraw();
  }

  void ScrollView::ScrollUp (float stepy, int mousedy)
  {
    if (m_CompositionLayout)
    {
      _delta_y += (float) stepy * (float) mousedy;

      if (_delta_y > 0)
      {
        _delta_y = 0;
      }
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    if (m_CompositionLayout)
    {
      _vscrollbar->SetContentOffset (_delta_x, _delta_y);
      _vscrollbar->NeedRedraw();
    }

    NeedRedraw();
  }

  void ScrollView::ScrollDown (float stepy, int mousedy)
  {
    if (m_CompositionLayout)
    {
      _delta_y -= (float) stepy * (float) mousedy;

      if (m_ViewY + _delta_y + m_ViewContentHeight < m_ViewY + m_ViewHeight)
      {
        _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      }
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    if (m_CompositionLayout)
    {
      _vscrollbar->SetContentOffset (_delta_x, _delta_y);
      _vscrollbar->NeedRedraw();
    }

    NeedRedraw();
  }

  void ScrollView::SetSizeMatchContent (bool b)
  {
    m_bSizeMatchContent = b;

    if (m_CompositionLayout)
      m_CompositionLayout->ComputeLayout2();
  }

  bool ScrollView::IsSizeMatchContent() const
  {
    return m_bSizeMatchContent;
  }

  void ScrollView::ResetScrollToLeft()
  {
    _delta_x = 0;
    if (m_CompositionLayout)
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);

    _hscrollbar->SetContentOffset (_delta_x, _delta_y);
    _hscrollbar->NeedRedraw();

    NeedRedraw();
  }

  void ScrollView::ResetScrollToRight()
  {
    if (m_CompositionLayout)
    {
      _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }
    else
    {
      _delta_x = 0;
    }



    _hscrollbar->SetContentOffset (_delta_x, _delta_y);
    _hscrollbar->NeedRedraw();

    NeedRedraw();
  }

  void ScrollView::ResetScrollToUp()
  {
    _delta_y = 0;
    if (m_CompositionLayout)
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);

    _vscrollbar->SetContentOffset (_delta_x, _delta_y);
    _vscrollbar->NeedRedraw();

    NeedRedraw();
  }

  void ScrollView::ResetScrollToDown()
  {
    if (m_CompositionLayout)
    {
      _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }
    else
    {
      _delta_y = 0;
    }

    _vscrollbar->SetContentOffset (_delta_x, _delta_y);
    _vscrollbar->NeedRedraw();

    NeedRedraw();
  }

  void ScrollView::RecvMouseWheel(int x, int y, int wheel_delta,  long button_flags, unsigned long key_flags)
  {
    // nux can't tell the difference between horizontal and vertical mouse wheel events
    // so we are only going to support vertical
    if (wheel_delta < 0)
    {
      ScrollDown (abs (wheel_delta / NUX_MOUSEWHEEL_DELTA), m_MouseWheelScrollSize);
    }
    else
    {
      ScrollUp (abs (wheel_delta / NUX_MOUSEWHEEL_DELTA), m_MouseWheelScrollSize);
    }
  }

  bool ScrollView::AcceptKeyNavFocus()
  {
    return false;
  }
}
