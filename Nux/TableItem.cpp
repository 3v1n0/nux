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
  }

//   ColumnHeader::ColumnHeader(const ColumnHeader& Other)
//   {
//     m_header_area = Other.m_header_area;
//     m_header_area->Reference();
//   }

  ColumnHeader::~ColumnHeader()
  {
//     m_header_area->UnReference();
//     m_header_area = 0;
  }

//   ColumnHeader& ColumnHeader::operator = (const ColumnHeader& Other)
//   {
//     if (m_header_area)
//       m_header_area->UnReference();
// 
//     if (Other.m_header_area)
//     {
//       m_header_area = Other.m_header_area;
//       m_header_area->Reference();
//     }
//     return *this;
//   }

  RowHeader::RowHeader()
  {
    m_item = 0;
  }

  RowHeader::~RowHeader()
  {
    NUX_SAFE_DELETE (m_item);
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
    m_row_header = new CoreArea (NUX_TRACKER_LOCATION);
    m_row_header->Reference();
  }

  TableItem::~TableItem()
  {
    m_row_header->UnReference();
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
      for (UINT i = 0; i < m_ItemGeometryVector.size(); i++)
      {
        if (m_ItemGeometryVector[i].IsPointInside (ievent.event_x() - ievent.event_x_root(), ievent.event_y() - ievent.event_y_root() ) )
        {
          sigCellFocus.emit();
          ret |= 0; // if we don't return 0, then we can't have doubleclick on this item. //eMouseEventSolved;
          break;
        }
      }
    }
    else
    {
      for (UINT i = 0; i < m_ItemGeometryVector.size(); i++)
      {
        if (m_ItemGeometryVector[i].IsPointInside (ievent.event_x() - ievent.event_x_root(), ievent.event_y() - ievent.event_y_root() ) )
        {
          if (! (ret & eMouseEventSolved) && ! (ProcessEventInfo & eDoNotProcess) )
          {
            if (m_bIsMouseInside == false)
            {
              setDirtyItem (true);

              WindowThread *application = GetGraphicsThread();

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

            WindowThread *application = GetGraphicsThread();

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


//    if(geo.IsPointInside(ievent.event_x()-ievent.event_x_root(), ievent.event_y()-ievent.event_y_root()))
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
      UINT nBackground;

      if (m_bIsMouseInside)
      {
        nBackground = table->PushItemBackground (GfxContext, this, true);
        //nuxDebugMsg(TEXT("DrawDirty"));
      }
      else
      {
        nBackground = table->PushItemBackground (GfxContext, this, false);
      }

      Painter.PaintTextLineStatic (GfxContext, GetSysFont(), geo, row->m_item->GetName(), Color (0xFF000000) /*m_item[r].c_str()*/);
      table->PopItemBackground (GfxContext, nBackground);
      setDirtyItem (false);
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
