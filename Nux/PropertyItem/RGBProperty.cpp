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
#include "ColorGradientPropertyItem.h"
#include "RGBProperty.h"

NAMESPACE_BEGIN_GUI

RGBPropertyItem::RGBPropertyItem(const TCHAR* name, float red /* = 1.0f*/, float green /* = 1.0f*/, float blue /* = 1.0f*/)
:   SectionProperty(name, NODE_TYPE_RGB)
,   m_color_model(CM_RGB)
,   m_color_format(Color::COLORFORMAT_FLOAT)
{
    m_red = new ColorGradientPropertyItem(TEXT("Red"));
    m_green = new ColorGradientPropertyItem(TEXT("Green"));
    m_blue = new ColorGradientPropertyItem(TEXT("Blue"));

    m_ColorModel = smptr(Button)(new Button(TEXT("RGB")));
    m_ColorModel->SetMinMaxSize(32, 14);
    m_ColorModel->SetFont(GFontBold);
    m_ColorFormat = smptr(Button)(new Button(TEXT("float")));
    m_ColorFormat->SetMinMaxSize(32, 14);
    m_ColorFormat->SetFont(GFontBold);


    PushChildBack(m_red);
    PushChildBack(m_green);
    PushChildBack(m_blue);

    float r = Clamp(red,   0.0f, 1.0f);
    float g = Clamp(green, 0.0f, 1.0f);
    float b = Clamp(blue,  0.0f, 1.0f);

    m_red->SetRange(0.0f, 1.0f);
    m_red->SetValue(r);
    m_green->SetRange(0.0f, 1.0f);
    m_green->SetValue(g);
    m_blue->SetRange(0.0f, 1.0f);
    m_blue->SetValue(b);

    m_red->SetColorFormat(m_color_format);
    m_green->SetColorFormat(m_color_format);
    m_blue->SetColorFormat(m_color_format);

    m_red->AddColorMark(0, Color(0.0f, g, b), false);
    m_red->AddColorMark(1, Color(1.0f, g, b), false);
    m_red->AddColorMark(0, Color(0.0f, g, b), false);
    m_red->AddColorMark(1, Color(1.0f, g, b), false);
    m_green->AddColorMark(0, Color(r, 0.0f, b), false);
    m_green->AddColorMark(1, Color(r, 1.0f, b), false);
    m_blue->AddColorMark(0, Color(r, g, 0.0f), false);
    m_blue->AddColorMark(1, Color(r, g, 1.0f), false);

    UpdateStartToEndColors();

    m_red->sigValueChanged.connect( sigc::mem_fun(this, &RGBPropertyItem::RedChange));
    m_green->sigValueChanged.connect( sigc::mem_fun(this, &RGBPropertyItem::GreenChange));
    m_blue->sigValueChanged.connect( sigc::mem_fun(this, &RGBPropertyItem::BlueChange));

    m_ColorModel->sigClick.connect(sigc::mem_fun(this, &RGBPropertyItem::OnChangeColorModel));
    m_ColorFormat->sigClick.connect(sigc::mem_fun(this, &RGBPropertyItem::OnChangeColorFormat));

    NODE_SIG_CONNECT(m_red->sigValueChanged, RGBPropertyItem, RecvPropertyChange);
    NODE_SIG_CONNECT(m_green->sigValueChanged, RGBPropertyItem, RecvPropertyChange);
    NODE_SIG_CONNECT(m_blue->sigValueChanged, RGBPropertyItem, RecvPropertyChange);
}

RGBPropertyItem::~RGBPropertyItem()
{
    NUX_SAFE_DELETE(m_red);
    NUX_SAFE_DELETE(m_green);
    NUX_SAFE_DELETE(m_blue);
}

long RGBPropertyItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    ret = m_ColorModel->ProcessEvent(ievent, ret, ProcessEventInfo);
    ret = m_ColorFormat->ProcessEvent(ievent, ret, ProcessEventInfo);
    //     if(!(ret & eMouseEventSolved))
    //         ret = TableItem::ProcessPropertyEvent(ievent, TraverseInfo, ProcessEventInfo);
    return ret;
}

void RGBPropertyItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector)
{
    if(m_ItemGeometryVector.size() >= 2)
    {
        Geometry geo;
        geo = m_ItemGeometryVector[1];
        geo = geo.GetExpand(-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);

        m_ColorModel->SetBaseX(geo.x + geo.GetWidth() - PROPERTY_BORDER_X - m_ColorModel->GetBaseWidth() - 2 - m_ColorModel->GetBaseWidth());
        m_ColorModel->SetBaseY(geo.y);

        m_ColorFormat->SetBaseX(geo.x + geo.GetWidth() - PROPERTY_BORDER_X - m_ColorModel->GetBaseWidth());
        m_ColorFormat->SetBaseY(geo.y);
    }

}

