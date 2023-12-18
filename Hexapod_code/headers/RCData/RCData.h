#ifndef RCDATA_H
#define RCDATA_H

#include "Vector/Vector2.h"

struct RCData {
    Vector2 direction = {0, 0};
    float rotation = 0;
    float height = 0;
};

extern RCData rcdata;

#endif