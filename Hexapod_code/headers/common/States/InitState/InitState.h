#pragma once
#include "common/States/IState.h"

class InitState : public IState {
private:
        
public:
    Move get_move(Leg leg, Move previousMove);

};