#include "RCData/RCData.h"
#include "common/defines.h"
#include "helpers/helpers.h"
#include <math.h>

Vector2 targetDirection = { 0, 0 };
Vector2 direction = { 0, 0 };
float targetRotation = 0;
float rotation = 0;
float targetHeight = DEFAULT_HEIGHT;
float height = DEFAULT_HEIGHT;
uint64_t lastUpdateTime = 0;
bool reset = false;

void smooth_controls() {
    direction = lerp(direction, targetDirection, 0.1);
    rotation = lerp(rotation, targetRotation, 0.1);
    height = lerp(height, targetHeight, 0.1);
}