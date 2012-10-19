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
#include "NuxGraphics/GLTextureResourceManager.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "ActionItem.h"
#include "ToolButton.h"

namespace nux
{

  ToolButton::ToolButton(const char *BitmapFilename, NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
    ,   m_ActionItem(0)
  {
    m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();

    if (BitmapFilename)
      m_Texture->Update(BitmapFilename);

    // Set Original State
    SetBaseString("ToolButton");

    // Set Signals
    mouse_click.connect(sigc::mem_fun(this, &ToolButton::EmitClick));
    mouse_double_click.connect(sigc::mem_fun(this, &ToolButton::RecvMouseDoubleClick));
    mouse_down.connect(sigc::mem_fun(this, &ToolButton::RecvMouseDown));
    mouse_up.connect(sigc::mem_fun(this, &ToolButton::RecvMouseUp));
    mouse_enter.connect(sigc::mem_fun(this, &ToolButton::RecvMouseEnter));
    mouse_leave.connect(sigc::mem_fun(this, &ToolButton::RecvMouseLeave));

    SetMinimumSize(28, 28);
    SetGeometry(Geometry(0, 0, 24, 24));
  }

  ToolButton::~ToolButton()
  {
    m_Texture->UnReference();
  }

  void ToolButton::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();

    if (IsMouseInside() && !IsMouseOwner())
    {
      GetPainter().PaintBackground(graphics_engine, base);
      GetPainter().PaintShape(graphics_engine, base, Color(COLOR_BACKGROUND_SECONDARY),  eSHAPE_CORNER_ROUND2);
    }
    else if (IsMouseOwner())
    {
      GetPainter().PaintBackground(graphics_engine, base);
      GetPainter().PaintShape(graphics_engine, base, Color(0xFF2A2A2A),  eSHAPE_CORNER_ROUND2);
    }
    else
    {
      GetPainter().PaintBackground(graphics_engine, base);
      GetPainter().PaintShape(graphics_engine, base, Color(COLOR_BACKGROUND_PRIMARY),  eSHAPE_CORNER_ROUND2);
    }

    if (m_Texture)
      GetPainter().Draw2DTextureAligned(graphics_engine, m_Texture, base, TextureAlignmentStyle(eTACenter, eTACenter));
  }

  void ToolButton::DrawContent(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {

  }

  void ToolButton::SetState(bool /* b */)
  {

  }

  void ToolButton::SetBitmap(const BaseTexture* Texture)
  {
    nuxAssert(Texture);
    NUX_RETURN_IF_NULL(Texture);

    if (m_Texture)
      m_Texture->UnReference();
    m_Texture = Texture->Clone();
  }

  void ToolButton::EmitClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    sigClick.emit();
    if (m_ActionItem)
      m_ActionItem->Trigger();
  }

  void ToolButton::RecvMouseDoubleClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void ToolButton::RecvMouseDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void ToolButton::RecvMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void ToolButton::RecvMouseEnter(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void ToolButton::RecvMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void ToolButton::SetAction(ActionItem *action)
  {
    m_ActionItem = action;
  }

}
