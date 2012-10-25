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


#ifndef VLAYOUT_H
#define VLAYOUT_H

#include "LinearLayout.h"

namespace nux
{

  class VLayout: public LinearLayout
  {
    NUX_DECLARE_OBJECT_TYPE(VLayout, LinearLayout);
  public:
    VLayout(NUX_FILE_LINE_PROTO);
    VLayout(std::string name, NUX_FILE_LINE_PROTO);
    ~VLayout();

    virtual long ComputeContentSize();
    virtual void ComputeContentPosition(float offsetX, float offsetY);
    virtual void VLayoutManagement(int width, int height);
    virtual unsigned int GetMaxStretchFactor();

    virtual void GetCompositeList(std::list<Area *> *ViewList);

    //! Compute the how elements are spread inside the layout
    /*!
        @param remaining_height Size that remains after subtracting elements height, inner and outer margins from the content height.
        @param offset_space     The space at the top of all elements.
        @param element_margin   The margin between elements.
    */
    void ComputeStacking(int length, int &offset_space, int &element_margin);

  protected:
    virtual Area* KeyNavIteration(KeyNavDirection direction);
  };
}

#endif // VLAYOUT_H
