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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "PropertyList.h"

#include "SplineCurvePreview.h"
#include "SplineCurvePropertyItem.h"

namespace nux
{

  SplineCurvePropertyItem::SplineCurvePropertyItem (const TCHAR *name)
    :   SectionProperty (name, NODE_TYPE_SPLINE)
  {
    SetMinMaxSize (32, 32);
    NODE_SIG_CONNECT (sigSplineChanged, SplineCurvePropertyItem, RecvPropertyChange);
  }

  SplineCurvePropertyItem::~SplineCurvePropertyItem()
  {

  }

  long SplineCurvePropertyItem::ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
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

  void SplineCurvePropertyItem::DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter,
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

  void SplineCurvePropertyItem::ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector)
  {
    if (m_ItemGeometryVector.size() >= 2)
    {
      Geometry geo;
      geo = m_ItemGeometryVector[1];
      geo = geo.GetExpand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
      SetGeometry (geo);
    }
  }

  int SplineCurvePropertyItem::GetItemBestHeight()
  {
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
  }

  SplineCurvePropertyItem *SplineCurvePropertyItem::CreateFromXML (const TiXmlElement *elementxml, NodeNetCom *parent, const char *Name, int id)
  {
    SplineCurvePropertyItem *node = new SplineCurvePropertyItem (Name);

    int NumKnot = 0;
    QueryNodeXMLIntAttribute (elementxml, "NumKnot",       &NumKnot,     id);
    const TiXmlElement *childxml = elementxml->FirstChildElement();
    int i = 0;
    node->Reset();

    while (childxml && (i < NumKnot) )
    {
      double x, y;
      QueryNodeXMLDoubleAttribute (childxml, TEXT ("X"), &x, -1);
      QueryNodeXMLDoubleAttribute (childxml, TEXT ("Y"), &y, -1); // - 1 means do not care about he ID

      node->AddKnot (x, y, false);

      ++i;
      childxml = childxml->NextSiblingElement();
    }

    node->SetID (id);
    return node;
  }

  TiXmlElement *SplineCurvePropertyItem::ToXML() const
  {
    TiXmlElement *elementxml = NodeNetCom::ToXML();
    elementxml->SetAttribute ("NumKnot", GetNumKnot() );

    for (int i = 0; i < GetNumKnot(); i++)
    {
      TiXmlElement *childxml = new TiXmlElement ("SplineKnot");
      childxml->SetDoubleAttribute (TEXT ("X"), GetKnot (i).GetX() );
      childxml->SetDoubleAttribute (TEXT ("Y"), GetKnot (i).GetY() );
      elementxml->LinkEndChild (childxml);
    }

    return elementxml;
  }

  bool SplineCurvePropertyItem::FromXML (const TiXmlElement *elementxml)
  {
    int NumKnot = 0;
    QueryNodeXMLIntAttribute (elementxml, "NumKnot",       &NumKnot,     GetID() );
    const TiXmlElement *childxml = elementxml->FirstChildElement();
    int i = 0;

    while (childxml && (i < NumKnot) )
    {
      double x, y;
      QueryNodeXMLDoubleAttribute (childxml, TEXT ("X"), &x, -1);
      QueryNodeXMLDoubleAttribute (childxml, TEXT ("Y"), &y, -1); // - 1 means do not care about he ID

      AddKnot (x, y, false);
      ++i;
      childxml = childxml->NextSiblingElement();
    }

    return NodeNetCom::FromXML (elementxml);
  }
}
