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

#if defined(NUX_OS_WINDOWS)
  #include "NuxGraphics/Events.h"
#elif defined(NUX_OS_LINUX)
  #include "NuxGraphics/Events.h"
#endif
#include "NuxGraphics/GraphicsDisplay.h"

#ifdef NUX_GESTURES_SUPPORT
#include <list>
#include <memory>
#include "Nux/GesturesSubscription.h"
#endif

namespace nux
{
#ifdef NUX_GESTURES_SUPPORT
  class GestureEvent;
#endif
  class WindowCompositor;
  class InputArea;

  typedef InputArea CoreArea;

  class InputArea : public Area
  {
  public:
    NUX_DECLARE_OBJECT_TYPE(InputArea, Area);
  public:
    InputArea(NUX_FILE_LINE_PROTO);
    virtual ~InputArea();

    //! Draw InputArea.
    /*!
        Draw a colored quad using m_AreaColor.
        Override this function to define a custom drawing function.
        If force_draw is true then the system requests that all objects redraw themselves completely.
        \param force_draw.
    */
    virtual void OnDraw(GraphicsEngine &graphics_engine, bool force_draw);

    virtual void OverlayDrawing(GraphicsEngine & /* graphics_engine */) {}

    bool HasKeyboardFocus();

    bool MouseFocusOnOtherArea();
    void CaptureMouseDownAnyWhereElse(bool b);
    bool IsCaptureMouseDownAnyWhereElse() const;

    Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);
    Area* FindKeyFocusArea(unsigned int key_symbol,
                          unsigned long x11_key_code,
                          unsigned long special_keys_state);

    virtual bool AcceptKeyNavFocus() const;

    /*!
      Sets whether this InputArea wants to know about all mouse events
      sent to a child InputArea.

      ChildMouseEvent() will be called for every mouse event that a child
      InputArea receives.

      \param enable Whether this InputArea should be informed about child mouse
                    events
      \sa ChildMouseEvent(), IsTrackingChildMouseEvents()
     */
    void SetTrackChildMouseEvents(bool enable);

    /*!
      Returns whether this InputArea wants to be informed about child mouse events.

      This property is false by default.

      \sa SetTrackChildMouseEvents(), ChildMouseEvent()
     */
    bool IsTrackingChildMouseEvents() const;

    /*!
      Called when a mouse event is sent to a child InputArea.

      If you return true, mouse ownership will be moved to this InputArea and the
      child InputArea will receive a MOUSE_CANCEL event.

      If you return false, nothing happens and the child will keep its ownership
      over the mouse and therefore get further mouse events.

      The default implementation just returns false;

      \return Whether you want to take ownership over the mouse.
      \sa SetTrackChildMouseEvents
     */
    virtual bool ChildMouseEvent(const Event& event);

  protected:

  private:
    bool _dnd_enabled_as_source;
    bool _dnd_enabled_as_target;
  public:
    // Override the virtual methods from Object Base
    // Here, we get a change to update the text of the keyboard handler.
    void SetKeyboardReceiverIgnoreMouseDownOutside(bool ignore_mouse_down_outside);

    void SetAcceptKeyNavFocusOnMouseDown(bool accept);
    void SetAcceptKeyNavFocusOnMouseEnter(bool accept);

    bool KeyboardReceiverIgnoreMouseDownOutside();

    virtual bool IsArea() const
    {
      return true;
    }

    void GrabPointer();
    void GrabKeyboard();
    void UnGrabPointer();
    void UnGrabKeyboard();
    bool OwnsPointerGrab();
    bool OwnsKeyboardGrab();

    //! Return true if this Area is the owner of the mouse pointer.
    /*!
        The owner of the mouse pointer is the Area that has received a Mouse down event and the 
        mouse button responsible for the event is still pressed.
        
        @return True if this Area is the owner of the mouse pointer.
    */
    bool IsMouseOwner();

    //! Returns true if the mouse pointer has been determined as inside the this area, following event processing.
    /*!
        Returns true if during a call to FindAreaUnderMouse, this area has been determined to be directly under the
        mouse pointer. Note that this is true only for the first area that is found. There might be other areas that 
        which have the mouse pointer inside of them. \n
        Call Area::IsMousePointerInside to find out if the mouse pointer is inside an area.
        
        @return Return true if the mouse pointer is inside the Area.
    */
    bool IsMouseInside();

    //! Enable the double click event signal on this InputArea.
    void EnableDoubleClick(bool double_click);

    //! Returns true if the double click signal is enable for this InputArea.
    bool DoubleClickEnabled() const;

