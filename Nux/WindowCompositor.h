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


#ifndef WINDOWCOMPOSITOR_H
#define WINDOWCOMPOSITOR_H

#include <functional>

#include "BaseWindow.h"

#include <sigc++/trackable.h>
#include <sigc++/connection.h>

#include <NuxCore/ObjectPtr.h>

#ifdef NUX_GESTURES_SUPPORT
#include <unordered_map>
#include "Gesture.h"
#include "GestureBroker.h"
#endif

namespace nux
{
  class MenuPage;
  class PBuffer;
  class WindowThread;
  class View;
  class InputArea;
  class InputAreaProximity;
  class Area;
  class PaintLayer;
  class Event;

  //! A user interface composition class created by WindowThread.
  class WindowCompositor : public sigc::trackable
  {
  public:
    typedef ObjectWeakPtr<BaseWindow> WeakBaseWindowPtr;
    typedef std::function <void(WeakBaseWindowPtr const&)> ForEachBaseWindowFunc;

    WindowCompositor(WindowThread* window_thread);
    ~WindowCompositor();

    //! Get the Geometry of the tooltip based on the BaseWindow that initiated it.
    Geometry GetTooltipGeometry() const;

    //! Get the Geometry of the tooltip based on the MainWindow.
    Geometry GetTooltipMainWindowGeometry() const;

//     bool MouseDown(Point pt);
// 
//     bool MouseMove(Point pt);
//     bool MouseUp(Point pt);

    void ProcessEvent(Event& event);

    //====================================
    void MouseEventCycle(Event& event);
    void DndEventCycle(Event& event);

    Point _mouse_position_on_owner;
    Point _mouse_position;

    //! Get Mouse position relative to the top left corner of the window.
    Point GetMousePosition();

    int GetProximityListSize() const;
    void AddAreaInProximityList(InputAreaProximity* area_prox);
    void RemoveAreaInProximityList(InputAreaProximity* area_prox);

    void KeyboardEventCycle(Event& event);

#if !defined(NUX_MINIMAL)
    void MenuEventCycle(Event& event);
    MenuPage* _mouse_owner_menu_page;
    MenuPage* _mouse_over_menu_page;
#endif

    bool      _starting_menu_event_cycle;
    bool      _menu_is_active;

    //! Set the area that has the keyboard focus.
    /*!
        Set the area that has the keyboard focus. If the \i area parameter is NULL, \n
        the area that has the keyboard focus looses it and the function returns false.

        @param The area that that gets the keyboard focus.
        @param The key nav direction that initiated the focus to the provided area.

        @return True if the area succeeded to get the keyboard focus or it already has it.
    */
    bool SetKeyFocusArea(InputArea* area, KeyNavDirection direction = KEY_NAV_NONE);

    //! Return the area that has the keyboard focus.
    /*!
        Return the area that has the keyboard focus.

        @return The area that has the keyboard focus.
    */
    InputArea* GetKeyFocusArea();

    void ForEachBaseWindow(ForEachBaseWindowFunc const&);

    //! Signal emitted when a BaseWindow becomes visible.
    /*!
        This signal is emitted after the BaseWindow has emitted it own sigVisible signal. 
    */
    sigc::signal<void, BaseWindow*> sigVisibleViewWindow;    //!< Signal emitted when the BaseWindow becomes visible.

    //! Signal emitted when a BaseWindow becomes hidden.
    /*!
        This signal is emitted after the BaseWindow has emitted it own sigHidden signal. 
    */
    sigc::signal<void, BaseWindow*> sigHiddenViewWindow;    //!< Signal emitted when the BaseWindow becomes hidden.

  private:
    /*** Helper functions for MouseEventCycle() ***/
    void UpdateKeyNavFocusOnMouseDown();
    /* Keeps track of mouse movement and emits mouse_move and mouse_drag
       accordingly. */
    void TrackMouseMovement(const Event &event, bool area_under_mouse_changed);
    /* Updates mouse_over_area_ and emits mouse_enter and mouse_leave signals
       accordingly. Returns whether mouse_over_area_ has changed */
    bool UpdateWhatAreaIsUnderMouse(const Event& event);
    /* Processes NUX_MOUSEWHEEL events */
    void ProcessMouseWheelEvent(Event& event);
    /* Updates mouse_owner_area_ and emits mouse_down, mouse_up, mouse_click
       and mouse_double_click accordingly. */
    void UpdateMouseOwner(const Event& event, bool area_under_mouse_changed);
    /*
       Feed the appropriate InputArea::ChildMouseEvent() and switch mouse ownership
       (including the emission of mouse_cancel) if asked to.
      */
    void UpdateEventTrackingByMouseOwnerAncestor(const Event& event);
    void FindAncestorInterestedInChildMouseEvents(Area *area);

