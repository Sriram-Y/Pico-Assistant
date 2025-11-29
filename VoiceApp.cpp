#include "VoiceApp.hpp"

int main(int argc, char *argv[])
{
    std::unique_ptr<VoiceApp> app = std::make_unique<VoiceApp>();
    std::cout << app->placeholder_function() << std::endl;

    return 0;
}

bool VoiceApp::placeholder_function()
{
    std::unique_ptr<VoiceInference> voice_inference = std::make_unique<VoiceInference>("prediction_model/voice_model.tflite");
    float result = voice_inference->predict(3.14f);
    std::cout << "Inference result: " << result << std::endl;
    return true;
}
