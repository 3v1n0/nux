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

#include "ValuatorInt.h"
#include "IntegerValuatorPropertyItem.h"

NAMESPACE_BEGIN_GUI

IntValuatorPropertyItem::IntValuatorPropertyItem(const TCHAR* name, int Value, int Step, int MinValue, int MaxValue)
:   SectionProperty(name, NODE_TYPE_INTVALUATOR)
{
    SetRange(MinValue, MaxValue);
    SetValue(Value);
    SetStep(Step);
    NODE_SIG_CONNECT(sigValueChanged, IntValuatorPropertyItem, RecvPropertyChange);
}

IntValuatorPropertyItem::~IntValuatorPropertyItem()
{

}

long IntValuatorPropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
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

void IntValuatorPropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, 
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

void IntValuatorPropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector)
{
    if(m_ItemGeometryVector.size() >= 2)
    {
        Geometry geo;
        geo = m_ItemGeometryVector[1];
        geo = geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        setGeometry(geo);
    }
}

int IntValuatorPropertyItem::GetItemBestHeight()
{
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
}

IntValuatorPropertyItem* IntValuatorPropertyItem::CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const char* Name, int id)
{
    int value = 0;
    int minvalue = 0;
    int maxvalue = 100;
    int step = 1;
    QueryNodeXMLIntAttribute(elementxml, TEXT("Value"),       &value,     id);
    QueryNodeXMLIntAttribute(elementxml, TEXT("Step"),        &step,     id);
    QueryNodeXMLIntAttribute(elementxml, TEXT("MinValue"),    &minvalue,  id);
    QueryNodeXMLIntAttribute(elementxml, TEXT("MaxValue"),    &maxvalue,  id);
    //QueryNodeXMLIntAttribute(elementxml, "Step",        &step,      id);

    IntValuatorPropertyItem *node = new IntValuatorPropertyItem(Name, value, step, minvalue, maxvalue);
    node->SetID(id);
    return node;
}

TiXmlElement* IntValuatorPropertyItem::ToXML() const
{
    TiXmlElement* elementxml = NodeNetCom::ToXML();
    elementxml->SetAttribute(TEXT("Value"),     GetValue());
    elementxml->SetAttribute(TEXT("Step"),      GetStep());
    elementxml->SetAttribute(TEXT("MinValue"),  GetMinValue());
    elementxml->SetAttribute(TEXT("MaxValue"),  GetMaxValue());
    return elementxml;
}

bool IntValuatorPropertyItem::FromXML(const TiXmlElement* elementxml)
{
    int value = 0;
    int minvalue = 0;
    int maxvalue = 100;
    int step = 1;
    QueryNodeXMLIntAttribute(elementxml, TEXT("Value"),       &value,     GetID());
    QueryNodeXMLIntAttribute(elementxml, TEXT("Step"),        &step,      GetID());
    QueryNodeXMLIntAttribute(elementxml, TEXT("MinValue"),    &minvalue,  GetID());
    QueryNodeXMLIntAttribute(elementxml, TEXT("MaxValue"),    &maxvalue,  GetID());
    SetRange(minvalue, maxvalue);
    SetValue(value);
    SetStep(step);
    return NodeNetCom::FromXML(elementxml);
}
NAMESPACE_END_GUI
