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
#include "ActionItem.h"
#include "TableCtrl.h"
#include "TableItem.h"
#include "DoubleValidator.h"
#include "PropertyList.h"

NAMESPACE_BEGIN_GUI

const int PROPERTY_BORDER_X = 2;
const int PROPERTY_BORDER_Y = 2;

SectionProperty::SectionProperty(const TCHAR* name, NodeParameterType type)
:   TableItem(name, type)
{
    Open();
    SetItemTextColor(GPropertyItemTextColor0);
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

void SectionProperty::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter,
                                   RowHeader* row, const std::vector<header2>& column_vector, Color ItemBackgroundColor)
{
    bool isSelected = (this == table->GetSelectedItem());
    Geometry FirstColumnGeometry = m_ItemGeometryVector[0];
    if(isDirtyItem())
    {
        UINT nBackground;
        if(m_bIsMouseInside)
        {
            nBackground = table->PushItemBackground(GfxContext, this, true);
        }
        else
        {
            nBackground = table->PushItemBackground(GfxContext, this, false);
        }
        Painter.PaintTextLineStatic(GfxContext, GFont, geo, row->item->GetName(), GetItemTextColor() /*m_item[r].c_str()*/); 
        if(m_ItemGeometryVector.size() >= 2)
        {
            Geometry prop_geo = m_ItemGeometryVector[1];
            prop_geo.Expand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
            GfxContext.PushClippingRectangle(prop_geo);
            Painter.PaintTextLineStatic(GfxContext, GFont, prop_geo, m_String.GetTCharPtr(), GetItemTextColor() /*m_item[r].c_str()*/); 
            GfxContext.PopClippingRectangle();
        }
        table->PopItemBackground(GfxContext, nBackground);
    }
}

void SectionProperty::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector)
{

}

int SectionProperty::GetItemBestHeight()
{
    return ITEMDEFAULTHEIGHT;
}   


NAMESPACE_END_GUI
