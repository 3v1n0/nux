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
#include "TableCtrl.h"
#include "TableItem.h"

namespace nux
{

  const int ITEMDEFAULTWIDTH = 140;
  const int ITEMDEFAULTHEIGHT = 20;

  Color GPropertyItemTextColor0 = Color (0xFFDEDEDE);
  Color GPropertyItemTextColor1 = Color (0xFFE9E9E9);

  ColumnHeader::ColumnHeader()
  {
    m_header_area = new InputArea (NUX_TRACKER_LOCATION);
    m_header_area->SinkReference();
    _fix_width = false;
    _fix_width_value = 80;
  }

  ColumnHeader::ColumnHeader(const ColumnHeader& Other)
  {
    m_header_area = 0;
    if (Other.m_header_area)
    {
      m_header_area = Other.m_header_area;
      m_header_area->Reference();
      _fix_width = Other._fix_width;
      _fix_width_value = Other._fix_width_value;
    }
  }

  ColumnHeader::~ColumnHeader()
  {
    if (m_header_area)
      m_header_area->UnReference();
    m_header_area = 0;
  }

  ColumnHeader& ColumnHeader::operator = (const ColumnHeader& Other)
  {
    if (m_header_area)
    {
      m_header_area->UnReference();
      m_header_area = 0;
    }

    if (Other.m_header_area)
    {
      m_header_area = Other.m_header_area;
      m_header_area->Reference();
      _fix_width = Other._fix_width;
      _fix_width_value = Other._fix_width_value;
    }
    return *this;
  }

  RowHeader::RowHeader()
  {
    _table_item = 0;
  }

  RowHeader::RowHeader(const RowHeader& Other)
  {
    _table_item = 0;
  }

  RowHeader::~RowHeader()
  {
    //NUX_SAFE_DELETE (_table_item);
    _table_item = 0;
  }

  RowHeader& RowHeader::operator = (const RowHeader& Other)
  {

    return *this;
  }

  TableItem::TableItem (const TCHAR *name, NodeParameterType type)
    :   NodeNetCom (name, type)
  {
    m_PropertyRedraw = false;
    m_bIsMouseInside = false;
    m_bIsFirstVisibleItem = false;
    m_bIsLastVisibleItem = false;
    m_Table = 0;
    m_PropertyTextColor = 0;
    m_AlwaysShowOpeningButton = false;

    m_isOpen = false;
    m_bParentOpen = false;
    m_bDirty = true;
    _row_header_area = new InputArea (NUX_TRACKER_LOCATION);
    _row_header_area->Reference();
  }

  TableItem::~TableItem()
  {
    _row_header_area->UnReference();
    NUX_SAFE_DELETE (m_PropertyTextColor);
  }

  void TableItem::PushChildFront ( NodeItem *child )
  {
    NodeItem::PushChildFront (child);
  }

  void TableItem::PushChildBack ( NodeItem *child )
  {
    NodeItem::PushChildBack (child);
  }

  void TableItem::AddNextSibling ( NodeItem *sibling )
  {
    NodeItem::AddNextSibling (sibling);
  }

  void TableItem::AddPrevSibling ( NodeItem *sibling )
  {
    NodeItem::AddPrevSibling (sibling);
  }

  void TableItem::Unlink ( void )
  {
    TableItem *root = (TableItem *) RootNode();
    NodeItem::Unlink();

    if (root->m_Table)
      root->m_Table->ResetTable();
  }

  long TableItem::ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    //m_bIsMouseInside = false;
    long ret = TraverseInfo;
    Geometry geo = m_ItemGeometryVector[0];

    if (ievent.e_event == NUX_MOUSE_PRESSED && ! (ret & eMouseEventSolved) && ! (ProcessEventInfo & eDoNotProcess) )
    {
      for (t_u32 i = 0; i < m_ItemGeometryVector.size(); i++)
      {
        if (m_ItemGeometryVector[i].IsPointInside (ievent.GetX() - ievent.GetRootX(), ievent.GetY() - ievent.GetRootY() ) )
        {
          sigCellFocus.emit();
          ret |= 0; // if we don't return 0, then we can't have doubleclick on this item. //eMouseEventSolved;
          break;
        }
      }
    }
    else
    {
      for (t_u32 i = 0; i < m_ItemGeometryVector.size(); i++)
      {
        if (m_ItemGeometryVector[i].IsPointInside (ievent.GetX() - ievent.GetRootX(), ievent.GetY() - ievent.GetRootY() ) )
        {
          if (! (ret & eMouseEventSolved) && ! (ProcessEventInfo & eDoNotProcess) )
          {
            if (m_bIsMouseInside == false)
            {
              setDirtyItem (true);

              WindowThread *application = GetWindowThread ();

              if (application)
                application->RequestRedraw();

              //nuxDebugMsg(TEXT("Dirty In: %s"), GetName());
            }

            m_bIsMouseInside = true;

            ret |=  eMouseEventSolved;
          }
        }
        else
        {
          if (m_bIsMouseInside)
          {
            setDirtyItem (true);

            WindowThread *application = GetWindowThread ();

            if (application)
              application->RequestRedraw();

            //nuxDebugMsg(TEXT("Dirty Out: %s"), GetName());
          }

          m_bIsMouseInside = false;
        }
      }
    }

    // We don't trap the mouse down here. We let the TableCtrl capture it.
    // The TableCtrl does the following:
    //  - Set the dirty flags of the previously selected TableItem if any.
    //  - Find the newly selected TableItem and set its dirty flag to true.


//    if(geo.IsPointInside(ievent.GetX()-ievent.GetRootX(), ievent.GetY()-ievent.GetRootY()))
//    {
//        if(m_bIsMouseInside != true)
//        {
//            setDirtyItem(true);
//            m_bIsMouseInside = true;
//            TraverseInfo = eMouseEventSolved;
//        }
//        //        else
//        //        {
//        //            setDirtyItem(true);
//        //            m_bIsMouseInside = false;
//        //        }
//    }
//    else
//    {
//        if(m_bIsMouseInside != false)
//        {
//            setDirtyItem(true);
//            m_bIsMouseInside = false;
//        }
//    }
    return ret;
  }

  void TableItem::DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter,
                                RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
  {
    Geometry FirstColumnGeometry = m_ItemGeometryVector[0];

    if (isDirtyItem() )
    {
      t_u32 nBackground;

      if (m_bIsMouseInside)
      {
        nBackground = table->PushItemBackground (GfxContext, this, true);
        //nuxDebugMsg(TEXT("DrawDirty"));
      }
      else
      {
        nBackground = table->PushItemBackground (GfxContext, this, false);
      }

      Painter.PaintTextLineStatic(GfxContext, GetSysFont(), geo, row->_table_item->GetName(), Color (0xFF000000));
      table->PopItemBackground(GfxContext, nBackground);
      setDirtyItem(false);
    }
  }

  int TableItem::GetItemBestHeight()
  {
    return COLUMNHEADERHEIGHT;
  }

  void TableItem::SetPropertyItemWidth()
  {

  }

}
