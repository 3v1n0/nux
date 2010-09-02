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
#include "HScrollBar.h"
#include "VScrollBar.h"
#include "HLayout.h"
#include "VLayout.h"
#include "WindowCompositor.h"
#include "TableCtrl.h"

NAMESPACE_BEGIN_GUI

const int HANDLERSIZE = 6;
extern const int ITEMDEFAULTWIDTH;
extern const int ITEMDEFAULTHEIGHT;
const Color HEADER_BACKGROUND_COLOR = Color(0xFF666666);
    
const int ROWHEADERWIDTH = 30;
const int OPENCLOSE_BTN_WIDTH = 15;
const int ITEM_DEPTH_MARGIN = 20;
const int MIN_COLUMN_WIDTH = 10;
const int TABLE_AREA_BOTTOM_SPACE = 0;
const int COLUMNHEADERHEIGHT = 20;

Color TABLE_HEADER_BASE_COLOR = Color(0xFF191919);
Color TABLE_HEADER_TEXT_COLOR = Color(0xFFFFFFFF);
//       ROWHEADERWIDTH
//    ------------------------------DEFAULTWIDTH--------------------------
//    |                     |                       |                      |
//  COLUMNHEADERHEIGHT      |                       |                      |   <---- Column Header
//    |_____________________|_______________________|______________________|
//    |                     |                       |                      |
//    |                     |                       |                      |
//  DEFAULTHEIGHT           |                       |                      |
//    |                     |                       |                      |
//    |_____________________|_______________________|______________________|
//    |
//
//                ^
//                |
//                |
//                |
//            Row Header

TableCtrl::TableCtrl(bool floating_column)
:   m_bEnableRowResizing(false)
,   m_bEnableColumnResizing(true)
,   m_bShowRowHeader(true)
,   m_bShowColumnHeader(true)
,   m_bShowVerticalSeparationLine(true)
,   m_bShowHorizontalSeparationLine(false)
,   m_bEnableItemBestHeight(true)
,   m_FloatingColumn(floating_column)
,   m_ClickSelect(CLICK_SELECT_CELL)
,   m_RowColorOdd(0xFF343434)
,   m_RowColorEven(0xFF3f3f3f)
,   m_TableBottomColor(0xFF555555)
,   m_SelectionColor(0xFF202020)
,   m_MouseOverColor(0xFF666666)
,   m_HorizontalSeparationLineColor(0xFF747474)
,   m_VerticalSeparationLineColor(0xFF747474)
,   m_IsEnableMouseOverColor(false)
,   m_DrawBackgroundOnPreviousGeometry(false)
,   m_TableItemHead(0)
{
    m_TableItemHead = new TableItem(TEXT("TableRootNode"));
    m_TableItemHead->m_Table = this;
    m_TableItemHead->Open();
    m_TableItemHead->Show();

    m_TableArea = smptr(CoreArea)(new CoreArea());
    m_TableArea->OnMouseDown.connect(sigc::mem_fun(this, &TableCtrl::OnMouseDown));
    m_TableArea->OnMouseDoubleClick.connect(sigc::mem_fun(this, &TableCtrl::OnMouseDoubleClick));
    m_TableArea->OnMouseUp.connect(sigc::mem_fun(this, &TableCtrl::OnMouseUp));
    m_TableArea->OnMouseDrag.connect(sigc::mem_fun(this, &TableCtrl::OnMouseDrag));
    m_TableArea->OnStartFocus.connect(sigc::mem_fun(this, &TableCtrl::OnKeyboardFocus));
    m_TableArea->OnEndFocus.connect(sigc::mem_fun(this, &TableCtrl::OnLostKeyboardFocus));
    


    m_tableNumRow = 0;
    m_tableNumColumn = 0;

    m_selectedRow = -1;
    m_selectedColumn = -1;

    m_selectedTableItem = 0;
    m_selectedMouseDownTableItem = 0;
    m_OpenItemTotalHeight = 0;

    EnableVerticalScrollBar(true); 
    EnableHorizontalScrollBar(true);

    m_VLayout = smptr(VLayout)(new VLayout("TABLELAYOUT"));
    m_VLayout->SetVerticalInternalMargin(4);

    m_TableArea->setGeometry(0, 0, 300, 40);
    // Setting the minimum of size m_TableArea forces the composition layout fit match the size of the Table area (+/- margins). if the layout is too large, then
    // the scrollbar will reflect that. 
    // It is not enough to do something like:
    //          m_VLayout->AddLayout(new WidgetLayout(&m_TableArea), 0, eLeft, eFull);
    // with a stretch factor equal to 0. Because m_VLayout is either a HLayout or a VLayout (here it is a VLayout), only one dimension of the composition layout
    // will match the similar dimension of TableArea. So only one scrollbar (Horizontal or Vertical) will be adjusted.
    // See FormatTable.
    m_VLayout->AddActiveInterfaceObject(m_TableArea, 1, eLeft, eFull);
    m_VLayout->SetContentStacking(eStackTop);
    m_VLayout->SetStretchFactor(1);
    SetMinimumSize(50, 50);
    setGeometry(Geometry(0, 0, 50, 50)); 

    setTopBorder(0);
    setBorder(0);

    SetViewContentRightMargin(2);
    SetViewContentBottomMargin(2);

    SetCompositionLayout(m_VLayout);
    FormatContent();

    m_TableArea->EnableDoubleClick(false);
    
    ShowRowHeader(true);
    ShowColumnHeader(true);
}

TableCtrl::~TableCtrl()
{
    std::vector<header2>::iterator it1;
    std::vector<RowHeader*>::iterator it2;
    std::vector<sizehandler2*>::iterator it3;

//    for(it1 = m_column_header.begin(); it1 != m_column_header.end(); it1++)
//    {
//        delete (*it1);
//    }
    for(it2 = m_row_header.begin(); it2 != m_row_header.end(); it2++)
    {
        delete (*it2);
    }
    for(it3 = m_column_sizehandler.begin(); it3 != m_column_sizehandler.end(); it3++)
    {
        delete (*it3);
    }

    for(it3 = m_row_sizehandler.begin(); it3 != m_row_sizehandler.end(); it3++)
    {
        delete (*it3);
    }

    //delete m_VLayout;
}

long TableCtrl::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;
    long ProcEvInfo = ProcessEventInfo;
    bool AllowCellSelection = (TraverseInfo & eMouseEventSolved)? false : true;

    if(ievent.e_event == INL_MOUSE_PRESSED)
    {
        Geometry ViewGeometry = Geometry(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight);
        if(!ViewGeometry.IsPointInside(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root))
        {
            ProcEvInfo |= eDoNotProcess;
        }
        Geometry TableGoe = GetGeometry();
        if(!TableGoe.IsPointInside(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root))
        {
            sigLostFocus.emit();
        }
    }
    
    bool EventAlreadySolved = ret & eMouseEventSolved;

    // Probe the column size handlers.
    std::vector<sizehandler2*>::iterator it;
    if(m_bEnableColumnResizing && m_bShowColumnHeader)
    {
       for(it = m_column_sizehandler.begin(); it != m_column_sizehandler.end(); it++)
        {
            ret = (*it)->OnEvent(ievent, ret, ProcEvInfo);
        }
    }

    // Probe the row size handlers.
    if(m_bEnableRowResizing && m_bShowRowHeader)
    {
        std::vector<RowHeader*>::iterator row_iterator;
        for(row_iterator = m_row_header.begin(), it = m_row_sizehandler.begin(); it != m_row_sizehandler.end(); it++, row_iterator++)
        {
            if((*row_iterator)->item->IsParentOpen() == true)
            {
                ret = (*it)->OnEvent(ievent, ret, ProcEvInfo);
            }
        }
    }

    
    // This bool checks if there is at least one item that is dirty.
    bool DirtyItem = false;
    bool ItemSolvedEvent = false;
    {
        std::vector<RowHeader*>::iterator row_iterator;
        for(row_iterator = m_row_header.begin(), it = m_row_sizehandler.begin(); it != m_row_sizehandler.end(); it++, row_iterator++)
        {
            if((*row_iterator)->item->IsParentOpen() == true)
            {
                ret = (*row_iterator)->item->ProcessPropertyEvent(ievent, ret, ProcEvInfo);
                if(ret & eMouseEventSolved)
                    ItemSolvedEvent = true;

                // Pure TableItem elements do not inherit from ActiveInterfaceObject. Therefore, they cannot call NeedRedraw() on themselves.
                // The TableCtrl that holds them must know that they need to be redrawn. Since Pure TableItem do not trap events, the event
                // will go to TableCtrl (like OnMouseDown) who will found out the cell where the event happened and set the dirty flag of the TableItem.
                //if((*row_iterator)->item->isDirtyItem())
                //    DirtyItem = true;
            }
        }
    }

//    if(DirtyItem)
//        NeedRedraw();

    if(m_vertical_scrollbar_enable)
        ret = vscrollbar->ProcessEvent(ievent, ret, ProcEvInfo & (~eDoNotProcess));
    if(m_horizontal_scrollbar_enable)
        ret = hscrollbar->ProcessEvent(ievent, ret, ProcEvInfo & (~eDoNotProcess));

    if(IsPartOfCombobox() && ievent.e_event == INL_MOUSE_RELEASED)
    {
        if(GetGeometry().IsPointInside(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root) &&
            m_TableArea->GetGeometry().IsPointInside(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root))
        {
            m_TableArea->ForceStartFocus(0, 0);
            ret = m_TableArea->OnEvent(ievent, ret, ProcEvInfo);
        }
        else
        {
            ret = m_TableArea->OnEvent(ievent, ret, ProcEvInfo);
        }
    }
    else
    {
        if(ievent.e_event == INL_MOUSE_PRESSED && ItemSolvedEvent && !(EventAlreadySolved) /*&& GetThreadWindowCompositor().GetMouseFocusArea()*/)
        {
            // The mouse down event has already been captured. This call is going to find out if a cell was hit.
            OnMouseDown(ievent.e_x - ievent.e_x_root - m_TableArea->GetBaseX(),
            ievent.e_y - ievent.e_y_root - m_TableArea->GetBaseY(), ievent.event_mouse_state(), ievent.event_key_state());
        }
        else
            ret = m_TableArea->OnEvent(ievent, ret, ProcEvInfo);
    }


    // PostProcessEvent2 must always have its last parameter set to 0
    // because the m_BackgroundArea is the real physical limit of the window.
    // So the previous test about IsPointInside do not prevail over m_BackgroundArea 
    // testing the evnt by itself.
    ret = PostProcessEvent2(ievent, ret, ProcEvInfo);

    // If it is a mouse down, check for the selected row and column
    if((ievent.e_event == INL_MOUSE_PRESSED) && ((ProcEvInfo & eDoNotProcess) == 0) && AllowCellSelection)
    {
        bool in_column = false;
        bool in_row = false;

        int x = ievent.e_x - ievent.e_x_root - m_TableArea->GetBaseX();
        int y = ievent.e_y - ievent.e_y_root - m_TableArea->GetBaseY();

        if((0 <= x) && (x < m_TableArea->GetBaseWidth()))
        {
            in_column = true;
        }
        if((0 <= y) && (y < m_TableArea->GetBaseHeight()))
        {
            in_row = true;
        }

        if(in_column && in_row)
        {
//            OnMouseDown(ievent.e_x-m_TableArea->GetX(), 
//                ievent.e_y-m_TableArea->GetY(), ievent.event_mouse_state(), ievent.event_key_state());
        }
        else
        {
            if((m_selectedColumn != -1) || (m_selectedRow != -1))
            {
                m_selectedColumn = -1;
                m_selectedRow = -1;
                NeedRedraw();
            }
        }
    }
    else if((ievent.e_event == INL_MOUSE_PRESSED) && ((ProcEvInfo & eDoNotProcess) || EventAlreadySolved)) 
    {
        if((m_selectedColumn != -1) || (m_selectedRow != -1))
        {
            m_selectedColumn = -1;
            m_selectedRow = -1;
            NeedRedraw();
        }
    }

    
    //GetThreadWindowCompositor()..AddToDrawList(this);
    return ret;
}

