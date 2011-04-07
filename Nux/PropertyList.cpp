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
#include "ActionItem.h"
#include "TableCtrl.h"
#include "TableItem.h"
#include "DoubleValidator.h"
#include "PropertyList.h"

namespace nux
{

  const int PROPERTY_BORDER_X = 2;
  const int PROPERTY_BORDER_Y = 2;

  SectionProperty::SectionProperty (const TCHAR *name, NodeParameterType type)
    :   TableItem (name, type)
  {
    Open();
    SetItemTextColor (GPropertyItemTextColor0);
  }

  SectionProperty::~SectionProperty()
  {
  }

//long SectionProperty::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
//{
//    long ret = TraverseInfo;
//
//    return ret;
//}

  void SectionProperty::DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter,
                                      RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
  {
    Geometry FirstColumnGeometry = m_ItemGeometryVector[0];

    if (isDirtyItem() )
    {
      t_u32 nBackground;

      if (m_bIsMouseInside)
      {
        nBackground = table->PushItemBackground (GfxContext, this, true);
      }
      else
      {
        nBackground = table->PushItemBackground (GfxContext, this, false);
      }

      Painter.PaintTextLineStatic (GfxContext, GetSysFont(), geo, row->_table_item->GetName(), GetItemTextColor());

      if (m_ItemGeometryVector.size() >= 2)
      {
        Geometry prop_geo = m_ItemGeometryVector[1];
        prop_geo.Expand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        GfxContext.PushClippingRectangle (prop_geo);
        Painter.PaintTextLineStatic (GfxContext, GetSysFont(), prop_geo, m_String.GetTCharPtr(), GetItemTextColor());
        GfxContext.PopClippingRectangle();
      }

      table->PopItemBackground (GfxContext, nBackground);
    }
  }

  void SectionProperty::ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector)
  {

  }

  int SectionProperty::GetItemBestHeight()
  {
    return ITEMDEFAULTHEIGHT;
  }


}
