# Hexapod code
* [General info](general-info)
* [Hardware setup](hardware-setup)
* [Firmware setup](firmware-setup)
* [Usage](usage)
* [Functionalities](functionalities)
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

## Functionalities

### IKSolve
IKSolve method is used to solve Inverse Kinematics for given leg and given end effector. It calculates three angles $\theta_1, \theta_2, \theta_3$ for three joints of leg. It uses this algorithm for calculations:

1. All coordinates are offset by leg offset, so that leg anchor is at (0, 0),

$Ef = Ef - (Ofc_x, Ofc_y, 0)$

2. $\theta_1$ is calculated,

$\gamma = \arctan(Ef_y, Ef_x)$

$\theta_1 = \gamma - Ofp_\phi$

$\theta_1$ needs to be in range $(-\pi, \pi>$, so if $\theta_1 > \pi$ then $\theta_1 = \theta_1 - 2\pi$

3. Coordinates are offset by coxa lenght, so that femur anchor is at (0, 0),

$Ef = Ef - (c_x, c_y, 0)$

4. P and K values are calculated,

$P = \sqrt{Ef_x^2 + Ef_y^2}$

$K = \sqrt{P^2 + Ef_z^2}$

5. $\theta_2$ is calculated

$\gamma = \arccos((Femur_l^2 + K^2 - Tibia_l^2)/(2 * Femur_l * K))$

$\delta = \arctan(Ef_z/P)$

$\theta_2 = \gamma + \delta$

6. $\theta_3$ is calculated

$\gamma = \arccos((Tibia_l^2 + Femur_l^2 - K^2)/(2 * Tibia_l * Femur_l))$

$\theta_3 = \pi - \gamma$

$Ef = (x, y, z)$ is end effector

$Ofc$ is cartesian leg offset

$Ofp$ is polar leg offset

### Step
Step method is called every loop. In this method actions are taken based on state of robot. Those actions are:
1. Check if state should be changed
2. If state != previousState or moves finished get new set of moves
3. Solve Invers Kinematics for current moves
4. Output signals to servos

Step 2 is different for walk state since leg moves are more independent and don't need to be tightly synchronized. In walk state first set of moves are initialized at the same time, but later moves are initialized when previous moves end which can be different for every leg.

### States
Currently there are four different states:
1. Reset,
2. Init,
3. Idle,
4. Walk

#### Reset state
Robot starts in reset state and enters it after failures. It initializes some basic variables and immediatley shitches to Init state

#### Init state
It moves legs to known positions in safe way in sets of three. First legs with even numbers are lifted and moved to default position, then the same happens for legs with odd numbers. After finishing procedure state is changed to Idle

#### Idle state
Currently it is similar to Init state. It moves legs to resting position in the same way that Init state moves them to known position. After finishing procedure legs stop moving and robot stays in Idle state. State is changed to Walk when input is received.

#### Walk state
In this state legs perform walk cycle. Walk state is divided into stance when leg moves on the ground and swing when leg is moving in the air. When entering state even legs are initialized to begin walk cycle and odd legs are initialized do be on 50% of walk cycle. This causes them to move in sync and creates tri-gait. other gaits can be implemented by changing percentage of walk cycle that legs start in and how long stance phase is. If microcontroller doesn't receive input for time specified in defines.h file state is switched back to Idle.

### Bluetooth
Robot uses BLE for communication. BLE specific methods are implemented according to BTstack specification and examples. BLE services and characteristics are specified in ble_hexapod.gatt file. If you want to specify new service please follow this schema:

1. Generate new GUID
2. Change first four bytes to 00000001

While creating new characteristic use GUID of service or last characteristic in service and increment first four bytes

## Additional resources
* BTstack examples - https://bluekitchen-gmbh.com/btstack/develop/#examples/examples/
* Inverse Kinematics - https://github.com/engineerm-jp/Inverse_Kinematics_YouTube/tree/main/Hexapod