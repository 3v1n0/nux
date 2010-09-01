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

NAMESPACE_BEGIN_GUI

class MenuPage;
class PBuffer;
class WindowThread;
class ActiveInterfaceObject;
class BaseArea;
class BaseObject;
class BaseWindow;
class PaintLayer;

class WindowCompositor
{
public:
    void Enumerate();

    void SetClientSize(int w, int h);
//    void DrawGrid(Graphics *g);
    void Draw(IEvent &event, bool force_draw);
    //void DrawList();

    void DrawPopup(bool force_draw);
    void DrawMenu(bool force_draw);
    void DrawOverlay(bool force_draw);
    void DrawTooltip(bool force_draw);
    void DrawFloatingWindows(bool force_draw, const std::list<smptr(BaseWindow)>& WindowList, bool drawModal, bool UseFBO);

    void RenderMainWindowComposition(bool force_draw, bool UseFBO);
    void RenderWindowComposition(smptr(BaseWindow) window, bool force_draw);


    /*!
        Render a textured quad the quad has the size of the texture. The texture maybe the main window texture or a BaseWindow texture. 

        @param HWTexture            Texture to render.
        @param x                    Coordinates of the top left corner of the quad.
        @param y                    Coordinates of the top left corner of the quad.
        @param RenderToMainTexture  If true, render to the main window texture. If false, render to the default back buffer.
        @param BluredBackground     If true, the texture is blended with the blurred version of the main window texture.
    */
    void PresentBufferToScreen(TRefGL<IOpenGLTexture2D> HWTexture, int x, int y, bool RenderToMainTexture, bool BluredBackground = false);
    void PresentRendering();

    /*!
        Set the composition render target as the texture to draw into.
    */
    void SetCompositionRT();

    /*!
        Draw a Texture into the composition render target.
        @param x    Destination coordinates.
        @param y    Destination coordinates.
    */
    void CopyTextureToCompositionRT(TRefGL<IOpenGLTexture2D> HWTexture, int x, int y);

    void AddToDrawList(smptr(ActiveInterfaceObject) ic);
    void ClearDrawList();


    bool MouseDown(Point pt);

    bool MouseMove(Point pt);
    bool MouseUp(Point pt);
    smptr(BaseWindow) GetSelectedWindow();

    void ProcessEvent(IEvent &ievent);

    void RegisterWindow(smptr(BaseWindow));
    void UnRegisterWindow(smptr(BaseWindow));
    void MoveWindowToFront(smptr(BaseWindow));
    void MoveWindowToBack(smptr(BaseWindow));

    TRefGL<IOpenGLFrameBufferObject>& GetWindowFrameBufferObject(){ return m_FrameBufferObject;}
    TRefGL<IOpenGLFrameBufferObject> m_FrameBufferObject;

    TRefGL< IOpenGLBaseTexture > GetScreenBlurTexture();

    // We use Rectangle texture to attach to the framebuffer because some GPU like the Geforce FX 5600 do not 
    // have support for ARB_texture_non_power_of_two. However it does support ARB_texture_recatangle.
    struct RenderTargetTextures
    {
        TRefGL<IOpenGLTexture2D> color_rt;
        TRefGL<IOpenGLTexture2D> depth_rt;
    };
    TRefGL<IOpenGLTexture2D> m_MainColorRT;
    TRefGL<IOpenGLTexture2D> m_MainDepthRT;
    TRefGL<IOpenGLTexture2D> m_CompositionRT;

    RenderTargetTextures& GetWindowBuffer(smptr(BaseWindow) window);

    void StartModalWindow(smptr(BaseWindow) );
    void StopModalWindow(smptr(BaseWindow) );

    std::vector<Rect> EventRect;
    void PushEventRectangle(Rect rect);
    void PopEventRectangle();
    void EmptyEventRegion();
    Rect getEventRect();

    void AddMenu(MenuPage* menu, smptr(BaseWindow) window, bool OverrideCurrentMenuChain = true);
    void RemoveMenu(MenuPage* menu);
    void CleanMenu();

    void PushModalWindow(smptr(BaseWindow) window);

//    typedef void(*AuxDrawCallback)();
//    AuxDrawCallback m_fAuxDrawCallback;
//
//    void SetAuxDrawCallback(AuxDrawCallback f) { m_fAuxDrawCallback = f; }



