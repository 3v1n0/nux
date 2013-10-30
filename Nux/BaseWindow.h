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


#ifndef BASEWINDOW_H
#define BASEWINDOW_H


#include <boost/scoped_ptr.hpp>
#include "ScrollView.h"

#include "NuxGraphics/Events.h"
#if defined(USE_X11)
#  include "NuxGraphics/XInputWindow.h"
#endif

#include "InputArea.h"
#include "PaintLayer.h"

namespace nux
{

  class BaseWindow;
  typedef BaseWindow ViewWindow;

  class HLayout;
  class PopUpWindow;

#ifdef NUX_GESTURES_SUPPORT
    class GestureEvent;
#endif

  //typedef TopView BaseWindow;

  /*!
      A user provided callback to assert the size and position of a floating area.
      The callback receives a proposed size and position for the floating area. Inside the callback,
      the size and position of the floating area can be modified.

      @param int          The width of the window.
      @param int          The height of the window.
      @param Geometry&    The tentative size of the window.
  */
  typedef void(*ConfigureNotifyCallback) (int, int, Geometry &, void *);

  /*!
    A floating area on top of the main window.
    A BaseWindow is referenced by the WindowCompositor who calls Reference on the BaseWindow during registration.
  */
  class BaseWindow: public View
  {
    NUX_DECLARE_OBJECT_TYPE(BaseWindow, View);
  public:
    BaseWindow(const char *WindowName = "", NUX_FILE_LINE_PROTO);
    virtual ~BaseWindow();

    nux::Property<bool> premultiply;

    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    virtual Layout* GetLayout();
    virtual bool SetLayout(Layout *layout);

    //! Push the view one level up the ViewWindow stack.
    /*!
        Push this view one level up the ViewWindow stack. Does it even if the view is hidden.
    */
    void PushHigher(BaseWindow* floating_view);

    //! Push the view one level down the ViewWindow stack.
    /*!
        Push this view one level down the ViewWindow stack. Does it even if the view is hidden.
    */
    void PushLower(BaseWindow* floating_view);

    //! Push the view to the front of the ViewWindow stack.
    /*!
        Push this view to the top of the ViewWindow stack. Does it even if the view is hidden.
        If a valid ViewWindow has been forced at the top of the stack with a call to WindowCompositor::SetAlwaysOnFrontWindow(),
        then this view will be positioned one level below that view.
        \sa SetAlwaysOnFrontWindow();
    */
    void PushToFront();

    //! Push the view to the back of the ViewWindow stack.
    /*!
        Push this view to the back of the ViewWindow stack. Does it even if the view is hidden.
    */
    void PushToBack();

    //! Set the window size to respect the layout container.
    /*!
        Set the window size to be such that the container layout size remains the same after ComputeSizeLayout2 is
        called on the layout. The window elements(title bar, minimize and close buttons) are positioned accordingly.
        The size grip is not responding anymore.
        @param b If b is true, the window size respect the size the layout container.
    */
    virtual void SetWindowSizeMatchLayout(bool b)
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

    virtual void ShowWindow(bool b, bool StartModal = false);
    void StopModal();
    bool IsModal() const;
    bool IsVisible() const;

    virtual void SetGeometry(const Geometry &geo);

    /*!
        Call this function to set a callback function that is called when this object is need to be resized or re-positioned.
        @param Callback Function to be called to set the this object size and position.
        @param Data     The callback data.
    */

    void SetConfigureNotifyCallback(ConfigureNotifyCallback Callback, void *Data);

    void SetBackgroundLayer(AbstractPaintLayer *layer);
    void SetBackgroundColor(const Color &color);

    void  SetOpacity(float opacity);
    float GetOpacity();

    #if defined(NUX_OS_LINUX)
    void EnableInputWindow(bool        b,
                            const char* title = "nux input window",
                            bool        take_focus = False,
                            bool        override_redirect = False);
    bool InputWindowEnabled();

