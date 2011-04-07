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


#ifndef LISTCONTROL_H
#define LISTCONTROL_H

#include "TableItem.h"
#include "TableCtrl.h"

namespace nux
{

  class ListControl: public TableCtrl
  {
  public:
    ListControl (bool floating_column = true);
    virtual ~ListControl();

    //    // The following have been overwritten to do nothing.
    //    //! Add a column to the list.
    //    /*!
    //    Add a column to the list.
    //    */
    //    virtual void addColumn(const TCHAR* column_name, bool fixed_width = false, int column_width = 10){};
    //    //! EnableResizing the height of row headers.
    //    /*!
    //    Enable the resizing to row headers by dragging the line at the bottom of the row.
    //    */
    //    virtual void EnableRowResizing(bool b){}
    //
    //    //! EnableResizing the width of column headers.
    //    /*!
    //    Enable the resizing to column headers by dragging the line at the right of the column.
    //    */
    //    virtual void EnableColumnResizing(bool b){}
    //    //! Show the row headers.
    //    /*!
    //    Show the row headers. Rows are numbered from 0 to the number of row - 1.
    //    */
    //    virtual void ShowRowHeader(bool b){};
    //    //! Show the column headers.
    //    /*!
    //    Show the column headers. Columns are titled according to the name given when addColumn() is called.
    //    */
    //    virtual void ShowColumnHeader(bool b){};

  protected:

  private:

  };


}

#endif // LISTCONTROL_H
