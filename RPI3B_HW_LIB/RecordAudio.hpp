#pragma once

#include <string>
#include <alsa/asoundlib.h>
#include <sndfile.h>

class RecordAudio {
public:
    RecordAudio();
    ~RecordAudio();

    // Pre-initializes hardware and opens the output file. 
    // Call this before asking the user to start to ensure instant start.
    bool initialize(const std::string& filename, 
                   const std::string& device = "default", 
                   unsigned int sampleRate = 44100, 
                   int channels = 1);

    // Prepares the ALSA handle for immediate capture.
    void start();

    // Captures a single period (chunk) of audio and writes it to the file.
    // Returns false if an error occurs.
    bool processLoop();

    // Stops the hardware immediately and finalizes the file.
    void stop();

    // Returns the total frames recorded so far.
    sf_count_t getTotalFrames() const;

private:
    // Configuration
    unsigned int m_sampleRate;
    int m_channels;
    unsigned int m_latencyUs; // Latency in microseconds

    // ALSA Resources
    snd_pcm_t *m_pcmHandle;
    snd_pcm_uframes_t m_framesPerPeriod;
    char *m_buffer;
    int m_bufferSize;

    // File Resources
    SNDFILE *m_sndFile;
    SF_INFO m_sfInfo;
    sf_count_t m_totalFramesWritten;

    // State
    bool m_initialized;
    bool m_recording;
    
    void cleanup();
};
