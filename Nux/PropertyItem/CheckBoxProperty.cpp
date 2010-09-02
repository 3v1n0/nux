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

#include "CheckBox.h"
#include "CheckBoxProperty.h"

NAMESPACE_BEGIN_GUI

CheckBoxPropertyItem::CheckBoxPropertyItem(const TCHAR* name, int On)
:   SectionProperty(name, NODE_TYPE_CHECKBOX)
,   CheckBox(TEXT(""), On)
{
    SetState(On? true : false);
    setUsingStyleDrawing(false);
    NODE_SIG_CONNECT(sigStateToggled, CheckBoxPropertyItem, RecvPropertyChange);
}

CheckBoxPropertyItem::~CheckBoxPropertyItem()
{

}

long CheckBoxPropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    ret = ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    return ret;
}

void CheckBoxPropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry ItemGeo, const BasePainter& Painter, 
                                        RowHeader* row, const std::vector<header2>& column_vector, Color ItemBackgroundColor)
{
    Geometry geo2 = m_FirstColumnUsableGeometry;
    bool isSelected = (this == table->GetSelectedItem());

    if(isDirtyItem() || IsRedrawNeeded())
    {
        UINT nBackground = table->PushItemBackground(GfxContext, this);
        Painter.PaintTextLineStatic(GfxContext, GetFont(), m_FirstColumnUsableGeometry, row->item->GetName(), GetItemTextColor()); 
        if(m_ItemGeometryVector.size() >= 2)
        {
            Geometry geo2 = m_ItemGeometryVector[1];
            Geometry prop_geo;
            prop_geo.SetX(ItemGeo.x + ItemGeo.GetWidth());
            prop_geo.SetY(ItemGeo.y);
            prop_geo.SetWidth(column_vector[1].header.GetBaseWidth());
            prop_geo.SetHeight(ItemGeo.GetHeight());

            geo2.Expand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
            GfxContext.PushClippingRectangle(geo2);
            GfxContext.PushClippingRectangle(prop_geo);
            Painter.Paint2DQuadColor(GfxContext, geo2, ItemBackgroundColor);
            ProcessDraw(GfxContext, true);
            GfxContext.PopClippingRectangle();
            GfxContext.PopClippingRectangle();
        }

        table->PopItemBackground(GfxContext, nBackground);
    }
}

void CheckBoxPropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector)
{
    if(m_ItemGeometryVector.size() >= 2)
    {
        Geometry geo;
        geo = m_ItemGeometryVector[1];
        geo = geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        setGeometry(geo);
    }
}

int CheckBoxPropertyItem::GetItemBestHeight()
{
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
}

CheckBoxPropertyItem* CheckBoxPropertyItem::CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const char* Name, int id)
{
    int check = 0;
    QueryNodeXMLIntAttribute(elementxml, "Check",       &check,     id);
    CheckBoxPropertyItem *node = new CheckBoxPropertyItem(Name, check);
    node->SetID(id);
    return node;
}

TiXmlElement* CheckBoxPropertyItem::ToXML() const
{
    TiXmlElement* elementxml = NodeNetCom::ToXML();
    elementxml->SetAttribute("Check", GetState()? 1 : 0);
    return elementxml;
}

bool CheckBoxPropertyItem::FromXML(const TiXmlElement* elementxml)
{
    int check;
    if(QueryNodeXMLIntAttribute(elementxml, "Check", &check, GetID()))
    {
        SetState(check? true : false);
        //nuxDebugMsg(TEXT("Updated Node %s - ID: %d - Type: %s"), GetName().c_str(), GetID(), ConvertTypeToString(GetParameterType()));
        return NodeNetCom::FromXML(elementxml);
    }
    return false;
}

NAMESPACE_END_GUI
