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


#ifndef STACKMANAGER_H
#define STACKMANAGER_H

#include "BaseWindow.h"

namespace nux
{

  class MenuPage;
  class PBuffer;
  class WindowThread;
  class View;
  class InputArea;
  class Area;
  class BaseWindow;
  class PaintLayer;

  class WindowCompositor
  {
  public:
    WindowCompositor();
    ~WindowCompositor();


    //! Get the Geometry of the tooltip based on the BaseWindow that initiated it.
    Geometry GetTooltipGeometry() const;

    //! Get the Geometry of the tooltip based on the MainWindow.
    Geometry GetTooltipMainWindowGeometry() const;

    bool MouseDown (Point pt);

    bool MouseMove (Point pt);
    bool MouseUp (Point pt);

    void ProcessEvent (IEvent &ievent);


    IntrusiveSP<IOpenGLFrameBufferObject>& GetWindowFrameBufferObject()
    {
      return m_FrameBufferObject;
    }
    IntrusiveSP<IOpenGLFrameBufferObject> m_FrameBufferObject;

    IntrusiveSP< IOpenGLBaseTexture > GetScreenBlurTexture();

    // We use Rectangle texture to attach to the framebuffer because some GPU like the Geforce FX 5600 do not
    // have support for ARB_texture_non_power_of_two. However it does support ARB_texture_recatangle.
    struct RenderTargetTextures
    {
      IntrusiveSP<IOpenGLBaseTexture> color_rt;
      IntrusiveSP<IOpenGLBaseTexture> depth_rt;
    };
    IntrusiveSP<IOpenGLBaseTexture> m_MainColorRT;
    IntrusiveSP<IOpenGLBaseTexture> m_MainDepthRT;
    IntrusiveSP<IOpenGLBaseTexture> m_CompositionRT;

    RenderTargetTextures &GetWindowBuffer (BaseWindow* window);

    void StartModalWindow (IntrusiveWeakSP<BaseWindow>);
    void StopModalWindow (IntrusiveWeakSP<BaseWindow>);

    void AddMenu(MenuPage* menu, BaseWindow *window, bool OverrideCurrentMenuChain = true);
    void RemoveMenu(MenuPage* menu);
    void CleanMenu();

    void PushModalWindow (IntrusiveWeakSP<BaseWindow> window);

    void SetWidgetDrawingOverlay (InputArea *ic, BaseWindow *OverlayWindow);
    InputArea *GetWidgetDrawingOverlay();

    void SetTooltip (InputArea *TooltipArea, const TCHAR *TooltipText, int x, int y);
    /*!
        Return true if the mouse is still inside the area that initiated the tooltip;

        @param x    The mouse x coordinate on screen.
        @param y    The mouse y coordinate on screen.
        @return     Return true is the mouse is still inside the area.
    */
    bool ValidateMouseInsideTooltipArea (int x, int y);
    /*!
        Return true if there is a valid tooltip active.

        @return     Return true if there is a valid tooltip active.
    */
    bool IsTooltipActive();
    void CancelTooltip();

    void SetAreaEventRoot (int x, int y)
    {
      m_EventRoot.Set (x, y);
    }

    const IEvent *GetCurrentEvent() const
    {
      return m_CurrentEvent;
    }
    long ProcessEventOnObject (IEvent &ievent, Area *object, long TraverseInfo, long ProcessEventInfo);

    void SetBackgroundPaintLayer (AbstractPaintLayer *bkg);

    /*!
        A special BaseWindow that is always on top of all other BaseWindow. It is even above the BaseWindow that is selected.
        \sa m_SelectedWindow, \sa GetSelectedWindow.
    */
    void SetAlwaysOnFrontWindow (BaseWindow *window);

  private:
    //! Render the interface.
    void Draw (bool SizeConfigurationEvent, bool force_draw);

    void DrawPopup (bool force_draw);
    void DrawMenu (bool force_draw);
    void DrawOverlay (bool force_draw);
    void DrawTooltip (bool force_draw);
    void DrawFloatingWindows (bool force_draw, std::list< IntrusiveWeakSP<BaseWindow> >& WindowList, bool drawModal, bool UseFBO);

    void RenderMainWindowComposition (bool force_draw, bool UseFBO);
    void RenderWindowComposition (BaseWindow *window, bool force_draw);


    /*!
        Render a textured quad the quad has the size of the texture. The texture maybe the main window texture or a BaseWindow texture.

        @param HWTexture            Texture to render.
        @param x                    Coordinates of the top left corner of the quad.
        @param y                    Coordinates of the top left corner of the quad.
        @param RenderToMainTexture  If true, render to the main window texture. If false, render to the default back buffer.
        @param BluredBackground     If true, the texture is blended with the blurred version of the main window texture.
    */
    void PresentBufferToScreen (IntrusiveSP<IOpenGLBaseTexture> HWTexture, int x, int y, bool RenderToMainTexture, bool BluredBackground = false);
    void PresentRendering();

    /*!
        Set the main color render target as the texture to draw into.
    */
    void SetMainColorRT();

    /*!
        Draw a Texture into the main color render target.
        @param x    Destination coordinates.
        @param y    Destination coordinates.
    */
    void CopyTextureToMainColorRT (IntrusiveSP<IOpenGLBaseTexture> HWTexture, int x, int y);

    /*!
        Set the composition render target as the texture to draw into.
    */
    void SetCompositionRT();