    //! Traverse the widget tree and found the area that is right below the mouse pointer.
    void FindAreaUnderMouse(const Point& mouse_position,
                           NuxEventType event_type,
                           ObjectWeakPtr<InputArea>& area_under_mouse_pointer);
    void GetAreaUnderMouse(const Point& mouse_position,
                           NuxEventType event_type,
                           ObjectWeakPtr<InputArea>& area_under_mouse_pointer,
                           WeakBaseWindowPtr& window);

    //! Traverse the widget tree and found the area has the key focus.
    void FindKeyFocusArea(NuxEventType event_type,
                          unsigned int key_symbol,
                          unsigned int special_keys_state,
                          ObjectWeakPtr<InputArea>& key_focus_area,
                          WeakBaseWindowPtr& window);

    //! Traverse the widget tree and found the area has the key focus, but start from a specified widget.
    void FindKeyFocusAreaFrom(NuxEventType event_type,
      unsigned int key_symbol,
      unsigned int special_keys_state,
      InputArea* root_search_area,
      ObjectWeakPtr<InputArea>& key_focus_area,
      WeakBaseWindowPtr& window);

    void ResetMousePointerAreas();

    //! Get the area upon which the mouse button is currently down.
    ObjectWeakPtr<InputArea> const& GetMouseOwnerArea() const;

    //! Set the area upon which the mouse button is currently down.
    void SetMouseOwnerArea(InputArea* area);

    //! Set the area that is right below the mouse pointer.
    void SetMouseOverArea(InputArea* area);

    void SendKeyEvent(InputArea* input_area, NuxEventType event_type,
      unsigned int key_sym,
      unsigned long x11_key_code,
      unsigned long special_keys_state,
      const char* text,
      int key_repeat_count);
    
    //! Checks the list of porximities to see if the mouse is near any areas. 
    void CheckMouseNearArea(Event const& event);

    //! The InputArea that has the keyboard navigation focus.
    /*!
        The InputArea that has the mouse focus also has the keyboard focus. That is if _mouse_focus_area is not Null
        then _mouse_focus_area is equal to _mouse_focus_area;
    */
    ObjectWeakPtr<InputArea> key_focus_area_;
    ObjectWeakPtr<InputArea> mouse_owner_area_;
    ObjectWeakPtr<InputArea> mouse_over_area_;

    /* An ancestor of the current mouse owner area that wants to be notified
       about all mouse events that a descendant receives and optionally
       take ownership over the mouse at any given moment.
       \sa InputArea::ChildMouseEvent()
     */
    /* TODO: Make it a list/vector once the need for nested trackers. */
    ObjectWeakPtr<InputArea> interested_mouse_owner_ancestor_;

    int dnd_safety_x_;
    int dnd_safety_y_;

  public:
    /*!
        This signal is similar to Area::key_nav_focus_change. It is emitted from the WindowCompositor.
        The user only needs to listen to this signal to find out the area that has received the keyboard focus.\n
        The signal is received whether the area receiving or loosing the keyboard focus.\n
        If the second parameter is true, it means the area is receiving the focus.\n
        The third parameter of this signal indicates the keyboard action that triggered this area \n
        to receive or loose the keyboard focus.
    */
    sigc::signal<void, Area*, bool, KeyNavDirection> key_nav_focus_change;

    /*!
        This signal is similar to Area::key_nav_focus_activate. It is emitted from the WindowCompositor.
        The user only needs to listen to this signal to find out the area that has has been activated
        through the keyboard (ENTER key has been pressed).\n
    */
    sigc::signal<void, Area*> key_nav_focus_activate;

    //====================================
  
  public:
    /*!
        Set and external fbo to draw Nux BaseWindow into. This external fbo will be
        restored after Nux completes it rendering. The external fbo is used only in embedded mode. \n
        If the fbo_object parameter 0, then the reference fbo is invalid and will not be used.

        @param draw_fbo_object The opengl index of the GL_DRAW_FRAMEBUFFER_EXT.
        @param read_fbo_object The opengl index of the GL_READ_FRAMEBUFFER_EXT.
        @param fbo_geometry The geometry of the fbo.
    */
    void SetReferenceFramebuffer(unsigned int draw_fbo_object, unsigned int read_fbo_object, const Geometry &fbo_geometry);

