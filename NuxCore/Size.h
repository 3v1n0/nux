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


#ifndef SIZE_H
#define SIZE_H

#include "SystemTypes.h"

namespace nux
{

  class Size
  {
  public:
    Size (t_int32 w = 0, t_int32 h = 0);
    ~Size();
    Size (const Size &s);

    Size &operator = (const Size &s);
    t_bool operator == (const Size &s) const;
    t_bool operator != (const Size &s) const;
    Size operator + (const Size &p) const;
    Size operator - (const Size &p) const;


    t_int32 GetWidth() const;
    t_int32 GetHeight() const;
    void SetWidth (t_int32 w);
    void SetHeight (t_int32 h);

  public:
    t_int32 width;
    t_int32 height;
  };

}

#endif // SIZE_H

