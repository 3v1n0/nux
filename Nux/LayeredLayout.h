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
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */


#ifndef GRIDHLAYOUT_H
#define GRIDHLAYOUT_H

#include "Layout.h"

namespace nux
{
  //! An horizontal grid layout.
  /*!
      Fills the grid from left to right and going down.
  */
  class LayeredLayout: public Layout
  {
    // The grid layout goes through the child elements and assign them a size and position.
    //  0   1   2   3   4   5
    //  6   7   8   9   10  11
    //  12  13  ..  ..  ..  ..
    // This is a left to right fill, going down.

    NUX_DECLARE_OBJECT_TYPE (LayeredLayout, Layout);
  public:
    LayeredLayout (NUX_FILE_LINE_PROTO);
    ~LayeredLayout ();

    virtual long ComputeLayout2 ();

    virtual void GetCompositeList (std::list<Area *> *ViewList);
    
    //! Control the visibility of elements on the bottom edge.
    /*!
        Controls how the layout places the elements at its bottom edge.
        @param partial_visibility If True, the layout will position elements at its bottom edge
        even if they are partially visible.
    */
    void EnablePartialVisibility (bool partial_visibility);

    //! Set the size of the grid element.
    /*!
        Set the size of the grid element.
        @param width  Width of elements.
        @param height Height of elements.
    */
    void SetChildrenSize (int width, int height);

    //! Get the size of the grid element.
    /*!
        @return Size of the grid elements.
    */
    Size GetChildrenSize () const;


    //! Force the grid elements size.
    /*!
        Force the grid elements size to be the one provided by SetChildrenSize.
    */
    void ForceChildrenSize (bool force);

    //! Get the number of columns in the grid.
    int GetNumColumn () const;

    //! Get the number of rows in the grid.
    int GetNumRow () const;

    //! Draw Element
    /*!
      Draw all elements inside the layout.
      If force_draw is true then the system requests that all objects redraw themselves completely.
      @param force_draw
      @param TraverseInfo
      @param ProcessEventInfo
      @return The state of the Process Event.
    */
    virtual void ProcessDraw (GraphicsEngine &GfxContext, bool force_draw);

  private:
    Size _children_size;
    bool _dynamic_column;
    bool _force_children_size;
    bool _partial_visibility;
    int _num_row;
    int _num_column;

//     //! Compute the how elements are spread inside the layout
//     /*!
//         @param remaining_width  Size that remains after subtracting elements width, inner and outer margins from the content width.
//         @param offset_space     The space at the left of all elements.
//         @param element_margin   The margin between elements.
//     */
//     void ComputeStacking (t_s32 remaining_width, t_s32 &offset_space, t_s32 &element_margin);
  };
}

#endif // GRIDHLAYOUT_H
