#pragma once

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
    Vector3 operator*(float multiplier);
    Vector3 operator/(float divisor);
};