    bool mouse_in_;

#if defined(NUX_OS_LINUX)
    void HandleDndEnter() {ProcessDndEnter();}
    void HandleDndLeave() {ProcessDndLeave();}
#endif
  private:

    //! Event processing in exclusive mode.
    /*!
        Bypass OnEvent and performs a simplified event processing mechanism.
    */
    long ProcessEventInExclusiveMode(Event &event, long TraverseInfo, long ProcessEventInfo);

    void HandleDndMove  (Event &event);
    void HandleDndDrop  (Event &event);

    //! Color of the InputArea
    /*
        Color of the InputArea use to draw a colored quad when OnDraw() is called.
    */
    Color area_color_;
    
    int _dnd_safety_x;
    int _dnd_safety_y;

  protected:
    bool _capture_mouse_down_any_where_else;
    bool _double_click;     //!< If True, this InputArea can emit the signal mouse_double_click. Default is false.

    bool _keyboard_receiver_ignore_mouse_down_outside;

    bool accept_key_nav_focus_on_mouse_down_;
    bool accept_key_nav_focus_on_mouse_enter_;


#if defined(NUX_OS_LINUX)
    // DnD support
    // Rather than being implemented with signals, DND support is implemented with protected virtual function.
    // This ensure that a class and it subclass don't process the same event more than once!

    virtual void ProcessDndMove  (int x, int y, std::list<char *>mimes);
    virtual void ProcessDndDrop  (int x, int y);
    virtual void ProcessDndEnter();
    virtual void ProcessDndLeave();

    void SendDndStatus(bool accept, DndAction action, Geometry region);
    void SendDndFinished(bool accepted, DndAction action);
    
    void SetDndEnabled(bool as_source, bool as_target);
    
    virtual bool                    DndSourceDragBegin      ();
    virtual NBitmapData *           DndSourceGetDragImage   ();
    virtual std::list<const char *> DndSourceGetDragTypes   ();
    virtual const char *            DndSourceGetDataForType(const char *type, int *size, int *format);
    virtual void                    DndSourceDragFinished   (DndAction result);
    
    void StartDragAsSource();
    
    static NBitmapData *           InnerDndSourceGetDragImage(void *data) { return static_cast<InputArea *> (data)->DndSourceGetDragImage();       }
    static std::list<const char *> InnerDndSourceGetDragTypes(void *data) { return static_cast<InputArea *> (data)->DndSourceGetDragTypes();       }
    static void                    InnerDndSourceDragFinished(DndAction result, void *data);
    
    static const char * InnerDndSourceGetDataForType(const char *type, int *size, int *format, void *data) 
      { return static_cast<InputArea *> (data)->DndSourceGetDataForType(type, size, format); }
#endif

  public:

#ifdef NUX_GESTURES_SUPPORT
    //! Creates a new gesture subscription for this input area and activates it
    /*
      Convenience function.
      It's the same as doing the following:
        ShGesturesSubscription sub(new GesturesSubscription);
        sub->SetGestureClasses(gesture_classes);
        sub->SetNumTouches(num_touches);
        sub->Activate();
        input_area->AddGesturesSubscription(sub);
     */
    void CreateGesturesSubscription(int gesture_classes,
                                    unsigned int num_touches);

    //! Adds a gestures subscription to this input area.
    /*!
      A single subscription can be shared among multiple InputAreas.
     */
    void AddGesturesSubscription(ShGesturesSubscription &subscription);

    //! Removes a gestures subscription from this input area.
    void RemoveGesturesSubscription(ShGesturesSubscription &subscription);

    //! Returns all multitouch gestures subscriptions that this area cares about.
    /*!
      An input area will receive GestureEvents for multitouch gestures that
      hit his area and matches any active subscription present in this list.
     */
    const std::list<ShGesturesSubscription> &GetGesturesSubscriptions() const;

    //! Returns whether this area has a subscription that matches the gesture event
    bool HasSubscriptionForGesture(const GestureEvent &event) const;

    //! Returns the InputArea hit by the given gesture
    /*!
      If this area or any of its children is hit by the gesture from the given
      event, that area will be returned. Otherwise, it returns nullptr.
     */
    virtual Area* GetInputAreaHitByGesture(const GestureEvent &event);

  private:
    std::list<ShGesturesSubscription> gestures_subscriptions_;
#endif // NUX_GESTURES_SUPPORT

  public:
    //! Signal emitted when the Mouse moves over the InputArea surface.
    sigc::signal<void, int, int, int, int, unsigned long, unsigned long> mouse_move;  // send the current position inside the area
    
