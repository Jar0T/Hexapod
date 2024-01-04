#pragma once
#include "Vector/Vector3.h"
#include "TimeInfo/TimeInfo.h"
#include <cstdint>

struct Move {
    uint8_t legNumber;
    uint64_t startTime;
    uint64_t endTime;
    uint64_t duration;
    Vector3 startPoint;
    Vector3 endPoint;
    Vector3 currentPoint;
    Vector3 ctrlPoint1;
    Vector3 ctrlPoint2;
    bool finished() { return TimeInfo::getInstance().CurrentTime() >= endTime; }
};
