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
#include "PropertyList.h"

#include "FilePropertyItem.h"

namespace nux
{

  FilePropertyItem::FilePropertyItem (const TCHAR *name)
    :   SectionProperty (name)
  {
    m_hlayout = new HLayout();
    m_TextEntry = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_OpenButton = new Button (TEXT (""), NUX_TRACKER_LOCATION);
    m_hlayout->AddView (m_TextEntry, 1, eCenter);
    m_hlayout->AddView (m_OpenButton, 0, eCenter);
    m_hlayout->SetHorizontalInternalMargin (2);
    GetGraphicsThread()->ComputeElementLayout (m_hlayout);

    m_OpenButton->SetCaption (TEXT ("...") );
    m_OpenButton->SetMinimumSize (32, DEFAULT_WIDGET_HEIGHT);
  }

  FilePropertyItem::~FilePropertyItem()
  {
  }

  long FilePropertyItem::ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    ret = m_TextEntry->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_OpenButton->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void FilePropertyItem::DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter,
                                       RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
  {
    if (isDirtyItem() ||
        m_OpenButton->IsRedrawNeeded() ||
        m_TextEntry->IsRedrawNeeded() )
    {
      t_u32 nBackground = table->PushItemBackground (GfxContext, this);
      Painter.PaintTextLineStatic (GfxContext, GetSysFont(), m_FirstColumnUsableGeometry, row->m_item->GetName(), GetItemTextColor() );

      if (m_ItemGeometryVector.size() >= 2)
      {
        Geometry prop_geo = m_ItemGeometryVector[1];
        Geometry geo2 = m_ItemGeometryVector[1];
        geo2.Expand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        GfxContext.PushClippingRectangle (geo2);
        GfxContext.PushClippingRectangle (prop_geo);
        m_TextEntry->ProcessDraw (GfxContext, true);
        m_OpenButton->ProcessDraw (GfxContext, true);
        GfxContext.PopClippingRectangle();
        GfxContext.PopClippingRectangle();
      }

      table->PopItemBackground (GfxContext, nBackground);
    }
  }

  void FilePropertyItem::ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector)
  {
    if (m_ItemGeometryVector.size() >= 2)
    {
      Geometry geo;
      geo = m_ItemGeometryVector[1];
      geo = geo.GetExpand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
      m_hlayout->SetGeometry (geo);
      GetGraphicsThread()->ComputeElementLayout (m_hlayout);
    }
  }

  int FilePropertyItem::GetItemBestHeight()
  {
    Size sz = m_TextEntry->GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
  }

}
