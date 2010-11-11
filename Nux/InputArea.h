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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
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


#define INPUT
#define OUTPUT

#define FOCUSNONE -1

namespace nux
{

////////////////////////////////////////////////////////////////////////////
// Base Area
////////////////////////////////////////////////////////////////////////////

  class InputArea;
  typedef InputArea CoreArea;

  class InputArea : public Area //public sigc::trackable
  {
  public:
    NUX_DECLARE_OBJECT_TYPE (InputArea, Area);
  public:
    InputArea (NUX_FILE_LINE_PROTO);
    virtual ~InputArea();

    void Deselect()
    {
      m_MouseEventCurrent.MouseIn = false;
    }

    void ForceStartFocus (int x, int y);
    void ForceStopFocus (int x, int y);

    virtual long OnEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    //! Draw CoreArea.
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

    void EnableDoubleClick (bool b);
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

  private:

    //! Color of the CoreArea
    /*
        Color of the CoreArea use to draw a colored quad when OnDraw() is called.
    */
    Color m_AreaColor;

  protected:
    struct MouseEvent m_MouseEventPrevious;
    struct MouseEvent m_MouseEventCurrent;

    BaseMouseHandler m_EventHandler;

    bool m_hasKeyboardFocus;
    bool m_CaptureMouseDownAnyWhereElse;
    bool m_EnableDoubleClick;
    bool m_EnableUserKeyboardProcessing;

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

    sigc::signal < void,
         GraphicsEngine &    ,   /*Graphics Context for text operation*/
         unsigned long       ,   /*event type*/
         unsigned long       ,   /*event keysym*/
         unsigned long       ,   /*event state*/
         TCHAR               ,   /*character*/
         unsigned short          /*key repeat count*/
         > OnKeyEvent;

    sigc::signal<void, int, int, unsigned long, unsigned long> OnMouseDownOutsideArea;
  };

}

#endif // BASEAREA_H
