#include "common/Leg.h"
#include "common/defines.h"
#include "helpers/helpers.h"

std::array<Leg, 6> init_legs() {
    return std::array<Leg, 6> {{
        {
            .legNumber = 1,
            .offsetPolar = { .r = LEG1_OFFSET, .fi = deg2rad(LEG1_ANGLE) },
            .coxa = { .length = COXA1_LENGTH, .minAngle = deg2rad(COXA1_MIN_ANGLE), .maxAngle = deg2rad(COXA1_MAX_ANGLE) },
            .coxaServo = Servo(deg2rad(COXA1_SERVO_MIN_ANGLE), deg2rad(COXA1_SERVO_MAX_ANGLE), COXA1_SERVO_MIN_PWM, COXA1_SERVO_MAX_PWM),
            .femur = { .length = FEMUR1_LENGTH, .minAngle = deg2rad(FEMUR1_MIN_ANGLE), .maxAngle = deg2rad(FEMUR1_MAX_ANGLE) },
            .femurServo = Servo(deg2rad(FEMUR1_SERVO_MIN_ANGLE), deg2rad(FEMUR1_SERVO_MAX_ANGLE), FEMUR1_SERVO_MIN_PWM, FEMUR1_SERVO_MAX_PWM),
            .tibia = { .length = TIBIA1_LENGTH, .minAngle = deg2rad(TIBIA1_MIN_ANGLE), .maxAngle = deg2rad(TIBIA1_MAX_ANGLE) },
            .tibiaServo = Servo(deg2rad(TIBIA1_SERVO_MIN_ANGLE), deg2rad(TIBIA1_SERVO_MAX_ANGLE), TIBIA1_SERVO_MIN_PWM, TIBIA1_SERVO_MAX_PWM),
        },
        {
            .legNumber = 2,
            .offsetPolar = { .r = LEG2_OFFSET, .fi = deg2rad(LEG2_ANGLE) },
            .coxa = { .length = COXA2_LENGTH, .minAngle = deg2rad(COXA2_MIN_ANGLE), .maxAngle = deg2rad(COXA2_MAX_ANGLE) },
            .coxaServo = Servo(deg2rad(COXA2_SERVO_MIN_ANGLE), deg2rad(COXA2_SERVO_MAX_ANGLE), COXA2_SERVO_MIN_PWM, COXA2_SERVO_MAX_PWM),
            .femur = { .length = FEMUR2_LENGTH, .minAngle = deg2rad(FEMUR2_MIN_ANGLE), .maxAngle = deg2rad(FEMUR2_MAX_ANGLE) },
            .femurServo = Servo(deg2rad(FEMUR2_SERVO_MIN_ANGLE), deg2rad(FEMUR2_SERVO_MAX_ANGLE), FEMUR2_SERVO_MIN_PWM, FEMUR2_SERVO_MAX_PWM),
            .tibia = { .length = TIBIA2_LENGTH, .minAngle = deg2rad(TIBIA2_MIN_ANGLE), .maxAngle = deg2rad(TIBIA2_MAX_ANGLE) },
            .tibiaServo = Servo(deg2rad(TIBIA2_SERVO_MIN_ANGLE), deg2rad(TIBIA2_SERVO_MAX_ANGLE), TIBIA2_SERVO_MIN_PWM, TIBIA2_SERVO_MAX_PWM),
        },
        {
            .legNumber = 3,
            .offsetPolar = { .r = LEG3_OFFSET, .fi = deg2rad(LEG3_ANGLE) },
            .coxa = { .length = COXA3_LENGTH, .minAngle = deg2rad(COXA3_MIN_ANGLE), .maxAngle = deg2rad(COXA3_MAX_ANGLE) },
            .coxaServo = Servo(deg2rad(COXA3_SERVO_MIN_ANGLE), deg2rad(COXA3_SERVO_MAX_ANGLE), COXA3_SERVO_MIN_PWM, COXA3_SERVO_MAX_PWM),
            .femur = { .length = FEMUR3_LENGTH, .minAngle = deg2rad(FEMUR3_MIN_ANGLE), .maxAngle = deg2rad(FEMUR3_MAX_ANGLE) },
            .femurServo = Servo(deg2rad(FEMUR3_SERVO_MIN_ANGLE), deg2rad(FEMUR3_SERVO_MAX_ANGLE), FEMUR3_SERVO_MIN_PWM, FEMUR3_SERVO_MAX_PWM),
            .tibia = { .length = TIBIA3_LENGTH, .minAngle = deg2rad(TIBIA3_MIN_ANGLE), .maxAngle = deg2rad(TIBIA3_MAX_ANGLE) },
            .tibiaServo = Servo(deg2rad(TIBIA3_SERVO_MIN_ANGLE), deg2rad(TIBIA3_SERVO_MAX_ANGLE), TIBIA3_SERVO_MIN_PWM, TIBIA3_SERVO_MAX_PWM),
        },
        {
            .legNumber = 4,
            .offsetPolar = { .r = LEG4_OFFSET, .fi = deg2rad(LEG4_ANGLE) },
            .coxa = { .length = COXA4_LENGTH, .minAngle = deg2rad(COXA4_MIN_ANGLE), .maxAngle = deg2rad(COXA4_MAX_ANGLE) },
            .coxaServo = Servo(deg2rad(COXA4_SERVO_MIN_ANGLE), deg2rad(COXA4_SERVO_MAX_ANGLE), COXA4_SERVO_MIN_PWM, COXA4_SERVO_MAX_PWM),
            .femur = { .length = FEMUR4_LENGTH, .minAngle = deg2rad(FEMUR4_MIN_ANGLE), .maxAngle = deg2rad(FEMUR4_MAX_ANGLE) },
            .femurServo = Servo(deg2rad(FEMUR4_SERVO_MIN_ANGLE), deg2rad(FEMUR4_SERVO_MAX_ANGLE), FEMUR4_SERVO_MIN_PWM, FEMUR4_SERVO_MAX_PWM),
            .tibia = { .length = TIBIA4_LENGTH, .minAngle = deg2rad(TIBIA4_MIN_ANGLE), .maxAngle = deg2rad(TIBIA4_MAX_ANGLE) },
            .tibiaServo = Servo(deg2rad(TIBIA4_SERVO_MIN_ANGLE), deg2rad(TIBIA4_SERVO_MAX_ANGLE), TIBIA4_SERVO_MIN_PWM, TIBIA4_SERVO_MAX_PWM),
        },
        {
            .legNumber = 5,
            .offsetPolar = { .r = LEG5_OFFSET, .fi = deg2rad(LEG5_ANGLE) },
            .coxa = { .length = COXA5_LENGTH, .minAngle = deg2rad(COXA5_MIN_ANGLE), .maxAngle = deg2rad(COXA5_MAX_ANGLE) },
            .coxaServo = Servo(deg2rad(COXA5_SERVO_MIN_ANGLE), deg2rad(COXA5_SERVO_MAX_ANGLE), COXA5_SERVO_MIN_PWM, COXA5_SERVO_MAX_PWM),
            .femur = { .length = FEMUR5_LENGTH, .minAngle = deg2rad(FEMUR5_MIN_ANGLE), .maxAngle = deg2rad(FEMUR5_MAX_ANGLE) },
            .femurServo = Servo(deg2rad(FEMUR5_SERVO_MIN_ANGLE), deg2rad(FEMUR5_SERVO_MAX_ANGLE), FEMUR5_SERVO_MIN_PWM, FEMUR5_SERVO_MAX_PWM),
            .tibia = { .length = TIBIA5_LENGTH, .minAngle = deg2rad(TIBIA5_MIN_ANGLE), .maxAngle = deg2rad(TIBIA5_MAX_ANGLE) },
            .tibiaServo = Servo(deg2rad(TIBIA5_SERVO_MIN_ANGLE), deg2rad(TIBIA5_SERVO_MAX_ANGLE), TIBIA5_SERVO_MIN_PWM, TIBIA5_SERVO_MAX_PWM),
        },
        {
            .legNumber = 6,
            .offsetPolar = { .r = LEG6_OFFSET, .fi = deg2rad(LEG6_ANGLE) },
            .coxa = { .length = COXA6_LENGTH, .minAngle = deg2rad(COXA6_MIN_ANGLE), .maxAngle = deg2rad(COXA6_MAX_ANGLE) },
            .coxaServo = Servo(deg2rad(COXA6_SERVO_MIN_ANGLE), deg2rad(COXA6_SERVO_MAX_ANGLE), COXA6_SERVO_MIN_PWM, COXA6_SERVO_MAX_PWM),
            .femur = { .length = FEMUR6_LENGTH, .minAngle = deg2rad(FEMUR6_MIN_ANGLE), .maxAngle = deg2rad(FEMUR6_MAX_ANGLE) },
            .femurServo = Servo(deg2rad(FEMUR6_SERVO_MIN_ANGLE), deg2rad(FEMUR6_SERVO_MAX_ANGLE), FEMUR6_SERVO_MIN_PWM, FEMUR6_SERVO_MAX_PWM),
            .tibia = { .length = TIBIA6_LENGTH, .minAngle = deg2rad(TIBIA6_MIN_ANGLE), .maxAngle = deg2rad(TIBIA6_MAX_ANGLE) },
            .tibiaServo = Servo(deg2rad(TIBIA6_SERVO_MIN_ANGLE), deg2rad(TIBIA6_SERVO_MAX_ANGLE), TIBIA6_SERVO_MIN_PWM, TIBIA6_SERVO_MAX_PWM),
        },
    }};
}