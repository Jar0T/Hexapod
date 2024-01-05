#include "Hexapod/Hexapod.h"
#include "TimeInfo/TimeInfo.h"
#include "IKSolver/IKSolver.h"
#include "helpers/helpers.h"
#include "common/States/InitState/InitState.h"
#include "common/States/IdleState/IdleState.h"
#include "common/ServoDrivers.h"
#include "common/Exception/ExceptionHandler.h"
#include <stdio.h>
#include <stdexcept>

void Hexapod::init_moves() {

}

void Hexapod::init_moves(Leg leg) {

}

void Hexapod::step() {
    uint16_t PWM[18];
    uint64_t currentTime = TimeInfo::getInstance().CurrentTime();
    switch (robotState) {
    case RobotState::Reset:
        previousState = robotState;
        robotState = RobotState::Init;
        state = new InitState;
        break;
    case RobotState::Init:
        // Init moves for Init state after state change
        if (previousState != robotState) {
            for (int i = 0; i < 6; i += 2) {
                moves[i] = state->get_move(legs[i], Move());
            }
        }
        previousState = robotState;
        // Init moves for Init state after finishing previous moves
        if (std::all_of(moves.begin(), moves.end(), [](Move move) { return move.finished(); })) {
            state->next_stage();
            if (state->get_stage() < 4) {
                for (int i = 0; i < 6; i++) {
                    moves[i] = state->get_move(legs[i], moves[i]);
                }
            } else {
                robotState = RobotState::Idle;
                state = new IdleState();
                break;
            }
        }
        // Write PWM to legs depending on state's stage
        if (state->get_stage() < 2) {
            for (int i = 0; i < 6; i+=2) {
                float t = (float)(currentTime - moves[i].startTime) / (float)moves[i].duration;
                Vector3 endPoint = lerp(
                    moves[i].startPoint,
                    moves[i].endPoint,
                    t);
                try {
                    Vector3 angles = IKSolve(endPoint, legs[i]);
                    PWM[i * 3 + 0] = legs[i].coxaServo.PwmFromAngle(angles.x);
                    PWM[i * 3 + 1] = legs[i].femurServo.PwmFromAngle(angles.y);
                    PWM[i * 3 + 2] = legs[i].tibiaServo.PwmFromAngle(angles.z);
                    moves[i].currentPoint = endPoint;
                } catch(const std::out_of_range& e) {
                    handle_exceptions(e);
                }
            }
            pca1.SetChannelsPWM(0, 3, PWM);
            pca1.SetChannelsPWM(6, 3, PWM + 6);
            pca2.SetChannelsPWM(3, 3, PWM + 12);
        } else {
            for (int i = 1; i < 6; i+=2) {
                float t = (float)(currentTime - moves[i].startTime) / (float)moves[i].duration;
                Vector3 endPoint = lerp(
                    moves[i].startPoint,
                    moves[i].endPoint,
                    t);
                try {
                    Vector3 angles = IKSolve(endPoint, legs[i]);
                    PWM[i * 3 + 0] = legs[i].coxaServo.PwmFromAngle(angles.x);
                    PWM[i * 3 + 1] = legs[i].femurServo.PwmFromAngle(angles.y);
                    PWM[i * 3 + 2] = legs[i].tibiaServo.PwmFromAngle(angles.z);
                    moves[i].currentPoint = endPoint;
                } catch(const std::out_of_range& e) {
                    handle_exceptions(e);
                }
            }
            pca1.SetChannelsPWM(3, 3, PWM + 3);
            pca2.SetChannelsPWM(0, 3, PWM + 9);
            pca2.SetChannelsPWM(6, 3, PWM + 15);
        }
        break;
    case RobotState::Idle:
        if (previousState != RobotState::Idle) {
            for (int i = 0; i < 6; i++) {
                moves[i] = state->get_move(legs[i], moves[i]);
            }
        }
        previousState = robotState;
        if (std::all_of(moves.begin(), moves.end(), [](Move move) { return move.finished(); })) {
            if (state->get_stage() < 2) {
                state->next_stage();
                for (int i = 0; i < 6; i++) {
                    moves[i] = state->get_move(legs[i], moves[i]);
                }
            }
        }
        if (state->get_stage() == 0) {
            for (int i = 0; i < 6; i+=2) {
                float t = (float)(currentTime - moves[i].startTime) / (float)moves[i].duration;
                Vector3 endPoint = bezier(
                    moves[i].startPoint,
                    moves[i].ctrlPoint1,
                    moves[i].ctrlPoint2,
                    moves[i].endPoint,
                    t);
                try {
                    Vector3 angles = IKSolve(endPoint, legs[i]);
                    PWM[i * 3 + 0] = legs[i].coxaServo.PwmFromAngle(angles.x);
                    PWM[i * 3 + 1] = legs[i].femurServo.PwmFromAngle(angles.y);
                    PWM[i * 3 + 2] = legs[i].tibiaServo.PwmFromAngle(angles.z);
                    moves[i].currentPoint = endPoint;
                } catch(const std::out_of_range& e) {
                    handle_exceptions(e);
                }
            }
            pca1.SetChannelsPWM(0, 3, PWM);
            pca1.SetChannelsPWM(6, 3, PWM + 6);
            pca2.SetChannelsPWM(3, 3, PWM + 12);
        } else if (state->get_stage() == 1) {
            for (int i = 1; i < 6; i+=2) {
                float t = (float)(currentTime - moves[i].startTime) / (float)moves[i].duration;
                Vector3 endPoint = bezier(
                    moves[i].startPoint,
                    moves[i].ctrlPoint1,
                    moves[i].ctrlPoint2,
                    moves[i].endPoint,
                    t);
                try {
                    Vector3 angles = IKSolve(endPoint, legs[i]);
                    PWM[i * 3 + 0] = legs[i].coxaServo.PwmFromAngle(angles.x);
                    PWM[i * 3 + 1] = legs[i].femurServo.PwmFromAngle(angles.y);
                    PWM[i * 3 + 2] = legs[i].tibiaServo.PwmFromAngle(angles.z);
                    moves[i].currentPoint = endPoint;
                } catch(const std::out_of_range& e) {
                    handle_exceptions(e);
                }
            }
            pca1.SetChannelsPWM(3, 3, PWM + 3);
            pca2.SetChannelsPWM(0, 3, PWM + 9);
            pca2.SetChannelsPWM(6, 3, PWM + 15);
        } else {
            for (int i = 0; i < 6; i++) {
                try {
                    Vector3 angles = IKSolve(moves[i].endPoint, legs[i]);
                    PWM[i * 3 + 0] = legs[i].coxaServo.PwmFromAngle(angles.x);
                    PWM[i * 3 + 1] = legs[i].femurServo.PwmFromAngle(angles.y);
                    PWM[i * 3 + 2] = legs[i].tibiaServo.PwmFromAngle(angles.z);
                    moves[i].currentPoint = moves[i].endPoint;
                } catch(const std::out_of_range& e) {
                    handle_exceptions(e);
                }
            }
            pca1.SetChannelsPWM(0, 9, PWM);
            pca2.SetChannelsPWM(0, 9, PWM + 9);
        }
        break;
    default:
        break;
    }
}