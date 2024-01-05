#include "IKSolver/IKSolver.h"

#include "Vector/Vector2.h"
#include "helpers/helpers.h"
#include <cmath>
#include <stdio.h>
#include <limits>
#include <stdexcept>
#include <sstream>

Vector3 IKSolve(Vector3 endEffector, Leg leg) {
    Vector3 ef = endEffector;
    Vector2 legOffsetCartesian = polar_to_cartesian(leg.offsetPolar);
    ef -= Vector3(legOffsetCartesian.x, legOffsetCartesian.y, 0);

    float gamma = arctan2(ef.y, ef.x);
    float theta1 = gamma - leg.offsetPolar.fi;
    if (theta1 > M_PI) theta1 -= M_TWOPI;
    if (!isInRange(theta1, leg.coxa.minAngle, leg.coxa.maxAngle)) {
        std::ostringstream oss;
        oss << "Theta 1 was out of range with value " << theta1 << " for leg " << leg.legNumber << " and point (" << endEffector.x << ", " << endEffector.y << ", " << endEffector.z << ")";
        throw std::out_of_range(oss.str());
    }

    Vector2 coxaOffset = polar_to_cartesian(leg.coxa.length, gamma);
    ef -= Vector3(coxaOffset.x, coxaOffset.y, 0);

    float P = sqrt(pow(ef.x, 2) + pow(ef.y, 2));
    float K = sqrt(pow(P, 2) + pow(ef.z, 2));

    float temp = (pow(leg.femur.length, 2) + pow(K, 2) - pow(leg.tibia.length, 2)) / (2 * leg.femur.length * K);
    if (temp < -1.f) temp += 2.f;
    if (temp > 1.f) temp -= 2.f;
    gamma = acos(temp);
    float delta = atan(ef.z/P);
    float theta2 = gamma + delta;
    if (!isInRange(theta2, leg.femur.minAngle, leg.femur.maxAngle)) {
        std::ostringstream oss;
        oss << "Theta 2 was out of range with value " << theta2 << " for leg " << leg.legNumber << " and point (" << endEffector.x << ", " << endEffector.y << ", " << endEffector.z << ")";
        throw std::out_of_range(oss.str());
    }

    temp = (pow(leg.tibia.length, 2) + pow(leg.femur.length, 2) - pow(K, 2)) / (2 * leg.tibia.length * leg.femur.length);
    if (temp < -1.f) temp += 2.f;
    if (temp > 1.f) temp -= 2.f;
    gamma = acos(temp);
    float theta3 = M_PI - gamma;
    if (!isInRange(theta3, leg.tibia.minAngle, leg.tibia.maxAngle)) {
        std::ostringstream oss;
        oss << "Theta 3 was out of range with value " << theta3 << " for leg " << leg.legNumber << " and point (" << endEffector.x << ", " << endEffector.y << ", " << endEffector.z << ")";
        throw std::out_of_range(oss.str());
    }

    return {theta1, theta2, theta3};
}