    //! Signal emitted when the InputArea receives a mouse down event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position(1st parameter of the callback).
        @param int Mouse Y position(2nd parameter of the callback).
        @param unsigned long Mouse button states(3rd parameter of the callback).
        @param unsigned long Keyboard special keys states(4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_down;
    
    //! Signal emitted when the InputArea receives a mouse up event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position(1st parameter of the callback).
        @param int Mouse Y position(2nd parameter of the callback).
        @param unsigned long Mouse button states(3rd parameter of the callback).
        @param unsigned long Keyboard special keys states(4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_up;
    
    //! Signal emitted when the InputArea receives a mouse enter event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position(1st parameter of the callback).
        @param int Mouse Y position(2nd parameter of the callback).
        @param unsigned long Mouse button states(3rd parameter of the callback).
        @param unsigned long Keyboard special keys states(4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_enter;
    
    //! Signal emitted when the InputArea receives a mouse leave event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position(1st parameter of the callback).
        @param int Mouse Y position(2nd parameter of the callback).
        @param unsigned long Mouse button states(3rd parameter of the callback).
        @param unsigned long Keyboard special keys states(4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_leave;
    
    //! Signal emitted when the InputArea receives a mouse down followed later by a mouse release over its surface.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position(1st parameter of the callback).
        @param int Mouse Y position(2nd parameter of the callback).
        @param unsigned long Mouse button states(3rd parameter of the callback).
        @param unsigned long Keyboard special keys states(4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_click;
    
    //! Signal emitted when the InputArea receives a double click event.
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position(1st parameter of the callback).
        @param int Mouse Y position(2nd parameter of the callback).
        @param unsigned long Mouse button states(3rd parameter of the callback).
        @param unsigned long Keyboard special keys states(4th parameter of the callback).
    */
    sigc::signal<void, int, int, unsigned long, unsigned long> mouse_double_click;

    //! Signal emitted when the InputArea receives a mouse down event, followed later by a mouse move event(while the mouse is still pressed).
    /*!
        @param void Return type of the callback hooked to this signal.
        @param int Mouse X position(1st parameter of the callback).
        @param int Mouse Y position(2nd parameter of the callback).
        @param int Mouse X delta(3thr parameter of the callback).
        @param int Mouse Y delta(4th parameter of the callback).
        @param unsigned long Mouse button states(5th parameter of the callback).
        @param unsigned long Keyboard special keys states(6th parameter of the callback).
    */
    sigc::signal<void, int, int, int, int, unsigned long, unsigned long> mouse_drag; // send(current X, current Y, delta X, delta Y)

    //! Signal emitted when the InputArea receives a mouse wheel event.
    sigc::signal < void,
         int,            // window x
         int,            // window y
         int,            // mouse wheel delta: +120/-120 correspond to one notch of the wheel
         unsigned long,  // mouse state
         unsigned long   // key state
         > mouse_wheel; // send(current X, current Y, delta X, delta Y)

    //! Signal emitted when the InputArea loses ownership over a pressed mouse.
    /*!
      Any actions or changes caused by the previous mouse_down should be reverted.
     */
    sigc::signal<void> mouse_cancel;

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
         const char*        ,   /*character*/
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

#ifdef NUX_GESTURES_SUPPORT
    //! Called whenever a GestureEvent is received
    /*!
      You should reimplement this method to handle gestures. Typically you would
      add code to make this area react to the gestures received.

      In order to receive events from a given kind of gesture, an area has to
      have a corresponding gesture subscription. See CreateGesturesSubscription()
      and AddGesturesSubscription().

      This method is called only with gestures that have already
      been accepted. Therefore you shouldn't call GestureEvent::Accept()
      or GestureEvent::Reject() from here as there's no point in doing so.

      Default implementation just returns GestureDeliveryRequest::NONE.
     */
    virtual GestureDeliveryRequest GestureEvent(const GestureEvent & /* event */)
    {
      return GestureDeliveryRequest::NONE;
    }
#endif

    protected:

      virtual bool AcceptKeyNavFocus();
      virtual bool AcceptKeyNavFocusOnMouseDown() const;
      virtual bool AcceptKeyNavFocusOnMouseEnter() const;

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
      virtual void EmitMouseDoubleClickSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state);

      virtual void EmitMouseDownOutsideArea   (int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state);

      virtual void EmitMouseCancelSignal();

      friend class WindowCompositor;

    private:
      bool is_tracking_child_mouse_events_;
  };

}

#endif // INPUTAREA_H
