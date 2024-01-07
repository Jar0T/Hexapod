#pragma once

#include "common/States/IState.h"

class WalkState : public IState {
private:
    Move get_direction_move(Leg leg, Move previousMove);
    Move get_rotation_move(Leg leg, Move previousMove);

public:
    Move get_move(Leg leg, Move previousMove);

};
