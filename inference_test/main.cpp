#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <cstdint>
#include "model_data.h"
#include "labels.h"

// TensorFlow Lite Micro headers
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// WAV reading
#include <fstream>

#define SAMPLE_RATE 16000
#define NUM_SAMPLES 24000  // 1.5 sec

// Audio preprocessing: simple log-magnitude STFT placeholder
void wav_to_input(const char* filename, float* input_data, int input_size)
{
    std::ifstream f(filename, std::ios::binary);
    if (!f.is_open()) {
        printf("Cannot open WAV file %s\n", filename);
        exit(1);
    }

    // Skip WAV header (44 bytes)
    f.seekg(44);

    std::vector<int16_t> buffer(NUM_SAMPLES);
    f.read(reinterpret_cast<char*>(buffer.data()), NUM_SAMPLES * sizeof(int16_t));

    // Convert to float [-1, 1]
    for (int i = 0; i < NUM_SAMPLES; i++) {
        input_data[i] = buffer[i] / 32768.0f;
    }

    f.close();
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: %s path/to/audio.wav\n", argv[0]);
        return 1;
    }

    const char* wav_file = argv[1];

    // Load WAV data
    float input_buffer[NUM_SAMPLES];
    wav_to_input(wav_file, input_buffer, NUM_SAMPLES);

    // -------------------------
    // Setup TFLite Micro
    // -------------------------
    const tflite::Model* model = tflite::GetModel(g_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("Model schema version %d not supported\n", model->version());
        return 1;
    }

    // Ops resolver
    tflite::AllOpsResolver resolver;

    // Tensor arena (adjust size if needed)
    constexpr int kTensorArenaSize = 1024*1024; // 1 MB
    uint8_t tensor_arena[kTensorArenaSize];

    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, kTensorArenaSize);
    TfLiteStatus allocate_status = interpreter.AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        printf("AllocateTensors() failed\n");
        return 1;
    }

    // Get input tensor
    TfLiteTensor* input = interpreter.input(0);
    if (input->type != kTfLiteFloat32) {
        printf("Input type not float32\n");
        return 1;
    }

    // Copy audio samples to input tensor
    // Here we just flatten raw audio; replace with proper mel-spectrogram preprocessing if needed
    int input_len = input->bytes / sizeof(float);
    for (int i = 0; i < input_len && i < NUM_SAMPLES; i++) {
        input->data.f[i] = input_buffer[i];
    }

    // Run inference
    if (interpreter.Invoke() != kTfLiteOk) {
        printf("Inference failed\n");
        return 1;
    }

    // Get output tensor
    TfLiteTensor* output = interpreter.output(0);

    // Find max probability
    int max_index = 0;
    float max_value = output->data.f[0];
    for (int i = 1; i < num_labels; i++) {
        if (output->data.f[i] > max_value) {
            max_value = output->data.f[i];
            max_index = i;
        }
    }

    printf("Detected command: %s (%.2f)\n", labels[max_index], max_value);

    return 0;
}
