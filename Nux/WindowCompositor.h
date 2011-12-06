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

#include "BaseWindow.h"

#include <sigc++/trackable.h>
#include <sigc++/connection.h>

#include <NuxCore/ObjectPtr.h>

namespace nux
{
  class MenuPage;
  class PBuffer;
  class WindowThread;
  class View;
  class InputArea;
  class Area;
  class PaintLayer;
  class Event;

  //! A user interface composition class created by WindowThread.
  class WindowCompositor : public sigc::trackable
  {
  public:
    typedef ObjectWeakPtr<BaseWindow> WeakBaseWindowPtr;

    WindowCompositor(WindowThread *window_thread);
    ~WindowCompositor();

    //! Get the Geometry of the tooltip based on the BaseWindow that initiated it.
    Geometry GetTooltipGeometry() const;

    //! Get the Geometry of the tooltip based on the MainWindow.
    Geometry GetTooltipMainWindowGeometry() const;

//     bool MouseDown(Point pt);
// 
//     bool MouseMove(Point pt);
//     bool MouseUp(Point pt);

    void ProcessEvent(Event &event);

    //====================================
    void MouseEventCycle(Event &event);
    void DndEventCycle(Event &event);
    bool _enable_nux_new_event_architecture;


    Point _mouse_position_on_owner;
    Point _mouse_position;

    //! Get Mouse position relative to the top left corner of the window.
    Point GetMousePosition();
    
    void KeyboardEventCycle(Event &event);

    void MenuEventCycle(Event &event);
    MenuPage* _mouse_owner_menu_page;
    MenuPage* _mouse_over_menu_page;
    bool      _starting_menu_event_cycle;
    bool      _menu_is_active;

    void SetKeyFocusArea(InputArea* area);
    InputArea* GetKeyFocusArea();

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

    //! Traverse the widget tree and found the area that is right below the mouse pointer.
    void GetAreaUnderMouse(const Point& mouse_position,
                           NuxEventType event_type,
                           InputArea** area_under_mouse_pointer,
                           BaseWindow** window);

    //! Traverse the widget tree and found the area has the key focus.
    void FindKeyFocusArea(NuxEventType event_type,
                          unsigned int key_symbol,
                          unsigned int special_keys_state,
                          InputArea** key_focus_area,
                          BaseWindow** window);
    
    //! Traverse the widget tree and found the area has the key focus, but start from a specified widget.
    void FindKeyFocusAreaFrom(NuxEventType event_type,
      unsigned int key_symbol,
      unsigned int special_keys_state,
      InputArea* root_search_area,
      InputArea** key_focus_area,
      BaseWindow** window);

    void ResetMousePointerAreas();

    //! Get the area upon which the mouse button is currently down.
    Area* GetMouseOwnerArea();
    //! Set the area upon which the mouse button is currently down.
    void SetMouseOwnerArea(Area* area);

    //! Set the area that is right below the mouse pointer.
    void SetMouseOverArea(Area* area);
    
    //! Set The BaseWindow of the area that is the mouse owner.
    void SetMouseOwnerBaseWindow(BaseWindow* base_window);

    //! Callback: called when mouse_over_area_ is destroyed.
    void OnMouseOverViewDestroyed(Object* area);
    
    //! Callback: called when mouse_owner_area_ is destroyed.
    void OnMouseOwnerViewDestroyed(Object* area);

    //! Callback: called when key_focus_area_ is destroyed.
    void OnKeyNavFocusDestroyed(Object* area);

    //! Callback: called when mouse_owner_basewindow_connection_ is destroyed.
    void OnMouseOwnerBaseWindowDestroyed(Object* area);

    void SendKeyEvent(InputArea* input_area, NuxEventType event_type,
      unsigned int key_sym,
      unsigned long x11_key_code,
      unsigned long special_keys_state,
      const char* text,
      int key_repeat_count);


    //! The InputArea that has the keyboard navigation focus.
    /*!
        The InputArea that has the mouse focus also has the keyboard focus. That is if _mouse_focus_area is not Null
        then _mouse_focus_area is equal to _mouse_focus_area;
    */
    InputArea* key_focus_area_;
    InputArea* mouse_owner_area_;
    InputArea* mouse_over_area_;
    BaseWindow* mouse_owner_base_window_;

    int dnd_safety_x_;
    int dnd_safety_y_;

    sigc::connection mouse_over_view_connection_;
    sigc::connection mouse_owner_view_connection_;
    sigc::connection mouse_owner_basewindow_connection_;
    sigc::connection key_focus_area_connection_;

    //====================================
  
