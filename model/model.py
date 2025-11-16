import tensorflow as tf
import numpy as np
import os

# ----------------------------------------------------------
# CONFIG (Must match your training script)
# ----------------------------------------------------------
SAMPLE_RATE = 16000
MEL_BINS = 64
FRAME_LENGTH = 640
FRAME_STEP = 320
TARGET_SAMPLES = int(1.5 * SAMPLE_RATE) # 24000 samples

# ----------------------------------------------------------
# PREPROCESSING FUNCTION (Copied from your script)
# This function must be pure TensorFlow ops
# ----------------------------------------------------------
def wav_to_melspectrogram(audio):
    """Convert waveform to log-mel spectrogram."""
    # Ensure input is the correct length
    # Note: This Lambda will operate on a batch, so shape is (batch, 24000)
    
    stft = tf.signal.stft(audio,
                          frame_length=FRAME_LENGTH,
                          frame_step=FRAME_STEP,
                          window_fn=tf.signal.hann_window, # Explicitly add default
                          pad_end=False) # Explicitly add default
    spectrogram = tf.abs(stft)

    # This is computed at graph-build time and baked in as a constant
    mel_filter = tf.signal.linear_to_mel_weight_matrix(
        num_mel_bins=MEL_BINS,
        num_spectrogram_bins=spectrogram.shape[-1],
        sample_rate=SAMPLE_RATE,
        lower_edge_hertz=80.0,
        upper_edge_hertz=7600.0
    )

    mel_spectrogram = tf.matmul(spectrogram, mel_filter)
    mel_spectrogram = tf.math.log(mel_spectrogram + 1e-6)
    
    # The CNN model expects a channel dimension
    mel_spectrogram = tf.expand_dims(mel_spectrogram, axis=-1)
    return mel_spectrogram

# ----------------------------------------------------------
# LOAD SAVED CNN MODEL
# ----------------------------------------------------------
print("Loading CNN model from pico_speech_model.h5...")
cnn_model = tf.keras.models.load_model("pico_speech_model.h5")
print("Model loaded successfully.")
cnn_model.summary()

# ----------------------------------------------------------
# BUILD NEW MODEL WITH PREPROCESSING
# ----------------------------------------------------------
print("Building new model with preprocessing...")

# 1. Define the input layer for raw audio
raw_audio_input = tf.keras.Input(
    shape=(TARGET_SAMPLES,), 
    batch_size=1, # Explicitly set batch size 1 for inference
    dtype=tf.float32, 
    name="raw_audio"
)

# 2. Add the preprocessing Lambda layer
mel_spec = tf.keras.layers.Lambda(
    wav_to_melspectrogram,
    name="log_mel_spectrogram"
)(raw_audio_input)

# 3. Connect the preprocessing output to the CNN model's input
cnn_output = cnn_model(mel_spec)

# 4. Create the new, end-to-end model
full_model = tf.keras.Model(
    inputs=raw_audio_input, 
    outputs=cnn_output, 
    name="pico_speech_e2e"
)

full_model.summary()

# ----------------------------------------------------------
# CONVERT TO TFLITE WITH "SELECT TF OPS"
# ----------------------------------------------------------
print("Converting new model to TFLite...")

converter = tf.lite.TFLiteConverter.from_keras_model(full_model)

# *** THIS IS THE MOST IMPORTANT PART ***
# This enables the tf.signal ops (like STFT) to run in TFLite
converter.target_spec.supported_ops = [
    tf.lite.OpsSet.TFLITE_BUILTINS,  # Enable TFLite ops
    tf.lite.OpsSet.SELECT_TF_OPS    # Enable select TensorFlow ops
]

# Set the input shape (required for TFLite conversion with dynamic ops)
converter.optimizations = [tf.lite.Optimize.DEFAULT]

tflite_model = converter.convert()

# ----------------------------------------------------------
# SAVE NEW TFLITE MODEL
# ----------------------------------------------------------
output_filename = "pico_speech_model_with_preprocessing.tflite"
with open(output_filename, "wb") as f:
    f.write(tflite_model)

print(f"\nSuccessfully saved new model to: {output_filename}")
print("This model accepts (1, 24000) float32 audio samples as input.")