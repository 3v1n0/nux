/*
 * Copyright 2010-2012 Inalogic® Inc.
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

#include "../NuxCore.h"
#include "MathInc.h"


namespace nux
{

  int PointInside2DPolygon(Point2* polygon, int n, Point2 pt, const int onedge)
  {
    nuxAssert(n > 3);
    if(n < 3)
      return 0;
    //cross points count of x
    int __count = 0;

    //neighbor bound vertices
    Point2D<float> p1, p2;

    //left vertex
    p1 = polygon[0];

    //check all rays
    for(int i = 1; i <= n; ++i)
    {
      //point is an vertex
      if(pt == p1) return onedge;

      //right vertex
      p2 = polygon[i % n];

      //ray is outside of our interests
      if(pt.y < Min<float>(p1.y, p2.y) || pt.y > Max<float>(p1.y, p2.y))
      {
        //next ray left point
        p1 = p2; continue;
      }

      //ray is crossing over by the algorithm (common part of)
      if(pt.y > Min<float>(p1.y, p2.y) && pt.y < Max<float>(p1.y, p2.y))
      {
        //x is before of ray
        if(pt.x <= Max<float>(p1.x, p2.x))
        {
          //overlies on a horizontal ray
          if(p1.y == p2.y && pt.x >= Min<float>(p1.x, p2.x)) return onedge;

          //ray is vertical
          if(p1.x == p2.x)
          {
            //overlies on a ray
            if(p1.x == pt.x) return onedge;
            //before ray
            else ++__count;
          }

          //cross point on the left side
          else
          {
            //cross point of x
            double xinters = (pt.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;

            //overlies on a ray
            if(fabs(pt.x - xinters) < constants::dbl_epsilon) return onedge;

            //before ray
            if(pt.x < xinters) ++__count;
          }
        }
      }
      //special case when ray is crossing through the vertex
      else
      {
        //p crossing over p2
        if(pt.y == p2.y && pt.x <= p2.x)
        {
          //next vertex
          const Point2& p3 = polygon[(i+1) % n];

          //pt.y lies between p1.y & p3.y
          if(pt.y >= Min<float>(p1.y, p3.y) && pt.y <= Max<float>(p1.y, p3.y))
          {
            ++__count;
          }
          else
          {
            __count += 2;
          }
        }
      }

      //next ray left point
      p1 = p2;
    }

    //EVEN
    if(__count % 2 == 0) return(0);
    //ODD
    else return(1);
  }
}