    /*!
        Draw a Texture into the composition render target.
        @param x    Destination coordinates.
        @param y    Destination coordinates.
    */
    void CopyTextureToCompositionRT (IntrusiveSP<IOpenGLBaseTexture> HWTexture, int x, int y);

    //! Push a floating view just above another floating view.
    /*!
        Note that only the top_floating_view is moving. The overall position of the reference view is not changing. 
        @param top_floating_view The view to place above bottom_floating_view.
        @param top_floating_view The reference view that will be below top_floating_view.
        @param strict If true and top_floating_view is already above bottom_floating_view, then bring top_floating_view lower
        so that it is strictly above bottom_floating_view.
    */
    void PushHigher (BaseWindow *top_floating_view, BaseWindow *bottom_floating_view, bool strict = false);
    //! Push a floating view at the top of the stack.
    void PushToFront (BaseWindow *bottom_floating_view);
    //! Push a floating view at the bottom of the stack.
    void PushToBack (BaseWindow *bottom_floating_view);

    /*!
        Returns the BaseWindow that is at the top of the BaseWindow stack, excluding the BaseWindow that is
        chosen to be always on to.
        \sa m_SelectedWindow. \sa SetAlwaysOnFrontWindow
    */
    BaseWindow *GetSelectedWindow();

    BaseWindow *GetCurrentWindow()
    {
      return m_CurrentWindow.GetPointer ();
    }
    
    BaseWindow *GetFocusAreaWindow()
    {
      return m_FocusAreaWindow.GetPointer ();
    }

    void SetCurrentWindow (BaseWindow* window)
    {
      m_CurrentWindow = window;
    }

    void SetFocusAreaWindow (BaseWindow *window)
    {
      m_FocusAreaWindow = window;
    }
    void SetCurrentEvent (IEvent *event)
    {
      m_CurrentEvent = event;
    }

    void EnsureAlwaysOnFrontWindow ();

    void FormatRenderTargets (int width, int height);

    //void UpdatePostProcessRT();

    /*!
        Floating Area need to be informed when the main window has been resized.

        @param Width    New width of the window.
        @param Height   New height of the window.
    */
    void FloatingAreaConfigureNotify(int Width, int Height);

    void SetMouseFocusArea (InputArea *area);
    InputArea *GetMouseFocusArea();
    void SetMouseOverArea (InputArea *area);
    InputArea *GetMouseOverArea();
    void SetPreviousMouseOverArea (InputArea *area);
    InputArea *GetPreviousMouseOverArea();

    void RegisterWindow (BaseWindow*);
    void UnRegisterWindow (BaseWindow*);

    IntrusiveWeakSP<BaseWindow> m_CurrentWindow;    //!< BaseWindow where event processing or rendering is happening.
    IntrusiveWeakSP<BaseWindow> m_FocusAreaWindow;  //!< The BaseWindow that contains the m_MouseFocusArea.
    IntrusiveWeakSP<BaseWindow> m_MenuWindow;       //!< The BaseWindow that owns the menu being displayed;
    IEvent* m_CurrentEvent; 

    InputArea* m_MouseFocusArea;     //!< The base area that has the mouse focus.
    InputArea* m_MouseOverArea;      //!< The base area that has the mouse directly over itself.
    InputArea* m_PreviousMouseOverArea;

    InputArea* OverlayDrawingCommand;
    IntrusiveWeakSP<BaseWindow> m_OverlayWindow;            //!< The window that owns the overlay;
    IntrusiveWeakSP<BaseWindow> _tooltip_window;            //!< The window that owns the tooltip;
    Geometry    _tooltip_geometry;          //!< The geometry of the entire tooltip It includes the decoration surrounding the text such as round corners.
    Geometry    _tooltip_mainwindow_geometry;   //!< Same as _tooltip_geometry but based on the entire physical window of the application.
    Geometry    _tooltip_text_geometry;     //!< The geometry of the text area of the tooltip.
    Point m_EventRoot;

    bool _inside_event_processing;
    
    AbstractPaintLayer *m_Background;

    std::list< IntrusiveWeakSP<BaseWindow> > m_WindowList;
    std::list< IntrusiveWeakSP<BaseWindow> > m_ModalWindowList;
    IntrusiveWeakSP<BaseWindow>            _always_on_front_window;  //!< Floating view that always remains on top.

    std::list<MenuPage *> *m_MenuList;

    /*!
        The BaseWindow where the last mouse down event happened. 
        This BaseWindow will be raised to the top of the stack. 
        \sa GetSelectedWindow.
    */
    IntrusiveWeakSP<BaseWindow> m_SelectedWindow;

    std::map< BaseWindow*, struct RenderTargetTextures > m_WindowToTextureMap;

    IntrusiveWeakSP<BaseWindow> m_ModalWindow;
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

//     bool m_FullSceneBlurUpdated;
//     IntrusiveSP<IOpenGLBaseTexture> m_BlurTexture;
//     IntrusiveSP<IOpenGLBaseTexture> m_FullSceneMip0;
//     IntrusiveSP<IOpenGLBaseTexture> m_FullSceneMip1;
//     IntrusiveSP<IOpenGLBaseTexture> m_FullSceneMip2;

  private:
    WindowCompositor (const WindowCompositor &);
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
  };

}
#endif // STACKMANAGER_H

