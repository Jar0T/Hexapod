#ifndef LEG_H
#define LEG_H

#include "Vector/Vector2.h"
#include "pca9685.h"
#include <array>

enum LegState {
    LegInit,
    LegIdle,
    LegStance,
    LegSwing
};

struct Joint {
    float length;
    float minAngle;
    float maxAngle;
};

struct Leg {
    int legNumber;
    Vector2 offsetPolar;
    Joint coxa;
    Servo coxaServo;
    Joint femur;
    Servo femurServo;
    Joint tibia;
    Servo tibiaServo;
    LegState state = LegState::LegInit;
};

std::array<Leg, 6> init_legs();

#endif