void PresentBufferToScreen(TRefGL<IOpenGLTexture2D> texture, int x, int y)
{
    nuxAssert(texture.IsValid());
    if(texture.IsNull())
        return;
    t_u32 window_width, window_height;
    window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().Push2DWindow(window_width, window_height);

    // Render the MAINFBO
    {
        GetGraphicsThread()->GetGraphicsContext().EnableTextureMode(GL_TEXTURE0, GL_TEXTURE_2D);
        GetGraphicsThread()->GetGraphicsContext().SetEnvModeSelectTexture(GL_TEXTURE0);
        texture->BindTextureToUnit(GL_TEXTURE0);

        UINT w, h;
        w = texture->GetWidth();
        h = texture->GetHeight();

        TexCoordXForm texxform0;
        texxform0.uwrap = TEXWRAP_CLAMP;
        texxform0.vwrap = TEXWRAP_CLAMP;
        texxform0.FlipVCoord(true);
        GetThreadGraphicsContext()->QRP_GLSL_1Tex(x, y, w, h, texture, texxform0, Color::White);
    }
}

void TableCtrl::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    if(IsSizeMatchContent())
    {
        if(m_DrawBackgroundOnPreviousGeometry)
            gPainter.PaintBackground(GfxContext, m_PreviousGeometry);
        m_DrawBackgroundOnPreviousGeometry = false;
    }


    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle(base);
    gPainter.PaintBackground(GfxContext, base);

    PushShapeCornerBackgroundScope pbs(gPainter, GfxContext, 
        Geometry(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight),
        eSHAPE_CORNER_ROUND10,
        Color(0xFF4D4D4D),
        eAllCorners, true);

    Geometry TableArea = m_TableArea->GetGeometry();

     base = GetGeometry();

    if(m_vertical_scrollbar_enable)
    {
        vscrollbar->NeedRedraw();
    }
    if(m_horizontal_scrollbar_enable)
    {
        hscrollbar->NeedRedraw();
    }

    DrawTable(GfxContext);
    GfxContext.PopClippingRectangle();
}

void TableCtrl::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    GfxContext.PushClippingRectangle(GetGeometry());

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////
    /// Draw the first column items ///
    ///////////////////////////////////

    int num_row = (t_u32)m_row_header.size();
    int ItemOffsetY = (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0);

    GfxContext.PushClippingRectangle(Geometry(m_ViewX, m_ViewY + (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0),
        m_ViewWidth, m_ViewHeight - (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0)));
    {
        int odd_or_even = 0;
        for(int r = 0; r < num_row; r++)
        {
            if(m_row_header[r]->item->IsParentOpen())
            {
                Geometry geo = m_row_header[r]->item->m_RowHeader.GetGeometry();
                if(m_row_header[r]->item->FirstChildNode() != 0)
                {
                    geo.SetX(m_TableArea->GetBaseX() + (m_bShowRowHeader? ROWHEADERWIDTH : 0) + ITEM_DEPTH_MARGIN * m_row_header[r]->item->m_depth + OPENCLOSE_BTN_WIDTH);
                    geo.SetY(m_TableArea->GetBaseY() + ItemOffsetY);
                    geo.SetWidth(m_column_header[0].header.GetBaseWidth() - ITEM_DEPTH_MARGIN * m_row_header[r]->item->m_depth - OPENCLOSE_BTN_WIDTH);
                }
                else
                {
                    geo.SetX(m_TableArea->GetBaseX() + (m_bShowRowHeader? ROWHEADERWIDTH : 0) + ITEM_DEPTH_MARGIN * m_row_header[r]->item->m_depth);
                    geo.SetY(m_TableArea->GetBaseY() + ItemOffsetY);
                    geo.SetWidth(m_column_header[0].header.GetBaseWidth() - ITEM_DEPTH_MARGIN * m_row_header[r]->item->m_depth);
                }

                // Draw content of a row
                {
                    m_row_header[r]->item->DrawProperty(GfxContext, this, force_draw, geo, gPainter, m_row_header[r], m_column_header, odd_or_even ? GetRowColorEven(): GetRowColorOdd());
                }
                ItemOffsetY += m_row_header[r]->item->m_RowHeader.GetBaseHeight();
            }
            odd_or_even = !odd_or_even;
        }
    }
    GfxContext.PopClippingRectangle();

    ///////////////////////////////////////////////////////////////////////////////


    if(m_vertical_scrollbar_enable)
    {
        vscrollbar->ProcessDraw(GfxContext, force_draw);
    }

    if(m_horizontal_scrollbar_enable)
    {
        hscrollbar->ProcessDraw(GfxContext, force_draw);
    }
    GfxContext.PopClippingRectangle();
}

void TableCtrl::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{
    if(IsRedrawNeeded())
    {
        // Drawing is complete
        // Reset the Item dirty flag to false;
        std::vector<RowHeader*>::iterator row_iterator;
        for(row_iterator = m_row_header.begin(); row_iterator != m_row_header.end(); row_iterator++)
        {
            if((*row_iterator)->item->IsParentOpen() == true)
            {
                // Mark all visible items as not dirty
                (*row_iterator)->item->setDirtyItem(false);
            }
        }
    }

    ScrollView::SwapTextureIndex();
}

void TableCtrl::PaintDecoration(GraphicsContext& GfxContext, TableItem* item)
{
    //int ItemOffsetY = (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0);

    Geometry geo = item->m_ItemGeometryVector[0];
    if((item->FirstChildNode() != 0)/* || item->AlwaysShowOpeningButton()*/)
    {
        geo.SetX(m_TableArea->GetBaseX() + (m_bShowRowHeader? ROWHEADERWIDTH : 0) + ITEM_DEPTH_MARGIN * item->m_depth + OPENCLOSE_BTN_WIDTH);
        //geo.SetY(m_TableArea->GetY() + ItemOffsetY);
        geo.SetWidth(m_column_header[0].header.GetBaseWidth() - ITEM_DEPTH_MARGIN * item->m_depth - OPENCLOSE_BTN_WIDTH);
    }
    else
    {
        geo.SetX(m_TableArea->GetBaseX() + (m_bShowRowHeader? ROWHEADERWIDTH : 0) + ITEM_DEPTH_MARGIN * item->m_depth);
        //geo.SetY(m_TableArea->GetY() + ItemOffsetY);
        geo.SetWidth(m_column_header[0].header.GetBaseWidth() - ITEM_DEPTH_MARGIN * item->m_depth);
    }

    if((item->FirstChildNode() != 0)/* || item->AlwaysShowOpeningButton()*/)
    {
        if(item->isOpen())
        {
            GfxContext.PushClippingRectangle(item->m_ItemGeometryVector[0]);
            geo.OffsetPosition(-OPENCLOSE_BTN_WIDTH, 0);
            geo.SetWidth(OPENCLOSE_BTN_WIDTH);

            GeometryPositioning gp(eHACenter, eVACenter);
            Geometry GeoPo = ComputeGeometryPositioning(geo, gTheme.GetImageGeometry(eTREE_NODE_OPEN), gp);
            gPainter.PaintShape(GfxContext, GeoPo, Color(0xFF000000), eTREE_NODE_OPEN);

            GfxContext.PopClippingRectangle();
        }
        else
        {
            GfxContext.PushClippingRectangle(item->m_ItemGeometryVector[0]);
            geo.OffsetPosition(-OPENCLOSE_BTN_WIDTH, 0);
            geo.SetWidth(OPENCLOSE_BTN_WIDTH);

            GeometryPositioning gp(eHACenter, eVACenter);
            Geometry GeoPo = ComputeGeometryPositioning(geo, gTheme.GetImageGeometry(eTREE_NODE_CLOSE), gp);
            gPainter.PaintShape(GfxContext, GeoPo, Color(0xFF000000), eTREE_NODE_CLOSE);

            GfxContext.PopClippingRectangle();
        }
    }
}

void TableCtrl::PaintRowDecoration(GraphicsContext& GfxContext, TableItem* item, Color color)
{
    if(m_bShowRowHeader)
    {
        Geometry geo = item->m_RowHeaderGeometry;
        gPainter.Paint2DQuadColor(GfxContext, geo, color);
        gPainter.PaintTextLineStatic(GfxContext, GetFont(), geo, "i");
    }
}

