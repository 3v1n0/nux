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

#include "Layout.h"
#include "HLayout.h"
#include "WindowCompositor.h"
#include "BaseWindow.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(BaseWindow);

  /*
      Elements inside the Window have coordinates based on the top-left corner of the window.
      This is true whether we are drawing or computing the layout. When computing the layout, use x_root and y_root to
      pass the top-left corner position of the window. When drawing, make a similar adjustment.
  */

  BaseWindow::BaseWindow(const char *WindowName, NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , _paint_layer(new ColorLayer(Color(0xFF707070)))
    , _opacity(1.0f)
    , _present_in_embedded_mode(false)
    , _contents_ready_for_presentation(false)
  {
    premultiply = true;
    _name = WindowName;
    _child_need_redraw = true;
    m_TopBorder = 0;
    m_Border = 0;
    _size_match_layout = false;
    _is_visible = false;
    _is_modal = false;
#if defined(USE_X11)
    m_input_window_enabled = false;
    m_input_window = 0;
#endif
    m_layout = 0;
    m_configure_notify_callback = NULL;
    m_configure_notify_callback_data = NULL;
    _entering_visible_state = false;
    _entering_hidden_state = false;
    _enter_focus_input_area = NULL;
    accept_key_nav_focus_ = false;

    SetAcceptKeyNavFocusOnMouseDown(false);

    // Should be at the end of the constructor
    GetWindowThread()->GetWindowCompositor().RegisterWindow(this);

    SetMinimumSize(1, 1);
    SetGeometry(Geometry(100, 100, 320, 200));
  }

  BaseWindow::~BaseWindow()
  {
    if (_enter_focus_input_area)
    {
      _enter_focus_input_area->UnReference();
    }

#if defined(USE_X11)
    if (m_input_window)
      delete m_input_window;
#endif
  }

  Area* BaseWindow::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

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

  void BaseWindow::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to(0, 0).
    base.SetX(0);
    base.SetY(0);
    graphics_engine.PushClippingRectangle(base);

    GetPainter().PushDrawLayer(graphics_engine, base, _paint_layer.get());

    GetPainter().PopBackground();
    graphics_engine.PopClippingRectangle();
  }

  void BaseWindow::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {

    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to(0, 0).
    base.SetX(0);
    base.SetY(0);


    GetPainter().PushLayer(graphics_engine, base, _paint_layer.get());

    if (m_layout)
    {
      graphics_engine.PushClippingRectangle(base);
      m_layout->ProcessDraw(graphics_engine, force_draw);
      graphics_engine.PopClippingRectangle();
    }

    GetPainter().PopBackground();
  }

  void BaseWindow::SetConfigureNotifyCallback(ConfigureNotifyCallback Callback, void *Data)
  {
    m_configure_notify_callback = Callback;
    m_configure_notify_callback_data = Data;
  }

  Layout* BaseWindow::GetLayout()
  {
    return m_layout;
  }

  bool BaseWindow::SetLayout(Layout *layout)
  {
    if (View::SetLayout(layout) == false)
      return false;

    m_layout = layout;
    Geometry geo = GetGeometry();
    Geometry layout_geo = Geometry(geo.x + m_Border, geo.y + m_TopBorder,
                                    geo.GetWidth() - 2 * m_Border, geo.GetHeight() - m_Border - m_TopBorder);
    m_layout->SetGeometry(layout_geo);

    // When this call returns the layout computation is done.
    ComputeContentSize();
    // or use
    //GetWindowThread()->QueueObjectLayout(m_layout);

    return true;
  }

  // Get a change to do any work on an element.
  // Here we need to position the header by hand because it is not under the control of vlayout.
  void BaseWindow::PreLayoutManagement()
  {
    Geometry geo = GetGeometry();

    if (m_configure_notify_callback)
    {
      (*m_configure_notify_callback)(GetGraphicsDisplay()->GetWindowWidth(), GetGraphicsDisplay()->GetWindowHeight(), geo, m_configure_notify_callback_data);

      if (geo.IsNull())
      {
        nuxDebugMsg("[BaseWindow::PreLayoutManagement] Received an invalid Geometry.");
        geo = GetGeometry();
      }
      else
      {
        Area::SetGeometry(geo);
        // Get the geometry adjusted with respect to min and max dimension of this area.
        geo = GetGeometry();
      }
    }

    if (m_layout)
    {
      Geometry layout_geo = Geometry(m_Border, m_TopBorder,
                                      geo.GetWidth() - 2 * m_Border, geo.GetHeight() - m_Border - m_TopBorder);

      if (IsSizeMatchContent())
        m_layout->SetGeometry(Geometry(0, 0, 1, 1));
      else
        m_layout->SetGeometry(layout_geo);
    }
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  long BaseWindow::PostLayoutManagement(long /* LayoutResult */)
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

    // A BaseWindow must kill the result of the management and pass it to the parent Layout.
    return (eCompliantHeight | eCompliantWidth);
    //return result;
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  void BaseWindow::ComputeContentPosition(float /* offsetX */, float /* offsetY */)
  {

  }

#if defined(NUX_OS_LINUX)
  void BaseWindow::EnableInputWindow(bool        b,
                                      const char* title,
                                      bool        take_focus,
                                      bool        override_redirect)
  {
#if defined(USE_X11)
    if (b)
    {
      if (m_input_window == 0)
        m_input_window = new XInputWindow(title, take_focus, override_redirect);

      m_input_window->Show();
      m_input_window->SetGeometry(GetGeometry());
      m_input_window_enabled = true;
    }
    else
    {
      if (m_input_window)
        m_input_window->Hide();
      m_input_window_enabled = false;
    }
#endif
  }

  bool BaseWindow::InputWindowEnabled()
  {
#if defined(USE_X11)
    return m_input_window_enabled;
#else
    return false;
#endif
  }

  void BaseWindow::InputWindowEnableStruts(bool enable)
  {
#if defined(USE_X11)
    if (m_input_window)
      m_input_window->EnableStruts(enable);
#endif
  }

  bool BaseWindow::InputWindowStrutsEnabled()
  {
#if defined(USE_X11)
    return m_input_window_enabled && m_input_window->StrutsEnabled();
#else
    return false;
#endif
  }

  void BaseWindow::InputWindowEnableOverlayStruts(bool enable)
  {
#if defined(USE_X11)
    if (m_input_window)
      m_input_window->EnableOverlayStruts(enable);
#endif
  }

  bool BaseWindow::InputWindowOverlayStrutsEnabled()
  {
#if defined(USE_X11)
    return m_input_window && m_input_window->OverlayStrutsEnabled();
#else
    return false;
#endif
  }

  void BaseWindow::SetInputFocus()
  {
#if defined(USE_X11)
    if (m_input_window)
      m_input_window->SetInputFocus();
#endif
  }

  Window BaseWindow::GetInputWindowId()
  {
#if defined(USE_X11)
    if (m_input_window)
      return m_input_window->GetWindow();
    else
      return 0;
#else
    return 0;
#endif
  }

#endif

  void BaseWindow::SetGeometry(const Geometry &geo)
  {
    Area::SetGeometry(geo);

    #if defined(USE_X11)
    if (m_input_window)
      m_input_window->SetGeometry(geo);
    #endif
    //LayoutWindowElements();
    //ComputeContentSize();
  }

  void BaseWindow::LayoutWindowElements()
  {
    // Define the geometry of some of the component of the window. Otherwise, if the composition layout is not set,
    // then the component won't be correctly placed after a SetGeometry. This can be redondant if the composition layout is set.
    GetGeometry();
  }

  void BaseWindow::SetBorder(int border)
  {
    if (m_Border != border)
    {
      m_Border = border;
    }
  }

  void BaseWindow::SetTopBorder(int border)
  {
    if (m_TopBorder != border)
    {
      m_TopBorder = border;
    }
  }

  int BaseWindow::GetBorder() const
  {
    return m_Border;
  }

  int BaseWindow::GetTopBorder() const
  {
    return m_TopBorder;
  }

  void BaseWindow::ShowWindow(bool visible, bool StartModal /*  = false */)
  {
    if (visible == _is_visible)
      return;

    _is_visible = visible;
    _is_modal = StartModal;

    if (_is_visible)
    {
      if (m_layout)
      {
        m_layout->SetGeometry(GetGeometry());
      }

      _entering_visible_state = true;

      sigVisible.emit(this);
      GetWindowThread()->GetWindowCompositor().sigVisibleViewWindow.emit(this);

      ComputeContentSize();
    }
    else
    {
      SetEnterFocusInputArea(NULL);

      _entering_hidden_state = true;
      sigHidden.emit(this);
      GetWindowThread()->GetWindowCompositor().sigHiddenViewWindow.emit(this);
    }

    if (_is_modal)
      GetWindowThread()->GetWindowCompositor().StartModalWindow(ObjectWeakPtr<BaseWindow>(this));

    // Whether this view is added or removed, call QueueDraw. in the case where this view is removed, this is a signal
    // that the region below this view need to be redrawn.
    QueueDraw();
  }

  bool BaseWindow::IsVisible() const
  {
    return _is_visible;
  }

  void BaseWindow::StopModal()
  {
    _is_visible = false;
    _is_modal = false;
    //ShowWindow(false);
    GetWindowThread()->GetWindowCompositor().StopModalWindow(ObjectWeakPtr<BaseWindow> (this));
  }

  bool BaseWindow::IsModal() const
  {
    return _is_modal;
  }

  void BaseWindow::NotifyConfigurationChange(int /* Width */, int /* Height */)
  {
    Geometry geo = GetGeometry();

    if (m_configure_notify_callback)
    {
      (*m_configure_notify_callback) (GetGraphicsDisplay()->GetWindowWidth(), GetGraphicsDisplay()->GetWindowHeight(), geo, m_configure_notify_callback_data);

      if (geo.IsNull())
      {
        nuxDebugMsg("[BaseWindow::NotifyConfigurationChange] Received an invalid Geometry.");
        geo = GetGeometry();
      }
      else
      {
        Area::SetGeometry(geo);
        // Get the geometry adjusted with respect to min and max dimension of this area.
        geo = GetGeometry();
      }
    }
    else
    {
      return;
    }
  }

  void BaseWindow::SetBackgroundLayer(AbstractPaintLayer *layer)
  {
    NUX_RETURN_IF_NULL(layer);
    _paint_layer.reset(layer->Clone());
  }

  void BaseWindow::SetBackgroundColor(const Color &color)
  {
    _paint_layer.reset(new ColorLayer(color));
  }

  void BaseWindow::PushHigher(BaseWindow* floating_view)
  {
    GetWindowThread()->GetWindowCompositor().PushHigher(this, floating_view);
  }

  void BaseWindow::PushToFront()
  {
    GetWindowThread()->GetWindowCompositor().PushToFront(this);
  }

  void BaseWindow::PushToBack()
  {
    GetWindowThread()->GetWindowCompositor().PushToBack(this);
  }

  bool BaseWindow::ChildNeedsRedraw()
  {
    return _child_need_redraw;
  }

  void* BaseWindow::GetBackupTextureData(int &width, int &height, int &format)
  {
    return GetWindowThread()->GetWindowCompositor().GetBackupTextureData(this, width, height, format);
  }

  void BaseWindow::PresentInEmbeddedModeOnThisFrame(bool force)
  {
    nuxAssertMsg (GetWindowThread()->IsEmbeddedWindow(),
                  "[BaseWindow::PresentInEmbeddedModeOnThisFrame] only "
                  "supported in embdded mode");

    /* Invisible windows are never presented */
    if (!IsVisible())
      return;

    if (nux::GetWindowThread()->AddToPresentationList(this, force))
      _present_in_embedded_mode = true;
  }

  void BaseWindow::MarkPresentedInEmbeddedMode()
  {
    _present_in_embedded_mode = false;
    _last_presented_geometry_in_embedded_mode = GetAbsoluteGeometry();
  }

  nux::Geometry const& BaseWindow::LastPresentedGeometryInEmbeddedMode() const
  {
    return _last_presented_geometry_in_embedded_mode;
  }

  bool BaseWindow::AllowPresentationInEmbeddedMode() const
  {
    return _present_in_embedded_mode;
  }

  void BaseWindow::PrepareParentRedirectedView()
  {
    Area::PrepareParentRedirectedView();
    if (GetWindowThread()->IsEmbeddedWindow())
      PresentInEmbeddedModeOnThisFrame();
  }

  void BaseWindow::SetEnterFocusInputArea(InputArea *input_area)
  {
    if (_enter_focus_input_area)
    {
      _enter_focus_input_area->UnReference();
    }

    _enter_focus_input_area = input_area;
    if (_enter_focus_input_area)
      _enter_focus_input_area->Reference();

  }

  void BaseWindow::SetOpacity(float opacity)
  {
    if (_opacity == opacity)
      return;

    _opacity = opacity;

    _opacity = CLAMP(_opacity, 0.0f, 1.0f);

    QueueDraw();
  }

  float BaseWindow::GetOpacity()
  {
    return _opacity;
  }

  void BaseWindow::SetAcceptKeyNavFocus(bool accept)
  {
    accept_key_nav_focus_ = accept;
  }

  bool BaseWindow::AcceptKeyNavFocus()
  {
    return accept_key_nav_focus_;
  }

#ifdef NUX_GESTURES_SUPPORT
  Area* BaseWindow::GetInputAreaHitByGesture(const nux::GestureEvent &event)
  {
    if (!IsVisible())
      return nullptr;

    if (!IsGestureInsideArea(event))
      return nullptr;

    if (m_layout)
    {
      Area *area_hit = m_layout->GetInputAreaHitByGesture(event);
      if (area_hit)
        return area_hit;
    }

    if (HasSubscriptionForGesture(event))
      return this;
    else
      return nullptr;
  }
#endif
}
