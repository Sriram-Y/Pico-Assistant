#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <pigpio.h>
#include <chrono>
#include <iostream>
#include <ctime>

#include "TFInference/WeatherInference.hpp"
#include "RPI3B_HW_LIB/ReadSensor.hpp"

#define GPIO_PIN 4

#define PREDICT_TEMP_CMD 0
#define PREDICT_HUM_CMD 1
#define READ_TEMP_CMD 2
#define READ_HUM_CMD 3
#define INVALID_CMD 255

class WeatherApp
{
public:
    // Command utilities
    uint8_t parse_command(std::string cmd);
    bool get_command_result(uint8_t cmd);

    // Sensor utilities
    bool read_temperature();
    bool read_humidity();

    // LR utilities
    bool predict_weather(uint8_t cmd);

    std::string get_time_tomorrow();

    uint8_t temperature_;
    uint8_t humidity_;

    const std::string TEMP_MODEL_PATH = "temperature_model.tflite";
    const std::string HUM_MODEL_PATH = "humidity_model.tflite";

    std::unique_ptr<WeatherInference> temperatureInference_ = std::make_unique<WeatherInference>(TEMP_MODEL_PATH);
    std::unique_ptr<WeatherInference> humidityInference_ = std::make_unique<WeatherInference>(HUM_MODEL_PATH);
};