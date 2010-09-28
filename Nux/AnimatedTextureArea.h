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


#ifndef ANIMATEDTEXTUREAREA_H
#define ANIMATEDTEXTUREAREA_H

#include "NuxGraphics/OpenGLEngine.h"
#include "TimerProc.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class AnimatedTextureArea : public ActiveInterfaceObject
{
public:
    AnimatedTextureArea(NUX_FILE_LINE_PROTO);
    ~AnimatedTextureArea();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void SetTexture(NAnimatedTexture* Texture);
    void RecvMouseDown(int x, int y, long button_flags, long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    sigc::signal<void, int, int> sigMouseDown;
    sigc::signal<void, int, int> sigMouseDrag;

    void StartAnimation();
    void StopAnimation();


private:
    void TimerNextFrame(void* v);
    TimerFunctor* m_TimerFunctor;
    TimerHandle m_TimerHandler;

    NAnimatedTexture* m_DefaultTexture;
    NAnimatedTexture* m_UserTexture;
};

} //NUX_NAMESPACE_END
#endif // ANIMATEDTEXTUREAREA_H
