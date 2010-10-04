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


#ifndef CLIENTAREA_H
#define CLIENTAREA_H

#include "InputArea.h"
#include "Painter.h"
#include "View.h"
#include "TimerProc.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class ClientArea;
class TimerFunctor;
class TimerHandle;

struct ClientAreaDraw
{
    ClientArea* clientarea;
    Geometry clipgeometry;
};

class ClientArea: public View
{
public:
    ClientArea(NUX_FILE_LINE_PROTO);
    ~ClientArea();

    virtual void BeginDraw(GraphicsContext& GfxContext, bool force_draw);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void NeedRedraw();

    void EnableClientDraw(bool b) { m_IsClientAreaEnabled = b; };
    bool IsClientDrawEnabled() const {return m_IsClientAreaEnabled;};
    
    virtual void ClientDraw(GraphicsContext& GfxContext, DrawAreaContext& ctx, bool force_draw);
    virtual void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvKeyEvent(
        GraphicsContext&    ,    /*Graphics Context for text operation*/
        unsigned long       ,    /*event type*/
        unsigned long       ,    /*event keysym*/
        unsigned long       ,    /*event state*/
        const char*         ,    /*character*/
        bool                ,    /*true if the key is repeated more than once*/
        unsigned short           /*key repeat count*/
        );

    sigc::signal<void, DrawAreaContext, bool> sigClientDraw;
    void SetClientViewport(GraphicsContext& GfxContext);
    void Setup2DMode(GraphicsContext& GfxContext);

    void RealTime(bool b);
    bool IsRealTime() const;
    void RealTimeHandler(void* v);

    // Before the client start drawing we set up a framebuffer object. We don't want the client to start messing
    // up the whole rendering by. If we setup a framebuffer instead, the client can never know the framebuffer 
    // we use fror the whole rendering. all we have to do is to copy the client framebuffer into the main framebuffer
    // after the client as finished with the draw.
    TRefGL<IOpenGLFrameBufferObject>& GetWindowFrameBufferObject(){ return m_FrameBufferObject;}
    TRefGL<IOpenGLFrameBufferObject> m_FrameBufferObject;

private:
    // We use Rectangle texture to attach to the framebuffer because some GPU like the Geforce FX 5600 do not 
    // have support for ARB_texture_non_power_of_two. However it does support ARB_texture_recatangle.
    TRefGL<IOpenGLTexture2D> m_MainColorRT;
    TRefGL<IOpenGLTexture2D> m_MainDepthRT;

    TimerFunctor* m_RealTimeCallback;
    TimerHandle m_RealTimeHandler;

    DrawAreaContext m_ctx;
    bool m_IsClientAreaEnabled;
    bool m_IsRealTime;
};


} //NUX_NAMESPACE_END

#endif // CLIENTAREA_H
