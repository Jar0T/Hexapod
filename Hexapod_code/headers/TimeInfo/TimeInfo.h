#ifndef TIMEINFO_H
#define TIMEINFO_H
#include <cstdint>

class TimeInfo {
public:
    static TimeInfo& getInstance();
    void update();

    uint64_t CurrentTime() { return currentTime; }
    // uint64_t PreviousTime() { return previousTime; }
    // uint64_t DeltaTime() { return deltaTime; }

private:
    uint64_t currentTime = 0;
    // uint64_t previousTime = 0;
    // uint64_t deltaTime = 0;

    TimeInfo() = default;
    TimeInfo(const TimeInfo&) = delete;
    TimeInfo& operator=(const TimeInfo&) = delete;
};

#endif