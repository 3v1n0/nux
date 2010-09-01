#ifndef QUATERNION_H
#define QUATERNION_H

#include "Vector3.h"
#include "Vector4.h"

NAMESPACE_BEGIN

/***************************************************************************************\
Class:       Quaternion

Description:    Define basic quaternion initialization and function.

Comments:       Member functions uses input angles in radian. 
\***************************************************************************************/
class Quaternion  
{
public:
    Quaternion();
    Quaternion(const Quaternion& s);
    Quaternion(const Vector3& vec, float angle);
    Quaternion(const Vector4& vec);

    // creates a Quaternion from an angle axis -- note that if angle > 2*PI the resulting 
    // rotation is angle mod 2*PI
    Quaternion(t_float axis_x, t_float axis_y, t_float axis_z, t_float angle_radian);
    // creates a Quaternion from an angle-around-XYZ euler triple using roll-pitch-yaw order
    Quaternion(t_float euler_x, t_float euler_y, t_float euler_z);
    ~Quaternion();

    Quaternion& operator = (const Quaternion& quat);

    // binary operators
    Quaternion operator + (const Quaternion& quat) const;
    Quaternion operator - (const Quaternion& quat) const;
    Quaternion operator * (const Quaternion& quat) const;
    Quaternion operator * (const t_float& f) const;
    Quaternion operator / (const t_float& f) const;

    // assignment operators
    Quaternion& operator += (const Quaternion& quat);
    Quaternion& operator -= (const Quaternion& quat);
    Quaternion& operator *= (const Quaternion& quat);
    Quaternion& operator *= (const t_float& f);
    Quaternion& operator /= (const t_float& f);

    // unary operators
    Quaternion operator + () const;
    Quaternion operator - () const;
    //const Quaternion operator / (const Quaternion& quat);
    

    bool operator == ( const Quaternion& ) const;
    bool operator != ( const Quaternion& ) const;

    void Conjugate();
    void Inverse();
    void Normalize();
    t_float DotProduct(const Quaternion& quat) const;
    t_float Length() const;

    void GetAngleAxis(Vector3& axis, t_float& angle_radian) const;         // fetches the angle/axis given by the quat

    // Fetches 4x4 homogeneous matrix given by the quat
    Matrix4 GetMatrix() const;    
    
    t_float x, y, z, w;

    friend Quaternion operator * (t_float f, const Quaternion& quat);
private:
    // set the quaternion by angle-axis (see AA constructor)
    void FromAngleAxis(t_float axis_x, t_float axis_y, t_float axis_z, t_float angle_radian);
    
    // set the quaternion by euler axis angles (see euler constructor)
    void FromEulerZXY(t_float euler_x, t_float euler_y, t_float euler_z);
};

NAMESPACE_END

#endif // QUATERNION_H
