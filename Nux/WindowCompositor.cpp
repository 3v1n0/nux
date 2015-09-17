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
#include "InputAreaProximity.h"
#if !defined(NUX_MINIMAL)
#include "MenuPage.h"
#endif
#include "PaintLayer.h"
#include "Painter.h"
#include "Layout.h"

#include "NuxGraphics/FontTexture.h"
namespace nux
{
DECLARE_LOGGER(logger, "nux.window");

  WindowCompositor::WindowCompositor(WindowThread* window_thread)
  : draw_reference_fbo_(0)
  , read_reference_fbo_(0)
  , window_thread_(window_thread)
  , currently_rendering_windows_(nullptr)
  , current_global_clip_rect_(nullptr)
  {
    m_OverlayWindow             = NULL;
    _tooltip_window             = NULL;
    m_TooltipArea               = NULL;
    m_Background                = NULL;
    _tooltip_window             = NULL;
    OverlayDrawingCommand       = NULL;
    m_CurrentWindow             = NULL;
    m_MenuWindow                = NULL;
    _always_on_front_window     = NULL;
    inside_event_cycle_         = false;
    inside_rendering_cycle_     = false;
    _dnd_area                   = NULL;
    _starting_menu_event_cycle  = false;
    _menu_is_active             = false;
    on_menu_closure_continue_with_event_ = false;
    _mouse_position_on_owner = Point(0, 0);

    platform_support_for_depth_texture_ = GetGraphicsDisplay()->GetGpuDevice()->GetGpuInfo().Support_Depth_Buffer();

    m_FrameBufferObject = GetGraphicsDisplay()->GetGpuDevice()->CreateFrameBufferObject();
    // Do not leave the Fbo binded. Deactivate it.
    m_FrameBufferObject->Deactivate();

    // At this stage, the size of the window may not be known yet.
    // FormatRenderTargets will be called the first time runtime gets into WindowThread::ExecutionLoop
    m_MainColorRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(2, 2, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);

    if (platform_support_for_depth_texture_)
    {
      m_MainDepthRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(2, 2, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
    }

#if !defined(NUX_MINIMAL)
    _mouse_over_menu_page       = NULL;
    _mouse_owner_menu_page      = NULL;
    _menu_chain                 = NULL;
    _menu_chain                 = new std::list<MenuPage*>;
#endif

    m_MenuRemoved = false;
    m_Background = new ColorLayer(Color(0xFF4D4D4D));

#ifdef NUX_GESTURES_SUPPORT
    gesture_broker_.reset(new DefaultGestureBroker(this));
#endif
  }

  void WindowCompositor::BeforeDestructor()
  {
    key_focus_area_ = NULL;
  }

  WindowCompositor::~WindowCompositor()
  {
    _window_to_texture_map.clear();
    m_FrameBufferObject.Release();
    m_MainColorRT.Release();
    m_MainDepthRT.Release();
    _view_window_list.clear();
    _modal_view_window_list.clear();

#if !defined(NUX_MINIMAL)
    _menu_chain->clear();
    delete _menu_chain;
#endif
    delete m_Background;
  }

  WindowCompositor::RenderTargetTextures& WindowCompositor::GetWindowBuffer(BaseWindow* window)
  {
    static RenderTargetTextures invalid;
    std::map< BaseWindow*, RenderTargetTextures>::iterator it = _window_to_texture_map.find(window);

    if (it != _window_to_texture_map.end())
    {
      return it->second;
    }
    LOG_WARN(logger) << "No RenderTargetTextures for window.";
    return invalid;
  }

  void WindowCompositor::RegisterWindow(BaseWindow* window)
  {
    LOG_DEBUG_BLOCK(logger);
    if (!window)
      return;

    WindowList::iterator it = find(_view_window_list.begin(),
                                   _view_window_list.end(), window);
    if (it == _view_window_list.end())
    {
      _view_window_list.push_front(WeakBaseWindowPtr(window));

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
      _view_window_list.erase(it);

    _window_to_texture_map.erase(window.GetPointer());
  }

  //! Get Mouse position relative to the top left corner of the window.
  Point WindowCompositor::GetMousePosition()
  {
    return _mouse_position;
  }

  void WindowCompositor::ResetMousePointerAreas()
  {
    mouse_over_area_ = NULL;
    SetMouseOwnerArea(NULL);
#if !defined(NUX_MINIMAL)
    _mouse_over_menu_page   = NULL;
    _mouse_owner_menu_page  = NULL;
#endif
  }

  void WindowCompositor::FindAreaUnderMouse(const Point& mouse_position,
                                           NuxEventType event_type,
                                           ObjectWeakPtr<InputArea>& area_under_mouse_pointer)
  {
    WeakBaseWindowPtr window;
    GetAreaUnderMouse(mouse_position, event_type, area_under_mouse_pointer, window);
  }

  void WindowCompositor::GetAreaUnderMouse(const Point& mouse_position,
                                           NuxEventType event_type,
                                           ObjectWeakPtr<InputArea>& area_under_mouse_pointer,
                                           WeakBaseWindowPtr& window)
  {
    window = NULL;
    area_under_mouse_pointer = NULL;

    // Go through the list of BaseWindo and find the first area over which the
    // mouse pointer is.
    for (auto const& window_it : _view_window_list)
    {
      // Since the mouse is really an input-level thing, we want to know
      // if the underlying input window is enabled or if the window is
      // visible

      if (!window_it.IsValid())
        continue;

      if (window_it->InputWindowEnabled() || window_it->IsVisible())
      {
        Area* area = window_it->FindAreaUnderMouse(mouse_position, event_type);
        if (area)
        {
          area_under_mouse_pointer = static_cast<InputArea*>(area);
          window = window_it;
          return;
        }
      }
    }

    // If area_under_mouse_pointer is NULL, then the mouse pointer is not over
    // any of the BaseWindow. Try the main window layout.
    if (!area_under_mouse_pointer.IsValid())
    {
      Layout* main_window_layout = window_thread_->GetLayout();
      if (main_window_layout)
        area_under_mouse_pointer = static_cast<InputArea*>(main_window_layout->FindAreaUnderMouse(mouse_position, event_type));
    }
  }

  void WindowCompositor::SetMouseOwnerArea(InputArea* area)
  {
    if (mouse_owner_area_ == area)
      return;

    auto* old_owner = mouse_owner_area_.GetPointer();
    mouse_owner_area_ = area;
    dnd_safety_x_ = 0;
    dnd_safety_y_ = 0;

    if (old_owner)
      old_owner->EmitMouseCancelSignal();
  }

  ObjectWeakPtr<InputArea> const& WindowCompositor::GetMouseOwnerArea() const
  {
    return mouse_owner_area_;
  }

  void WindowCompositor::DndEventCycle(Event& event)
  {
    if (event.type == NUX_DND_MOVE)
    {
      ObjectWeakPtr<InputArea> hit_area;

      FindAreaUnderMouse(Point(event.x, event.y), event.type, hit_area);

      if (hit_area.IsValid())
      {
        SetDnDArea(hit_area.GetPointer());
        hit_area->HandleDndMove(event);
      }
      else
      {
        ResetDnDArea();
      }
    }
    else if (event.type == NUX_DND_ENTER_WINDOW)
    {
      // do nothing for now
    }
    else if (event.type == NUX_DND_LEAVE_WINDOW)
    {
      ResetDnDArea();
    }
    else if (event.type == NUX_DND_DROP)
    {
      if (_dnd_area && _dnd_area->GetAbsoluteGeometry().IsPointInside(event.x - event.x_root, event.y - event.y_root))
        _dnd_area->HandleDndDrop(event);
    }
  }

  void WindowCompositor::UpdateKeyNavFocusOnMouseDown()
  {
    /* In the case of a mouse down event, if there is currently a keyboard event
       receiver and it is different from the area returned by FindAreaUnderMouse,
       then stop that receiver from receiving anymore keyboard events and switch
       make mouse_over_area_ the new receiver(if it accept keyboard events). */
    if (mouse_over_area_.IsValid() && mouse_over_area_ != GetKeyFocusArea() &&
        mouse_over_area_->AcceptKeyNavFocusOnMouseDown())
    {
      InputArea* grab_area = GetKeyboardGrabArea();
      if (grab_area)
      {
        if (mouse_over_area_->IsChildOf(grab_area)
          /*&& mouse_over_area_->AcceptKeyboardEvent()*/)
        {
          SetKeyFocusArea(mouse_over_area_.GetPointer());
        }
        else
        {
          SetKeyFocusArea(grab_area);
        }
      }
      else
      {
        SetKeyFocusArea(mouse_over_area_.GetPointer());
      }
    }
  }

  void WindowCompositor::TrackMouseMovement(const Event &event,
                                            bool area_under_mouse_changed)
  {
    if (!mouse_owner_area_.IsValid())
    {
      if (mouse_over_area_.IsValid())
      {
        if (event.type == NUX_MOUSE_MOVE)
        {
          Geometry hit_view_geo = mouse_over_area_->GetAbsoluteGeometry();
          int hit_view_x = event.x - hit_view_geo.x;
          int hit_view_y = event.y - hit_view_geo.y;
          int dx = event.x - _mouse_position.x;
          int dy = event.y - _mouse_position.y;
          mouse_over_area_->EmitMouseMoveSignal(hit_view_x,
                                                hit_view_y,
                                                area_under_mouse_changed ? 0 : dx,
                                                area_under_mouse_changed ? 0 : dy,
                                                event.GetMouseState(),
                                                event.GetKeyState());
        }
      }
    }
    else
    {
      if (event.type == NUX_MOUSE_PRESSED || event.type == NUX_MOUSE_DOUBLECLICK)
      {
        // We just got a new mouse owner. Let's update the mouse position on him.
        Geometry const& mouse_owner_geo = mouse_owner_area_->GetAbsoluteGeometry();
        int mouse_owner_x = event.x - mouse_owner_geo.x;
        int mouse_owner_y = event.y - mouse_owner_geo.y;

        _mouse_position_on_owner = Point(mouse_owner_x, mouse_owner_y);
      }
      else if (event.type == NUX_MOUSE_MOVE)
      {
        Geometry const& mouse_owner_geo = mouse_owner_area_->GetAbsoluteGeometry();
        int mouse_owner_x = event.x - mouse_owner_geo.x;
        int mouse_owner_y = event.y - mouse_owner_geo.y;

        int dx = mouse_owner_x - _mouse_position_on_owner.x;
        int dy = mouse_owner_y - _mouse_position_on_owner.y;

        if (mouse_owner_area_->_dnd_enabled_as_source)
        {
          dnd_safety_x_ += dx;
          dnd_safety_y_ += dy;

          if (abs(dnd_safety_y_) > 30 || abs(dnd_safety_x_) > 30)
          {
#if defined(DRAG_AND_DROP_SUPPORTED)
            mouse_owner_area_->StartDragAsSource();
#endif
            ResetMousePointerAreas();
            return;
          }
        }
        else
        {
          mouse_owner_area_->EmitMouseDragSignal(mouse_owner_x, mouse_owner_y,
                                                 dx, dy,
                                                 event.GetMouseState(),
                                                 event.GetKeyState());
        }

        _mouse_position_on_owner = Point(mouse_owner_x, mouse_owner_y);
      }
      else if (event.type == NUX_MOUSE_RELEASED)
      {
        _mouse_position_on_owner = Point(0, 0);
      }
    }

    _mouse_position = Point(event.x, event.y);
  }

  bool WindowCompositor::UpdateWhatAreaIsUnderMouse(const Event& event)
  {
    ObjectWeakPtr<InputArea> new_area_under_mouse;
    ObjectWeakPtr<InputArea> old_mouse_over_area = mouse_over_area_;

    if (mouse_owner_area_.IsValid())
    {
      FindAreaUnderMouse(Point(event.x, event.y), event.type, new_area_under_mouse);
    }
    else
    {
      // Look for the area below the mouse pointer in the BaseWindow.
      Area* pointer_grab_area = GetPointerGrabArea();
      if (pointer_grab_area)
      {
        // If there is a pending mouse pointer grab, test that area only
        new_area_under_mouse = NUX_STATIC_CAST(InputArea*,
            pointer_grab_area->FindAreaUnderMouse(Point(event.x, event.y),
                                                  event.type));
      }
      else
      {
        FindAreaUnderMouse(Point(event.x, event.y), event.type, new_area_under_mouse);
      }
    }

    if (!mouse_owner_area_.IsValid())
    {
      if (event.type == NUX_WINDOW_MOUSELEAVE)
      {
        if (mouse_over_area_.IsValid())
        {
          // The area where the mouse was in the previous cycle and the area
          // returned by GetAreaUnderMouse are different.
          // The area from the previous cycle receive a "mouse leave signal".
          Geometry const& geo = mouse_over_area_->GetAbsoluteGeometry();
          int x = event.x - geo.x;
          int y = event.y - geo.y;

          mouse_over_area_->EmitMouseLeaveSignal(x, y, event.GetMouseState(),
                                                 event.GetKeyState());
          mouse_over_area_ = NULL;
        }
      }
      if (new_area_under_mouse.IsValid())
      {
        if (new_area_under_mouse != mouse_over_area_)
        {
          if (mouse_over_area_.IsValid())
          {
            // The area where the mouse was in the previous cycle and the area
            // returned by FindAreaUnderMouse are different.
            // The area from the previous cycle receive a "mouse leave signal".
            Geometry const& geo = mouse_over_area_->GetAbsoluteGeometry();
            int x = event.x - geo.x;
            int y = event.y - geo.y;

            mouse_over_area_->EmitMouseLeaveSignal(x, y, event.GetMouseState(),
                                                   event.GetKeyState());
          }
          // The area we found under the mouse pointer receives a "mouse enter signal".
          mouse_over_area_ = new_area_under_mouse;

          if (mouse_over_area_.IsValid() && mouse_over_area_ != GetKeyFocusArea() &&
              mouse_over_area_->AcceptKeyNavFocusOnMouseEnter())
          {
            SetKeyFocusArea(mouse_over_area_.GetPointer());
          }

          Geometry hit_view_geo = new_area_under_mouse->GetAbsoluteGeometry();
          int hit_view_x = event.x - hit_view_geo.x;
          int hit_view_y = event.y - hit_view_geo.y;

          mouse_over_area_->EmitMouseEnterSignal(hit_view_x, hit_view_y,
                                                 event.GetMouseState(),
                                                 event.GetKeyState());
        }
      }
      else
      {
        if (mouse_over_area_.IsValid())
        {
          // Mouse wheel events are stationary. The mouse can remain inside an
          // area while the mouse wheel is spinning.
          // This shouldn't qualify as a mouse leave event.
          if (event.type != NUX_MOUSE_WHEEL)
          {
            Geometry const& geo = mouse_over_area_->GetAbsoluteGeometry();
            int x = event.x - geo.x;
            int y = event.y - geo.y;

            mouse_over_area_->EmitMouseLeaveSignal(x, y, event.GetMouseState(),
                                                   event.GetKeyState());
          }
        }

        mouse_over_area_ = NULL;
      }
    }
    else
    {
      // Context: The left mouse button down over an area. All events goes to
      // that area.

      Geometry const& mouse_owner_geo = mouse_owner_area_->GetAbsoluteGeometry();
      int mouse_owner_x = event.x - mouse_owner_geo.x;
      int mouse_owner_y = event.y - mouse_owner_geo.y;

      if (event.type == NUX_MOUSE_MOVE)
      {
        if (mouse_over_area_ == mouse_owner_area_ && new_area_under_mouse != mouse_owner_area_)
        {
          mouse_owner_area_->EmitMouseLeaveSignal(mouse_owner_x, mouse_owner_y,
                                                 event.GetMouseState(),
                                                 event.GetKeyState());
          mouse_over_area_ = new_area_under_mouse;
        }
        else if (mouse_over_area_ != mouse_owner_area_ && new_area_under_mouse == mouse_owner_area_)
        {
          mouse_owner_area_->EmitMouseEnterSignal(mouse_owner_x, mouse_owner_y,
                                                  event.GetMouseState(),
                                                  event.GetKeyState());
          mouse_over_area_ = new_area_under_mouse;
        }
      }
      else if (event.type == NUX_MOUSE_RELEASED)
      {
        if (new_area_under_mouse != mouse_over_area_)
        {
          if (mouse_over_area_ == mouse_owner_area_)
          {
            mouse_owner_area_->EmitMouseLeaveSignal(mouse_owner_x, mouse_owner_y,
                                                    event.GetMouseState(),
                                                    event.GetKeyState());
          }

          mouse_over_area_ = new_area_under_mouse;

          if (mouse_over_area_.IsValid())
          {
            auto const& over_geo = mouse_over_area_->GetAbsoluteGeometry();
            mouse_over_area_->EmitMouseEnterSignal(event.x - over_geo.x,
                                                   event.y - over_geo.y,
                                                   event.GetMouseState(),
                                                   event.GetKeyState());
          }
        }
      }
    }

    return mouse_over_area_ != old_mouse_over_area;
  }

  void WindowCompositor::ProcessMouseWheelEvent(Event& event)
  {
    if (mouse_over_area_.IsValid())
    {
      Geometry hit_view_geo = mouse_over_area_->GetAbsoluteGeometry();
      int hit_view_x = event.x - hit_view_geo.x;
      int hit_view_y = event.y - hit_view_geo.y;

      mouse_over_area_->EmitMouseWheelSignal(hit_view_x,
                                             hit_view_y,
                                             event.wheel_delta,
                                             event.GetMouseState(),
                                             event.GetKeyState());
    }
  }

  void WindowCompositor::UpdateMouseOwner(const Event& event,
                                          bool area_under_mouse_changed)
  {
    if (!mouse_owner_area_.IsValid())
    {
      if (mouse_over_area_.IsValid())
      {
        Geometry hit_view_geo = mouse_over_area_->GetAbsoluteGeometry();
        int hit_view_x = event.x - hit_view_geo.x;
        int hit_view_y = event.y - hit_view_geo.y;

        if (event.type == NUX_MOUSE_PRESSED
            || event.type == NUX_MOUSE_DOUBLECLICK)
        {
          SetMouseOwnerArea(mouse_over_area_.GetPointer());

          UpdateKeyNavFocusOnMouseDown();

          if (event.type == NUX_MOUSE_DOUBLECLICK
                   && mouse_over_area_->DoubleClickEnabled()
                   && !area_under_mouse_changed)
          {
            mouse_over_area_->EmitMouseDoubleClickSignal(hit_view_x, hit_view_y,
                                                         event.GetMouseState(),
                                                         event.GetKeyState());
          }
          else
          {
            mouse_over_area_->EmitMouseDownSignal(hit_view_x, hit_view_y,
                                                  event.GetMouseState(),
                                                  event.GetKeyState());
          }
        }
        else if (event.type == NUX_MOUSE_RELEASED)
        {
          // We only get a NUX_MOUSE_RELEASED event when the mouse was pressed
          // over another area and released here. There are a few situations that
          // can cause mouse_owner_area_ to be NULL on a NUX_MOUSE_RELEASED event:
          //  - The mouse down event happens on a area. That area is set into
          //     mouse_owner_area_
          //    Then the area is destroyed, before the mouse is released
          //  - The mouse down event happens. Then a call to AddGrabPointer triggers
          //    a call to ResetMousePointerAreas. mouse_owner_area_ is then set to
          //    NULL.

          mouse_over_area_->EmitMouseUpSignal(hit_view_x, hit_view_y,
                                              event.GetMouseState(),
                                              event.GetKeyState());
        }
      }
      else
      {
        Area* pointer_grab_area = GetPointerGrabArea();
        if (event.type == NUX_MOUSE_PRESSED && pointer_grab_area)
        {
          Geometry geo = pointer_grab_area->GetAbsoluteGeometry();
          int x = event.x - geo.x;
          int y = event.y - geo.y;

          NUX_STATIC_CAST(InputArea*, pointer_grab_area)->
            EmitMouseDownOutsideArea(x, y, event.GetMouseState(), event.GetKeyState());
        }
      }
    }
    else
    {
      if (event.type == NUX_MOUSE_RELEASED)
      {
        Geometry const& mouse_owner_geo = mouse_owner_area_->GetAbsoluteGeometry();
        int mouse_owner_x = event.x - mouse_owner_geo.x;
        int mouse_owner_y = event.y - mouse_owner_geo.y;
        auto* old_mouse_owner_area = mouse_owner_area_.GetPointer();

        SetMouseOwnerArea(NULL);

        old_mouse_owner_area->EmitMouseUpSignal(mouse_owner_x, mouse_owner_y,
                                                event.GetMouseState(),
                                                event.GetKeyState());

        if (mouse_over_area_ == old_mouse_owner_area &&
            (!mouse_over_area_->DoubleClickEnabled() || (event.mouse_state & NUX_STATE_FIRST_EVENT) != 0))
        {
          old_mouse_owner_area->EmitMouseClickSignal(mouse_owner_x, mouse_owner_y,
                                                     event.GetMouseState(),
                                                     event.GetKeyState());
        }
      }
    }
  }

  void WindowCompositor::FindAncestorInterestedInChildMouseEvents(Area *area)
  {
    if (!area)
      return;

    Area *parent = area->GetParentObject();
    if (!parent)
      return;

    if (parent->IsInputArea())
    {
      InputArea *parent_input_area = static_cast<InputArea*>(parent);
      if (parent_input_area->IsTrackingChildMouseEvents())
        interested_mouse_owner_ancestor_ = parent_input_area;
    }

    if (!interested_mouse_owner_ancestor_.IsValid())
    {
      // Keep searching...
      FindAncestorInterestedInChildMouseEvents(parent);
    }
  }

  void WindowCompositor::UpdateEventTrackingByMouseOwnerAncestor(const Event& event)
  {
    if (event.type == NUX_MOUSE_PRESSED || event.type == NUX_MOUSE_DOUBLECLICK)
      FindAncestorInterestedInChildMouseEvents(mouse_owner_area_.GetPointer());

    if (!interested_mouse_owner_ancestor_.IsValid())
      return;

    bool wants_ownership =
      interested_mouse_owner_ancestor_->ChildMouseEvent(event);

    if (wants_ownership)
    {
      mouse_owner_area_->EmitMouseCancelSignal();

      SetMouseOwnerArea(interested_mouse_owner_ancestor_.GetPointer());
      _mouse_position_on_owner = Point(event.x - mouse_owner_area_->GetAbsoluteX(),
                                       event.y - mouse_owner_area_->GetAbsoluteY());

      interested_mouse_owner_ancestor_ = NULL;

    }

    if (event.type == NUX_MOUSE_RELEASED)
      interested_mouse_owner_ancestor_ = NULL;
  }

  void WindowCompositor::MouseEventCycle(Event& event)
  {
    // Checks the area_proximities_ list for any mouse near/beyond signals
    if (event.type == NUX_MOUSE_MOVE || event.type == NUX_WINDOW_MOUSELEAVE)
    {
      CheckMouseNearArea(event);
    }

    // Updates mouse_over_area_ and emits mouse_enter and mouse_leave signals
    // accordingly.
    bool area_under_mouse_changed = UpdateWhatAreaIsUnderMouse(event);

    // Updates mouse_owner_area_ and emits mouse_down, mouse_up,
    // mouse_click and mouse_double_click accordingly.
    UpdateMouseOwner(event, area_under_mouse_changed);

    // Keeps track of mouse movement and emits mouse_move and mouse_drag
    // accordingly.
    TrackMouseMovement(event, area_under_mouse_changed);

    if (event.type == NUX_MOUSE_WHEEL)
        ProcessMouseWheelEvent(event);

    // Feed the appropriate InputArea::ChildMouseEvent() and switch mouse
    // ownership (including the emission of mouse_cancel) if asked to.
    UpdateEventTrackingByMouseOwnerAncestor(event);
  }

#if !defined(NUX_MINIMAL)
  void WindowCompositor::MenuEventCycle(Event& event)
  {
    // _mouse_owner_menu_page: the menu page that has the mouse down
    // _mouse_over_menu_page: the menu page that is directly below the mouse pointer

    _mouse_position = Point(event.x, event.y);

    if (_mouse_owner_menu_page == NULL)
    {
      if ((event.type == NUX_MOUSE_PRESSED) ||
        (event.type == NUX_MOUSE_RELEASED) ||
        (event.type == NUX_MOUSE_MOVE) ||
        (event.type == NUX_MOUSE_DOUBLECLICK) ||
        (event.type == NUX_MOUSE_WHEEL))
      {
        // Find the MenuPage under the mouse
        MenuPage* hit_menu_page = NULL;
        std::list<MenuPage*>::iterator menu_it;
        for (menu_it = _menu_chain->begin(); menu_it != _menu_chain->end(); ++menu_it)
        {
          // The leaf of the menu chain is in the front of the list.
          hit_menu_page = NUX_STATIC_CAST(MenuPage*, (*menu_it)->FindAreaUnderMouse(Point(event.x, event.y), event.type));
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
          hit_menu_page_x = event.x - hit_menu_page_geo.x;
          hit_menu_page_y = event.y - hit_menu_page_geo.y;
        }

        if (hit_menu_page && (event.type == NUX_MOUSE_RELEASED))
        {
          hit_menu_page->EmitMouseUpSignal(hit_menu_page_x, hit_menu_page_y, event.GetMouseState(), event.GetKeyState());

          (*_menu_chain->begin())->sigClosingMenu(*_menu_chain->begin());
          (*_menu_chain->begin())->StopMenu();
        }
        else if (hit_menu_page && (event.type == NUX_MOUSE_MOVE))
        {
          if (hit_menu_page != _mouse_over_menu_page)
          {
            if (_mouse_over_menu_page != 0)
            {
              Geometry geo = _mouse_over_menu_page->GetAbsoluteGeometry();
              int x = event.x - geo.x;
              int y = event.y - geo.y;

              _mouse_over_menu_page->EmitMouseLeaveSignal(x, y, event.GetMouseState(), event.GetKeyState());
            }

            _mouse_over_menu_page = hit_menu_page;
            _mouse_over_menu_page->EmitMouseEnterSignal(hit_menu_page_x, hit_menu_page_y, event.GetMouseState(), event.GetKeyState());
          }

          _mouse_over_menu_page->EmitMouseMoveSignal(hit_menu_page_x, hit_menu_page_y, event.dx, event.dy, event.GetMouseState(), event.GetKeyState());
        }
        else if (hit_menu_page && ((event.type == NUX_MOUSE_PRESSED) || (event.type == NUX_MOUSE_DOUBLECLICK)))
        {
          if (!hit_menu_page->DoubleClickEnabled())
          {

          }

          if (_mouse_over_menu_page && (hit_menu_page != _mouse_over_menu_page))
          {
            Geometry geo = _mouse_over_menu_page->GetAbsoluteGeometry();
            int x = event.x - geo.x;
            int y = event.y - geo.y;

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
        else if (hit_menu_page && (event.type == NUX_MOUSE_WHEEL))
        {
          hit_menu_page->EmitMouseWheelSignal(hit_menu_page_x, hit_menu_page_y, event.wheel_delta, event.GetMouseState(), event.GetKeyState());
        }
        else if (hit_menu_page == NULL)
        {
          if (_mouse_over_menu_page)
          {
            Geometry geo = _mouse_over_menu_page->GetAbsoluteGeometry();
            int x = event.x - geo.x;
            int y = event.y - geo.y;

            _mouse_over_menu_page->EmitMouseLeaveSignal(x, y, event.GetMouseState(), event.GetKeyState());
          }

          if (event.type == NUX_MOUSE_PRESSED || event.type == NUX_MOUSE_DOUBLECLICK)
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
      for (menu_it = _menu_chain->begin(); menu_it != _menu_chain->end(); ++menu_it)
      {
        // The leaf of the menu chain is in the front of the list.
        hit_menu_page = NUX_STATIC_CAST(MenuPage*, (*menu_it)->FindAreaUnderMouse(Point(event.x, event.y), event.type));
        if (hit_menu_page)
        {
          break;
        }
      }

      Geometry mouse_owner_geo = _mouse_owner_menu_page->GetAbsoluteGeometry();
      int mouse_owner_x = event.x - mouse_owner_geo.x;
      int mouse_owner_y = event.y - mouse_owner_geo.y;

      // the mouse is down over a view
      if (event.type == NUX_MOUSE_MOVE)
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
      else if (event.type == NUX_MOUSE_RELEASED)
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
#endif

  void WindowCompositor::FindKeyFocusArea(NuxEventType event_type,
    unsigned int key_symbol,
    unsigned int special_keys_state,
    ObjectWeakPtr<InputArea>& key_focus_area,
    WeakBaseWindowPtr& window)
  {
    key_focus_area = NULL;
    window = NULL;

    // Go through the list of BaseWindos and find the first area over which the mouse pointer is.
    for (auto const& window_it : _view_window_list)
    {
      if (window_it.IsValid() && window_it->IsVisible())
      {
        key_focus_area = NUX_STATIC_CAST(InputArea*, window_it->FindKeyFocusArea(event_type, key_symbol, special_keys_state));
        if (key_focus_area.IsValid())
        {
          // We have found an area. We are going to exit the while loop.
          window = window_it;
          break;
        }
      }
    }

    // If key_focus_area is NULL, then try the main window layout.
    if (!key_focus_area.IsValid())
    {
      Layout* main_window_layout = window_thread_->GetLayout();
      if (main_window_layout)
      {
        key_focus_area = NUX_STATIC_CAST(InputArea*, main_window_layout->FindKeyFocusArea(event_type, key_symbol, special_keys_state));
      }
    }
  }

  void WindowCompositor::FindKeyFocusAreaFrom(NuxEventType event_type,
    unsigned int key_symbol,
    unsigned int special_keys_state,
    InputArea* root_search_area,
    ObjectWeakPtr<InputArea>& key_focus_area,
    WeakBaseWindowPtr& window)
  {
    key_focus_area = NULL;
    window = NULL;

    if (root_search_area == NULL)
    {
      return;
    }

    key_focus_area = NUX_STATIC_CAST(InputArea*, root_search_area->FindKeyFocusArea(event_type, key_symbol, special_keys_state));
    if (key_focus_area.IsValid())
    {
      window = NUX_STATIC_CAST(BaseWindow*, root_search_area->GetTopLevelViewWindow());
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

  void WindowCompositor::KeyboardEventCycle(Event& event)
  {
    InputArea* keyboard_event_grab_view = GetKeyboardGrabArea();

    ObjectWeakPtr<InputArea> focus_area;   // The view under the mouse
    WeakBaseWindowPtr base_window; // The BaseWindow below the mouse pointer.

    if (keyboard_event_grab_view)
    {
      // There is a keyboard grab.
      // Find the key focus area, under the keyboard grab area. That is to say, the key focus area is in the widget tree
      // whose root is the keyboard grab area. This phase is known as the capture phase.
      FindKeyFocusAreaFrom(event.type, event.GetKeySym(), event.GetKeyState(),
                           keyboard_event_grab_view, focus_area, base_window);
    }
    else
    {
      FindKeyFocusArea(event.type, event.GetKeySym(), event.GetKeyState(),
                       focus_area, base_window);
    }

    KeyNavDirection direction = KEY_NAV_NONE;

    if (event.type == EVENT_KEY_DOWN)
    {
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
    }

    if (focus_area.IsValid())
    {
      SetKeyFocusArea(focus_area.GetPointer(), direction);
    }
    else
    {
      SetKeyFocusArea(NULL, KEY_NAV_NONE);
    }

    if (key_focus_area_.IsValid())
    {
      if (key_focus_area_->InspectKeyEvent(event.type, event.GetKeySym(), event.GetText()))
      {
        SendKeyEvent(key_focus_area_.GetPointer(),
                    event.type,
                    event.GetKeySym(),
#if defined(NUX_OS_WINDOWS)
                    event.win32_keycode,
#elif defined(USE_X11)
                    event.x11_keycode,
#else
                    0,
#endif
                    event.GetKeyState(),
                    event.GetText(),
                    event.GetKeyRepeatCount());
      }
      else if (direction == KEY_NAV_NONE)
      {
        Area* parent = key_focus_area_->GetParentObject();

        while (parent && !parent->InspectKeyEvent(event.type, event.GetKeySym(), event.GetText()))
        {
          parent = parent->GetParentObject();
        }

        if (parent)
        {
          SendKeyEvent(static_cast<InputArea*>(parent),
            event.type,
            event.GetKeySym(),
#if defined(NUX_OS_WINDOWS)
            event.win32_keycode,
#elif defined(USE_X11)
            event.x11_keycode,
#else
            0,
#endif
            event.GetKeyState(),
            event.GetText(),
            event.GetKeyRepeatCount());
        }
      }
      else if (event.type == NUX_KEYDOWN)
      {
        if (direction == KEY_NAV_ENTER)
        {
          if (key_focus_area_.IsValid() && key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
          {
            // Signal emitted from the WindowCompositor.
            key_nav_focus_activate.emit(key_focus_area_.GetPointer());
            // Signal emitted from the area itsel.
            key_focus_area_->key_nav_focus_activate.emit(key_focus_area_.GetPointer());
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
            SetKeyFocusArea(key_nav_focus, direction);
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
  void WindowCompositor::ProcessEvent(Event& event)
  {
    inside_event_cycle_ = true;
    if (((event.type >= NUX_MOUSE_PRESSED) && (event.type <= NUX_MOUSE_WHEEL)) ||
    (event.type == NUX_WINDOW_MOUSELEAVE))
    {
#if !defined(NUX_MINIMAL)
      bool menu_active = false;
      if (!_menu_chain->empty())
      {
        menu_active = true;
        MenuEventCycle(event);
        CleanMenu();
      }

      if ((menu_active && on_menu_closure_continue_with_event_) || !(menu_active))
#endif
      {
        MouseEventCycle(event);
      }

      on_menu_closure_continue_with_event_ = false;

      if (_starting_menu_event_cycle)
      {
        _starting_menu_event_cycle = false;
      }
    }
    else if ((event.type >= NUX_KEYDOWN) && (event.type <= NUX_KEYUP))
    {
      KeyboardEventCycle(event);
    }
    else if ((event.type >= NUX_DND_MOVE) && (event.type <= NUX_DND_LEAVE_WINDOW))
    {
      DndEventCycle(event);
    }
#ifdef NUX_GESTURES_SUPPORT
    else if (event.type == EVENT_GESTURE_BEGIN)
    {
      gesture_broker_->ProcessGestureBegin(static_cast<GestureEvent&>(event));
    }
    else if (event.type == EVENT_GESTURE_UPDATE)
    {
      gesture_broker_->ProcessGestureUpdate(static_cast<GestureEvent&>(event));
    }
    else if (event.type == EVENT_GESTURE_END)
    {
      gesture_broker_->ProcessGestureEnd(static_cast<GestureEvent&>(event));
    }
#endif
    inside_event_cycle_ = false;
  }

  void WindowCompositor::StartModalWindow(WeakBaseWindowPtr window)
  {
    if (window == 0)
      return;

    WindowList::iterator it = find(_modal_view_window_list.begin(), _modal_view_window_list.end(), window);

    if (it == _modal_view_window_list.end())
    {
      _modal_view_window_list.push_front(window);
    }
  }

  void WindowCompositor::StopModalWindow(WeakBaseWindowPtr window)
  {
    if (!_modal_view_window_list.empty())
    {
      if (*_modal_view_window_list.begin() == window)
        _modal_view_window_list.pop_front();
    }
  }

  //! Push a floating view at the top of the stack.
  void WindowCompositor::PushToFront(BaseWindow* window)
  {
    if (window == 0)
      return;

    WindowList::iterator it = find(_view_window_list.begin(), _view_window_list.end(), window);

    if (it != _view_window_list.end())
    {
      _view_window_list.erase(it);
      _view_window_list.push_front(WeakBaseWindowPtr(window));
    }

    EnsureAlwaysOnFrontWindow();
  }

  //! Push a floating view at the bottom of the stack.
  void WindowCompositor::PushToBack(BaseWindow* window)
  {
    if (window == 0)
      return;

    if (window == _always_on_front_window)
      return;

    WindowList::iterator it = find(_view_window_list.begin(), _view_window_list.end(), window);

    if (it != _view_window_list.end())
    {
      _view_window_list.erase(it);
      _view_window_list.push_back(WeakBaseWindowPtr(window));
    }

    EnsureAlwaysOnFrontWindow();
  }

  //! Push a floating view just above another floating view.
  void WindowCompositor::PushHigher(BaseWindow* top_floating_view, BaseWindow* bottom_floating_view, bool strict)
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

    for (it_top = _view_window_list.begin(), i = 0; it_top != _view_window_list.end(); ++it_top, ++i)
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
      _view_window_list.insert(it_bot, WeakBaseWindowPtr(top_floating_view));
    }

    EnsureAlwaysOnFrontWindow();
  }

  void WindowCompositor::SetAlwaysOnFrontWindow(BaseWindow* window)
  {
    _always_on_front_window = WeakBaseWindowPtr(window);

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

  int WindowCompositor::GetProximityListSize() const
  {
    return area_proximities_.size();
  }

  void WindowCompositor::AddAreaInProximityList(InputAreaProximity* prox_area)
  {
    if (prox_area)
    {
      area_proximities_.push_back(prox_area);
    }
    else
    {
      LOG_ERROR(logger) << "Error, attempted to add a NULL InputAreaProximity to the list.";
    }
  }

  void WindowCompositor::RemoveAreaInProximityList(InputAreaProximity* prox_area)
  {
    if (prox_area)
    {
      area_proximities_.remove(prox_area);
    }
  }

  void WindowCompositor::CheckMouseNearArea(Event const& event)
  {
    for (auto area : area_proximities_)
    {
      if (area)
      {
        area->CheckMousePosition(Point(event.x, event.y));
      }
    }
  }

  void WindowCompositor::ForEachBaseWindow(ForEachBaseWindowFunc const& func)
  {
    for (auto const& window : _view_window_list)
    {
      if (window.IsValid())
        func(window);
    }

    for (auto const& window : _modal_view_window_list)
    {
      if (window.IsValid())
        func(window);
    }

    if (m_MenuWindow.IsValid())
      func(m_MenuWindow);

    if (_tooltip_window.IsValid())
      func(_tooltip_window);

    if (m_OverlayWindow.IsValid())
      func(m_OverlayWindow);
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
      window_thread_->GetGraphicsEngine().Push2DTranslationModelViewMatrix(0.0f, 0.0f, 0.0f);


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
      else if (m_MenuRemoved)
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

      m_MenuRemoved = false;

      window_thread_->GetGraphicsEngine().Pop2DWindow();
    }
    inside_rendering_cycle_ = false;
  }

  void WindowCompositor::DrawMenu(bool force_draw)
  {
#if !defined(NUX_MINIMAL)
    WeakBaseWindowPtr window(m_MenuWindow);

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

    std::list<MenuPage*>::reverse_iterator rev_it_menu;

    for (rev_it_menu = _menu_chain->rbegin(); rev_it_menu != _menu_chain->rend( ); ++rev_it_menu)
    {
      SetProcessingTopView(m_MenuWindow.GetPointer());
      (*rev_it_menu)->ProcessDraw(window_thread_->GetGraphicsEngine(), force_draw);
      SetProcessingTopView(NULL);
    }
#endif
  }

  void WindowCompositor::DrawOverlay(bool /* force_draw */)
  {
    int buffer_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    int buffer_height = window_thread_->GetGraphicsEngine().GetWindowHeight();

    if (m_OverlayWindow.IsValid())
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

  void WindowCompositor::DrawTooltip(bool /* force_draw */)
  {
    int buffer_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    int buffer_height = window_thread_->GetGraphicsEngine().GetWindowHeight();

    if (_tooltip_window.IsValid())
    {
      //window_thread_->GetGraphicsEngine().SetContext(x, y, buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().EmptyClippingRegion();
    }
    else
      window_thread_->GetGraphicsEngine().SetOpenGLClippingRectangle(0, 0, buffer_width, buffer_height);

    if (m_TooltipText.size())
    {
        //SetProcessingTopView(_tooltip_window);
        GetPainter().PaintShape(window_thread_->GetGraphicsEngine(), _tooltip_geometry, Color(0xA0000000), eSHAPE_CORNER_ROUND10, true);
        GetPainter().PaintTextLineStatic(window_thread_->GetGraphicsEngine(), GetSysBoldFont(), _tooltip_text_geometry, m_TooltipText, Color(0xFFFFFFFF));
        //SetProcessingTopView(NULL);
    }

    //GetGraphicsDisplay()->GetGraphicsEngine()->SetContext(0, 0, buffer_width, buffer_height);
  }

  void WindowCompositor::RenderTopViewContent(BaseWindow* window, bool force_draw)
  {
    GetPainter().EmptyBackgroundStack();
    SetProcessingTopView(window);
    window->ProcessDraw(window_thread_->GetGraphicsEngine(), force_draw || window->IsRedrawNeeded());
    SetProcessingTopView(NULL);
    GetPainter().EmptyBackgroundStack();
  }

  void WindowCompositor::PresentAnyReadyWindows()
  {
    if (!currently_rendering_windows_ || !current_global_clip_rect_)
      return;

    GraphicsEngine& graphics_engine = window_thread_->GetGraphicsEngine();

    // Present all buffers to the screen
    graphics_engine.ApplyClippingRectangle();
    CHECKGL(glDepthMask(GL_FALSE));

    WindowList &windows = *currently_rendering_windows_;
    Geometry   &global_clip_rect = *current_global_clip_rect_;

    for (auto const& window_ptr : windows)
    {
      if (window_ptr.IsNull())
        continue;

      BaseWindow* window = window_ptr.GetPointer();

      if (window->IsVisible())
      {
        auto const& win_geo = window->GetGeometry();

        if (!global_clip_rect.IsIntersecting(win_geo))
        {
          // The global clipping area can be seen as a per monitor clipping
          // region. It is mostly used in embedded mode with compiz.  If we
          // get here, it means that the BaseWindow we want to render is not
          // in area of the monitor that compiz is currently rendering. So
          // skip it.
          continue;
        }

        RenderTargetTextures& rt = GetWindowBuffer(window);

        if (rt.color_rt.IsValid())
        {
          /* Already been presented */
          if (!window->_contents_ready_for_presentation)
            continue;

          /* Caller doesn't want us to render this yet */
          if (window_thread_->IsEmbeddedWindow() && !window->AllowPresentationInEmbeddedMode())
            continue;

          // Nux is done rendering a BaseWindow into a texture. The previous call to Deactivate
          // has cancelled any opengl framebuffer object that was set.
          PresentBufferToScreen(rt.color_rt, win_geo.x, win_geo.y, false, false, window->GetOpacity(), window->premultiply());

          window->_contents_ready_for_presentation = false;
        }
      }
    }
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
    graphics_engine.SetViewport(0, 0, window_width, window_height);
    graphics_engine.EmptyClippingRegion();

    Geometry global_clip_rect = graphics_engine.GetScissorRect();
    global_clip_rect.y = window_height - global_clip_rect.y - global_clip_rect.height;

    current_global_clip_rect_ = &global_clip_rect;

    // We don't need to restore framebuffers if we didn't update any windows
    bool updated_any_windows = false;

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

    currently_rendering_windows_ = &windows;

    for (auto const& window_ptr : windows)
    {
      if (window_ptr.IsNull())
        continue;

      BaseWindow* window = window_ptr.GetPointer();

      if (!drawModal && window->IsModal())
        continue;

      if (window->IsVisible())
      {
        auto const& win_geo = window->GetGeometry();
        if (!global_clip_rect.IsIntersecting(win_geo))
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
            m_FrameBufferObject->SetTextureAttachment(0, rt.color_rt, 0);
            m_FrameBufferObject->SetDepthTextureAttachment(rt.depth_rt, 0);
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
            Matrix4 mat;
            mat.Translate(win_geo.x, win_geo.y, 0);
            graphics_engine.SetOrthographicProjectionMatrix(window_width, window_height);
          }

          RenderTopViewContent(window, force_draw);

          m_FrameBufferObject->Deactivate();
          CHECKGL(glDepthMask(GL_TRUE));
          graphics_engine.GetRenderStates().SetBlend(false);
          updated_any_windows = true;
        }

        window->_child_need_redraw = false;
        window->_contents_ready_for_presentation = true;
      }
      else
      {
        // Invisible window, nothing to draw.
        window->_child_need_redraw = false;
        window->DoneRedraw();
      }
    }

    /* If any windows were updated, then we need to rebind our
     * reference framebuffer */
    if (updated_any_windows)
    {
      if (GetWindowThread()->IsEmbeddedWindow())
      {
        // Restore the reference framebuffer
        if (!RestoreReferenceFramebuffer())
        {
          LOG_DEBUG(logger) << "RenderTopViews: Setting the Reference fbo has failed.";
        }
      }
      else
      {
        GetGraphicsDisplay()->GetGpuDevice()->DeactivateFrameBuffer();
      }
    }

    /* Present any windows which haven't yet been presented */
    PresentAnyReadyWindows();

    currently_rendering_windows_ = nullptr;
    current_global_clip_rect_ = nullptr;
  }

  void WindowCompositor::RenderMainWindowComposition(bool force_draw)
  {
    int buffer_width, buffer_height;

    buffer_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    buffer_height = window_thread_->GetGraphicsEngine().GetWindowHeight();

    if ((!m_MainColorRT.IsValid()) || (m_MainColorRT->GetWidth() != buffer_width) || (m_MainColorRT->GetHeight() != buffer_height))
    {
      m_MainColorRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
    }

    if (platform_support_for_depth_texture_)
    {
      if ((!m_MainDepthRT.IsValid()) || (m_MainDepthRT->GetWidth() != buffer_width) || (m_MainDepthRT->GetHeight() != buffer_height))
      {
        m_MainDepthRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
      }
    }

    m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
    m_FrameBufferObject->SetTextureAttachment(0, m_MainColorRT, 0);
    m_FrameBufferObject->SetDepthTextureAttachment(m_MainDepthRT, 0);
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

  void WindowCompositor::PresentBufferToScreen(ObjectPtr<IOpenGLBaseTexture> HWTexture, int x, int y, bool RenderToMainTexture, bool /* BluredBackground */, float opacity, bool premultiply)
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
      m_FrameBufferObject->SetTextureAttachment(0, m_MainColorRT, 0);
      m_FrameBufferObject->SetDepthTextureAttachment(m_MainDepthRT, 0);
      m_FrameBufferObject->Activate();
    }

    // Reference framebuffer is already restored

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

#if !defined(NUX_MINIMAL)
  void WindowCompositor::AddMenu(MenuPage* menu, BaseWindow* window, bool OverrideCurrentMenuChain)
  {
    if (_menu_chain->empty())
    {
      // A menu is opening.
      _starting_menu_event_cycle = true;
      _menu_is_active = true;
    }

    std::list<MenuPage*>::iterator it = find(_menu_chain->begin(), _menu_chain->end(), menu);
    if (it == _menu_chain->end())
    {
      // When adding a MenuPage, make sure that it is a child of the MenuPage in _menu_chain->begin().
      if (!_menu_chain->empty())
      {
        if (menu->GetParentMenu() != (*_menu_chain->begin()))
        {
          if (OverrideCurrentMenuChain)
          {
            // Remove the current menu chain
            for (it = _menu_chain->begin(); it != _menu_chain->end(); ++it)
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
  void WindowCompositor::RemoveMenu(MenuPage* menu)
  {
    std::list<MenuPage*>::iterator it = find(_menu_chain->begin(), _menu_chain->end(), menu);

    if (it == _menu_chain->end())
    {
      return;
    }

    _menu_chain->erase(it);
    m_MenuRemoved = true;

    if (_menu_is_active && (_menu_chain->empty()))
    {
      // The menu is closed
      _menu_is_active         = false;
      ResetMousePointerAreas();
      m_MenuWindow            = NULL;
    }
  }

  void WindowCompositor::CleanMenu()
  {
    if (_menu_chain->empty())
      return;

    std::list<MenuPage*>::iterator menu_it = _menu_chain->begin();

    while (menu_it != _menu_chain->end())
    {
      if ((*menu_it)->IsActive() == false)
      {
        menu_it = _menu_chain->erase(menu_it);
        m_MenuRemoved = true;
      }
      else
      {
        ++menu_it;
      }
    }

    if (_menu_is_active && (_menu_chain->empty()))
    {
      _menu_is_active         = false;
      ResetMousePointerAreas();
      m_MenuWindow            = NULL;
    }
  }
#endif

  void WindowCompositor::SetWidgetDrawingOverlay(InputArea* ic, BaseWindow* OverlayWindow)
  {
    OverlayDrawingCommand = ic;
    m_OverlayWindow = OverlayWindow;
  }

  InputArea* WindowCompositor::GetWidgetDrawingOverlay()
  {
    return OverlayDrawingCommand;
  }

  void WindowCompositor::SetTooltip(InputArea* TooltipArea, const char* TooltipText, int x, int y)
  {
    _tooltip_window = GetProcessingTopView();
    m_TooltipArea = TooltipArea;
    m_TooltipText = TooltipText;
    m_TooltipX = x;
    m_TooltipY = y;

    if (m_TooltipText.size())
    {
      int w = GetSysBoldFont()->GetCharStringWidth(m_TooltipText.c_str());
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

  bool WindowCompositor::SetKeyFocusArea(InputArea* area, KeyNavDirection direction)
  {
    InputArea* keyboard_grab_area = GetKeyboardGrabArea();

    if (keyboard_grab_area  && area && (area != keyboard_grab_area) && (!area->IsChildOf(keyboard_grab_area)))
    {
      // There is a keyboard grab pending. Only an area that is a child of the area that has
      // the keyboard grab can be set to receive keyboard events.
      LOG_DEBUG(logger) << "SetKeyFocusArea: There is a keyboard grab pending. Cannot change the keyboard event receiver.";
      return false;
    }

    if (key_focus_area_ == area)
    {
      // Already has the keyboard focus.
      return true;
    }

    if (area && (area->AcceptKeyNavFocus() == false))
    {
      // Area does not want the keyboard focus.
      return false;
    }

    if (key_focus_area_.IsValid())
    {
      // This is the area that has the keyboard focus. Emit the signal 'end_key_focus'.
      key_focus_area_->end_key_focus.emit();

      // From the area that has the keyboard focus to the top level parent, delete the path that
      // leads to the keyboard focus area.
      key_focus_area_->ResetUpwardPathToKeyFocusArea();

      if (key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
      {
        // Signal emitted from the WindowCompositor.
        key_nav_focus_change.emit(key_focus_area_.GetPointer(), false, direction);
        // Signal emitted from the area itself.
        key_focus_area_->key_nav_focus_change.emit(key_focus_area_.GetPointer(), false, direction);
        // nuxDebugMsg("[WindowCompositor::SetKeyFocusArea] Area type '%s' named '%s': Lost key nav focus.",
        //   key_focus_area_->Type().name,
        //   key_focus_area_->GetBaseString().c_str());
      }

      if (key_focus_area_->Type().IsDerivedFromType(View::StaticObjectType))
      {
        static_cast<View*>(key_focus_area_.GetPointer())->QueueDraw();
      }
    }

    if (area)
    {
      key_focus_area_ = area;

      // From the area that has the keyboard focus to the top level parent, mark the path that
      // leads to the keyboard focus area.
      key_focus_area_->SetPathToKeyFocusArea();

      // Emit the signal 'begin_key_focus'.
      key_focus_area_->begin_key_focus.emit();

      if (key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
      {
        // Signal emitted from the WindowCompositor.
        key_nav_focus_change.emit(key_focus_area_.GetPointer(), true, direction);
        // Signal emitted from the area itself.
        key_focus_area_->key_nav_focus_change.emit(key_focus_area_.GetPointer(), true, direction);
        // nuxDebugMsg("[WindowCompositor::SetKeyFocusArea] Area type '%s' named '%s': Has key nav focus.",
        //   key_focus_area_->Type().name,
        //   key_focus_area_->GetBaseString().c_str());
      }

      if (key_focus_area_->Type().IsDerivedFromType(View::StaticObjectType))
      {
        static_cast<View*>(key_focus_area_.GetPointer())->QueueDraw();
      }

      key_focus_area_->ChildFocusChanged.emit(key_focus_area_.GetPointer());

    }
    else
    {
      key_focus_area_ = NULL;

      // Signal emitted from the area itself.
      key_nav_focus_change.emit(NULL, false, direction);
    }

    return key_focus_area_.IsValid();
  }

  InputArea* WindowCompositor::GetKeyFocusArea()
  {
    return key_focus_area_.GetPointer();
  }

  void WindowCompositor::SetBackgroundPaintLayer(AbstractPaintLayer* bkg)
  {
    NUX_SAFE_DELETE(m_Background);
    m_Background = bkg->Clone();
  }

  void WindowCompositor::FloatingAreaConfigureNotify(int Width, int Height)
  {
    WindowList::iterator it;

    for (auto const& win : _view_window_list)
    {
      if (!win.IsValid())
        continue;

      if (win->IsVisible())
        win->NotifyConfigurationChange(Width, Height);
    }
  }

  void WindowCompositor::FormatRenderTargets(int /* width */, int /* height */)
  {
    int buffer_width = window_thread_->GetGraphicsEngine().GetWindowWidth();
    int buffer_height = window_thread_->GetGraphicsEngine().GetWindowHeight();

    nuxAssert(buffer_width >= 1);
    nuxAssert(buffer_height >= 1);

    m_MainColorRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
    if (platform_support_for_depth_texture_)
    {
      m_MainDepthRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
    }

    // Clear the buffer the first time...
    m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
    m_FrameBufferObject->SetTextureAttachment(0, m_MainColorRT, 0);
    m_FrameBufferObject->SetDepthTextureAttachment(m_MainDepthRT, 0);
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
    BaseWindow* top_view = GetProcessingTopView();

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
      }

      if (platform_support_for_depth_texture_)
      {
        if ((rt.depth_rt->GetWidth() != buffer_width) || (rt.depth_rt->GetHeight() != buffer_height))
        {
          rt.depth_rt = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(buffer_width, buffer_height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
        }
      }

      m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
      m_FrameBufferObject->SetTextureAttachment(0, rt.color_rt, 0);
      m_FrameBufferObject->SetDepthTextureAttachment(rt.depth_rt, 0);
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
      // Restore Main Frame Buffer if not in embedded mode
      if (!GetWindowThread()->IsEmbeddedWindow())
      {
        m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
        m_FrameBufferObject->SetTextureAttachment(0, m_MainColorRT, 0);
        m_FrameBufferObject->SetDepthTextureAttachment(m_MainDepthRT, 0);
        m_FrameBufferObject->Activate();
      }
      else
      {
        // Restore reference framebuffer
        RestoreReferenceFramebuffer();

        // Present any ready windows
        PresentAnyReadyWindows();
      }

      window_thread_->GetGraphicsEngine().SetViewport(0, 0, buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().SetOrthographicProjectionMatrix(buffer_width, buffer_height);
      window_thread_->GetGraphicsEngine().ApplyClippingRectangle();
      //window_thread_->GetGraphicsEngine().ApplyModelViewMatrix(); ???
    }
  }

  void* WindowCompositor::GetBackupTextureData(BaseWindow* base_window, int& width, int& height, int& format)
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
#if defined(DRAG_AND_DROP_SUPPORTED)
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
      LOG_DEBUG(logger) << "GrabPointerAdd: The area already has the grab";
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
      LOG_DEBUG(logger) << "GrabKeyboardAdd: The area already has the grab";
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
      if (key_focus_area_.IsValid())
      {
        key_focus_area_->end_key_focus.emit();
        key_focus_area_->ResetUpwardPathToKeyFocusArea();

        if (key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
        {
          // Signal emitted from the WindowCompositor.
          key_nav_focus_change.emit(key_focus_area_.GetPointer(), false, KEY_NAV_NONE);
          // Signal emitted from the area itself.
          key_focus_area_->key_nav_focus_change.emit(key_focus_area_.GetPointer(), false, KEY_NAV_NONE);
          // nuxDebugMsg("[WindowCompositor::GrabKeyboardAdd] Area type '%s' named '%s': Lost key nav focus.",
          //   key_focus_area_->Type().name,
          //   key_focus_area_->GetBaseString().c_str());

        }

        if (key_focus_area_->Type().IsDerivedFromType(View::StaticObjectType))
        {
          static_cast<View*>(key_focus_area_.GetPointer())->QueueDraw();
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
      if (key_focus_area_.IsValid())
      {
        key_focus_area_->end_key_focus.emit();
        key_focus_area_->ResetUpwardPathToKeyFocusArea();

        if (key_focus_area_->Type().IsDerivedFromType(InputArea::StaticObjectType))
        {
          // Signal emitted from the WindowCompositor.
          key_nav_focus_change.emit(key_focus_area_.GetPointer(), false, KEY_NAV_NONE);
          // Signal emitted from the area itself.
          key_focus_area_->key_nav_focus_change.emit(key_focus_area_.GetPointer(), false, KEY_NAV_NONE);
          // nuxDebugMsg("[WindowCompositor::GrabKeyboardRemove] Area type '%s' named '%s': Lost key nav focus.",
          //   key_focus_area_->Type().name,
          //   key_focus_area_->GetBaseString().c_str());
        }

        if (key_focus_area_->Type().IsDerivedFromType(View::StaticObjectType))
        {
          static_cast<View*>(key_focus_area_.GetPointer())->QueueDraw();
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
    if (keyboard_grab_stack_.empty())
      return NULL;

    return (*keyboard_grab_stack_.begin());
  }

  void WindowCompositor::SetReferenceFramebuffer(unsigned int draw_fbo_object, unsigned int read_fbo_object, Geometry const& fbo_geometry)
  {
    draw_reference_fbo_ = draw_fbo_object;
    read_reference_fbo_ = read_fbo_object;
    reference_fbo_geometry_ = fbo_geometry;
  }

  namespace
  {
    bool CheckExternalFramebufferStatus (GLenum binding)
    {
      bool ok = false;
      // Nux does some sanity checks to make sure that the FBO is in good condition.
      GLenum status;
      status = glCheckFramebufferStatusEXT(binding);
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

    void SetReferenceFramebufferViewport(const nux::Geometry &reference_fbo_geometry_)
    {
      CHECKGL(glViewport(reference_fbo_geometry_.x,
        reference_fbo_geometry_.y,
        reference_fbo_geometry_.width,
        reference_fbo_geometry_.height));
    }
  }

  bool WindowCompositor::RestoreReferenceFramebuffer()
  {
    // It is assumed that the reference fbo contains valid textures.
    // Nux does the following:
    //    - Bind the reference fbo (reference_fbo_)
    //    - Call glDrawBuffer with GL_COLOR_ATTACHMENT0
    //    - Set the opengl viewport size (reference_fbo_geometry_)

#ifndef NUX_OPENGLES_20
    CHECKGL(glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, draw_reference_fbo_));
    CHECKGL(glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, read_reference_fbo_));
    if (draw_reference_fbo_)
    {
      CHECKGL(glDrawBuffer(GL_COLOR_ATTACHMENT0));
    }
    else
    {
      CHECKGL(glDrawBuffer(GL_BACK));
    }

    if (read_reference_fbo_)
    {
      CHECKGL(glReadBuffer(GL_COLOR_ATTACHMENT0));
    }
    else
    {
      CHECKGL(glReadBuffer(GL_BACK));
    }
#else
    nuxAssertMsg(draw_reference_fbo_ == read_reference_fbo_,
                 "[WindowCompositor::RestoreReferenceFramebuffer]: OpenGL|ES does not"\
                 " support separate draw and read framebuffer bindings, using the supplied"\
                 " draw binding");
    CHECKGL(glBindFramebufferEXT(GL_FRAMEBUFFER, draw_reference_fbo_));
#endif

    SetReferenceFramebufferViewport(reference_fbo_geometry_);

#ifndef NUX_OPENGLES_20
    int restore_status =
           (!draw_reference_fbo_ ||
            CheckExternalFramebufferStatus(GL_DRAW_FRAMEBUFFER_EXT)) &&
           (!read_reference_fbo_ ||
            CheckExternalFramebufferStatus(GL_READ_FRAMEBUFFER_EXT));
#else
    int restore_status = CheckExternalFramebufferStatus(GL_FRAMEBUFFER);
#endif

    return restore_status;
  }

  void WindowCompositor::RestoreMainFramebuffer()
  {
    // This is a bit inefficient as we unbind and then rebind
    nux::GetGraphicsDisplay()->GetGpuDevice()->DeactivateFrameBuffer ();
    RestoreReferenceFramebuffer ();

    /* Present any ready windows after restoring
     * the reference framebuffer. This ensures that if
     * we need to restore the reference framebuffer to
     * get access to its contents through glCopyTexSubImage2D
     * that it will also have any rendered views in it too
     */
    PresentAnyReadyWindows();
  }

#ifdef NUX_GESTURES_SUPPORT
  void WindowCompositor::SetGestureBroker(std::unique_ptr<GestureBroker> gesture_broker)
  {
    gesture_broker_ = std::move(gesture_broker);
  }

  InputArea *WindowCompositor::LocateGestureTarget(const GestureEvent &event)
  {
    InputArea *input_area = nullptr;

    for (auto const& window : _view_window_list)
    {
      if (!window.IsValid())
        continue;

      input_area = static_cast<InputArea*>(window->GetInputAreaHitByGesture(event));
      if (input_area)
        break;
    }

    // If a target InputArea wasn't found in any of the BaseWindows, then check
    // the InputAreas in the layout of the main window.
    if (!input_area)
    {
      Layout* main_window_layout = window_thread_->GetLayout();
      if (main_window_layout)
        input_area = static_cast<InputArea*>(
            main_window_layout->GetInputAreaHitByGesture(event));
    }

    return input_area;
  }

  DefaultGestureBroker::DefaultGestureBroker(WindowCompositor *window_compositor)
    : window_compositor_(window_compositor)
  {
  }

  std::vector<ShPtGestureTarget>
  DefaultGestureBroker::FindGestureTargets(const nux::GestureEvent &event)
  {
    std::vector<ShPtGestureTarget> targets;

    InputArea *target_area = window_compositor_->LocateGestureTarget(event);
    if (target_area)
      targets.push_back(ShPtGestureTarget(new InputAreaTarget(target_area)));

    return targets;
  }
#endif
}