    void InputWindowEnableStruts(bool enable);
    bool InputWindowStrutsEnabled();

    void InputWindowEnableOverlayStruts(bool enable);
    bool InputWindowOverlayStrutsEnabled();

    void SetInputFocus();
    Window GetInputWindowId();
    #endif

    //! Set an InputArea to receive the keyboard focus when the BaseWIndow receives the NUX_WINDOW_ENTER_FOCUS event.
    /*!
        \sa _enter_focus_input_area.
        @param input_area An InputArea pointer object. Must be a child of this BaseWindow.
    */
    void SetEnterFocusInputArea(InputArea *input_area);


    //! Get the backup texture data of this BaseWindow,
    void* GetBackupTextureData(int &width, int &height, int &format);

    //! Emit a signal when the BaseWindow becomes visible.
    sigc::signal<void, BaseWindow*> sigVisible;
    //! Emit a signal when the BaseWindow becomes hidden.
    sigc::signal<void, BaseWindow*> sigHidden;

    std::string GetWindowName()
    {
      return _name;
    }

    void PresentInEmbeddedModeOnThisFrame(bool force = false);
    void MarkPresentedInEmbeddedMode();
    bool AllowPresentationInEmbeddedMode() const;
    nux::Geometry const& LastPresentedGeometryInEmbeddedMode() const;

    void PrepareParentRedirectedView();

#ifdef NUX_GESTURES_SUPPORT
    virtual Area* GetInputAreaHitByGesture(const nux::GestureEvent &event);
#endif

  protected:

    void SetAcceptKeyNavFocus(bool accept);
    bool accept_key_nav_focus_;
    virtual bool AcceptKeyNavFocus();

     //! Callback function to set the window position and size.
    ConfigureNotifyCallback m_configure_notify_callback;
    //! Callback data for ConfigureNotifyCallback.
    void *m_configure_notify_callback_data;

    //sigc::signal< bool, unsigned int, unsigned int, Geometry & > sigRequestConfigure;

    Layout *m_layout;

    friend class ComboBox_Logic_WindowView;

    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement(long LayoutResult);
    virtual void ComputeContentPosition(float offsetX, float offsetY);
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
    virtual void NotifyConfigurationChange(int Width, int Height);

    int GetBorder() const;
    int GetTopBorder() const;
    void SetBorder(int border);
    void SetTopBorder(int border);
    int m_TopBorder;
    int m_Border;
    boost::scoped_ptr<AbstractPaintLayer> _paint_layer;

    bool _entering_visible_state;  //!< the window is about to be made visible during event processing
    bool _entering_hidden_state;   //!< the window is about to be made hidden during event processing

    bool ChildNeedsRedraw();

    #if defined(USE_X11)
    bool m_input_window_enabled;
    XInputWindow *m_input_window;
    #endif

  private:
    //! Contains the background of the texture. Can be used to blur. It is set by the window compositor.
    ObjectPtr<BaseTexture> _background_texture;

    std::string _name;
    bool _size_match_layout;
    bool _is_visible;
    bool _is_modal;

    InputArea *_enter_focus_input_area;  //!< An input Area to set the keyboad focus on in response to NUX_WINDOW_ENTER_FOCUS.

    std::list<View *> m_InterfaceObject;

    bool _child_need_redraw;   //!<True is there is a child of the BaseWindow that needs to be redrawn;
    float _opacity;

    bool _present_in_embedded_mode; //!<True if we should draw this window's
                                    // backing texture to the screen on the
                                    // next frame
    nux::Geometry _last_presented_geometry_in_embedded_mode;

    bool _contents_ready_for_presentation; //!<True if this window's contents has just been
                                           // redrawn and it is awaiting presentation

    friend class PopUpWindow;

    friend class WindowThread;
    friend class WindowCompositor;
  };

}

#endif // BASEWINDOW_H
