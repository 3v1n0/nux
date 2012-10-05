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


#ifndef CLIENTAREA_H
#define CLIENTAREA_H

#include "InputArea.h"
#include "Painter.h"
#include "View.h"
#include "TimerProc.h"

namespace nux
{

  class ClientArea;

  struct ClientAreaDraw
  {
    ClientArea *clientarea;
    Geometry clipgeometry;
  };

  class ClientArea: public View
  {
  public:
    ClientArea(NUX_FILE_LINE_PROTO);
    ~ClientArea();

    virtual void BeginDraw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    void EnableClientDraw(bool b)
    {
      m_IsClientAreaEnabled = b;
    };
    bool IsClientDrawEnabled() const
    {
      return m_IsClientAreaEnabled;
    };

    virtual void ClientDraw(GraphicsEngine &graphics_engine, DrawAreaContext &ctx, bool force_draw);
    virtual void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvKeyEvent(
      unsigned long       ,   /*event type*/
      unsigned long       ,   /*event keysym*/
      unsigned long       ,   /*event state*/
      const char*        ,   /*character*/
      unsigned short          /*key repeat count*/
    );

    sigc::signal<void, DrawAreaContext, bool> sigClientDraw;
    void SetClientViewport(GraphicsEngine &graphics_engine);
    void Setup2DMode(GraphicsEngine &graphics_engine);

    // Before the client start drawing we set up a framebuffer object. We don't want the client to start messing
    // up the whole rendering by. If we setup a framebuffer instead, the client can never know the framebuffer
    // we use fror the whole rendering. all we have to do is to copy the client framebuffer into the main framebuffer
    // after the client as finished with the draw.
    ObjectPtr<IOpenGLFrameBufferObject>& GetWindowFrameBufferObject()
    {
      return m_FrameBufferObject;
    }
    ObjectPtr<IOpenGLFrameBufferObject> m_FrameBufferObject;

  protected:
    virtual bool AcceptKeyNavFocus();

  private:
    // We use Rectangle texture to attach to the framebuffer because some GPU like the Geforce FX 5600 do not
    // have support for ARB_texture_non_power_of_two. However it does support ARB_texture_recatangle.
    ObjectPtr<IOpenGLBaseTexture> m_MainColorRT;
    ObjectPtr<IOpenGLBaseTexture> m_MainDepthRT;

    DrawAreaContext m_ctx;
    bool m_IsClientAreaEnabled;
  };


}

#endif // CLIENTAREA_H
