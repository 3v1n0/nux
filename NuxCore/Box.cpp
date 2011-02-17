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

#include "NuxCore.h"
#include "Point.h"
#include "Size.h"
#include "Box.h"

namespace nux
{
  Box::Box()
  {
    _left   = 0;
    _top    = 0;
    _bottom = 0;
    _right  = 0;
  }

  Box::Box (t_int32 left, t_int32 top, t_int32 right, t_int32 bottom)
  {
    _left   = left;
    _top    = top;
    _bottom = bottom;
    _right  = right;
  }

  //----------------------------------------------------------------------------
  Box::~Box()
  {

  }

  //----------------------------------------------------------------------------
  Box::Box (const Box &b)
  {
    _left   = b._left;
    _top    = b._top;
    _bottom = b._bottom;
    _right  = b._right;
  }

  //----------------------------------------------------------------------------
  Box &Box::operator = (const Box &b)
  {
    if (&b == this)
      return *this;

    _left   = b._left;
    _top    = b._top;
    _bottom = b._bottom;
    _right  = b._right;
    return *this;
  }

  //----------------------------------------------------------------------------
  t_bool Box::operator == (const Box &b) const
  {
    if ( (_left == b._left) && (_top == b._top) && (_bottom == b._bottom) && (_right == b._right) )
    {
      return true;
    }

    return false;
  }

  //----------------------------------------------------------------------------
  t_bool Box::operator != (const Box &b) const
  {
    if ( (_left == b._left) && (_top == b._top) && (_bottom == b._bottom) && (_right == b._right) )
    {
      return false;
    }

    return true;
  }

  void Box::Set (t_int32 left, t_int32 top, t_int32 right, t_int32 bottom)
  {
    _left   = left;
    _top    = top;
    _bottom = bottom;
    _right  = right;
  }
}

