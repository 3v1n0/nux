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
#include "NuxGraphics/ImageSurface.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(TextureArea);

  TextureArea::TextureArea(NUX_FILE_LINE_DECL)
  : View(NUX_FILE_LINE_PARAM)
  {
    mouse_down.connect(sigc::mem_fun(this, &TextureArea::RecvMouseDown));
    mouse_up.connect(sigc::mem_fun(this, &TextureArea::RecvMouseUp));
    mouse_enter.connect(sigc::mem_fun(this, &TextureArea::RecvMouseEnter));
    mouse_leave.connect(sigc::mem_fun(this, &TextureArea::RecvMouseLeave));
    mouse_click.connect(sigc::mem_fun(this, &TextureArea::RecvMouseClick));
    mouse_drag.connect(sigc::mem_fun(this, &TextureArea::RecvMouseDrag));

    paint_layer_ = new ColorLayer(nux::color::Black);
    rotation_2d_.Identity();
  }

  TextureArea::~TextureArea()
  {
    if (paint_layer_)
      delete paint_layer_;
  }

  void TextureArea::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    // Ability to rotate the widget around its center
    graphics_engine.PushModelViewMatrix(Matrix4::TRANSLATE(-GetBaseX() - GetBaseWidth() / 2, -GetBaseY() - GetBaseHeight() / 2, 0));
    graphics_engine.PushModelViewMatrix(Get2DRotation());
    graphics_engine.PushModelViewMatrix(Matrix4::TRANSLATE(GetBaseX() + GetBaseWidth() / 2, GetBaseY() + GetBaseHeight() / 2, 0));

    // The TextureArea should not render the accumulated background. That is left to the caller.
    // GetPainter().PaintBackground(graphics_engine, GetGeometry());

    if (paint_layer_)
    {
      paint_layer_->SetGeometry(GetGeometry());
      GetPainter().RenderSinglePaintLayer(graphics_engine, GetGeometry(), paint_layer_);
    }

    graphics_engine.PopModelViewMatrix();
    graphics_engine.PopModelViewMatrix();
    graphics_engine.PopModelViewMatrix();
  }

  void TextureArea::DrawContent(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {

  }

  void TextureArea::SetTexture(BaseTexture *texture)
  {
    NUX_RETURN_IF_NULL(texture);
    delete paint_layer_;

    TexCoordXForm texxform;
    texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap(TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    paint_layer_ = new TextureLayer(texture->GetDeviceTexture(), texxform, color::White);

    QueueDraw();
  }

  void TextureArea::SetColor(const Color &color)
  {
    delete paint_layer_;
    paint_layer_ = new ColorLayer(color);
    QueueDraw();
  }

  void TextureArea::LoadImageFile(const std::string &filename)
  {
    if (paint_layer_)
    {
      delete paint_layer_;
      paint_layer_ = NULL;
    }

    BaseTexture *texture = LoadTextureFromFile(filename);

    if (texture)
    {
      TexCoordXForm texxform;
      ROPConfig rop;
      rop.Blend = true;
      rop.SrcBlend = GL_SRC_ALPHA;
      rop.DstBlend = GL_ONE_MINUS_SRC_ALPHA;

      paint_layer_ = new TextureLayer(texture->GetDeviceTexture(), texxform, color::White, true, rop);
      texture->UnReference();
    }
    else
    {
      paint_layer_ = new ColorLayer(nux::color::Black);
    }
  }

  void TextureArea::SetPaintLayer(AbstractPaintLayer *layer)
  {
    NUX_SAFE_DELETE(paint_layer_);
    paint_layer_ = layer->Clone();

    QueueDraw();
  }

  AbstractPaintLayer* TextureArea::GetPaintLayer() const
  {
    if (paint_layer_ == NULL)
      return NULL;
    return paint_layer_->Clone();
  }

// void TextureArea::SetTexture(const char* TextureFilename)
// {
//     // Who should delete the texture? This class or the user?
//     m_UserTexture = CreateTextureFromFile(TextureFilename);
//     QueueDraw();
// }

  void TextureArea::RecvMouseDown(int x, int y, long /* button_flags */, long /* key_flags */)
  {
    sigMouseDown.emit(x, y);
    QueueDraw();
  }

  void TextureArea::RecvMouseClick(int /* x */, int /* y */, long /* button_flags */, long /* key_flags */)
  {

  }

  void TextureArea::RecvMouseUp(int /* x */, int /* y */, long /* button_flags */, long /* key_flags */)
  {
    QueueDraw();
  }

  void TextureArea::RecvMouseEnter(int /* x */, int /* y */, long /* button_flags */, long /* key_flags */)
  {

  }

  void TextureArea::RecvMouseLeave(int /* x */, int /* y */, long /* button_flags */, long /* key_flags */)
  {

  }

  void TextureArea::RecvMouseDrag(int x, int y, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    sigMouseDrag.emit(x, y);
  }

  void TextureArea::Set2DRotation(float angle)
  {
    rotation_2d_.Rotate_z(angle);
    QueueDraw();
  }

  Matrix4 TextureArea::Get2DRotation() const
  {
    return rotation_2d_;
  }
}
