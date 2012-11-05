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
  NUX_IMPLEMENT_OBJECT_TYPE(FloatingWindow);

  /*
      Elements inside the Window have coordinates based on the top-left corner of the window.
      This is true whether we are drawing or computing the layout. When computing the layout, use x_root and y_root to
      pass the top-left corner position of the window. When drawing, make a similar adjustment.
  */
  FloatingWindow::FloatingWindow(const char *WindowName, NUX_FILE_LINE_DECL)
    :   BaseWindow(WindowName, NUX_FILE_LINE_PARAM)
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

    _minimize_button    = new BasicView(NUX_TRACKER_LOCATION);
    _minimize_button->SetParentObject(this);

    _resize_handle      = new BasicView(NUX_TRACKER_LOCATION);
    _resize_handle->SinkReference();
    _resize_handle->SetParentObject(this);

    _title_bar          = new BasicView(NUX_TRACKER_LOCATION);
    _title_bar->SinkReference();
    _title_bar->SetParentObject(this);

    _close_button       = new BasicView(NUX_TRACKER_LOCATION);
    _window_title_bar   = new StaticTextBox("", NUX_TRACKER_LOCATION);

    _title_bar_layout   = new HLayout(NUX_TRACKER_LOCATION);
    _title_bar_layout->Reference();
    
    _minimize_button->SetMinMaxSize(20, 20);
    _minimize_button->SetGeometry(Geometry(0, 0, 20, 20));
    _close_button->SetMinimumSize(20, 20);
    _close_button->SetGeometry(Geometry(0, 0, 20, 20));
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


    if (HasTitleBar())
      SetTopBorder(24);
    else
      SetTopBorder(6);

    SetBorder(2);

    SetMinimumSize(32, 32);
    SetGeometry(Geometry(100, 100, 320, 200));

    std::string Path = NUX_FINDRESOURCELOCATION("UITextures/AddButton.png");
    MinimizeIcon = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    MinimizeIcon->Update(Path.c_str());
    Path = NUX_FINDRESOURCELOCATION("UITextures/CancelButton.png");
    CloseIcon = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    CloseIcon->Update(Path.c_str());

    SetWindowTitle(WindowName);
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

  Area* FloatingWindow::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    NUX_RETURN_VALUE_IF_TRUE(_resize_handle->TestMousePointerInclusion(mouse_position, event_type), _resize_handle);
    NUX_RETURN_VALUE_IF_TRUE(_close_button->TestMousePointerInclusion(mouse_position, event_type), _close_button);

    if (HasTitleBar())
    {
      NUX_RETURN_VALUE_IF_TRUE(_title_bar->TestMousePointerInclusion(mouse_position, event_type), _title_bar);
    }

