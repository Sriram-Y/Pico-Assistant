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
#include <alsa/asoundlib.h>

#include "TFInference/VoiceInference.hpp"
#include "RPI3B_HW_LIB/RecordAudio.hpp"
#include "RPI3B_HW_LIB/TerminalControl.hpp"

int main(int argc, char *argv[])
{
    auto recorder = std::make_unique<RecordAudio>();
    auto console = std::make_unique<TerminalControl>();

    std::string filename = "recording.wav";

    std::cout << "Initializing Audio Subsystem..." << std::endl;

    if (!recorder->initialize(filename, "default", 16000, 1))
    {
        std::cerr << "Failed to initialize recorder hardware." << std::endl;
        return 1;
    }

    std::cout << "Hardware Ready." << std::endl;
    std::cout << "Press [SPACE] to start recording..." << std::flush;

    console->setRawMode();

    while (true)
    {
        if (console->kbhit() && console->getch() == ' ')
        {
            break;
        }
        usleep(10000);
    }

    std::cout << "\rRECORDING! Press [SPACE] to stop.        " << std::flush;

    recorder->start();

    bool isRecording = true;
    while (isRecording)
    {
        if (!recorder->processLoop())
        {
            std::cerr << "\nError during recording loop." << std::endl;
            break;
        }

        if (console->kbhit())
        {
            if (console->getch() == ' ')
            {
                isRecording = false;
            }
        }
    }

    recorder->stop();
    console->resetMode();

    std::cout << "\nStopped." << std::endl;

    std::string modelPath = "deepspeech-0.9.3-models.tflite";
    std::string scorerPath = "deepspeech-0.9.3-models.scorer";
    std::string wavPath = "recording.wav";

    try
    {
        std::unique_ptr<VoiceInference> vinference = std::make_unique<VoiceInference>(modelPath, scorerPath);
        std::cout << "Model loaded. Processing " << wavPath << "..." << std::endl;

        std::string transcript = vinference->infer(wavPath);
        std::cout << "Transcription: " << transcript << std::endl;

        // call weather app with the transcript
        std::system(("sudo ./WeatherApp \"" + transcript + "\"").c_str());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
