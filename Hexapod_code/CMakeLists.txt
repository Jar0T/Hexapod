cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico_w)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

include(pico_sdk_import.cmake)

project(Hexapod)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pico_sdk_init()

add_executable(Hexapod
    main.cpp
    src/BLE/BLE.cpp
    src/helpers/helpers.cpp
    src/Vector/Vector2.cpp
    src/Vector/Vector3.cpp
    src/IKSolver/IKSolver.cpp
    src/Common/Leg.cpp
    src/RCData/RCData.cpp
    src/TimeInfo/TimeInfo.cpp
)

# Add pico_stdlib library, add more if used
target_link_libraries(Hexapod
    pico_stdlib
    hardware_i2c
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
)
target_link_libraries(Hexapod ${CMAKE_SOURCE_DIR}/libs/pca9685/libpca9685.a)

target_include_directories(Hexapod PRIVATE
    ${CMAKE_CURRENT_LIST_DIR} # For btstack config
)
pico_btstack_make_gatt_header(Hexapod PRIVATE "${CMAKE_CURRENT_LIST_DIR}/ble_hexapod.gatt")

include_directories(headers)
include_directories(${CMAKE_SOURCE_DIR}/libs/pca9685/include)

# enable usb output, disable uart output
pico_enable_stdio_usb(Hexapod 1)
pico_enable_stdio_uart(Hexapod 0)

# Need to generate UF2 file for upload to RP2040
pico_add_extra_outputs(Hexapod)