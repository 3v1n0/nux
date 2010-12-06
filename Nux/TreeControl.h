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


#ifndef TREECONTROL_H
#define TREECONTROL_H

#include "TableItem.h"
#include "TableCtrl.h"

namespace nux
{

  class TreeItem: public TableItem
  {
    //NODE_XML_CLASS_MEMBER(TreeItem);
  public:
    TreeItem (const TCHAR *name, NodeParameterType type = NODE_TYPE_STATICTEXT);
    virtual ~TreeItem();

    virtual long ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter, RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color (0x0) );
    virtual void ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector);
    virtual int GetItemBestHeight();
  };

  class TreeControl: public TableCtrl
  {
  public:
    TreeControl();
    virtual ~TreeControl();

    virtual void OnMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void OnMouseDoubleClick (int x, int y, unsigned long button_flags, unsigned long key_flags);

    //sigc::signal<void, TableCtrl&, TableItem&, int, int> sigFolderItemSelected;
  };


}

#endif // TREECONTROL_H
