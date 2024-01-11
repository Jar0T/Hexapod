#pragma once
#include "Vector/Vector3.h"
#include "TimeInfo/TimeInfo.h"
#include <cstdint>

struct Move {
    uint8_t legNumber = 0;
    uint64_t startTime = 0;
    uint64_t endTime = 0;
    uint64_t duration = 0;
    Vector3 startPoint = Vector3(0, 0, 0);
    Vector3 endPoint = Vector3(0, 0, 0);
    Vector3 currentPoint = Vector3(0, 0, 0);
    Vector3 ctrlPoint1 = Vector3(0, 0, 0);
    Vector3 ctrlPoint2 = Vector3(0, 0, 0);
    bool finished() { return TimeInfo::getInstance().CurrentTime() >= endTime; }
    void finish() { endTime = startTime + 1; duration = 1; }
};
