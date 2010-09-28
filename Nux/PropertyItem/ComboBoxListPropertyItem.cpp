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

#include "ComboBoxComplex.h"
#include "ComboBoxListPropertyItem.h"

namespace nux { //NUX_NAMESPACE_BEGIN

ComboBoxListPropertyItem::ComboBoxListPropertyItem(const TCHAR* name)
:   SectionProperty(name, NODE_TYPE_COMBOCOMPLEX)
{
    //    SetRange(0.0f, 1.0f);
    //    SetValue(0.0f);
    AddItem(new TableItem("True"));
    AddItem(new TableItem("False"));
    AddItem(new TableItem("Test"));
}

ComboBoxListPropertyItem::~ComboBoxListPropertyItem()
{

}

long ComboBoxListPropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    ret = ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    return ret;
}

void ComboBoxListPropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, 
                                        RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
{
    Geometry geo2 = m_FirstColumnUsableGeometry;
    if(isDirtyItem() || IsRedrawNeeded())
    {
        UINT nBackground = table->PushItemBackground(GfxContext, this);
        Painter.PaintTextLineStatic(GfxContext, GetFont(), m_FirstColumnUsableGeometry, row->m_item->GetName(), GetItemTextColor()); 

        if(m_ItemGeometryVector.size() >= 2)
        {
            Geometry geo2 = m_ItemGeometryVector[1];
            Geometry prop_geo;
            prop_geo.SetX(geo.x + geo.GetWidth());
            prop_geo.SetY(geo.y);
            prop_geo.SetWidth(column_vector[1].m_header_area->GetBaseWidth());
            prop_geo.SetHeight(geo.GetHeight());

            geo2.Expand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
            GfxContext.PushClippingRectangle(geo2);
            GfxContext.PushClippingRectangle(prop_geo);
            ProcessDraw(GfxContext, true);
            GfxContext.PopClippingRectangle();
            GfxContext.PopClippingRectangle();
        }
        table->PopItemBackground(GfxContext, nBackground);
    }
}

void ComboBoxListPropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector)
{
    if(m_ItemGeometryVector.size() >= 2)
    {
        Geometry geo;
        geo = m_ItemGeometryVector[1];
        geo = geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        setGeometry(geo);
    }
}

int ComboBoxListPropertyItem::GetItemBestHeight()
{
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
}
} //NUX_NAMESPACE_END
