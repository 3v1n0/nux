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

#include "EditTextLinePropertyItem.h"
#include "Vector4Property.h"

NAMESPACE_BEGIN_GUI

Vector4PropertyItem::Vector4PropertyItem(const TCHAR* name, 
                                         double X,
                                         double Y,
                                         double Z,
                                         double W,
                                         const TCHAR* XName,
                                         const TCHAR* YName,
                                         const TCHAR* ZName,
                                         const TCHAR* WName)
                                         :   SectionProperty(name, NODE_TYPE_VECTOR4)
{
    m_X = new EditTextLinePropertyItem(XName);
    m_Y = new EditTextLinePropertyItem(YName);
    m_Z = new EditTextLinePropertyItem(ZName);
    m_W = new EditTextLinePropertyItem(WName);

    DoubleValidator validator;
    m_X->SetValidator(&validator);
    m_Y->SetValidator(&validator);
    m_Z->SetValidator(&validator);
    m_W->SetValidator(&validator);

    m_X->SetText(inlPrintf("%.3f", X));
    m_Y->SetText(inlPrintf("%.3f", Y));
    m_Z->SetText(inlPrintf("%.3f", Z));
    m_W->SetText(inlPrintf("%.3f", W));

    m_X->SetTextColor(GPropertyItemTextColor1);
    m_Y->SetTextColor(GPropertyItemTextColor1);
    m_Z->SetTextColor(GPropertyItemTextColor1);
    m_W->SetTextColor(GPropertyItemTextColor1);

    PushChildBack(m_X);
    PushChildBack(m_Y);
    PushChildBack(m_Z);
    PushChildBack(m_W);

    NODE_SIG_CONNECT(m_X->sigValidateEntry, Vector4PropertyItem, RecvPropertyChange);
    NODE_SIG_CONNECT(m_Y->sigValidateEntry, Vector4PropertyItem, RecvPropertyChange);
    NODE_SIG_CONNECT(m_Z->sigValidateEntry, Vector4PropertyItem, RecvPropertyChange);
    NODE_SIG_CONNECT(m_W->sigValidateEntry, Vector4PropertyItem, RecvPropertyChange);
}

Vector4PropertyItem::~Vector4PropertyItem()
{
    NUX_SAFE_DELETE(m_X);
    NUX_SAFE_DELETE(m_Y);
    NUX_SAFE_DELETE(m_Z);
    NUX_SAFE_DELETE(m_W);
}

long Vector4PropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    //ret = ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    return ret;
}

void Vector4PropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry ItemGeo, const BasePainter& Painter, 
                                       RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
{
    bool isSelected = (this == table->GetSelectedItem());
    if(isDirtyItem() ||
        m_X->IsRedrawNeeded() ||
        m_Y->IsRedrawNeeded() ||
        m_Z->IsRedrawNeeded())
    {
        UINT nBackground = table->PushItemBackground(GfxContext, this);
        Painter.PaintTextLineStatic(GfxContext, GFontBold /*GetFont()*/, m_FirstColumnUsableGeometry, row->m_item->GetName(), GetItemTextColor()); 

        if(m_ItemGeometryVector.size() >= 2)
        {
            Geometry prop_geo = m_ItemGeometryVector[1];
            prop_geo = prop_geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);

            TCHAR buffer[256];
            Snprintf(buffer, 256, 256-1, TEXT(" [ %s, %s, %s, %s ] "), m_X->GetText(), m_Y->GetText(), m_Z->GetText(), m_W->GetText());
            if(isSelected && table->GetSelectedColumn() == 1)
                Painter.Paint2DQuadColor(GfxContext, prop_geo, table->GetSelectionColor());
            else
                Painter.Paint2DQuadColor(GfxContext, prop_geo, ItemBackgroundColor);
            Painter.PaintTextLineStatic(GfxContext, GFontBold /*GetFont()*/, prop_geo, buffer, GetItemTextColor());
        }
        table->PopItemBackground(GfxContext, nBackground);
    }
}

