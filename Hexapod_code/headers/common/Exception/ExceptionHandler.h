#pragma once
#include <stdio.h>
#include <exception>

void handle_exceptions(const std::exception &e) {
    printf("%s\n", e.what());
    while(true) {}
}