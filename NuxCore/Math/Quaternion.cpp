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


#include "../NuxCore.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Quaternion.h"

// When writing to a matrix at row r and column c use m[r][c].
// When reading from a matrix at row r and column c use m[c][r].

namespace nux
{

  Quaternion::Quaternion()
  {
    x = y = z = w = 0.0f;
  }

  Quaternion::Quaternion (const Quaternion &quat)
  {
    x = quat.x;
    y = quat.y;
    z = quat.z;
    w = quat.w;
  }

  Quaternion::Quaternion (const Vector3 &vec, float angle)
  {
    FromAngleAxis (vec.x, vec.y, vec.z, angle);
  }

  Quaternion::Quaternion (const Vector4 &vec)
  {
    FromAngleAxis (vec.w, vec.x, vec.y, vec.z);
  }

  Quaternion::Quaternion (float axis_x, float axis_y, float axis_z, float angle_radian)
  {
    FromAngleAxis (axis_x, axis_y, axis_z, angle_radian);
  }

  Quaternion::Quaternion (float euler_x, float euler_y, float euler_z)
  {
    FromEulerZXY (euler_x, euler_y, euler_z);
  }

  Quaternion::~Quaternion()
  {
  }

  Quaternion &Quaternion::operator = (const Quaternion &quat)
  {
    x = quat.x;
    y = quat.y;
    z = quat.z;
    w = quat.w;

    return (*this);
  }

  Quaternion Quaternion::operator + (const Quaternion &quat) const
  {
    Quaternion qt;
    qt.x = x + quat.x;
    qt.y = y + quat.y;
    qt.z = z + quat.z;
    qt.w = w + quat.w;
    return qt;
  }

  Quaternion Quaternion::operator - (const Quaternion &quat) const
  {
    Quaternion qt;
    qt.x = x - quat.x;
    qt.y = y - quat.y;
    qt.z = z - quat.z;
    qt.w = w - quat.w;
    return qt;
  }

  Quaternion Quaternion::operator * (const Quaternion &quat) const
  {
    Quaternion qt;
    float x1, x2, y1, y2, z1, z2, w1, w2;

    x1 = x;
    y1 = y;
    z1 = z;
    w1 = w;
    x2 = quat.x;
    y2 = quat.y;
    z2 = quat.z;
    w2 = quat.w;

    qt.x = w1 * x2 + x1 * w2 - z1 * y2 + y1 * z2;
    qt.y = w1 * y2 + y1 * w2 + z1 * x2 - x1 * z2;
    qt.z = w1 * z2 + z1 * w2 + x1 * y2 - y1 * x2;
    qt.w = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2;

    return qt;
  }

  Quaternion Quaternion::operator * (const float &f) const
  {
    Quaternion qt;
    qt.x = x * f;
    qt.y = y * f;
    qt.z = z * f;
    qt.w = w * f;
    return qt;
  }

  Quaternion Quaternion::operator / (const float &f) const
  {
    Quaternion qt;
    qt.x = x / f;
    qt.y = y / f;
    qt.z = z / f;
    qt.w = w / f;
    return qt;
  }

//////////////////////////////////////////////////////////////////////////
  Quaternion &Quaternion::operator += (const Quaternion &quat)
  {
    x = x + quat.x;
    y = y + quat.y;
    z = z + quat.z;
    w = w + quat.w;
    return *this;
  }

  Quaternion &Quaternion::operator -= (const Quaternion &quat)
  {
    x = x - quat.x;
    y = y - quat.y;
    z = z - quat.z;
    w = w - quat.w;
    return *this;
  }

  Quaternion &Quaternion::operator *= (const Quaternion &quat)
  {
    Quaternion qt;
    float x1, x2, y1, y2, z1, z2, w1, w2;

    x1 = x;
    y1 = y;
    z1 = z;
    w1 = w;
    x2 = quat.x;
    y2 = quat.y;
    z2 = quat.z;
    w2 = quat.w;

    qt.x = w1 * x2 + x1 * w2 - z1 * y2 + y1 * z2;
    qt.y = w1 * y2 + y1 * w2 + z1 * x2 - x1 * z2;
    qt.z = w1 * z2 + z1 * w2 + x1 * y2 - y1 * x2;
    qt.w = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2;

    x = qt.x;
    y = qt.y;
    z = qt.z;
    w = qt.w;
    return *this;
  }

  Quaternion &Quaternion::operator *= (const float &f)
  {
    x = x * f;
    y = y * f;
    z = z * f;
    w = w * f;
    return *this;
  }

  Quaternion &Quaternion::operator /= (const float &f)
  {
    x = x / f;
    y = y / f;
    z = z / f;
    w = w / f;
    return *this;
  }


