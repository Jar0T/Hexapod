#include "helpers/helpers.h"
#include "math.h"
#include <stdio.h>

Vector2 polar_to_cartesian(Vector2 polarCoordinates) {
    return {
        .x = polarCoordinates.r * cos(polarCoordinates.fi),
        .y = polarCoordinates.r * sin(polarCoordinates.fi)
    };
}

Vector2 polar_to_cartesian(float r, float fi) {
    return { .x =r * cos(fi), .y = r * sin(fi) };
}

float deg2rad(float angle) {
    return angle * M_PI / 180.0;
}

float rad2deg(float angle) {
    return angle * 180.0 / M_PI;

}

bool isInRange(float value, float min, float max) {
    return value >= min && value <= max;
}

Vector2 lerp(Vector2 start, Vector2 end, float t) {
    return {
        start.x + t * (end.x - start.x),
        start.y + t * (end.y - start.y)
    };
}

Vector3 lerp(Vector3 start, Vector3 end, float t) {
    return {
        start.x + t * (end.x - start.x),
        start.y + t * (end.y - start.y),
        start.z + t * (end.z - start.z)
    };
}

Vector3 bezier(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t) {
    float u = 1.0f - t;
    float uu = u * u;
    float uuu = uu * u;
    float tt = t * t;
    float ttt = tt * t;

    Vector3 p = p0 * uuu;
    p += p1 * t * uu * 3;
    p += p2 * tt * u * 3;
    p += p3 * ttt;

    return p;
}

float arctan2(float y, float x) {
    if (x >= 0.f && y >= 0.f) return atan(y / x);
    else if (x < 0.f) return atan(y / x) + M_PI;
    else if (x >= 0.f && y < 0.f) return atan(y / x) + M_TWOPI;
    return NAN;
}
