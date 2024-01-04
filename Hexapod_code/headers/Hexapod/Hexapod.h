#pragma once

#include <array>
#include "common/Leg.h"
#include "Vector/Vector3.h"
#include "common/Move/Move.h"
#include "common/States/IState.h"

class Hexapod {
private:
    enum RobotState {
        Reset,
        Init,
        Idle,
        Walk
    };

    RobotState robotState = RobotState::Reset;
    RobotState previousState = RobotState::Reset;
    IState *state;
    std::array<Move, 6> moves;
    std::array<Leg, 6> legs = init_legs();
    float movementDurationUs = 500000;

public:
    Hexapod() {}

    void init_moves();
    void init_moves(Leg leg);
    void step();

};
