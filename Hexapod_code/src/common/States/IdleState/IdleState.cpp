#include "common/States/IdleState/IdleState.h"
#include "common/defines.h"
#include "helpers/helpers.h"
#include "RCData/RCData.h"
#include <cmath>
#include <stdio.h>

Move IdleState::get_move(Leg leg, Move previousMove) {
    float sixtyDegree = M_PI / 3.f;
    float angle = leg.legNumber * sixtyDegree - sixtyDegree;
    Vector2 offset = polar_to_cartesian(DEFAULT_RADIUS, angle);
    Vector3 endPoint = Vector3(offset.x, offset.y, -height);
    Vector3 P1;
    Vector3 P2;
    Move move;
    if (previousMove.finished()) {
        Vector3 vector = (endPoint - previousMove.currentPoint) / 3;
        P1 = previousMove.currentPoint + vector + Vector3(0, 0, 150);
        P2 = previousMove.currentPoint + vector * 2 + Vector3(0, 0, 150);
        move.startTime = TimeInfo::getInstance().CurrentTime();
        move.duration = get_stage() < 2 ? 1000000 : 500;
        move.endTime = move.startTime + move.duration;
        move.startPoint = previousMove.currentPoint;
    } else {
        Vector3 vector = (endPoint - previousMove.startPoint) / 3;
        P1 = previousMove.startPoint + vector + Vector3(0, 0, 150);
        P2 = previousMove.startPoint + vector * 2 + Vector3(0, 0, 150);
        move.startTime = previousMove.startTime;
        move.duration = previousMove.duration;
        move.endTime = previousMove.endTime;
        move.startPoint = previousMove.startPoint;
    }
    move.currentPoint = previousMove.currentPoint;
    move.ctrlPoint1 = P1;
    move.ctrlPoint2 = P2;
    move.endPoint = endPoint;
    move.legNumber = leg.legNumber;
    return move;
}