    /*!
        Bind the reference opengl framebuffer object.

        @return True if no error was detected.
    */
    bool RestoreReferenceFramebuffer();
    void RestoreMainFramebuffer();

    ObjectPtr<IOpenGLFrameBufferObject>& GetWindowFrameBufferObject()
    {
      return m_FrameBufferObject;
    }
    
    ObjectPtr<IOpenGLFrameBufferObject> m_FrameBufferObject;

    void StartModalWindow(WeakBaseWindowPtr);
    void StopModalWindow(WeakBaseWindowPtr);

#if !defined(NUX_MINIMAL)
    void AddMenu(MenuPage* menu, BaseWindow* window, bool OverrideCurrentMenuChain = true);
    void RemoveMenu(MenuPage* menu);
    void CleanMenu();
#endif

    void PushModalWindow(WeakBaseWindowPtr window);

    void SetWidgetDrawingOverlay(InputArea* ic, BaseWindow* OverlayWindow);
    InputArea* GetWidgetDrawingOverlay();

    void SetTooltip(InputArea* TooltipArea, const char* TooltipText, int x, int y);
    /*!
        Return true if the mouse is still inside the area that initiated the tooltip;

        @param x    The mouse x coordinate on screen.
        @param y    The mouse y coordinate on screen.
        @return     Return true is the mouse is still inside the area.
    */
    bool ValidateMouseInsideTooltipArea(int x, int y);
    /*!
        Return true if there is a valid tooltip active.

        @return     Return true if there is a valid tooltip active.
    */
    bool IsTooltipActive();
    void CancelTooltip();

    void SetBackgroundPaintLayer(AbstractPaintLayer* bkg);

    /*!
        A special BaseWindow that is always on top of all other BaseWindow. It is even above the BaseWindow that is selected.
    */
    void SetAlwaysOnFrontWindow(BaseWindow* window);

    //! Set the rendering surface for the current rendering.
    /*!
        This function is used to restore the rendering surface according to the system state. This is necessary after using a custom frame buffer object.
    */
    void RestoreRenderingSurface();

    //! Get the backup texture data of this BaseWindow,
    void* GetBackupTextureData(BaseWindow* base_window, int& width, int& height, int& format);

    //! Reset the DND focus area
    /*!
        Set the DND focus area to NULL.
        \sa _dnd_area;
    */
    void ResetDnDArea();

    // SetDnDArea is declared as private.
    //void SetDnDArea(InputArea* area);
    InputArea* GetDnDArea();

    //! Get the top view that is being processed(event or rendering).
    /*!
        Get the active ViewWindow during and event processing or rendering.
    */
    BaseWindow* GetProcessingTopView()
    {
      return m_CurrentWindow.GetPointer();
    }

    // Pointer Grab API

    //! Add an area at the top of the pointer grab stack
    /*!
        Add an area at the top of the pointer grab stack. The area at the top of the pointer grab stack has the exclusivity on
        the pointer events.
        And area can be added multiple times to the stack but not successively.

        @param area The area to put at the top of the pointer grab stack.
        @return True if the Area was successfully added at the top of the pointer grab stack.
    */
    bool GrabPointerAdd(InputArea* area);

    //! Remove an area from the pointer grab stack
    /*!
        If the Area was added multiple time to the pointer grab stack then the top most instance
        of the parameter area is removed.

        @param area The area to remove from the top of the pointer grab stack.
        @return True if the Area was successfully removed.
    */
    bool GrabPointerRemove(InputArea* area);

    //! Returns True if the area parameter is inside the pointer grab stack.
    bool IsInPointerGrabStack(InputArea* area);

    //! Returns the area at the top of the pointer grab stack.
    InputArea* GetPointerGrabArea();

    // Keyboard Grab API

    //! Add an area at the top of the keyboard grab stack
    /*!
        Add an area at the top of the keyboard grab stack. The area at the top of the keyboard grab stack has the exclusivity on
        the keyboard events.
        And area can be added multiple times to the stack but not successively.

        @param area The area to put at the top of the keyboard grab stack.
        @return True if the Area was successfully added at the top of the keyboard grab stack.
    */
    bool GrabKeyboardAdd(InputArea* area);

