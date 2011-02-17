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


#ifndef VLAYOUT_H
#define VLAYOUT_H

#include "Layout.h"

namespace nux
{

  class VLayout: public Layout
  {
    NUX_DECLARE_OBJECT_TYPE (VLayout, Layout);
  public:
    VLayout (NUX_FILE_LINE_PROTO);
    VLayout (NString name, NUX_FILE_LINE_PROTO);
    ~VLayout ();

    virtual long ComputeLayout2();
    virtual void ComputePosition2 (float offsetX, float offsetY);
    virtual void VLayoutManagement (t_s32 width, t_s32 height);
    virtual t_u32 getMaxStretchFactor();

    virtual void GetCompositeList (std::list<Area *> *ViewList);

    //! Compute the how elements are spread inside the layout
    /*!
        @param remaining_height Size that remains after subtracting elements height, inner and outer margins from the content height.
        @param offset_space     The space at the top of all elements.
        @param element_margin   The margin between elements.
    */
    void ComputeStacking (t_s32 length, t_s32 &offset_space, t_s32 &element_margin);
  };
}

#endif // VLAYOUT_H
