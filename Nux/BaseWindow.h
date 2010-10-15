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


#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include "ScrollView.h"

#if defined(NUX_OS_WINDOWS)
#include "NuxGraphics/Gfx_Events.h"
#elif defined(NUX_OS_LINUX)
#include "NuxGraphics/GfxEventsX11.h"
#include "NuxGraphics/XInputWindow.h"
#endif

#include "InputArea.h"
#include "MouseHandler.h"
#include "StaticTextBox.h"
#include "PaintLayer.h"

namespace nux
{

  class HLayout;
  class PopUpWindow;

  /*!
      A user provided callback to assert the size and position of a floating area.
      The callback receives a proposed size and position for the floating area. Inside the callback,
      the size and position of the floating area can be modified.

      @param int          The width of the window.
      @param int          The height of the window.
      @param Geometry&    The tentative size of the window.
  */
  typedef void (*ConfigureNotifyCallback) (int, int, Geometry &, void *);

  class BaseWindow: public View
  {
    NUX_DECLARE_OBJECT_TYPE (BaseWindow, View);
  public:
    BaseWindow (const TCHAR *WindowName = TEXT (""), NUX_FILE_LINE_PROTO);
    ~BaseWindow();


    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsContext &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw);

    void AddWidget (View *ic);
    void AddWidget (View *ic, int stretchfactor);
    void AddWidget (std::list<View *> *ViewList);
    void SetLayout (Layout *layout);

    //! Set the window size to respect the layout container.
    /*!
        Set the window size to be such that the container layout size remains the same after ComputeSizeLayout2 is
        called on the layout. The window elements (title bar, minimize and close buttons) are positioned accordingly.
        The size grip is not responding anymore.
        @param b If b is true, the window size respect the size the layout container.
    */
    virtual void SetWindowSizeMatchLayout (bool b)
    {
      m_bSizeMatchLayout = b;
    }
    //! Check if the window size is constrained to the layout container size.
    /*!
        Check if the window size is constrained to the layout container size.
        @return If the return value is true, the window size is constrained by the size the layout container.
    */
    bool IsSizeMatchContent() const
    {
      return m_bSizeMatchLayout;
    }

    void ShowWindow (bool b, bool StartModal = false);
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

    void SetBlurredBackground (bool b)
    {
      m_blured_background = b;
    }

    bool UseBlurredBackground()
    {
      return m_blured_background;
    }
    
    #if defined(NUX_OS_LINUX)
    void EnableInputWindow (bool b);
    bool InputWindowEnabled ();
    void InputWindowEnableStruts (bool enable);
    bool InputWindowStrutsEnabled ();
    #endif

    std::vector<Geometry> m_dirty_areas;    //!< Geometry of areas inside the BaseWindow that requested a redraw.
    
  protected:
    ConfigureNotifyCallback m_configure_notify_callback;    //!< Callback function to set the window position and size.
    void *m_configure_notify_callback_data;     //!< Callback data for ConfigureNotifyCallback.

    sigc::signal< bool, unsigned int, unsigned int, Geometry & > sigRequestConfigure;

    Layout *m_layout;

    friend class ComboBox_Logic_WindowView;

    virtual void PreLayoutManagement();
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

    void ClearDirtyAreas();

    int GetBorder() const;
    int GetTopBorder() const;
    void SetBorder (int border);
    void SetTopBorder (int border);
    int m_TopBorder;
    int m_Border;
    Color m_background_color;   //!< Background color of the floating area.
    AbstractPaintLayer *m_PaintLayer;

    bool m_blured_background;
  private:
    #if defined(NUX_OS_LINUX)
    bool m_input_window_enabled;
    XInputWindow *m_input_window;
    #endif
    
    bool m_bSizeMatchLayout;
    bool m_bIsVisible;
    bool m_bIsModal;

    std::list<View *> m_InterfaceObject;
    HLayout *m_TitleBarLayout;

    friend class PopUpWindow;
    friend class WindowCompositor;
  };

}

#endif // BASEWINDOW_H