UINT TableCtrl::PushItemBackground(GraphicsContext& GfxContext, TableItem* item, bool MouseOver)
{
    Geometry row_geometry = item->m_TotalGeometry;
    Color BackgroundColor = item->m_ItemBackgroundColor;
    
    if(MouseOver && m_IsEnableMouseOverColor)
        BackgroundColor = GetMouseOverColor();
    UINT NumBackground = 0;

    gPainter.PushShapeLayer(GfxContext, 
        Geometry(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight),
        eSHAPE_CORNER_ROUND2,
        Color(0xFF4D4D4D),
        eAllCorners);

    NumBackground++;

    if(item->m_bIsFirstVisibleItem && item->m_bIsLastVisibleItem)
    {
        if(m_bShowColumnHeader)
        {
            gPainter.PushDrawShapeLayer(GfxContext, row_geometry, eSHAPE_CORNER_ROUND2, BackgroundColor, eCornerBottomLeft | eCornerBottomRight);
            NumBackground++;
        }
        else
        {
            gPainter.PushDrawShapeLayer(GfxContext, row_geometry, eSHAPE_CORNER_ROUND2, BackgroundColor, eAllCorners);
            NumBackground++;
        }
    }
    else if(item->m_bIsFirstVisibleItem)
    {
        if(m_bShowColumnHeader)
        {
            gPainter.PushDrawColorLayer(GfxContext, row_geometry, BackgroundColor);
            NumBackground++;
        }
        else
        {
            gPainter.PushDrawShapeLayer(GfxContext, row_geometry, eSHAPE_CORNER_ROUND2, BackgroundColor, eCornerTopLeft | eCornerTopRight);
            NumBackground++;
        }
    }
    else if(item->m_bIsLastVisibleItem)
    {
        gPainter.PushDrawShapeLayer(GfxContext, row_geometry, eSHAPE_CORNER_ROUND2, BackgroundColor, eCornerBottomLeft | eCornerBottomRight);
        NumBackground++;
    }
    else
    {
        gPainter.PushDrawColorLayer(GfxContext, row_geometry, BackgroundColor);
        NumBackground++;
    }

    bool isSelected = (item == GetSelectedItem());
    Color SelectionColor = GetSelectionColor();
    if(MouseOver && m_IsEnableMouseOverColor)
        SelectionColor = GetMouseOverColor();

    int SelectedColumn = GetSelectedColumn();
    for(t_u32 Column = 0; Column < GetNumColumn(); Column++)
    {
        if(isSelected && (SelectedColumn == Column))
        {
            if(item->m_bIsFirstVisibleItem && item->m_bIsLastVisibleItem)
            {
                if(m_bShowColumnHeader)
                {
                    t_u32 corners = 0L;
                    if(Column == 0 && !m_bShowRowHeader)
                        corners |= eCornerBottomLeft;
                    if(Column == GetNumColumn() - 1)
                        corners |= eCornerBottomRight;

                    gPainter.PushDrawShapeLayer(GfxContext, item->m_ItemGeometryVector[Column], eSHAPE_CORNER_ROUND2, SelectionColor, corners);
                    NumBackground++;
                }
                else
                {
                    gPainter.PushDrawShapeLayer(GfxContext, item->m_ItemGeometryVector[Column], eSHAPE_CORNER_ROUND2, SelectionColor, eAllCorners);
                    NumBackground++;
                }
            }
            else if(item->m_bIsFirstVisibleItem)
            {
                if(m_bShowColumnHeader)
                {
                    gPainter.PushDrawColorLayer(GfxContext, item->m_ItemGeometryVector[Column], SelectionColor);
                    NumBackground++;
                }
                else
                {
                    t_u32 corners = 0L;
                    if(Column == 0 && !m_bShowRowHeader)
                        corners |= eCornerTopLeft;
                    if(Column == GetNumColumn() - 1)
                        corners |= eCornerTopRight;

                    gPainter.PushDrawShapeLayer(GfxContext, item->m_ItemGeometryVector[Column], eSHAPE_CORNER_ROUND2, SelectionColor, corners);
                    NumBackground++;
                }
            }
            else if(item->m_bIsLastVisibleItem)
            {
                if(Column == 0)
                {
                    if(m_bShowRowHeader == false)
                    {
                        gPainter.PushDrawShapeLayer(GfxContext, item->m_ItemGeometryVector[Column], eSHAPE_CORNER_ROUND2, SelectionColor, eCornerBottomLeft);
                        NumBackground++;
                    }
                    else
                    {
                        gPainter.PushDrawColorLayer(GfxContext, item->m_ItemGeometryVector[Column], SelectionColor);
                        NumBackground++;
                    }
                }
                else if(Column == GetNumColumn() - 1)
                {
                    gPainter.PushDrawShapeLayer(GfxContext, item->m_ItemGeometryVector[Column], eSHAPE_CORNER_ROUND2, SelectionColor, eCornerBottomRight);
                    NumBackground++;
                }
                else
                {
                    gPainter.PushDrawColorLayer(GfxContext, item->m_ItemGeometryVector[Column], SelectionColor);
                    NumBackground++;
                }
            }
            else
            {
                gPainter.PushDrawColorLayer(GfxContext, item->m_ItemGeometryVector[Column], SelectionColor);
                NumBackground++;
            }
        }

        if(m_bShowRowHeader == true || Column != 0)
        {
            if(m_bShowVerticalSeparationLine)
            {
                gPainter.Draw2DLine(GfxContext, 
                    item->m_ItemGeometryVector[Column].x,
                    item->m_ItemGeometryVector[Column].y,
                    item->m_ItemGeometryVector[Column].x,
                    item->m_ItemGeometryVector[Column].y + item->m_ItemGeometryVector[Column].GetHeight(),
                    GetVerticalSeparationLineColor());
            }
        }

        if(Column == 0)
            PaintDecoration(GfxContext, item);

    }

    // Draw horizontal line
    if(item->m_bIsLastVisibleItem == false)
        if(m_bShowHorizontalSeparationLine)
            gPainter.Draw2DLine(GfxContext, row_geometry.x, row_geometry.y + row_geometry.GetHeight()-1, row_geometry.x + row_geometry.GetWidth(), row_geometry.y + row_geometry.GetHeight()-1, GetHorizontalSeparationLineColor());

    return NumBackground;
}

void TableCtrl::PopItemBackground(GraphicsContext& GfxContext, UINT NumBackground)
{
    gPainter.PopBackground(NumBackground);
}

void TableCtrl::DrawTable(GraphicsContext& GfxContext)
{
    int TextureIndex = ScrollView::GetTextureIndex();
    TRefGL<IOpenGLFrameBufferObject> CurrentFrameBuffer = GetThreadWindowCompositor().GetWindowFrameBufferObject();
    if(0 /*m_ReformatTexture*/)
    {
        SetTextureIndex(0);
        m_FrameBufferObject->FormatFrameBufferObject(m_ViewWidth, m_ViewHeight, BITFMT_R8G8B8A8);
        m_TextureBuffer[0] = GetThreadGLDeviceFactory()->CreateTexture(m_ViewWidth, m_ViewHeight, 1, BITFMT_R8G8B8A8);
        m_TextureBuffer[1] = GetThreadGLDeviceFactory()->CreateTexture(m_ViewWidth, m_ViewHeight, 1, BITFMT_D24S8);
        m_FrameBufferObject->SetRenderTarget( 0, m_TextureBuffer[0]->GetSurfaceLevel(0) );
        m_FrameBufferObject->SetDepthSurface( 0 );
        m_FrameBufferObject->Activate();

        GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, m_ViewWidth, m_ViewHeight);
        GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
        GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, m_ViewWidth, m_ViewHeight);
        GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, m_ViewWidth, m_ViewHeight);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(m_ViewWidth, m_ViewHeight);

        Matrix4 mat, rot, trans;
        mat.Translate(-m_ViewX, -m_ViewY, 0);
        GfxContext.Clear2DModelViewMatrix();
        GfxContext.Push2DModelViewMatrix(mat);

        //glClearColor(1.0f, 0.5, 0, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

    }
    //    else
    //    {
    //        int TextureIndex = ScrollView::GetTextureIndex();
    //        m_FrameBufferObject->SetRenderTarget( 0, m_TextureBuffer[TextureIndex]->GetSurfaceLevel(0) );
    //        m_FrameBufferObject->SetDepthSurface( 0 );
    //        m_FrameBufferObject->Activate();
    //    }


    Geometry tableGeometry = m_TableArea->GetGeometry();
    int xl, yl, wl, hl;

    //--->gPainter.Paint2DQuadColor(tableGeometry, Color(HEADER_BACKGROUND_COLOR));

    int colum_width = (m_bShowRowHeader? ROWHEADERWIDTH : 0);
    std::vector<header2>::iterator column_iterator;
    for(column_iterator = m_column_header.begin(); column_iterator != m_column_header.end(); column_iterator++)
    {
        colum_width += (*column_iterator).header.GetGeometry().GetWidth();
    }

    std::vector<RowHeader*>::iterator row_iterator;
    std::vector<sizehandler2*>::iterator row_sizehandler_iterator;
    int ItemOffsetY = 0;

    GfxContext.PushClippingRectangle(Geometry(m_ViewX, m_ViewY + (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0),
        m_ViewWidth, m_ViewHeight - (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0)));
    {
        int every_two = 0;
        for(row_iterator = m_row_header.begin(); row_iterator != m_row_header.end(); row_iterator++)
        {
            if((*row_iterator)->item->IsParentOpen() == true)
            {
                Geometry row_geometry = (*row_iterator)->item->m_TotalGeometry;

                // Mark all visible items as dirty
                (*row_iterator)->item->setDirtyItem(true);
            }
            every_two++;
        }

        // Render row name and horizontal separation
        xl = m_TableArea->GetGeometry().x;
        yl = m_TableArea->GetGeometry().y + (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0);  // skip the Height of the Column header;
        wl = m_TableArea->GetGeometry().GetWidth();
        hl = m_TableArea->GetGeometry().GetHeight();

        // Paint the first horizontal separation line.
        gPainter.Draw2DLine(GfxContext, xl, yl, xl + m_ContentGeometry.GetWidth(), yl, GetHorizontalSeparationLineColor());
        for(row_iterator = m_row_header.begin(), row_sizehandler_iterator = m_row_sizehandler.begin();
            row_iterator != m_row_header.end(); 
            row_iterator++, row_sizehandler_iterator++)
        {
            if((*row_iterator)->item->IsParentOpen() == true)
            {
                // elements in m_column_header are relative to m_TableArea-> Get their absolute position in geo.
                Geometry geo = (*row_iterator)->item->m_RowHeader.GetGeometry();
                geo.OffsetPosition(m_TableArea->GetBaseX(), m_TableArea->GetBaseY()/* - ItemOffsetY*/);

                // Paint the number of the row
                //gPainter.PaintTextLineStatic(geo, (*row_iterator)->header.GetBaseString());

                // Paint row horizontal separation line. 
                yl += (*row_iterator)->item->m_RowHeader.GetGeometry().GetHeight();
                gPainter.Draw2DLine(GfxContext, xl, yl, xl + m_ContentGeometry.GetWidth(), yl, GetHorizontalSeparationLineColor());

                // Draw the row size handler on the separation line.
                Geometry sizehandler_geo(xl, yl - HANDLERSIZE/2, (m_bShowRowHeader? ROWHEADERWIDTH : 0), HANDLERSIZE);
            }
            else
            {
                ItemOffsetY += (*row_iterator)->item->m_RowHeader.GetBaseHeight();
            }
        }
    }
    GfxContext.PopClippingRectangle();


    if(0)
    {
        GfxContext.Pop2DModelViewMatrix();
        GfxContext.Clear2DModelViewMatrix();
        CurrentFrameBuffer->Activate();
        int window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
        int window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
        GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, window_width, window_height);
        GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
        GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, window_width, window_height);
        GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, window_width, window_height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(window_width, window_height);
        PresentBufferToScreen(m_TextureBuffer[0], m_ViewX, m_ViewY);
    }

    DrawHeader(GfxContext);
}

