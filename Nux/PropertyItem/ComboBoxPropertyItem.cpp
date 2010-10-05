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

#include "ComboBoxSimple.h"
#include "ComboBoxPropertyItem.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  ComboBoxPropertyItem::ComboBoxPropertyItem (const TCHAR *name)
    :   SectionProperty (name, NODE_TYPE_COMBOSIMPLE)
  {
    NODE_SIG_CONNECT (sigTriggered, ComboBoxPropertyItem, RecvPropertyChange);
  }

  ComboBoxPropertyItem::~ComboBoxPropertyItem()
  {

  }

  long ComboBoxPropertyItem::ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    ret = ProcessEvent (ievent, TraverseInfo, ProcessEventInfo);
    return ret;
  }

  void ComboBoxPropertyItem::DrawProperty (GraphicsContext &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter,
      RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
  {
    Geometry geo2 = m_FirstColumnUsableGeometry;

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

  void ComboBoxPropertyItem::ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector)
  {
    if (m_ItemGeometryVector.size() >= 2)
    {
      Geometry geo;
      geo = m_ItemGeometryVector[1];
      geo = geo.GetExpand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
      SetGeometry (geo);
    }
  }

  int ComboBoxPropertyItem::GetItemBestHeight()
  {
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
  }

  ComboBoxPropertyItem *ComboBoxPropertyItem::CreateFromXML (const TiXmlElement *elementxml, NodeNetCom *parent, const char *Name, int id)
  {
    ComboBoxPropertyItem *node = new ComboBoxPropertyItem (Name);

    int NumItem = 0;
    int SelectionIndex = 0;
    QueryNodeXMLIntAttribute (elementxml, "NumItem",       &NumItem,     id);
    QueryNodeXMLIntAttribute (elementxml, "ItemSelected",  &SelectionIndex,     id);
    const TiXmlElement *childxml = elementxml->FirstChildElement();
    int i = 0;

    while (childxml && (i < NumItem) )
    {
      int UserValue;
      TCHAR label[32];
      Snprintf (label, 32, 32 - 1, TEXT ("Label_%d"), i);
      tstring ItemLabel;
      QueryNodeXMLStringAttribute (childxml, label, ItemLabel, -1);
      QueryNodeXMLIntAttribute (childxml, TEXT ("UserValue"), &UserValue, -1); // - 1 means do not care about he ID

      node->AddItem (ItemLabel.c_str(), UserValue);
      ++i;
      childxml = childxml->NextSiblingElement();
    }

    if (SelectionIndex < 0)
      node->SetSelectionIndex (0);
    else
      node->SetSelectionIndex (SelectionIndex);

    node->SetID (id);
    return node;
  }

  TiXmlElement *ComboBoxPropertyItem::ToXML() const
  {
    TiXmlElement *elementxml = NodeNetCom::ToXML();
    elementxml->SetAttribute ("NumItem", GetNumItem() );
    int SelectionIndex = GetSelectionIndex();

    if (SelectionIndex < 0)
      SelectionIndex = 0;

    elementxml->SetAttribute ("ItemSelected", SelectionIndex);

    for (int i = 0; i < GetNumItem(); i++)
    {
      TiXmlElement *childxml = new TiXmlElement ("ComboBoxItem");
      TCHAR label[32];
      Snprintf (label, 32, 32 - 1, TEXT ("Label_%d"), i);
      childxml->SetAttribute (label, GetItem (i)->GetLabel() );
      childxml->SetAttribute (TEXT ("UserValue"), GetItem (i)->GetUserValue() );
      elementxml->LinkEndChild (childxml);
    }

    return elementxml;
  }

  bool ComboBoxPropertyItem::FromXML (const TiXmlElement *elementxml)
  {
    int NumItem = 0;
    int SelectionIndex = 0;
    QueryNodeXMLIntAttribute (elementxml, "NumItem",       &NumItem,     GetID() );
    QueryNodeXMLIntAttribute (elementxml, "ItemSelected",  &SelectionIndex,     GetID() );
    const TiXmlElement *childxml = elementxml->FirstChildElement();
    RemoveAllItem();
    int i = 0;

    while (childxml && (i < NumItem) )
    {
      int UserValue;
      TCHAR label[32];
      Snprintf (label, 32, 32 - 1, TEXT ("Label_%d"), i);
      tstring ItemLabel;
      QueryNodeXMLStringAttribute (childxml, label, ItemLabel, -1);
      QueryNodeXMLIntAttribute (childxml, TEXT ("UserValue"), &UserValue, -1); // - 1 means do not care about he ID

      AddItem (ItemLabel.c_str(), UserValue);
      ++i;
      childxml = childxml->NextSiblingElement();
    }

    if (SelectionIndex < 0)
      SetSelectionIndex (0);
    else
      SetSelectionIndex (SelectionIndex);

    return NodeNetCom::FromXML (elementxml);
  }
} //NUX_NAMESPACE_END
