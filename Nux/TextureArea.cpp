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
#include "TextureArea.h"
#include "NuxImage/ImageSurface.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE (TextureArea);

  TextureArea::TextureArea (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    //SetMinMaxSize(50, 50);

    mouse_down.connect (sigc::mem_fun (this, &TextureArea::RecvMouseDown));
    mouse_up.connect (sigc::mem_fun (this, &TextureArea::RecvMouseUp));
    
    mouse_enter.connect (sigc::mem_fun (this, &TextureArea::RecvMouseEnter));
    mouse_leave.connect (sigc::mem_fun (this, &TextureArea::RecvMouseLeave));
    mouse_click.connect (sigc::mem_fun (this, &TextureArea::RecvMouseClick));

    mouse_drag.connect (sigc::mem_fun (this, &TextureArea::RecvMouseDrag));

    m_PaintLayer = new ColorLayer (Color (0xFFFF40FF));
    _2d_rotate.Identity ();
  }

  TextureArea::~TextureArea()
  {
    NUX_SAFE_DELETE (m_PaintLayer);
    // m_UserTexture is delete by the user
  }

  long TextureArea::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return PostProcessEvent2 (ievent, TraverseInfo, ProcessEventInfo);
  }

  void TextureArea::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushModelViewMatrix (Matrix4::TRANSLATE(-GetBaseX () - GetBaseWidth () / 2, -GetBaseY () - GetBaseHeight () / 2, 0));
    GfxContext.PushModelViewMatrix (Get2DRotation ());    
    GfxContext.PushModelViewMatrix (Matrix4::TRANSLATE(GetBaseX () + GetBaseWidth () / 2, GetBaseY () + GetBaseHeight () / 2, 0));

    // The TextureArea should not render the accumulated background. That is left to the caller.
    // GetPainter().PaintBackground (GfxContext, GetGeometry() );

    if (m_PaintLayer)
    {
      m_PaintLayer->SetGeometry (GetGeometry ());
      GetPainter ().RenderSinglePaintLayer (GfxContext, GetGeometry (), m_PaintLayer);
    }

    GfxContext.PopModelViewMatrix ();
    GfxContext.PopModelViewMatrix ();
    GfxContext.PopModelViewMatrix ();
  }

  void TextureArea::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void TextureArea::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void TextureArea::SetTexture (BaseTexture *texture)
  {
    NUX_RETURN_IF_NULL (texture);
    NUX_SAFE_DELETE (m_PaintLayer);

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_PaintLayer = new TextureLayer (texture->GetDeviceTexture(), texxform, color::White);

    QueueDraw();
  }

  void TextureArea::SetPaintLayer (AbstractPaintLayer *layer)
  {
    NUX_SAFE_DELETE (m_PaintLayer);
    m_PaintLayer = layer->Clone();

    QueueDraw();
  }

// void TextureArea::SetTexture(const TCHAR* TextureFilename)
// {
//     // Who should delete the texture? This class or the user?
//     m_UserTexture = CreateTextureFromFile(TextureFilename);
//     QueueDraw();
// }

  void TextureArea::RecvMouseDown (int x, int y, long button_flags, long key_flags)
  {
    sigMouseDown.emit (x, y);
    QueueDraw ();
  }

  void TextureArea::RecvMouseClick (int x, int y, long button_flags, long key_flags)
  {

  }

  void TextureArea::RecvMouseUp (int x, int y, long button_flags, long key_flags)
  {
    QueueDraw ();
  }

  void TextureArea::RecvMouseEnter (int x, int y, long button_flags, long key_flags)
  {

  }

  void TextureArea::RecvMouseLeave (int x, int y, long button_flags, long key_flags)
  {

  }

  void TextureArea::RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    sigMouseDrag.emit (x, y);
  }

  void TextureArea::Set2DRotation (float angle)
  {
    _2d_rotate.Rotate_z (angle);
    QueueDraw ();
  }

  Matrix4 TextureArea::Get2DRotation () const
  {
    return _2d_rotate;
  }
}
