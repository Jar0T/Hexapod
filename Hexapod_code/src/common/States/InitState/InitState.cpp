#include "common/States/InitState/InitState.h"
#include "common/defines.h"
#include "helpers/helpers.h"
#include "Vector/Vector2.h"
#include "TimeInfo/TimeInfo.h"
#include <cmath>

Vector3 InitState::get_end_point(Leg leg) {
    float z = get_stage() % 2 == 0 ? 50 : -DEFAULT_HEIGHT;
    float sixtyDegree = M_PI / 3.f;
    float angle = leg.legNumber * sixtyDegree - sixtyDegree;
    Vector2 offset = polar_to_cartesian(DEFAULT_RADIUS, angle);
    return Vector3(offset.x, offset.y, z);
}