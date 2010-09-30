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
#include "TreeControl.h"

namespace nux { //NUX_NAMESPACE_BEGIN

TreeItem::TreeItem(const TCHAR* name, NodeParameterType type)
:   TableItem(name, type)
{
    Close();
}

TreeItem::~TreeItem()
{

}

long TreeItem::ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    return ret;
}

void TreeItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter,
                            RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
{
    Geometry FirstColumnGeometry = m_ItemGeometryVector[0];
    if(isDirtyItem())
    {
        UINT nBackground;
        if(m_bIsMouseInside)
        {
            nBackground = table->PushItemBackground(GfxContext, this, true);
        }
        else
        {
            nBackground = table->PushItemBackground(GfxContext, this, false);
        }
        Painter.PaintTextLineStatic(GfxContext, GetThreadFont(), geo, row->m_item->GetName(), GetItemTextColor() /*m_item[r].c_str()*/); 
        table->PopItemBackground(GfxContext, nBackground);
    }
}

void TreeItem::ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector)
{

}

int TreeItem::GetItemBestHeight()
{
    return ITEMDEFAULTHEIGHT;
}   

////////////////////////////////////////////////////////////////////////////////////////////////
TreeControl::TreeControl()
:   TableCtrl(false)
{
    TableCtrl::ShowColumnHeader(true);
    TableCtrl::ShowRowHeader(false);
    TableCtrl::EnableColumnResizing(true);
    TableCtrl::EnableApplyItemBestHeight(false);
}

TreeControl::~TreeControl()
{

}

void TreeControl::OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    int previous_click_row = m_selectedRow;

    m_selectedTableItem = 0;
    FindItemUnderPointer(x, y, &m_selectedTableItem, m_selectedRow, m_selectedColumn);

    m_selectedGeometry.SetX(0);
    m_selectedGeometry.SetY(0);
    m_selectedGeometry.SetWidth(0);
    m_selectedGeometry.SetHeight(0);

    if(m_selectedTableItem /*(m_selectedRow != -1) && (m_selectedColumn != -1)*/)
    {
        // selected item geometry
        int sx, sy, sw, sh;
        sx = m_column_header[m_selectedColumn].m_header_area->GetBaseX();
        sw = m_column_header[m_selectedColumn].m_header_area->GetBaseWidth();
        sy = m_row_header[m_selectedRow]->m_item->m_row_header->GetBaseY();
        sh = m_row_header[m_selectedRow]->m_item->m_row_header->GetBaseHeight();

        m_selectedGeometry = Geometry(sx, sy, sw, sh);
        //        sigItemSelected.emit(m_selectedRow, m_selectedColumn);
        //        sigTableItemSelected.emit(*this, *m_selectedTableItem, m_selectedRow, m_selectedColumn);
    }

    if(1)
    {
        // This is a double click
        if((m_selectedRow == -1) || (m_selectedColumn == -1))
            return;


        if(!m_row_header[m_selectedRow]->m_item->isOpen() && (m_row_header[m_selectedRow]->m_item->FirstChildNode() || m_row_header[m_selectedRow]->m_item->AlwaysShowOpeningButton()))
        {
            // If it is not open, then open it.
            OpOpenItem(m_row_header[m_selectedRow]->m_item);
            //if(m_selectedTableItem /*(m_selectedRow != -1) && (m_selectedColumn != -1)*/)
        }
        else
        {
            if(m_row_header[m_selectedRow]->m_item->FirstChildNode() /*|| m_row_header[m_selectedRow]->m_item->AlwaysShowOpeningButton()*/)
            {
                Geometry geo = m_row_header[m_selectedRow]->m_item->m_ItemGeometryVector[0];
                geo.SetX((m_bShowRowHeader? ROWHEADERWIDTH : 0) + ITEM_DEPTH_MARGIN * m_row_header[m_selectedRow]->m_item->m_depth);
                geo.SetY(m_row_header[m_selectedRow]->m_item->m_ItemGeometryVector[0].y - m_TableArea->GetBaseY());
                geo.SetWidth(OPENCLOSE_BTN_WIDTH);
                if(geo.IsPointInside(x, y))
                {
                    if(m_row_header[m_selectedRow]->m_item->isOpen())
                    {
                        OpCloseItem(m_row_header[m_selectedRow]->m_item);
                        if(IsSizeMatchContent())
                        {
                            // when closing and item, the Table gets shorter and might leave a dirty area filled with part of the Table content.
                            // We need to call a PaintBackground on the area of the Table (area before the item is closed).
                            m_DrawBackgroundOnPreviousGeometry = true;
                            m_PreviousGeometry = GetGeometry();
                            // Initiate layout re computation from the top. This should be done with InitiateResizeLayout();
                            // but it is a private member of BaseObject. We can do it with a call to SetGeometry(GetGeometry());
                            SetGeometry(GetGeometry());
                        }
                    }
                    else
                    {
                        OpOpenItem(m_row_header[m_selectedRow]->m_item);
                    }
                }
                else
                {
                    //do nothing 
                    // You have to click on the Open/Close button to close the item
                }
            }
        }
        FormatTable();
        ComputeChildLayout();
    }

    if((previous_click_row >= 0) && (previous_click_row != m_selectedRow) && (m_row_header[previous_click_row]->m_item->FirstChildNode() == 0))
        OpCloseItem(m_row_header[previous_click_row]->m_item);
    //else if((previous_click_row >= 0) && (m_row_header[previous_click_row]->m_item->FirstChildNode() == 0))
    //  OpCloseItem(m_row_header[previous_click_row]->m_item);


    {
        sigItemSelected.emit(m_selectedRow, m_selectedColumn);
        sigTableItemSelected.emit(*this, *m_selectedTableItem, m_selectedRow, m_selectedColumn);
    }
    NeedRedraw();
}