  Quaternion Quaternion::operator + () const
  {
    Quaternion qt;
    qt.x = x;
    qt.y = y;
    qt.z = z;
    qt.w = w;
    return qt;
  }

  Quaternion Quaternion::operator - () const
  {
    Quaternion qt;
    qt.x = -x;
    qt.y = -y;
    qt.z = -z;
    qt.w = -w;
    return qt;
  }

  bool Quaternion::operator == (const Quaternion &quat) const
  {
    if ( (x == quat.x) &&
         (y == quat.y) &&
         (z == quat.z) &&
         (w == quat.w) )
    {
      return true;
    }

    return true;
  }

  bool Quaternion::operator != ( const Quaternion &quat) const
  {
    return ! ( (*this) == quat);
  }

  void Quaternion::Conjugate()
  {
    x = -x;
    y = -y;
    z = -z;
  }

  void Quaternion::Inverse()
  {
    float len;
    len = (float) std::sqrt (x * x + y * y + z * z + w * w);
    Conjugate();
    x = x / len;
    y = y / len;
    z = z / len;
    w = w / len;
  }

  void Quaternion::Normalize()
  {
    float len;
    len = (float) std::sqrt (x * x + y * y + z * z + w * w);
    x = x / len;
    y = y / len;
    z = z / len;
    w = w / len;

  }

  /***************************************************************************************\
  Function:       Quaternion::dot_product

  Description:    Compute the dot product of *this and the input quaternion vector

  Parameters:     - quat

  Return Value:   float.

  Comments:       None.
  \***************************************************************************************/
  float Quaternion::DotProduct (const Quaternion &quat) const
  {
    float d_p;
    d_p = x * quat.x + y * quat.y + z * quat.z + w * quat.w;
    return d_p;
  }

  /***************************************************************************************\
  Function:       Quaternion::length

  Description:    Compute the length of a quaternion vector

  Parameters:     None.

  Return Value:   float

  Comments:       None.
  \***************************************************************************************/
  float Quaternion::Length() const
  {
    float len;
    len = (float) std::sqrt (x * x + y * y + z * z + w * w);
    return len;
  }

  /***************************************************************************************\
  Function:       Quaternion::FromAngleAxis

  Description:    Set the quaternion to define a rotation of angle_randian around the
                  vector define by (axis_x, axis_y, axis_z)

  Parameters:     - angle_radian: angle in radian
                  - axis_x
                  - axis_y
                  - axis_z

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  void Quaternion::FromAngleAxis (float axis_x, float axis_y, float axis_z, float angle_radian)
  {
    float ax = axis_x;
    float ay = axis_y;
    float az = axis_z;

    // required: Normalize the axis

    float len = 1.0f / (float) std::sqrt ( ax * ax + ay * ay + az * az );

    ax = ax * len;
    ay = ay * len;
    az = az * len;

    float sin_theta_over_two = (float) std::sin ( angle_radian / 2.0);
    x = ax * sin_theta_over_two;
    y = ay * sin_theta_over_two;
    z = az * sin_theta_over_two;
    w = (float) std::cos (angle_radian / 2.0);
  }

  /***************************************************************************************\
  Function:       Quaternion::FromEulerZXY

  Description:    Set the quaternion to define a Euler transform

  Parameters:     - euler_x: rotation angle around X axis (in radian)
                  - euler_y: rotation angle around Y axis (in radian)
                  - euler_z: rotation angle around Z axis (in radian)

  Return Value:   None.

  Comments:       The rotation are applied in the following order:
                      rotation around Y axis
                      rotation around X axis
                      rotation around Z axis
  \***************************************************************************************/
  void Quaternion::FromEulerZXY (float euler_x, float euler_y, float euler_z)
  {
    float roll_axis[3]  = {0.0f, 0.0f, 1.0f};
    float pitch_axis[3] = {1.0f, 0.0f, 0.0f};
    float yaw_axis[3]   = {0.0f, 1.0f, 0.0f};

    Quaternion roll (euler_z, roll_axis[0], roll_axis[1], roll_axis[2]);
    Quaternion pitch (euler_x, pitch_axis[0], pitch_axis[1], pitch_axis[2]);
    Quaternion yaw (euler_y, yaw_axis[0], yaw_axis[1], yaw_axis[2]);

    (*this) = roll * pitch * yaw;
  }

  void Quaternion::GetAngleAxis (Vector3 &axis, float &angle_radian) const
  {
    Quaternion qt;

    qt.x = x;
    qt.y = y;
    qt.z = z;
    qt.w = w;

    // make a unit quaternion
    qt.Normalize();                                 // qt = sin(angle/2)*Uq + cos(angle/2)
    angle_radian = 2.0f * (float) std::acos (qt.w);
    float one_over_sin = 1.0f / (float) std::sqrt (qt.x * qt.x + qt.y * qt.y + qt.z * qt.z);
    //float one_over_sin = 1.0f / (float) sin(angle_radian / 2.0f);

    axis.x = qt.x * one_over_sin;
    axis.y = qt.y * one_over_sin;
    axis.z = qt.z * one_over_sin;
  }

