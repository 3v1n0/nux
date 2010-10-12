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

#include "ValuatorDouble.h"
#include "DoubleValuatorPropertyItem.h"

namespace nux
{

  DoubleValuatorPropertyItem::DoubleValuatorPropertyItem (const TCHAR *name, float Value, float Step, float MinValue, float MaxValue)
    :   SectionProperty (name, NODE_TYPE_DOUBLEVALUATOR)
  {
    SetRange (MinValue, MaxValue);
    SetValue (Value);
    SetStep (Step);
    NODE_SIG_CONNECT (sigValueChanged, DoubleValuatorPropertyItem, RecvPropertyChange);
  }

  DoubleValuatorPropertyItem::~DoubleValuatorPropertyItem()
  {

  }

  long DoubleValuatorPropertyItem::ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    Geometry geo = m_ItemGeometryVector[1];

    if ( (ievent.e_event == NUX_MOUSE_PRESSED) && geo.IsPointInside (ievent.e_x, ievent.e_y) == false)
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
      ret = ProcessEvent (ievent, TraverseInfo, eDoNotProcess | ProcessEventInfo);
    }
    else
    {
      ret = ProcessEvent (ievent, TraverseInfo, ProcessEventInfo);
    }

    return ret;
  }

  void DoubleValuatorPropertyItem::DrawProperty (GraphicsContext &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter,
      RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
  {
    if (isDirtyItem() || IsRedrawNeeded() )
    {
      UINT nBackground = table->PushItemBackground (GfxContext, this);
      Painter.PaintTextLineStatic (GfxContext, GetFont(), m_FirstColumnUsableGeometry, row->m_item->GetName(), GetItemTextColor() );

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

  void DoubleValuatorPropertyItem::ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector)
  {
    if (m_ItemGeometryVector.size() >= 2)
    {
      Geometry geo;
      geo = m_ItemGeometryVector[1];
      geo = geo.GetExpand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
      SetGeometry (geo);
    }
  }

  int DoubleValuatorPropertyItem::GetItemBestHeight()
  {
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
  }

  DoubleValuatorPropertyItem *DoubleValuatorPropertyItem::CreateFromXML (const TiXmlElement *elementxml, NodeNetCom *parent, const char *Name, int id)
  {
    double value = 0.0;
    double minvalue = 0.0;
    double maxvalue = 100.0;
    double step = 0.1;
    QueryNodeXMLDoubleAttribute (elementxml, TEXT ("Value"),       &value,     -1);
    QueryNodeXMLDoubleAttribute (elementxml, TEXT ("Value"),       &value,     -1);
    QueryNodeXMLDoubleAttribute (elementxml, TEXT ("Step"),        &step,      -1);
    QueryNodeXMLDoubleAttribute (elementxml, TEXT ("MinValue"),    &minvalue,  -1);
    QueryNodeXMLDoubleAttribute (elementxml, TEXT ("MaxValue"),    &maxvalue,  -1);

    DoubleValuatorPropertyItem *node = new DoubleValuatorPropertyItem (Name, value, step, minvalue, maxvalue);
    node->SetID (id);
    return node;
  }

  TiXmlElement *DoubleValuatorPropertyItem::ToXML() const
  {
    TiXmlElement *elementxml = NodeNetCom::ToXML();
    elementxml->SetDoubleAttribute (TEXT ("Value"), GetValue() );
    elementxml->SetDoubleAttribute (TEXT ("Step"), GetStep() );
    elementxml->SetDoubleAttribute (TEXT ("MinValue"), GetMinValue() );
    elementxml->SetDoubleAttribute (TEXT ("MaxValue"), GetMaxValue() );
    return elementxml;
  }

  bool DoubleValuatorPropertyItem::FromXML (const TiXmlElement *elementxml)
  {
    double value = 0.0;
    double step = 0.0;
    double minvalue = 0.0;
    double maxvalue = 1.0;
    //double step = 1;
    QueryNodeXMLDoubleAttribute (elementxml, TEXT ("Value"),       &value,     GetID() );
    QueryNodeXMLDoubleAttribute (elementxml, TEXT ("Step"),        &step,      GetID() );
    QueryNodeXMLDoubleAttribute (elementxml, TEXT ("MinValue"),    &minvalue,  GetID() );
    QueryNodeXMLDoubleAttribute (elementxml, TEXT ("MaxValue"),    &maxvalue,  GetID() );
    //QueryNodeXMLIntAttribute(elementxml, "Step",        &step,      GetID());
    SetRange (minvalue, maxvalue);
    SetValue (value);
    SetStep (step);
    return NodeNetCom::FromXML (elementxml);
  }
}
