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

#include "EditTextBox.h"
#include "EditTextLinePropertyItem.h"

namespace nux
{

  EditTextLinePropertyItem::EditTextLinePropertyItem (const TCHAR *name, const TCHAR *text)
    :   SectionProperty (name, NODE_TYPE_EDITTEXT)
    ,   EditTextBox (text, NUX_TRACKER_LOCATION)
  {
    SetText (text);
    SetTextColor (GPropertyItemTextColor1);
    NODE_SIG_CONNECT (sigValidateEntry, EditTextLinePropertyItem, RecvPropertyChange);
  }

  EditTextLinePropertyItem::~EditTextLinePropertyItem()
  {

  }

  long EditTextLinePropertyItem::ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    ret = ProcessEvent (ievent, TraverseInfo, ProcessEventInfo);
    return ret;
  }

  void EditTextLinePropertyItem::DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter,
      RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
  {
    if (isDirtyItem() || IsRedrawNeeded() )
    {
      UINT nBackground = table->PushItemBackground (GfxContext, this);
      Painter.PaintTextLineStatic (GfxContext, GetFont(), m_FirstColumnUsableGeometry, row->m_item->GetName(), GetItemTextColor() );

      if (m_ItemGeometryVector.size() >= 2)
      {
        Geometry prop_geo = m_ItemGeometryVector[1];
        prop_geo.Expand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
        GfxContext.PushClippingRectangle (prop_geo);
        ProcessDraw (GfxContext, true);
        GfxContext.PopClippingRectangle();
      }

      table->PopItemBackground (GfxContext, nBackground);
    }
  }

  void EditTextLinePropertyItem::ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector)
  {
    if (m_ItemGeometryVector.size() >= 2)
    {
      Geometry geo;
      geo = m_ItemGeometryVector[1];
      geo = geo.GetExpand (-PROPERTY_BORDER_X, -PROPERTY_BORDER_Y);
      SetGeometry (geo);
    }
  }

  int EditTextLinePropertyItem::GetItemBestHeight()
  {
    Size sz = GetMinimumSize();
    return sz.GetHeight() + 2 * PROPERTY_BORDER_Y;
  }

  void EditTextLinePropertyItem::SetBackgroundColor (Color c)
  {
    EditTextBox::SetTextBackgroundColor (c);
    m_ItemBackgroundColor = c;
  }

  EditTextLinePropertyItem *EditTextLinePropertyItem::CreateFromXML (const TiXmlElement *elementxml, NodeNetCom *parent, const char *Name, int id)
  {
    tstring text;
    QueryNodeXMLStringAttribute (elementxml, "EditText",       text,     id);
    EditTextLinePropertyItem *node = new EditTextLinePropertyItem (Name);
    node->SetText (text);
    node->SetID (id);
    return node;
  }

  TiXmlElement *EditTextLinePropertyItem::ToXML() const
  {
    TiXmlElement *elementxml = NodeNetCom::ToXML();
    elementxml->SetAttribute ("EditText", GetText() );
    return elementxml;
  }

  bool EditTextLinePropertyItem::FromXML (const TiXmlElement *elementxml)
  {
    tstring text;

    if (QueryNodeXMLStringAttribute (elementxml, "EditText", text, GetID() ) )
    {
      SetText (text);
      return NodeNetCom::FromXML (elementxml);
    }

    return false;
  }
}