void TableCtrl::DrawHeader(GraphicsContext& GfxContext)
{
    GfxContext.PushClippingRectangle(Geometry(m_ViewX, m_ViewY /*+ (!m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0)*/,
        m_ViewWidth, m_ViewHeight /*- (!m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0)*/));


    std::vector<header2>::iterator column_iterator;
    std::vector<RowHeader*>::iterator row_iterator;

    int ItemOffsetY = 0;
    ///////////////////////////////////
    /// Draw the first column items ///
    ///////////////////////////////////
    t_u32 num_row = (t_u32)m_row_header.size();
    ItemOffsetY = (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0);
    for(t_u32 r = 0; r < num_row; r++)
    {
        if(m_row_header[r]->item->IsParentOpen())
        {
            ItemOffsetY += m_row_header[r]->item->m_RowHeader.GetBaseHeight();
        }
    }

    int row_height = 0;
    for(row_iterator = m_row_header.begin(); row_iterator != m_row_header.end(); row_iterator++)
    {
        if((*row_iterator)->item->IsParentOpen())
            row_height += (*row_iterator)->item->m_RowHeader.GetBaseHeight();
    }

    int TotalHeight = ItemOffsetY;

    ////////////////////////////
    /// DRAW THE HEADER PART ///
    ////////////////////////////
    // Draw Top Header Background
    Geometry header_geo = Geometry(m_ViewX, m_ViewY, m_ViewWidth, (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0));
    //--->gPainter.Paint2DQuadColor(header_geo, Color(HEADER_BACKGROUND_COLOR));

    if(m_bShowColumnHeader)
    {
        //gPainter.PaintShapeCorner(GfxContext, header_geo, 0xFF2f2f2f, eSHAPE_CORNER_ROUND10, eCornerTopLeft|eCornerTopRight);
    }

    int xl, yl, wl, hl;
    // Render header name and vertical separations
    xl = m_TableArea->GetGeometry().x + (m_bShowRowHeader? ROWHEADERWIDTH : 0);  // skip the width of the Row header
    yl = m_TableArea->GetGeometry().y + (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0);
    wl = m_TableArea->GetGeometry().GetWidth();
    hl = m_TableArea->GetGeometry().GetHeight() - (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0);

    // Paint the first vertical separation line.
    if(m_bShowRowHeader == false)
    {
        // Do not draw the first vertical separation line if the row header is not visible.
    }
    else
    {
        gPainter.Draw2DLine(GfxContext, xl, yl, xl, yl + row_height, GetVerticalSeparationLineColor());
    }

    if(m_bShowColumnHeader)
    {
        if(m_bShowRowHeader)
        {
            // Draw the Top-Left corner of the header
            Geometry geo = Geometry(GetBaseX(), GetBaseY(), ROWHEADERWIDTH, COLUMNHEADERHEIGHT);
            geo.OffsetSize(-1, 0);
            gPainter.PaintShape(GfxContext, geo, TABLE_HEADER_BASE_COLOR, eSHAPE_CORNER_ROUND4/*, eCornerTopLeft|eCornerTopRight*/);
        }
        for(column_iterator = m_column_header.begin(); column_iterator != m_column_header.end(); column_iterator++)
        {
            //        if(!isFloatingColumn() && (column_iterator + 1 == m_column_header.end()))
            //        {
            //            // Do not draw the last vertical separation line
            //            break;
            //        }

            // elements in m_column_header are relative to m_TableArea-> Get their absolute position in geo.
            Geometry geo = (*column_iterator).header.GetGeometry();
            Geometry header_title_geo = geo;
            geo.OffsetPosition(m_TableArea->GetBaseX(), m_ViewY);
            header_title_geo = geo;
            if((column_iterator == m_column_header.begin()) && (m_bShowRowHeader == false))
            {
                header_title_geo.OffsetSize(-1, 0);
                // this is not the first column
            }
            else if(column_iterator+1 == m_column_header.end())
            {
                // The last column
                header_title_geo.OffsetSize(-1, 0);
                header_title_geo.OffsetPosition(1, 0);
            }
            else
            {
                header_title_geo.OffsetSize(-2, 0);
                header_title_geo.OffsetPosition(1, 0);
            }

            gPainter.PaintShape(GfxContext, header_title_geo, TABLE_HEADER_BASE_COLOR/*0xFF2f2f2f*/, eSHAPE_CORNER_ROUND4/*, eCornerTopLeft|eCornerTopRight*/);
            // This is arbitrary to get the text away from the borders of the column.
            {
                geo.OffsetPosition(4, 0);
                geo.OffsetSize(-8, 0);
            }

            gPainter.PaintTextLineStatic(GfxContext, GFontBold, geo, (*column_iterator).header.GetBaseString().GetTCharPtr(), TABLE_HEADER_TEXT_COLOR);

            if(!isFloatingColumn() && (column_iterator + 1 == m_column_header.end()))
            {
                // Do not draw the last vertical separation line if the columns are not floating
                break;
            }
            else
            {
                // Paint column vertical separation.
                xl += (*column_iterator).header.GetGeometry().GetWidth();
                //gPainter.Draw2DLine(GfxContext, xl, yl, xl, yl + row_height, GetVerticalSeparationLineColor());
            }
            ////////////////////////////////////
            // Draw the column size handler on the line
            Geometry sizehandler_geo(xl - HANDLERSIZE/2, m_ViewY, HANDLERSIZE, (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0));
            //gPainter.Paint2DQuadColor(sizehandler_geo, Color(0.1, 0.1, 0.1, 1.0));
        }

        if(isFloatingColumn() && (m_column_header.size()>0) )
        {
            column_iterator = m_column_header.end() - 1;
            // Draw the Top-Left corner of the header
            int width = m_ViewWidth - ((*column_iterator).header.GetBaseX() + (*column_iterator).header.GetBaseWidth());
            if(width > 5)
            {
                Geometry geo = Geometry((*column_iterator).header.GetBaseX() + (*column_iterator).header.GetBaseWidth(),
                    GetBaseY(), width, COLUMNHEADERHEIGHT);
                geo.OffsetSize(-2, 0);
                geo.OffsetPosition(4, 0);
                gPainter.PaintShape(GfxContext, geo, TABLE_HEADER_BASE_COLOR, eSHAPE_CORNER_ROUND4/*, eCornerTopLeft|eCornerTopRight*/);
            }
        }
    }
    GfxContext.PopClippingRectangle();
}

void TableCtrl::DrawHeaderPreview(GraphicsContext& GfxContext)
{
    GfxContext.PushClippingRectangle(Geometry(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight));
    std::vector<header2>::iterator column_iterator;
    std::vector<RowHeader*>::iterator row_iterator;

    int row_height = 0;
    int TotalHeight = (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0);
    for(row_iterator = m_row_header.begin(); row_iterator != m_row_header.end(); row_iterator++)
    {
        if((*row_iterator)->item->IsParentOpen())
        {
            row_height += (*row_iterator)->item->m_RowHeader.GetBaseHeight();
            TotalHeight += (*row_iterator)->item->m_RowHeader.GetBaseHeight();
        }
    }

    // Draw Top Header Background
    Geometry header_geo = Geometry(m_ViewX, m_ViewY, m_ViewWidth, (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0));

    gPainter.PaintShape(GfxContext, header_geo, TABLE_HEADER_BASE_COLOR, eSHAPE_CORNER_ROUND4/*, eCornerTopLeft|eCornerTopRight*/);

    Color header_base_color = Color(0xa0808080);

    if(m_bShowColumnHeader)
    {
        if(m_bShowRowHeader)
        {
            // Draw the Top-Left corner of the header
            Geometry geo = Geometry(GetBaseX(), GetBaseY(), ROWHEADERWIDTH, COLUMNHEADERHEIGHT);
            geo.OffsetSize(-1, 0);
            gPainter.PaintShape(GfxContext, geo, header_base_color, eSHAPE_CORNER_ROUND4/*, eCornerTopLeft|eCornerTopRight*/);
        }
        for(column_iterator = m_column_header.begin(); column_iterator != m_column_header.end(); column_iterator++)
        {
            // elements in m_column_header are relative to m_TableArea-> Get their absolute position in geo.
            Geometry geo = (*column_iterator).header.GetGeometry();
            Geometry header_title_geo = geo;
            geo.OffsetPosition(m_TableArea->GetBaseX(), m_ViewY);
            header_title_geo = geo;
            if((column_iterator == m_column_header.begin()) && (m_bShowRowHeader == false))
            {
                header_title_geo.OffsetSize(-1, 0);
                // this is not the first column
            }
            else if(column_iterator+1 == m_column_header.end())
            {
                // The last column
                header_title_geo.OffsetSize(-1, 0);
                header_title_geo.OffsetPosition(1, 0);
            }
            else
            {
                header_title_geo.OffsetSize(-2, 0);
                header_title_geo.OffsetPosition(1, 0);
            }

            gPainter.PaintShape(GfxContext, header_title_geo, header_base_color/*0xFF2f2f2f*/, eSHAPE_CORNER_ROUND4/*, eCornerTopLeft|eCornerTopRight*/);
            // This is arbitrary to get the text away from the borders of the column.
            {
                geo.OffsetPosition(4, 0);
                geo.OffsetSize(-8, 0);
            }
            gPainter.PaintTextLineStatic(GfxContext, GFontBold, geo, (*column_iterator).header.GetBaseString().GetTCharPtr(), TABLE_HEADER_TEXT_COLOR);
        }

        if(isFloatingColumn() && (m_column_header.size()>0) )
        {
            column_iterator = m_column_header.end() - 1;
            // Draw the Top-Left corner of the header
            int width = m_ViewWidth - ((*column_iterator).header.GetBaseX() + (*column_iterator).header.GetBaseWidth());
            if(width > 5)
            {
                Geometry geo = Geometry((*column_iterator).header.GetBaseX() + (*column_iterator).header.GetBaseWidth(),
                    GetBaseY(), width, COLUMNHEADERHEIGHT);
                geo.OffsetSize(-2, 0);
                geo.OffsetPosition(4, 0);
                gPainter.PaintShape(GfxContext, geo, header_base_color, eSHAPE_CORNER_ROUND4/*, eCornerTopLeft|eCornerTopRight*/);
            }
        }
    }
    GfxContext.PopClippingRectangle();
}

void TableCtrl::OverlayDrawing(GraphicsContext& GfxContext)
{
    DrawHeaderPreview(GfxContext);
}

/////////
// API //
/////////

void TableCtrl::setGeometry(const Geometry& geo)
{
    ScrollView::setGeometry(geo);
}

