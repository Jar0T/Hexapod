#include "Hexapod/Hexapod.h"
#include "TimeInfo/TimeInfo.h"
#include "IKSolver/IKSolver.h"
#include "helpers/helpers.h"
#include "common/States/InitState/InitState.h"
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
    case Reset:
        previousState = robotState;
        robotState = RobotState::Init;
        state = new InitState;
        break;
    case Init:
        // Init moves for Init state after state change
        if (previousState != robotState) {
            for (int i = 0; i < 6; i += 2) {
                Move move;
                move.startTime = currentTime;
                move.endTime = move.startTime + 1000000;
                move.duration = 1000000;
                move.endPoint = state->get_end_point(legs[i]);
                move.startPoint = move.endPoint;
                move.currentPoint = move.endPoint;
                move.legNumber = i;
                moves[i] = move;
            }
        }
        previousState = robotState;
        // Init moves for Init state after finishing previous moves
        if (std::all_of(moves.begin(), moves.end(), [](Move move) { return move.finished(); })) {
            state->next_stage();
            if (state->get_stage() < 4) {
                for (int i = 0; i < 6; i++) {
                    Move move;
                    move.startTime = currentTime;
                    move.endTime = move.startTime + 1000000;
                    move.duration = 1000000;
                    move.endPoint = state->get_end_point(legs[i]);
                    move.startPoint = state->get_stage() == 2 ? move.endPoint : moves[i].startPoint;
                    move.currentPoint = move.startPoint;
                    move.legNumber = i;
                    moves[i] = move;
                }
            } else {
                robotState = RobotState::Idle;
                break;
            }
        }
        // TODO: write PWM to legs depending on state's stage
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
    default:
        break;
    }
}