    void SetWidgetDrawingOverlay(smptr(BaseArea) ic, smptr(BaseWindow) OverlayWindow);
    smptr(BaseArea) GetWidgetDrawingOverlay();

    void SetTooltip(smptr(BaseArea) TooltipArea, const TCHAR *TooltipText, int x, int y);
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
        m_EventRoot.Set(x, y);
    }

    void SetMouseFocusArea(smptr(BaseArea) area);
    smptr(BaseArea) GetMouseFocusArea();
    void SetMouseOverArea(smptr(BaseArea) area);
    smptr(BaseArea) GetMouseOverArea();
    void SetPreviousMouseOverArea(smptr(BaseArea) area);
    smptr(BaseArea) GetPreviousMouseOverArea();

    const IEvent* GetCurrentEvent() const {return m_CurrentEvent;}
    long ProcessEventOnObject(IEvent &ievent, smptr(BaseObject) object, long TraverseInfo, long ProcessEventInfo);
    
    smptr(BaseWindow) GetCurrentWindow() {return m_CurrentWindow;}
    smptr(BaseWindow) GetFocusAreaWindow() {return m_FocusAreaWindow;}

    void SetBackgroundPaintLayer(AbstractPaintLayer* bkg);

private:
    void SetCurrentWindow(smptr(BaseWindow) window) {m_CurrentWindow = window;}
    void SetFocusAreaWindow(smptr(BaseWindow) window) {m_FocusAreaWindow = window;}
    void SetCurrentEvent(IEvent* event) {m_CurrentEvent = event;}

    void FormatRenderTargets(int width, int height);

    void UpdatePostProcessRT();

    /*!
        Floating Area need to be informed when the main window has been resized.

        @param Width    New width of the window.
        @param Height   New height of the window.
    */
    void FloatingAreaConfigureNotify(int Width, int Height);

    smptr(BaseWindow) m_CurrentWindow;
    smptr(BaseWindow) m_FocusAreaWindow;
    smptr(BaseWindow) m_MenuWindow; // the window that owns the menu being displayed;
    IEvent* m_CurrentEvent; 

    smptr(BaseArea) m_MouseFocusArea;      // the base ares that has the mouse down.
    smptr(BaseArea) m_MouseOverArea;  // the base area that has the mouse directly over itself.
    smptr(BaseArea) m_PreviousMouseOverArea;

    smptr(BaseArea) OverlayDrawingCommand;
    smptr(BaseWindow) m_OverlayWindow; // the window that owns the overlay;
    smptr(BaseWindow) m_TooltipWindow; // the window that owns the tooltip;
    Point m_EventRoot;

    AbstractPaintLayer* m_Background;

    std::list<smptr(BaseWindow)> m_WindowList;
    std::list<smptr(BaseWindow)> m_ModalWindowList;
    std::list<smptr(ActiveInterfaceObject)> *m_DrawList;
    std::list<Rect> *m_EventRectList;

    std::list<MenuPage*> *m_MenuList;

    smptr(BaseWindow) m_SelectedWindow;

    std::map< smptr(BaseWindow), struct RenderTargetTextures > m_WindowToTextureMap;

    smptr(BaseWindow) m_ModalWindow;
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
    smptr(BaseArea) m_TooltipArea;
    int m_TooltipX;
    int m_TooltipY;

    bool m_FullSceneBlurUpdated;
    TRefGL<IOpenGLTexture2D> m_BlurTexture;
    TRefGL<IOpenGLTexture2D> m_FullSceneMip0;
    TRefGL<IOpenGLTexture2D> m_FullSceneMip1;
    TRefGL<IOpenGLTexture2D> m_FullSceneMip2;

    //PBuffer *m_pbuffer;
public:
    WindowCompositor();
    WindowCompositor(const WindowCompositor&);
    // Does not make sense for a singleton. This is a self assignment.
    WindowCompositor& operator=(const WindowCompositor&);
    // Declare operator address-of as private 
    WindowCompositor* operator &();
    ~WindowCompositor();

    friend class GfxServerImpl;
    friend class BaseArea;
    friend class WindowThread;
};

NAMESPACE_END_GUI
#endif // STACKMANAGER_H