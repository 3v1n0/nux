/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "PropertyList.h"
#include "ColorGradientPropertyItem.h"
#include "RGBAProperty.h"

namespace nux
{

  RGBAPropertyItem::RGBAPropertyItem (const TCHAR *name, float red /* = 1.0f*/, float green /* = 1.0f*/, float blue /* = 1.0f*/, float alpha /* = 1.0f*/)
    : SectionProperty(name, NODE_TYPE_RGBA)
    , color_(red, green, blue, alpha)
    , color_model_(color::RGB)
    , color_format_(color::FLOAT)
  {
    m_red = new ColorGradientPropertyItem (TEXT ("Red") );
    m_green = new ColorGradientPropertyItem (TEXT ("Green") );
    m_blue = new ColorGradientPropertyItem (TEXT ("Blue") );
    m_alpha = new ColorGradientPropertyItem (TEXT ("Alpha") );

    m_ColorModel = new ToggleButton ("RGB", NUX_TRACKER_LOCATION);
    m_ColorModel->SetMinMaxSize (32, 14);
    m_ColorModel->SetFont (GetSysBoldFont () );
    m_ColorFormat = new ToggleButton ("float", NUX_TRACKER_LOCATION);
    m_ColorFormat->SetMinMaxSize (32, 14);
    m_ColorFormat->SetFont (GetSysBoldFont () );

    PushChildBack (m_red);
    PushChildBack (m_green);
    PushChildBack (m_blue);
    PushChildBack (m_alpha);

    float r = Clamp (red,   0.0f, 1.0f);
    float g = Clamp (green, 0.0f, 1.0f);
    float b = Clamp (blue,  0.0f, 1.0f);
    float a = Clamp (alpha, 0.0f, 1.0f);

    m_red->SetColorFormat (color_format_);
    m_green->SetColorFormat (color_format_);
    m_blue->SetColorFormat (color_format_);
    m_alpha->SetColorFormat (color_format_);

    m_red->SetRange (0.0f, 1.0f);
    m_red->SetValue (r);
    m_green->SetRange (0.0f, 1.0f);
    m_green->SetValue (g);
    m_blue->SetRange (0.0f, 1.0f);
    m_blue->SetValue (b);
    m_alpha->SetRange (0.0f, 1.0f);
    m_alpha->SetValue (a);

    m_red->AddColorMark (0, Color (0.0f, g, b), false);
    m_red->AddColorMark (1, Color (1.0f, g, b), false);
    m_green->AddColorMark (0, Color (r, 0.0f, b), false);
    m_green->AddColorMark (1, Color (r, 1.0f, b), false);
    m_blue->AddColorMark (0, Color (r, g, 0.0f), false);
    m_blue->AddColorMark (1, Color (r, g, 1.0f), false);
    m_alpha->AddColorMark (0, Color (0xFF000000), false);
    m_alpha->AddColorMark (1, Color (0xFFFFFFFF), false);

    UpdateStartToEndColors();

    m_red->sigValueChanged.connect ( sigc::mem_fun (this, &RGBAPropertyItem::RedChange) );
    m_green->sigValueChanged.connect ( sigc::mem_fun (this, &RGBAPropertyItem::GreenChange) );
    m_blue->sigValueChanged.connect ( sigc::mem_fun (this, &RGBAPropertyItem::BlueChange) );
    m_alpha->sigValueChanged.connect ( sigc::mem_fun (this, &RGBAPropertyItem::AlphaChange) );

    //FIXME - m_ColorModel->sigClick.connect (sigc::mem_fun (this, &RGBAPropertyItem::OnChangeColorModel) );
    //FIXME - m_ColorFormat->sigClick.connect (sigc::mem_fun (this, &RGBAPropertyItem::OnChangeColorFormat) );

    NODE_SIG_CONNECT (m_red->sigValueChanged, RGBAPropertyItem, RecvPropertyChange);
    NODE_SIG_CONNECT (m_green->sigValueChanged, RGBAPropertyItem, RecvPropertyChange);
    NODE_SIG_CONNECT (m_blue->sigValueChanged, RGBAPropertyItem, RecvPropertyChange);
    NODE_SIG_CONNECT (m_alpha->sigValueChanged, RGBAPropertyItem, RecvPropertyChange);
  }

