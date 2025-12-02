
#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <cmath>
#include "TFInference/VoiceInference.hpp"

#include "tensorflow/lite/kernels/register.h"

// Constructor: Loads the model and optional scorer
VoiceInference::VoiceInference(const std::string &modelPath, const std::string &scorerPath)
{
    int status = DS_CreateModel(modelPath.c_str(), &modelCtx);
    if (status != 0)
    {
        throw std::runtime_error("Failed to create DeepSpeech model. Error code: " + std::to_string(status));
    }

    if (!scorerPath.empty())
    {
        status = DS_EnableExternalScorer(modelCtx, scorerPath.c_str());
        if (status != 0)
        {
            // We don't throw here to allow running without a scorer, but we warn the user.
            std::cerr << "Warning: Could not enable scorer. Error code: " << status << std::endl;
        }
    }
}

// Destructor: Cleans up DeepSpeech memory
VoiceInference::~VoiceInference()
{
    if (modelCtx)
    {
        DS_FreeModel(modelCtx);
        modelCtx = nullptr;
    }
}

// Main inference function
std::string VoiceInference::infer(const std::string &wavPath)
{
    // 1. Open Audio File using libsndfile
    SF_INFO sfInfo;
    sfInfo.format = 0; // Must be set to 0 when opening for reading

    SNDFILE *sndFile = sf_open(wavPath.c_str(), SFM_READ, &sfInfo);
    if (!sndFile)
    {
        throw std::runtime_error("Could not open audio file: " + wavPath);
    }

    // 2. Validate Audio Format
    if (sfInfo.channels != 1)
    {
        sf_close(sndFile);
        throw std::runtime_error("Error: Audio file must be Mono (1 channel). Input has " + std::to_string(sfInfo.channels));
    }

    int modelSampleRate = DS_GetModelSampleRate(modelCtx);
    if (sfInfo.samplerate != modelSampleRate)
    {
        sf_close(sndFile);
        throw std::runtime_error("Error: Audio sample rate " + std::to_string(sfInfo.samplerate) +
                                 "Hz does not match model rate " + std::to_string(modelSampleRate) + "Hz.");
    }

    // 3. Read Audio Data
    // DeepSpeech expects 16-bit integers (shorts).
    // sf_read_short handles conversion from float/24-bit wavs automatically.
    std::vector<short> audioBuffer(sfInfo.frames);
    sf_count_t numItems = sf_read_short(sndFile, audioBuffer.data(), sfInfo.frames);
    sf_close(sndFile);

    if (numItems != sfInfo.frames)
    {
        std::cerr << "Warning: Read fewer frames than expected." << std::endl;
    }

    // 4. Perform Inference
    char *text = DS_SpeechToText(modelCtx, audioBuffer.data(), audioBuffer.size());

    std::string result = "";
    if (text)
    {
        result = std::string(text);
        DS_FreeString(text); // Important: Free the memory allocated by DeepSpeech
    }

    return result;
}
