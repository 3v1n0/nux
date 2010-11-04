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


#ifndef TABLECTRL_H
#define TABLECTRL_H

#include "TableItem.h"
#include "ScrollView.h"

namespace nux
{

  extern const int ROWHEADERWIDTH;
  extern const int OPENCLOSE_BTN_WIDTH;
  extern const int ITEM_DEPTH_MARGIN;
  extern const int MIN_COLUMN_WIDTH;
  extern const int TABLE_AREA_BOTTOM_SPACE;
  extern const int COLUMNHEADERHEIGHT;

  class VLayout;
  class HLayout;
  class TableCtrl;
  class TableItem;

  typedef CoreArea sizehandler2;

  class TableCtrl: public ScrollView
  {
  public:

    //! Constructor
    /*!
        Construct the TableCtrl class. The parameter <B><VAR>floating_column</VAR></B> columns can be resized past the size of the content view area.
        @param floating_column set the column as floating. Default is set to true
        @see SetFloatingColumn()
        @see isFloatingColumn()
    */
    TableCtrl (bool floating_column = true);
    virtual ~TableCtrl();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);
    void DrawTable (GraphicsEngine &GfxContext);
    void DrawHeader (GraphicsEngine &GfxContext);
    void DrawHeaderPreview (GraphicsEngine &GfxContext);
    void OverlayDrawing (GraphicsEngine &GfxContext);

    // API
    void addHeader (const TCHAR *name, bool fixed_width = false, int column_width = 10);
    void FormatTable();
    /*!
        @return The total width of the header;
    */
    int FormatHeader();

    //! Add a column to the table.
    /*!
        Add a column to the table.

        @param title        The title of the column.
        @param fixed_wided  True if the column should have a fixed width.
        @param width        The width of the column. This parameter is used if the width is not fixed.
    */
    virtual void AddColumn (const TCHAR *column_name, bool fixed_width = false, int column_width = 10);

  private:
    //! Add a row item. Used internally.
    /*!
        Add a row item.
    */
    virtual void AddItem (TableItem *item /*const TCHAR* item_name*/);
  public:
    typedef void (TableCtrl::*ItemOperator) (TableItem *);

    /*!
        Add an item has a direct child of the Root of the Table. The root of the Table is never displayed.
        Only its children are displayed.
        @param item     The item to add.
        @param isOpen   If TRUE, the item will .
    */
    void setTableItems (TableItem *item, bool isOpen = false);
    //! Add a table item as a child of the root node of the table controller.
    void AddTableItem (TableItem *item);
    /*!
        Repopulate the table starting at the root node. Recreate the m_row_header.
    */
    void ResetTable();
    void TraverseItem (TableItem *item,  bool open_only, ItemOperator);
    /*!
        Delete the row size handler. All elements are delete.
    */
    void DeleteRowSizeHandler();
    /*!
        Delete the row header. All elements are delete.
    */
    void DeleteRowHeader();

    //Item Operator
    int m_ItemX;
    int m_ItemY;
    int m_OpenItemTotalHeight;

    void OpBuildItemDepth (TableItem *item, int depth);
    void OpAddItem (TableItem *item);
    void OpCloseItem (TableItem *item);
    void OpOpenItem (TableItem *item);
    void OpHideItem (TableItem *item);
    void OpShowItem (TableItem *item);

    //! EnableResizing the height of row headers.
    /*!
        Enable the resizing to row headers by dragging the line at the bottom of the row.
    */
    virtual void EnableRowResizing (bool b)
    {
      m_bEnableRowResizing = b;
    }

    //! EnableResizing the width of column headers.
    /*!
        Enable the resizing to column headers by dragging the line at the right of the column.
    */
    virtual void EnableColumnResizing (bool b)
    {
      m_bEnableColumnResizing = b;
    }
    //! Show the row headers.
    /*!
        Show the row headers. Rows are numbered from 0 to the number of row - 1.
    */
    virtual void ShowRowHeader (bool b);
    //! Show the column headers.
    /*!
        Show the column headers. Columns are titled according to the name given when addColumn() is called.
    */
    virtual void ShowColumnHeader (bool b);

    //! Show the columns vertical separation lines.
    /*!
        Show the columns vertical separation lines.
        @param b If b is true the vertical separation lines are visible.
    */
    void ShowVerticalSeparationLine (bool b);

    //! Show the rows horizontal separation lines.
    /*!
        Show the rows horizontal separation lines.
        @param  b   If b is true the horizontal separation lines are visible.
    */
    void ShowHorizontalSeparationLine (bool b);

    void ShowSeparationLine (bool bHor, bool bVert);

    void ClickSelectRow();
    void ClickSelectColumn();
    void ClickSelectCell();

    //! Paint Decoration.
    /*!
        Paint the item hierarchy, open/close decoration in the first column.
        @param  item    The item whose decoration must be drawn.
    */
    virtual void PaintDecoration (GraphicsEngine &GfxContext, TableItem *item);

