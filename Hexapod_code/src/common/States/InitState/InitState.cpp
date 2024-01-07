#include "common/States/InitState/InitState.h"
#include "common/defines.h"
#include "helpers/helpers.h"
#include "Vector/Vector2.h"
#include "TimeInfo/TimeInfo.h"
#include "RCData/RCData.h"
#include <cmath>

Move InitState::get_move(Leg leg, Move previousMove) {
    bool evenStage = get_stage() % 2 == 0;
    float z = evenStage ? 50 : -height;
    float sixtyDegree = M_PI / 3.f;
    float angle = leg.legNumber * sixtyDegree - sixtyDegree;
    Vector2 offset = polar_to_cartesian(DEFAULT_RADIUS, angle);
    Vector3 endPoint = Vector3(offset.x, offset.y, z);
    Move move;
    move.startTime = TimeInfo::getInstance().CurrentTime();
    move.duration = 1000000;
    move.endTime = move.startTime + move.duration;
    move.startPoint = evenStage ? endPoint : previousMove.currentPoint;
    move.endPoint = endPoint;
    move.currentPoint = previousMove.currentPoint;
    move.legNumber = leg.legNumber;
    return move;
}