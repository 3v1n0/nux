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


#ifndef INPUTAREA_H
#define INPUTAREA_H

#include "Area.h"
#include "MouseHandler.h"

#if defined(NUX_OS_WINDOWS)
  #include "NuxGraphics/Events.h"
#elif defined(NUX_OS_LINUX)
  #include "NuxGraphics/Events.h"
#endif
#include "NuxGraphics/GraphicsDisplay.h"

namespace nux
{
  class WindowCompositor;
  class InputArea;

  typedef InputArea CoreArea;

  class InputArea : public Area
  {
  public:
    NUX_DECLARE_OBJECT_TYPE (InputArea, Area);
  public:
    InputArea (NUX_FILE_LINE_PROTO);
    virtual ~InputArea();

    //! Simulate a mouse down state on an area
    /*!
        Simulate a mouse down event on an area at the provided relative coordinates. If the provided coordinates
        do not fall inside the geometry of the area, return false.
        @param x Coordinates of the mouse down event relative to the top left corner of the area.
        @param y Coordinates of the mouse down event relative to the top left corner of the area.
        @return True if the simulated action was successful.
    */
    bool ForceStartFocus (int x, int y);
    void ForceStopFocus (int x, int y);

    virtual long OnEvent (Event &ievent, long TraverseInfo, long ProcessEventInfo);

    //! Draw InputArea.
    /*!
        Draw a colored quad using m_AreaColor.
        Override this function to define a custom drawing function.
        If force_draw is true then the system requests that all objects redraw themselves completely.
        \param force_draw.
    */
    virtual void OnDraw (GraphicsEngine &GfxContext, bool force_draw);

    virtual void OverlayDrawing (GraphicsEngine &GfxContext) {}

    bool HasKeyboardFocus();
    void SetKeyboardFocus (bool b);
    int GetMouseX();
    int GetMouseY();
    
    bool HasMouseFocus();
    bool MouseFocusOnOtherArea();
    void CaptureMouseDownAnyWhereElse (bool b);
    bool IsCaptureMouseDownAnyWhereElse() const;

    Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);
    Area* FindKeyFocusArea(unsigned int key_symbol,
                          unsigned long x11_key_code,
                          unsigned long special_keys_state);

  private:
    bool _dnd_enabled_as_source;
    bool _dnd_enabled_as_target;
    //bool m_EnableKeyboardInput;
  public:
//    void EnableKeyEntry(bool b)
//    {
//        m_EnableKeyboardInput = true;
//    }
//
//    bool IsEnableKeyEntry()
//    {
//        return m_EnableKeyboardInput; //m_KeyboardHandler.IsEnableKeyEntry();
//    }

    // Override the virtual methods from Object Base
    // Here, we get a change to update the text of the keyboard handler.
    virtual void SetBaseString (const TCHAR *Caption);

    void SetKeyboardReceiverIgnoreMouseDownOutside(bool ignore_mouse_down_outside);

    bool KeyboardReceiverIgnoreMouseDownOutside();

    virtual bool IsArea() const
    {
      return true;
    }

    // This method reset the mouse position inside the Area. This is necessary for areas that serve as Drag
    // when the area position is referred to (x_root, y_root) instead of being the system window coordinates (0, 0).
    void SetAreaMousePosition (int x, int y);

    void GrabPointer ();
    void GrabKeyboard ();
    void UnGrabPointer ();
    void UnGrabKeyboard ();
    bool OwnsPointerGrab ();
    bool OwnsKeyboardGrab ();

    //! Return true if this Area is the owner of the mouse pointer.
    /*!
        The owner of the mouse pointer is the Area that has received a Mouse down event and the 
        mouse button responsible for the event is still pressed.
        
        @return True if this Area is the owner of the mouse pointer.
    */
    bool IsMouseOwner();

    //! Returns true if the mouse pointer is inside the Area.
    /*!
        Returns true if during a call to FindAreaUnderMouse it the Area has been determined to be directly under the
        mouse pointer. Note that is is true only for the first area that is found. there might be other areas that 
        which have the mouse pointer inside of them.
        
        @return Return true if the mouse pointer is inside the Area.
    */
    bool IsMouseInside();

    //! Enable the double click event signal on this InputArea.
    void EnableDoubleClick(bool double_click);

    //! Returns true if the double click signal is enable for this InputArea.
    bool DoubleClickEnabled() const;

