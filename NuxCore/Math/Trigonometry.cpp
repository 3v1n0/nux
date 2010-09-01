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


#include "../NKernel.h"
#include "Trigonometry.h"
#include "Constants.h"

NAMESPACE_BEGIN

// Assume the spherical coordinate system relatively to a right handed xyz, 
// with Z pointing up.
// 0 <= phi < 180
// 0 <= theta < 360 ->>> along X axis, theta = 0.
Vector3 SphericalToCartesianXBaseDeg(float r, float theta, float phi)
{
    Vector3 v;
    v.x = r * std::cos(theta*Const::pi/180.0f) * std::sin(phi*Const::pi/180.0f);
    v.y = r * std::sin(theta*Const::pi/180.0f) * std::sin(phi*Const::pi/180.0f);
    v.z = r * std::cos(phi*Const::pi/180.0f);
    return v;
}

Vector3 SphericalToCartesianXBaseRad(float r, float theta, float phi)
{
    Vector3 v;
    v.x = r * std::cos(theta) * std::sin(phi);
    v.y = r * std::sin(theta) * std::sin(phi);
    v.z = r * std::cos(phi);
    return v;
}

// Assume the spherical coordinate system relatively to a right handed xyz, 
// with Y pointing up.
// 0 <= phi < 180
// 0 <= theta < 360 ->>> along Z axis, theta = 0.
Vector3 SphericalToCartesianZBaseDeg(float r, float theta, float phi)
{
    Vector3 v;
    v.z = r * std::cos(theta*Const::pi/180.0f) * std::sin(phi*Const::pi/180.0f);
    v.x = r * std::sin(theta*Const::pi/180.0f) * std::sin(phi*Const::pi/180.0f);
    v.y = r * std::cos(phi*Const::pi/180.0f);
    return v;
}

Vector3 SphericalToCartesianZBaseRad(float r, float theta, float phi)
{
    Vector3 v;
    v.z = r * std::cos(theta) * std::sin(phi);
    v.x = r * std::sin(theta) * std::sin(phi);
    v.y = r * std::cos(phi);
    return v;
}

Vector3 CartesianToSphericalXBaseRad(float x, float y, float z)
{
    float r, theta, phi;
    r = std::sqrt(x*x + y*y + z*z);
    theta = std::atan(y/x);
    phi = std::acos(z/r);
    return Vector3(r, theta, phi);
}

Vector3 CartesianToSphericalZBaseDeg(float x, float y, float z)
{
    float r, theta, phi;
    r = std::sqrt(x*x + y*y + z*z);
    theta = std::atan(x/z);
    phi = std::acos(y/r);
    return Vector3(r, theta, phi);
}

NAMESPACE_END

