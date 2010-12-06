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
    width_  = w;
    height_ = h;
  }

//----------------------------------------------------------------------------
  Size::~Size()
  {

  }

//----------------------------------------------------------------------------
  Size::Size (const Size &s)
  {
    width_ = s.width_;
    height_ = s.height_;
  }

//----------------------------------------------------------------------------
  Size &Size::operator = (const Size &s)
  {
    width_ = s.width_;
    height_ = s.height_;
    return *this;
  }

  t_bool Size::operator== (const Size &s) const
  {
    if ( (width_ == s.width_) && (height_ == s.height_) )
    {
      return true;
    }

    return false;
  }

//----------------------------------------------------------------------------
  t_bool Size::operator!= (const Size &s) const
  {
    if ( (width_ == s.width_) && (height_ == s.height_) )
    {
      return false;
    }

    return true;
  }

//----------------------------------------------------------------------------
  Size Size::operator+ (const Size &s) const
  {
    Size size;
    size.width_ = width_ + size.width_;
    size.height_ = height_ + size.height_;
    return size;
  }

//----------------------------------------------------------------------------
  Size Size::operator- (const Size &s) const
  {
    Size size;
    size.width_ = width_ - size.width_;
    size.height_ = height_ - size.height_;
    return size;
  }

//----------------------------------------------------------------------------
  t_int32 Size::GetWidth() const
  {
    return width_;
  }

//----------------------------------------------------------------------------
  t_int32 Size::GetHeight() const
  {
    return height_;
  }

//----------------------------------------------------------------------------
  void Size::SetWidth (t_int32 w)
  {
    width_ = w;
  }

//----------------------------------------------------------------------------
  void Size::SetHeight (t_int32 h)
  {
    height_ = h;
  }

//----------------------------------------------------------------------------

}
