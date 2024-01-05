#pragma once
#include <cinttypes>
#include "common/Leg.h"
#include "common/Move/Move.h"

class IState {
private:
    uint8_t stage = 0;

public:
    uint8_t get_stage() { return stage; }
    void next_stage() { stage++; }
    virtual Move get_move(Leg leg, Move previousMove) = 0;

};