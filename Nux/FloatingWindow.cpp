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

#include "NuxGraphics/GLTextureResourceManager.h"

#include "HLayout.h"
#include "WindowThread.h"
#include "WindowCompositor.h"
#include "FloatingWindow.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE (FloatingWindow);

  /*
      Elements inside the Window have coordinates based on the top-left corner of the window.
      This is true whether we are drawing or computing the layout. When computing the layout, use x_root and y_root to
      pass the top-left corner position of the window. When drawing, make a similar adjustment.
  */
  FloatingWindow::FloatingWindow (const TCHAR *WindowName, NUX_FILE_LINE_DECL)
    :   BaseWindow (WindowName, NUX_FILE_LINE_PARAM)
  {
    m_bIsVisible                = false;
    m_bSizeMatchLayout          = false;
    m_bIsModal                  = false;
    m_bIsVisibleSizeGrip        = true;
    m_SizeGripDragPositionX     = 0;
    m_SizeGripDragPositionY     = 0;
    m_hasTitleBar               = true;
    _resize_handle_width        = 20;
    _resize_handle_height       = 20;
    _title_bar_height           = 20;

    _minimize_button    = new InputArea(NUX_TRACKER_LOCATION);
    _minimize_button->Reference();
    _minimize_button->SetParentObject(this);

    _resize_handle      = new InputArea(NUX_TRACKER_LOCATION);
    _resize_handle->SinkReference();
    _resize_handle->SetParentObject(this);

    _title_bar          = new InputArea(NUX_TRACKER_LOCATION);
    _title_bar->SinkReference();
    _title_bar->SetParentObject(this);

    _close_button       = new InputArea(NUX_TRACKER_LOCATION);
    _window_title_bar   = new StaticTextBox (TEXT (""), NUX_TRACKER_LOCATION);

    _title_bar_layout   = new HLayout (NUX_TRACKER_LOCATION);
    _title_bar_layout->Reference();
    
    _minimize_button->SetMinMaxSize(20, 20);
    _minimize_button->SetGeometry(0, 0, 20, 20);
    _close_button->SetMinimumSize(20, 20);
    _close_button->SetGeometry(0, 0, 20, 20);
    _resize_handle->SetMinimumSize(_resize_handle_width, _resize_handle_height);
    _resize_handle->SetGeometry(Geometry(0, 0, _resize_handle_width, _resize_handle_height));

    _title_bar->mouse_down.connect(sigc::mem_fun(this, &FloatingWindow::RecvTitleBarMouseDown));
    _title_bar->mouse_drag.connect(sigc::mem_fun(this, &FloatingWindow::RecvTitleBarMouseDrag));
    _close_button->mouse_click.connect(sigc::mem_fun(this, &FloatingWindow::RecvCloseButtonClick));

    _resize_handle->mouse_drag.connect(sigc::mem_fun(this, &FloatingWindow::OnSizeGrigMouseDrag));
    _resize_handle->mouse_down.connect(sigc::mem_fun(this, &FloatingWindow::OnSizeGrigMouseDown));

    _title_bar_layout->AddView((_window_title_bar), 1, eCenter, eFix);
    _title_bar_layout->AddView((_close_button), 0, eCenter, eFix);
    _title_bar_layout->SetParentObject(this);


    if(HasTitleBar())
      SetTopBorder(24);
    else
      SetTopBorder(6);

    SetBorder(2);

    SetMinimumSize(32, 32);
    SetGeometry(Geometry(100, 100, 320, 200));

    NString Path = NUX_FINDRESOURCELOCATION (TEXT ("UITextures/AddButton.png") );
    MinimizeIcon = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture ();
    MinimizeIcon->Update (Path.GetTCharPtr() );
    Path = NUX_FINDRESOURCELOCATION (TEXT ("UITextures/CancelButton.png") );
    CloseIcon = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture ();
    CloseIcon->Update (Path.GetTCharPtr() );

    SetWindowTitle (WindowName);
  }

  FloatingWindow::~FloatingWindow()
  {
    m_InterfaceObject.clear();

    _resize_handle->UnParentObject();
    _resize_handle->UnReference();

    _title_bar->UnParentObject();
    _title_bar->UnReference();

    _minimize_button->UnReference();
    CloseIcon->UnReference();
    MinimizeIcon->UnReference();
    
    _title_bar_layout->UnParentObject();
    _title_bar_layout->UnReference();
  }

  long FloatingWindow::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    long ProcEvInfo = 0;

    IEvent window_event = ievent;
    Geometry base = GetGeometry();
    window_event.e_x_root = base.x;
    window_event.e_y_root = base.y;

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      if (!GetGeometry().IsPointInside (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root) )
      {
        ProcEvInfo = eDoNotProcess;
        //return TraverseInfo;
      }
    }

    ret = _close_button->OnEvent (window_event, ret, ProcEvInfo);

    if (HasTitleBar() )
    {
      ret = _title_bar->OnEvent (window_event, ret, ProcEvInfo);
    }

