#ifndef HELPERS_H
#define HELPERS_H

#include "Vector/Vector2.h"
#include "Vector/Vector3.h"

Vector2 polar_to_cartesian(Vector2 polarCoordinates);
Vector2 polar_to_cartesian(float r, float fi);
float deg2rad(float angle);
float rad2deg(float angle);
bool isInRange(float value, float min, float max);
Vector3 lerp(Vector3 start, Vector3 end, float t);
Vector3 bezier(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t);
// Custom atan2 function that returns value in range 0 to 2PI rad.
float arctan2(float y, float x);


#endif // HELPERS_H