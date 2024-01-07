#include "common/States/WalkState/WalkState.h"
#include "RCData/RCData.h"
#include "common/defines.h"
#include "helpers/helpers.h"
#include <cmath>
#include <stdio.h>

Move WalkState::get_direction_move(Leg leg, Move previousMove) {
    Move move;
    move.startPoint = previousMove.currentPoint;
    float sixtyDegree = M_PI / 3.f;
    float angle = leg.legNumber * sixtyDegree - sixtyDegree;
    Vector2 offset = polar_to_cartesian(DEFAULT_RADIUS, angle);
    if (leg.state == LegState::LegStance) {
        offset -= (direction * MAX_SPEED);
        move.duration = MOVE_CYCLE_LENGTH * 60 / 100;
    } else {
        offset += (direction * MAX_SPEED);
        move.duration = MOVE_CYCLE_LENGTH * 40 / 100;
    }
    move.endPoint = Vector3(offset.x, offset.y, -height);
    Vector3 vector = (move.endPoint - move.startPoint) / 3;
    move.ctrlPoint1 = move.startPoint + vector + Vector3(0, 0, 100);
    move.ctrlPoint2 = move.ctrlPoint1 + vector;
    if (get_stage() == 0) {
        if (leg.legNumber % 2 == 0) {
            move.startTime = TimeInfo::getInstance().CurrentTime();
        } else {
            move.startTime = TimeInfo::getInstance().CurrentTime() - MOVE_CYCLE_LENGTH / 2;
        }
    } else {
        move.startTime = TimeInfo::getInstance().CurrentTime();
    }
    move.endTime = move.startTime + move.duration;
    move.legNumber = leg.legNumber;
    return move;
}

Move WalkState::get_rotation_move(Leg leg, Move previousMove) {
    Move move;
    move.startPoint = previousMove.currentPoint;
    float sixtyDegree = M_PI / 3.f;
    float angle = leg.legNumber * sixtyDegree - sixtyDegree;
    Vector2 offset = polar_to_cartesian(DEFAULT_RADIUS, angle);
    angle -= M_PI/ 2.f;
    Vector2 rotationVector = polar_to_cartesian(rotation * MAX_SPEED, angle);
    if (leg.state == LegState::LegStance) {
        offset -= rotationVector;
        move.duration = MOVE_CYCLE_LENGTH * 60 / 100;
    } else {
        offset += rotationVector;
        move.duration = MOVE_CYCLE_LENGTH * 40 / 100;
    }
    move.endPoint = Vector3(offset.x, offset.y, -height);
    Vector3 vector = (move.endPoint - move.startPoint) / 3;
    move.ctrlPoint1 = move.startPoint + vector + Vector3(0, 0, 100);
    move.ctrlPoint2 = move.ctrlPoint1 + vector;
    if (get_stage() == 0) {
        if (leg.legNumber % 2 == 0) {
            move.startTime = TimeInfo::getInstance().CurrentTime();
        } else {
            move.startTime = TimeInfo::getInstance().CurrentTime() - MOVE_CYCLE_LENGTH / 2;
        }
    } else {
        move.startTime = TimeInfo::getInstance().CurrentTime();
    }
    move.endTime = move.startTime + move.duration;
    move.legNumber = leg.legNumber;
    return move;
}

Move WalkState::get_move(Leg leg, Move previousMove) {
    Move move = get_direction_move(leg, previousMove);
    Move rotation = get_rotation_move(leg, previousMove);
    move.endPoint = (move.endPoint + rotation.endPoint) / 2;
    move.ctrlPoint1 = (move.ctrlPoint1 + rotation.ctrlPoint1) / 2;
    move.ctrlPoint2 = (move.ctrlPoint2 + rotation.ctrlPoint2) / 2;
    return move;
}