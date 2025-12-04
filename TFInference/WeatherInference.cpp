#include "WeatherInference.hpp"

#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <stdexcept>

#include "tensorflow/lite/kernels/register.h"

WeatherInference::WeatherInference(const std::string &model_path)
{
    // 1. Load Model
    model_ = tflite::FlatBufferModel::BuildFromFile(model_path.c_str());
    if (!model_)
    {
        throw std::runtime_error("Failed to load model: " + model_path);
    }

    // 2. Build Interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model_, resolver)(&interpreter_);
    if (!interpreter_)
    {
        throw std::runtime_error("Failed to create interpreter");
    }

    // 3. Allocate Tensors
    if (interpreter_->AllocateTensors() != kTfLiteOk)
    {
        throw std::runtime_error("Failed to allocate tensors");
    }
}

float WeatherInference::runInference(float input_value)
{
    float *input_tensor = interpreter_->typed_input_tensor<float>(0);
    
    input_tensor[0] = input_value;

    // Run Inference
    interpreter_->Invoke();

    float *output_tensor = interpreter_->typed_output_tensor<float>(0);
    return output_tensor[0];
}

float WeatherInference::preprocessTimestamp(const std::string &timestamp)
{
    std::tm tm = {};
    // parse time string
    if (strptime(timestamp.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == nullptr)
    {
        std::cerr << "Error parsing time" << std::endl;
        return 0.0f;
    }

    // Convert to Unix Epoch
    time_t raw_time = mktime(&tm);

    float unix_float = static_cast<float>(raw_time);

    // SCALE (StandardScaler logic: (x - u) / s)
    return (unix_float - SCALER_MEAN) / SCALER_SCALE;
}

uint8_t WeatherInference::predictValue(const std::string &timestamp)
{
    float scaled_input = preprocessTimestamp(timestamp);
    float result = this->runInference(scaled_input);

    return static_cast<uint8_t>(std::round(result));
}