//     if (_title_bar_layout)
//     {
//       nuxAssert(_title_bar_layout->IsLayout());
//       Area* found_area = _title_bar_layout->FindAreaUnderMouse(mouse_position, event_type);
//       if (found_area)
//         return found_area;
//     }

    if (m_layout)
    {
      nuxAssert(m_layout->IsLayout());
      Area* found_area = m_layout->FindAreaUnderMouse(mouse_position, event_type);
      if (found_area)
        return found_area;
    }

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  void FloatingWindow::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to(0, 0).
    base.SetX(0);
    base.SetY(0);
    graphics_engine.PushClippingRectangle(base);

    GetPainter().PushDrawShapeLayer(graphics_engine, base, eSHAPE_CORNER_ROUND10, Color(0xFF707070), eCornerTopLeft | eCornerTopRight, true);

    if (HasTitleBar())
    {
      GetPainter().PaintShapeCorner(graphics_engine, Geometry(_title_bar->GetBaseX(), _title_bar->GetBaseY(),
                                 _title_bar->GetBaseWidth(), _title_bar->GetBaseHeight()), Color(0xFF2f2f2f),
                                 eSHAPE_CORNER_ROUND10, eCornerTopLeft | eCornerTopRight);

      GetPainter().PaintTextLineStatic(graphics_engine, GetSysBoldFont(), _window_title_bar->GetGeometry(), _window_title, Color(0xFFFFFFFF), true, eAlignTextCenter);
      GetPainter().Draw2DTextureAligned(graphics_engine, CloseIcon, _close_button->GetGeometry(), TextureAlignmentStyle(eTACenter, eTACenter));
    }

    GetPainter().PopBackground();
    graphics_engine.PopClippingRectangle();
  }

  void FloatingWindow::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to(0, 0).
    base.SetX(0);
    base.SetY(0);

    GetPainter().PushShapeLayer(graphics_engine, base, eSHAPE_CORNER_ROUND10, Color(0xFF707070), eCornerTopLeft | eCornerTopRight, true);

    if (m_layout)
    {
      graphics_engine.PushClippingRectangle(base);

      m_layout->ProcessDraw(graphics_engine, force_draw);

      graphics_engine.PopClippingRectangle();
    }

    GetPainter().PopBackground();
  }

  void FloatingWindow::EnableTitleBar(bool b)
  {
    m_hasTitleBar = b;
    ComputeContentSize();
  }

  bool FloatingWindow::HasTitleBar() const
  {
    return m_hasTitleBar;
  }

  void FloatingWindow::OnSizeGrigMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (IsSizeMatchContent())
    {
      return;
    }

    // Do not let the _resize_handle test the event because the window is not displaying it;
    int XGrip = x;
    int YGrip = y;


    // We want to false on one half of the size grip square to register a mouse down. This is meant to leave more room
    // for the scrollbar buttons(if any) at the bottom right of the window.
    if ((XGrip > 0) && (YGrip > 0) && (XGrip > _resize_handle_height - YGrip))
    {
       // has grip
    }

    m_SizeGripDragPositionX = x;
    m_SizeGripDragPositionY = y;
    //GetWindowCompositor().SetMouseFocusArea(this);
  }

  void FloatingWindow::OnSizeGrigMouseDrag(int x, int y, int dx, int dy, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (IsSizeMatchContent())
    {
      return;
    }

    Geometry geo;
    geo = GetGeometry();

    int ddx = 0;
    int ddy = 0;

    if ((dx > 0) && (x > m_SizeGripDragPositionX))
    {
      ddx = dx;
    }

    if ((dx < 0) && (x < m_SizeGripDragPositionX))
    {
      ddx = dx;
    }

    if ((dy > 0) && (y > m_SizeGripDragPositionY))
    {
      ddy = dy;
    }

    if ((dy < 0) && (y < m_SizeGripDragPositionY))
    {
      ddy = dy;
    }

    geo.OffsetSize(ddx, ddy);

    SetGeometry(geo);

#if defined(USE_X11)
    if (m_input_window != 0)
    {
      //nuxDebugMsg("Resize Input window: %d, %d, %d, %d", geo.x, geo.y, geo.width, geo.height);
      m_input_window->SetGeometry(GetGeometry());
    }
#endif

    GetWindowThread()->QueueObjectLayout(this);
    QueueDraw();
  }

  void FloatingWindow::RecvTitleBarMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    _title_bar_mouse_down_location = Point(x, y);
  }

  void FloatingWindow::RecvTitleBarMouseDrag(int /* x */, int /* y */, int dx, int dy, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    Geometry geo;
    geo = GetGeometry();
    geo.OffsetPosition(dx, dy);

    // Set the Window Size and Position
    Area::SetGeometry(geo);
    // No need to compute the window layout elements [LayoutWindowElements()]. They haven't changed.
    // No need to compute the layout [ComputeContentSize()]. It hasn't changed.

    _title_bar->SetGeometry(Geometry(0, 0, geo.GetWidth(), _title_bar_height));

#if defined(USE_X11)
    if (m_input_window != 0)
    {
      //nuxDebugMsg("Resize Input window: %d, %d, %d, %d", geo.x, geo.y, geo.width, geo.height);
      m_input_window->SetGeometry(GetGeometry());
    }
#endif

    QueueDraw();
  }

  void FloatingWindow::RecvCloseButtonClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
