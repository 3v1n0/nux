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


#include "Nux.h"

#if defined(INL_OS_WINDOWS)
    #include "NuxGraphics/Gfx_OpenGL.h"
#elif defined(INL_OS_LINUX)
    #include "NuxGraphics/GfxSetupX11.h"
#endif

#include "NuxGraphics/OpenGLEngine.h"
#include "TimerProc.h"
#include "ClientArea.h"

NAMESPACE_BEGIN_GUI

ClientArea::ClientArea()
:   m_IsRealTime(false)
{
    m_IsClientAreaEnabled = false;
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, 4*PRACTICAL_WIDGET_HEIGHT);

    OnMouseDown.connect(sigc::mem_fun(this, &ClientArea::RecvMouseDown));
    OnMouseUp.connect(sigc::mem_fun(this, &ClientArea::RecvMouseUp));
    OnMouseDrag.connect(sigc::mem_fun(this, &ClientArea::RecvMouseDrag));
    OnMouseMove.connect(sigc::mem_fun(this, &ClientArea::RecvMouseMove));
    OnKeyEvent.connect(sigc::mem_fun(this, &ClientArea::RecvKeyEvent));

    m_RealTimeCallback = new TimerFunctor;
    m_RealTimeCallback->OnTimerExpired.connect(sigc::mem_fun(this, &ClientArea::RealTimeHandler));

    if(GetGraphicsThread()->GetWindow().HasFrameBufferSupport())
    {
        m_FrameBufferObject = GetThreadGLDeviceFactory()->CreateFrameBufferObject();
        m_MainColorRT = GetThreadGLDeviceFactory()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8);
        m_MainDepthRT = GetThreadGLDeviceFactory()->CreateTexture(2, 2, 1, BITFMT_D24S8);
    }
}

ClientArea::~ClientArea()
{
    INL_SAFE_DELETE(m_RealTimeCallback);
}

long ClientArea::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

//     // A is obtained from GfxContext. So A dimension's are in relative window coordinates.
//     Rect A = GetThreadGraphicsContext()->GetClippingRegion();
//     Rect B = Rect(m_Geometry.x, m_Geometry.y, m_Geometry.GetWidth(), m_Geometry.GetHeight());
//     Rect C = A.intersect(B);
//     if((ievent.e_event == INL_MOUSE_MOVE) && !HasMouseFocus())
//     {
//         if(!C.IsPointInside(ievent.e_x, ievent.e_y))
//             return ret;
//     }
    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
}

void ClientArea::BeginDraw(GraphicsContext& GfxContext, bool force_draw)
{
    if((IsRedrawNeeded() == false) && (force_draw == false))
        return;

    if(GetGraphicsThread()->GetWindow().HasFrameBufferSupport())
    {
        int buffer_width = m_Geometry.GetWidth();
        int buffer_height = m_Geometry.GetHeight();
        int window_width, window_height;
        window_width = GfxContext.GetContextWidth();
        window_height =GfxContext.GetContextHeight();

        m_ctx.x = m_Geometry.x;
        m_ctx.y = m_Geometry.y;
        m_ctx.width  = m_Geometry.GetWidth();
        m_ctx.height = m_Geometry.GetHeight();

        // A is obtained from GfxContext. So A dimension's are in relative window coordinates.
        Rect A = GfxContext.GetClippingRegion();
        Rect B = Rect(m_ctx.x, m_ctx.y, m_ctx.width, m_ctx.height);
        Rect C = A.Intersect(B);

        m_ctx.x_clipregion = C.x;
        m_ctx.y_clipregion = C.y;
        m_ctx.width_clipregion  = C.GetWidth();
        m_ctx.height_clipregion = C.GetHeight();

        TRefGL<IOpenGLFrameBufferObject> prevFBO = GetThreadGLDeviceFactory()->GetCurrentFrameBufferObject();

        if((m_FrameBufferObject->GetWidth() != buffer_width) || (m_FrameBufferObject->GetHeight() != buffer_height))
        {
            m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
            m_MainColorRT = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8);
            m_MainDepthRT = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_D24S8);
        }
        m_FrameBufferObject->SetRenderTarget(0, m_MainColorRT->GetSurfaceLevel(0));
        m_FrameBufferObject->SetDepthSurface(m_MainDepthRT->GetSurfaceLevel(0));
        m_FrameBufferObject->Activate();

        GfxContext.SetViewport(0, 0, buffer_width, buffer_height);
        m_FrameBufferObject->EmptyClippingRegion();

        ClientDraw(GfxContext, m_ctx, force_draw);

        // Restore the main frame buffer object
        prevFBO->Activate();
        GfxContext.SetViewport(0, 0, window_width, window_height);
        GfxContext.ApplyClippingRectangle();
        GfxContext.Push2DWindow(window_width, window_height);

        // Copy the client frame buffer into the main frame buffer.
        {
            UINT w, h;
            w = m_MainColorRT->GetWidth();
            h = m_MainColorRT->GetHeight();
            int x = m_ctx.x;
            int y = m_ctx.y;

            TexCoordXForm texxform0;
            texxform0.uwrap = TEXWRAP_CLAMP;
            texxform0.vwrap = TEXWRAP_CLAMP;
            texxform0.FlipVCoord(true);
            GetThreadGraphicsContext()->QRP_GLSL_1Tex(x, y, w, h, m_MainColorRT, texxform0, Color(Color::White));
        }

        // After the client area is drawn reset the texture environment so that the color is used for next drawing.
        GfxContext.SetEnvModeSelectColor(GL_TEXTURE0);

        // go back to 2D in case that was changed by the client.
        GfxContext.SetViewport(0, 0, window_width, window_height);
        GfxContext.ApplyClippingRectangle();
        GfxContext.Push2DWindow(window_width, window_height);
    }
    else
    {
        int x = GfxContext.GetContextX();
        int y = GfxContext.GetContextY();

        // The clientarea is in absolute window coordinates. It needs to be offset so that it is in relative window coordinates.
        m_ctx.x = m_Geometry.x + x;
        m_ctx.y = m_Geometry.y + y;
        m_ctx.width  = m_Geometry.GetWidth();
        m_ctx.height = m_Geometry.GetHeight();

        // A is obtained from GfxContext. So A dimension's are in relative window coordinates.
        Rect A = GfxContext.GetClippingRegion();

        Rect B = Rect(m_ctx.x, m_ctx.y, m_ctx.width, m_ctx.height);
        Rect C = A.Intersect(B);

        m_ctx.x_clipregion = C.x;
        m_ctx.y_clipregion = C.y;
        m_ctx.width_clipregion  = C.GetWidth();
        m_ctx.height_clipregion = C.GetHeight();

        int window_width, window_height;
        window_width = GfxContext.GetContextWidth();
        window_height = GfxContext.GetContextHeight();

        SetClientViewport(GfxContext);
//         GfxContext.SetViewport(
//             m_ctx.x, window_height - m_ctx.y - m_ctx.height, m_ctx.width, m_ctx.height);
// 
//         GfxContext.SetDrawClippingRegion(
//             m_ctx.x_clipregion, 
//             window_height - m_ctx.y_clipregion - m_ctx.height_clipregion,
//             m_ctx.width_clipregion, 
//             m_ctx.height_clipregion);

        ClientDraw(GfxContext, m_ctx, force_draw);

        // After the client area is drawn reset the texture environment so that the color is used for next drawing.
        GfxContext.SetEnvModeSelectColor(GL_TEXTURE0);

        // go back to 2D in case that was changed by the client.
        GfxContext.SetViewport(0, 0, window_width, window_height);
        GfxContext.ApplyClippingRectangle();
        GfxContext.Push2DWindow(window_width, window_height);
    }
}