//    if(m_vertical_scrollbar_enable)
//        ret = vscrollbar->ProcessEvent(ievent, ret, ProcEvInfo);
//    if(m_horizontal_scrollbar_enable)
//        ret = hscrollbar->ProcessEvent(ievent, ret, ProcEvInfo);

    if (IsSizeMatchContent() == false)
    {
      // Do not let the _resize_handle test the event because the window is not displaying it;
      int XGrip = window_event.e_x - window_event.e_x_root - _resize_handle->GetBaseX();
      int YGrip = window_event.e_y - window_event.e_y_root - _resize_handle->GetBaseY();

      if (ievent.e_event == NUX_MOUSE_PRESSED)
      {
        // We want to false on one half of the size grip square to register a mouse down. This is meant to leave more room
        // for the scrollbar buttons (if any) at the bottom right of the window.
        if ((XGrip > 0) && (YGrip > 0) && (XGrip > _resize_handle_height - YGrip))
        {
          ret = _resize_handle->OnEvent(window_event, ret, ProcEvInfo);
        }
      }
      else
      {
        ret = _resize_handle->OnEvent(window_event, ret, ProcEvInfo);
      }
    }

    // The child layout get the Mouse down button only if the MouseDown happened inside the client view Area
    Geometry viewGeometry = GetGeometry(); //Geometry(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight);

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      if (!viewGeometry.IsPointInside (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root) )
      {
        ProcEvInfo = eDoNotProcess;
      }
    }

    if (m_layout)
      ret = m_layout->ProcessEvent (window_event, ret, ProcEvInfo);

    // PostProcessEvent2 must always have its last parameter set to 0
    // because the m_BackgroundArea is the real physical limit of the window.
    // So the previous test about IsPointInside do not prevail over m_BackgroundArea
    // testing the event by itself.
    ret = PostProcessEvent2 (ievent, ret, 0);
    return ret;
  }

  Area* FloatingWindow::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if(mouse_inside == false)
      return NULL;

    NUX_RETURN_VALUE_IF_TRUE(_resize_handle->TestMousePointerInclusion(mouse_position, event_type), _resize_handle);
    NUX_RETURN_VALUE_IF_TRUE(_close_button->TestMousePointerInclusion(mouse_position, event_type), _close_button);

    if(HasTitleBar())
    {
      NUX_RETURN_VALUE_IF_TRUE(_title_bar->TestMousePointerInclusion(mouse_position, event_type), _title_bar);
    }