int RGBPropertyItem::GetItemBestHeight()
{
    int sz = 16; //m_ColorModel->GetBaseHeight();
    return sz + 2 * PROPERTY_BORDER_Y;
}

void RGBPropertyItem::RedChange(const weaksmptr(ColorGradient) slider)
{
    if(slider->IsCtrlKeyPressed())
    {
        m_green->SetValue(slider->GetValue());
        m_blue->SetValue(slider->GetValue());
    }
    UpdateStartToEndColors();
    m_green->NeedRedraw();
    m_blue->NeedRedraw();
}

void RGBPropertyItem::GreenChange(const weaksmptr(ColorGradient) slider)
{
    if(slider->IsCtrlKeyPressed())
    {
        m_red->SetValue(slider->GetValue());
        m_blue->SetValue(slider->GetValue());
    }
    UpdateStartToEndColors();
    m_red->NeedRedraw();
    m_blue->NeedRedraw();
}

void RGBPropertyItem::BlueChange(const weaksmptr(ColorGradient) slider)
{
    if(slider->IsCtrlKeyPressed())
    {
        m_red->SetValue(slider->GetValue());
        m_green->SetValue(slider->GetValue());
    }
    UpdateStartToEndColors();
    m_red->NeedRedraw();
    m_green->NeedRedraw();
}

void RGBPropertyItem::AlphaChange(const weaksmptr(ColorGradient) slider)
{
    UpdateStartToEndColors();

    m_red->NeedRedraw();
    m_green->NeedRedraw();
    m_blue->NeedRedraw();
}

void RGBPropertyItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, 
                                    RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
{
    if(isDirtyItem() ||
        m_red->IsRedrawNeeded() ||
        m_green->IsRedrawNeeded() ||
        m_blue->IsRedrawNeeded())
    {
        UINT nBackground = table->PushItemBackground(GfxContext, this);
        Painter.PaintTextLineStatic(GfxContext, GFontBold /*GetFont()*/, m_FirstColumnUsableGeometry, row->m_item->GetName(), GetItemTextColor()); 

        if(m_ItemGeometryVector.size() >= 2)
        {
            Geometry prop_geo = m_ItemGeometryVector[1];
            prop_geo = prop_geo.GetExpand(-PROPERTY_BORDER_X,
                -2*PROPERTY_BORDER_Y);
            prop_geo.OffsetSize( - 2 - m_ColorFormat->GetBaseWidth() - 2 - m_ColorModel->GetBaseWidth() - 2, 0);

            // Draw the resulting color
            Painter.Paint2DQuadColor(GfxContext, prop_geo, Color(m_Red, m_Green, m_Blue)); 
            // Draw black border around the color
            Painter.Paint2DQuadWireframe(GfxContext, prop_geo, Color(0));
            m_ColorModel->ProcessDraw(GfxContext, true);
            m_ColorFormat->ProcessDraw(GfxContext, true);
        }
        table->PopItemBackground(GfxContext, nBackground);
    }
}

void RGBPropertyItem::OnChangeColorModel()
{
    if(m_color_model == CM_RGB)
    {
        SetColorModel(CM_HLS);
        float H, L, S;
        RGBtoHLS(m_red->GetValue(), m_green->GetValue(), m_blue->GetValue(), H, L, S);
        m_red->SetValue(H); m_green->SetValue(L); m_blue->SetValue(S);
    }
    else if(m_color_model == CM_HLS)
    {
        SetColorModel(CM_HSV);
        float H, S, V;
        float R, G, B;
        HLStoRGB(R, G, B, m_red->GetValue(), m_green->GetValue(), m_blue->GetValue());
        RGBtoHSV(R, G, B, H, S, V);
        m_red->SetValue(H); m_green->SetValue(S); m_blue->SetValue(V);

        if(H == -1.0f)
        {
            H = 0;
        }
    }
    else if(m_color_model == CM_HSV)
    {
        SetColorModel(CM_RGB);
        float R, G, B;
        HSVtoRGB(R, G, B, m_red->GetValue(), m_green->GetValue(), m_blue->GetValue());
        m_red->SetValue(R); m_green->SetValue(G); m_blue->SetValue(B);

    }

    UpdateStartToEndColors();
    m_green->NeedRedraw();
    m_blue->NeedRedraw();
}

