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
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3 Vector3::operator*(float other) {
    return Vector3(x * other, y * other, z * other);
}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}