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


#ifndef GRIDVLAYOUT_H
#define GRIDVLAYOUT_H

#include "Layout.h"

namespace nux
{
  //! A vertical grid layout
  /*!
      Fills the grid from top to bottom and going right.
  */
  class GridVLayout: public Layout
  {
    // The grid layout goes through the child elements and assign them a size and position.
    //  0   3   6   9   ..
    //  1   4   7   10  ..
    //  2   5   8   ..
    // This is a top to bottom fill, going right.

    NUX_DECLARE_OBJECT_TYPE(GridVLayout, Layout);
  public:
    GridVLayout(NUX_FILE_LINE_PROTO);
    ~GridVLayout();

    virtual long ComputeContentSize();

    virtual void GetCompositeList(std::list<Area *> *ViewList);
    
    //! Control the visibility of elements on the bottom edge.
    /*!
        Controls how the layout places the elements at its bottom edge.
        @param partial_visibility If True, the layout will position elements at its bottom edge
        even if they are partially visible.
    */
    void EnablePartialVisibility(bool partial_visibility);

    //! Set the size of the grid element.
    /*!
        Set the size of the grid element.
        @param width  Width of elements.
        @param height Height of elements.
    */
    void SetChildrenSize(int width, int height);

    //! Get the size of the grid element.
    /*!
        @return Size of the grid elements.
    */
    Size GetChildrenSize() const;

    //! Force the grid elements size.
    /*!
        Force the grid elements size to be the one provided by SetChildrenSize.
    */
    void ForceChildrenSize(bool force);

    //! Get the number of columns in the grid.
    int GetNumColumn() const;
    
    //! Get the number of rows in the grid.
    int GetNumRow() const;

    //! Make the grid width match the size of its content.
    /*!
        @param match_content If True, force the width of the layout to match the height of the content. This can also be achieve 
        if the stretch factor of this layout is set to 0;
    */
    void SetWidthMatchContent(bool match_content);

    //! Return True if the grid width match the size of its content.
    bool GetWidthMatchContent() const;

    //! Draw Element
    /*!
      Draw all elements inside the layout.
      If force_draw is true then the system requests that all objects redraw themselves completely.
      @param force_draw
      @param TraverseInfo
      @param ProcessEventInfo
      @return The state of the Process Event.
    */
    virtual void ProcessDraw(GraphicsEngine &graphics_engine, bool force_draw);

  protected:
    int GetChildPos(Area *child);
    Area* GetChildAtPosition(int pos);
    virtual Area* KeyNavIteration(KeyNavDirection direction);

  private:
    Size _children_size;
    bool _dynamic_column;
    bool _force_children_size;
    bool _partial_visibility;
    bool _width_match_content;  //!< If True, for the width of the layout to match the width of the content.

    int _num_row;
    int _num_column;
  };
}

#endif // GRIDVLAYOUT_H