void RGBPropertyItem::SetColorModel(eColorModel cm) 
{
    if(cm == CM_RGB)
    {
        m_color_model = CM_RGB;
        m_ColorModel->SetCaption(TEXT("RGB"));

        m_red->SetName(TEXT("Red"));
        m_green->SetName(TEXT("Green"));
        m_blue->SetName(TEXT("Blue"));
    }
    if(cm == CM_HSV)
    {
        m_color_model = CM_HSV;
        m_ColorModel->SetCaption(TEXT("HSV"));

        m_red->SetName(TEXT("Hue"));
        m_green->SetName(TEXT("Saturation"));
        m_blue->SetName(TEXT("Value"));
    }
    if(cm == CM_HLS)
    {
        m_color_model = CM_HLS;
        m_ColorModel->SetCaption(TEXT("HLS"));

        m_red->SetName(TEXT("Hue"));
        m_green->SetName(TEXT("Light"));
        m_blue->SetName(TEXT("Saturation"));
    }
    if(cm == CM_YUV)
    {
        m_color_model = CM_YUV;
        m_ColorModel->SetBaseString(TEXT("YUV"));

        //         m_ComponentLabel0->SetBaseString(TEXT("Y"));
        //         m_ComponentLabel1->SetBaseString(TEXT("U"));
        //         m_ComponentLabel2->SetBaseString(TEXT("V"));
        //         m_ComponentAlpha->SetBaseString(TEXT("A"));
    }
}


void RGBPropertyItem::OnChangeColorFormat()
{
    if(m_color_format == Color::COLORFORMAT_FLOAT)
    {
        m_color_format = Color::COLORFORMAT_INT;
        m_ColorFormat->SetCaption(TEXT("int"));
    }
    else if(m_color_format == Color::COLORFORMAT_INT)
    {
        m_color_format = Color::COLORFORMAT_HEX;
        m_ColorFormat->SetCaption(TEXT("hex"));
    }
    else if(m_color_format == Color::COLORFORMAT_HEX)
    {
        m_color_format = Color::COLORFORMAT_FLOAT;
        m_ColorFormat->SetCaption(TEXT("float"));
    }

    m_red->SetColorFormat(m_color_format);
    m_green->SetColorFormat(m_color_format);
    m_blue->SetColorFormat(m_color_format);
}

void RGBPropertyItem::UpdateStartToEndColors()
{
    m_red->Reset();
    m_green->Reset();
    m_blue->Reset();

    if(m_color_model == CM_RGB)
    {
        float r, g, b;
        r = m_red->GetValue();
        g = m_green->GetValue();
        b = m_blue->GetValue();

        m_red->AddColorMark(0, Color(0.0f, g, b), false);
        m_red->AddColorMark(1, Color(1.0f, g, b), false);
        m_green->AddColorMark(0, Color(r, 0.0f, b), false);
        m_green->AddColorMark(1, Color(r, 1.0f, b), false);
        m_blue->AddColorMark(0, Color(r, g, 0.0f), false);
        m_blue->AddColorMark(1, Color(r, g, 1.0f), false);

        m_Red = r;
        m_Green = g;
        m_Blue = b;
    }

    if(m_color_model == CM_HSV)
    {
        float r, g, b;
        float h, s, v;
        h = m_red->GetValue();
        s = m_green->GetValue();
        v = m_blue->GetValue();

        HSVtoRGB(r, g, b, h, 1.0f, 1.0f);

        m_red->AddColorMark(0.0f, Color(1.0f, 0.0, 0.0), false);
        m_red->AddColorMark(1.0f/6.0f, Color(1.0f, 1.0, 0.0), false);
        m_red->AddColorMark(2.0f/6.0f, Color(0.0f, 1.0, 0.0), false);
        m_red->AddColorMark(3.0f/6.0f, Color(0.0f, 1.0, 1.0), false);
        m_red->AddColorMark(4.0f/6.0f, Color(0.0f, 0.0, 1.0), false);
        m_red->AddColorMark(5.0f/6.0f, Color(1.0f, 0.0, 1.0), false);
        m_red->AddColorMark(1.0f, Color(1.0f, 0.0, 0.0), false);

        if(h == 1.0f)
            h = 0.0f;
        HSVtoRGB(r, g, b, h, 1.0f, 1.0f);
        m_green->AddColorMark(0, Color(v, v, v), false);
        m_green->AddColorMark(1.0f, Color(r * v, g * v, b * v), false);

        HSVtoRGB(r, g, b, h, s, 1.0f);
        m_blue->AddColorMark(0, Color(0, 0, 0), false);
        m_blue->AddColorMark(1.0f, Color(r, g, b), false);

        HSVtoRGB(m_Red, m_Green, m_Blue, h, s, v);
    }

    if(m_color_model == CM_HLS)
    {
        float r, g, b;
        float h, l, s;
        h = m_red->GetValue();
        l = m_green->GetValue();
        s = m_blue->GetValue();

        m_red->AddColorMark(0.0f, Color(1.0f, 0.0, 0.0), false);
        m_red->AddColorMark(1.0f/6.0f, Color(1.0f, 1.0, 0.0), false);
        m_red->AddColorMark(2.0f/6.0f, Color(0.0f, 1.0, 0.0), false);
        m_red->AddColorMark(3.0f/6.0f, Color(0.0f, 1.0, 1.0), false);
        m_red->AddColorMark(4.0f/6.0f, Color(0.0f, 0.0, 1.0), false);
        m_red->AddColorMark(5.0f/6.0f, Color(1.0f, 0.0, 1.0), false);
        m_red->AddColorMark(1.0f, Color(1.0f, 0.0, 0.0), false);

        s = 1.0f - s;
        if(h == 1.0f)
            h = 0.0f;

        // Need to use HSVtoRGB to compute the primary color
        HSVtoRGB(r, g, b, h, 1.0f, 1.0f);
        m_green->AddColorMark(0.0f, Color(0, 0, 0), false);
        m_green->AddColorMark(0.5f, Color(r*(1-s)+0.5f*s, g*(1-s)+0.5f*s, b*(1-s)+0.5f*s), false);
        m_green->AddColorMark(1.0f, Color(1.0f, 1.0f, 1.0f), false);

        float cr, cg, cb;
        if(l > 0.5)
        {
            float factor = (l-0.5f)/0.5f;
            cr = (1 - factor) * r*(1-s)+0.5*s + factor * 1.0f;
            cg = (1 - factor) * g*(1-s)+0.5*s + factor * 1.0f;
            cb = (1 - factor) * b*(1-s)+0.5*s + factor * 1.0f;
        }
        else
        {
            float factor = l/0.5f;
            cr = (factor) * r*(1-s)+0.5*s;
            cg = (factor) * g*(1-s)+0.5*s;
            cb = (factor) * b*(1-s)+0.5*s;
        }

        m_blue->AddColorMark(0, Color(l, l, l), false);
        m_blue->AddColorMark(1.0f, Color(cr, cg, cb), false);

        HLStoRGB(m_Red, m_Green, m_Blue, h, l, 1.0f - s);
    }
}