    //! Remove an area from the keyboard grab stack
    /*!
        If the Area was added multiple time to the keyboard grab stack then the top most instance
        of the parameter area is removed.

        @param area The area to remove from the top of the keyboard grab stack.
        @return True if the Area was successfully removed.
    */
    bool GrabKeyboardRemove(InputArea* area);

    //! Returns True if the area parameter is inside the keyboard grab stack.
    bool IsInKeyboardGrabStack(InputArea* area);

    //! Returns the area at the top of the keyboard grab stack.
    InputArea* GetKeyboardGrabArea();

    // We use Rectangle texture to attach to the frame-buffer because some GPU like the Geforce FX 5600 do not
    // have support for ARB_texture_non_power_of_two. However it does support ARB_texture_recatangle.
    struct RenderTargetTextures
    {
      ObjectPtr<IOpenGLBaseTexture> color_rt;
      ObjectPtr<IOpenGLBaseTexture> depth_rt;
    };
    
    //! Return the RenderTargetTextures structure of a BaseWindow.
    /*!
        Return the color and depth texture of a BaseWindow.

        @param window The BaseWindow.
        @return A structure that contains the color and depth texture of a BaseWindow.
    */
    RenderTargetTextures& GetWindowBuffer(BaseWindow* window);

#ifdef NUX_GESTURES_SUPPORT
    InputArea *LocateGestureTarget(const GestureEvent &event);

    void SetGestureBroker(std::unique_ptr<GestureBroker> gesture_broker);
#endif

  private:
    typedef std::list<WeakBaseWindowPtr> WindowList;

    //! Render the interface.
    void Draw(bool SizeConfigurationEvent, bool force_draw);

    void DrawPopup(bool force_draw);
    void DrawMenu(bool force_draw);
    void DrawOverlay(bool force_draw);
    void DrawTooltip(bool force_draw);

    //! Render all top views.
    /*!
        @force_draw True if the entire surface of the backup rendering mush flushed.
        @WindowList The list of top views.
        @draw_modal True if the top view that is modal is to be rendered.
    */
    void RenderTopViews(bool force_draw, WindowList&, bool draw_modal);

    void PresentAnyReadyWindows();

    //! Render the content of a top view.
    void RenderTopViewContent(BaseWindow* window, bool force_draw);

    void RenderMainWindowComposition(bool force_draw);

    /*!
        Render a textured quad the quad has the size of the texture. The texture maybe the main window texture or a BaseWindow texture.

        @param HWTexture            Texture to render.
        @param x                    Coordinates of the top left corner of the quad.
        @param y                    Coordinates of the top left corner of the quad.
        @param RenderToMainTexture  If true, render to the main window texture. If false, render to the default back buffer.
        @param BluredBackground     If true, the texture is blended with the blurred version of the main window texture.
    */
    void PresentBufferToScreen(ObjectPtr<IOpenGLBaseTexture> HWTexture, int x, int y, bool RenderToMainTexture, bool BluredBackground = false, float opacity=1.0f, bool premultiply = false);

    //! Push a floating view just above another floating view.
    /*!
        Note that only the top_floating_view is moving. The overall position of the reference view is not changing. 
        @param top_floating_view The view to place above bottom_floating_view.
        @param top_floating_view The reference view that will be below top_floating_view.
        @param strict If true and top_floating_view is already above bottom_floating_view, then bring top_floating_view lower
        so that it is strictly above bottom_floating_view.
    */
    void PushHigher(BaseWindow* top_floating_view, BaseWindow* bottom_floating_view, bool strict = false);
    //! Push a floating view at the top of the stack.
    void PushToFront(BaseWindow* bottom_floating_view);
    //! Push a floating view at the bottom of the stack.
    void PushToBack(BaseWindow* bottom_floating_view);

    //! Set the top view that is about to be processed(event or rendering).
    /*!
        Before event processing or rendering, this should be called to set the ViewWindow that is about 
        to be processed. This function is used internally by the system.
    */
    void SetProcessingTopView(BaseWindow* window)
    {
      m_CurrentWindow = window;
    }

    private:

    void EnsureAlwaysOnFrontWindow();

    void FormatRenderTargets(int width, int height);

    //void UpdatePostProcessRT();

    /*!
        Floating Area need to be informed when the main window has been resized.

        @param Width    New width of the window.
        @param Height   New height of the window.
    */
    void FloatingAreaConfigureNotify(int Width, int Height);

    void RegisterWindow(BaseWindow*);
    // UnRegister is called via the object destroyed event, hence the Object*.
    void UnRegisterWindow(Object*);

