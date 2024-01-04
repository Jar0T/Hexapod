#include "common/ServoDrivers.h"
#include "hardware/i2c.h"

PCA9685 pca1(
    0x00,
    [](uint8_t addr, uint8_t* buf, size_t len, bool nostop) -> int { return i2c_read_blocking(i2c0, addr, buf, len, nostop); },
    [](uint8_t addr, uint8_t* buf, size_t len, bool nostop) -> int { return i2c_write_blocking(i2c0, addr, buf, len, nostop); },
    [](int ms) -> void { sleep_ms(ms); }
);

PCA9685 pca2(
    0x01,
    [](uint8_t addr, uint8_t* buf, size_t len, bool nostop) -> int { return i2c_read_blocking(i2c0, addr, buf, len, nostop); },
    [](uint8_t addr, uint8_t* buf, size_t len, bool nostop) -> int { return i2c_write_blocking(i2c0, addr, buf, len, nostop); },
    [](int ms) -> void { sleep_ms(ms); }
);

void init_drivers() {
    pca1.Init();
    pca1.SetServoFrequency();
    pca2.Init();
    pca2.SetServoFrequency();
}