RGBPropertyItem* RGBPropertyItem::CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const char* Name, int id)
{
    RGBPropertyItem *node = new RGBPropertyItem(Name, 0, 0, 0);
    double red, green, blue;

    const TiXmlElement* childxml = elementxml->FirstChildElement();
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Red"),        &red,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Green"),      &green,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Blue"),       &blue,     -1);

    node->SetRed(red);
    node->SetGreen(green);
    node->SetBlue(blue);

    node->SetID(id);
    return node;
}

TiXmlElement* RGBPropertyItem::ToXML() const
{
    TiXmlElement* elementxml = NodeNetCom::ToXML();
    TiXmlElement* childxml;
    childxml = new TiXmlElement(TEXT("RGBComponent"));
    //childxml->SetAttribute(TEXT("Name"), m_X->GetName());
    childxml->SetDoubleAttribute(TEXT("Red"), (double)m_Red);
    elementxml->LinkEndChild(childxml);
    childxml = new TiXmlElement(TEXT("RGBComponent"));
    //childxml->SetAttribute(TEXT("Name"), m_Y->GetName());
    childxml->SetDoubleAttribute(TEXT("Green"), (double)m_Green);
    elementxml->LinkEndChild(childxml);
    childxml = new TiXmlElement(TEXT("RGBComponent"));
    //childxml->SetAttribute(TEXT("Name"), m_Z->GetName());
    childxml->SetDoubleAttribute(TEXT("Blue"), (double)m_Blue);
    elementxml->LinkEndChild(childxml);

    return elementxml;
}

bool RGBPropertyItem::FromXML(const TiXmlElement* elementxml)
{
    double red, green, blue;
    tstring NameX, NameY, NameZ, NameW;
    const TiXmlElement* childxml;
    childxml = elementxml->FirstChildElement();
    //QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameX, GetID());
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Red"),       &red,     -1);
    childxml = childxml->NextSiblingElement();
    //QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameY, GetID());
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Green"),       &green,     -1);
    childxml = childxml->NextSiblingElement();
    //QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameZ, GetID());
    QueryNodeXMLDoubleAttribute(childxml, TEXT("Blue"),       &blue,     -1);

    SetRed(red);
    SetGreen(green);
    SetBlue(blue);

    return NodeNetCom::FromXML(elementxml);
}

NAMESPACE_END_GUI
