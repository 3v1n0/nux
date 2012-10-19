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


#include "Nux.h"
#include "NuxGraphics/ImageSurface.h"
#include "AnimatedTextureArea.h"

namespace nux
{

  AnimatedTextureArea::AnimatedTextureArea(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
    ,   m_UserTexture(0)
  {
    SetMinMaxSize(32, 32);

    mouse_down.connect(sigc::mem_fun(this, &AnimatedTextureArea::RecvMouseDown));
    mouse_drag.connect(sigc::mem_fun(this, &AnimatedTextureArea::RecvMouseDrag));

    m_TimerFunctor = new TimerFunctor();
    m_TimerFunctor->tick.connect(sigc::mem_fun(this, &AnimatedTextureArea::TimerNextFrame));
  }

  AnimatedTextureArea::~AnimatedTextureArea()
  {
    GetTimer().RemoveTimerHandler(m_TimerHandler);
    m_TimerHandler = 0;
    delete m_TimerFunctor;
    m_TimerFunctor = 0;
  }

  void AnimatedTextureArea::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    if (m_UserTexture)
    {
      GetPainter().PaintBackground(graphics_engine, GetGeometry());
      graphics_engine.GetRenderStates().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      nux::Geometry base = GetGeometry();
      nux::TexCoordXForm texxform;
      graphics_engine.QRP_1Tex(base.x, base.y, base.width, base.height, m_UserTexture->GetDeviceTexture(), texxform, nux::color::White);

      graphics_engine.GetRenderStates().SetBlend(false);
    }
  }
  void AnimatedTextureArea::DrawContent(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {

  }

  void AnimatedTextureArea::SetTexture(TextureFrameAnimation *t)
  {
    m_UserTexture = t;

    if (m_UserTexture)
    {
      ObjectPtr< CachedTextureFrameAnimation > Texture = GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(m_UserTexture);
      ObjectPtr<IOpenGLAnimatedTexture> AnimatedTexture = Texture->m_Texture; //Texture->m_Texture.CastRef<IOpenGLAnimatedTexture>();
      ObjectPtr<IOpenGLBaseTexture> Texture2D = Texture->m_Texture; //Texture->m_Texture.CastRef<IOpenGLAnimatedTexture>();

      AnimatedTexture->SetFiltering(GL_LINEAR, GL_LINEAR);
      AnimatedTexture->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    }

    QueueDraw();
  }

  void AnimatedTextureArea::RecvMouseDown(int x, int y, long /* button_flags */, long /* key_flags */)
  {
    sigMouseDown.emit(x, y);
  }

  void AnimatedTextureArea::RecvMouseDrag(int x, int y, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    sigMouseDrag.emit(x, y);
  }

  void AnimatedTextureArea::StartAnimation()
  {
    if (m_TimerHandler.IsValid())
    {
      GetTimer().RemoveTimerHandler(m_TimerHandler);
      m_TimerHandler = 0;
    }

    m_TimerHandler = GetTimer().AddOneShotTimer(41, m_TimerFunctor, 0);
    QueueDraw();
  }

  void AnimatedTextureArea::StopAnimation()
  {
    if (m_TimerHandler.IsValid())
    {
      GetTimer().RemoveTimerHandler(m_TimerHandler);
      m_TimerHandler = 0;
    }
  }

  void AnimatedTextureArea::TimerNextFrame(void * /* v */)
  {
    if (m_UserTexture)
    {
      ObjectPtr< CachedTextureFrameAnimation > Texture = GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(m_UserTexture);
      ObjectPtr<IOpenGLAnimatedTexture> AnimatedTexture = Texture->m_Texture; //Texture->m_Texture.CastRef<IOpenGLAnimatedTexture>();
      ObjectPtr<IOpenGLBaseTexture> Texture2D = Texture->m_Texture; //Texture->m_Texture.CastRef<IOpenGLAnimatedTexture>();

      AnimatedTexture->PresentNextFrame();
      m_TimerHandler = GetTimer().AddOneShotTimer(41, m_TimerFunctor, 0);
    }

    QueueDraw();
  }

}
