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

#include "SpinBox.h"
#include "SpinBoxPropertyItem.h"

NAMESPACE_BEGIN_GUI

SpinBoxPropertyItem::SpinBoxPropertyItem(const TCHAR* name, int Value, int Step, int MinValue, int MaxValue)
:   SectionProperty(name, NODE_TYPE_SPINBOX)
,   SpinBox(Value, Step, MinValue, MaxValue)
{
    NODE_SIG_CONNECT(sigValueChanged, SpinBoxPropertyItem, RecvPropertyChange);
}

SpinBoxPropertyItem::~SpinBoxPropertyItem()
{

}

long SpinBoxPropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    ret = ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    return ret;
}

void SpinBoxPropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, 
                                       RowHeader* row, const std::vector<header2>& column_vector, Color ItemBackgroundColor)
{
    Geometry geo2 = m_FirstColumnUsableGeometry;
    if(isDirtyItem() || IsRedrawNeeded())
    {
        UINT nBackground = table->PushItemBackground(GfxContext, this);
        Painter.PaintTextLineStatic(GfxContext, GetFont(), m_FirstColumnUsableGeometry, row->item->GetName(), GetItemTextColor()); 

        if(m_ItemGeometryVector.size() >= 2)
        {
            Geometry geo2 = m_ItemGeometryVector[1];
            Geometry prop_geo;
            prop_geo.SetX(geo.x + geo.GetWidth());
            prop_geo.SetY(geo.y);
            prop_geo.SetWidth(column_vector[1].header.GetBaseWidth());
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

void SpinBoxPropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector)
{
    if(m_ItemGeometryVector.size() >= 2)
    {
        Geometry geo;
        geo = m_ItemGeometryVector[1];
        geo = geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        setGeometry(geo);
    }
}

int SpinBoxPropertyItem::GetItemBestHeight()
{
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
}

SpinBoxPropertyItem* SpinBoxPropertyItem::CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const char* Name, int id)
{
    int value = 0;
    int minvalue = 0;
    int maxvalue = 100;
    int step = 1;
    QueryNodeXMLIntAttribute(elementxml, "Value",       &value,     id);
    QueryNodeXMLIntAttribute(elementxml, "MinValue",    &minvalue,  id);
    QueryNodeXMLIntAttribute(elementxml, "MaxValue",    &maxvalue,  id);
    QueryNodeXMLIntAttribute(elementxml, "Step",        &step,      id);

    SpinBoxPropertyItem *node = new SpinBoxPropertyItem(Name, value, step, minvalue, maxvalue);
    node->SetID(id);
    return node;
}

TiXmlElement* SpinBoxPropertyItem::ToXML() const
{
    TiXmlElement* elementxml = NodeNetCom::ToXML();
    elementxml->SetAttribute("Value", GetValue());
    elementxml->SetAttribute("Step", GetStep());
    elementxml->SetAttribute("MinValue", GetMinValue());
    elementxml->SetAttribute("MaxValue", GetMaxValue());
    return elementxml;
}

bool SpinBoxPropertyItem::FromXML(const TiXmlElement* elementxml)
{
    int value = 0;
    int minvalue = 0;
    int maxvalue = 100;
    int step = 1;
    QueryNodeXMLIntAttribute(elementxml, "Value",       &value,     GetID());
    QueryNodeXMLIntAttribute(elementxml, "MinValue",    &minvalue,  GetID());
    QueryNodeXMLIntAttribute(elementxml, "MaxValue",    &maxvalue,  GetID());
    QueryNodeXMLIntAttribute(elementxml, "Step",        &step,      GetID());
    SetRange(minvalue, maxvalue);
    SetStep(step);
    SetValue(value);
    return NodeNetCom::FromXML(elementxml);
}
NAMESPACE_END_GUI
