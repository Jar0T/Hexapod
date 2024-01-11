#pragma once

#include "Vector/Vector2.h"
#include <cinttypes>

extern Vector2 targetDirection;
extern Vector2 direction;
extern float targetRotation;
extern float rotation;
extern float targetHeight;
extern float height;
extern uint64_t lastUpdateTime;
extern bool reset;

void smooth_controls();
