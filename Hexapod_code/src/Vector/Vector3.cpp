#include "Vector/Vector3.h"

Vector3 Vector3::operator-(Vector3 const &other) {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 &Vector3::operator-=(const Vector3 &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3 Vector3::operator+(const Vector3 &other) {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 &Vector3::operator+=(const Vector3 &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3 Vector3::operator*(float multiplier) {
    return Vector3(x * multiplier, y * multiplier, z * multiplier);
}

Vector3 Vector3::operator/(float divisor) {
    return Vector3(x / divisor, y / divisor, z / divisor);
}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3::Vector3() : x(0), y(0), z(0) {}