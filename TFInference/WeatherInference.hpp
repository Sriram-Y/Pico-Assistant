#pragma once

#include <string>
#include <memory>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

class WeatherInference
{
public:
    WeatherInference(const std::string &model_path);

    float predict(float input_value);

    float preprocessTimestamp(const std::string &timestamp);

    uint8_t predictTemperature(const std::string &timestamp);

    uint8_t predictHumidity(const std::string &timestamp);

private:
    std::unique_ptr<tflite::FlatBufferModel> model_;
    std::unique_ptr<tflite::Interpreter> interpreter_;

    const float SCALER_MEAN = 1758033831.146447f;
    const float SCALER_SCALE = 17479823.521328453f;
};
