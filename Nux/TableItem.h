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


#ifndef TABLEITEM_H
#define TABLEITEM_H

#include "NodeItem.h"
#include "NodeNetProtocol.h"

NAMESPACE_BEGIN_GUI

class TableItem;
class TableCtrl;
class TreeView;

extern const int ITEMDEFAULTWIDTH;
extern const int ITEMDEFAULTHEIGHT;
extern Color GPropertyItemTextColor0;
extern Color GPropertyItemTextColor1;

typedef struct
{
    CoreArea header;
    bool bFixWidth;
    int FixWidthValue;
} header2;

typedef struct
{
    TableItem* item;
} RowHeader;

class TableItem: public NodeNetCom
{
public:
    TableItem(const TCHAR* name, NodeParameterType type = NODE_TYPE_STATICTEXT);
    virtual ~TableItem();

    virtual long ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, RowHeader* row, const std::vector<header2>& column_vector, Color ItemBackgroundColor = Color(0x0));
    virtual void ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector){};
    virtual int GetItemBestHeight();
    virtual void SetPropertyItemWidth();

    void SetItemTextColor(const Color& color)
    {
        m_PropertyTextColor = color.Clone();
    }

    void SetItemTextColor(const Color* color)
    {
        if(color == 0)
        {
            NUX_SAFE_DELETE(m_PropertyTextColor);
        }
        else
        {
            NUX_SAFE_DELETE(m_PropertyTextColor);
            m_PropertyTextColor = color->Clone();
        }
    }

    const Color& GetItemTextColor()
    {
        if(m_PropertyTextColor)
            return *m_PropertyTextColor;
        else
            return GPropertyItemTextColor0;
    }

    virtual void SetBackgroundColor(Color c) { m_ItemBackgroundColor = c; }
    Color GetBackgroundColor() const { return m_ItemBackgroundColor; }

    /*!
        Set if this item always show an opening button even if it has no children
    */
    void SetAlwaysShowOpeningButton(bool b) {m_AlwaysShowOpeningButton = b;}
    /*!
        Return true if we must always show an opening button at the left of this item.
    */
    bool AlwaysShowOpeningButton() {return m_AlwaysShowOpeningButton;}
    /*!
        Open the item. The direct children of this item will have their flag m_bParentOpen set to true.
        The other children flag depends on their direct parent.
    */
    void Open() {m_isOpen = true;}
    /*!
        Close the item. All children (direct and indirect) of this item will have their flag m_bParentOpen set to false.
    */
    void Close() {m_isOpen = false;}
    /*!
        @return True if the item is open. 
    */
    bool isOpen() const {return m_isOpen;}
    /*!
        A parent of this item is closed. Set the item flag m_bParentOpen to false.
    */
    void Hide() {m_bParentOpen = false;}
    /*!
        All parents of this item are open. Set the item flag m_bParentOpen to true.
    */
    void Show() {m_bParentOpen = true;}
    /*!
        @return True if all parents (direct or indirect) of this item are open. False otherwise.
    */
    bool IsParentOpen() const {return m_bParentOpen;};
    void setDirtyItem(bool b) {m_bDirty = b;}
    bool isDirtyItem() const {return m_bDirty;}

    void RequestPropertyRedraw()
    {
        m_PropertyRedraw = true;
    }

    virtual void      PushChildFront( NodeItem *child );
    virtual void      PushChildBack( NodeItem *child );
    virtual void      AddNextSibling( NodeItem *sibling );
    virtual void      AddPrevSibling( NodeItem *sibling );
    virtual void      Unlink( void );

    sigc::signal<void> sigCellFocus;
    sigc::signal<void> sigRowFocus;

    TableCtrl* GetTable() {return m_Table;}
protected:
    //  Only the Root node of the table has this value defined. Every other item should have it set to NULL;
    TableCtrl* m_Table;
    bool m_isOpen;
    //! This parameter is true if the item is not hidden by its hierarchy. That is a parent (direct or indirect parent) of this item is not closed.
    bool m_bParentOpen;

    int m_x;
    int m_y;
    int m_height;
    int m_width;
    int m_depth;

    bool m_bDirty;

    bool m_PropertyRedraw;
    Color m_ItemBackgroundColor;
    Color* m_PropertyTextColor;

    bool  m_AlwaysShowOpeningButton;

    //! Geometry of the header of the row. Located at the left of the row.
    CoreArea m_RowHeader;
    //! Array of geometries of the column inside the row.
    std::vector<Geometry> m_ItemGeometryVector;
    //Geometry m_FirstColumnInRowGeometry;
    Geometry m_RowHeaderGeometry;
    Geometry m_FirstColumnUsableGeometry;
    Geometry m_TotalGeometry;

    bool m_bIsMouseInside;
    bool m_bIsFirstVisibleItem;
    bool m_bIsLastVisibleItem;

    friend class TableCtrl;
    friend class TreeControl;
    friend class ListControl;
};

NAMESPACE_END_GUI

#endif // TABLEITEM_H

