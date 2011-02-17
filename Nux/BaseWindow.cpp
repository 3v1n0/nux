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

#include "Nux.h"

#include "NuxGraphics/GLTextureResourceManager.h"

#include "Layout.h"
#include "HLayout.h"
#include "WindowCompositor.h"
#include "BaseWindow.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (BaseWindow);

  const int SizeGripWidth = 20;
  const int SizeGripHeight = 20;
  const int TitleBarHeight = 20;

  /*
      Elements inside the Window have coordinates based on the top-left corner of the window.
      This is true whether we are drawing or computing the layout. When computing the layout, use x_root and y_root to
      pass the top-left corner position of the window. When drawing, make a similar adjustment.
  */

  BaseWindow::BaseWindow (const TCHAR *WindowName, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    _child_need_redraw = true;
    m_TopBorder = 0;
    m_Border = 0;
    _size_match_layout = false;
    _is_visible = false;
    _is_modal = false;
#if defined(NUX_OS_LINUX)
    m_input_window_enabled = false;
    m_input_window = 0;
#endif
    m_layout = 0;
    m_configure_notify_callback = NULL;
    m_configure_notify_callback_data = NULL;
    _entering_visible_state = false;
    _entering_hidden_state = false;

    // Should be at the end of the constructor
    GetWindowCompositor().RegisterWindow (this);

    SetMinimumSize (1, 1);
    SetGeometry(Geometry(100, 100, 320, 200));

    _paint_layer = new ColorLayer(Color(0xFF707070));
    //_background_texture = GetWindow ()->GetGpuDevice ()->CreateSystemCapableDeviceTexture(1, 1, 1, BITFMT_R8G8B8A8);
  }

  BaseWindow::~BaseWindow()
  {
#if defined(NUX_OS_LINUX)
    if (m_input_window)
    {
      EnableInputWindow (false);
    }
#endif

    // At this stage, the reference count of this object is 0 and while the weak reference count is > 0.
    // The weak reference count is probably 2: one reference in m_WindowList and another in m_WindowToTextureMap.
    // Reference the object here to avoid it being destroy when the call from UnRegisterWindow returns;
    GetWindowCompositor().UnRegisterWindow (this);

    NUX_SAFE_DELETE (_paint_layer);
  }

  long BaseWindow::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    long ProcEvInfo = 0;

    IEvent window_event = ievent;
    Geometry base = GetGeometry();
    window_event.e_x_root = base.x;
    window_event.e_y_root = base.y;

    // The child layout get the Mouse down button only if the MouseDown happened inside the client view Area
    Geometry viewGeometry = GetGeometry();

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

  void BaseWindow::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to (0, 0).
    base.SetX (0);
    base.SetY (0);
    GfxContext.PushClippingRectangle (base);

    GetPainter().PushDrawLayer(GfxContext, base, _paint_layer);

    GetPainter().PopBackground();
    GfxContext.PopClippingRectangle();
  }

  void BaseWindow::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to (0, 0).
    base.SetX (0);
    base.SetY (0);


    GetPainter().PushLayer(GfxContext, base, _paint_layer);

    if (m_layout)
    {
      GfxContext.PushClippingRectangle (base);
      m_layout->ProcessDraw (GfxContext, force_draw);
      GfxContext.PopClippingRectangle();
    }

    GetPainter().PopBackground();
  }

  void BaseWindow::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void BaseWindow::SetConfigureNotifyCallback (ConfigureNotifyCallback Callback, void *Data)
  {
    m_configure_notify_callback = Callback;
    m_configure_notify_callback_data = Data;
  }

  void BaseWindow::AddWidget (View *ic)
  {
    if (ic && m_layout)
    {
      m_layout->AddView (ic, 0);
      // 0: the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
      // and the children will take their natural size by expending WidgetLayout.
      // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.

      ComputeChildLayout();
    }
  }

  void BaseWindow::AddWidget (View *ic, int stretchfactor)
  {
    if (ic && m_layout)
    {
      m_layout->AddView (ic, stretchfactor);
      // if(stretchfactor ==0): the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
      // and the children will take their natural size by expending WidgetLayout.
      // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.

      ComputeChildLayout();
    }
  }

  void BaseWindow::AddWidget (std::list<View *> *ViewList)
  {
    m_CompositionLayout->Clear();

    std::list<View *>::iterator it;

    for (it = ViewList->begin(); it != ViewList->end(); it++)
    {
      AddWidget ( (*it) );
    }
  }

  Layout* BaseWindow::GetLayout ()
  {
    return m_layout;
  }

  bool BaseWindow::SetLayout (Layout *layout)
  {
    if (View::SetLayout (layout) == false)
      return false;

    m_layout = layout;
    Geometry geo = GetGeometry();
    Geometry layout_geo = Geometry (geo.x + m_Border, geo.y + m_TopBorder,
                                    geo.GetWidth() - 2 * m_Border, geo.GetHeight() - m_Border - m_TopBorder);
    m_layout->SetGeometry (layout_geo);

    // When this call returns the layout computation is done.
    ComputeChildLayout();
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
      (*m_configure_notify_callback) (GetThreadGLWindow()->GetWindowWidth(), GetThreadGLWindow()->GetWindowHeight(), geo, m_configure_notify_callback_data);

      if (geo.IsNull() )
      {
        nuxDebugMsg (TEXT ("[BaseWindow::PreLayoutManagement] Received an invalid Geometry.") );
        geo = GetGeometry();
      }
      else
      {
        Area::SetGeometry (geo);
        // Get the geometry adjusted with respect to min and max dimension of this area.
        geo = GetGeometry();
      }
    }

    if (m_layout)
    {
      Geometry layout_geo = Geometry (m_Border, m_TopBorder,
                                      geo.GetWidth() - 2 * m_Border, geo.GetHeight() - m_Border - m_TopBorder);

      if (IsSizeMatchContent ())
        m_layout->SetGeometry (Geometry (0, 0, 1, 1));
      else
        m_layout->SetGeometry (layout_geo);
    }
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  long BaseWindow::PostLayoutManagement (long LayoutResult)
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

    // A BaseWindow must kill the result of the management and pass it to the parent Layout.
    return (eCompliantHeight | eCompliantWidth);
    //return result;
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  void BaseWindow::PositionChildLayout (float offsetX, float offsetY)
  {

  }
  
  #if defined(NUX_OS_LINUX)
  void BaseWindow::EnableInputWindow (bool b, bool override_redirect)
  {
    if (b)
    {
      if (m_input_window == 0)
      {
        m_input_window = new XInputWindow (override_redirect);
        m_input_window->SetGeometry (GetGeometry());
      }
      m_input_window_enabled = true;
    }
    else
    {
      if (m_input_window)
      {
        delete (m_input_window);
        m_input_window = 0;
      }
      m_input_window_enabled = false;
    }
  }
  
  bool BaseWindow::InputWindowEnabled ()
  {
    return m_input_window_enabled;
  }
  
  void BaseWindow::InputWindowEnableStruts (bool enable)
  {
    if (!m_input_window_enabled)
      return;
    
    m_input_window->EnableStruts (enable);
  }
  
  bool BaseWindow::InputWindowStrutsEnabled ()
  {
    return m_input_window_enabled && m_input_window->StrutsEnabled ();
  }
  
  void BaseWindow::GrabPointer ()
  {
    if (m_input_window)
      m_input_window->GrabPointer ();
      
  }
  
  void BaseWindow::UnGrabPointer ()
  {
    if (m_input_window)
      m_input_window->UnGrabPointer ();    
  }

  void BaseWindow::GrabKeyboard ()
  {
    if (m_input_window)
      m_input_window->GrabKeyboard ();
      
  }
  
  void BaseWindow::UnGrabKeyboard ()
  {
    if (m_input_window)
      m_input_window->UnGrabKeyboard ();    
  }

  Window BaseWindow::GetInputWindowId ()
  {
    if (m_input_window)
      return m_input_window->GetWindow ();
    else
      return 0;
  }

  #endif

  void BaseWindow::SetGeometry (const Geometry &geo)
  {
    Area::SetGeometry (geo);
    
    #if defined(NUX_OS_LINUX)
    if (m_input_window_enabled)
      m_input_window->SetGeometry (geo);
    #endif
    //LayoutWindowElements();
    //ComputeChildLayout();
  }

  void BaseWindow::LayoutWindowElements()
  {
    // Define the geometry of some of the component of the window. Otherwise, if the composition layout is not set,
    // then the component won't be correctly placed after a SetGeometry. This can be redondant if the composition layout is set.
    Geometry base = GetGeometry();
  }

  void BaseWindow::SetBorder (int border)
  {
    if (m_Border != border)
    {
      m_Border = border;
    }
  }

  void BaseWindow::SetTopBorder (int border)
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

  void BaseWindow::ShowWindow (bool visible, bool StartModal /*  = false */)
  {
    if (visible == _is_visible)
      return;

    if (m_layout)
    {
      m_layout->SetGeometry (GetGeometry() );
    }

    _is_visible = visible;
    _is_modal = StartModal;

    if (_is_visible)
    {
      _entering_visible_state = true;
      sigVisible.emit (this);
    }
    else
    {
      _entering_hidden_state = true;
      sigHidden.emit (this);
    }
    
    ComputeChildLayout();

    if (_is_modal)
      GetWindowCompositor().StartModalWindow (ObjectWeakPtr<BaseWindow> (this));

    // Whether this view is added or removed, call NeedRedraw. in the case where this view is removed, this is a signal 
    // that the region below this view need to be redrawn.
    NeedRedraw();
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
    GetWindowCompositor().StopModalWindow (ObjectWeakPtr<BaseWindow> (this));
  }

  bool BaseWindow::IsModal() const
  {
    return _is_modal;
  }

  void BaseWindow::NotifyConfigurationChange (int Width, int Height)
  {
    Geometry geo = GetGeometry();

    if (m_configure_notify_callback)
    {
      (*m_configure_notify_callback) (GetThreadGLWindow()->GetWindowWidth(), GetThreadGLWindow()->GetWindowHeight(), geo, m_configure_notify_callback_data);

      if (geo.IsNull() )
      {
        nuxDebugMsg (TEXT ("[BaseWindow::NotifyConfigurationChange] Received an invalid Geometry.") );
        geo = GetGeometry();
      }
      else
      {
        Area::SetGeometry (geo);
        // Get the geometry adjusted with respect to min and max dimension of this area.
        geo = GetGeometry();
      }
    }
    else
    {
      return;
    }
  }

  void BaseWindow::SetBackgroundLayer (AbstractPaintLayer *layer)
  {
    NUX_RETURN_IF_NULL (layer);
    NUX_SAFE_DELETE (_paint_layer);
    _paint_layer = layer->Clone();
  }

  void BaseWindow::SetBackgroundColor (const Color &color)
  {
    if (_paint_layer)
      NUX_SAFE_DELETE(_paint_layer);

    _paint_layer = new ColorLayer(color);
  }

  void BaseWindow::PushHigher (BaseWindow* floating_view)
  {
    GetWindowCompositor().PushHigher (this, floating_view);
  }

  void BaseWindow::PushToFront ()
  {
    GetWindowCompositor().PushToFront (this);
  }

  void BaseWindow::PushToBack ()
  {
    GetWindowCompositor().PushToBack (this);
  }

  bool BaseWindow::ChildNeedsRedraw ()
  {
    return _child_need_redraw;
  }

  void* BaseWindow::GetBackupTextureData (int &width, int &height, int &format)
  {
    return GetWindowCompositor ().GetBackupTextureData (this, width, height, format);
  }

  bool BaseWindow::ProcessSpecialEvent(Event event)
  {
    return false;
  }
}