  RGBAPropertyItem::~RGBAPropertyItem()
  {
    m_ColorModel->Dispose();
    m_ColorFormat->Dispose();
    delete m_red;
    delete m_green;
    delete m_blue;
    delete m_alpha;
  }

  long RGBAPropertyItem::ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    ret = m_ColorModel->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_ColorFormat->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void RGBAPropertyItem::ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector)
  {
    if (m_ItemGeometryVector.size() >= 2)
    {
      Geometry geo;
      geo = m_ItemGeometryVector[1];
      geo = geo.GetExpand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);

      m_ColorModel->SetBaseX (geo.x + geo.GetWidth() - PROPERTY_BORDER_X - m_ColorModel->GetBaseWidth() - 2 - m_ColorModel->GetBaseWidth() );
      m_ColorModel->SetBaseY (geo.y);

      m_ColorFormat->SetBaseX (geo.x + geo.GetWidth() - PROPERTY_BORDER_X - m_ColorModel->GetBaseWidth() );
      m_ColorFormat->SetBaseY (geo.y);
    }

  }

  int RGBAPropertyItem::GetItemBestHeight()
  {
    int sz = 16; //m_ColorModel->GetBaseHeight();
    return sz + 2 * PROPERTY_BORDER_Y;
  }

  void RGBAPropertyItem::RedChange (ColorGradient *slider)
  {
    if (slider->IsCtrlKeyPressed() )
    {
      m_green->SetValue (slider->GetValue() );
      m_blue->SetValue (slider->GetValue() );
    }

    UpdateStartToEndColors();
    m_green->QueueDraw();
    m_blue->QueueDraw();
    m_alpha->QueueDraw();
  }

  void RGBAPropertyItem::GreenChange (ColorGradient *slider)
  {
    if (slider->IsCtrlKeyPressed() )
    {
      m_red->SetValue (slider->GetValue() );
      m_blue->SetValue (slider->GetValue() );
    }

    UpdateStartToEndColors();
    m_red->QueueDraw();
    m_blue->QueueDraw();
    m_alpha->QueueDraw();
  }

  void RGBAPropertyItem::BlueChange (ColorGradient *slider)
  {
    if (slider->IsCtrlKeyPressed() )
    {
      m_red->SetValue (slider->GetValue() );
      m_green->SetValue (slider->GetValue() );
    }

    UpdateStartToEndColors();
    m_red->QueueDraw();
    m_green->QueueDraw();
    m_alpha->QueueDraw();
  }

  void RGBAPropertyItem::AlphaChange (ColorGradient *slider)
  {
    UpdateStartToEndColors();

    m_red->QueueDraw();
    m_green->QueueDraw();
    m_blue->QueueDraw();
    m_alpha->QueueDraw();
  }

  void RGBAPropertyItem::DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter,
                                       RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
  {
    if (isDirtyItem() ||
        m_red->IsRedrawNeeded() ||
        m_green->IsRedrawNeeded() ||
        m_blue->IsRedrawNeeded() ||
        m_alpha->IsRedrawNeeded() )
    {
      t_u32 nBackground = table->PushItemBackground (GfxContext, this);
      Painter.PaintTextLineStatic (GfxContext, GetSysBoldFont() /*GetFont ()*/, m_FirstColumnUsableGeometry, row->_table_item->GetName(), GetItemTextColor() );

      if (m_ItemGeometryVector.size() >= 2)
      {
        Geometry prop_geo = m_ItemGeometryVector[1];
        prop_geo = prop_geo.GetExpand (-PROPERTY_BORDER_X,
                                       -2 * PROPERTY_BORDER_Y);
        prop_geo.OffsetSize ( - 2 - m_ColorFormat->GetBaseWidth() - 2 - m_ColorModel->GetBaseWidth() - 2, 0);

        // Draw the resulting color
        Painter.Paint2DQuadColor (GfxContext, prop_geo, MakeOpaque(color_));
        // Draw black border around the color
        Painter.Paint2DQuadWireframe (GfxContext, prop_geo, color::Black);
        m_ColorModel->ProcessDraw (GfxContext, true);
        m_ColorFormat->ProcessDraw (GfxContext, true);
      }

      table->PopItemBackground (GfxContext, nBackground);
    }
  }

  void RGBAPropertyItem::OnChangeColorModel()
  {
    if (color_model_ == color::RGB)
    {
      SetColorModel (color::HLS);
      color::HueLightnessSaturation hls(color_);
      SetColor(hls.hue, hls.lightness, hls.saturation, color_.alpha);
    }
    else if (color_model_ == color::HLS)
    {
      SetColorModel (color::HSV);
      color::HueSaturationValue hsv(color_);
      SetColor(hsv.hue, hsv.saturation, hsv.value, color_.alpha);
    }
    else if (color_model_ == color::HSV)
    {
      SetColorModel (color::RGB);
      SetColor(color_.red, color_.green, color_.blue, color_.alpha);
    }

    m_green->QueueDraw();
    m_blue->QueueDraw();
  }

  void RGBAPropertyItem::SetColorModel(color::Model cm)
  {
    if (cm == color::RGB)
    {
      color_model_ = color::RGB;
      //FIXME - m_ColorModel->SetCaption (TEXT ("RGB") );

      m_red->SetName (TEXT ("Red") );
      m_green->SetName (TEXT ("Green") );
      m_blue->SetName (TEXT ("Blue") );
      m_alpha->SetName (TEXT ("Alpha") );
    }

    if (cm == color::HSV)
    {
      color_model_ = color::HSV;
      //FIXME - m_ColorModel->SetCaption (TEXT ("HSV") );

      m_red->SetName (TEXT ("Hue") );
      m_green->SetName (TEXT ("Saturation") );
      m_blue->SetName (TEXT ("Value") );
      m_alpha->SetName (TEXT ("Alpha") );
    }

    if (cm == color::HLS)
    {
      color_model_ = color::HLS;
      //FIXME - m_ColorModel->SetCaption (TEXT ("HLS") );

      m_red->SetName (TEXT ("Hue") );
      m_green->SetName (TEXT ("Light") );
      m_blue->SetName (TEXT ("Saturation") );
      m_alpha->SetName (TEXT ("Alpha") );
    }

    if (cm == color::YUV)
    {
      color_model_ = color::YUV;
      //FIXME - m_ColorModel->SetBaseString (TEXT ("YUV") );

//         m_ComponentLabel0->SetBaseString(TEXT("Y"));
//         m_ComponentLabel1->SetBaseString(TEXT("U"));
//         m_ComponentLabel2->SetBaseString(TEXT("V"));
//         m_ComponentAlpha->SetBaseString(TEXT("A"));
    }
  }


  void RGBAPropertyItem::OnChangeColorFormat()
  {
    if (color_format_ == color::FLOAT)
    {
      color_format_ = color::INT;
      //FIXME - m_ColorFormat->SetCaption (TEXT ("int") );
    }
    else if (color_format_ == color::INT)
    {
      color_format_ = color::HEX;
      //FIXME - m_ColorFormat->SetCaption (TEXT ("hex") );
    }
    else if (color_format_ == color::HEX)
    {
      color_format_ = color::FLOAT;
      //FIXME - m_ColorFormat->SetCaption (TEXT ("float") );
    }

    m_red->SetColorFormat (color_format_);
    m_green->SetColorFormat (color_format_);
    m_blue->SetColorFormat (color_format_);
    m_alpha->SetColorFormat (color_format_);
  }

  void RGBAPropertyItem::UpdateStartToEndColors()
  {
    m_red->Reset();
    m_green->Reset();
    m_blue->Reset();

    if (color_model_ == color::RGB)
    {
      color_ = Color(m_red->GetValue(),
                     m_green->GetValue(),
                     m_blue->GetValue(),
                     m_alpha->GetValue());

      m_red->AddColorMark (0, Color (0.0f, color_.green, color_.blue), false);
      m_red->AddColorMark (1, Color (1.0f, color_.green, color_.blue), false);
      m_green->AddColorMark (0, Color (color_.red, 0.0f, color_.blue), false);
      m_green->AddColorMark (1, Color (color_.red, 1.0f, color_.blue), false);
      m_blue->AddColorMark (0, Color (color_.red, color_.green, 0.0f), false);
      m_blue->AddColorMark (1, Color (color_.red, color_.green, 1.0f), false);
    }

    if (color_model_ == color::HSV)
    {
      color::HueSaturationValue hsv(m_red->GetValue(),
                                    m_green->GetValue(),
                                    m_blue->GetValue());

      m_red->AddColorMark (0.0f, Color (1.0f, 0.0, 0.0), false);
      m_red->AddColorMark (1.0f / 6.0f, Color (1.0f, 1.0, 0.0), false);
      m_red->AddColorMark (2.0f / 6.0f, Color (0.0f, 1.0, 0.0), false);
      m_red->AddColorMark (3.0f / 6.0f, Color (0.0f, 1.0, 1.0), false);
      m_red->AddColorMark (4.0f / 6.0f, Color (0.0f, 0.0, 1.0), false);
      m_red->AddColorMark (5.0f / 6.0f, Color (1.0f, 0.0, 1.0), false);
      m_red->AddColorMark (1.0f, Color (1.0f, 0.0, 0.0), false);

      if (hsv.hue == 1.0f)
        hsv.hue = 0.0f;

      color::RedGreenBlue rgb(hsv);
      color_ = Color(rgb, m_alpha->GetValue());

      // The green holds the saturation.
      Color min_green(hsv.value, hsv.value, hsv.value);

      // The blue slider handles full value.
      hsv.value = 1.0f;
      color::RedGreenBlue blue_slider(hsv);
      m_blue->AddColorMark (0, color::Black, false);
      m_blue->AddColorMark (1.0f, Color(blue_slider), false);

      // Max green slider has full saturation and value
      hsv.saturation = 1.0f;
      color::RedGreenBlue green_slider(hsv);
      Color max_green = Color(green_slider) * hsv.value;
      m_green->AddColorMark (0, min_green, false);
      m_green->AddColorMark (1.0f, max_green, false);
    }

    if (color_model_ == color::HLS)
    {
      color::HueLightnessSaturation hls(m_red->GetValue(),
                                        m_green->GetValue(),
                                        m_blue->GetValue());

      m_red->AddColorMark (0.0f, Color (1.0f, 0.0, 0.0), false);
      m_red->AddColorMark (1.0f / 6.0f, Color (1.0f, 1.0, 0.0), false);
      m_red->AddColorMark (2.0f / 6.0f, Color (0.0f, 1.0, 0.0), false);
      m_red->AddColorMark (3.0f / 6.0f, Color (0.0f, 1.0, 1.0), false);
      m_red->AddColorMark (4.0f / 6.0f, Color (0.0f, 0.0, 1.0), false);
      m_red->AddColorMark (5.0f / 6.0f, Color (1.0f, 0.0, 1.0), false);
      m_red->AddColorMark (1.0f, Color (1.0f, 0.0, 0.0), false);

      if (hls.hue == 1.0f)
        hls.hue = 0.0f;

      color::RedGreenBlue rgb(hls);
      color_ = Color(rgb, m_alpha->GetValue());

      float s = (1.0f - hls.saturation) * 0.5;

      // Need to use HSVtoRGB to compute the primary color
      color::HueSaturationValue primary_hsv(hls.hue, 1.0f, 1.0f);
      color::RedGreenBlue primary_rgb(primary_hsv);
      Color primary = Color(primary_rgb) * hls.saturation + s;
      m_green->AddColorMark (0.0f, color::Black, false);
      m_green->AddColorMark (0.5f, primary, false);
      m_green->AddColorMark (1.0f, color::White, false);

      // Not sure on the name of this color...
      Color secondary = Color(primary_rgb);

      if (hls.lightness > 0.5)
      {
        float factor = (hls.lightness - 0.5f) / 0.5f;
        secondary = secondary * ((1 - factor) * hls.saturation) + (s + factor);
      }
      else
      {
        float factor = hls.lightness / 0.5f * hls.saturation;
        secondary = secondary * factor + s;
      }

      m_blue->AddColorMark (0, Color (hls.lightness, hls.lightness, hls.lightness), false);
      m_blue->AddColorMark (1.0f, secondary, false);
    }
  }

  RGBAPropertyItem *RGBAPropertyItem::CreateFromXML (const TiXmlElement *elementxml, NodeNetCom *parent, const char *Name, int id)
  {
    RGBAPropertyItem *node = new RGBAPropertyItem (Name, 0, 0, 0, 0);
    double red, green, blue, alpha;

    const TiXmlElement *childxml = elementxml->FirstChildElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Red"),        &red,       -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Green"),      &green,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Blue"),       &blue,      -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Alpha"),      &alpha,     -1);

    node->SetRed (red);
    node->SetGreen (green);
    node->SetBlue (blue);
    node->SetAlpha (alpha);

    node->SetID (id);
    return node;
  }

  TiXmlElement *RGBAPropertyItem::ToXML() const
  {
    TiXmlElement *elementxml = NodeNetCom::ToXML();
    TiXmlElement *childxml;
    childxml = new TiXmlElement (TEXT ("RGBAComponent") );
    //childxml->SetAttribute(TEXT("Name"), m_X->GetName());
    childxml->SetDoubleAttribute (TEXT ("Red"), color_.red);
    elementxml->LinkEndChild (childxml);
    childxml = new TiXmlElement (TEXT ("RGBAComponent") );
    //childxml->SetAttribute(TEXT("Name"), m_Y->GetName());
    childxml->SetDoubleAttribute (TEXT ("Green"), color_.green);
    elementxml->LinkEndChild (childxml);
    childxml = new TiXmlElement (TEXT ("RGBAComponent") );
    //childxml->SetAttribute(TEXT("Name"), m_Z->GetName());
    childxml->SetDoubleAttribute (TEXT ("Blue"), color_.blue);
    elementxml->LinkEndChild (childxml);
    childxml = new TiXmlElement (TEXT ("RGBAComponent") );
    //childxml->SetAttribute(TEXT("Name"), m_W->GetName());
    childxml->SetDoubleAttribute (TEXT ("Alpha"), color_.alpha);
    elementxml->LinkEndChild (childxml);

    return elementxml;
  }

  bool RGBAPropertyItem::FromXML (const TiXmlElement *elementxml)
  {
    double red, green, blue, alpha;
    tstring NameX, NameY, NameZ, NameW;
    const TiXmlElement *childxml = elementxml->FirstChildElement();
    //QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameX, GetID());
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Red"),       &red,     -1);
    childxml = childxml->NextSiblingElement();
    //QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameY, GetID());
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Green"),       &green,     -1);
    childxml = childxml->NextSiblingElement();
    //QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameZ, GetID());
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Blue"),       &blue,     -1);
    childxml = childxml->NextSiblingElement();
    //QueryNodeXMLStringAttribute(childxml, TEXT("Name"), NameW, GetID());
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Alpha"),       &alpha,     -1);
    childxml = childxml->NextSiblingElement();

    SetRed (red);
    SetGreen (green);
    SetBlue (blue);
    SetAlpha (alpha);

    return NodeNetCom::FromXML (elementxml);
  }

}
