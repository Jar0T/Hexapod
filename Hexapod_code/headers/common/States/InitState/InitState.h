#pragma once
#include "common/States/IState.h"

class InitState : public IState {
private:
        
public:
    Vector3 get_end_point(Leg leg);

};