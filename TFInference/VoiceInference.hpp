#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// DeepSpeech Header
#include "deepspeech.h"

// SndFile Header
#include <sndfile.h>

class VoiceInference
{
private:
    ModelState *modelCtx = nullptr;

public:
    // Constructor: Loads the model and optional scorer
    VoiceInference(const std::string &modelPath, const std::string &scorerPath);

    // Destructor: Cleans up DeepSpeech memory
    ~VoiceInference();

    // Main inference function
    std::string infer(const std::string &wavPath);
};
