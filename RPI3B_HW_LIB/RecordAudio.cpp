#include "RecordAudio.hpp"

#include <iostream>
#include <cstring>
#include <vector>

RecordAudio::RecordAudio()
    : isInitialized(false), isRecording(false)
{
    config.latencyUs = 20000;
    config.sampleRate = 0;
    config.channels = 0;
}

RecordAudio::~RecordAudio()
{
    cleanup();
}

bool RecordAudio::initialize(const std::string &filename, const std::string &device, unsigned int sampleRate, int channels)
{
    if (isInitialized)
    {
        cleanup();
    }

    config.sampleRate = sampleRate;
    config.channels = channels;
    
    int rc;
    int dir;
    unsigned int val = config.sampleRate;

    // open pcm device for recording
    rc = snd_pcm_open(&alsa.handle, device.c_str(), SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0)
    {
        std::cerr << "[RecordAudio] Unable to open pcm device: " << snd_strerror(rc) << std::endl;
        return false;
    }

    // alsa parameters
    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(alsa.handle, params);

    snd_pcm_hw_params_set_access(alsa.handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(alsa.handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(alsa.handle, params, config.channels);
    snd_pcm_hw_params_set_rate_near(alsa.handle, params, &val, &dir);

    unsigned int periodTime = config.latencyUs;
    snd_pcm_hw_params_set_period_time_near(alsa.handle, params, &periodTime, &dir);

    rc = snd_pcm_hw_params(alsa.handle, params);
    if (rc < 0)
    {
        std::cerr << "[RecordAudio] Unable to set hw parameters: " << snd_strerror(rc) << std::endl;
        return false;
    }

    snd_pcm_hw_params_get_period_size(params, &alsa.framesPerPeriod, &dir);
    alsa.bufferSize = alsa.framesPerPeriod * config.channels * 2; // 2 bytes per sample
    alsa.buffer = (char *)malloc(alsa.bufferSize);

    memset(&file.info, 0, sizeof(file.info));
    file.info.samplerate = config.sampleRate;
    file.info.channels = config.channels;
    file.info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    file.handle = sf_open(filename.c_str(), SFM_WRITE, &file.info);
    if (!file.handle)
    {
        std::cerr << "[RecordAudio] Error opening output file: " << sf_strerror(file.handle) << std::endl;
        return false;
    }

    isInitialized = true;
    return true;
}

void RecordAudio::start()
{
    if (isInitialized && alsa.handle)
    {
        snd_pcm_prepare(alsa.handle);
        isRecording = true;
    }
}

bool RecordAudio::processLoop()
{
    if (!isInitialized || !isRecording)
        return false;

    int rc = snd_pcm_readi(alsa.handle, alsa.buffer, alsa.framesPerPeriod);

    if (rc == -EPIPE)
    {
        // Overrun (Buffer full)
        snd_pcm_prepare(alsa.handle);
        return true; // Recovered, continue
    }
    else if (rc < 0)
    {
        std::cerr << "[RecordAudio] Read error: " << snd_strerror(rc) << std::endl;
        return false;
    }

    if (rc > 0)
    {
        sf_write_short(file.handle, (short *)alsa.buffer, rc * config.channels);
        file.totalFramesWritten += rc;
    }

    return true;
}

void RecordAudio::stop()
{
    if (isRecording && alsa.handle)
    {
        // Immediate drop (don't wait for drain)
        snd_pcm_drop(alsa.handle);
        isRecording = false;
    }
    cleanup();
}

void RecordAudio::cleanup()
{
    if (file.handle)
    {
        sf_close(file.handle);
        file.handle = nullptr;
    }
    if (alsa.handle)
    {
        snd_pcm_close(alsa.handle);
        alsa.handle = nullptr;
    }
    if (alsa.buffer)
    {
        free(alsa.buffer);
        alsa.buffer = nullptr;
    }
    isInitialized = false;
    isRecording = false;
}