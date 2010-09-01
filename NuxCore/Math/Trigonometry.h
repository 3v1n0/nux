#ifndef TRIGONOMETRY_H
#define TRIGONOMETRY_H

#include "Vector3.h"

NAMESPACE_BEGIN
// Assume the spherical coordinate system relatively to a right handed xyz, 
// with Z pointing up.
// 0 <= phi < 180
// 0 <= theta < 360 ->>> along X axis, theta = 0.
Vector3 SphericalToCartesianXBaseDeg(float r, float theta, float phi);
Vector3 SphericalToCartesianXBaseRad(float r, float theta, float phi);

// Assume the spherical coordinate system relatively to a right handed xyz, 
// with Y pointing up.
// 0 <= phi < 180
// 0 <= theta < 360 ->>> along Z axis, theta = 0.
Vector3 SphericalToCartesianZBaseDeg(float r, float theta, float phi);
Vector3 SphericalToCartesianZBaseRad(float r, float theta, float phi);

Vector3 CartesianToSphericalXBaseRad(float x, float y, float z);
Vector3 CartesianToSphericalZBaseDeg(float x, float y, float z);


NAMESPACE_END



#endif // TRIGONOMETRY_H