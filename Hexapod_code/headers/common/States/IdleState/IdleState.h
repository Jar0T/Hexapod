#pragma once

#include "common/States/IState.h"

class IdleState : public IState {
private:
    
public:
    Move get_move(Leg leg, Move previousMove);

};
