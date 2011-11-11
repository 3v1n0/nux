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


#ifndef LINEARLAYOUT_H
#define LINEARLAYOUT_H

#include "Layout.h"

namespace nux
{
  // The Space layout is a layout object that is used to create fixed or re-sizable empty space.
  class LinearLayout: public Layout
  {
    NUX_DECLARE_OBJECT_TYPE(LinearLayout, Layout);
  public:
    virtual void AddLayout(Layout *, unsigned int stretchFactor = 1, MinorDimensionPosition = eAbove, MinorDimensionSize extend = eFull, float percentage = 100.0f, LayoutPosition = NUX_LAYOUT_END);
 
     //! Add an object to the layout.
     /*! Add an object to the layout.
         A baseobject minor dimension with respect to a layout object is the dimension opposite to the layout flow.
         A baseobject major dimension with respect to a layout object is the dimension aligned with the layout flow.
         A layout object minor dimension is the dimension opposite to the layout flow.
         A layout object major dimension is the dimension aligned with the layout flow.
 
         Add an object to the layout. The added object get its size and position managed by the layout.
         When a baseobject is added with a stretches factor equal to 0, its major dimension assumes its minimum value.
         For instance, if the layout is a vertical layout and the added object has a stretch factor equal 0, then during the layout,
         the added object height will be set to its minimum value using ApplyMinHeight().
 
         The positioning parameter controls how the layout will place the object within itself. A vertical layout object controls the horizontal positioning
         of its children baseobject, While an horizontal layout object controls the vertical positioning of its children baseobject.
 
         The extend parameter controls how much size the baseobject minor dimension gets from the layout minor dimension. See MinorDimensionSize.
 
         /param baseobject The object that is being added.
         /param stretchFactor This value controls how the layout object share space between its children baseobject.
         /param positioning Controls how the layout position the object.
         /param extend Controls the object minor dimension size.
         /param percentage Controls the object minor dimension size in percentage of the layout minor dimension size.
         /param index Controls the object position in the layout.
     */
     virtual void AddView(Area *baseobject, unsigned int stretchFactor = 1, MinorDimensionPosition positioning = eAbove, MinorDimensionSize extend = eFull, float percentage = 100.0f, LayoutPosition index = NUX_LAYOUT_END);
     virtual void AddSpace(unsigned int width, unsigned int stretchFactor = 0, LayoutPosition index = NUX_LAYOUT_END);
 

    //! Deprecated. Use SetSpaceBetweenChildren;
    void SetHorizontalInternalMargin(int space);
    //! Deprecated. Use SetSpaceBetweenChildren;
    void SetVerticalInternalMargin(int space);

    //! Set the space between the children of a HLayout or VLayout.
    /*!
        Set the horizontal space between the children of the layout. In a VLayout, children of the layout are placed
        horizontally, on after the other, from left to right. This function set the space allowed between the children.
        Valid only for HLayout and VLayout.

        @param horizontal_space The horizontal space between the children of the layout.
    */
    void SetSpaceBetweenChildren(int space);

  protected:
    LinearLayout(NUX_FILE_LINE_PROTO);
    virtual ~LinearLayout();
  };
}

#endif // LINEARLAYOUT_H
