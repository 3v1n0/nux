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


#ifndef BASEAREA_H
#define BASEAREA_H

#include "Area.h"
#include "MouseHandler.h"

#if defined(NUX_OS_WINDOWS)
#include "NuxGraphics/Events.h"
#elif defined(NUX_OS_LINUX)
#include "NuxGraphics/Events.h"
#endif

#include "NuxGraphics/GraphicsDisplay.h"


#define FOCUSNONE -1

namespace nux
{

////////////////////////////////////////////////////////////////////////////
// Base Area
////////////////////////////////////////////////////////////////////////////

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
    bool IsMouseInside();
    bool HasMouseFocus();
    bool MouseFocusOnOtherArea();
    void CaptureMouseDownAnyWhereElse (bool b);
    bool IsCaptureMouseDownAnyWhereElse() const;

    /*!
        In debug mode, print the processing steps of events for this widget.
        On linux, this works only if nux has been compiled with --enable-debug
        \sa _print_event_debug_trace
        
        @param enable If True, print the event processing deug info to the console.
    */
    void EnableEventDebugTrace (bool enable);

    /*!
        Return True if event processing debuigh information as been activated for this widget.
        \sa _print_event_debug_trace
        
        @return True, if event processing debug information for this widget has been activated.
    */    
    bool GetEventDebugTrace () const;
    
  private:
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

    //! Enable the double click signal on this InputArea.
    void EnableDoubleClick (bool b);

    //! Return True if the double click signal is enable for this InputArea.
    bool IsDoubleClickEnabled();
    void EnableUserKeyboardProcessing (bool b);
    bool IsUserKeyboardProcessingEnabled();

    virtual bool IsArea() const
    {
      return true;
    }

    unsigned short getKeyState (int nVirtKey);

    // This method reset the mouse position inside the Area. This is necessary for areas that serve as Drag
    // when the area position is reffered to (x_root, y_root) instead of being the system window coordinates (0, 0).
    void SetAreaMousePosition (int x, int y);

#if defined (NUX_OS_LINUX)
    void HandleDndEnter () { ProcessDndEnter (); }
    void HandleDndLeave () { ProcessDndLeave (); }
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

  protected:
    AreaEventProcessor _event_processor;

    bool _has_keyboard_focus;
    bool _capture_mouse_down_any_where_else;
    bool _double_click;     //!< If True, this InputArea can emit the signal OnMouseDoubleClick. Default is false.
    bool _print_event_debug_trace;

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
#endif

  public:
    sigc::signal<void, int, int, int, int, unsigned long, unsigned long> OnMouseMove;  // send the current position inside the area
    sigc::signal<void, int, int, unsigned long, unsigned long> OnMouseDown;
    sigc::signal<void, int, int, unsigned long, unsigned long> OnMouseUp;
    sigc::signal<void, int, int, unsigned long, unsigned long> OnMouseEnter;
    sigc::signal<void, int, int, unsigned long, unsigned long> OnMouseLeave;
    sigc::signal<void, int, int, unsigned long, unsigned long> OnMouseClick;
    sigc::signal<void, int, int, unsigned long, unsigned long> OnMouseDoubleClick;
    sigc::signal<void, int, int, int, int, unsigned long, unsigned long> OnMouseDrag; // send (current X, current Y, delta X, delta Y)

    sigc::signal < void,
         int,            // window x
         int,            // window y
         int,            // mouse wheel delta: +120/-120 correspond to one notch of the wheel
         unsigned long,  // mouse state
         unsigned long   // key state
         > OnMouseWheel; // send (current X, current Y, delta X, delta Y)

    sigc::signal<void, unsigned int, unsigned long, unsigned long> OnKeyPressed;
    sigc::signal<void, unsigned int, unsigned long, unsigned long> OnKeyReleased;
    sigc::signal<void> OnValidateKeyboardEntry;

    //! Signal emitted when the area gets the mouse focus. This is the result of a mouse down event.
    sigc::signal<void> OnStartMouseFocus;
    //! Signal emitted when the mouse is released. The area that had the mouse focus receives this signal.
    //! Note that the area may still have the keyboard focus after OnEndMouseFocus is emitted.
    sigc::signal<void> OnEndMouseFocus;

    //! Signal emitted when the area gets the keyboard focus. The is a result of a mouse down event or a call to ForceStartFocus.
    sigc::signal<void> OnStartFocus;
    //! Signal emitted when the area looses the keyboard focus.
    sigc::signal<void> OnEndFocus;

    //! Signal emitted when the area receives an WM_TAKE_FOCUS ClientMessage
    sigc::signal<void, Time> OnTakeFocus;

    sigc::signal < void,
         GraphicsEngine &    ,   /*Graphics Context for text operation*/
         unsigned long       ,   /*event type*/
         unsigned long       ,   /*event keysym*/
         unsigned long       ,   /*event state*/
         const TCHAR*        ,   /*character*/
         unsigned short          /*key repeat count*/
         > OnKeyEvent;

    sigc::signal<void, int, int, unsigned long, unsigned long> OnMouseDownOutsideArea;
  };

}

#endif // BASEAREA_H
