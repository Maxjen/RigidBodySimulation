#pragma once

#include <math.h>

class Vec3
{
public:
        float x, y, z;
        
        // Constructors
        Vec3();
        Vec3(const Vec3 &a);
        Vec3(float nx, float ny, float nz);
        
        Vec3 &operator =(const Vec3 &a);
        bool operator ==(const Vec3 &a) const;
        bool operator !=(const Vec3 &a) const;
        void zero();
        Vec3 operator -() const;
        Vec3 operator +(const Vec3 &a) const;
        Vec3 operator -(const Vec3 &a) const;
        Vec3 operator *(float a) const;
        Vec3 operator /(float a) const;
        Vec3 &operator +=(const Vec3 &a);
        Vec3 &operator -=(const Vec3 &a);
        Vec3 &operator *=(float a);
        Vec3 &operator /=(float a);
        void normalize();
        float operator *(const Vec3 &a) const;
};

//Nonmember functions
/*float vectorMag(const Vec3 &a);
float vectorMagP2(const Vec3 &a);
Vec3 operator *(float k, const Vec3 &v);
float distance(const Vec3 &a, const Vec3 &b);*/

inline float vectorMag(const Vec3 &a) {
    return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}
inline Vec3 operator *(float k, const Vec3 &v) {
    return Vec3(k*v.x, k*v.y, k*v.z);
}
inline float distance(const Vec3 &a, const Vec3 &b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}
inline Vec3 cross(const Vec3 &a, const Vec3 &b) {
        return Vec3(
                a.y*b.z - a.z*b.y,
                a.z*b.x - a.x*b.z,
                a.x*b.y - a.y*b.x
        );
}