void TableCtrl::AdjustNonFloatingColumn()
{
    int new_width = m_ViewWidth;

    new_width -= (m_bShowRowHeader? ROWHEADERWIDTH : 0);

    // space_to_share represents the difference between the available width and the total width occupied by the headers.
    // This value can be close but not equal to 0 due to fractional errors when computing the sizes.
    // Computations is done is float but results are stored in integers. Every things below 1.0 is rounded down to the integer value 0.

    int space_to_share = new_width;
    int total_non_fix_width = 0;
    std::vector<header2>::iterator it;
    header2* LastNonFixHeader = 0;
    for(it = m_column_header.begin(); it != m_column_header.end(); it++)
    {
        if((*it).bFixWidth)
        {
            // First do a little correction in case the fix width is less than MIN_COLUMN_WIDTH.
            if((*it).FixWidthValue < MIN_COLUMN_WIDTH)
            {
                (*it).FixWidthValue = MIN_COLUMN_WIDTH;
            }
            space_to_share -= (*it).FixWidthValue;
        }
        else
        {
            LastNonFixHeader = &(*it);
            total_non_fix_width += (*it).header.GetBaseWidth();
            space_to_share -= (*it).header.GetBaseWidth();
        }
    }
    int RemainingSpace = new_width;
    for(it = m_column_header.begin(); it != m_column_header.end(); it++)
    {
        if((*it).bFixWidth)
        {
            RemainingSpace -= (*it).FixWidthValue;
        }
    }
    for(it = m_column_header.begin(); it != m_column_header.end(); it++)
    {
        if(!(*it).bFixWidth)
        {
            int w = (*it).header.GetBaseWidth();
            int nw = w + space_to_share*(float(w)/float(total_non_fix_width));
            if(nw < MIN_COLUMN_WIDTH)
                nw = MIN_COLUMN_WIDTH;

            if(&(*it) == LastNonFixHeader)
            {
                // This is to absorb the remaining space completely. We do that because of rounding
                // error(in the division and multiply calculus) that might leave some space unoccupied.
                // Set the RemainingSpace only if it is bigger than nw.
                if(RemainingSpace > nw)
                    (*it).header.SetBaseWidth(RemainingSpace);
                else
                    (*it).header.SetBaseWidth(nw);
            }
            else
            {
                RemainingSpace -= nw;
                (*it).header.SetBaseWidth(nw);
            }
        }
    }
}

void TableCtrl::ComputeNonFloatingColumn()
{
    int total_non_fix_column_width = m_ViewWidth;
    t_u32 num_non_fix_column = (t_u32)m_column_header.size();

    total_non_fix_column_width -= (m_bShowRowHeader? ROWHEADERWIDTH : 0);

    std::vector<header2>::iterator it;
    for(it = m_column_header.begin(); it != m_column_header.end(); it++)
    {
        if((*it).bFixWidth)
        {
            // First do a little correction in case the fix width is less than MIN_COLUMN_WIDTH.
            if((*it).FixWidthValue < MIN_COLUMN_WIDTH)
            {
                (*it).FixWidthValue = MIN_COLUMN_WIDTH;
            }

            total_non_fix_column_width -= (*it).FixWidthValue;
            num_non_fix_column--;
        }
    }

    int initial_column_width = 0;
    if(num_non_fix_column != 0)
    {
        initial_column_width = total_non_fix_column_width/num_non_fix_column;
    }

    if(initial_column_width < MIN_COLUMN_WIDTH)
    {
        initial_column_width = MIN_COLUMN_WIDTH;
    }

    for(it = m_column_header.begin(); it != m_column_header.end(); it++)
    {
        if((*it).bFixWidth)
        {
            (*it).header.SetBaseWidth((*it).FixWidthValue);
        }
        else
        {
            if(initial_column_width < MIN_COLUMN_WIDTH)
                (*it).header.SetBaseWidth(MIN_COLUMN_WIDTH);
            else
                (*it).header.SetBaseWidth(initial_column_width);
        }
    }
}

void TableCtrl::addHeader(const TCHAR* name, bool fixed_width, int column_width)
{
    sizehandler2 *column_sizehandler = new sizehandler2;
    column_sizehandler->SetBaseWidth(HANDLERSIZE);
    column_sizehandler->SetBaseHeight((m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0));

    t_u32 no = (t_u32)m_column_sizehandler.size();
    column_sizehandler->OnMouseDown.connect(sigc::bind( sigc::mem_fun(this, &TableCtrl::OnResizeHeaderMouseDown), no));
    column_sizehandler->OnMouseUp.connect(sigc::bind( sigc::mem_fun(this, &TableCtrl::OnResizeHeaderMouseUp), no));
    column_sizehandler->OnMouseDrag.connect(sigc::bind( sigc::mem_fun(this, &TableCtrl::OnResizeHeaderMouseDrag), no));
    m_column_sizehandler.push_back(column_sizehandler);
    


    t_u32 num_header = (t_u32)m_column_header.size();
    if(num_header == 0)
    {
        header2 h2;
        h2.header.SetBaseString(name);
        h2.header.SetBaseX((m_bShowRowHeader? ROWHEADERWIDTH : 0)); // if has row header, this should be ROWHEADERWIDTH
        h2.header.SetBaseY(0);
        h2.header.SetBaseWidth(ITEMDEFAULTWIDTH);
        h2.header.SetBaseHeight((m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0));
        h2.bFixWidth = fixed_width;
        h2.FixWidthValue = column_width;
        m_column_header.push_back(h2);
    }
    else
    {
        int max_width = 0;

        Geometry g = m_column_header[num_header-1].header.GetGeometry();
        header2 h2;
        h2.header.SetBaseString(name);
        h2.header.SetBaseX(g.x + g.GetWidth());
        h2.header.SetBaseY(0);
        h2.header.SetBaseWidth(ITEMDEFAULTWIDTH);
        h2.header.SetBaseHeight((m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0));
        h2.bFixWidth = fixed_width;
        h2.FixWidthValue = column_width;
        m_column_header.push_back(h2);
    }

    if(isFloatingColumn() == false)
    {
        ComputeNonFloatingColumn();
    }
    FormatTable();
}

int TableCtrl::FormatHeader()
{
    int x = m_TableArea->GetBaseX();
    int y = m_TableArea->GetBaseY();
    int totalWidth = (m_bShowRowHeader? ROWHEADERWIDTH : 0);
    int totalHeight = (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0);

    std::vector<sizehandler2*>::iterator size_it;
    std::vector<header2>::iterator column_it;

    for(column_it = m_column_header.begin(), size_it = m_column_sizehandler.begin();
        (column_it != m_column_header.end()) && (size_it != m_column_sizehandler.end());
        column_it++, size_it++)
    {
        (*column_it).header.SetBaseX(x - m_TableArea->GetBaseX() + (m_bShowRowHeader? ROWHEADERWIDTH : 0));
        (*column_it).header.SetBaseY(y - m_TableArea->GetBaseY());
        int w = (*column_it).header.GetBaseWidth();

        if((column_it + 1) == m_column_header.end())
        {
            // This is the last column
            //int w = (*column_it)->header.GetWidth();
        }

        x += w;
        totalWidth += w;

        (*size_it)->SetBaseXY(x  + (m_bShowRowHeader? ROWHEADERWIDTH : 0) - HANDLERSIZE / 2, m_ViewY);
    }
    return totalWidth;
}

