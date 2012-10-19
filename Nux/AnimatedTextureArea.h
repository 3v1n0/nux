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


#ifndef ANIMATEDTEXTUREAREA_H
#define ANIMATEDTEXTUREAREA_H

#include "NuxGraphics/GraphicsEngine.h"
#include "TimerProc.h"

namespace nux
{

  class AnimatedTextureArea : public View
  {
  public:
    AnimatedTextureArea(NUX_FILE_LINE_PROTO);
    ~AnimatedTextureArea();
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    void SetTexture(TextureFrameAnimation *Texture);
    void RecvMouseDown(int x, int y, long button_flags, long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    sigc::signal<void, int, int> sigMouseDown;
    sigc::signal<void, int, int> sigMouseDrag;

    void StartAnimation();
    void StopAnimation();


  private:
    void TimerNextFrame(void *v);
    TimerFunctor *m_TimerFunctor;
    TimerHandle m_TimerHandler;

    TextureFrameAnimation *m_UserTexture;
  };

}
#endif // ANIMATEDTEXTUREAREA_H