    //! Paint Item row header decoration.
    /*!
        @param  item    The item whose row header must be drawn.
    */
    virtual void PaintRowDecoration (GraphicsEngine &GfxContext, TableItem *item, Color color);
    //! Paint Item Background.
    /*!
        Paint Item Background. Also push the background on the painter background stack.

        @return The number of background pushed onto the painter stack.
    */
    UINT PushItemBackground (GraphicsEngine &GfxContext, TableItem *item, bool MouseOver = false);

    //! Pop the item background pushed on the painter stack.
    /*!
        Pop the item background pushed on the painter stack.

        @param NumBackground The number of background to pop of the painter stack.
    */
    void PopItemBackground (GraphicsEngine &GfxContext, UINT NumBackground);

    //! Enable  row to have a height fixed by the contained item.
    /*!
        Enable  row to have a height fixed by the contained item.
    */
    void EnableApplyItemBestHeight (bool b);

    //! Set ItemBackgroundColor when the mouse is over the Item.
    /*!
        Set ItemBackgroundColor when the mouse is over the Item.
        This color supersedes other background colors.
    */
    void SetMouseOverColor (Color color)
    {
      m_MouseOverColor = color;
    }
    //! Return the user defined color used as background color when the mouse is over the item.
    /*!
        @return the user defined color used as background color when the mouse is over the item.
    */
    Color GetMouseOverColor()
    {
      return m_MouseOverColor;
    }
    //! Enable the use of a user defined color when the mouse is over the item.
    /*!
        @param b If b is True the user defined color replaces the background color when the mouse is over the item.
    */
    void EnableMouseOverColor (bool b)
    {
      m_IsEnableMouseOverColor = b;
    }

    void SetRowColorOdd (Color color)
    {
      m_RowColorOdd = color;
    }

    Color GetRowColorOdd() const
    {
      return m_RowColorOdd;
    }

    void SetRowColorEven (Color color)
    {
      m_RowColorEven = color;
    }

    Color GetRowColorEven() const
    {
      return m_RowColorEven;
    }

    void SetRowColor (Color evencolor, Color oddcolor)
    {
      m_RowColorEven = evencolor;
      m_RowColorOdd = oddcolor;
    }
    void SetHorizontalSeparationLineColor (Color color)
    {
      m_HorizontalSeparationLineColor = color;
    }

    Color GetHorizontalSeparationLineColor() const
    {
      return m_HorizontalSeparationLineColor;
    }

    void SetVerticalSeparationLineColor (Color color)
    {
      m_VerticalSeparationLineColor = color;
    }

    Color GetVerticalSeparationLineColor() const
    {
      return m_VerticalSeparationLineColor;
    }

    void SetSeparationLineColor (const Color &horcolor, const Color &vertcolor)
    {
      m_HorizontalSeparationLineColor = horcolor;
      m_VerticalSeparationLineColor = vertcolor;
    }

    //! Return true if this object can break the layout.
    /*
        Return true if this object can break the layout, meaning, the layout can be done on the composition layout only without
        recomputing the whole window layout.
        Inherited from View
    */
    virtual bool CanBreakLayout()
    {
      if (IsSizeMatchContent() )
        return false;

      return true;
    }

  private:

    void UsedForComboBox (bool b)
    {
      m_IsPartOfComboBox = b;
    }
    bool IsPartOfCombobox() const
    {
      return m_IsPartOfComboBox;
    }
    bool m_IsPartOfComboBox;

    /*!
    When the tableCtrl is used in a combo box, it only shows the content and the TableArea is fully exposed.
    Sometimes we need force the focus on that TableArea.
    */
    void ForceStartFocusOnContentTable (int x, int y);

    Color m_MouseOverColor;
    bool m_IsEnableMouseOverColor;

  public:
    //! set Floating column.
    /*!
        Set if the header have floating columns or not. With floating column, the total width of
        the headers may be greater that the width of the content view area (the horizontal scrollbar will reflect that).
        With non floating columns, the sum of all headers width is exactly equal to the width of the content view area.

        @see isFloatingColumn()
    */
    void SetFloatingColumn (bool b);
  public:
    //! Return true if the columns are floating.
    /*!
        Return true if the columns are floating.

        @see SetFloatingColumn()
    */
    bool isFloatingColumn();

    //! Get information about the item below the pointer.
    /*!

    */
    void FindItemUnderPointer (int x, int y, TableItem **ppItem, int &row, int &column);

    void SetSelectionColor (const Color &color)
    {
      m_SelectionColor = color;
    }

    const Color GetSelectionColor()
    {
      return m_SelectionColor;
    }

    TableItem *GetSelectedItem()
    {
      return m_selectedTableItem;
    }

    t_u32 GetSelectedRow()
    {
      return m_selectedRow;
    }

    t_u32 GetSelectedColumn()
    {
      return m_selectedColumn;
    }