void Vector4PropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector)
{
}

double Vector4PropertyItem::GetX() const 
{
    double ret;
    ret = CharToDouble(m_X->GetText());
    return ret;
}

double Vector4PropertyItem::GetY() const 
{
    double ret;
    ret = CharToDouble(m_Y->GetText());
    return ret;
}

double Vector4PropertyItem::GetZ() const 
{
    double ret;
    ret = CharToDouble(m_Z->GetText());
    return ret;
}

double Vector4PropertyItem::GetW() const 
{
    double ret;
    ret = CharToDouble(m_W->GetText());
    return ret;
}

Vector4PropertyItem* Vector4PropertyItem::CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const char* Name, int id)
{
    double X, Y, Z, W;
    tstring NameX, NameY, NameZ, NameW;
    const TiXmlElement* childxml = elementxml->FirstChildElement();
    QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameX, -1);
    QueryNodeXMLDoubleAttribute(childxml, TEXT("X"),       &X,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameY, -1);
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Y"),       &Y,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameZ, -1);
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Z"),       &Z,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameW, -1);
    QueryNodeXMLDoubleAttribute(childxml, TEXT("W"),       &W,     -1);
    childxml = childxml->NextSiblingElement();

    if(NameX.size() == 0) NameX = TEXT("X");
    if(NameY.size() == 0) NameY = TEXT("Y");
    if(NameZ.size() == 0) NameZ = TEXT("Z");
    if(NameW.size() == 0) NameW = TEXT("W");
    Vector4PropertyItem *node = new Vector4PropertyItem(Name, X, Y, Z, W, NameX.c_str(), NameY.c_str(), NameZ.c_str(), NameW.c_str());

    //    node->SetX(X);
    //    node->SetY(Y);
    //    node->SetZ(Z);
    //    node->SetW(W);
    node->SetID(id);
    return node;
}

TiXmlElement* Vector4PropertyItem::ToXML() const
{
    TiXmlElement* elementxml = NodeNetCom::ToXML();
    TiXmlElement* childxml = new TiXmlElement(TEXT("VecX"));
    childxml->SetAttribute(TEXT("Name"), GetXLabel());
    childxml->SetDoubleAttribute(TEXT("X"), GetX());
    elementxml->LinkEndChild(childxml);
    childxml = new TiXmlElement(TEXT("VecY"));
    childxml->SetAttribute(TEXT("Name"), GetYLabel());
    childxml->SetDoubleAttribute(TEXT("Y"), GetY());
    elementxml->LinkEndChild(childxml);
    childxml = new TiXmlElement(TEXT("VecZ"));
    childxml->SetAttribute(TEXT("Name"), GetZLabel());
    childxml->SetDoubleAttribute(TEXT("Z"), GetZ());
    elementxml->LinkEndChild(childxml);
    childxml = new TiXmlElement(TEXT("VecW"));
    childxml->SetAttribute(TEXT("Name"), GetWLabel());
    childxml->SetDoubleAttribute(TEXT("W"), GetW());
    elementxml->LinkEndChild(childxml);

    return elementxml;
}

bool Vector4PropertyItem::FromXML(const TiXmlElement* elementxml)
{
    double X, Y, Z, W;
    tstring NameX, NameY, NameZ, NameW;
    const TiXmlElement* childxml = elementxml->FirstChildElement();
    QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameX, -1);
    QueryNodeXMLDoubleAttribute(childxml, TEXT("X"),       &X,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameY, -1);
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Y"),       &Y,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameZ, -1);
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Z"),       &Z,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameW, -1);
    QueryNodeXMLDoubleAttribute(childxml, TEXT("W"),       &W,     -1);
    childxml = childxml->NextSiblingElement();

    SetX(X);
    SetY(Y);
    SetZ(Z);
    SetW(W);

    return NodeNetCom::FromXML(elementxml);
}
NAMESPACE_END_GUI