  public:
    ObjectPtr<IOpenGLFrameBufferObject>& GetWindowFrameBufferObject()
    {
      return m_FrameBufferObject;
    }
    ObjectPtr<IOpenGLFrameBufferObject> m_FrameBufferObject;

    ObjectPtr<IOpenGLBaseTexture> GetScreenBlurTexture();

    void StartModalWindow(ObjectWeakPtr<BaseWindow>);
    void StopModalWindow(ObjectWeakPtr<BaseWindow>);

    void AddMenu(MenuPage* menu, BaseWindow *window, bool OverrideCurrentMenuChain = true);
    void RemoveMenu(MenuPage* menu);
    void CleanMenu();

    void PushModalWindow(ObjectWeakPtr<BaseWindow> window);

    void SetWidgetDrawingOverlay(InputArea *ic, BaseWindow *OverlayWindow);
    InputArea *GetWidgetDrawingOverlay();

    void SetTooltip(InputArea *TooltipArea, const char *TooltipText, int x, int y);
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

    void SetAreaEventRoot(int x, int y)
    {
      _event_root = Point(x, y);
    }

    void SetBackgroundPaintLayer(AbstractPaintLayer *bkg);

    /*!
        A special BaseWindow that is always on top of all other BaseWindow. It is even above the BaseWindow that is selected.
        \sa m_SelectedWindow, \sa GetSelectedWindow.
    */
    void SetAlwaysOnFrontWindow(BaseWindow *window);


    //! Enable the exclusive event input mode.
    /*!
        Set the exclusive event input area(\sa _exclusive_input_area). The greedy input area gets all input events(mouse and keyboard).
        The exclusive input mode can only be set if there is no exclusive input area already set.
        To disable the exclusive input move, call DisableExclusiveInputArea with the current exclusive input area as parameter.
        The exclusive event input mode can only change once during the processing of one event. The change it again, 
        you have to wait for the next event cycle.
        \sa DisableExclusiveInputArea.
        @return True, if the exclusive input mode was enabled.
    */
    bool EnableExclusiveInputArea(InputArea *input_area);
    
    //! Disable the exclusive event input mode.
    /*!
        Disable the exclusive event input mode. It can only be disable if the current exclusive input area is passed as parameter.
        \sa EnableExclusiveInputArea.
        @return True, if the exclusive input mode was disabled.
    */
    bool DisableExclusiveInputArea(InputArea *input_area);

    //! Return true if the system is in exclusive input event mode.
    /*!
        @return True if the system is in exclusive input mode.
    */
    bool InExclusiveInputMode();

    //! Set the rendering surface for the current rendering.
    /*!
        This function is used to restore the rendering surface according to the system state. This is necessary after using a custom frame buffer object.
    */
    void RestoreRenderingSurface();

    //! Get the backup texture data of this BaseWindow,
    void* GetBackupTextureData(BaseWindow *base_window, int &width, int &height, int &format);

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

  private:
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
    void RenderTopViews(bool force_draw, std::list< ObjectWeakPtr<BaseWindow> >& WindowList, bool draw_modal);

    //! Render the content of a top view.
    void RenderTopViewContent(BaseWindow *window, bool force_draw);

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
    void PresentRendering();

    /*!
        Set the main color render target as the texture to draw into.
    */
    void SetMainColorRT();

    //! Push a floating view just above another floating view.
    /*!
        Note that only the top_floating_view is moving. The overall position of the reference view is not changing. 
        @param top_floating_view The view to place above bottom_floating_view.
        @param top_floating_view The reference view that will be below top_floating_view.
        @param strict If true and top_floating_view is already above bottom_floating_view, then bring top_floating_view lower
        so that it is strictly above bottom_floating_view.
    */
    void PushHigher(BaseWindow *top_floating_view, BaseWindow *bottom_floating_view, bool strict = false);
    //! Push a floating view at the top of the stack.
    void PushToFront(BaseWindow *bottom_floating_view);
    //! Push a floating view at the bottom of the stack.
    void PushToBack(BaseWindow *bottom_floating_view);

    /*!
        Returns the BaseWindow that is at the top of the BaseWindow stack, excluding the BaseWindow that is
        chosen to be always on to.
        \sa m_SelectedWindow. \sa SetAlwaysOnFrontWindow
    */
    BaseWindow *GetSelectedWindow();

    
    BaseWindow *GetFocusAreaWindow()
    {
      return m_FocusAreaWindow.GetPointer();
    }

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

    void SetFocusAreaWindow(BaseWindow *window)
    {
      m_FocusAreaWindow = window;
    }

