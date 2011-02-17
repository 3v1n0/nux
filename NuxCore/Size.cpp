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
#include "Size.h"

namespace nux
{

  Size::Size (t_int32 w, t_int32 h)
  {
    width  = w;
    height = h;
  }

  Size::~Size()
  {

  }

  Size::Size (const Size &s)
  {
    width = s.width;
    height = s.height;
  }

  Size &Size::operator = (const Size &s)
  {
    width = s.width;
    height = s.height;
    return *this;
  }

  t_bool Size::operator== (const Size &s) const
  {
    if ( (width == s.width) && (height == s.height) )
    {
      return true;
    }

    return false;
  }

  t_bool Size::operator!= (const Size &s) const
  {
    if ( (width == s.width) && (height == s.height) )
    {
      return false;
    }

    return true;
  }

  Size Size::operator+ (const Size &s) const
  {
    Size size;
    size.width = width + size.width;
    size.height = height + size.height;
    return size;
  }

  Size Size::operator- (const Size &s) const
  {
    Size size;
    size.width = width - size.width;
    size.height = height - size.height;
    return size;
  }

  t_int32 Size::GetWidth() const
  {
    return width;
  }

  t_int32 Size::GetHeight() const
  {
    return height;
  }

  void Size::SetWidth (t_int32 w)
  {
    width = w;
  }

  void Size::SetHeight (t_int32 h)
  {
    height = h;
  }
}
