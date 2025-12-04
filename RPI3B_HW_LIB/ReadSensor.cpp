#include "ReadSensor.hpp"

#include <pigpio.h>
#include <iostream>
#include <unistd.h>

ReadSensor::ReadSensor() : pin(-1), temperatureC(0), humidity(0) {}

ReadSensor::ReadSensor(int gpioPin)
    : pin(gpioPin), temperatureC(0), humidity(0) {}

void ReadSensor::setGpio(int gpioPin)
{
    pin = gpioPin;
}

bool ReadSensor::read()
{
    if (pin < 0)
    {
        std::cerr << "GPIO pin not set!\n";
        return false;
    }

    uint8_t data[5] = {0};

    if (!readRawData(data))
    {
        return false;
    }

    uint16_t rawHumidity = (data[0] << 8) | data[1];
    uint16_t rawTemp = (data[2] << 8) | data[3];

    humidity = rawHumidity / 10.0f;

    if (rawTemp & 0x8000)
    {
        rawTemp &= 0x7FFF;
        temperatureC = -rawTemp / 10.0f;
    }
    else
    {
        temperatureC = rawTemp / 10.0f;
    }

    return true;
}

bool ReadSensor::readRawData(uint8_t data[5])
{
    gpioSetMode(pin, PI_OUTPUT);
    gpioWrite(pin, PI_OFF);
    gpioDelay(20000); // pull low for 20ms to start
    gpioWrite(pin, PI_ON);
    gpioDelay(40);
    gpioSetMode(pin, PI_INPUT);

    // Wait for sensor response
    uint32_t count = 0;

    while (gpioRead(pin) == PI_ON)
    {
        if (++count > 10000)
            return false;
    }
    count = 0;
    while (gpioRead(pin) == PI_OFF)
    {
        if (++count > 10000)
            return false;
    }
    count = 0;
    while (gpioRead(pin) == PI_ON)
    {
        if (++count > 10000)
            return false;
    }

    // Read the 40 bits
    for (int i = 0; i < 40; i++)
    {
        while (gpioRead(pin) == PI_OFF);

        uint32_t t = gpioTick();
        
        while (gpioRead(pin) == PI_ON);

        uint32_t duration = gpioTick() - t;

        int byteIdx = i / 8;
        data[byteIdx] <<= 1;

        if (duration > 50)
        {
            data[byteIdx] |= 1;
        }
    }

    // Checksum
    if (((data[0] + data[1] + data[2] + data[3]) & 0xFF) != data[4])
    {
        return false;
    }

    return true;
}
