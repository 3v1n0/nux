// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2010,2011 Inalogic® Inc.
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
#include "WindowCompositor.h"
#include "NuxCore/Logger.h"
#include "NuxGraphics/GLError.h"
#include "WindowThread.h"
#include "BaseWindow.h"
#include "MenuPage.h"
#include "PaintLayer.h"
#include "Painter.h"

#include "NuxGraphics/FontTexture.h"
namespace nux
{
namespace
{
  logging::Logger logger("nux.window");
}

  WindowCompositor::WindowCompositor(WindowThread *window_thread)
  : window_thread_(window_thread)  
  , reference_fbo_(0)
  {
    m_FocusAreaWindow           = NULL;
    m_MenuWindow                = NULL;
    m_OverlayWindow             = NULL;
    _tooltip_window             = NULL;
    m_TooltipArea               = NULL;
    m_ModalWindow               = NULL;
    m_SelectedWindow            = NULL;
    _menu_chain                 = NULL;
    m_Background                = NULL;
    _tooltip_window             = NULL;
    m_OverlayWindow             = NULL;
    OverlayDrawingCommand       = NULL;
    m_CurrentWindow             = NULL;
    m_MenuWindow                = NULL;
    _mouse_over_area            = NULL;
    key_focus_area_             = NULL;
    _always_on_front_window     = NULL;
    inside_event_cycle_         = false;
    inside_rendering_cycle_     = false;
    _exclusive_input_area       = NULL;
    _in_exclusive_input_mode    = false;
    _pending_exclusive_input_mode_action = false;

    _dnd_area                   = NULL;
    mouse_over_area_            = NULL;
    mouse_owner_area_           = NULL;
    _mouse_over_menu_page       = NULL;
    _mouse_owner_menu_page      = NULL;
    mouse_owner_base_window_    = NULL;
    _starting_menu_event_cycle  = false;
    _menu_is_active             = false;
    _enable_nux_new_event_architecture   = true;
    on_menu_closure_continue_with_event_ = false;

    m_FrameBufferObject = GetGraphicsDisplay()->GetGpuDevice()->CreateFrameBufferObject();
    // Do not leave the Fbo binded. Deactivate it.
    m_FrameBufferObject->Deactivate();

    // At this stage, the size of the window may not be known yet.
    // FormatRenderTargets will be called the first time runtime gets into WindowThread::ExecutionLoop
    m_MainColorRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(2, 2, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
    m_MainDepthRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(2, 2, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);

    _menu_chain = new std::list<MenuPage*>;
    m_PopupRemoved = false;
    m_MenuRemoved = false;
    m_ModalWindow = NULL;
    m_Background = new ColorLayer(Color(0xFF4D4D4D));
  }

  void WindowCompositor::BeforeDestructor()
  {
    if (key_focus_area_)
    {
      key_focus_area_->ResetDownwardPathToKeyFocusArea();
      key_focus_area_->ResetUpwardPathToKeyFocusArea();
      key_focus_area_ = NULL;
    }
  }

  WindowCompositor::~WindowCompositor()
  {
    _window_to_texture_map.clear();
    m_FrameBufferObject.Release();
    m_MainColorRT.Release();
    m_MainDepthRT.Release();
    _menu_chain->clear();
    _view_window_list.clear();
    _modal_view_window_list.clear();

    NUX_SAFE_DELETE(_menu_chain);
    NUX_SAFE_DELETE(m_Background);
  }


  BaseWindow* WindowCompositor::GetSelectedWindow()
  {
    return m_SelectedWindow.GetPointer();
  }

  WindowCompositor::RenderTargetTextures &WindowCompositor::GetWindowBuffer(BaseWindow *window)
  {
    static RenderTargetTextures invalid;
    std::map< BaseWindow*, RenderTargetTextures >::iterator it = _window_to_texture_map.find(window);

    if (it != _window_to_texture_map.end())
    {
      return it->second;
    }
    LOG_WARN(logger) << "No RenderTargetTextures for window.";
    return invalid;
  }

  void WindowCompositor::RegisterWindow(BaseWindow *window)
  {
    LOG_DEBUG_BLOCK(logger);
    if (!window)
      return;

    WindowList::iterator it = find(_view_window_list.begin(),
                                   _view_window_list.end(), window);
    if (it == _view_window_list.end())
    {
      _view_window_list.push_front(ObjectWeakPtr<BaseWindow>(window));
      m_SelectedWindow = window;

      RenderTargetTextures rt;

      // Don't size the texture to the dimension of the window yet. this will be done later.
      auto device = GetGraphicsDisplay()->GetGpuDevice();
      rt.color_rt = device->CreateSystemCapableDeviceTexture(2, 2, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
      rt.depth_rt = device->CreateSystemCapableDeviceTexture(2, 2, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);

      _window_to_texture_map[window] = rt;

      window->object_destroyed.connect(sigc::mem_fun(this, &WindowCompositor::UnRegisterWindow));
    }
  }

  void WindowCompositor::UnRegisterWindow(Object* obj)
  {
    LOG_DEBUG_BLOCK(logger);
    WeakBaseWindowPtr window(obj);
    if (window.IsNull())
      return;

    WindowList::iterator it = find(_view_window_list.begin(),
                                   _view_window_list.end(), window);
    if (it == _view_window_list.end())
    {
      // look for a weak pointer that has been cleared out.
      it = find(_view_window_list.begin(),
                _view_window_list.end(), WeakBaseWindowPtr());
    }

    if (it != _view_window_list.end())
    {
      _view_window_list.erase(it);

      if (_view_window_list.size())
        m_SelectedWindow = _view_window_list.front();
    }
    _window_to_texture_map.erase(window.GetPointer());
  }

  Area* WindowCompositor::GetMouseOwnerArea()
  {
    return mouse_owner_area_;
  }

  //! Get Mouse position relative to the top left corner of the window.
  Point WindowCompositor::GetMousePosition()
  {
    return _mouse_position;
  }

  void WindowCompositor::ResetMousePointerAreas()
  {
    SetMouseOverArea(NULL);
    SetMouseOwnerArea(NULL);
    _mouse_over_menu_page   = NULL;
    _mouse_owner_menu_page  = NULL;
  }

  void WindowCompositor::GetAreaUnderMouse(const Point& mouse_position,
                                           NuxEventType event_type,
                                           InputArea** area_under_mouse_pointer,
                                           BaseWindow** window)
  {
    *area_under_mouse_pointer = NULL;

    // Go through the list of BaseWindo and find the first area over which the
    // mouse pointer is.
    WindowList::iterator window_it;

    for (window_it = _view_window_list.begin(); window_it != _view_window_list.end(); ++window_it)
    {
      if ((*window_it).IsValid() && (*window_it)->IsVisible())
      {
        InputArea *area = static_cast<InputArea*>((*window_it)->FindAreaUnderMouse(mouse_position, event_type));
        if (area)
        {
          *area_under_mouse_pointer = area;
          *window = (*window_it).GetPointer();
          return;
        }
      }
    }

    // If area_under_mouse_pointer is NULL, then the mouse pointer is not over
    // any of the BaseWindow. Try the main window layout.
    if (*area_under_mouse_pointer == NULL)
    {
      Layout* main_window_layout = window_thread_->GetLayout();
      if (main_window_layout)
        *area_under_mouse_pointer = static_cast<InputArea*>(main_window_layout->FindAreaUnderMouse(mouse_position, event_type));
    }
  }

  void WindowCompositor::OnMouseOverViewDestroyed(Object* object)
  {
    if (mouse_over_area_ == object)
    {
      mouse_over_area_ = NULL;
    }
  }

  void WindowCompositor::SetMouseOverArea(Area* area)
  {
    mouse_over_area_ = static_cast<InputArea*>(area);

    mouse_over_view_connection_.disconnect();

    if (mouse_over_area_)
    {
      mouse_over_view_connection_ = mouse_over_area_->object_destroyed.connect(sigc::mem_fun(this, &WindowCompositor::OnMouseOverViewDestroyed));
    }
  }

  void WindowCompositor::OnMouseOwnerViewDestroyed(Object* object)
  {
    if (mouse_owner_area_ == object)
    {
      mouse_owner_area_ = NULL;
    }
  }

  void WindowCompositor::SetMouseOwnerArea(Area* area)
  {
    if (mouse_owner_area_ == area)
      return;

    mouse_owner_area_ = static_cast<InputArea*>(area);
    dnd_safety_x_ = 0;
    dnd_safety_y_ = 0;

    mouse_owner_view_connection_.disconnect();

    if (mouse_owner_area_)
    {
      mouse_owner_view_connection_ = mouse_owner_area_->object_destroyed.connect(sigc::mem_fun(this, &WindowCompositor::OnMouseOwnerViewDestroyed));
    }
  }

  void WindowCompositor::OnMouseOwnerBaseWindowDestroyed(Object* object)
  {
    if (mouse_owner_base_window_ == object)
    {
      mouse_owner_base_window_ = NULL;
    }
  }

  void WindowCompositor::SetMouseOwnerBaseWindow(BaseWindow* base_window)
  {
    mouse_owner_base_window_ = base_window;

    mouse_owner_basewindow_connection_.disconnect();

    if (mouse_owner_base_window_)
    {
      mouse_owner_basewindow_connection_ = mouse_owner_base_window_->object_destroyed.connect(sigc::mem_fun(this, &WindowCompositor::OnMouseOwnerBaseWindowDestroyed));
    }
  }

  void WindowCompositor::DndEventCycle(Event &event)
  {
    if (event.e_event == NUX_DND_MOVE)
    {
      InputArea *hit_area = NULL;
      BaseWindow *hit_base_window = NULL;

      GetAreaUnderMouse(Point(event.e_x, event.e_y), event.e_event, &hit_area, &hit_base_window);

      if (hit_area)
      {
        SetDnDArea(hit_area);
        hit_area->HandleDndMove(event);
      }
      else
      {
        ResetDnDArea();
      }
    }
    else if (event.e_event == NUX_DND_ENTER_WINDOW)
    {
      // do nothing for now
    }
    else if (event.e_event == NUX_DND_LEAVE_WINDOW)
    {
      ResetDnDArea();
    }
    else if (event.e_event == NUX_DND_DROP)
    {
      InputArea *current_dnd_area = GetDnDArea();
      if (current_dnd_area->GetGeometry().IsPointInside(event.e_x - event.e_x_root, event.e_y - event.e_y_root))
        current_dnd_area->HandleDndDrop(event);
    }
  }

  void WindowCompositor::MouseEventCycle(Event &event)
  {
    // mouse_owner_area_: the view that has the mouse down
    // mouse_over_area_: the view that is directly below the mouse pointer

    int dx = event.e_x - _mouse_position.x;
    int dy = event.e_y - _mouse_position.y;

    _mouse_position = Point(event.e_x, event.e_y);

    if (mouse_owner_area_ == NULL)
    {
      // Context: The left mouse button is not down over an area.
      // We look for the area where the mouse pointer is located.
      
      // NUX_MOUSE_RELEASED is tipically processed in cases where mouse_owner_area_ is not NULL.
      // See below for the case when NUX_MOUSE_RELEASED is processed here while mouse_owner_area_ is NULL.
      if ((event.e_event == NUX_MOUSE_PRESSED) ||
        (event.e_event == NUX_MOUSE_MOVE) ||
        (event.e_event == NUX_MOUSE_DOUBLECLICK) ||
        (event.e_event == NUX_MOUSE_WHEEL) ||
        (event.e_event == NUX_WINDOW_MOUSELEAVE) ||
        (event.e_event == NUX_MOUSE_RELEASED))
      {
        InputArea* hit_view = NULL;         // The view under the mouse
        BaseWindow* hit_base_window = NULL; // The BaseWindow below the mouse pointer.

        // Look for the area below the mouse pointer in the BaseWindow.
        Area *pointer_grab_area = GetPointerGrabArea();
        if (pointer_grab_area)
        {
          // If there is a pending mouse pointer grab, test that area only
          hit_view = NUX_STATIC_CAST(InputArea*, pointer_grab_area->FindAreaUnderMouse(Point(event.e_x, event.e_y), event.e_event));
          if ((hit_view == NULL) && (event.e_event == NUX_MOUSE_PRESSED))
          {
            Geometry geo = pointer_grab_area->GetAbsoluteGeometry();
            int x = event.e_x - geo.x;
            int y = event.e_y - geo.y;

            NUX_STATIC_CAST(InputArea*, pointer_grab_area)->EmitMouseDownOutsideArea(x, y, event.GetMouseState(), event.GetKeyState());
          }
        }
        else
        {
          GetAreaUnderMouse(Point(event.e_x, event.e_y), event.e_event, &hit_view, &hit_base_window);
          SetMouseOwnerBaseWindow(hit_base_window);
        }

        Geometry hit_view_geo;
        int hit_view_x = 0;
        int hit_view_y = 0;

        if (hit_view)
        {
          hit_view_geo = hit_view->GetAbsoluteGeometry();
          hit_view_x = event.e_x - hit_view_geo.x;
          hit_view_y = event.e_y - hit_view_geo.y;
        }

        if (event.e_event == NUX_WINDOW_MOUSELEAVE)
        {
          if (mouse_over_area_ != NULL)
          {
            // The area where the mouse was in the previous cycle and the area returned by GetAreaUnderMouse are different.
            // The area from the previous cycle receive a "mouse leave signal".
            Geometry geo = mouse_over_area_->GetAbsoluteGeometry();
            int x = event.e_x - geo.x;
            int y = event.e_y - geo.y;

            mouse_over_area_->EmitMouseLeaveSignal(x, y, event.GetMouseState(), event.GetKeyState());
            SetMouseOverArea(NULL);
          }
        }
        else if (hit_view && (event.e_event == NUX_MOUSE_MOVE))
        {
          if (hit_view != mouse_over_area_)
          {
            if (mouse_over_area_ != NULL)
            {
              // The area where the mouse was in the previous cycle and the area returned by GetAreaUnderMouse are different.
              // The area from the previous cycle receive a "mouse leave signal".
              Geometry geo = mouse_over_area_->GetAbsoluteGeometry();
              int x = event.e_x - geo.x;
              int y = event.e_y - geo.y;

              mouse_over_area_->EmitMouseLeaveSignal(x, y, event.GetMouseState(), event.GetKeyState());
            }

            // The area we found under the mouse pointer receives a "mouse enter signal".
            SetMouseOverArea(hit_view);
            mouse_over_area_->EmitMouseEnterSignal(hit_view_x, hit_view_y, event.GetMouseState(), event.GetKeyState());
          }

          // Send a "mouse mouse signal".
          mouse_over_area_->EmitMouseMoveSignal(hit_view_x, hit_view_y, dx, dy, event.GetMouseState(), event.GetKeyState());
        }
        else if (hit_view && ((event.e_event == NUX_MOUSE_PRESSED) || (event.e_event == NUX_MOUSE_DOUBLECLICK)))
        {
          if ((event.e_event == NUX_MOUSE_DOUBLECLICK) && (!hit_view->DoubleClickEnabled()))
          {
            // If the area does not accept double click events, transform the event into a mouse pressed.
            event.e_event = NUX_MOUSE_PRESSED;
          }

          bool emit_double_click_signal = false;
          if (mouse_over_area_ && (hit_view != mouse_over_area_))
          {
            // The area where the mouse was in the previous cycle and the area returned by GetAreaUnderMouse are different.
            // The area from the previous cycle receive a "mouse leave signal".
            // This case should be rare. I would happen if the mouse is over an area and that area is removed and reveals
            // a new area. If the next mouse event is a NUX_MOUSE_PRESSED, then the revealed area will be the one 
            // that is returned by GetAreaUnderMouse.
            Geometry geo = mouse_over_area_->GetAbsoluteGeometry();
            int x = event.e_x - geo.x;
            int y = event.e_y - geo.y;

            mouse_over_area_->EmitMouseLeaveSignal(x, y, event.GetMouseState(), event.GetKeyState());
          }
          else if (mouse_over_area_ && (hit_view == mouse_over_area_) && (event.e_event == NUX_MOUSE_DOUBLECLICK))
          {
            // Double click is emitted, if the second click happened on the same area as the first click.
            // This means mouse_over_area_ is not null and is equal to hit_view.
            emit_double_click_signal = true;
          }

          SetMouseOverArea(hit_view);
          SetMouseOwnerArea(hit_view);
          _mouse_position_on_owner = Point(hit_view_x, hit_view_y);

          // In the case of a mouse down event, if there is currently a keyboard event receiver and it is different
          // from the area returned by GetAreaUnderMouse, then stop that receiver from receiving anymore keyboard events and switch
          // make mouse_over_area_ the new receiver(if it accept keyboard events).
          if (mouse_over_area_ != GetKeyFocusArea() and mouse_over_area_->AcceptKeyNavFocusOnMouseDown())
          {
            InputArea* grab_area = GetKeyboardGrabArea();
            if (grab_area)
            {
              if (mouse_over_area_->IsChildOf(grab_area) /*&& mouse_over_area_->AcceptKeyboardEvent()*/)
              {
                SetKeyFocusArea(mouse_over_area_);
              }
              else
              {
                SetKeyFocusArea(grab_area);
              }
            }
            else
            {
              SetKeyFocusArea(mouse_over_area_);
            }
          }

          if (emit_double_click_signal)
          {
            mouse_over_area_->EmitMouseDoubleClickSignal(hit_view_x, hit_view_y, event.GetMouseState(), event.GetKeyState());
          }
          else
          {
            mouse_over_area_->EmitMouseDownSignal(hit_view_x, hit_view_y, event.GetMouseState(), event.GetKeyState());
          }
        }
        else if (hit_view && (event.e_event == NUX_MOUSE_WHEEL))
        {
          hit_view->EmitMouseWheelSignal(hit_view_x, hit_view_y, event.e_wheeldelta, event.GetMouseState(), event.GetKeyState());
        }
        else if (hit_view && (event.e_event == NUX_MOUSE_RELEASED))
        {
          // We only get a NUX_MOUSE_RELEASED event when the mouse was pressed
          // over another area and released here. There are a few situations that can cause 
          // mouse_owner_area_ to be NULL on a NUX_MOUSE_RELEASED event:
          //  - The mouse down event happens on a area. That area is set into mouse_owner_area_.
          //    Then the area is destroyed, before the mouse is released.
          //  - The mouse down event happens. Then a call to AddGrabPointer triggers a call to 
          //    ResetMousePointerAreas. mouse_owner_area_ is then set to NULL.

          hit_view->EmitMouseUpSignal(hit_view_x, hit_view_y, event.GetMouseState(), event.GetKeyState());
        }
        else if (hit_view == NULL)
        {
          if (mouse_over_area_)
          {
            Geometry geo = mouse_over_area_->GetAbsoluteGeometry();
            int x = event.e_x - geo.x;
            int y = event.e_y - geo.y;

            // Mouse wheel events are stationary. The mouse can remain inside an area while the mouse wheel is spinning.
            // This shouldn't qualify as a mouse leave event.
            if (event.e_event != NUX_MOUSE_WHEEL)
            {
              mouse_over_area_->EmitMouseLeaveSignal(x, y, event.GetMouseState(), event.GetKeyState());
            }
          }

//           if (GetKeyFocusArea() && (event.e_event == NUX_MOUSE_PRESSED))
//           {
//             InputArea* grab_area = GetKeyFocusArea();
// 
//             if (grab_area)
//             {
//               SetKeyFocusArea(grab_area);
//             }
//             else
//             {
//               SetKeyFocusArea(NULL);
//             }
//           }
          SetMouseOverArea(NULL);
        }
      }
    }
    else
    {
      // Context: The left mouse button down over an area. All events goes to that area.
      // But we still need to know where the mouse is.

      InputArea* hit_view = NULL;         // The view under the mouse
      BaseWindow* hit_base_window = NULL; // The BaseWindow below the mouse pointer.

      GetAreaUnderMouse(Point(event.e_x, event.e_y), event.e_event, &hit_view, &hit_base_window);

      Geometry mouse_owner_geo = mouse_owner_area_->GetAbsoluteGeometry();
      int mouse_owner_x = event.e_x - mouse_owner_geo.x;
      int mouse_owner_y = event.e_y - mouse_owner_geo.y;

      // the mouse is down over a view
      if (event.e_event == NUX_MOUSE_MOVE)
      {
        int dx = mouse_owner_x - _mouse_position_on_owner.x;
        int dy = mouse_owner_y - _mouse_position_on_owner.y;

        if (mouse_owner_area_->_dnd_enabled_as_source)
        {
          dnd_safety_x_ += dx;
          dnd_safety_y_ += dy;

          if (abs(dnd_safety_y_) > 30 || abs(dnd_safety_x_) > 30)
          {
#ifdef NUX_OS_LINUX
            mouse_owner_area_->StartDragAsSource();
#endif
            ResetMousePointerAreas();
            return;
          }
        }
        else
        {
          mouse_owner_area_->EmitMouseDragSignal(mouse_owner_x, mouse_owner_y, dx, dy, event.GetMouseState(), event.GetKeyState());
        }

        if ((mouse_over_area_ == mouse_owner_area_) && (hit_view != mouse_owner_area_))
        {
          mouse_owner_area_->EmitMouseLeaveSignal(mouse_owner_x, mouse_owner_y, event.GetMouseState(), event.GetKeyState());
          SetMouseOverArea(hit_view);
        }
        else if ((mouse_over_area_ != mouse_owner_area_) && (hit_view == mouse_owner_area_))
        {
          mouse_owner_area_->EmitMouseEnterSignal(mouse_owner_x, mouse_owner_y, event.GetMouseState(), event.GetKeyState());
          SetMouseOverArea(mouse_owner_area_);
        }

        _mouse_position_on_owner = Point(mouse_owner_x, mouse_owner_y);
      }
      else if (event.e_event == NUX_MOUSE_RELEASED)
      {
        mouse_owner_area_->EmitMouseUpSignal(mouse_owner_x, mouse_owner_y, event.GetMouseState(), event.GetKeyState());

        if (hit_view == mouse_owner_area_)
        {
          mouse_owner_area_->EmitMouseClickSignal(mouse_owner_x, mouse_owner_y, event.GetMouseState(), event.GetKeyState());
          SetMouseOverArea(mouse_owner_area_);
        }
        else
        {
          SetMouseOverArea(hit_view);
        }

        SetMouseOwnerArea(NULL);
        _mouse_position_on_owner = Point(0, 0);
      }
    }
  }

  void WindowCompositor::MenuEventCycle(Event& event)
  {
    // _mouse_owner_menu_page: the menu page that has the mouse down
    // _mouse_over_menu_page: the menu page that is directly below the mouse pointer

    _mouse_position = Point(event.e_x, event.e_y);

    if (_mouse_owner_menu_page == NULL)
    {
      if ((event.e_event == NUX_MOUSE_PRESSED) ||
        (event.e_event == NUX_MOUSE_RELEASED) ||
        (event.e_event == NUX_MOUSE_MOVE) ||
        (event.e_event == NUX_MOUSE_DOUBLECLICK) ||
        (event.e_event == NUX_MOUSE_WHEEL))
      {
        // Find the MenuPage under the mouse
        MenuPage* hit_menu_page = NULL;
        std::list<MenuPage*>::iterator menu_it;
        for (menu_it = _menu_chain->begin(); menu_it != _menu_chain->end(); menu_it++)
        {
          // The leaf of the menu chain is in the front of the list.
          hit_menu_page = NUX_STATIC_CAST(MenuPage*, (*menu_it)->FindAreaUnderMouse(Point(event.e_x, event.e_y), event.e_event));
          if (hit_menu_page)
          {
            break;
          }
        }

        Geometry hit_menu_page_geo;
        int hit_menu_page_x = 0;
        int hit_menu_page_y = 0;

        if (hit_menu_page)
        {
          hit_menu_page_geo = hit_menu_page->GetAbsoluteGeometry();
          hit_menu_page_x = event.e_x - hit_menu_page_geo.x;
          hit_menu_page_y = event.e_y - hit_menu_page_geo.y;
        }

        if (hit_menu_page && (event.e_event == NUX_MOUSE_RELEASED))
        {
          hit_menu_page->EmitMouseUpSignal(hit_menu_page_x, hit_menu_page_y, event.GetMouseState(), event.GetKeyState());

          (*_menu_chain->begin())->sigClosingMenu(*_menu_chain->begin());
          (*_menu_chain->begin())->StopMenu();
        }
        else if (hit_menu_page && (event.e_event == NUX_MOUSE_MOVE))
        {
          if (hit_menu_page != _mouse_over_menu_page)
          {
            if (_mouse_over_menu_page != 0)
            {
              Geometry geo = _mouse_over_menu_page->GetAbsoluteGeometry();
              int x = event.e_x - geo.x;
              int y = event.e_y - geo.y;

              _mouse_over_menu_page->EmitMouseLeaveSignal(x, y, event.GetMouseState(), event.GetKeyState());
            }

            _mouse_over_menu_page = hit_menu_page;
            _mouse_over_menu_page->EmitMouseEnterSignal(hit_menu_page_x, hit_menu_page_y, event.GetMouseState(), event.GetKeyState());
          }

          _mouse_over_menu_page->EmitMouseMoveSignal(hit_menu_page_x, hit_menu_page_y, event.e_dx, event.e_dy, event.GetMouseState(), event.GetKeyState());
        }
        else if (hit_menu_page && ((event.e_event == NUX_MOUSE_PRESSED) || (event.e_event == NUX_MOUSE_DOUBLECLICK)))
        {
          if (!hit_menu_page->DoubleClickEnabled())
          {

          }

          if (_mouse_over_menu_page && (hit_menu_page != _mouse_over_menu_page))
          {
            Geometry geo = _mouse_over_menu_page->GetAbsoluteGeometry();
            int x = event.e_x - geo.x;
            int y = event.e_y - geo.y;

            _mouse_over_menu_page->EmitMouseLeaveSignal(x, y, event.GetMouseState(), event.GetKeyState());
          }

          _mouse_over_menu_page = hit_menu_page;
          _mouse_owner_menu_page = hit_menu_page;
          _mouse_position_on_owner = Point(hit_menu_page_x, hit_menu_page_y);

          if (_mouse_over_menu_page != GetKeyFocusArea())
          {
            if (_mouse_over_menu_page->AcceptKeyboardEvent())
              SetKeyFocusArea(_mouse_over_menu_page);
          }

          _mouse_over_menu_page->EmitMouseDownSignal(hit_menu_page_x, hit_menu_page_y, event.GetMouseState(), event.GetKeyState());
        }
        else if (hit_menu_page && (event.e_event == NUX_MOUSE_WHEEL))
        {
          hit_menu_page->EmitMouseWheelSignal(hit_menu_page_x, hit_menu_page_y, event.e_wheeldelta, event.GetMouseState(), event.GetKeyState());
        }
        else if (hit_menu_page == NULL)
        {
          if (_mouse_over_menu_page)
          {
            Geometry geo = _mouse_over_menu_page->GetAbsoluteGeometry();
            int x = event.e_x - geo.x;
            int y = event.e_y - geo.y;

            _mouse_over_menu_page->EmitMouseLeaveSignal(x, y, event.GetMouseState(), event.GetKeyState());
          }

          if (event.e_event == NUX_MOUSE_PRESSED || event.e_event == NUX_MOUSE_DOUBLECLICK)
          {
            (*_menu_chain->begin())->sigClosingMenu(*_menu_chain->begin());
            (*_menu_chain->begin())->StopMenu();

            on_menu_closure_continue_with_event_ = (*_menu_chain->begin())->OnClosureContinueEventCycle();
          }
          _mouse_over_menu_page = 0;
        }
      }
    }
    else
    {
      // We should never get here for a NUX_MOUSE_PRESSED event.
      MenuPage* hit_menu_page = NULL;
      std::list<MenuPage*>::iterator menu_it;
      for (menu_it = _menu_chain->begin(); menu_it != _menu_chain->end(); menu_it++)
      {
        // The leaf of the menu chain is in the front of the list.
        hit_menu_page = NUX_STATIC_CAST(MenuPage*, (*menu_it)->FindAreaUnderMouse(Point(event.e_x, event.e_y), event.e_event));
        if (hit_menu_page)
        {
          break;
        }
      }

      Geometry mouse_owner_geo = _mouse_owner_menu_page->GetAbsoluteGeometry();
      int mouse_owner_x = event.e_x - mouse_owner_geo.x;
      int mouse_owner_y = event.e_y - mouse_owner_geo.y;

      // the mouse is down over a view
      if (event.e_event == NUX_MOUSE_MOVE)
      {
        int dx = mouse_owner_x - _mouse_position_on_owner.x;
        int dy = mouse_owner_y - _mouse_position_on_owner.y;

        _mouse_owner_menu_page->EmitMouseDragSignal(mouse_owner_x, mouse_owner_y, dx, dy, event.GetMouseState(), event.GetKeyState());

        if ((_mouse_over_menu_page == _mouse_owner_menu_page) && (hit_menu_page != _mouse_owner_menu_page))
        {
          _mouse_owner_menu_page->EmitMouseLeaveSignal(mouse_owner_x, mouse_owner_y, event.GetMouseState(), event.GetKeyState());
          _mouse_over_menu_page = hit_menu_page;
        }
        else if ((_mouse_over_menu_page != _mouse_owner_menu_page) && (hit_menu_page == _mouse_owner_menu_page))
        {
          _mouse_owner_menu_page->EmitMouseEnterSignal(mouse_owner_x, mouse_owner_y, event.GetMouseState(), event.GetKeyState());
          _mouse_over_menu_page = _mouse_owner_menu_page;
        }

        _mouse_position_on_owner = Point(mouse_owner_x, mouse_owner_y);
      }
      else if (event.e_event == NUX_MOUSE_RELEASED)
      {
        _mouse_owner_menu_page->EmitMouseUpSignal(mouse_owner_x, mouse_owner_y, event.GetMouseState(), event.GetKeyState());

        if (hit_menu_page == _mouse_owner_menu_page)
        {
          _mouse_owner_menu_page->EmitMouseClickSignal(mouse_owner_x, mouse_owner_y, event.GetMouseState(), event.GetKeyState());
          _mouse_over_menu_page = _mouse_owner_menu_page;
        }
        else
        {
          _mouse_over_menu_page = hit_menu_page;
        }

        (*_menu_chain->begin())->sigClosingMenu(*_menu_chain->begin());
        (*_menu_chain->begin())->StopMenu();

        _mouse_owner_menu_page = NULL;
        _mouse_position_on_owner = Point(0, 0);
      }
    }
  }

  void WindowCompositor::FindKeyFocusArea(NuxEventType event_type,
    unsigned int key_symbol,
    unsigned int special_keys_state,
    InputArea** key_focus_area,
    BaseWindow** window)
  {
    *key_focus_area = NULL;
    *window = NULL;

    // Go through the list of BaseWindos and find the first area over which the mouse pointer is.
    WindowList::iterator window_it;
    window_it = _view_window_list.begin();
    while ((*key_focus_area == NULL) && (window_it != _view_window_list.end()))
    {
      if ((*window_it).IsValid() && (*window_it)->IsVisible())
      {
        *key_focus_area = NUX_STATIC_CAST(InputArea*, (*window_it)->FindKeyFocusArea(event_type, key_symbol, special_keys_state));
        if (key_focus_area)
        {
          // We have found an area. We are going to exit the while loop.
          *window = (*window_it).GetPointer();
        }
      }
      ++window_it;
    }

    // If key_focus_area is NULL, then try the main window layout.
    if (*key_focus_area == NULL)
    {
      Layout* main_window_layout = window_thread_->GetLayout();
      if (main_window_layout)
      {
        *key_focus_area = NUX_STATIC_CAST(InputArea*, main_window_layout->FindKeyFocusArea(event_type, key_symbol, special_keys_state));
      }
    }
  }

  void WindowCompositor::FindKeyFocusAreaFrom(NuxEventType event_type,
    unsigned int key_symbol,
    unsigned int special_keys_state,
    InputArea* root_search_area,
    InputArea** key_focus_area,
    BaseWindow** window)
  {
    *key_focus_area = NULL;
    *window = NULL;

    if (root_search_area == NULL)
    {
      return;
    }

    *key_focus_area = NUX_STATIC_CAST(InputArea*, root_search_area->FindKeyFocusArea(event_type, key_symbol, special_keys_state));
    if (key_focus_area)
    {
      *window = NUX_STATIC_CAST(BaseWindow*, root_search_area->GetTopLevelViewWindow());
    }
  }

  void WindowCompositor::SendKeyEvent(InputArea* input_area,
    NuxEventType event_type,
    unsigned int key_sym,
    unsigned long x11_key_code,
    unsigned long special_keys_state,
    const char* text,
    int key_repeat_count)
  {
    if (input_area == NULL)
      return;

    if (event_type == NUX_KEYDOWN)
    {
      input_area->EmitKeyEventSignal(event_type,
        key_sym,
        special_keys_state,
        text,
        key_repeat_count);
    }
    else if (event_type == NUX_KEYUP)
    {
      input_area->EmitKeyUpSignal(key_sym, x11_key_code, special_keys_state);
    }
  }

  void WindowCompositor::KeyboardEventCycle(Event &event)
  {
    InputArea* keyboard_event_grab_view = GetKeyboardGrabArea();

    InputArea* focus_area = NULL;   // The view under the mouse
    BaseWindow* base_window = NULL; // The BaseWindow below the mouse pointer.

    if (keyboard_event_grab_view)
    {
      // There is a keyboard grab.
      // Find the key focus area, under the keyboard grab area. That is to say, the key focus area is in the widget tree 
      // whose root is the keyboard grab area. This phase is known as the capture phase.
      
      FindKeyFocusAreaFrom(event.e_event, event.GetKeySym(), event.GetKeyState(),
        keyboard_event_grab_view,
        &focus_area,
        &base_window);
    }
    else
    {
      FindKeyFocusArea(event.e_event, event.GetKeySym(), event.GetKeyState(),
        &focus_area,
        &base_window);
    }

    if (focus_area)
      SetKeyFocusArea(focus_area);
    else
      SetKeyFocusArea(NULL);

    KeyNavDirection direction = KEY_NAV_NONE;

    switch(event.GetKeySym())
    {
    case NUX_VK_UP:
      direction = KEY_NAV_UP;
      break;
    case NUX_VK_DOWN:
      direction = KEY_NAV_DOWN;
      break;
    case NUX_VK_LEFT:
      direction = KEY_NAV_LEFT;
      break;
    case NUX_VK_RIGHT:
      direction = KEY_NAV_RIGHT;
      break;
    case NUX_VK_LEFT_TAB:
      direction = KEY_NAV_TAB_PREVIOUS;
      break;
    case NUX_VK_TAB:
      direction = KEY_NAV_TAB_NEXT;
      break;
    case NUX_VK_ENTER:
    case NUX_KP_ENTER:
      // Not sure if Enter should be a navigation key
      direction = KEY_NAV_ENTER;
      break;
    default:
      direction = KEY_NAV_NONE;
      break;
    }

    if (key_focus_area_)
    {
      if (key_focus_area_->InspectKeyEvent(event.e_event, event.GetKeySym(), event.GetText()))
      {
        SendKeyEvent(key_focus_area_,
                    event.e_event,
                    event.GetKeySym(),
                    event.e_x11_keycode,
                    event.GetKeyState(),
                    event.GetText(),
                    event.GetKeyRepeatCount());
      }
      else if (direction == KEY_NAV_NONE)
      {
        Area* parent = key_focus_area_->GetParentObject();

        while (parent && !parent->InspectKeyEvent(event.e_event, event.GetKeySym(), event.GetText()))
        {
          parent = parent->GetParentObject();
        }

        if (parent)
        {
          SendKeyEvent(static_cast<InputArea*>(parent),
            event.e_event,
            event.GetKeySym(),
            event.e_x11_keycode,
            event.GetKeyState(),
            event.GetText(),
            event.GetKeyRepeatCount());
        }
      }
      else if (event.e_event == NUX_KEYDOWN)
      {        
        if (direction == KEY_NAV_ENTER)
        {
          if (key_focus_area_ && key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
          {
            static_cast<InputArea*>(key_focus_area_)->OnKeyNavFocusActivate.emit(key_focus_area_);
          }
        }
        else
        {
          InputArea* key_nav_focus = NULL;
          Area* parent = key_focus_area_->GetParentObject();
          
          if (parent)
            key_nav_focus = NUX_STATIC_CAST(InputArea*, parent->KeyNavIteration(direction));
          
          while (key_nav_focus == NULL && parent != NULL)
          {
            parent = parent->GetParentObject();
            if (parent)
              key_nav_focus = NUX_STATIC_CAST(InputArea*, parent->KeyNavIteration(direction));
          }

          if (key_nav_focus)
          {
            SetKeyFocusArea(key_nav_focus);
          }
        }
      }
    }
    else
    {
      // The event is for the Application itself
    }
  }

  // NUXTODO: rename as EventCycle
  void WindowCompositor::ProcessEvent(Event &event)
  {
    inside_event_cycle_ = true;
    if (_enable_nux_new_event_architecture)
    {
      if (((event.e_event >= NUX_MOUSE_PRESSED) && (event.e_event <= NUX_MOUSE_WHEEL)) ||
      (event.e_event == NUX_WINDOW_MOUSELEAVE))
      {
        bool menu_active = false;
        if (_menu_chain->size())
        {
          menu_active = true;
          MenuEventCycle(event);
          CleanMenu();
        }

        if ((menu_active && on_menu_closure_continue_with_event_) || !(menu_active))
        {
          MouseEventCycle(event);
        }

        on_menu_closure_continue_with_event_ = false;

        if (_starting_menu_event_cycle)
        {
          _starting_menu_event_cycle = false;
        }
      }
      else if ((event.e_event >= NUX_KEYDOWN) && (event.e_event <= NUX_KEYUP))
      {
        KeyboardEventCycle(event);
      }
      else if ((event.e_event >= NUX_DND_MOVE) && (event.e_event <= NUX_DND_LEAVE_WINDOW))
      {
        DndEventCycle(event);
      }
    }
    inside_event_cycle_ = false;
  }

  void WindowCompositor::StartModalWindow(ObjectWeakPtr<BaseWindow> window)
  {
    if (window == 0)
      return;

    WindowList::iterator it = find(_modal_view_window_list.begin(), _modal_view_window_list.end(), window);

    if (it == _modal_view_window_list.end())
    {
      _modal_view_window_list.push_front(window);
    }
  }

  void WindowCompositor::StopModalWindow(ObjectWeakPtr<BaseWindow> window)
  {
    if (_modal_view_window_list.size() > 0)
    {
      if (*_modal_view_window_list.begin() == window)
        _modal_view_window_list.pop_front();
    }
  }

  //! Push a floating view at the top of the stack.
  void WindowCompositor::PushToFront(BaseWindow *window)
  {
    if (window == 0)
      return;

    WindowList::iterator it = find(_view_window_list.begin(), _view_window_list.end(), window);

    if (it != _view_window_list.end())
    {
      _view_window_list.erase(it);
      _view_window_list.push_front(ObjectWeakPtr<BaseWindow> (window));
    }

    EnsureAlwaysOnFrontWindow();
  }

  //! Push a floating view at the bottom of the stack.
  void WindowCompositor::PushToBack(BaseWindow *window)
  {
    if (window == 0)
      return;

    if (window == _always_on_front_window)
      return;

    WindowList::iterator it = find(_view_window_list.begin(), _view_window_list.end(), window);

    if (it != _view_window_list.end())
    {
      _view_window_list.erase(it);
      _view_window_list.push_back(ObjectWeakPtr<BaseWindow> (window));
    }

    EnsureAlwaysOnFrontWindow();
  }

  //! Push a floating view just above another floating view.
  void WindowCompositor::PushHigher(BaseWindow *top_floating_view, BaseWindow *bottom_floating_view, bool strict)
  {
    NUX_RETURN_IF_NULL(bottom_floating_view);
    NUX_RETURN_IF_NULL(top_floating_view);
    NUX_RETURN_IF_FALSE(bottom_floating_view != top_floating_view)

    WindowList::iterator it;
    WindowList::iterator it_top;
    WindowList::iterator it_bot;

    int i = 0;
    int top_pos = -1;
    int bot_pos = -1;

    for (it_top = _view_window_list.begin(), i = 0; it_top != _view_window_list.end(); it_top++, i++)
    {
      if (*it == bottom_floating_view)
      {
        it_bot = it;
        bot_pos = i;
      }

      if (*it == top_floating_view)
      {
        it_top = it;
        top_pos = i;
      }

      if ((top_pos >= 0) && (bot_pos >= 0))
        break;
    }

    if ((it_top == _view_window_list.end()) || (it_bot == _view_window_list.end()))
    {
      return;
    }

    if ((top_pos < bot_pos) && (strict == false))
    {
      _view_window_list.erase(it_top);
      _view_window_list.insert(it_bot, ObjectWeakPtr<BaseWindow> (top_floating_view));
    }

    EnsureAlwaysOnFrontWindow();
  }

  void WindowCompositor::SetAlwaysOnFrontWindow(BaseWindow *window)
  {
    _always_on_front_window = ObjectWeakPtr<BaseWindow> (window);

    EnsureAlwaysOnFrontWindow();
  }

  void WindowCompositor::EnsureAlwaysOnFrontWindow()
  {
    // Do not re-order while we are traversing the list of BaseWindow.
    if (inside_event_cycle_)
      return;

    if (_always_on_front_window == NULL)
      return;

    WindowList::iterator always_top_it = find(_view_window_list.begin(), _view_window_list.end(), _always_on_front_window);
    if ((always_top_it != _view_window_list.end()) &&
        (always_top_it != _view_window_list.begin()) &&
        _always_on_front_window.IsValid())
    {
      _view_window_list.erase(always_top_it);
      _view_window_list.push_front(_always_on_front_window);
    }
  }

  InputArea *WindowCompositor::GetExclusiveInputArea()
  {
    return _exclusive_input_area;
  }

  bool WindowCompositor::InExclusiveInputMode()
  {
    return _in_exclusive_input_mode;
  }

  void WindowCompositor::Draw(bool SizeConfigurationEvent, bool force_draw)
  {
    inside_rendering_cycle_ = true;
    if (!window_thread_->GetGraphicsDisplay().isWindowMinimized())
    {
      //int w, h;
      window_thread_->GetGraphicsEngine().GetContextSize(m_Width, m_Height);
      window_thread_->GetGraphicsEngine().SetViewport(0, 0, m_Width, m_Height);
      
      // Reset the Model view Matrix and the projection matrix
      window_thread_->GetGraphicsEngine().ResetProjectionMatrix();
      
      window_thread_->GetGraphicsEngine().ResetModelViewMatrixStack();
      window_thread_->GetGraphicsEngine().Push2DTranslationModelViewMatrix(0.375f, 0.375f, 0.0f);


      if (force_draw || SizeConfigurationEvent)
      {
        // We fall here after something dramatic has happen to the window such as a resizing. In this case
        // everything must be rendered. This is very costly and should happen rarely.
        if (!window_thread_->IsEmbeddedWindow())
          RenderMainWindowComposition(true);

        {
          RenderTopViews(true, _view_window_list, false);
          RenderTopViews(true, _modal_view_window_list, true);

          DrawMenu(true);
          DrawTooltip(true);
          DrawOverlay(true);
        }
      }
      else if (m_PopupRemoved || m_MenuRemoved)
      {
        // A popup removed cause the whole window to be dirty(at least some part of it).
        // So exchange DrawList with a real Draw.
        if (!window_thread_->IsEmbeddedWindow())
          RenderMainWindowComposition(false);

        {
          RenderTopViews(false, _view_window_list, false);
          RenderTopViews(false, _modal_view_window_list, true);

          DrawMenu(true);
          DrawTooltip(true);
          DrawOverlay(true);
        }
      }
      else
      {
        if (!window_thread_->IsEmbeddedWindow())
          RenderMainWindowComposition(false);

        {
          RenderTopViews(false, _view_window_list, false);
          RenderTopViews(false, _modal_view_window_list, true);

          DrawMenu(true);
          DrawTooltip(true);
          DrawOverlay(true);
        }
      }

      m_PopupRemoved = false;
      m_MenuRemoved = false;

      window_thread_->GetGraphicsEngine().Pop2DWindow();
    }
    inside_rendering_cycle_ = false;
  }

  void WindowCompositor::DrawMenu(bool force_draw)
  {
    ObjectWeakPtr<BaseWindow> window = m_MenuWindow;

    if (window.IsValid())
    {
      //window_thread_->GetGraphicsEngine().SetContext(x, y, buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(window_thread_->GetGraphicsEngine().GetWindowWidth(),
          window_thread_->GetGraphicsEngine().GetWindowHeight());
      window_thread_->GetGraphicsEngine().EmptyClippingRegion();
    }
    else
    {
      window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(window_thread_->GetGraphicsEngine().GetWindowWidth(),
          window_thread_->GetGraphicsEngine().GetWindowHeight());
      window_thread_->GetGraphicsEngine().EmptyClippingRegion();
    }

    std::list<MenuPage *>::reverse_iterator rev_it_menu;

    for (rev_it_menu = _menu_chain->rbegin(); rev_it_menu != _menu_chain->rend( ); rev_it_menu++)
    {
      SetProcessingTopView(m_MenuWindow.GetPointer());
      (*rev_it_menu)->ProcessDraw(window_thread_->GetGraphicsEngine(), force_draw);
      SetProcessingTopView(NULL);
    }

//     GetGraphicsDisplay()->GetGraphicsEngine()->SetContext(0, 0,
//                                             window_thread_->GetGraphicsEngine().GetWindowWidth(),
//                                             window_thread_->GetGraphicsEngine().GetWindowHeight());
  }

  void WindowCompositor::DrawOverlay(bool force_draw)
  {
    ObjectWeakPtr<BaseWindow> window = m_OverlayWindow;
    int buffer_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    int buffer_height = window_thread_->GetGraphicsEngine().GetWindowHeight();

    if (window.IsValid())
    {
      //window_thread_->GetGraphicsEngine().SetContext(x, y, buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().EmptyClippingRegion();
    }
    else
      window_thread_->GetGraphicsEngine().SetOpenGLClippingRectangle(0, 0, buffer_width, buffer_height);

    if (OverlayDrawingCommand)
    {
      SetProcessingTopView(m_OverlayWindow.GetPointer());
      OverlayDrawingCommand->OverlayDrawing(window_thread_->GetGraphicsEngine());
      SetProcessingTopView(NULL);
    }

    //GetGraphicsDisplay()->GetGraphicsEngine()->SetContext(0, 0, buffer_width, buffer_height);
  }

  void WindowCompositor::DrawTooltip(bool force_draw)
  {
    ObjectWeakPtr<BaseWindow> window = _tooltip_window;
    int buffer_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    int buffer_height = window_thread_->GetGraphicsEngine().GetWindowHeight();

    if (window.IsValid())
    {
      //window_thread_->GetGraphicsEngine().SetContext(x, y, buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().EmptyClippingRegion();
    }
    else
      window_thread_->GetGraphicsEngine().SetOpenGLClippingRectangle(0, 0, buffer_width, buffer_height);

    if (m_TooltipText.Size())
    {
        //SetProcessingTopView(_tooltip_window);
        GetPainter().PaintShape(window_thread_->GetGraphicsEngine(), _tooltip_geometry, Color(0xA0000000), eSHAPE_CORNER_ROUND10, true);
        GetPainter().PaintTextLineStatic(window_thread_->GetGraphicsEngine(), GetSysBoldFont(), _tooltip_text_geometry, m_TooltipText, Color(0xFFFFFFFF));
        //SetProcessingTopView(NULL);
    }

    //GetGraphicsDisplay()->GetGraphicsEngine()->SetContext(0, 0, buffer_width, buffer_height);
  }

  void WindowCompositor::RenderTopViewContent(BaseWindow *window, bool force_draw)
  {
    GetPainter().EmptyBackgroundStack();
    SetProcessingTopView(window);
    window->ProcessDraw(window_thread_->GetGraphicsEngine(), force_draw || window->IsRedrawNeeded());
    SetProcessingTopView(NULL);
    GetPainter().EmptyBackgroundStack();
  }

  void WindowCompositor::RenderTopViews(bool force_draw,
                                        WindowList& windows_to_render,
                                        bool drawModal)
  {
    // Before anything, deactivate the current frame buffer, set the viewport 
    // to the size of the display and call EmptyClippingRegion().
    // Then call GetScissorRect() to get the size of the global clipping area.
    // This is is hack until we implement SetGlobalClippingRectangle() (the opposite of SetGlobalClippingRectangle).
    GraphicsEngine& graphics_engine = window_thread_->GetGraphicsEngine();
    unsigned int window_width = graphics_engine.GetWindowWidth();
    unsigned int window_height = graphics_engine.GetWindowHeight();
    GetGraphicsDisplay()->GetGpuDevice()->DeactivateFrameBuffer();
    graphics_engine.SetViewport(0, 0, window_width, window_height);
    graphics_engine.EmptyClippingRegion();

    Geometry global_clip_rect = graphics_engine.GetScissorRect();
    global_clip_rect.y = window_height - global_clip_rect.y - global_clip_rect.height;

    // Always make a copy of the windows to render.  We have no control over
    // the windows we are actually drawing.  It has been observed that some
    // windows modify the windows stack during the draw process.
    //
    // So... we take a copy of the window list.  As much as I'd love to just
    // have BaseWindow* in the container, again, we have no control over the
    // windows we are drawing and one may just decide to unregister or destroy
    // another window mid-render.  Since we are contructing a copy of the
    // list, lets reverse it as we are constructing, as we want to draw the
    // windows from back to front.
    WindowList windows(windows_to_render.rbegin(), windows_to_render.rend());
    for (WindowList::iterator it = windows.begin(), end = windows.end(); it != end; ++it)
    {
      WeakBaseWindowPtr& window_ptr = *it;
      if (window_ptr.IsNull())
        continue;

      BaseWindow* window = window_ptr.GetPointer();
      if (!drawModal && window->IsModal())
        continue;

      if (window->IsVisible())
      {
        if (global_clip_rect.Intersect(window->GetGeometry()).IsNull())
        {
          // The global clipping area can be seen as a per monitor clipping
          // region. It is mostly used in embedded mode with compiz.  If we
          // get here, it means that the BaseWindow we want to render is not
          // in area of the monitor that compiz is currently rendering. So
          // skip it.
          continue;
        }

        RenderTargetTextures& rt = GetWindowBuffer(window);

        // Based on the areas that requested a rendering inside the
        // BaseWindow, render the BaseWindow or just use its cache.
        if (force_draw || window->IsRedrawNeeded() || window->ChildNeedsRedraw())
        {
          if (rt.color_rt.IsValid() /*&& rt.depth_rt.IsValid()*/)
          {
            int buffer_width = window->GetBaseWidth();
            int buffer_height = window->GetBaseHeight();

            if ((rt.color_rt->GetWidth() != buffer_width) ||
                (rt.color_rt->GetHeight() != buffer_height))
            {
              rt.color_rt = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
              rt.depth_rt = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
            }

            m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
            m_FrameBufferObject->SetRenderTarget( 0, rt.color_rt->GetSurfaceLevel(0));
            m_FrameBufferObject->SetDepthSurface( rt.depth_rt->GetSurfaceLevel(0));
            m_FrameBufferObject->Activate();
            graphics_engine.SetViewport(0, 0, buffer_width, buffer_height);
            graphics_engine.SetOrthographicProjectionMatrix(buffer_width, buffer_height);
            graphics_engine.EmptyClippingRegion();

            graphics_engine.SetOpenGLClippingRectangle(0, 0, buffer_width, buffer_height);

            CHECKGL( glClearColor(0, 0, 0, 0));
            GLuint clear_color_buffer_bit = (force_draw || window->IsRedrawNeeded()) ? GL_COLOR_BUFFER_BIT : 0;
            CHECKGL( glClear(clear_color_buffer_bit | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
          }
          else
          {
            int x = window->GetBaseX();
            int y = window->GetBaseY();
            Matrix4 mat;
            mat.Translate(x, y, 0);
            graphics_engine.SetOrthographicProjectionMatrix(window_width, window_height);
          }

          RenderTopViewContent(window, force_draw);
        }

        if (rt.color_rt.IsValid())
        {
          m_FrameBufferObject->Deactivate();

          // Nux is done rendering a BaseWindow into a texture. The previous call to Deactivate
          // has cancelled any opengl framebuffer object that was set.

          CHECKGL(glDepthMask(GL_FALSE));
          {
            graphics_engine.ApplyClippingRectangle();
            PresentBufferToScreen(rt.color_rt, window->GetBaseX(), window->GetBaseY(), false, false, window->GetOpacity(), window->premultiply());
          }
          CHECKGL(glDepthMask(GL_TRUE));
          graphics_engine.GetRenderStates().SetBlend(false);
        }

        window->_child_need_redraw = false;
      }
      else
      {
        // Invisible window, nothing to draw.
        window->_child_need_redraw = false;
        window->DoneRedraw();
      }
    }

    m_FrameBufferObject->Deactivate();
  }

  void WindowCompositor::RenderMainWindowComposition(bool force_draw)
  {
    int buffer_width, buffer_height;

    buffer_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    buffer_height = window_thread_->GetGraphicsEngine().GetWindowHeight();

    if ((!m_MainColorRT.IsValid()) || (!m_MainDepthRT.IsValid()) || (m_MainColorRT->GetWidth() != buffer_width) || (m_MainColorRT->GetHeight() != buffer_height))
    {
      m_MainColorRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
      m_MainDepthRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
    }

    m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
    m_FrameBufferObject->SetRenderTarget(0, m_MainColorRT->GetSurfaceLevel(0));
    m_FrameBufferObject->SetDepthSurface(m_MainDepthRT->GetSurfaceLevel(0));
    m_FrameBufferObject->Activate();

    window_thread_->GetGraphicsEngine().EmptyClippingRegion();
    window_thread_->GetGraphicsEngine().SetOpenGLClippingRectangle(0, 0, buffer_width, buffer_height);
    window_thread_->GetGraphicsEngine().SetViewport(0, 0, buffer_width, buffer_height);
    window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(buffer_width, buffer_height);
    {
      CHECKGL(glClear(/*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
      //Begin 2D Drawing
      {
        if (force_draw)
        {
          GetPainter().PushDrawLayer(window_thread_->GetGraphicsEngine(), Geometry(0, 0, buffer_width, buffer_height), m_Background);
          //GetPainter().PushBackground(window_thread_->GetGraphicsEngine(), Geometry(0, 0, buffer_width, buffer_height), m_Background, true);

          window_thread_->ProcessDraw(window_thread_->GetGraphicsEngine(), true);

          nuxAssert(window_thread_->GetGraphicsEngine().GetNumberOfClippingRegions() == 0);
          GetPainter().PopBackground();
          GetPainter().EmptyBackgroundStack();
        }
        else
        {
          GetPainter().PushLayer(window_thread_->GetGraphicsEngine(), Geometry(0, 0, buffer_width, buffer_height), m_Background);
          //GetPainter().PushBackground(window_thread_->GetGraphicsEngine(), Geometry(0, 0, buffer_width, buffer_height), m_Background, false);

          window_thread_->ProcessDraw(window_thread_->GetGraphicsEngine(), false);

          nuxAssert(window_thread_->GetGraphicsEngine().GetNumberOfClippingRegions() == 0);
          GetPainter().PopBackground();
          GetPainter().EmptyBackgroundStack();
        }
      }
      // End 2D Drawing
    }

    if (key_focus_area_)
    {
      // key focus test
      Geometry geo= key_focus_area_->GetRootGeometry();
      //GetGraphicsDisplay()->GetGraphicsEngine()->QRP_Color(geo.x, geo.y, geo.width, geo.height, color::Blue);
    }

    window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(buffer_width, buffer_height);
    m_FrameBufferObject->Deactivate();

    unsigned int window_width, window_height;
    window_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    window_height = window_thread_->GetGraphicsEngine().GetWindowHeight();
    window_thread_->GetGraphicsEngine().EmptyClippingRegion();
    window_thread_->GetGraphicsEngine().SetOpenGLClippingRectangle(0, 0, window_width, window_height);
    window_thread_->GetGraphicsEngine().SetViewport(0, 0, window_width, window_height);
    window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(window_width, window_height);

    PresentBufferToScreen(m_MainColorRT, 0, 0, false);

  }

  void WindowCompositor::PresentBufferToScreen(ObjectPtr<IOpenGLBaseTexture> HWTexture, int x, int y, bool RenderToMainTexture, bool BluredBackground, float opacity, bool premultiply)
  {
    nuxAssert(HWTexture.IsValid());

    if (HWTexture.IsNull())
      return;

    int window_width, window_height;
    window_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    window_height = window_thread_->GetGraphicsEngine().GetWindowHeight();


    if (RenderToMainTexture && (HWTexture != m_MainColorRT))
    {
      nuxAssert(m_MainColorRT->GetWidth() == window_width);
      nuxAssert(m_MainColorRT->GetHeight() == window_height);
      m_FrameBufferObject->FormatFrameBufferObject(window_width, window_height, BITFMT_R8G8B8A8);
      m_FrameBufferObject->SetRenderTarget( 0, m_MainColorRT->GetSurfaceLevel(0));
      m_FrameBufferObject->SetDepthSurface( m_MainDepthRT->GetSurfaceLevel(0));
      m_FrameBufferObject->Activate();
    }
    else
    {
      if (GetWindowThread()->IsEmbeddedWindow() && reference_fbo_)
      {
        // In the context of Unity, we may want Nux to restore a specific fbo and render the
        // BaseWindow texture into it. That fbo is called a reference framebuffer object. if a
        // Reference framebuffer object is present, Nux sets it.
        if (!RestoreReferenceFramebuffer())
        {
          nuxDebugMsg("[WindowCompositor::RenderTopViews] Setting the Reference fbo has failed.");
        }
      }
      else
      {
        GetGraphicsDisplay()->GetGpuDevice()->DeactivateFrameBuffer();
      }
    }

    window_thread_->GetGraphicsEngine().EmptyClippingRegion();
    window_thread_->GetGraphicsEngine().SetOpenGLClippingRectangle(0, 0, window_width, window_height);
    window_thread_->GetGraphicsEngine().SetViewport(0, 0, window_width, window_height);
    window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(window_width, window_height);

    // Render the MAINFBO
    {
      int src_width, src_height;
      src_width = HWTexture->GetWidth();
      src_height = HWTexture->GetHeight();

      TexCoordXForm texxform0;
      texxform0.FlipVCoord(true);

      if (premultiply)
      {
        window_thread_->GetGraphicsEngine().GetRenderStates().SetBlend(true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        GetGraphicsDisplay()->GetGraphicsEngine()->QRP_1Tex(x, y, src_width, src_height, HWTexture, texxform0, Color(opacity, opacity, opacity, opacity));
      }
      else
      {
        window_thread_->GetGraphicsEngine().GetRenderStates().SetBlend(false);
        GetGraphicsDisplay()->GetGraphicsEngine()->QRP_1Tex(x, y, src_width, src_height, HWTexture, texxform0, Color(1.0f, 1.0f, 1.0f, opacity));
      }
      window_thread_->GetGraphicsEngine().GetRenderStates().SetBlend(false);
    }
  }

  void WindowCompositor::AddMenu(MenuPage *menu, BaseWindow *window, bool OverrideCurrentMenuChain)
  {
    if (_menu_chain->size() == 0)
    {
      // A menu is opening.
      _starting_menu_event_cycle = true;
      _menu_is_active = true;
    }

    std::list<MenuPage*>::iterator it = find(_menu_chain->begin(), _menu_chain->end(), menu);
    if (it == _menu_chain->end())
    {
      // When adding a MenuPage, make sure that it is a child of the MenuPage in _menu_chain->begin().
      if (_menu_chain->size())
      {
        if (menu->GetParentMenu() != (*_menu_chain->begin()))
        {
          if (OverrideCurrentMenuChain)
          {
            // Remove the current menu chain
            for (it = _menu_chain->begin(); it != _menu_chain->end(); it++)
            {
              // Stop all pages
              (*it)->StopMenu();
            }

            _menu_chain->clear();
          }
          else
          {
            // The MenuPage argument is not a descendent of the current menu chain.
            return;
          }
        }
      }

      m_MenuWindow = window;
      // The deepest menu is added in front of the list and tested first for events.
      _menu_chain->push_front(menu);
    }
  }

  // Be careful never call this function while you are iterating through the elements of _menu_chain.
  void WindowCompositor::RemoveMenu(MenuPage *menu)
  {
    std::list<MenuPage *>::iterator it = find(_menu_chain->begin(), _menu_chain->end(), menu);

    if (it == _menu_chain->end())
    {
      return;
    }

    _menu_chain->erase(it);
    m_MenuRemoved = true;

    if (_menu_is_active && (_menu_chain->size() == 0))
    {
      // The menu is closed
      _menu_is_active         = false;
      ResetMousePointerAreas();
      m_MenuWindow            = NULL;
    }
  }

  void WindowCompositor::CleanMenu()
  {
    if (_menu_chain->size() == 0)
      return;

    std::list<MenuPage *>::iterator menu_it = _menu_chain->begin();

    while (menu_it != _menu_chain->end())
    {
      if ((*menu_it)->IsActive() == false)
      {
        menu_it = _menu_chain->erase(menu_it);
        m_MenuRemoved = true;
      }
      else
      {
        menu_it++;
      }
    }

    if (_menu_is_active && (_menu_chain->size() == 0))
    {
      _menu_is_active         = false;
      ResetMousePointerAreas();
      m_MenuWindow            = NULL;
    }
  }

  void WindowCompositor::SetWidgetDrawingOverlay(InputArea *ic, BaseWindow* OverlayWindow)
  {
    OverlayDrawingCommand = ic;
    m_OverlayWindow = OverlayWindow;
  }

  InputArea *WindowCompositor::GetWidgetDrawingOverlay()
  {
    return OverlayDrawingCommand;
  }

  void WindowCompositor::SetTooltip(InputArea* TooltipArea, const char *TooltipText, int x, int y)
  {
    _tooltip_window = GetProcessingTopView();
    m_TooltipArea = TooltipArea;
    m_TooltipText = TooltipText;
    m_TooltipX = x;
    m_TooltipY = y;

    if (m_TooltipText.Size())
    {
      int w = GetSysBoldFont()->GetCharStringWidth(m_TooltipText.GetTCharPtr());
      int h = GetSysBoldFont()->GetFontHeight();

      _tooltip_text_geometry = Geometry(
          m_TooltipX + 10,
          m_TooltipY - h/2 - 2,
          w,
          h + 4);

      _tooltip_geometry = _tooltip_text_geometry;
      _tooltip_geometry.OffsetSize(20, 8);
      _tooltip_geometry.OffsetPosition(-10, -4);

      _tooltip_mainwindow_geometry = _tooltip_geometry;

      if (_tooltip_window.IsValid())
      {
        _tooltip_mainwindow_geometry.OffsetPosition(_tooltip_window->GetBaseX(), _tooltip_window->GetBaseY());
      }
    }
    else
    {
      _tooltip_mainwindow_geometry = _tooltip_geometry = _tooltip_text_geometry = Geometry(0, 0, 0, 0);
    }
  }

  Geometry WindowCompositor::GetTooltipGeometry() const
  {
    return _tooltip_geometry;
  }

  Geometry WindowCompositor::GetTooltipMainWindowGeometry() const
  {
      return _tooltip_mainwindow_geometry;
  }

  void WindowCompositor::CancelTooltip()
  {
    _tooltip_window = NULL;
    m_TooltipArea = NULL;
    m_TooltipText = "";
  }

  bool WindowCompositor::ValidateMouseInsideTooltipArea(int x, int y)
  {
    NUX_RETURN_VALUE_IF_FALSE(m_TooltipArea, false);
    NUX_RETURN_VALUE_IF_FALSE(_tooltip_window.IsValid(), false);

    Geometry geo = m_TooltipArea->GetGeometry();
    geo.OffsetPosition(_tooltip_window->GetBaseX(), _tooltip_window->GetBaseY());

    return geo.IsPointInside(x, y);
  }

  bool WindowCompositor::IsTooltipActive()
  {
    NUX_RETURN_VALUE_IF_FALSE(m_TooltipArea, false);
    return true;
  }

  void WindowCompositor::OnKeyNavFocusDestroyed(Object* area)
  {
    if (key_focus_area_ == area)
    {
      key_focus_area_->ResetDownwardPathToKeyFocusArea();
      key_focus_area_->ResetUpwardPathToKeyFocusArea();

      key_focus_area_ = NULL;
    }
  }
  
  void WindowCompositor::SetKeyFocusArea(InputArea *area)
  {
    InputArea* keyboard_grab_area = GetKeyboardGrabArea();

    if (keyboard_grab_area  && area && (area != keyboard_grab_area) && (!area->IsChildOf(keyboard_grab_area)))
    {
      // There is a keyboard grab pending. Only an area that is a child of the area that has
      // the keyboard grab can be set to receive keyboard events.
      nuxDebugMsg("[WindowCompositor::SetKeyFocusArea] There is a keyboard grab pending. Cannot change the keyboard event receiver.");
      return;
    }

    if (key_focus_area_ == area)
    {
      return;
    }

    if (area && (area->AcceptKeyNavFocus() == false))
      return;

    if (key_focus_area_)
    {
      key_focus_area_->EmitEndKeyboardFocus();
      key_focus_area_->ResetUpwardPathToKeyFocusArea();

      if (key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
      {
        static_cast<InputArea*>(key_focus_area_)->OnKeyNavFocusChange.emit(key_focus_area_);
      }

      if (key_focus_area_->Type().IsDerivedFromType(View::StaticObjectType))
      {
        static_cast<View*>(key_focus_area_)->QueueDraw();
      }
    }

    if (area)
    {
      key_focus_area_ = area;

      key_focus_area_->SetPathToKeyFocusArea();
      key_focus_area_->EmitStartKeyboardFocus();

      if (key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
      {
        static_cast<InputArea*>(key_focus_area_)->OnKeyNavFocusChange.emit(key_focus_area_);
      }

      if (key_focus_area_->Type().IsDerivedFromType(View::StaticObjectType))
      {
        static_cast<View*>(key_focus_area_)->QueueDraw();
      }

      key_focus_area_->ChildFocusChanged.emit(key_focus_area_);

    }
    else
    {
      key_focus_area_ = NULL;
    }

    // Even if the the area parameter cannot receive keyboard events, it will get the
    // keyboard navigatiuon focus.

    if (key_focus_area_)
    {
      key_focus_area_->begin_key_focus.emit();
    }


    key_focus_area_connection_.disconnect();
    if (area)
    {
      key_focus_area_connection_ = area->object_destroyed.connect(sigc::mem_fun(this, &WindowCompositor::OnKeyNavFocusDestroyed));
    }
  }

  InputArea* WindowCompositor::GetKeyFocusArea()
  {
    return key_focus_area_;
  }

  void WindowCompositor::SetBackgroundPaintLayer(AbstractPaintLayer *bkg)
  {
    NUX_SAFE_DELETE(m_Background);
    m_Background = bkg->Clone();
  }

  void WindowCompositor::FloatingAreaConfigureNotify(int Width, int Height)
  {
    WindowList::iterator it;

    for (it = _view_window_list.begin(); it != _view_window_list.end(); it++)
    {
      if (!(*it).IsValid())
        continue;
      if ((*it)->IsVisible())
      {
        (*it)->NotifyConfigurationChange(Width, Height);
      }
    }
  }

  void WindowCompositor::FormatRenderTargets(int width, int height)
  {
    int buffer_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    int buffer_height = window_thread_->GetGraphicsEngine().GetWindowHeight();

    nuxAssert(buffer_width >= 1);
    nuxAssert(buffer_height >= 1);

    m_MainColorRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
    m_MainDepthRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);

    // Clear the buffer the first time...
    m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
    m_FrameBufferObject->SetRenderTarget(0, m_MainColorRT->GetSurfaceLevel(0));
    m_FrameBufferObject->SetDepthSurface(m_MainDepthRT->GetSurfaceLevel(0));
    m_FrameBufferObject->Activate();

    CHECKGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    CHECKGL(glClearDepth(1.0f));
    CHECKGL(glClearStencil(0));
    CHECKGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    m_FrameBufferObject->Deactivate();
    CHECKGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    CHECKGL(glClearDepth(1.0f));
    CHECKGL(glClearStencil(0));
    CHECKGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

//     m_BlurTexture   = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(Max(buffer_width, 1), Max(buffer_height, 1), 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
//     m_FullSceneMip0 = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(Max(buffer_width / 2, 1), Max(buffer_height / 2, 1), 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
//     m_FullSceneMip1 = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(Max(buffer_width / 4, 1), Max(buffer_height / 4, 1), 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
//     m_FullSceneMip2 = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(Max(buffer_width / 8, 1), Max(buffer_height / 8, 1), 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
  }

  void WindowCompositor::RestoreRenderingSurface()
  {
    BaseWindow *top_view = GetProcessingTopView();

    if (top_view && inside_rendering_cycle_)
    {
      nuxAssert(top_view->Type().IsDerivedFromType(BaseWindow::StaticObjectType));

      RenderTargetTextures rt = GetWindowBuffer(top_view);

      int buffer_width = top_view->GetBaseWidth();
      int buffer_height = top_view->GetBaseHeight();

      nuxAssert(buffer_width >= 1);
      nuxAssert(buffer_height >= 1);

      if ((rt.color_rt->GetWidth() != buffer_width) || (rt.color_rt->GetHeight() != buffer_height))
      {
        rt.color_rt = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
        rt.depth_rt = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
      }

      m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
      m_FrameBufferObject->SetRenderTarget(0, rt.color_rt->GetSurfaceLevel(0));
      m_FrameBufferObject->SetDepthSurface(rt.depth_rt->GetSurfaceLevel(0));
      m_FrameBufferObject->Activate();

      window_thread_->GetGraphicsEngine().SetViewport(0, 0, buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().ApplyClippingRectangle();
      //window_thread_->GetGraphicsEngine().ApplyModelViewMatrix(); ???
    }
    else
    {
      int buffer_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
      int buffer_height = window_thread_->GetGraphicsEngine().GetWindowHeight();

      nuxAssert(buffer_width >= 1);
      nuxAssert(buffer_height >= 1);
      // Restore Main Frame Buffer
      m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
      m_FrameBufferObject->SetRenderTarget(0, m_MainColorRT->GetSurfaceLevel(0));
      m_FrameBufferObject->SetDepthSurface(m_MainDepthRT->GetSurfaceLevel(0));
      m_FrameBufferObject->Activate();

      window_thread_->GetGraphicsEngine().SetViewport(0, 0, buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().ApplyClippingRectangle();
      //window_thread_->GetGraphicsEngine().ApplyModelViewMatrix(); ???
    }
  }

  void* WindowCompositor::GetBackupTextureData(BaseWindow *base_window, int &width, int &height, int &format)
  {
    width = height = format = 0;

    NUX_RETURN_VALUE_IF_NULL(base_window, 0);

    std::map<BaseWindow*, struct RenderTargetTextures>::iterator it;

    it = _window_to_texture_map.find(base_window);

    if (it == _window_to_texture_map.end())
    {
      return 0;
    }

    if ((*it).second.color_rt.IsNull())
    {
      return 0;
    }

    return (*it).second.color_rt->GetSurfaceData(0, width, height, format);
  }


  void WindowCompositor::ResetDnDArea()
  {
    SetDnDArea(NULL);
  }

  void WindowCompositor::SetDnDArea(InputArea* area)
  {
#if defined(NUX_OS_LINUX)
    if (_dnd_area == area)
      return;

    if (_dnd_area)
    {
      _dnd_area->HandleDndLeave();
      _dnd_area->UnReference();
    }
    _dnd_area = area;
    
    if (_dnd_area)
    {
      _dnd_area->Reference();
      _dnd_area->HandleDndEnter();
    }
#endif
  }

  InputArea* WindowCompositor::GetDnDArea()
  {
    return _dnd_area;
  }

  bool WindowCompositor::GrabPointerAdd(InputArea* area)
  {
    NUX_RETURN_VALUE_IF_NULL(area, false);
    bool result = true;

    if (GetPointerGrabArea() == area)
    {
      nuxDebugMsg("[WindowCompositor::GrabPointerAdd] The area already has the grab");
      return result;
    }
    
    if (window_thread_->GetGraphicsDisplay().PointerGrabData() != this)
      result = window_thread_->GetGraphicsDisplay().GrabPointer(NULL, this, true);

    if (result)
      pointer_grab_stack_.push_front(area);

    // reset the mouse pointers areas.
    ResetMousePointerAreas();

    return result;
  }

  bool WindowCompositor::GrabPointerRemove(InputArea* area)
  {
    NUX_RETURN_VALUE_IF_NULL(area, false);

    std::list<InputArea*>::iterator it;

    // find the first instance of the area pointer in the stack
    it = find(pointer_grab_stack_.begin(), pointer_grab_stack_.end(), area);

    if (it == pointer_grab_stack_.end())
      return false;

    pointer_grab_stack_.erase(it);
    
    if (pointer_grab_stack_.empty())
      window_thread_->GetGraphicsDisplay().UngrabPointer(this);
    
    // reset the mouse pointers areas.
    ResetMousePointerAreas();

    return true;
  }

  bool WindowCompositor::IsInPointerGrabStack(InputArea* area)
  {
    NUX_RETURN_VALUE_IF_NULL(area, false);

    std::list<InputArea*>::iterator it;
    it = find(pointer_grab_stack_.begin(), pointer_grab_stack_.end(), area);

    if (it == pointer_grab_stack_.end())
      return false;

    return true;
  }

  InputArea* WindowCompositor::GetPointerGrabArea()
  {
    if (pointer_grab_stack_.empty())
      return NULL;

    return (*pointer_grab_stack_.begin());
  }

  bool WindowCompositor::GrabKeyboardAdd(InputArea* area)
  {
    NUX_RETURN_VALUE_IF_NULL(area, false);
    bool result = true;

    if (GetKeyboardGrabArea() == area)
    {
      nuxDebugMsg("[WindowCompositor::GrabKeyboardAdd] The area already has the grab");
      return result;
    }

    if (window_thread_->GetGraphicsDisplay().KeyboardGrabData() != this)
    {
      result = window_thread_->GetGraphicsDisplay().GrabKeyboard(NULL, this, true);
    }
    
    if (result)
    {
      InputArea* current_keyboard_grab = GetKeyboardGrabArea();
      if (current_keyboard_grab)
        current_keyboard_grab->end_keyboard_grab.emit(current_keyboard_grab);

      keyboard_grab_stack_.push_front(area);
      
      // If there is any area with the key focus, cancel it.
      if (key_focus_area_)
      {
        key_focus_area_->EmitEndKeyboardFocus();
        key_focus_area_->ResetUpwardPathToKeyFocusArea();

        if (key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
        {
          static_cast<InputArea*>(key_focus_area_)->OnKeyNavFocusChange.emit(key_focus_area_);
        }

        if (key_focus_area_->Type().IsDerivedFromType(View::StaticObjectType))
        {
          static_cast<View*>(key_focus_area_)->QueueDraw();
        }
        key_focus_area_ = NULL;
      }
      // Must be called only after the area has been added to the front of keyboard_grab_stack_.
      SetKeyFocusArea(area);

      area->start_keyboard_grab.emit(area);
    }
    
    return result;
  }

  bool WindowCompositor::GrabKeyboardRemove(InputArea* area)
  {
    NUX_RETURN_VALUE_IF_NULL(area, false);

    std::list<InputArea*>::iterator it;

    // find the first instance of the area keyboard in the stack
    it = find(keyboard_grab_stack_.begin(), keyboard_grab_stack_.end(), area);

    if (it == keyboard_grab_stack_.end())
      return false;

    InputArea* current_keyboard_grab = (*it);
    bool has_grab = false;
    
    if (it == keyboard_grab_stack_.begin())
    {
      // At the top of the keyboard_grab_stack_. Means it has the keyboard grab.
      has_grab = true;
    }

    // First remove the area from the grab list
    keyboard_grab_stack_.erase(it);

    // Then emit end_keyboard_grab if the area had the keyboard grab
    if (has_grab && current_keyboard_grab)
    {
      current_keyboard_grab->end_keyboard_grab.emit(current_keyboard_grab);
    }

    if (keyboard_grab_stack_.empty())
    {
      window_thread_->GetGraphicsDisplay().UngrabKeyboard(this);
    }

    // Must be called only after the area has been added to the front of keyboard_grab_stack_.
    if (keyboard_grab_stack_.empty())
    {
      SetKeyFocusArea(NULL);
    }
    else
    {
      // If there is any area with the key focus, cancel it.
      if (key_focus_area_)
      {
        key_focus_area_->EmitEndKeyboardFocus();
        key_focus_area_->ResetUpwardPathToKeyFocusArea();

        if (key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
        {
          static_cast<InputArea*>(key_focus_area_)->OnKeyNavFocusChange.emit(key_focus_area_);
        }

        if (key_focus_area_->Type().IsDerivedFromType(View::StaticObjectType))
        {
          static_cast<View*>(key_focus_area_)->QueueDraw();
        }
        key_focus_area_ = NULL;
      }
            
      it = keyboard_grab_stack_.begin();
      SetKeyFocusArea(*it);

      InputArea* new_keyboard_grab = (*it);
      new_keyboard_grab->start_keyboard_grab.emit(new_keyboard_grab);
    }
    
    return true;
  }

  bool WindowCompositor::IsInKeyboardGrabStack(InputArea* area)
  {
    NUX_RETURN_VALUE_IF_NULL(area, false);

    std::list<InputArea*>::iterator it;
    it = find(keyboard_grab_stack_.begin(), keyboard_grab_stack_.end(), area);

    if (it == keyboard_grab_stack_.end())
      return false;

    return true;
  }

  InputArea* WindowCompositor::GetKeyboardGrabArea()
  {
    if (keyboard_grab_stack_.size() == 0)
      return NULL;

    return (*keyboard_grab_stack_.begin());
  }

  void WindowCompositor::SetReferenceFramebuffer(unsigned int fbo_object, Geometry fbo_geometry)
  {
    reference_fbo_ = fbo_object;
    reference_fbo_geometry_ = fbo_geometry;
  }

  bool WindowCompositor::RestoreReferenceFramebuffer()
  {
    // It is assumed that the reference fbo contains valid textures.
    // Nux does the following:
    //    - Bind the reference fbo (reference_fbo_)
    //    - Call glDrawBuffer with GL_COLOR_ATTACHMENT0
    //    - Set the opengl viewport size (reference_fbo_geometry_)

    bool ok = false;

    CHECKGL(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, reference_fbo_));
#ifndef NUX_OPENGLES_20
    CHECKGL(glDrawBuffer(GL_COLOR_ATTACHMENT0));
#endif
    CHECKGL(glViewport(reference_fbo_geometry_.x,
      reference_fbo_geometry_.y,
      reference_fbo_geometry_.width,
      reference_fbo_geometry_.height));

    // Nux does some sanity checks to make sure that the FBO is in good condition.
    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    CHECKGL_MSG(glCheckFramebufferStatusEXT);

    switch(status)
    {
      case GL_FRAMEBUFFER_COMPLETE_EXT: // Everything's OK
        ok = true;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT");
        ok = false;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
        ok = false;
        break;
        // See issue(87) of http://www.opengl.org/registry/specs/EXT/framebuffer_object.txt
        //  case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        //      nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT");
        //      ok = false;
        //      break;
      case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
        ok = false;
        break;
#ifndef NUX_OPENGLES_20
      case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
        ok = false;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT");
        ok = false;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT");
        ok = false;
        break;
#endif
      //  case GL_FRAMEBUFFER_STATUS_ERROR_EXT:
      //      nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_STATUS_ERROR_EXT");
      //      ok = false;
      //      break;
      case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        nuxError("[GLFramebufferObject::IsValid] GL_FRAMEBUFFER_UNSUPPORTED_EXT");
        ok = false;
        break;
      default:
        nuxError("[GLFramebufferObject::IsValid] Unknown ERROR");
        ok = false;
    }    

    return ok;
  }
}

