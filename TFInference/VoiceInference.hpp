#pragma once

#include <string>
#include <memory>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

class VoiceInference
{
public:
    VoiceInference(const std::string &model_path, const std::string wav_path);

    float predict(float input_value);
};