    AreaEventProcessor _event_processor;

#if defined (NUX_OS_LINUX)
    void HandleDndEnter() {ProcessDndEnter ();}
    void HandleDndLeave() {ProcessDndLeave ();}
#endif
  private:

    //! Event processing in exclusive mode.
    /*!
        Bypass OnEvent and performs a simplified event processing mechanism.
    */
    long ProcessEventInExclusiveMode (Event &ievent, long TraverseInfo, long ProcessEventInfo);

    void HandleDndMove  (Event &event);
    void HandleDndDrop  (Event &event);

    //! Color of the InputArea
    /*
        Color of the InputArea use to draw a colored quad when OnDraw() is called.
    */
    Color m_AreaColor;
    
    int _dnd_safety_x;
    int _dnd_safety_y;

  protected:

    bool _has_keyboard_focus;
    bool _capture_mouse_down_any_where_else;
    bool _double_click;     //!< If True, this InputArea can emit the signal mouse_double_click. Default is false.

    bool _keyboard_receiver_ignore_mouse_down_outside;

#if defined (NUX_OS_LINUX)
    // DnD support
    // Rather than being implemented with signals, DND support is implemented with protected virtual function.
    // This ensure that a class and it subclass don't process the same event more than once!

    virtual void ProcessDndMove  (int x, int y, std::list<char *>mimes);
    virtual void ProcessDndDrop  (int x, int y);
    virtual void ProcessDndEnter ();
    virtual void ProcessDndLeave ();

    void SendDndStatus (bool accept, DndAction action, Geometry region);
    void SendDndFinished (bool accepted, DndAction action);
    
    void SetDndEnabled (bool as_source, bool as_target);
    
    virtual bool                    DndSourceDragBegin      ();
    virtual NBitmapData *           DndSourceGetDragImage   ();
    virtual std::list<const char *> DndSourceGetDragTypes   ();
    virtual const char *            DndSourceGetDataForType (const char *type, int *size, int *format);
    virtual void                    DndSourceDragFinished   (DndAction result);
    
    void StartDragAsSource ();
    
    static NBitmapData *           InnerDndSourceGetDragImage (void *data) { return static_cast<InputArea *> (data)->DndSourceGetDragImage ();       }
    static std::list<const char *> InnerDndSourceGetDragTypes (void *data) { return static_cast<InputArea *> (data)->DndSourceGetDragTypes ();       }
    static void                    InnerDndSourceDragFinished (DndAction result, void *data);
    
    static const char * InnerDndSourceGetDataForType (const char *type, int *size, int *format, void *data) 
      { return static_cast<InputArea *> (data)->DndSourceGetDataForType (type, size, format); }
#endif

  public:
    //! Signal emitted when the Mouse moves over the InputArea surface.
    sigc::signal<void, int, int, int, int, unsigned long, unsigned long> mouse_move;  // send the current position inside the area
    
