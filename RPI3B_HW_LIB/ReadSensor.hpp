#pragma once

#include <cstdint>

class ReadSensor
{
public:
    ReadSensor();
    explicit ReadSensor(int gpioPin);

    // Set the GPIO pin for DHT22
    void setGpio(int gpioPin);

    // Read from the sensor (returns true on success)
    bool read();

    // Getters
    float getTemperatureC() const { return temperatureC; }
    float getHumidity() const { return humidity; }

private:
    int pin;
    float temperatureC;
    float humidity;

    bool readRawData(uint8_t data[5]);
};
