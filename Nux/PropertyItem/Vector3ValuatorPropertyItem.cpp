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

#include "Vector3ValuatorDouble.h"
#include "Vector3ValuatorPropertyItem.h"

NAMESPACE_BEGIN_GUI

Vector3ValuatorPropertyItem::Vector3ValuatorPropertyItem(const TCHAR* name, double X, double Y, double Z,
                                                         double Step, double MinValue, double MaxValue)
                                                         :   SectionProperty(name, NODE_TYPE_SPINBOX)
                                                         ,   Vector3DoubleValuator(X, Y, Z, Step, MinValue, MaxValue)
{
    NODE_SIG_CONNECT(sigValueChanged, Vector3ValuatorPropertyItem, RecvPropertyChange);
}

Vector3ValuatorPropertyItem::~Vector3ValuatorPropertyItem()
{

}

long Vector3ValuatorPropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    ret = ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    return ret;
}

void Vector3ValuatorPropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, 
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

void Vector3ValuatorPropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector)
{
    if(m_ItemGeometryVector.size() >= 2)
    {
        Geometry geo;
        geo = m_ItemGeometryVector[1];
        geo = geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        setGeometry(geo);
    }
}

int Vector3ValuatorPropertyItem::GetItemBestHeight()
{
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
}

Vector3ValuatorPropertyItem* Vector3ValuatorPropertyItem::CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const char* Name, int id)
{
    double value = 0;
    double minvalue = 0;
    double maxvalue = 100;
    double step = 1;
    double X, Y, Z;
    QueryNodeXMLDoubleAttribute(elementxml, "X",        &X,     id);
    QueryNodeXMLDoubleAttribute(elementxml, "Y",        &Y,     id);
    QueryNodeXMLDoubleAttribute(elementxml, "Z",        &Z,     id);
    QueryNodeXMLDoubleAttribute(elementxml, "MinValue",    &minvalue,  id);
    QueryNodeXMLDoubleAttribute(elementxml, "MaxValue",    &maxvalue,  id);
    QueryNodeXMLDoubleAttribute(elementxml, "Step",        &step,      id);

    Vector3ValuatorPropertyItem *node = new Vector3ValuatorPropertyItem(Name, value, step, minvalue, maxvalue);
    node->SetID(id);
    return node;
}

TiXmlElement* Vector3ValuatorPropertyItem::ToXML() const
{
    TiXmlElement* elementxml = NodeNetCom::ToXML();
    elementxml->SetDoubleAttribute("X", GetVectorX());
    elementxml->SetDoubleAttribute("Y", GetVectorY());
    elementxml->SetDoubleAttribute("Z", GetVectorZ());
    elementxml->SetDoubleAttribute("Step", GetStep());
    elementxml->SetDoubleAttribute("MinValue", GetMinValue());
    elementxml->SetDoubleAttribute("MaxValue", GetMaxValue());
    return elementxml;
}

bool Vector3ValuatorPropertyItem::FromXML(const TiXmlElement* elementxml)
{
    double minvalue = 0;
    double maxvalue = 100;
    double step = 1;
    double X, Y, Z;
    QueryNodeXMLDoubleAttribute(elementxml, "X",        &X,     GetID());
    QueryNodeXMLDoubleAttribute(elementxml, "Y",        &Y,     GetID());
    QueryNodeXMLDoubleAttribute(elementxml, "Z",        &Z,     GetID());
    QueryNodeXMLDoubleAttribute(elementxml, "MinValue",    &minvalue,  GetID());
    QueryNodeXMLDoubleAttribute(elementxml, "MaxValue",    &maxvalue,  GetID());
    QueryNodeXMLDoubleAttribute(elementxml, "Step",        &step,      GetID());
    SetRange(minvalue, maxvalue);
    SetStep(step);
    SetValue(X, Y, Z);
    return NodeNetCom::FromXML(elementxml);
}
NAMESPACE_END_GUI
