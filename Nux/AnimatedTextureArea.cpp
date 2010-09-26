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
#include "NuxImage/ImageSurface.h"
#include "AnimatedTextureArea.h"

NAMESPACE_BEGIN_GUI

AnimatedTextureArea::AnimatedTextureArea(NUX_FILE_LINE_DECL)
:   ActiveInterfaceObject(NUX_FILE_LINE_PARAM)
,   m_UserTexture(0)
{
    SetMinMaxSize(32, 32);

//     NAnimatedTextureData texData;
//     texData.AllocateCheckBoardTexture(16, 16, 1, 0xFF222222, 0xFF666666);
//     m_DefaultTexture = new NTexture2D();
//     m_DefaultTexture->Update(&texData);

    OnMouseDown.connect(sigc::mem_fun(this, &AnimatedTextureArea::RecvMouseDown));
    OnMouseDrag.connect(sigc::mem_fun(this, &AnimatedTextureArea::RecvMouseDrag));

    m_TimerFunctor = new TimerFunctor();
    m_TimerFunctor->OnTimerExpired.connect(sigc::mem_fun(this, &AnimatedTextureArea::TimerNextFrame));
}

AnimatedTextureArea::~AnimatedTextureArea()
{
    NUX_SAFE_DELETE(m_DefaultTexture);

    GetThreadTimer().RemoveTimerHandler(m_TimerHandler);
    m_TimerHandler = 0;
    delete m_TimerFunctor;
    m_TimerFunctor = 0;
}

long AnimatedTextureArea::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    return PostProcessEvent2(ievent, TraverseInfo, ProcessEventInfo);
}

void AnimatedTextureArea::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    if(m_UserTexture)
    {
        gPainter.PaintBackground(GfxContext, GetGeometry());
        GfxContext.GetRenderStates().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        nux::Geometry base = GetGeometry();
        nux::TexCoordXForm texxform;
        GfxContext.QRP_GLSL_1Tex(base.x, base.y, base.width, base.height, m_UserTexture->GetDeviceTexture(), texxform, nux::Color::White);

        GfxContext.GetRenderStates().SetBlend(false);
    }
}
void AnimatedTextureArea::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{

}

void AnimatedTextureArea::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void AnimatedTextureArea::SetTexture(NAnimatedTexture* Texture)
{
    m_UserTexture = Texture;
    if(m_UserTexture)
    {
        TRefGL< NGLAnimatedTexture > Texture = GetThreadGraphicsContext()->CacheResource(m_UserTexture);
        TRefGL<IOpenGLAnimatedTexture> AnimatedTexture = Texture->m_Texture.CastRef<IOpenGLAnimatedTexture>();
        TRefGL<IOpenGLTexture2D> Texture2D = Texture->m_Texture.CastRef<IOpenGLAnimatedTexture>();

        AnimatedTexture->SetFiltering(GL_LINEAR, GL_LINEAR);
        AnimatedTexture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    }
    NeedRedraw();
}

void AnimatedTextureArea::RecvMouseDown(int x, int y, long button_flags, long key_flags)
{
    sigMouseDown.emit(x, y);
}

void AnimatedTextureArea::RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    sigMouseDrag.emit(x, y);
}

void AnimatedTextureArea::StartAnimation()
{
    if(m_TimerHandler.IsValid())
    {
        GetThreadTimer().RemoveTimerHandler(m_TimerHandler);
        m_TimerHandler = 0;
    }
    m_TimerHandler = GetThreadTimer().AddTimerHandler(41, m_TimerFunctor, 0);
    NeedRedraw();
}

void AnimatedTextureArea::StopAnimation()
{
    if(m_TimerHandler.IsValid())
    {
        GetThreadTimer().RemoveTimerHandler(m_TimerHandler);
        m_TimerHandler = 0;
    }
}

void AnimatedTextureArea::TimerNextFrame(void* v)
{
    if(m_UserTexture)
    {
        TRefGL< NGLAnimatedTexture > Texture = GetThreadGraphicsContext()->CacheResource(m_UserTexture);
        TRefGL<IOpenGLAnimatedTexture> AnimatedTexture = Texture->m_Texture.CastRef<IOpenGLAnimatedTexture>();
        TRefGL<IOpenGLTexture2D> Texture2D = Texture->m_Texture.CastRef<IOpenGLAnimatedTexture>();

        AnimatedTexture->PresentNextFrame();
        m_TimerHandler = GetThreadTimer().AddTimerHandler(41, m_TimerFunctor, 0);
    }
    NeedRedraw();
}

NAMESPACE_END_GUI
