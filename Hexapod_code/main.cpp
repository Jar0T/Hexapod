#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "BLE/BLE.h"
#include "helpers/helpers.h"
#include "TimeInfo/TimeInfo.h"
#include "Hexapod/Hexapod.h"
#include "common/ServoDrivers.h"
#include "RCData/RCData.h"

Hexapod hexapod;

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

    init_drivers();

    TimeInfo::getInstance().update();
    
    printf("Init done\n");
    return 0;
}

void loop() {
    TimeInfo::getInstance().update();
    smooth_controls();
    hexapod.step();
}

int main() {
    if (setup() < 0) return -1;
    while (1) {
        loop();
    }
    return 0;
}