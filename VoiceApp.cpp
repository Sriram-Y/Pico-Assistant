#include "VoiceApp.hpp"
#include <memory>

int main(int argc, char *argv[])
{
    // Instantiate our controller objects
    auto recorder = std::make_unique<RecordAudio>();
    auto console = std::make_unique<TerminalControl>();

    std::string filename = "recording.wav";

    // 1. Initialize Hardware
    // We do this BEFORE the user presses space so the start is instant.
    std::cout << "Initializing Audio Subsystem..." << std::endl;

    // You can change "default" to specific device like "plughw:1,0" if needed
    if (!recorder->initialize(filename, "default", 16000, 1))
    {
        std::cerr << "Failed to initialize recorder hardware." << std::endl;
        return 1;
    }

    std::cout << "Hardware Ready." << std::endl;
    std::cout << "Press [SPACE] to start recording..." << std::flush;

    // 2. Wait for Start Signal
    console->setRawMode(); // Enter non-blocking key detection mode

    while (true)
    {
        // Check if key is pressed AND if that key is space (' ')
        if (console->kbhit() && console->getch() == ' ')
        {
            break;
        }
        usleep(10000); // Sleep 10ms to prevent 100% CPU usage while waiting
    }

    // 3. Start Recording
    // \r moves cursor to start of line to overwrite the "Press Space" message
    std::cout << "\rRECORDING! Press [SPACE] to stop.        " << std::flush;

    recorder->start();

    // 4. Recording Loop
    bool isRecording = true;
    while (isRecording)
    {
        // A. Process one chunk of audio (captures ~20ms of audio)
        // processLoop returns false if a critical error occurs (e.g., USB unplugged)
        if (!recorder->processLoop())
        {
            std::cerr << "\nError during recording loop." << std::endl;
            break;
        }

        // B. Check for Stop Signal
        if (console->kbhit())
        {
            if (console->getch() == ' ')
            {
                isRecording = false;
            }
        }
    }

    // 5. Cleanup
    recorder->stop();
    console->resetMode(); // Restore terminal to normal behavior

    std::cout << "\nStopped." << std::endl;
    std::cout << "Saved " << recorder->getTotalFrames() << " frames to " << filename << std::endl;

    std::string modelPath = "deepspeech-0.9.3-models.tflite";
    std::string scorerPath = "deepspeech-0.9.3-models.scorer";
    std::string wavPath = "recording.wav";

    try
    {
        // Instantiate the class
        std::unique_ptr<VoiceInference> vinference = std::make_unique<VoiceInference>(modelPath, scorerPath);

        std::cout << "Model loaded. Processing " << wavPath << "..." << std::endl;

        // Call the inference method
        std::string transcript = vinference->infer(wavPath);

        std::cout << "Transcription: " << transcript << std::endl;

        // std::system sudo call to WeatherApp and transcript string
        std::system(("sudo ./WeatherApp \"" + transcript + "\"").c_str());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