void ClientArea::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    // don't draw here or we risk drawing more than one time.
    //BeginDraw(GfxContext, force_draw);
}

void ClientArea::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    BeginDraw(GfxContext, force_draw);

}
void ClientArea::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{
    // don't draw here or we risk drawing more than one time.
    //BeginDraw(GfxContext, force_draw);
}

void ClientArea::ClientDraw(GraphicsContext& GfxContext, DrawAreaContext& ctx, bool force_draw)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ClientArea::SetClientViewport(GraphicsContext& GfxContext)
{
    if(GetGraphicsThread()->GetWindow().HasFrameBufferSupport())
    {
        GfxContext.SetViewport(0, 0, m_FrameBufferObject->GetWidth(), m_FrameBufferObject->GetHeight());
        m_FrameBufferObject->EmptyClippingRegion();
    }
    else
    {
        int window_width, window_height;
        window_width = GfxContext.GetContextWidth();
        window_height =GfxContext.GetContextHeight();

        int client_viewport_x = m_Geometry.x;
        int client_viewport_y = m_Geometry.y; //window_height - getViewPosY() - getViewHeight();
        int client_viewport_width = m_Geometry.GetWidth();
        int client_viewport_height = m_Geometry.GetHeight();

        GfxContext.SetViewport(
            m_ctx.x, window_height - m_ctx.y - m_ctx.height, m_ctx.width, m_ctx.height);

        GfxContext.SetDrawClippingRegion(
            m_ctx.x_clipregion, 
            window_height - m_ctx.y_clipregion - m_ctx.height_clipregion,
            m_ctx.width_clipregion, 
            m_ctx.height_clipregion);
    }
}

void ClientArea::Setup2DMode(GraphicsContext& GfxContext)
{
    int window_width, window_height;
    window_width = GfxContext.GetContextWidth();
    window_height =GfxContext.GetContextHeight();

    //Restore 2D ViewPort
    GfxContext.SetViewport(0, 0, m_Geometry.GetWidth(), m_Geometry.GetHeight());
    GfxContext.Push2DWindow(m_Geometry.GetWidth(), m_Geometry.GetHeight());

}


void ClientArea::RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{

}

void ClientArea::RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{

}

void ClientArea::RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{

}

void ClientArea::RecvMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{

}

void ClientArea::RecvKeyEvent(
                          GraphicsContext&    GfxContext,    /*Graphics Context for text operation*/
                          unsigned long       event_type,    /*event type*/
                          unsigned long       event_keysym,    /*event keysym*/
                          unsigned long       event_state,    /*event state*/
                          const char*         event_char,    /*character*/
                          bool                repeated,    /*true if the key is repeated more than once*/
                          unsigned short      repeat_count     /*key repeat count*/
                          )
{

}

void ClientArea::NeedRedraw()
{
    GetGraphicsThread()->AddClientAreaToRedrawList(this);
    m_NeedRedraw = true;
}

void ClientArea::RealTime(bool b)
{
    m_IsRealTime = b;

    if(m_IsRealTime)
    {
        GetGraphicsThread()->AddClientAreaToRedrawList(this);
        m_RealTimeHandler = GetThreadTimer().AddTimerHandler(10, m_RealTimeCallback, this);
    }
    else
    {
        m_RealTimeHandler = 0;
    }
}

bool ClientArea::IsRealTime() const
{
    return m_IsRealTime;
}

void ClientArea::RealTimeHandler(void* v)
{
    GetGraphicsThread()->AddClientAreaToRedrawList(this);
    if(m_IsRealTime)
    {
        // With a time set to 0, the timer will be trigger at the next frame (whenever that frame happens);
        m_RealTimeHandler = GetThreadTimer().AddTimerHandler(0, m_RealTimeCallback, this);
    }
}

NAMESPACE_END_GUI