  /***************************************************************************************\
  Function:       Quaternion::get_matrix

  Description:    Return a Matrix4 object containing the rotation defined by the
                  quaternion.

  Parameters:     None.

  Return Value:   Matrix4.

  Comments:       None.
  \***************************************************************************************/
  Matrix4 Quaternion::GetMatrix() const
  {
    Matrix4 mat4;
    /*float x = x;
    float y = y;
    float z = z;
    float w = w;*/
    float s;

    s = 2.0f / ( (float) std::sqrt (x * x + y * y + z * z + w * w) );

    mat4.m[0][0]  = 1.0f - s * (y * y + z * z);
    mat4.m[0][1]  = s * (x * y - w * z);
    mat4.m[0][2]  = s * (x * z + w * y);
    mat4.m[0][3]  = 0.0f;

    mat4.m[1][0]  = s * (x * y + w * z);
    mat4.m[1][1]  = 1.0f - s * (x * x + z * z);
    mat4.m[1][2]  = s * (y * z - w * x);
    mat4.m[1][3]  = 0.0f;

    mat4.m[2][0]  = s * (x * z - w * y);
    mat4.m[2][1]  = s * (y * z + w * x);
    mat4.m[2][2] = 1.0f - s * (x * x + y * y);
    mat4.m[2][3] = 0.0f;

    mat4.m[3][0] = 0.0f;
    mat4.m[3][1] = 0.0f;
    mat4.m[3][2] = 0.0f;
    mat4.m[3][3] = 1.0f;

    return mat4;
  }

  /*const Quaternion operator * (const Quaternion& quat1, const Quaternion& quat2)
  {
      Quaternion qt;
      float x1, x2, y1, y2, z1, z2, w1, w2;

      x1 = quat1.x;
      y1 = quat1.y;
      z1 = quat1.z;
      w1 = quat1.w;

      x2 = quat2.x;
      y2 = quat2.y;
      z2 = quat2.z;
      w2 = quat2.w;

      qt.x = w1*x2 + x1*w2 - z1*y2 + y1*z2;
      qt.y = w1*y2 + y1*w2 + z1*x2 - x1*z2;
      qt.z = w1*z2 + z1*w2 + x1*y2 - y1*x2;
      qt.w = w1*w2 - x1*x2 - y1*y2 - z1*z2;

      return qt;
  }*/

  Quaternion operator * (float f, const Quaternion &quat)
  {
    Quaternion qt;
    qt.x = quat.x * f;
    qt.y = quat.y * f;
    qt.z = quat.z * f;
    qt.w = quat.w * f;
    return qt;
  }

  Quaternion Slerp (const float t, const Quaternion &lhs, const Quaternion &rhs)
  {
    // the slerp of a pair of unit quaterions is the weighted
    // interpolation between them, where the interpolation weight is
    // given by t = [0, 1.0]
    //
    // the trick to slerping is that we find the angle between the two
    // quats by treating them as a pair of four vectors and getting the
    // cosine [as the dot product].
    //
    // then the slerp between two quaternions A and B is:
    //
    //       A * (upper_weight) + B * (lower_weight)
    //
    //	where the weights are the sines of the t-weighted angle
    //	divided by the sine of the angle.
    //
    // the resulting quaternion is also a unit quaternion.


    // find the angle between the two quats by treating
    // them as 4-length vectors -- V1.V2 = cos(theta)


    float cosine_angle, angle_over_two;
    float coef1, coef2;
    Quaternion qt;
    Quaternion lhs_n, rhs_n;

    lhs_n = lhs;
    rhs_n = rhs;
    lhs_n.Normalize();
    rhs_n.Normalize();

    cosine_angle = lhs_n.DotProduct (rhs_n);  // = cos(angle_over_two)

    // adjust signs (if necessary)
    if ( cosine_angle < 0.0 )
    {
      cosine_angle = -cosine_angle;
      rhs_n = - rhs_n;
    }

    angle_over_two = (float) std::acos (cosine_angle);

    if ( (1 - cosine_angle) > 0.000001)
    {
      coef1 = (float) std::sin (angle_over_two * (1 - t) ) / (float) std::sin (angle_over_two);
      coef2 = (float) std::sin (angle_over_two * t)      / (float) std::sin (angle_over_two);
    }
    else
    {
      // lhs and rhs are very close ... so we can do a linear interpolation
      coef1 = 1 - t;
      coef2 = t;
    }

    qt = coef1 * lhs_n + coef2 * rhs_n;
    return qt;
  }

}
