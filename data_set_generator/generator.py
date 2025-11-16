import os
from gtts import gTTS
from pydub import AudioSegment
import random

# --------------------------------------------
# CONFIGURATION
# --------------------------------------------
OUTPUT_DIR = "dataset"
NUM_SAMPLES_PER_PHRASE = 40

phrases = {
    "temperature": [
        "Pico, what's the temperature?",
    ],
    "humidity": [
        "Pico, what's the humidity?",
    ],
    "light_on": [
        "Pico, turn on the light.",
        "Pico, turn on the lights."
    ],
    "weather_query": [
        # Time/date variable examples
        "Pico, what will the weather be at eleven A M fifteen November twenty twenty five?",
        "Pico, what will the weather be at one P M twenty first December twenty twenty five?",
        "Pico, what will the weather be at seven A M tenth October twenty twenty six?",
        "Pico, what will the weather be at nine P M first January twenty twenty seven?"
    ]
}

# Create folders
for label in phrases.keys():
    os.makedirs(os.path.join(OUTPUT_DIR, label), exist_ok=True)

def synthesize_tts(text, filename):
    """Generate raw TTS audio."""
    tts = gTTS(text=text, lang="en")
    tts.save(filename)

def apply_audio_variation(infile, outfile):
    """Add pitch, speed, and volume variations."""
    audio = AudioSegment.from_file(infile)

    # Random speed: 0.9x – 1.1x
    speed = random.uniform(0.9, 1.1)
    new_frame_rate = int(audio.frame_rate * speed)
    audio = audio._spawn(audio.raw_data, overrides={"frame_rate": new_frame_rate})
    audio = audio.set_frame_rate(16000)

    # Random volume between -4dB and +4dB
    audio += random.uniform(-4, 4)

    # Export as wav
    audio.export(outfile, format="wav")

# --------------------------------------------
# GENERATE DATASET
# --------------------------------------------
counter = 0
for category, text_list in phrases.items():
    print(f"Generating: {category}")

    for i in range(NUM_SAMPLES_PER_PHRASE):
        # Pick a random phrase
        text = random.choice(text_list)

        # Generate temporary mp3 file
        temp_mp3 = "temp.mp3"
        wav_path = os.path.join(OUTPUT_DIR, category, f"{category}_{i:03d}.wav")

        synthesize_tts(text, temp_mp3)
        apply_audio_variation(temp_mp3, wav_path)

        counter += 1

print(f"\nDone! Generated {counter} audio files.")
