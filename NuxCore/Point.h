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


#ifndef POINT_H
#define POINT_H

namespace nux
{

  class Point
  {
  public:
    Point();
    Point (int x, int y);
    ~Point();
    Point (const Point &p);

    Point &operator = (const Point &p);
    bool operator == (const Point &p) const;
    bool operator != (const Point &p) const;
    Point operator + (const Point &p) const;
    Point operator - (const Point &p) const;
    Point &operator += (const Point &p);
    Point &operator -= (const Point &p);

    void Set (int x_, int y_)
    {
      x = x_;
      y = y_;
    }
    void Get (int &x_, int &y_)
    {
      x_ = x;
      y_ = y;
    }

    int x;
    int y;
  };

}
#endif // POINT_H


