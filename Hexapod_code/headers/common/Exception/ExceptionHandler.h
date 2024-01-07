#pragma once
#include <stdio.h>
#include <exception>
#include "RCData/RCData.h"

void handle_exceptions(const std::exception &e) {
    printf("%s\n", e.what());
    while (reset == false) {}
}