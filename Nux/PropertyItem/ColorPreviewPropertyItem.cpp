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

#include "ColorPreview.h"
#include "ColorPreviewPropertyItem.h"

namespace nux
{

  ColorPreviewPropertyItem::ColorPreviewPropertyItem(const TCHAR *name, float red, float green, float blue, color::Model colormodel)
    : SectionProperty(name, NODE_TYPE_COLORPREVIEW)
    , ColorPreview(Color(red, green, blue))
  {
    NODE_SIG_CONNECT (sigColorChanged, ColorPreviewPropertyItem, RecvPropertyChange);
  }

  ColorPreviewPropertyItem::~ColorPreviewPropertyItem()
  {
  }

  long ColorPreviewPropertyItem::ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    Geometry geo = m_ItemGeometryVector[1];

    if ( (ievent.e_event == NUX_MOUSE_PRESSED) && geo.IsPointInside (ievent.e_x, ievent.e_y) == false)
    {
      ret = ProcessEvent (ievent, TraverseInfo, eDoNotProcess | ProcessEventInfo);
    }
    else
    {
      ret = ProcessEvent (ievent, TraverseInfo, ProcessEventInfo);
    }

    return ret;
  }

  void ColorPreviewPropertyItem::DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter,
      RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
  {
    if (isDirtyItem() || IsRedrawNeeded() )
    {
      t_u32 nBackground = table->PushItemBackground (GfxContext, this);
      Painter.PaintTextLineStatic (GfxContext, GetFont (), m_FirstColumnUsableGeometry, row->_table_item->GetName(), GetItemTextColor() );

      if (m_ItemGeometryVector.size() >= 2)
      {
        Geometry geo2 = m_ItemGeometryVector[1];
        Geometry prop_geo;
        prop_geo.SetX (geo.x + geo.GetWidth() );
        prop_geo.SetY (geo.y);
        prop_geo.SetWidth (column_vector[1].m_header_area->GetBaseWidth() );
        prop_geo.SetHeight (geo.GetHeight() );

        geo2.Expand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        GfxContext.PushClippingRectangle (geo2);
        GfxContext.PushClippingRectangle (prop_geo);
        ProcessDraw (GfxContext, true);
        GfxContext.PopClippingRectangle();
        GfxContext.PopClippingRectangle();
      }

      table->PopItemBackground (GfxContext, nBackground);
    }
  }

  void ColorPreviewPropertyItem::ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector)
  {
    if (m_ItemGeometryVector.size() >= 2)
    {
      Geometry geo;
      geo = m_ItemGeometryVector[1];
      geo = geo.GetExpand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
      SetGeometry (geo);
    }
  }

  int ColorPreviewPropertyItem::GetItemBestHeight()
  {
    Size sz = GetMinimumSize();
    return sz.height + 2 * PROPERTY_BORDER_Y;
  }

  ColorPreviewPropertyItem *ColorPreviewPropertyItem::CreateFromXML (const TiXmlElement *elementxml, NodeNetCom *parent, const char *Name, int id)
  {
    ColorPreviewPropertyItem *node = new ColorPreviewPropertyItem (Name, 0, 0, 0, color::RGB);
    double red, green, blue;
    tstring colormodel;

    const TiXmlElement *childxml = elementxml->FirstChildElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Red"),        &red,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Green"),      &green,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Blue"),       &blue,     -1);

    node->SetColor(Color(static_cast<float>(red),
                         static_cast<float>(green),
                         static_cast<float>(blue)));
    node->SetID (id);
    return node;
  }

  TiXmlElement *ColorPreviewPropertyItem::ToXML() const
  {
    TiXmlElement *elementxml = NodeNetCom::ToXML();
    TiXmlElement *childxml;
    Color color = GetRGBColor();
    childxml = new TiXmlElement (TEXT ("RGBComponent") );
    childxml->SetDoubleAttribute (TEXT ("Red"), color.red );
    elementxml->LinkEndChild (childxml);
    childxml = new TiXmlElement (TEXT ("RGBComponent") );
    childxml->SetDoubleAttribute (TEXT ("Green"), color.green );
    elementxml->LinkEndChild (childxml);
    childxml = new TiXmlElement (TEXT ("RGBComponent") );
    childxml->SetDoubleAttribute (TEXT ("Blue"), color.blue );
    elementxml->LinkEndChild (childxml);

    return elementxml;
  }

  bool ColorPreviewPropertyItem::FromXML (const TiXmlElement *elementxml)
  {
    double red, green, blue;
    tstring NameX, NameY, NameZ, NameW;
    const TiXmlElement *childxml;
    childxml = elementxml->FirstChildElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Red"),       &red,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Green"),       &green,     -1);
    childxml = childxml->NextSiblingElement();
    QueryNodeXMLDoubleAttribute (childxml, TEXT ("Blue"),       &blue,     -1);

    SetColor(Color(static_cast<float>(red),
                   static_cast<float>(green),
                   static_cast<float>(blue)));

    return NodeNetCom::FromXML (elementxml);
  }

}