    void SetCurrentEvent(Event *event)
    {
      m_CurrentEvent = event;
    }

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

    // We use Rectangle texture to attach to the frame-buffer because some GPU like the Geforce FX 5600 do not
    // have support for ARB_texture_non_power_of_two. However it does support ARB_texture_recatangle.
    struct RenderTargetTextures
    {
      ObjectPtr<IOpenGLBaseTexture> color_rt;
      ObjectPtr<IOpenGLBaseTexture> depth_rt;
    };
    ObjectPtr<IOpenGLBaseTexture> m_MainColorRT;
    ObjectPtr<IOpenGLBaseTexture> m_MainDepthRT;

    //! Return the RenderTargetTextures structure of a ViewWindow.
    RenderTargetTextures &GetWindowBuffer(BaseWindow* window);

    WeakBaseWindowPtr m_CurrentWindow;    //!< BaseWindow where event processing or rendering is happening.
    WeakBaseWindowPtr m_FocusAreaWindow;  //!< The BaseWindow that contains the _mouse_focus_area.
    WeakBaseWindowPtr m_MenuWindow;       //!< The BaseWindow that owns the menu being displayed;
    Event* m_CurrentEvent; 

    InputArea* _mouse_over_area;      //!< The base area that has the mouse directly over itself.
    InputArea* _previous_mouse_over_area;

    void SetDnDArea(InputArea* area);

    // DnD support
    InputArea* _dnd_area;   //!< the area where the mouse is located during a DND action.

    //! Set the exclusive input area according to _pending_exclusive_input_mode_action.
    /*!
        Following the event processing cycle, it is necessary to setup the exclusive input area is _pending_exclusive_input_mode_action is true.
        The exclusive input area status always takes effect after the event processing cycle.
    */
    void ExecPendingExclusiveInputAreaAction();

    //! Get the input area that has the exclusivity on events.
    /*!
        @return The input area that has the exclusivity on all events.
    */
    InputArea *GetExclusiveInputArea();

    /*!
        The exclusive input area gets all events without exception(greedy). The exclusive input area may decide to pass events 
        down to other areas. If it does, the following restrictions apply:
          - The other input area cannot have the mouse focus.
          - They cannot have the keyboard focus.
          - No synthetic events: 
            * mouse click
            * mouse drag
          - Only atomic events:
            * mouse down
            * mouse up
            * mouse move
            * mouse enter/leave
    */
    InputArea* _exclusive_input_area;

    /*!
        \a _exclusive_input_area is true when there is an active greedy input area.
    */
    bool _in_exclusive_input_mode;

    /*!
        The exclusive input mode starts after after events have been processed inside ProcessEvent().
        This flags signals that some action are required to enable/disable the exclusive input mode.
    */
    bool _pending_exclusive_input_mode_action;

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
    Point _event_root;

    bool on_menu_closure_continue_with_event_;
    AbstractPaintLayer *m_Background;

    typedef std::list<WeakBaseWindowPtr> WindowList;
    WindowList _view_window_list;
    WindowList _modal_view_window_list;
    WeakBaseWindowPtr _always_on_front_window;  //!< Floating view that always remains on top.

    std::list<MenuPage* > *_menu_chain;

    /*!
        The BaseWindow where the last mouse down event happened.
        This BaseWindow will be raised to the top of the stack.
        \sa GetSelectedWindow.
    */
    WeakBaseWindowPtr m_SelectedWindow;

    std::map<BaseWindow*, struct RenderTargetTextures > _window_to_texture_map;

    WeakBaseWindowPtr m_ModalWindow;
    Point m_MouseLastPos;
    Point m_MouseCurrentPos;

    bool m_PopupRemoved;
    bool m_MenuRemoved;
    bool m_MouseUp;
    bool m_MouseDown;

    // Window Geometry
    int m_Width;
    int m_Height;

    NString m_TooltipText;
    InputArea *m_TooltipArea;
    int m_TooltipX;
    int m_TooltipY;

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

  private:
    WindowThread *window_thread_; //!< The WindowThread to which this object belongs.

    //! Perform some action before destruction.
    /*!
        Perform some action before destruction. This function should only be 
        called from WindowThread::ThreadDtor(). It will invalidate the area that currently has the keyboard focus.
    */
    void BeforeDestructor();

    WindowCompositor(const WindowCompositor &);
    // Does not make sense for a singleton. This is a self assignment.
    WindowCompositor &operator= (const WindowCompositor &);
    // Declare operator address-of as private
    WindowCompositor *operator &();

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
  };

}
#endif // WINDOWCOMPOSITOR_H

