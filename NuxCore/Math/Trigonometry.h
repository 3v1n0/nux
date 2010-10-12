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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef TRIGONOMETRY_H
#define TRIGONOMETRY_H

#include "Vector3.h"

namespace nux
{
// Assume the spherical coordinate system relatively to a right handed xyz,
// with Z pointing up.
// 0 <= phi < 180
// 0 <= theta < 360 ->>> along X axis, theta = 0.
  Vector3 SphericalToCartesianXBaseDeg (float r, float theta, float phi);
  Vector3 SphericalToCartesianXBaseRad (float r, float theta, float phi);

// Assume the spherical coordinate system relatively to a right handed xyz,
// with Y pointing up.
// 0 <= phi < 180
// 0 <= theta < 360 ->>> along Z axis, theta = 0.
  Vector3 SphericalToCartesianZBaseDeg (float r, float theta, float phi);
  Vector3 SphericalToCartesianZBaseRad (float r, float theta, float phi);

  Vector3 CartesianToSphericalXBaseRad (float x, float y, float z);
  Vector3 CartesianToSphericalZBaseDeg (float x, float y, float z);


}



#endif // TRIGONOMETRY_H