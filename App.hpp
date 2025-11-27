#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <pigpio.h>

#include "RPI3B_HW_LIB/ReadSensor.hpp"

class App
{
public:
    // Command utilities
    uint8_t parse_command(std::string cmd);
    bool get_command_result(uint8_t cmd);

    // Sensor utilities
    bool read_temperature();
    bool read_humidity();

    // LR utilities
    bool predict_temperature();
    bool predict_humidity();

    uint8_t temperature_;
    uint8_t humidity_;
};