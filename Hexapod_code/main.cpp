#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "BLE/BLE.h"
#include "common/Leg.h"
#include "common/defines.h"
#include "helpers/helpers.h"
#include "RCData/RCData.h"
#include "TimeInfo/TimeInfo.h"
#include "IKSolver/IKSolver.h"
#include "pca9685.h"
#include <array>
#include <cmath>

PCA9685 pca9685[2] = {
    PCA9685(
        0x00,
        [](uint8_t addr, uint8_t* buf, size_t len, bool nostop) -> int { return i2c_read_blocking(i2c0, addr, buf, len, nostop); },
        [](uint8_t addr, uint8_t* buf, size_t len, bool nostop) -> int { return i2c_write_blocking(i2c0, addr, buf, len, nostop); },
        [](int ms) -> void { sleep_ms(ms); }
    ),
    PCA9685(
        0x01,
        [](uint8_t addr, uint8_t* buf, size_t len, bool nostop) -> int { return i2c_read_blocking(i2c0, addr, buf, len, nostop); },
        [](uint8_t addr, uint8_t* buf, size_t len, bool nostop) -> int { return i2c_write_blocking(i2c0, addr, buf, len, nostop); },
        [](int ms) -> void { sleep_ms(ms); }
    )
};

std::array<Leg, 6> legs = init_legs();
Vector3 startPosition[6] = {
    Vector3(0, 0, 0),
    Vector3(0, 0, 0),
    Vector3(0, 0, 0),
    Vector3(0, 0, 0),
    Vector3(0, 0, 0),
    Vector3(0, 0, 0)
};
Vector3 endPosition[6] = {
    Vector3(0, 0, 0),
    Vector3(0, 0, 0),
    Vector3(0, 0, 0),
    Vector3(0, 0, 0),
    Vector3(0, 0, 0),
    Vector3(0, 0, 0)
};
float movementDurationUs = 500000;
float startTime[6];
float endTime[6];

const int numSteps = 4;

Vector3 testPositions[6][numSteps] = {
    {
        Vector3(229.43, -377.66, 0),
        Vector3(229.43, -277.66, 0),
        Vector3(229.43, -277.66, -50),
        Vector3(229.43, -377.66, -50)
    },
    {
        Vector3(400, 50, -50),
        Vector3(400, -50, -50),
        Vector3(400, -50, 0),
        Vector3(400, 50, 0)
    },
    {
        Vector3(229.43, 277.66, 0),
        Vector3(229.43, 377.66, 0),
        Vector3(229.43, 377.66, -50),
        Vector3(229.43, 277.66, -50)
    },
    {
        Vector3(-229.43, 277.66, 0),
        Vector3(-229.43, 377.66, 0),
        Vector3(-229.43, 377.66, -50),
        Vector3(-229.43, 277.66, -50)
    },
    {
        Vector3(-400, 50, -50),
        Vector3(-400, -50, -50),
        Vector3(-400, -50, 0),
        Vector3(-400, 50, 0)
    },
    {
        Vector3(-229.43, -377.66, 0),
        Vector3(-229.43, -277.66, 0),
        Vector3(-229.43, -277.66, -50),
        Vector3(-229.43, -377.66, -50)
    }
};

int currentTestPos = 0;

void exceptionHandler(const std::exception &e) {
    printf("%s\n", e.what());
    while(true) {}
}

int setup() {
    stdio_init_all();

    if (cyw43_arch_init())
    {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    le_counter_setup();

    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    pca9685[0].Init();
    pca9685[0].SetServoFrequency();
    pca9685[1].Init();
    pca9685[1].SetServoFrequency();

    TimeInfo::getInstance().update();
    
    return 0;
}

void loop() {
    TimeInfo::getInstance().update();
    uint16_t PWM[18];
    for (int i = 0; i < 6; i++) {
        if (TimeInfo::getInstance().CurrentTime() > endTime[i]) {
            startTime[i] = TimeInfo::getInstance().CurrentTime();
            endTime[i] = startTime[i] + movementDurationUs;
            startPosition[i] = testPositions[i][currentTestPos % numSteps];
            endPosition[i] = testPositions[i][(currentTestPos + 1) % numSteps];
            if (i == 5) currentTestPos++;
        }

        Vector3 ep = lerp(startPosition[i], endPosition[i], (TimeInfo::getInstance().CurrentTime() - startTime[i]) / movementDurationUs);
        try {
            Vector3 angles = IKSolve(ep, legs[i]);
            PWM[i * 3 + 0] = legs[i].coxaServo.PwmFromAngle(angles.x);
            PWM[i * 3 + 1] = legs[i].femurServo.PwmFromAngle(angles.y);
            PWM[i * 3 + 2] = legs[i].tibiaServo.PwmFromAngle(angles.z);
            // PWM[i * 3 + 0] = legs[i].coxaServo.PwmFromAngle(deg2rad(0));
            // PWM[i * 3 + 1] = legs[i].femurServo.PwmFromAngle(deg2rad(90));
            // PWM[i * 3 + 2] = legs[i].tibiaServo.PwmFromAngle(deg2rad(90));
        }
        catch(const std::exception& e) {
            exceptionHandler(e);
        }
        
        // if (i == 0) printf("Point: (%f, %f, %f)\tA: %f\tB: %f\tC: %f\n", ep.x, ep.y, ep.z, angles.x * 180.f / M_PI, angles.y * 180.f / M_PI, angles.z * 180.f / M_PI);
    }
    pca9685[0].SetChannelsPWM(0, 9, PWM);
    pca9685[1].SetChannelsPWM(0, 9, PWM + 9);
}

int main() {
    if (setup() < 0) return -1;
    while (1) {
        loop();
    }
    return 0;
}