void TableCtrl::FormatTable()
{
    int x = m_TableArea->GetBaseX();
    int y = m_TableArea->GetBaseY();
    int totalWidth = (m_bShowRowHeader? ROWHEADERWIDTH : 0);
    int totalHeight = (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0);

    totalWidth = FormatHeader();
    x = m_TableArea->GetBaseX();
    y = m_TableArea->GetBaseY();

    bool FirstVisibleItem = true;
    TableItem* LastVisibleItem = 0;

    int i = 0;
    std::vector<sizehandler2*>::iterator it2;
    std::vector<header2>::iterator it3;
    std::vector<RowHeader*>::iterator it4;
    // position the visible parts of m_row_header and m_row_sizehandler only according to m_row_header[i]->item->IsParentOpen()
    for(it4 = m_row_header.begin(), it2 = m_row_sizehandler.begin(), i = 0; it4 != m_row_header.end(); it4++, it2++, i++)
    {
        if(m_row_header[i]->item->IsParentOpen() == true)
        {
            //--->
            int ItemBestHeight = ITEMDEFAULTHEIGHT;
            if((m_bEnableRowResizing == false) && (m_bEnableItemBestHeight == true))
                ItemBestHeight = m_row_header[i]->item->GetItemBestHeight();
            //<---

            // This mean we found the first visible item in the tree
            if(FirstVisibleItem)
            {
                FirstVisibleItem = false;
                m_row_header[i]->item->m_bIsFirstVisibleItem = true;
            }
            LastVisibleItem = m_row_header[i]->item;
            m_row_header[i]->item->m_bIsLastVisibleItem = false;

            m_row_header[i]->item->m_RowHeader.SetBaseX(x - m_TableArea->GetBaseX());
            m_row_header[i]->item->m_RowHeader.SetBaseY(y - m_TableArea->GetBaseY() + (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0));
            m_row_header[i]->item->m_RowHeader.SetBaseWidth((m_bShowRowHeader? ROWHEADERWIDTH : 0));
            m_row_header[i]->item->m_RowHeader.SetBaseHeight(ItemBestHeight);

            if(m_bShowRowHeader)
            {
                m_row_header[i]->item->m_RowHeaderGeometry = Geometry(x, y + (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0),
                    ROWHEADERWIDTH, m_row_header[i]->item->m_RowHeader.GetBaseHeight());
            }
            else
            {
                m_row_header[i]->item->m_RowHeaderGeometry = Geometry(0, 0, 0, 0);
            }

            int h = (*it4)->item->m_RowHeader.GetBaseHeight();

            y += h;
            totalHeight += h;

            (*it2)->SetBaseXY(x, y + (m_bShowColumnHeader? COLUMNHEADERHEIGHT : 0) - HANDLERSIZE/2);


            m_row_header[i]->item->m_ItemGeometryVector.clear();
            for(t_u32 j = 0; j < (t_u32)m_column_header.size(); j++)
            {
                int x_column,
                    y_row,
                    column_width,
                    row_height;

                if(j == 0)
                {
                    // geometry of the first column of the row (minus the open/close bitmap decoration geometry).
                    if(m_row_header[i]->item->FirstChildNode() != 0)
                    {
                        int depth = m_row_header[i]->item->m_depth;
                        x_column = m_TableArea->GetBaseX() + m_column_header[j].header.GetBaseX() /*+ (m_bShowRowHeader? ROWHEADERWIDTH : 0)*/ +
                            ITEM_DEPTH_MARGIN * m_row_header[i]->item->m_depth + OPENCLOSE_BTN_WIDTH;
                        y_row    = m_TableArea->GetBaseY() + m_row_header[i]->item->m_RowHeader.GetBaseY();
                        column_width = m_column_header[j].header.GetBaseWidth() - 
                            (ITEM_DEPTH_MARGIN * m_row_header[i]->item->m_depth + OPENCLOSE_BTN_WIDTH);
                        row_height = m_row_header[i]->item->m_RowHeader.GetBaseHeight();

                        m_row_header[i]->item->m_FirstColumnUsableGeometry = Geometry(x_column, y_row, column_width, row_height);
                    }
                    else
                    {
                        x_column = m_TableArea->GetBaseX() + m_column_header[j].header.GetBaseX() /*+ (m_bShowRowHeader? ROWHEADERWIDTH : 0)*/ +
                            ITEM_DEPTH_MARGIN * m_row_header[i]->item->m_depth;
                        y_row    = m_TableArea->GetBaseY() + m_row_header[i]->item->m_RowHeader.GetBaseY();
                        column_width = m_column_header[j].header.GetBaseWidth() - 
                            (ITEM_DEPTH_MARGIN * m_row_header[i]->item->m_depth);
                        row_height = m_row_header[i]->item->m_RowHeader.GetBaseHeight();

                        m_row_header[i]->item->m_FirstColumnUsableGeometry = Geometry(x_column, y_row, column_width, row_height);
                    }
                }

                // geometry of the column of the row.
                x_column = m_TableArea->GetBaseX() + m_column_header[j].header.GetBaseX() /*+ (m_bShowRowHeader? ROWHEADERWIDTH : 0)*/;
                y_row    = m_TableArea->GetBaseY() + m_row_header[i]->item->m_RowHeader.GetBaseY();
                column_width = m_column_header[j].header.GetBaseWidth();
                row_height = m_row_header[i]->item->m_RowHeader.GetBaseHeight();

                Geometry geo(x_column, y_row, column_width, row_height);

                m_row_header[i]->item->m_ItemGeometryVector.push_back(geo);
            }

            m_row_header[i]->item->ComputePropertyLayout(m_TableArea->GetBaseX(),
                m_TableArea->GetBaseY(),
                m_row_header[i], m_column_header);
        }

        m_row_header[i]->item->m_TotalGeometry = Geometry(m_TableArea->GetBaseX()/* + m_column_header[0].header.x*/,
            m_TableArea->GetBaseY() + m_row_header[i]->item->m_RowHeader.GetBaseY(),
            m_TableArea->GetBaseWidth()/* - m_column_header[0].header.x*/,
            m_row_header[i]->item->m_RowHeader.GetBaseHeight());
    }
    if(LastVisibleItem)
    {
        LastVisibleItem->m_bIsLastVisibleItem = true;
    }

    if((m_selectedRow != -1) && (m_selectedColumn != -1))
    {
        m_selectedGeometry = Geometry(m_column_header[m_selectedColumn].header.GetBaseX(), m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseY(),
            m_column_header[m_selectedColumn].header.GetBaseWidth(), m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseHeight());
    }

    // Setting the minimum size of m_TableArea forces the composition layout fit match the size of the m_TableArea (+/- margins). If the layout is too large, then
    // the scrollbar will reflect that. 
    // It is not enough to do something like:
    //          m_VLayout->AddLayout(new WidgetLayout(&m_TableArea), 0, eLeft, eFull);
    // with a stretch factor equal to 0. Because m_VLayout is either a HLayout or a VLayout (here it is a VLayout), only one dimension of the composition layout
    // will match the similar dimension of m_TableArea-> So only one scrollbar (Horizontal or Vertical) will be adjusted.
    m_TableArea->SetMinimumSize(totalWidth, totalHeight + TABLE_AREA_BOTTOM_SPACE);
    m_TableArea->SetBaseSize(totalWidth, totalHeight + TABLE_AREA_BOTTOM_SPACE);
    
    // Force the Stretch factor of m_TableArea so that it does not grow bigger than its content if IsSizeMatchContent() returns TRUE.
    if(IsSizeMatchContent())
    {
        m_TableArea->SetStretchFactor(0);
    }
    else
    {
        m_TableArea->SetStretchFactor(1);
    }
}

void TableCtrl::AddColumn(const TCHAR* title, bool fixed_width, int width)
{
    if(width < MIN_COLUMN_WIDTH)
        addHeader(title, fixed_width, MIN_COLUMN_WIDTH);
    else
        addHeader(title, fixed_width, width);

    m_tableNumColumn += 1; 
    // m_tableNumColumn = HeaderList.size();
}

void TableCtrl::AddItem(TableItem* item /*const TCHAR* item_name*/)
{
    if(m_tableNumColumn == 0)
    {
        return;
    }
    
    if(item == 0)
        return;

    TableItem* element = item;
    while(element)
    {
        sizehandler2 *row_sizehandler = new sizehandler2;
        row_sizehandler->SetBaseWidth((m_bShowRowHeader? ROWHEADERWIDTH : 0));
        row_sizehandler->SetBaseHeight(HANDLERSIZE);

        t_u32 row_no = (t_u32)m_row_sizehandler.size();
        row_sizehandler->OnMouseDown.connect(sigc::bind( sigc::mem_fun(this, &TableCtrl::OnResizeRowMouseDown), row_no));
        row_sizehandler->OnMouseDrag.connect(sigc::bind( sigc::mem_fun(this, &TableCtrl::OnResizeRowMouseDrag), row_no));
        m_row_sizehandler.push_back(row_sizehandler);


        //m_item.push_back(item->GetName());
        m_tableNumRow += 1;

        t_u32 num_row = (t_u32)m_row_header.size();
        if(num_row == 0)
        {
            RowHeader* h2 = new RowHeader;
            Color ItemBackgroundColor = ((num_row-1) & 0x1)? GetRowColorOdd(): GetRowColorEven();
            element->SetBackgroundColor(ItemBackgroundColor);
            h2->item = element;
            //h2->header.setCaption(num_row);
            h2->item->m_RowHeader.SetBaseX(0);
            h2->item->m_RowHeader.SetBaseY(ITEMDEFAULTHEIGHT);
            h2->item->m_RowHeader.SetBaseWidth((m_bShowRowHeader? ROWHEADERWIDTH : 0));
            h2->item->m_RowHeader.SetBaseHeight(ITEMDEFAULTHEIGHT);
            m_row_header.push_back(h2);
        }
        else
        {
            Geometry g = m_row_header[num_row-1]->item->m_RowHeader.GetGeometry();
            RowHeader* h2 = new RowHeader;

            Color ItemBackgroundColor = ((num_row-1) & 0x1)? GetRowColorOdd(): GetRowColorEven();
            element->SetBackgroundColor(ItemBackgroundColor);
            h2->item = element;
            //h2->header.setCaption(num_row);
            h2->item->m_RowHeader.SetBaseX(0);
            h2->item->m_RowHeader.SetBaseY(g.y + g.GetHeight());
            h2->item->m_RowHeader.SetBaseWidth((m_bShowRowHeader? ROWHEADERWIDTH : 0));
            h2->item->m_RowHeader.SetBaseHeight(ITEMDEFAULTHEIGHT);
            m_row_header.push_back(h2);
        }

        if(element->FirstChildNode())
            AddItem(static_cast<TableItem*>(element->FirstChildNode()));
        element = static_cast<TableItem*> (element->Next());
    }
    //FormatTable();
}

void TableCtrl::ResetItems()
{
    std::vector<RowHeader*>::iterator it;
    // position the visible parts of m_row_header and m_row_sizehandler only according to m_row_header[i]->item->IsParentOpen()
    for(it = m_row_header.begin(); it != m_row_header.end(); it++)
    {
        (*it)->item->setDirtyItem(false);
        (*it)->item->m_bIsMouseInside = false;

    }

    m_selectedColumn = -1;
    m_selectedRow = -1;
    m_selectedTableItem = 0;

}

void TableCtrl::setTableItems(TableItem* item, bool isOpen)
{
    if(item == 0)
        return;

    m_TableItemHead->PushChildBack(item);
    if(isOpen)
        OpOpenItem(item);
    else
        OpCloseItem(item);
    item->Show();

    TraverseItem(item, false, &TableCtrl::OpAddItem);
    OpBuildItemDepth(item, 0);

    FormatTable();
    ComputeChildLayout();
}

void TableCtrl::ResetTable()
{
    DeleteRowSizeHandler();
    DeleteRowHeader();
    m_tableNumRow = 0;
    m_selectedRow = -1;
    m_selectedColumn = -1;
    m_selectedTableItem = 0;

    TableItem* child = (TableItem*) m_TableItemHead->FirstChildNode();
    if(child == 0)
        return;

    while(child)
    {
        if(child->isOpen())
            OpOpenItem(child);
        else
            OpCloseItem(child);
        child->Show();
        child = (TableItem*) child->Next();
    }

    child = (TableItem*) m_TableItemHead->FirstChildNode();
    TraverseItem(child, false, &TableCtrl::OpAddItem);
    OpBuildItemDepth(child, 0);

    FormatTable();
    ComputeChildLayout();
}

void TableCtrl::DeleteRowSizeHandler()
{
    t_u32 si = (t_u32)m_row_header.size();
    std::vector<sizehandler2*>::iterator it;
    for(it = m_row_sizehandler.begin(); it != m_row_sizehandler.end(); it++)
    {
        delete (*it);
    }
    m_row_sizehandler.clear();
}

void TableCtrl::DeleteRowHeader()
{
    t_u32 si = (t_u32)m_row_header.size();
    std::vector<RowHeader*>::iterator it;
    for(it = m_row_header.begin(); it != m_row_header.end(); it++)
    {
        delete (*it);
    }
    m_row_header.clear();
}

void TableCtrl::TraverseItem(TableItem* item, bool open_only, ItemOperator f)
{
    (this->*f)(item);

//    TableItem* item_traverser = item;
//
//    while(item_traverser)
//    {
//        (this->*f)(item_traverser);
//        if(open_only)
//        {
//            if(item_traverser->isOpen() && (item_traverser->FirstChildNode() != 0))
//            {
//                TraverseItem(static_cast<TableItem*>(item_traverser->FirstChildNode()), open_only, f);
//            }
//        }
//        else if(item_traverser->FirstChildNode() != 0)
//        {
//            TraverseItem(static_cast<TableItem*>(item_traverser->FirstChildNode()), open_only, f);
//
//        }
//        item_traverser = static_cast<TableItem*>(item_traverser->Next());
//    }
}

void TableCtrl::OpBuildItemDepth(TableItem* item, int depth)
{
    while(item)
    {
        item->m_depth = depth;
        if(item->FirstChildNode() != 0)
        {
            OpBuildItemDepth(static_cast<TableItem*>(item->FirstChildNode()), depth+1);
        }
        item = static_cast<TableItem*>(item->Next());
    }
}

void TableCtrl::OpAddItem(TableItem* item)
{
    AddItem(item);
}

