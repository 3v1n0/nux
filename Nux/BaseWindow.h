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


#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include "ScrollView.h"

#if defined(NUX_OS_WINDOWS)
#include "NuxGraphics/Events.h"
#elif defined(NUX_OS_LINUX)
#include "NuxGraphics/Events.h"
#include "NuxGraphics/XInputWindow.h"
#endif

#include "InputArea.h"
#include "MouseHandler.h"
#include "StaticTextBox.h"
#include "PaintLayer.h"

namespace nux
{

  class BaseWindow;
  typedef BaseWindow ViewWindow;

  class HLayout;
  class PopUpWindow;

  //typedef TopView BaseWindow;

  /*!
      A user provided callback to assert the size and position of a floating area.
      The callback receives a proposed size and position for the floating area. Inside the callback,
      the size and position of the floating area can be modified.

      @param int          The width of the window.
      @param int          The height of the window.
      @param Geometry&    The tentative size of the window.
  */
  typedef void (*ConfigureNotifyCallback) (int, int, Geometry &, void *);

  /*!
    A floating area on top of the main window.
    A BaseWindow is referenced by the WindowCompositor who calls Reference on the BaseWindow during registration.
  */
  class BaseWindow: public View
  {
    NUX_DECLARE_OBJECT_TYPE (BaseWindow, View);
  public:
    BaseWindow (const TCHAR *WindowName = TEXT (""), NUX_FILE_LINE_PROTO);
    virtual ~BaseWindow();


    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void AddWidget (View *ic);
    void AddWidget (View *ic, int stretchfactor);
    void AddWidget (std::list<View *> *ViewList);
    
    virtual Layout* GetLayout();
    virtual bool SetLayout(Layout *layout);

    void PushHigher (BaseWindow* floating_view);
    void PushLower (BaseWindow* floating_view);
    void PushToFront ();
    void PushToBack ();

    //! Set the window size to respect the layout container.
    /*!
        Set the window size to be such that the container layout size remains the same after ComputeSizeLayout2 is
        called on the layout. The window elements (title bar, minimize and close buttons) are positioned accordingly.
        The size grip is not responding anymore.
        @param b If b is true, the window size respect the size the layout container.
    */
    virtual void SetWindowSizeMatchLayout (bool b)
    {
      _size_match_layout = b;
    }
    //! Check if the window size is constrained to the layout container size.
    /*!
        Check if the window size is constrained to the layout container size.
        @return If the return value is true, the window size is constrained by the size the layout container.
    */
    bool IsSizeMatchContent() const
    {
      return _size_match_layout;
    }

    virtual void ShowWindow (bool b, bool StartModal = false);
    void StopModal();
    bool IsModal() const;
    bool IsVisible() const;

    virtual void SetGeometry (const Geometry &geo);

    /*!
        Call this function to set a callback function that is called when this object is need to be resized or re-positioned.
        @param Callback Function to be called to set the this object size and position.
        @param Data     The callback data.
    */

    void SetConfigureNotifyCallback (ConfigureNotifyCallback Callback, void *Data);

    void SetBackgroundLayer (AbstractPaintLayer *layer);
    void SetBackgroundColor (const Color &color);

    #if defined(NUX_OS_LINUX)
    void EnableInputWindow (bool b, bool override_redirect = 0);
    bool InputWindowEnabled ();
    void InputWindowEnableStruts (bool enable);
    bool InputWindowStrutsEnabled ();
    void GrabPointer ();
    void UnGrabPointer ();
    void GrabKeyboard ();
    void UnGrabKeyboard ();
    Window GetInputWindowId ();
    #endif

    //! Get the backup texture data of this BaseWindow,
    void* GetBackupTextureData (int &width, int &height, int &format);

    //! Emit a signal when the BaseWindow becomes visible.
    sigc::signal<void, BaseWindow*> sigVisible;
    //! Emit a signal when the BaseWindow becomes hidden.
    sigc::signal<void, BaseWindow*> sigHidden;
    
  protected:
    
     //! Callback function to set the window position and size.
    ConfigureNotifyCallback m_configure_notify_callback;
    //! Callback data for ConfigureNotifyCallback.
    void *m_configure_notify_callback_data;

    //sigc::signal< bool, unsigned int, unsigned int, Geometry & > sigRequestConfigure;

    Layout *m_layout;

    friend class ComboBox_Logic_WindowView;

    virtual void PreLayoutManagement ();
    virtual long PostLayoutManagement (long LayoutResult);
    virtual void PositionChildLayout (float offsetX, float offsetY);
    //! Layout the window elements.
    /*!
        Layout elements such as button on the title bar, and the resize widget according to the current
        geometry of the window. Also initiate the computation of the child layout if there is one.
    */
    virtual void LayoutWindowElements();

    /*!
        Floating Area need to be informed when the main window has been resized.

        @param Width    New width of the window.
        @param Height   New height of the window.
    */
    virtual void NotifyConfigurationChange (int Width, int Height);

    //! Process special events.
    /*!
        Some events are not meant to be processed by all views. These events only make sense 
        for ViewWindow. NUX_WINDOW_ENTER_FOCUS, NUX_WINDOW_EXIT_FOCUS are such events.

        @param event The event to inspect.
        @return True if this BaseWindow has claimed the event. If so, then other BaseWindow should not be given
        a chance to process the event.
    */
    virtual bool ProcessSpecialEvent(Event event);

    int GetBorder() const;
    int GetTopBorder() const;
    void SetBorder (int border);
    void SetTopBorder (int border);
    int m_TopBorder;
    int m_Border;
    AbstractPaintLayer *_paint_layer;

    bool _entering_visible_state;  //!< the window is about to be made visible during event processing
    bool _entering_hidden_state;   //!< the window is about to be made hidden during event processing
    
    bool ChildNeedsRedraw ();

    #if defined(NUX_OS_LINUX)
    bool m_input_window_enabled;
    XInputWindow *m_input_window;
    #endif
    
  private:
    //! Contains the background of the texture. Can be used to blur. It is set by the window compositor.
    ObjectPtr<BaseTexture> _background_texture;
   
    bool _size_match_layout;
    bool _is_visible;
    bool _is_modal;

    std::list<View *> m_InterfaceObject;

    bool _child_need_redraw;   //!<True is there is a child of the BaseWindow that needs to be redrawn;
    friend class PopUpWindow;

    friend class WindowThread;
    friend class WindowCompositor;
  };

}

#endif // BASEWINDOW_H
