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

#include "SpinBoxDouble.h"
#include "SpinBoxDoublePropertyItem.h"

NAMESPACE_BEGIN_GUI

SpinBoxDoublePropertyItem::SpinBoxDoublePropertyItem(const TCHAR* name, double Value, double Step, double MinValue, double MaxValue)
:   SectionProperty(name, NODE_TYPE_SPINBOX)
,   SpinBoxDouble(Value, Step, MinValue, MaxValue)
{
    NODE_SIG_CONNECT(sigValueChanged, SpinBoxDoublePropertyItem, RecvPropertyChange);
}

SpinBoxDoublePropertyItem::~SpinBoxDoublePropertyItem()
{

}

long SpinBoxDoublePropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    ret = ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    return ret;
}

void SpinBoxDoublePropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, 
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

void SpinBoxDoublePropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector)
{
    if(m_ItemGeometryVector.size() >= 2)
    {
        Geometry geo;
        geo = m_ItemGeometryVector[1];
        geo = geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        setGeometry(geo);
    }
}

int SpinBoxDoublePropertyItem::GetItemBestHeight()
{
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
}

SpinBoxDoublePropertyItem* SpinBoxDoublePropertyItem::CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const char* Name, int id)
{
    double value = 0;
    double minvalue = 0;
    double maxvalue = 100;
    double step = 1;
    QueryNodeXMLDoubleAttribute(elementxml, "Value",       &value,     id);
    QueryNodeXMLDoubleAttribute(elementxml, "MinValue",    &minvalue,  id);
    QueryNodeXMLDoubleAttribute(elementxml, "MaxValue",    &maxvalue,  id);
    QueryNodeXMLDoubleAttribute(elementxml, "Step",        &step,      id);

    SpinBoxDoublePropertyItem *node = new SpinBoxDoublePropertyItem(Name, value, step, minvalue, maxvalue);
    node->SetID(id);
    return node;
}

TiXmlElement* SpinBoxDoublePropertyItem::ToXML() const
{
    TiXmlElement* elementxml = NodeNetCom::ToXML();
    elementxml->SetDoubleAttribute("Value", GetValue());
    elementxml->SetDoubleAttribute("Step", GetStep());
    elementxml->SetDoubleAttribute("MinValue", GetMinValue());
    elementxml->SetDoubleAttribute("MaxValue", GetMaxValue());
    return elementxml;
}

bool SpinBoxDoublePropertyItem::FromXML(const TiXmlElement* elementxml)
{
    double value = 0;
    double minvalue = 0;
    double maxvalue = 100;
    double step = 1;
    QueryNodeXMLDoubleAttribute(elementxml, "Value",       &value,     GetID());
    QueryNodeXMLDoubleAttribute(elementxml, "MinValue",    &minvalue,  GetID());
    QueryNodeXMLDoubleAttribute(elementxml, "MaxValue",    &maxvalue,  GetID());
    QueryNodeXMLDoubleAttribute(elementxml, "Step",        &step,      GetID());
    SetRange(minvalue, maxvalue);
    SetStep(step);
    SetValue(value);
    return NodeNetCom::FromXML(elementxml);
}
NAMESPACE_END_GUI