    //! Get the number of column in the table.
    /*!
        Get the number of column in the table.
        @return the number of column.
    */
    t_s32 GetNumColumn();

    //! Get the number of row in the table.
    /*!
        Get the number of row in the table.
        @return the number of row.
    */
    t_s32 GetNumRow();

    //! reset Items flags.
    /*!
        Reset the dirty flag of items.
    */
    void ResetItems();

    /*!
        Get Root Node.
        @return const TableItem*
    */
    const TableItem *GetRootNode() const
    {
      return m_TableItemHead;
    }
    /*!
        Get Root Node.
        @return TableItem*
    */
    TableItem *GetRootNode()
    {
      return m_TableItemHead;
    }

    /*!
        Get number of child element of the root node.
    */
    int GetNumRootChild() const
    {
      return m_TableItemHead->NumChild();
    }

    /*!
        Delete all elements of the tabled below the root node.
    */
    void EmptyTable();

    void HighlightItem (int row, int column);

    /////////////////
    //  EMITERS    //
    /////////////////
    // OnMouseDoubleClick and OnMouseDown have a custom implementation in TreeView. We need to make them virtual.
    virtual void OnMouseDoubleClick (int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void OnMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void OnKeyboardFocus();
    void OnLostKeyboardFocus();
    void OnResizeHeaderMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos);
    void OnResizeHeaderMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos);
    void OnResizeHeaderMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos);

    void OnResizeRowMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos);
    void OnResizeRowMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags, t_u32 header_pos);


    /////////////////
    //  SIGNALS    //
    /////////////////
    sigc::signal<void, int, int> sigItemSelected;
    sigc::signal<void, const TableItem &> sigItemChange;
    sigc::signal<void, TableCtrl &, TableItem &, int, int> sigTableItemSelected;

    sigc::signal<void> sigLostFocus;

  protected:

    // for debugging purpose to find out who called NeedReedraw for the TableCtrl.
    void NeedRedraw();

    virtual void SetGeometry (const Geometry &geo);
    friend class ComboBoxComplex;
    friend class ComboBoxSimple;

    /*!
        Distribute the width to headers equally, except for fix-width headers.
        ComputeNonFloatingColumn is called when a new column is added to the table.
    */
    void ComputeNonFloatingColumn();

    /*!
        This function member computes the size of the headers for a non-floating column table.
        Given the m_ViewWidth, the width of the view content area. AdjustNonFloatingColumn rescale the width of headers so they
        fit within the width of the view content area. The new width of each headers respect the its current scales compare to other headers.
        Fix-width columns remain unchanged.
    */
    void AdjustNonFloatingColumn();

    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement (long LayoutResult);
    virtual void PositionChildLayout (float offsetX, float offsetY);

    /////////////////
    //  RECEIVERS  //
    /////////////////
    virtual void ScrollLeft (float stepx, int mousedx);
    virtual void ScrollRight (float stepx, int mousedx);
    virtual void ScrollUp (float stepy, int mousedy);
    virtual void ScrollDown (float stepy, int mousedy);



  protected:
    CoreArea   *m_TableArea;
    VLayout    *m_VLayout;

    int m_tableNumRow;
    int m_tableNumColumn;

    int m_selectedRow;
    int m_selectedColumn;
    Geometry m_selectedGeometry;

    TableItem *m_selectedTableItem;
    TableItem *m_selectedMouseDownTableItem;
    TableItem *m_TableItemHead;

    bool m_bEnableRowResizing;
    bool m_bEnableColumnResizing;
    bool m_bShowRowHeader;
    bool m_bShowColumnHeader;
    bool m_bShowVerticalSeparationLine;
    bool m_bShowHorizontalSeparationLine;
    bool m_bEnableItemBestHeight;

    std::vector<RowHeader *> m_row_header;
    //std::vector<Geometry> m_column;
    std::vector<ColumnHeader> m_column_header;
    std::vector<ColumnHeader> m_column_header_preview;
    std::vector<sizehandler2 *> m_column_sizehandler;
    std::vector<sizehandler2 *> m_row_sizehandler;
    //std::vector<std::string> m_item;

    bool m_FloatingColumn;

    Color m_RowColorOdd;
    Color m_RowColorEven;
    Color m_SelectionColor;
    Color m_TableBottomColor;
    Color m_HorizontalSeparationLineColor;
    Color m_VerticalSeparationLineColor;

    // We need to draw the background on the previous size of the Table if its
    // size is set to match the content(IsSizeMatchContent) and an item is close.
    Geometry m_PreviousGeometry;
    bool m_DrawBackgroundOnPreviousGeometry;

    enum CELL_SELECT
    {
      CLICK_SELECT_CELL,
      CLICK_SELECT_COLUMN,
      CLICK_SELECT_ROW,
    };

    CELL_SELECT m_ClickSelect;
    //TableItem* m_TableItem;
    Point m_point0;
    Point m_point1;
  };


}

#endif // TABLECTRL_H
