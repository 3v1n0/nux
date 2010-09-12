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

#include "Matrix4Preview.h"
#include "Matrix4PreviewPropertyItem.h"

NAMESPACE_BEGIN_GUI

Matrix4PreviewPropertyItem::Matrix4PreviewPropertyItem(const TCHAR* name, Matrix4 matrix)
:   SectionProperty(name, NODE_TYPE_MATRIX4PREVIEW)
,   Matrix4Preview(matrix)
{
    NODE_SIG_CONNECT(sigMatrixChanged, Matrix4PreviewPropertyItem, RecvPropertyChange);
}

Matrix4PreviewPropertyItem::~Matrix4PreviewPropertyItem()
{

}

long Matrix4PreviewPropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    Geometry geo = m_ItemGeometryVector[1];
    if((ievent.e_event == NUX_MOUSE_PRESSED) && geo.IsPointInside(ievent.e_x, ievent.e_y) == false)
    {
        ret = ProcessEvent(ievent, TraverseInfo, eDoNotProcess | ProcessEventInfo);
    }
    else
    {
        ret = ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    }
    return ret;
}

void Matrix4PreviewPropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, 
                                              RowHeader* row, const std::vector<header2>& column_vector, Color ItemBackgroundColor)
{
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

void Matrix4PreviewPropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector)
{
    if(m_ItemGeometryVector.size() >= 2)
    {
        Geometry geo;
        geo = m_ItemGeometryVector[1];
        geo = geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        setGeometry(geo);
    }
}

int Matrix4PreviewPropertyItem::GetItemBestHeight()
{
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
}

Matrix4PreviewPropertyItem* Matrix4PreviewPropertyItem::CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const char* Name, int id)
{
    Matrix4PreviewPropertyItem *node = new Matrix4PreviewPropertyItem(Name);

    Matrix4 matrix;
    const TiXmlElement* childxml = elementxml->FirstChildElement();
    double d;
    for(int i = 0; i < 4; i++)
    {
        if(childxml == 0)
            break;
        TCHAR m[6];
        for(int j = 0; j < 4; j++)
        {
            Snprintf(m, 6, 6-1, TEXT("m%d%d"), i, j);
            QueryNodeXMLDoubleAttribute(childxml, m, &d, -1);
            matrix(i, j) = d;
        }
        childxml = childxml->NextSiblingElement();
    }
    node->SetMatrix(matrix);
    return node;
}

TiXmlElement* Matrix4PreviewPropertyItem::ToXML() const
{
    TiXmlElement* elementxml = NodeNetCom::ToXML();

    for(int i = 0; i < 4; i++)
    {
        TCHAR row[6];
        TCHAR m[6];
        Snprintf(row, 6, 6-1, TEXT("row%d"), i);
        TiXmlElement* childxml = new TiXmlElement(row);
        for(int j = 0; j < 4; j++)
        {
            Snprintf(m, 6, 6-1, TEXT("m%d%d"), i, j);
            childxml->SetDoubleAttribute(m, GetMatrix()(i, j));
        }
        childxml = childxml->NextSiblingElement();
    }
    return elementxml;
}

bool Matrix4PreviewPropertyItem::FromXML(const TiXmlElement* elementxml)
{
    Matrix4 matrix;
    const TiXmlElement* childxml = elementxml->FirstChildElement();
    double d;

    for(int i = 0; i < 4; i++)
    {
        if(childxml == 0)
            break;
        TCHAR m[6];
        for(int j = 0; j < 4; j++)
        {
            Snprintf(m, 6, 6-1, TEXT("m%d%d"), i, j);
            QueryNodeXMLDoubleAttribute(childxml, m, &d, -1);
            matrix(i, j) = d;
        }
        childxml = childxml->NextSiblingElement();
    }
    SetMatrix(matrix);
    return NodeNetCom::FromXML(elementxml);
}

NAMESPACE_END_GUI
