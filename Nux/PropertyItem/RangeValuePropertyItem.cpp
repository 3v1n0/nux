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

#include "RangeValuePropertyItem.h"

NAMESPACE_BEGIN_GUI

RangeValuePropertyItem::RangeValuePropertyItem(const TCHAR* name, float Value, float MinValue, float MaxValue)
:   SectionProperty(name, NODE_TYPE_RANGE)
{
    SetRange(MinValue, MaxValue);
    SetValue(Value);
    m_ValueString->SetMinimumSize(DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    //m_ValueString->SetMaximumHeight(14 /*PRACTICAL_WIDGET_HEIGHT*/);
    //m_ValueString->setGeometry(Geometry(0, 0, 3*DEFAULT_WIDGET_WIDTH, 16 /*PRACTICAL_WIDGET_HEIGHT*/));

    m_Percentage->SetMinimumSize(2*DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    //m_Percentage->SetMaximumHeight(14 /*PRACTICAL_WIDGET_HEIGHT*/);
    //m_Percentage->setGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, 16 /*PRACTICAL_WIDGET_HEIGHT*/));
    SetMaximumHeight(14);

    NODE_SIG_CONNECT(sigValueChanged, RangeValuePropertyItem, RecvPropertyChange);
}

RangeValuePropertyItem::~RangeValuePropertyItem()
{

}

long RangeValuePropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    Geometry geo = m_ItemGeometryVector[1];
    if((ievent.e_event == INL_MOUSE_PRESSED) && geo.IsPointInside(ievent.e_x, ievent.e_y) == false)
    {
        // This will filter out mouse down event that happened in the item in the same row on the right.
        // This is necessary because the widget we are testing maybe larger that the table element where it resides.
        // 
        //      ____________________________________________________________
        //      | NAME      | WIDGET         |     :             |
        //      |___________|________________|_____:_____________|___________
        //                                         ^
        //                                         |
        //                                   end of widget
        //
        ret = ProcessEvent(ievent, TraverseInfo, eDoNotProcess | ProcessEventInfo);
    }
    else
    {
        ret = ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    }
    return ret;
}

void RangeValuePropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, 
                                          RowHeader* row, const std::vector<header2>& column_vector, Color ItemBackgroundColor)
{
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

void RangeValuePropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector)
{
    if(m_ItemGeometryVector.size() >= 2)
    {
        Geometry geo;
        geo = m_ItemGeometryVector[1];
        geo = geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        setGeometry(geo);
    }
}

int RangeValuePropertyItem::GetItemBestHeight()
{
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
}

RangeValuePropertyItem* RangeValuePropertyItem::CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const char* Name, int id)
{
    double value = 0;
    double minvalue = 0;
    double maxvalue = 100;
    //double step = 1;
    QueryNodeXMLDoubleAttribute(elementxml, TEXT("Value"),       &value,     id);
    QueryNodeXMLDoubleAttribute(elementxml, TEXT("MinValue"),    &minvalue,  id);
    QueryNodeXMLDoubleAttribute(elementxml, TEXT("MaxValue"),    &maxvalue,  id);
    //QueryNodeXMLIntAttribute(elementxml, "Step",        &step,      id);

    RangeValuePropertyItem *node = new RangeValuePropertyItem(Name, value, minvalue, maxvalue);
    node->SetID(id);
    return node;
}

TiXmlElement* RangeValuePropertyItem::ToXML() const
{
    TiXmlElement* elementxml = NodeNetCom::ToXML();
    elementxml->SetDoubleAttribute(TEXT("Value"), GetValue());
    //elementxml->SetAttribute("Step", GetStep());
    elementxml->SetDoubleAttribute(TEXT("MinValue"), GetMinValue());
    elementxml->SetDoubleAttribute(TEXT("MaxValue"), GetMaxValue());
    return elementxml;
}

bool RangeValuePropertyItem::FromXML(const TiXmlElement* elementxml)
{
    double value = 0;
    double minvalue = 0;
    double maxvalue = 1.0;
    //double step = 1;
    QueryNodeXMLDoubleAttribute(elementxml, TEXT("Value"),       &value,     GetID());
    QueryNodeXMLDoubleAttribute(elementxml, TEXT("MinValue"),    &minvalue,  GetID());
    QueryNodeXMLDoubleAttribute(elementxml, TEXT("MaxValue"),    &maxvalue,  GetID());
    //QueryNodeXMLIntAttribute(elementxml, "Step",        &step,      GetID());
    SetRange(minvalue, maxvalue);
    SetValue(value);
    return NodeNetCom::FromXML(elementxml);
}
NAMESPACE_END_GUI