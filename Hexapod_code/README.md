# Hexapod code
* [General info](general-info)
* [Hardware setup](hardware-setup)
* [Firmware setup](firmware-setup)
* [Usage](usage)
* [Additional resources](additional-resources)

## General info
This is a firmware of Hexapod robot based on Raspberry Pico microcontroller.

## Hardware setup
I have used Raspberry Pico W microcontroller and two PCA9685 connected to I2C0. Servos from right side of robot are connected to first nine channels of first PCA and servos from left side are connected to first nine channels of second PCA.

## Firmware setup
In [defines.h](/headers/common/defines.h) file you can specify layout of your robot. For now only six-legged configuration is supported. # sign is number of leg going from 1 up to 6. When entering data you should start from back-right leg and go counterclockwise. Angles are measured in degrees and lenghts are measured in milimeters. Joint min and max angles are min and max allowed angles, calculated angles in that range are considered safe. If algorithm returns angles beyond this range robot will go into exception handling state and wait for reset. Servo min and max angles are angles that servo can obtain at min and max PWM values. They should be adjusted so that 0 angle is at position where leg is fully extended.

```
//polar coordinates of leg anchor point
#define LEG#_ANGLE 305
#define LEG#_OFFSET 95

#define COXA#_MIN_ANGLE -30
#define COXA#_MAX_ANGLE 30
#define COXA#_LENGTH 64.85
#define COXA#_SERVO_MIN_ANGLE -90
#define COXA#_SERVO_MAX_ANGLE 90
#define COXA#_SERVO_MIN_PWM 220
#define COXA#_SERVO_MAX_PWM 442

#define FEMUR#_MIN_ANGLE -50
#define FEMUR#_MAX_ANGLE 130
#define FEMUR#_LENGTH 120
#define FEMUR#_SERVO_MIN_ANGLE -50
#define FEMUR#_SERVO_MAX_ANGLE 130
#define FEMUR#_SERVO_MIN_PWM 220
#define FEMUR#_SERVO_MAX_PWM 442

#define TIBIA#_MIN_ANGLE -30
#define TIBIA#_MAX_ANGLE 150
#define TIBIA#_LENGTH 205
#define TIBIA#_SERVO_MIN_ANGLE -30
#define TIBIA#_SERVO_MAX_ANGLE 150
#define TIBIA#_SERVO_MIN_PWM 220
#define TIBIA#_SERVO_MAX_PWM 442
```

Rest of define values

```
// default distance from center to end effector in xy plane
#define DEFAULT_RADIUS 300
// default height of hexapod
#define DEFAULT_HEIGHT 100
// max time between rc data updates
#define MAX_NO_DATA_TIME 1000000
// length of move cycle in us
#define MOVE_CYCLE_LENGTH 1000000
// max speed in mm/cycle
#define MAX_SPEED 150
```

In [BLE.cpp](/src/BLE/BLE.cpp) file find ```sm_use_fixed_passkey_in_display_role(123456);``` line and change passkey to your liking. It is used when pairing device, which is necessary to write BLE characteristics.

## Usage
To build project you need Pico SDK.
If you have Pico SDK already installed then run Pico Visual Studio Code and configure project on CMake tab. Then build project using build button. After building project you can drop .uf2 file to Raspberry Pico or use PicoProbe to upload project to microcontroller.

## Additional resources
* BTstack examples - https://bluekitchen-gmbh.com/btstack/develop/#examples/examples/
* Inverse Kinematics - https://github.com/engineerm-jp/Inverse_Kinematics_YouTube/tree/main/Hexapod