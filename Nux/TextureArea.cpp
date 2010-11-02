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
#include "TextureArea.h"
#include "NuxImage/ImageSurface.h"

namespace nux
{

  TextureArea::TextureArea (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    //SetMinMaxSize(50, 50);

    OnMouseDown.connect (sigc::mem_fun (this, &TextureArea::RecvMouseDown) );
    OnMouseDrag.connect (sigc::mem_fun (this, &TextureArea::RecvMouseDrag) );

    m_PaintLayer = new ColorLayer (Color (0xFFFF40FF) );
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

  void TextureArea::Draw (GraphicsContext &GfxContext, bool force_draw)
  {
    gPainter.PaintBackground (GfxContext, GetGeometry() );

    if (m_PaintLayer)
    {
      m_PaintLayer->SetGeometry (GetGeometry() );
      gPainter.RenderSinglePaintLayer (GfxContext, GetGeometry(), m_PaintLayer);
    }
  }

  void TextureArea::DrawContent (GraphicsContext &GfxContext, bool force_draw)
  {

  }

  void TextureArea::PostDraw (GraphicsContext &GfxContext, bool force_draw)
  {

  }

  void TextureArea::SetTexture (BaseTexture *texture)
  {
    NUX_RETURN_IF_NULL (texture);
    NUX_SAFE_DELETE (m_PaintLayer);

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_PaintLayer = new TextureLayer (texture->GetDeviceTexture(), texxform, Color::White);

    NeedRedraw();
  }

  void TextureArea::SetPaintLayer (AbstractPaintLayer *layer)
  {
    NUX_SAFE_DELETE (m_PaintLayer);
    m_PaintLayer = layer->Clone();
  }

// void TextureArea::SetTexture(const TCHAR* TextureFilename)
// {
//     // Who should delete the texture? This class or the user?
//     m_UserTexture = CreateTextureFromFile(TextureFilename);
//     NeedRedraw();
// }

  void TextureArea::RecvMouseDown (int x, int y, long button_flags, long key_flags)
  {
    sigMouseDown.emit (x, y);
  }

  void TextureArea::RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    sigMouseDrag.emit (x, y);
  }

}