//     if(_title_bar_layout)
//     {
//       nuxAssert(_title_bar_layout->IsLayout());
//       Area* found_area = _title_bar_layout->FindAreaUnderMouse(mouse_position, event_type);
//       if(found_area)
//         return found_area;
//     }

    if(m_layout)
    {
      nuxAssert(m_layout->IsLayout());
      Area* found_area = m_layout->FindAreaUnderMouse(mouse_position, event_type);
      if(found_area)
        return found_area;
    }

    if((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  void FloatingWindow::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to (0, 0).
    base.SetX (0);
    base.SetY (0);
    GfxContext.PushClippingRectangle (base);

    GetPainter().PushDrawShapeLayer (GfxContext, base, eSHAPE_CORNER_ROUND10, Color (0xFF707070), eCornerTopLeft | eCornerTopRight, true);

    if (HasTitleBar() )
    {
      GetPainter().PaintShapeCorner (GfxContext, Geometry (_title_bar->GetBaseX(), _title_bar->GetBaseY(),
                                 _title_bar->GetBaseWidth(), _title_bar->GetBaseHeight() ), Color (0xFF2f2f2f),
                                 eSHAPE_CORNER_ROUND10, eCornerTopLeft | eCornerTopRight);

      GetPainter().PaintTextLineStatic (GfxContext, GetSysBoldFont(), _window_title_bar->GetGeometry(), _window_title, Color (0xFFFFFFFF), true, eAlignTextCenter);
      GetPainter().Draw2DTextureAligned (GfxContext, CloseIcon, _close_button->GetGeometry(), TextureAlignmentStyle (eTACenter, eTACenter) );
    }

    GetPainter().PopBackground();
    GfxContext.PopClippingRectangle();
  }

  void FloatingWindow::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to (0, 0).
    base.SetX (0);
    base.SetY (0);

    GetPainter().PushShapeLayer (GfxContext, base, eSHAPE_CORNER_ROUND10, Color (0xFF707070), eCornerTopLeft | eCornerTopRight, true);

    if (m_layout)
    {
      GfxContext.PushClippingRectangle (base);

      m_layout->ProcessDraw (GfxContext, force_draw);

      GfxContext.PopClippingRectangle();
    }

    GetPainter().PopBackground();
  }

  void FloatingWindow::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    if (force_draw == false)
    {
      return;
    }

    if ((IsVisibleSizeGrip () == true) && (IsSizeMatchContent () == false))
    {
      // Do not draw the size grip if the window is constrained by the size of the container layout.
      Geometry geo = _resize_handle->GetGeometry ();
      GfxContext.QRP_Triangle (geo.x + geo.width, geo.y, geo.x, geo.y + geo.height, geo.x + geo.width, geo.y + geo.height, Color (0xFF999999));
    }
  }

  void FloatingWindow::EnableTitleBar (bool b)
  {
    m_hasTitleBar = b;
    ComputeChildLayout();
  }

  bool FloatingWindow::HasTitleBar() const
  {
    return m_hasTitleBar;
  }

  void FloatingWindow::OnSizeGrigMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if(IsSizeMatchContent())
    {
      return;
    }

    // Do not let the _resize_handle test the event because the window is not displaying it;
    int XGrip = x;
    int YGrip = y;


    // We want to false on one half of the size grip square to register a mouse down. This is meant to leave more room
    // for the scrollbar buttons (if any) at the bottom right of the window.
    if ((XGrip > 0) && (YGrip > 0) && (XGrip > _resize_handle_height - YGrip))
    {
       // has grip
    }

    m_SizeGripDragPositionX = x;
    m_SizeGripDragPositionY = y;
    //GetWindowCompositor().SetMouseFocusArea(this);
  }

  void FloatingWindow::OnSizeGrigMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    if(IsSizeMatchContent())
    {
      return;
    }

    Geometry geo;
    geo = GetGeometry();

    int ddx = 0;
    int ddy = 0;

    if((dx > 0) && (x > m_SizeGripDragPositionX))
    {
      ddx = dx;
    }

    if((dx < 0) && (x < m_SizeGripDragPositionX))
    {
      ddx = dx;
    }

    if((dy > 0) && (y > m_SizeGripDragPositionY))
    {
      ddy = dy;
    }

    if((dy < 0) && (y < m_SizeGripDragPositionY))
    {
      ddy = dy;
    }

    geo.OffsetSize(ddx, ddy);

    SetGeometry(geo);

#if defined(NUX_OS_LINUX)
    if (m_input_window != 0)
    {
      //nuxDebugMsg (TEXT("Resize Input window: %d, %d, %d, %d"), geo.x, geo.y, geo.width, geo.height);
      m_input_window->SetGeometry (GetGeometry());
    }
#endif

    GetWindowThread()->AddObjectToRefreshList(this);
    QueueDraw();
  }

  void FloatingWindow::RecvTitleBarMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    _title_bar_mouse_down_location = Point(x, y);
  }

  void FloatingWindow::RecvTitleBarMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    Geometry geo;
    geo = GetGeometry();
    geo.OffsetPosition (dx, dy);

    // Set the Window Size and Position
    Area::SetGeometry (geo);
    // No need to compute the window layout elements [LayoutWindowElements()]. They haven't changed.
    // No need to compute the layout [ComputeChildLayout()]. It hasn't changed.

    _title_bar->SetGeometry (0, 0, geo.GetWidth(), _title_bar_height);

#if defined(NUX_OS_LINUX)
    if (m_input_window != 0)
    {
      //nuxDebugMsg (TEXT("Resize Input window: %d, %d, %d, %d"), geo.x, geo.y, geo.width, geo.height);
      m_input_window->SetGeometry (GetGeometry());
    }
#endif

    QueueDraw();
  }

  void FloatingWindow::RecvCloseButtonClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
#if defined(NUX_OS_LINUX)
    // Disable the input window if there is one.
    EnableInputWindow(false);
