#pragma once

#include <string>
#include <alsa/asoundlib.h>
#include <sndfile.h>

struct AudioConfig
{
    unsigned int sampleRate;
    int channels;
    unsigned int latencyUs;
};

struct AlsaState
{
    snd_pcm_t *handle = nullptr;
    snd_pcm_uframes_t framesPerPeriod = 0;
    char *buffer = nullptr;
    int bufferSize = 0;
};

struct FileState
{
    SNDFILE *handle = nullptr;
    SF_INFO info = {0};
    sf_count_t totalFramesWritten = 0;
};

class RecordAudio
{
public:
    RecordAudio();
    ~RecordAudio();

    // Pre-initializes hardware and opens the output file.
    bool initialize(const std::string &filename,
                    const std::string &device = "default",
                    unsigned int sampleRate = 16000,
                    int channels = 1);

    // Prepares the ALSA handle for immediate capture.
    void start();

    // Captures a single period (chunk) of audio and writes it to the file.
    bool processLoop();

    // Stops the hardware immediately and finalizes the file.
    void stop();

private:
    void cleanup();

    AudioConfig config;
    AlsaState alsa;
    FileState file;

    bool isInitialized;
    bool isRecording;
};