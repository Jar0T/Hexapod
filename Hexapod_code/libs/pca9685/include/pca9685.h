#ifndef PCA9685_H
#define PCA9685_H

#include <cstdint>
#include <functional>

//device address
#define ADDRESS_BASE    (uint8_t)0x40
#define ADDRESS_MASK    (uint8_t)0x3F

//device registers
#define REG_MODE1       (uint8_t)0x00
#define REG_MODE2       (uint8_t)0x01
#define REG_SUBADR1     (uint8_t)0x02
#define REG_SUBADR2     (uint8_t)0x03
#define REG_SUBADR3     (uint8_t)0x04
#define REG_ALLCALLADR  (uint8_t)0x05
#define REG_LED0        (uint8_t)0x06
#define REG_LED_ALL     (uint8_t)0xFA
#define REG_PRESCALE    (uint8_t)0xFE

//MODE1 values
#define MODE1_RESTART   (uint8_t)0x80   //State of restart logic
#define MODE1_EXTCLK    (uint8_t)0x40   //Enable external oscillator
#define MODE1_AI        (uint8_t)0x20   //Enable auto-increment
#define MODE1_SLEEP     (uint8_t)0x10   //Turn off internal oscillator
#define MODE1_SUB1      (uint8_t)0x08   //Enable Subaddress 1
#define MODE1_SUB2      (uint8_t)0x04   //Enable Subaddress 2
#define MODE1_SUB3      (uint8_t)0x02   //Enable Subaddress 3
#define MODE1_ALLCALL   (uint8_t)0x01   //Enable AllCall

//MODE2 values
#define MODE2_INVRT     (uint8_t)0x10   //Output inverted (when OE = 0)
#define MODE2_OCH       (uint8_t)0x08   //Output change on ACK
#define MODE2_OUTDRV    (uint8_t)0x04   //Totem pole output configuration
#define MODE2_OUTNE_Z   (uint8_t)0x02   //High impedance when OE = 1
#define MODE2_OUTNE_N   (uint8_t)0x01   //When OE = 1 LEDn = 1 if totem pole, LEDn = Z if open-drain

#define SW_RESET        (uint8_t)0x06
#define CH_FULL_ON_OFF  (uint16_t)0x1000

#define MAX_CHANNELS    16

typedef std::function<int(uint8_t addr, uint8_t* buf, size_t len, bool nostop)> i2c_callback;
typedef std::function<void(int ms)> delay_callback;

struct OutputControl {
    uint16_t on_time;
    uint16_t off_time;
};

class PCA9685 {
public:
    PCA9685(uint8_t address, i2c_callback i2c_read_callback, i2c_callback i2c_write_callback, delay_callback delay);

    void Init();
    void Reset();

    void SetFrequency(float frequency);
    void SetServoFrequency();
    
    void SetChannelDutyCycle(uint8_t channel, float delay, float duty_cycle);
    void SetChannelsDutyCycles(uint8_t start_channel, uint8_t num_of_channels, float delay[], float duty_cycle[]);
    void SetChannelTimes(uint8_t channel, uint16_t on_time, uint16_t off_time);
    void SetChannelsTimes(uint8_t start_channel, uint8_t num_of_channels, uint16_t on_times[], uint16_t off_times[]);
    void SetChannelPWM(uint8_t channel, uint16_t PWM);
    void SetChannelsPWM(uint8_t start_channel, uint8_t num_of_channels, uint16_t PWM[]);

    uint8_t GetRegisterValue(uint8_t reg_address);

private:
    uint8_t _address = ADDRESS_BASE;
    i2c_callback _i2c_read_callback;
    i2c_callback _i2c_write_callback;
    delay_callback _delay;

    void _WriteRegister(uint8_t reg_address, uint8_t value);
    uint8_t _ReadRegister(uint8_t reg_address);
    void _WriteChannels(uint8_t start_channel, uint8_t *buf, size_t buffer_length);
    void _WriteChannels(uint8_t start_channel, OutputControl *buf, size_t buffer_length);

    OutputControl GetChannelTimes(float delay, float duty_cycle);
    
};

class Servo {
public:
    Servo(float min_angle, float max_angle, uint16_t min_pwm, uint16_t max_pwm);

    uint16_t PwmFromAngle(float angle);
    float getMinAngle();
    float getMaxAngle();

private:
    float _min_angle = 0;
    float _max_angle = 180;
    uint16_t _min_pwm;
    uint16_t _max_pwm;

};

#endif