    ObjectPtr<IOpenGLBaseTexture> m_MainColorRT;
    ObjectPtr<IOpenGLBaseTexture> m_MainDepthRT;

    WeakBaseWindowPtr m_CurrentWindow;    //!< BaseWindow where event processing or rendering is happening.
    WeakBaseWindowPtr m_MenuWindow;       //!< The BaseWindow that owns the menu being displayed;

    void SetDnDArea(InputArea* area);

    // DnD support
    InputArea* _dnd_area;   //!< the area where the mouse is located during a DND action.

    //! True while events are being processed inside ProcessEvent().
    bool inside_event_cycle_;

    //! True while inside the rendering cycle.
    bool inside_rendering_cycle_;

    InputArea* OverlayDrawingCommand;
    WeakBaseWindowPtr m_OverlayWindow;            //!< The window that owns the overlay;
    WeakBaseWindowPtr _tooltip_window;            //!< The window that owns the tooltip;
    Geometry    _tooltip_geometry;              //!< The geometry of the entire tooltip It includes the decoration surrounding the text such as round corners.
    Geometry    _tooltip_mainwindow_geometry;   //!< Same as _tooltip_geometry but based on the entire physical window of the application.
    Geometry    _tooltip_text_geometry;         //!< The geometry of the text area of the tooltip.

    bool on_menu_closure_continue_with_event_;
    AbstractPaintLayer* m_Background;

    WindowList _view_window_list;
    WindowList _modal_view_window_list;
    WeakBaseWindowPtr _always_on_front_window;  //!< Floating view that always remains on top.

#if !defined(NUX_MINIMAL)
    std::list<MenuPage* >* _menu_chain;
#endif

    std::map<BaseWindow*, struct RenderTargetTextures> _window_to_texture_map;

    bool m_MenuRemoved;

    // Window Geometry
    int m_Width;
    int m_Height;

    std::string m_TooltipText;
    InputArea* m_TooltipArea;
    int m_TooltipX;
    int m_TooltipY;

    //! The fbo to restore after Nux rendering in embedded mode.
    unsigned int draw_reference_fbo_;
    unsigned int read_reference_fbo_;
    Geometry reference_fbo_geometry_;

    //! True if the platform has support for depth textures.
    bool platform_support_for_depth_texture_;

    //! Pointer grab stack.
    /*!
        The head of the list is the top of the stack.
        \sa GrabPointerAdd, GrabPointerRemove.

    */
    std::list<InputArea*> pointer_grab_stack_;

    //! Keyboard grab stack.
    /*!
        The head of the list is the top of the stack.
        \sa GrabKeyboardAdd, GrabKeyboardRemove.

    */
    std::list<InputArea*> keyboard_grab_stack_;

    //! List of views that will get checked for the mouse_near signal
    /*!
        A list of views that will be checked if
        \near the mouse. Must add views to this 
        \list to be checked.
    */
    std::list<InputAreaProximity*> area_proximities_;

  private:
    WindowThread* window_thread_; //!< The WindowThread to which this object belongs.

#ifdef NUX_GESTURES_SUPPORT
    std::unique_ptr<GestureBroker> gesture_broker_;
#endif

    WindowList* currently_rendering_windows_;
    Geometry* current_global_clip_rect_;

    //! Perform some action before destruction.
    /*!
        Perform some action before destruction. This function should only be 
        called from WindowThread::ThreadDtor(). It will invalidate the area that currently has the keyboard focus.
    */
    void BeforeDestructor();

    WindowCompositor(const WindowCompositor&);
    // Does not make sense for a singleton. This is a self assignment.
    WindowCompositor& operator= (const WindowCompositor&);
    // Declare operator address-of as private
    WindowCompositor* operator & ();

    friend class InputArea;
    friend class WindowThread;
    friend class TimerHandler;
    friend class MenuBar;
    friend class MenuPage;
    friend class BaseWindow;
    friend class HSplitter;
    friend class VSplitter;
    friend class TableCtrl;
    friend class View;
    friend class TestWindowCompositor;
  };

#ifdef NUX_GESTURES_SUPPORT
  class DefaultGestureBroker : public GestureBroker
  {
    public:
      DefaultGestureBroker(WindowCompositor *window_compositor);
    private:
      std::vector<ShPtGestureTarget>
      virtual FindGestureTargets(const nux::GestureEvent &event);

      WindowCompositor *window_compositor_;
  };
#endif
}
#endif // WINDOWCOMPOSITOR_H