void TreeControl::OnMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    if((m_selectedRow == -1) || (m_selectedColumn == -1))
    {
        // The double click becomes a simple click if we know in advance that the first click didn't select anything.
        //OnMouseDown(x, y, button_flags, key_flags);
        return;
    }

    int previous_click_row = m_selectedRow;

    m_selectedTableItem = 0;
    FindItemUnderPointer(x, y, &m_selectedTableItem, m_selectedRow, m_selectedColumn);

    if((m_selectedTableItem == 0) || (m_selectedRow == -1) || (m_selectedColumn == -1)) 
        return;

    if(m_selectedRow != previous_click_row)
    {
        // The second button down of this double click is not on the same row. Interpret this as a mouse down.
        OnMouseDown(x, y, button_flags, key_flags);
        return;
    }

    m_selectedGeometry.SetX(0);
    m_selectedGeometry.SetY(0);
    m_selectedGeometry.SetWidth(0);
    m_selectedGeometry.SetHeight(0);

    if(m_selectedTableItem /*(m_selectedRow != -1) && (m_selectedColumn != -1)*/)
    {
        // selected item geometry
        int sx, sy, sw, sh;
        sx = m_column_header[m_selectedColumn].m_header_area->GetBaseX();
        sw = m_column_header[m_selectedColumn].m_header_area->GetBaseWidth();
        sy = m_row_header[m_selectedRow]->m_item->m_row_header->GetBaseY();
        sh = m_row_header[m_selectedRow]->m_item->m_row_header->GetBaseHeight();

        m_selectedGeometry = Geometry(sx, sy, sw, sh);
        // we couldsend a signal meaning a double click has happened on an item.
        //sigItemDoubleClick.emit(m_row_header[m_selectedRow]->m_item);
    }

    // Check if item as a child node. If not, there is no point in opening/closing it.
    if(m_row_header[m_selectedRow]->m_item->FirstChildNode() /*|| m_row_header[m_selectedRow]->m_item->AlwaysShowOpeningButton()*/)
    {
        Geometry geo = m_row_header[m_selectedRow]->m_item->m_ItemGeometryVector[0];
        geo.SetX((m_bShowRowHeader? ROWHEADERWIDTH : 0) + ITEM_DEPTH_MARGIN * m_row_header[m_selectedRow]->m_item->m_depth);
        geo.SetY(m_row_header[m_selectedRow]->m_item->m_ItemGeometryVector[0].y - m_TableArea->GetBaseY());
        geo.SetWidth(OPENCLOSE_BTN_WIDTH);
        if(geo.IsPointInside(x, y))
        {
            OnMouseDown(x, y, button_flags, key_flags);
        }
        else
        {
            //            if(m_row_header[m_selectedRow]->m_item->isOpen())
            //            {
            //                OpCloseItem(m_row_header[m_selectedRow]->m_item);
            //                if(IsSizeMatchContent())
            //                {
            //                    // when closing and item, the Table gets shorter and might leave a dirty area filled with part of the Table content.
            //                    // We need to call a PaintBackground on the area of the Table (area before the item is closed).
            //                    m_DrawBackgroundOnPreviousGeometry = true;
            //                    m_PreviousGeometry = GetGeometry();
            //                    // Initiate layout re computation from the top. This should be done with InitiateResizeLayout();
            //                    // but it is a private member of BaseObject. We can do it with a call to SetGeometry(GetGeometry());
            //                    SetGeometry(GetGeometry());
            //                }
            //            }
            //            else
            //            {
            //                OpOpenItem(m_row_header[m_selectedRow]->m_item);
            //            }
        }
    }
    FormatTable();
    ComputeChildLayout();
}


} //NUX_NAMESPACE_END