    //! Signal emitted when the InputArea receives a mouse down event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position (1st parameter of the callback).
        @param int Mouse Y position (2nd parameter of the callback).
        @param unsigned long Mouse button states (3rd parameter of the callback).
        @param unsigned long Keyboard special keys states (4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_down;
    
    //! Signal emitted when the InputArea receives a mouse up event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position (1st parameter of the callback).
        @param int Mouse Y position (2nd parameter of the callback).
        @param unsigned long Mouse button states (3rd parameter of the callback).
        @param unsigned long Keyboard special keys states (4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_up;
    
    //! Signal emitted when the InputArea receives a mouse enter event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position (1st parameter of the callback).
        @param int Mouse Y position (2nd parameter of the callback).
        @param unsigned long Mouse button states (3rd parameter of the callback).
        @param unsigned long Keyboard special keys states (4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_enter;
    
    //! Signal emitted when the InputArea receives a mouse leave event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position (1st parameter of the callback).
        @param int Mouse Y position (2nd parameter of the callback).
        @param unsigned long Mouse button states (3rd parameter of the callback).
        @param unsigned long Keyboard special keys states (4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_leave;
    
    //! Signal emitted when the InputArea receives a mouse down followed later by a mouse release over its surface.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position (1st parameter of the callback).
        @param int Mouse Y position (2nd parameter of the callback).
        @param unsigned long Mouse button states (3rd parameter of the callback).
        @param unsigned long Keyboard special keys states (4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_click;
    
    //! Signal emitted when the InputArea receives a double click event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position (1st parameter of the callback).
        @param int Mouse Y position (2nd parameter of the callback).
        @param unsigned long Mouse button states (3rd parameter of the callback).
        @param unsigned long Keyboard special keys states (4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_double_click;
    
    //! Signal emitted when the InputArea receives a mouse down event, followed later by a mouse move event (while the mouse is still pressed).
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position (1st parameter of the callback).
        @param int Mouse Y position (2nd parameter of the callback).
        @param int Mouse X delta (3thr parameter of the callback).
        @param int Mouse Y delta (4th parameter of the callback).
        @param unsigned long Mouse button states (5th parameter of the callback).
        @param unsigned long Keyboard special keys states (6th parameter of the callback).
    */
    sigc::signal<void, int, int, int, int, unsigned long, unsigned long> mouse_drag; // send (current X, current Y, delta X, delta Y)

    //! Signal emitted when the InputArea receives a mouse wheel event.
    sigc::signal < void,
         int,            // window x
         int,            // window y
         int,            // mouse wheel delta: +120/-120 correspond to one notch of the wheel
         unsigned long,  // mouse state
         unsigned long   // key state
         > mouse_wheel; // send (current X, current Y, delta X, delta Y)

    //! Signal emitted when the InputArea receives a key release event.
    sigc::signal<void, unsigned int, unsigned long, unsigned long> key_up;

    //! Signal emitted when the area gets the keyboard focus. The is a result of a mouse down event or a call to ForceStartFocus.
    sigc::signal<void> begin_key_focus;
    //! Signal emitted when the area looses the keyboard focus.
    sigc::signal<void> end_key_focus;

    sigc::signal < void,
         unsigned long       ,   /*event type*/
         unsigned long       ,   /*event keysym*/
         unsigned long       ,   /*event state*/
         const TCHAR*        ,   /*character*/
         unsigned short          /*key repeat count*/
         > key_down;

    //! Signal emitted when a mouse down event occurs outside of this area
    /*!
        If this area is inside the main layout, then the position is relative to the top left corner of the window.
        If this area is a BaseWindow, then the position is relative to the top left corner of the window.
        If this area is inside a BaseWindow, then the position is relative to the top left corner of the BaseWIndow.

        @param int    Position of the mouse down event.
        @param int    Position of the mouse down event.
        @param ulong  Mouse button states.
        @param ulong  Keyboard modifiers states.
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_down_outside_pointer_grab_area;

    sigc::signal<void, InputArea*> start_keyboard_grab;
    sigc::signal<void, InputArea*> end_keyboard_grab;

    protected:

      virtual bool AcceptKeyNavFocus();

      // == Signals with 1 to 1 mapping to input device ==
      virtual void EmitMouseDownSignal        (int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state);
      virtual void EmitMouseUpSignal          (int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state);
      virtual void EmitMouseMoveSignal        (int x, int y, int dx, int dy, unsigned long mouse_button_state, unsigned long special_keys_state);
      virtual void EmitMouseWheelSignal       (int x, int y, int wheel_delta, unsigned long mouse_button_state, unsigned long special_keys_state);
      virtual void EmitKeyDownSignal          (unsigned int key_symbol, unsigned long x11_key_code, unsigned long special_keys_state);
      virtual void EmitKeyUpSignal            (unsigned int key_symbol, unsigned long x11_key_code, unsigned long special_keys_state);
      virtual void EmitKeyEventSignal         (unsigned long event_type,
                                              unsigned int key_symbol,
                                              unsigned long special_keys_state,
                                              const char* text,
                                              int key_repeat_count);

      // == Interpreted signals ==

      // Mouse down + mouse move on an area
      virtual void EmitMouseDragSignal        (int x, int y, int dx, int dy, unsigned long mouse_button_state, unsigned long special_keys_state);
      virtual void EmitMouseEnterSignal       (int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state);
      virtual void EmitMouseLeaveSignal       (int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state);

      virtual void EmitMouseClickSignal       (int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state);
      virtual void EmitMouseDoubleClickSignal (int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state);

      virtual void EmitStartKeyboardFocus();
      virtual void EmitEndKeyboardFocus();

      virtual void EmitMouseDownOutsideArea   (int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state);

      friend class WindowCompositor;
  };

}

#endif // INPUTAREA_H
