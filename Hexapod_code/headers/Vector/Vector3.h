#ifndef VECTOR3_H
#define VECTOR3_H

struct Vector3 {
    float x;
    float y;
    float z;

    Vector3();
    Vector3(float x, float y, float z);

    Vector3 operator-(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3 operator+(const Vector3& other);
    Vector3& operator+=(const Vector3& other);
    Vector3 operator*(float other);
};

#endif // VECTOR3_H