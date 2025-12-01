#include "RecordAudio.hpp"

#include <iostream>
#include <cstring>

RecordAudio::RecordAudio() 
    : m_pcmHandle(nullptr), m_buffer(nullptr), m_sndFile(nullptr), 
      m_totalFramesWritten(0), m_initialized(false), m_recording(false),
      m_latencyUs(20000) // 20ms latency target
{
}

RecordAudio::~RecordAudio() {
    cleanup();
}

bool RecordAudio::initialize(const std::string& filename, const std::string& device, unsigned int sampleRate, int channels) {
    if (m_initialized) cleanup(); // Safety reset

    m_sampleRate = sampleRate;
    m_channels = channels;
    int rc;
    int dir;
    unsigned int val = m_sampleRate;

    // 1. Open ALSA PCM device
    rc = snd_pcm_open(&m_pcmHandle, device.c_str(), SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        std::cerr << "[RecordAudio] Unable to open pcm device: " << snd_strerror(rc) << std::endl;
        return false;
    }

    // 2. Configure Hardware Parameters
    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(m_pcmHandle, params);

    snd_pcm_hw_params_set_access(m_pcmHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(m_pcmHandle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(m_pcmHandle, params, m_channels);
    snd_pcm_hw_params_set_rate_near(m_pcmHandle, params, &val, &dir);

    // Set period time to ~20ms for low latency responsiveness
    unsigned int periodTime = m_latencyUs;
    snd_pcm_hw_params_set_period_time_near(m_pcmHandle, params, &periodTime, &dir);

    rc = snd_pcm_hw_params(m_pcmHandle, params);
    if (rc < 0) {
        std::cerr << "[RecordAudio] Unable to set hw parameters: " << snd_strerror(rc) << std::endl;
        return false;
    }

    // 3. Allocate Buffer
    snd_pcm_hw_params_get_period_size(params, &m_framesPerPeriod, &dir);
    m_bufferSize = m_framesPerPeriod * m_channels * 2; // 2 bytes per sample (S16)
    m_buffer = (char *) malloc(m_bufferSize);

    // 4. Open Output File (libsndfile)
    memset(&m_sfInfo, 0, sizeof(m_sfInfo));
    m_sfInfo.samplerate = m_sampleRate;
    m_sfInfo.channels = m_channels;
    m_sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    m_sndFile = sf_open(filename.c_str(), SFM_WRITE, &m_sfInfo);
    if (!m_sndFile) {
        std::cerr << "[RecordAudio] Error opening output file: " << sf_strerror(m_sndFile) << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

void RecordAudio::start() {
    if (m_initialized && m_pcmHandle) {
        snd_pcm_prepare(m_pcmHandle);
        m_recording = true;
    }
}

bool RecordAudio::processLoop() {
    if (!m_initialized || !m_recording) return false;

    // A. Read from ALSA (Blocks for ~20ms)
    int rc = snd_pcm_readi(m_pcmHandle, m_buffer, m_framesPerPeriod);

    if (rc == -EPIPE) {
        // Overrun (Buffer full)
        snd_pcm_prepare(m_pcmHandle);
        return true; // Recovered, continue
    } else if (rc < 0) {
        std::cerr << "[RecordAudio] Read error: " << snd_strerror(rc) << std::endl;
        return false;
    }

    // B. Write to File
    if (rc > 0) {
        sf_write_short(m_sndFile, (short*)m_buffer, rc * m_channels);
        m_totalFramesWritten += rc;
    }

    return true;
}

void RecordAudio::stop() {
    if (m_recording && m_pcmHandle) {
        // Immediate drop (don't wait for drain)
        snd_pcm_drop(m_pcmHandle);
        m_recording = false;
    }
    cleanup();
}

sf_count_t RecordAudio::getTotalFrames() const {
    return m_totalFramesWritten;
}

void RecordAudio::cleanup() {
    if (m_sndFile) {
        sf_close(m_sndFile);
        m_sndFile = nullptr;
    }
    if (m_pcmHandle) {
        snd_pcm_close(m_pcmHandle);
        m_pcmHandle = nullptr;
    }
    if (m_buffer) {
        free(m_buffer);
        m_buffer = nullptr;
    }
    m_initialized = false;
    m_recording = false;
}