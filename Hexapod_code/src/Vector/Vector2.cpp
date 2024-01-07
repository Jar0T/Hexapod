#include "Vector/Vector2.h"

Vector2 &Vector2::operator-=(const Vector2 &other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2 &Vector2::operator+=(const Vector2 &other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2 Vector2::operator*(float multiplier) {
    Vector2 vec;
    vec.x = x * multiplier;
    vec.y = y * multiplier;
    return vec;
}