void TableCtrl::OpCloseItem(TableItem* item)
{
    item->Close();
    OpHideItem(static_cast<TableItem*>(item->FirstChildNode()));
}

// Open the argument item. Then 
void TableCtrl::OpOpenItem(TableItem* item)
{
    item->Open();
    OpShowItem(static_cast<TableItem*>(item->FirstChildNode()));
}

void TableCtrl::OpHideItem(TableItem* item)
{
    while(item)
    {
        item->Hide();
        if(item->FirstChildNode())
        {
            OpHideItem(static_cast<TableItem*>(item->FirstChildNode()));
        }
        item = static_cast<TableItem*>(item->Next());
    }
}

void TableCtrl::OpShowItem(TableItem* item)
{
    while(item)
    {
        item->Show();
        if(item->FirstChildNode() && (item->isOpen()))
        {
            OpShowItem(static_cast<TableItem*>(item->FirstChildNode()));
        }
        item = static_cast<TableItem*>(item->Next());
    }
}

void TableCtrl::FindItemUnderPointer(int x, int y, TableItem** ppItem, int &row, int &column)
{
    std::vector<Geometry>::iterator it;

    // selected item geometry
    int sx, sy, sw, sh;

    row = -1;
    t_u32 num_row = (t_u32)m_row_header.size();

    // test the visible parts of m_row_header only according to m_row_header[i]->item->IsParentOpen()
    for(t_u32 i = 0; i < num_row; i++)
    {
        if(m_row_header[i]->item->IsParentOpen())
        {
            if((m_row_header[i]->item->m_RowHeader.GetBaseY()  <= y) &&
                (m_row_header[i]->item->m_RowHeader.GetBaseY() + m_row_header[i]->item->m_RowHeader.GetBaseHeight() > y))
            {
                row = i;
                sy = m_row_header[i]->item->m_RowHeader.GetBaseY();
                sh = m_row_header[i]->item->m_RowHeader.GetBaseHeight();
                break;
            }
        }
    }
    column = -1;
    t_u32 num_col = (t_u32)m_column_header.size();
    for(t_u32 i = 0; i < num_col; i++)
    {
        if((m_column_header[i].header.GetBaseX() <= x) && (m_column_header[i].header.GetBaseX() + m_column_header[i].header.GetBaseWidth() > x))
        {
            column = i;
            sx = m_column_header[i].header.GetBaseX();
            sw = m_column_header[i].header.GetBaseWidth();
            break;
        }
    }

    *ppItem = 0;
    if((row != -1) && (column != -1))
    {
        *ppItem = m_row_header[row]->item;
    }
}

void TableCtrl::OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    int previous_click_row = m_selectedRow;
    int previous_click_column = m_selectedColumn;

    if(m_selectedTableItem)
    {
        // Dirty the previously selected item. No need to call NeedRedraw because this is a mouse down and the 
        // WindowThread will call Draw(event, false) on a INL_MOUSE_PRESSED event.
        // When this item redraws, it will draw without the selection background color.
        m_selectedTableItem->setDirtyItem(true);
    }

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
        sx = m_column_header[m_selectedColumn].header.GetBaseX();
        sw = m_column_header[m_selectedColumn].header.GetBaseWidth();
        sy = m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseY();
        sh = m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseHeight();

        m_selectedGeometry = Geometry(sx, sy, sw, sh);
        sigItemSelected.emit(m_selectedRow, m_selectedColumn);
        sigTableItemSelected.emit(*this, *m_selectedTableItem, m_selectedRow, m_selectedColumn);

        // Dirty the newly selected item. No need to call NeedRedraw because this is a mouse down and the 
        // WindowThread will call Draw(event, false) on a INL_MOUSE_PRESSED event.
        // When this item redraws, it will draw with the selection background color.
        m_selectedTableItem->setDirtyItem(true);
    }

    if((m_selectedRow == -1) || (m_selectedColumn == -1))
        return;

    bool Reorganize = false;
    if(m_row_header[m_selectedRow]->item->FirstChildNode() || m_row_header[m_selectedRow]->item->AlwaysShowOpeningButton())
    {
        Geometry geo = m_row_header[m_selectedRow]->item->m_ItemGeometryVector[0];
        geo.SetX((m_bShowRowHeader? ROWHEADERWIDTH : 0) + ITEM_DEPTH_MARGIN * m_row_header[m_selectedRow]->item->m_depth);
        geo.SetY(m_row_header[m_selectedRow]->item->m_ItemGeometryVector[0].y - m_TableArea->GetBaseY());
        geo.SetWidth(OPENCLOSE_BTN_WIDTH);
        if(geo.IsPointInside(x, y))
        {
            if(m_row_header[m_selectedRow]->item->isOpen())
            {
                Reorganize = true;
                OpCloseItem(m_row_header[m_selectedRow]->item);
                if(IsSizeMatchContent())
                {
                    // when closing and item, the Table gets shorter and might leave a dirty area filled with part of the Table content.
                    // We need to call a PaintBackground on the area of the Table (area before the item is closed).
                    m_DrawBackgroundOnPreviousGeometry = true;
                    m_PreviousGeometry = GetGeometry();
                    // Initiate layout re computation from the top. This should be done with InitiateResizeLayout();
                    // but it is a private member of BaseObject. We can do it with a call to setGeometry(GetGeometry());
                    setGeometry(GetGeometry());
                }
            }
            else
            {
                Reorganize = true;
                OpOpenItem(m_row_header[m_selectedRow]->item);
            }
        }
        else
        {
            //do nothing 
            // You have to click on the Open/Close button to close the item
        }
    }

    if(Reorganize == true)
    {
        // The TableCtrl is completely dirty.
        // By calling NeedRedraw(), we make sure we go through TableCtrl::Draw() then TableCtrl::DrawContent() when drawing happens.
        ComputeChildLayout();
        PositionChildLayout(0, 0);
        NeedRedraw();
    }
}

//All Windows users are occasionally slowed by the need to double-click the mouse several times
//to launch an icon. Even after you have adjusted your mouse's Double Click Speed setting in the
//Windows Control Panel, you may still be plagued by double takes on your double-clicks. This is
//because there is a second, often overlooked setting that controls Windows' ability to register
//a double-click. By default, Windows allows the mouse to move the distance of only two pixels
//during a double-click. If you move your mouse more than two pixels in any direction between
//mouse clicks, Windows will interpret this as two single clicks instead of a single double-click.
//As monitor resolutions get higher and pixel density increases, jerky mouse movements are even 
//more susceptible to double-click failures.
//Fortunately, you can increase the two-pixel limit by:
//Open Start Menu/Control Panel/Mouse/Buttons Tab Click on Options and choose a higher number, 
//(5-12), and you should begin to notice that Windows is no longer ignoring as many double-clicks
//as it did before.

void TableCtrl::OnMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    if((m_selectedRow == -1) || (m_selectedColumn == -1))
    {
        // The double click becomes a simple click if we know in advance that the first click didn't select anything.
        //OnMouseDown(x, y, button_flags, key_flags);
        return;
    }

    int previous_click_row = m_selectedRow;
    int previous_click_column = m_selectedColumn;

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
        sx = m_column_header[m_selectedColumn].header.GetBaseX();
        sw = m_column_header[m_selectedColumn].header.GetBaseWidth();
        sy = m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseY();
        sh = m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseHeight();

        m_selectedGeometry = Geometry(sx, sy, sw, sh);
        // we couldsend a signal meaning a double click has happened on an item.
        //sigItemDoubleClick.emit(m_row_header[m_selectedRow]->item);
    }

    bool Reorganize = false;
    // Check if item as a child node. If not, there is no point in opening/closing it.
    if(m_row_header[m_selectedRow]->item->FirstChildNode() || m_row_header[m_selectedRow]->item->AlwaysShowOpeningButton())
    {
        Geometry geo = m_row_header[m_selectedRow]->item->m_ItemGeometryVector[0];
        geo.SetX((m_bShowRowHeader? ROWHEADERWIDTH : 0) + ITEM_DEPTH_MARGIN * m_row_header[m_selectedRow]->item->m_depth);
        geo.SetY(m_row_header[m_selectedRow]->item->m_ItemGeometryVector[0].y - m_TableArea->GetBaseY());
        geo.SetWidth(OPENCLOSE_BTN_WIDTH);
        if(geo.IsPointInside(x, y))
        {
            Reorganize = true;
            OnMouseDown(x, y, button_flags, key_flags);
        }
        else
        {
            if(m_row_header[m_selectedRow]->item->isOpen())
            {
                Reorganize = true;
                OpCloseItem(m_row_header[m_selectedRow]->item);
                if(IsSizeMatchContent())
                {
                    // when closing and item, the Table gets shorter and might leave a dirty area filled with part of the Table content.
                    // We need to call a PaintBackground on the area of the Table (area before the item is closed).
                    m_DrawBackgroundOnPreviousGeometry = true;
                    m_PreviousGeometry = GetGeometry();
                    // Initiate layout re computation from the top. This should be done with InitiateResizeLayout();
                    // but it is a private member of BaseObject. We can do it with a call to setGeometry(GetGeometry());
                    setGeometry(GetGeometry());
                }
            }
            else
            {
                Reorganize = true;
                OpOpenItem(m_row_header[m_selectedRow]->item);
            }
        }
    }
    if(Reorganize)
    {
        FormatTable();
        ComputeChildLayout();
        NeedRedraw();
    }
}

void TableCtrl::OnMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    return;
    int previous_click_row = m_selectedRow;
    int previous_click_column = m_selectedColumn;

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
        sx = m_column_header[m_selectedColumn].header.GetBaseX();
        sw = m_column_header[m_selectedColumn].header.GetBaseWidth();
        sy = m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseY();
        sh = m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseHeight();

        m_selectedGeometry = Geometry(sx, sy, sw, sh);
        // These signals are called at the mouse down on an item. Do not call them here.
        //sigItemSelected.emit(m_selectedRow, m_selectedColumn);
        //sigTableItemSelected.emit(*this, *m_selectedTableItem, m_selectedRow, m_selectedColumn);
    }
}

void TableCtrl::OnMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    return;
    TableItem* item = 0;
    if(m_selectedTableItem)
        m_selectedTableItem->setDirtyItem(true);
    m_selectedTableItem = 0;
    FindItemUnderPointer(x, y, &m_selectedTableItem, m_selectedRow, m_selectedColumn);

    if((m_selectedRow != -1) && (m_selectedColumn != -1))
    {
        // selected item geometry
        int sx, sy, sw, sh;
        sx = m_column_header[m_selectedColumn].header.GetBaseX();
        sw = m_column_header[m_selectedColumn].header.GetBaseWidth();
        sy = m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseY();
        sh = m_row_header[m_selectedRow]->item->m_RowHeader.GetBaseHeight();

        m_selectedTableItem->setDirtyItem(true);
    }

    NeedRedraw();
}

