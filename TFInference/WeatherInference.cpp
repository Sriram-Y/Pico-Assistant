#include "WeatherInference.hpp"

#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <cmath>

#include "tensorflow/lite/kernels/register.h"

WeatherInference::WeatherInference(const std::string &model_path)
{
    // 1. Load Model
    model_ = tflite::FlatBufferModel::BuildFromFile(model_path.c_str());
    if (!model_)
    {
        std::cerr << "Failed to load model: " << model_path << std::endl;
        exit(1);
    }

    // 2. Build Interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model_, resolver)(&interpreter_);
    if (!interpreter_)
    {
        std::cerr << "Failed to create interpreter" << std::endl;
        exit(1);
    }

    // 3. Allocate Tensors
    if (interpreter_->AllocateTensors() != kTfLiteOk)
    {
        std::cerr << "Failed to allocate tensors" << std::endl;
        exit(1);
    }
}

float WeatherInference::predict(float input_value)
{
    // Get Input Tensor Pointer
    float *input_tensor = interpreter_->typed_input_tensor<float>(0);

    // Assign value
    input_tensor[0] = input_value;

    // Run Inference
    interpreter_->Invoke();

    // Get Output
    float *output_tensor = interpreter_->typed_output_tensor<float>(0);
    return output_tensor[0];
}

float WeatherInference::preprocessTimestamp(const std::string &timestamp)
{
    std::tm tm = {};
    // Parse "2025-09-04 20:47:48"
    // Note: strptime is standard on Linux/Mac. On Windows use std::get_time
    if (strptime(timestamp.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == nullptr)
    {
        std::cerr << "Error parsing time" << std::endl;
        return 0.0f;
    }

    // Convert to Unix Epoch
    // Note: mktime assumes local time. If data is UTC, use timegm
    time_t raw_time = mktime(&tm);

    // Cast to float for calculation
    float unix_float = static_cast<float>(raw_time);

    // SCALE (StandardScaler logic: (x - u) / s)
    return (unix_float - SCALER_MEAN) / SCALER_SCALE;
}

uint8_t WeatherInference::predictTemperature(const std::string &timestamp)
{
    // 1. Preprocess
    float scaled_input = preprocessTimestamp(timestamp);

    // 2. Inference
    float result = this->predict(scaled_input);

    // 3. Cast to uint8_t (rounding to nearest whole number)
    return static_cast<uint8_t>(std::round(result));
}

uint8_t WeatherInference::predictHumidity(const std::string &timestamp)
{
    // 1. Preprocess
    float scaled_input = preprocessTimestamp(timestamp);

    // 2. Inference
    float result = this->predict(scaled_input);

    // 3. Cast to uint8_t
    return static_cast<uint8_t>(std::round(result));
}
