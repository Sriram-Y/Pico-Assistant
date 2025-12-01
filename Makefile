CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I.
LDFLAGS = -lpigpio -ltensorflow-lite -lasound -lsndfile -ldl -lpthread

WEATHEROBJS = WeatherApp.o RPI3B_HW_LIB/ReadSensor.o TFInference/WeatherInference.o
VOICEOBJS = VoiceApp.o RPI3B_HW_LIB/RecordAudio.o RPI3B_HW_LIB/TerminalControl.o TFInference/VoiceInference.o

all: WeatherApp VoiceApp

WeatherApp: $(WEATHEROBJS)
	$(CXX) $(CXXFLAGS) -o WeatherApp $(WEATHEROBJS) $(LDFLAGS)

VoiceApp: $(VOICEOBJS)
	$(CXX) $(CXXFLAGS) -o VoiceApp $(VOICEOBJS) $(LDFLAGS)

WeatherApp.o: WeatherApp.cpp WeatherApp.hpp RPI3B_HW_LIB/ReadSensor.hpp TFInference/WeatherInference.hpp
	$(CXX) $(CXXFLAGS) -c WeatherApp.cpp

VoiceApp.o: VoiceApp.cpp VoiceApp.hpp RPI3B_HW_LIB/RecordAudio.o RPI3B_HW_LIB/TerminalControl.o TFInference/VoiceInference.hpp
	$(CXX) $(CXXFLAGS) -c VoiceApp.cpp

RPI3B_HW_LIB/ReadSensor.o: RPI3B_HW_LIB/ReadSensor.cpp RPI3B_HW_LIB/ReadSensor.hpp
	$(CXX) $(CXXFLAGS) -c RPI3B_HW_LIB/ReadSensor.cpp -o RPI3B_HW_LIB/ReadSensor.o

RPI3B_HW_LIB/RecordAudio.o: RPI3B_HW_LIB/RecordAudio.cpp RPI3B_HW_LIB/RecordAudio.hpp
	$(CXX) $(CXXFLAGS) -c RPI3B_HW_LIB/RecordAudio.cpp -o RPI3B_HW_LIB/RecordAudio.o

RPI3B_HW_LIB/TerminalControl.o: RPI3B_HW_LIB/TerminalControl.cpp RPI3B_HW_LIB/TerminalControl.hpp
	$(CXX) $(CXXFLAGS) -c RPI3B_HW_LIB/TerminalControl.cpp -o RPI3B_HW_LIB/TerminalControl.o

TFInference/WeatherInference.o: TFInference/WeatherInference.cpp TFInference/WeatherInference.hpp
	$(CXX) $(CXXFLAGS) -c TFInference/WeatherInference.cpp -o TFInference/WeatherInference.o

TFInference/VoiceInference.o: TFInference/VoiceInference.cpp TFInference/VoiceInference.hpp
	$(CXX) $(CXXFLAGS) -c TFInference/VoiceInference.cpp -o TFInference/VoiceInference.o

clean:
	rm -f WeatherApp.o VoiceApp.o RPI3B_HW_LIB/*.o TFInference/*.o WeatherApp VoiceApp

# FIXME: Maybe missing: voice_model/*.keras voice_model/*.pkl
transfer:
	scp -r WeatherApp.cpp WeatherApp.hpp Makefile RPI3B_HW_LIB TFInference prediction_model/*.tflite prediction_model/*.keras prediction_model/*.pkl ./weather_app_test.sh dev@192.168.1.34:/home/dev/Pico-Assistant/
	scp -r VoiceApp.cpp VoiceApp.hpp voice_model/*.tflite dev@192.168.1.34:/home/dev/Pico-Assistant/
	