void TableCtrl::OnKeyboardFocus()
{

}
void TableCtrl::OnLostKeyboardFocus()
{

}

void TableCtrl::OnResizeHeaderMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos)
{
    //m_column_header_preview = m_column_header;
    m_point0.Set(x, y);
}

void TableCtrl::OnResizeHeaderMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos)
{
    //m_column_header = m_column_header_preview;
    //m_column_header_preview.clear();
    GetThreadWindowCompositor().SetWidgetDrawingOverlay(smptr(BaseArea)(0), GetThreadWindowCompositor().GetCurrentWindow());
    if(true)
    {   
        FormatTable();
        //ComputeChildLayout();
    }

    NeedRedraw();
}

void TableCtrl::OnResizeHeaderMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos)
{
    GetThreadWindowCompositor().SetWidgetDrawingOverlay(smptr(BaseArea)(this, false), GetThreadWindowCompositor().GetCurrentWindow());

    bool recompute = false;

    t_u32 num_header = (t_u32)m_column_header.size();
    if((isFloatingColumn() == false) && (header_pos >= m_column_header.size() - 1))
    {
        // The columns takes all available space. The last column size handler
        // is fix(always at the extreme right of the available space.
            return;
    }

    Geometry geo = m_column_header[header_pos].header.GetGeometry();
    
    int deltax = 0;
    if(isFloatingColumn() == true)
    {
        recompute = false;
        if((dx > 0) && (x > m_point0.x))
        {
            geo.OffsetSize( x - m_point0.x, 0);
            recompute = true;
        }

        if((dx < 0) && (x < m_point0.x))
        {
            geo.OffsetSize(x - m_point0.x, 0);
            recompute = true;
        }

        deltax = x - m_point0.x;
        int w = m_column_header[header_pos].header.GetBaseWidth() + deltax;
        m_column_header[header_pos].header.SetBaseWidth(w);
        if(m_column_header[header_pos].header.GetBaseWidth() < MIN_COLUMN_WIDTH)
        {
            m_column_header[header_pos].header.SetBaseWidth(MIN_COLUMN_WIDTH);
        }

    }

    if(isFloatingColumn() == false)
    {
        // Need to find the next element who can absorb the difference
        bool findElement = false;
        int element_pos;
        int deltax = x - m_point0.x;

        if(m_column_header[header_pos].bFixWidth == true)
        {
            bool found = false;
            for(int i = header_pos - 1; i >= 0; i--)
            {
                if(m_column_header[i].bFixWidth == false)
                {
                    header_pos = i;
                    found = true;
                    break;
                }
            }
            if(found == false)
                return;
        }
        int header_width = m_column_header[header_pos].header.GetBaseWidth();


        for(t_u32 i = header_pos + 1; ((i < num_header) && (deltax != 0)); i++)
        {
            t_u32 element_width =(t_u32) m_column_header[i].header.GetBaseWidth();
            if((m_column_header[i].bFixWidth == false) /*&& (element_width > 10)*/)
            {
                findElement = true;
                element_pos = i;

                if((dx > 0) && (x > m_point0.x))
                {
                    // Increasing the width of the header; delta is > 0.

                    int absorbed_width = 0;
                    if(element_width - deltax < MIN_COLUMN_WIDTH)
                    {
                        absorbed_width = element_width - MIN_COLUMN_WIDTH;
                    }
                    else
                    {
                        // the element can absorb the totality of deltax
                        absorbed_width = deltax;
                    }

                    int w = m_column_header[header_pos].header.GetBaseWidth() + absorbed_width;
                    m_column_header[header_pos].header.SetBaseWidth(w);

                    int x = m_column_header[element_pos].header.GetBaseX() +  absorbed_width;
                    m_column_header[element_pos].header.SetBaseX(x);
                    w = m_column_header[element_pos].header.GetBaseWidth() -  absorbed_width;
                    m_column_header[element_pos].header.SetBaseWidth(w);

                    for(int j = header_pos + 1; j < element_pos; j++)
                    {
                        int x = m_column_header[j].header.GetBaseX() +  absorbed_width;
                        m_column_header[j].header.SetBaseX(x);
                    }

                    recompute = true;
                    deltax = deltax - absorbed_width;
                }

                if((dx < 0) && (x < m_point0.x))
                {
                    // Decreasing the width of the header; delta is < 0.

                    int absorbed_width = 0;
                    header_width = m_column_header[header_pos].header.GetBaseWidth();
                    if(header_width + deltax < MIN_COLUMN_WIDTH)
                    {
                        absorbed_width = header_width - MIN_COLUMN_WIDTH;
                    }
                    else
                    {
                        // the element can absorb the totality of deltax
                        absorbed_width = -deltax;
                    }

                    int w = m_column_header[header_pos].header.GetBaseWidth() - absorbed_width;
                    m_column_header[header_pos].header.SetBaseWidth(w);

                    int x = m_column_header[element_pos].header.GetBaseX() -  absorbed_width;
                    m_column_header[element_pos].header.SetBaseX(x);
                    w = m_column_header[element_pos].header.GetBaseWidth() +  absorbed_width;
                    m_column_header[element_pos].header.SetBaseWidth(w);

                    for(int j = header_pos + 1; j < element_pos; j++)
                    {
                        int x = m_column_header[j].header.GetBaseX() -  absorbed_width;
                        m_column_header[j].header.SetBaseX(x);
                    }

                    recompute = true;
                    deltax = deltax + absorbed_width;
                }
            }
        }
    }

    if(recompute == true)
    {
        FormatHeader();
    }

    //NeedRedraw();
}

void TableCtrl::OnResizeRowMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos)
{
    m_point1.Set(x, y);

    char s[42];
    sprintf(s, "%d ", header_pos);
    //OutputDebugString(s);
}

void TableCtrl::OnResizeRowMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos)
{
    bool recompute = false;
    Geometry geo = m_row_header[header_pos]->item->m_RowHeader.GetGeometry();

    if((dy > 0) && (y > m_point1.y))
    {
        geo.OffsetSize(0, y - m_point1.y);
        recompute = true;
    }
    if((dy < 0) && (y < m_point1.y))
    {
        geo.OffsetSize(0, y - m_point1.y);
        recompute = true;
    }
    m_row_header[header_pos]->item->m_RowHeader.setGeometry(geo);
    if(m_row_header[header_pos]->item->m_RowHeader.GetBaseHeight() < MIN_COLUMN_WIDTH)
    {
        m_row_header[header_pos]->item->m_RowHeader.SetBaseHeight(MIN_COLUMN_WIDTH);
    }

    if(recompute == true)
    {   
        FormatTable();
        ComputeChildLayout();
    }

    NeedRedraw();
}

void TableCtrl::PreLayoutManagement()
{
    // This will configure sizes of different elements of the window. Including the size of the layout.
    ScrollView::PreLayoutManagement();
    if(isFloatingColumn() == false)
    {
        // This will adjust the width of header according to the new size.
        AdjustNonFloatingColumn();
    }
    // This is going to resize the m_TableArea according to the header width and the height
    // of individual rows summed up together.
    FormatTable();
    // During the layout computation, the m_VLayout (which has a stretch factor of 1) will respect the size of m_TableArea->
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of m_VLayout.
long TableCtrl::PostLayoutManagement(long LayoutResult)
{
    // Set the ViewContext Size and position before going into ScrollView::PostLayoutManagement.
    // this is needed for SetSizeMatchContent() to work correctly.
    m_ViewContentX = m_TableArea->GetBaseX();
    m_ViewContentY = m_TableArea->GetBaseY();
    m_ViewContentWidth = m_TableArea->GetBaseWidth();
    m_ViewContentHeight = m_TableArea->GetBaseHeight();

    long result = ScrollView::PostLayoutManagement(LayoutResult);
    if(isFloatingColumn() == false)
    {
        AdjustNonFloatingColumn();
    }
    ScrollView::PositionChildLayout(0, 0);
    FormatTable();
    return result;
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of m_VLayout.
void TableCtrl::PositionChildLayout(float offsetX, float offsetY)
{
    ScrollView::PositionChildLayout(offsetX, offsetY);
    FormatTable();
}

void TableCtrl::ScrollLeft(float stepx, int mousedx)
{
    ScrollView::ScrollLeft(stepx, mousedx);
    FormatTable();
}

void TableCtrl::ScrollRight(float stepx, int mousedx)
{
    ScrollView::ScrollRight(stepx, mousedx);
    FormatTable();
}

void TableCtrl::ScrollUp(float stepy, int mousedy)
{
    ScrollView::ScrollUp(stepy, mousedy);
    FormatTable();
}

void TableCtrl::ScrollDown(float stepy, int mousedy)
{
    ScrollView::ScrollDown(stepy, mousedy);
    FormatTable();
}

void TableCtrl::ShowRowHeader(bool b)
{
    m_bShowRowHeader = b;
    FormatTable();
}

void TableCtrl::ShowColumnHeader(bool b)
{
    m_bShowColumnHeader = b;
    FormatTable();
}

void TableCtrl::ShowVerticalSeparationLine(bool b)
{
    m_bShowVerticalSeparationLine = b;
    FormatTable();
}

void TableCtrl::ShowHorizontalSeparationLine(bool b)
{
    m_bShowHorizontalSeparationLine = b;
    FormatTable();
}

void TableCtrl::ShowSeparationLine(bool bHor, bool bVert)
{
    m_bShowHorizontalSeparationLine = bHor;
    m_bShowVerticalSeparationLine = bVert;
    FormatTable();
}

void TableCtrl::SetFloatingColumn(bool b)
{
    m_FloatingColumn = b;
    if(m_FloatingColumn == false)
    {
        ComputeNonFloatingColumn();
    }
    FormatTable();
}

bool TableCtrl::isFloatingColumn()
{
    return m_FloatingColumn;
}

t_u32 TableCtrl::GetNumColumn()
{
    return (t_u32)m_column_header.size();
}

t_u32 TableCtrl::GetNumRow()
{
    return (t_u32)m_row_header.size();
}

void TableCtrl::EnableApplyItemBestHeight(bool b)
{
    m_bEnableItemBestHeight = b;
}

void TableCtrl::ForceStartFocusOnContentTable(int x, int y)
{
    m_TableArea->ForceStartFocus(x, y);
}

void TableCtrl::EmptyTable()
{
    m_TableItemHead->DeleteTree();
    NeedRedraw();
}

void TableCtrl::NeedRedraw()
{
    nuxAssertNoRecursion();
    ActiveInterfaceObject::NeedRedraw();
}

void TableCtrl::HighlightItem(int row, int column)
{
    m_selectedRow = row;
    m_selectedColumn = column;
    NeedRedraw();
}


NAMESPACE_END_GUI