#if defined(USE_X11)
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

      if (geo.IsNull())
      {
        nuxDebugMsg("[FloatingWindow::PreLayoutManagement] Received an invalid Geometry.");
        geo = GetGeometry();
      }
      else
      {
        Area::SetGeometry(geo);
        // Get the geometry adjusted with respect to min and max dimension of this area.
        geo = GetGeometry();
      }
    }

    // Drag Bar Geometry
    if (HasTitleBar())
    {
      _title_bar->SetGeometry(Geometry(0, 0, geo.GetWidth(), _title_bar_height));
    }

    // Size grip Geometry
    Geometry SizeGripGeometry(geo.GetWidth() - _resize_handle_width, geo.GetHeight() - _resize_handle_height,
                               _resize_handle_width, _resize_handle_height);
    _resize_handle->SetGeometry(SizeGripGeometry);

    if (m_layout)
    {
      Geometry layout_geo = Geometry(m_Border, m_TopBorder,
                                      geo.GetWidth() - 2 * m_Border, geo.GetHeight() - m_Border - m_TopBorder);
      m_layout->SetGeometry(layout_geo);
    }
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  long FloatingWindow::PostLayoutManagement(long /* LayoutResult */)
  {
    if (IsSizeMatchContent() && m_layout)
    {
      Geometry layout_geometry = m_layout->GetGeometry();

      Geometry WindowGeometry = Geometry(GetGeometry().x,
                                          GetGeometry().y,
                                          layout_geometry.GetWidth() + 2 * m_Border,
                                          layout_geometry.GetHeight() + m_Border + m_TopBorder);

      Area::SetGeometry(WindowGeometry);
    }

    Geometry geo = GetGeometry();

    // Drag Bar Geometry
    if (HasTitleBar())
    {
      _title_bar->SetGeometry(Geometry(0, 0, geo.GetWidth(), _title_bar_height));
    }

    // Size grip Geometry
    Geometry temp(geo.GetWidth() - _resize_handle_width, geo.GetHeight() - _resize_handle_height,
                   _resize_handle_width, _resize_handle_height);
    _resize_handle->SetGeometry(temp);

    // Title Bar
    _title_bar_layout->SetGeometry(_title_bar->GetGeometry());
    GetWindowThread()->ComputeElementLayout(_title_bar_layout);

    // A FloatingWindow must kill the result of the management and pass it to the parent Layout.
    return (eCompliantHeight | eCompliantWidth);
    //return result;
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  void FloatingWindow::ComputeContentPosition(float /* offsetX */, float /* offsetY */)
  {
    //ScrollView::ComputeContentPosition(offsetX, offsetY);

    Geometry geo = GetGeometry();

    // Drag Bar Geometry
    if (HasTitleBar())
    {
      _title_bar->SetGeometry(Geometry(0, 0, geo.GetWidth(), _title_bar_height));
    }

    // Size grip Geometry
    Geometry temp(geo.GetWidth() - _resize_handle_width, geo.GetHeight() - _resize_handle_height,
                   _resize_handle_width, _resize_handle_height);
    _resize_handle->SetGeometry(temp);

    // Title Bar
    _title_bar_layout->SetGeometry(_title_bar->GetGeometry());
    GetWindowThread()->ComputeElementLayout(_title_bar_layout);

  }

  void FloatingWindow::LayoutWindowElements()
  {
    // Define the geometry of some of the component of the window. Otherwise, if the composition layout is not set,
    // then the component won't be correctly placed after a SetGeometry. This can be redundant if the composition layout is set.
    Geometry base = GetGeometry();
    _title_bar->SetGeometry(Geometry(0, 0, base.GetWidth(), _title_bar_height));

    _title_bar_layout->SetGeometry(_title_bar->GetGeometry());
    GetWindowThread()->ComputeElementLayout(_title_bar_layout);

    // Size grip Geometry
    Geometry temp(base.GetWidth() - _resize_handle_width, base.GetHeight() - _resize_handle_height,
                   _resize_handle_width, _resize_handle_height);
    _resize_handle->SetGeometry(temp);
  }

  void FloatingWindow::SetWindowTitle(const char *title)
  {
    NUX_RETURN_IF_NULL(title)
    _window_title = title;
  }

  std::string FloatingWindow::GetWindowTitle()
  {
    return _window_title;
  }

}
