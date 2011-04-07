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

#ifndef ALGO_H
#define ALGO_H

#include "Constants.h"
#include "Constants.h"


namespace nux
{
  //http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
  //! Determine if a point lies inside a polygon.
  /*!
      If the point is on an edge or a point of the polygon, return the parameter "OnEdge"
      @polygon The polygon to test.
      @n Number of points in the polygon
      @n pt The point to test.
      @onedge Value to return if the point lies on an edge or a vertex of the polygon
      @return 1 if the point is inside, 0 if it is outside, onedge if it is on the edges of the polygon.
  */
  int PointInside2DPolygon(Point2* polygon, int n, Point2 pt, const int onedge);

}

#endif // ALGO_H
