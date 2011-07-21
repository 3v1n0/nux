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


#ifndef QUATERNION_H
#define QUATERNION_H

#include "Vector3.h"
#include "Vector4.h"

namespace nux
{

  /***************************************************************************************\
  Class:       Quaternion

  Description:    Define basic quaternion initialization and function.

  Comments:       Member functions uses input angles in radian.
  \***************************************************************************************/
  class Quaternion
  {
  public:
    Quaternion();
    Quaternion (const Quaternion &s);
    Quaternion (const Vector3 &vec, float angle);
    Quaternion (const Vector4 &vec);

    // creates a Quaternion from an angle axis -- note that if angle > 2*PI the resulting
    // rotation is angle mod 2*PI
    Quaternion (float axis_x, float axis_y, float axis_z, float angle_radian);
    // creates a Quaternion from an angle-around-XYZ euler triple using roll-pitch-yaw order
    Quaternion (float euler_x, float euler_y, float euler_z);
    ~Quaternion();

    Quaternion &operator = (const Quaternion &quat);

    // binary operators
    Quaternion operator + (const Quaternion &quat) const;
    Quaternion operator - (const Quaternion &quat) const;
    Quaternion operator * (const Quaternion &quat) const;
    Quaternion operator * (const float &f) const;
    Quaternion operator / (const float &f) const;

    // assignment operators
    Quaternion &operator += (const Quaternion &quat);
    Quaternion &operator -= (const Quaternion &quat);
    Quaternion &operator *= (const Quaternion &quat);
    Quaternion &operator *= (const float &f);
    Quaternion &operator /= (const float &f);

    // unary operators
    Quaternion operator + () const;
    Quaternion operator - () const;
    //const Quaternion operator / (const Quaternion& quat);


    bool operator == ( const Quaternion & ) const;
    bool operator != ( const Quaternion & ) const;

    void Conjugate();
    void Inverse();
    void Normalize();
    float DotProduct (const Quaternion &quat) const;
    float Length() const;

    void GetAngleAxis (Vector3 &axis, float &angle_radian) const;        // fetches the angle/axis given by the quat

    // Fetches 4x4 homogeneous matrix given by the quat
    Matrix4 GetMatrix() const;

    float x, y, z, w;

    friend Quaternion operator * (float f, const Quaternion &quat);
  private:
    // set the quaternion by angle-axis (see AA constructor)
    void FromAngleAxis (float axis_x, float axis_y, float axis_z, float angle_radian);

    // set the quaternion by euler axis angles (see euler constructor)
    void FromEulerZXY (float euler_x, float euler_y, float euler_z);
  };

}

#endif // QUATERNION_H
