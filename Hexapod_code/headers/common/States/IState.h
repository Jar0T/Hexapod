#pragma once
#include <cinttypes>
#include "Vector/Vector3.h"
#include "common/Leg.h"

class IState {
private:
    uint8_t stage = 0;

public:
    uint8_t get_stage() { return stage; }
    void next_stage() { stage++; }
    virtual Vector3 get_end_point(Leg leg) = 0;

};