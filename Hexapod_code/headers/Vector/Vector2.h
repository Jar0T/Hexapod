#pragma once

struct Vector2 {
    union {
        float x;
        float r;
    };
    union {
        float y;
        float fi;
    };

    Vector2& operator-=(const Vector2& other);
    Vector2& operator+=(const Vector2& other);
    Vector2 operator*(float multiplier);
};
