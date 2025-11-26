#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdint>

#include "RPI3B_HW_LIB/ReadSensor.hpp"

// Command utilities
uint8_t parse_command(std::string cmd);
uint8_t get_command_result(uint8_t cmd);

// Sensor utilities
uint8_t read_temperature();
uint8_t read_humidity();

// LR utilities
uint8_t predict_temperature();
uint8_t predict_humidity();

int main()
{
    const std::string speechCommand = "what is the humidity";
    const uint8_t res = get_command_result(parse_command(speechCommand));

    std::cout << "The result is: " << static_cast<int>(res) << std::endl;

    return 0;
}

uint8_t get_command_result(uint8_t cmd)
{
    uint8_t result_temp = 0;
    switch(cmd)
    {
    case 0:
        result_temp = predict_temperature();
        break;
    case 1:
        result_temp = predict_humidity();
        break;
    case 2:
        result_temp = static_cast<uint8_t>(read_temperature());
        break;
    case 3:
        result_temp = static_cast<uint8_t>(read_humidity());
        break;
    default:
        std::cout << "Unrecognized command or not command" << std::endl;
        break;
    }

    return result_temp;
}

uint8_t parse_command(std::string speechCommand)
{
    // if "tomorrow" & "temperature" is in the string return 0
    // if "tomorrow" & "humidity" is in the string return 1
    // if "temperature" found in speechCommand return 2
    // if "humidity" found in speechCommand return 3

    if (speechCommand.find("tomorrow") != std::string::npos &&
        speechCommand.find("temperature") != std::string::npos)
    {
        return 0;
    }
    else if (speechCommand.find("tomorrow") != std::string::npos &&
             speechCommand.find("humidity") != std::string::npos)
    {
        return 1;
    }
    else if (speechCommand.find("temperature") != std::string::npos)
    {
        return 2;
    }
    else if (speechCommand.find("humidity") != std::string::npos)
    {
        return 3;
    }
    else
    {
        return 255;
    }
}

uint8_t read_temperature()
{
    return 100;
}

uint8_t read_humidity()
{
    return 50;
}

uint8_t predict_temperature()
{
    return 75;
}

uint8_t predict_humidity()
{
    return 65;
}
