#include "TimeInfo/TimeInfo.h"
#include <hardware/timer.h>

TimeInfo &TimeInfo::getInstance() {
    static TimeInfo instance;
    return instance;
}

void TimeInfo::update() {
    // previousTime = currentTime;
    currentTime = time_us_64();
    // deltaTime = currentTime - previousTime;
}
