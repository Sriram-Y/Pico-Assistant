#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "deepspeech.h"

#include <sndfile.h>

class VoiceInference
{
private:
    ModelState *modelCtx = nullptr;

public:
    VoiceInference(const std::string &modelPath, const std::string &scorerPath);

    ~VoiceInference();

    std::string infer(const std::string &wavPath);
};