#endif
    
    StopModal();
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  void FloatingWindow::PreLayoutManagement()
  {
    Geometry geo = GetGeometry();

    if (m_configure_notify_callback)
    {
      (*m_configure_notify_callback) (GetGraphicsDisplay()->GetWindowWidth(), GetGraphicsDisplay()->GetWindowHeight(), geo, m_configure_notify_callback_data);

      if (geo.IsNull() )
      {
        nuxDebugMsg (TEXT ("[FloatingWindow::PreLayoutManagement] Received an invalid Geometry.") );
        geo = GetGeometry();
      }
      else
      {
        Area::SetGeometry (geo);
        // Get the geometry adjusted with respect to min and max dimension of this area.
        geo = GetGeometry();
      }
    }

    // Drag Bar Geometry
    if (HasTitleBar() )
    {
      _title_bar->SetGeometry (0, 0, geo.GetWidth(), _title_bar_height);
    }

    // Size grip Geometry
    Geometry SizeGripGeometry (geo.GetWidth() - _resize_handle_width, geo.GetHeight() - _resize_handle_height,
                               _resize_handle_width, _resize_handle_height);
    _resize_handle->SetGeometry (SizeGripGeometry);

    if (m_layout)
    {
      Geometry layout_geo = Geometry (m_Border, m_TopBorder,
                                      geo.GetWidth() - 2 * m_Border, geo.GetHeight() - m_Border - m_TopBorder);
      m_layout->SetGeometry (layout_geo);
    }
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  long FloatingWindow::PostLayoutManagement (long LayoutResult)
  {
    if (IsSizeMatchContent() && m_layout)
    {
      Geometry layout_geometry = m_layout->GetGeometry();

      Geometry WindowGeometry = Geometry (GetGeometry().x,
                                          GetGeometry().y,
                                          layout_geometry.GetWidth() + 2 * m_Border,
                                          layout_geometry.GetHeight() + m_Border + m_TopBorder);

      Area::SetGeometry (WindowGeometry);
    }

    Geometry geo = GetGeometry();

    // Drag Bar Geometry
    if (HasTitleBar() )
    {
      _title_bar->SetGeometry (0, 0, geo.GetWidth(), _title_bar_height);
    }

    // Size grip Geometry
    Geometry temp (geo.GetWidth() - _resize_handle_width, geo.GetHeight() - _resize_handle_height,
                   _resize_handle_width, _resize_handle_height);
    _resize_handle->SetGeometry (temp);

    // Title Bar
    _title_bar_layout->SetGeometry (_title_bar->GetGeometry() );
    GetWindowThread ()->ComputeElementLayout (_title_bar_layout);

    // A FloatingWindow must kill the result of the management and pass it to the parent Layout.
    return (eCompliantHeight | eCompliantWidth);
    //return result;
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  void FloatingWindow::PositionChildLayout (float offsetX, float offsetY)
  {
    //ScrollView::PositionChildLayout(offsetX, offsetY);

    Geometry geo = GetGeometry();

    // Drag Bar Geometry
    if (HasTitleBar() )
    {
      _title_bar->SetGeometry (0, 0, geo.GetWidth(), _title_bar_height);
    }

    // Size grip Geometry
    Geometry temp (geo.GetWidth() - _resize_handle_width, geo.GetHeight() - _resize_handle_height,
                   _resize_handle_width, _resize_handle_height);
    _resize_handle->SetGeometry (temp);

    // Title Bar
    _title_bar_layout->SetGeometry (_title_bar->GetGeometry() );
    GetWindowThread ()->ComputeElementLayout (_title_bar_layout);

  }

  void FloatingWindow::LayoutWindowElements()
  {
    // Define the geometry of some of the component of the window. Otherwise, if the composition layout is not set,
    // then the component won't be correctly placed after a SetGeometry. This can be redundant if the composition layout is set.
    Geometry base = GetGeometry();
    _title_bar->SetGeometry (0, 0, base.GetWidth(), _title_bar_height);

    _title_bar_layout->SetGeometry (_title_bar->GetGeometry() );
    GetWindowThread ()->ComputeElementLayout (_title_bar_layout);

    // Size grip Geometry
    Geometry temp (base.GetWidth() - _resize_handle_width, base.GetHeight() - _resize_handle_height,
                   _resize_handle_width, _resize_handle_height);
    _resize_handle->SetGeometry (temp);
  }

  void FloatingWindow::SetWindowTitle (const TCHAR *title)
  {
    NUX_RETURN_IF_NULL(title)
    _window_title = title;
  }

  NString FloatingWindow::GetWindowTitle()
  {
    return _window_title;
  }

}
