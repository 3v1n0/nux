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
#include "ActionItem.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(ActionItem);

  ActionItem::ActionItem(const char *label, int UserValue, NUX_FILE_LINE_DECL)
    :   Object(true, NUX_FILE_LINE_PARAM)
    ,   m_UserValue(UserValue)
    ,   m_Label("")
    ,   m_IsActivated(true)
    ,   m_Menu(0)
    ,   m_Enable(true)
  {
    m_Icon = 0;
    SetLabel(label);
  }

  ActionItem::~ActionItem()
  {
    if (m_Icon)
      m_Icon->UnReference();
  }

  void ActionItem::DrawAsMenuItem(GraphicsEngine &graphics_engine, InputArea &area, bool is_highlighted, bool /* draw_icone */)
  {
    Geometry geo = area.GetGeometry();
    Geometry icon_geo(0, 0, 20, 20);
    Geometry text_geo = geo;

    text_geo.OffsetPosition(24, 2);
    text_geo.OffsetSize(2 * 24, 2 * 2);

    icon_geo.SetX(geo.x + 2);
    icon_geo.SetY(geo.y + 2);

    const char *label = GetLabel();

    if (is_highlighted)
    {
      GetPainter().Paint2DQuadColor(graphics_engine, geo, Color(COLOR_FOREGROUND_SECONDARY));
    }

    if (m_Icon)
      GetPainter().Draw2DTextureAligned(graphics_engine, m_Icon, icon_geo, TextureAlignmentStyle(eTACenter, eTACenter));

    GetPainter().PaintTextLineStatic(graphics_engine, GetSysFont(), text_geo, std::string(label), Color(0xFF000000), eAlignTextLeft);
  }

  void ActionItem::DrawAsToolButton(GraphicsEngine &graphics_engine, InputArea &area)
  {
    Geometry base = area.GetGeometry();

    if (area.IsMouseOwner())
    {
      if (area.IsMouseInside())
      {
        GetPainter().PaintShape(graphics_engine, base, Color(COLOR_BACKGROUND_SECONDARY),  eSHAPE_CORNER_ROUND2);
        GetPainter().PaintShape(graphics_engine, base, Color(COLOR_BLACK),  eSTROKE_CORNER_ROUND2);
      }
      else
      {
        GetPainter().PaintShape(graphics_engine, base, Color(COLOR_FOREGROUND_PRIMARY),  eSHAPE_CORNER_ROUND2);
        GetPainter().PaintShape(graphics_engine, base, Color(COLOR_BLACK),  eSTROKE_CORNER_ROUND2);
      }
    }
    else
    {
      if (area.IsMouseInside() /*TODO: && (!area.MouseFocusOnOtherArea())*/)
      {
        GetPainter().PaintShape(graphics_engine, base, Color(COLOR_FOREGROUND_PRIMARY),  eSHAPE_CORNER_ROUND2);
        GetPainter().PaintShape(graphics_engine, base, Color(COLOR_BLACK),  eSTROKE_CORNER_ROUND2);
      }
      else
      {
        GetPainter().PaintShape(graphics_engine, base, Color(COLOR_BACKGROUND_SECONDARY),  eSHAPE_CORNER_ROUND2);
        GetPainter().PaintShape(graphics_engine, base, Color(COLOR_BLACK),  eSTROKE_CORNER_ROUND2);
      }
    }

    GetPainter().Draw2DTextureAligned(graphics_engine, m_Icon, base, TextureAlignmentStyle(eTACenter, eTACenter));
  }

  void ActionItem::Activate(bool b)
  {
    m_IsActivated = b;
  }

  void ActionItem::Trigger() const
  {
    sigAction.emit();
  }

  void ActionItem::Enable(bool b)
  {
    m_Enable = b;
  }

  bool ActionItem::isEnabled() const
  {
    return m_Enable;
  }

  void ActionItem::SetLabel(const char *label)
  {
    m_Label = label;
  }

  const char *ActionItem::GetLabel() const
  {
    return m_Label.c_str();
  }

  void ActionItem::SetIcon(const BaseTexture* icon)
  {
    if (m_Icon)
      m_Icon->UnReference();
    m_Icon = icon->Clone();
  }

  // NUXTODO: should return the bitmap data instead or a const pointer?.
  const BaseTexture* ActionItem::GetIcon()
  {
    return m_Icon;
  }

//void ActionItem::SetMenu(Menu* menu)
//{
//    m_Menu = menu;
//}
//Menu* ActionItem::GetMenu() const
//{
//    return m_Menu;
//}
}
