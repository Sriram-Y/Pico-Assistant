CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I.
LDFLAGS = -lpigpio -ltensorflow-lite -ldl -lpthread

OBJS = WeatherApp.o RPI3B_HW_LIB/ReadSensor.o TFInference/WeatherInference.o

WeatherApp: $(OBJS)
	$(CXX) $(CXXFLAGS) -o WeatherApp $(OBJS) $(LDFLAGS)

WeatherApp.o: WeatherApp.cpp WeatherApp.hpp RPI3B_HW_LIB/ReadSensor.hpp
	$(CXX) $(CXXFLAGS) -c WeatherApp.cpp

RPI3B_HW_LIB/ReadSensor.o: RPI3B_HW_LIB/ReadSensor.cpp RPI3B_HW_LIB/ReadSensor.hpp
	$(CXX) $(CXXFLAGS) -c RPI3B_HW_LIB/ReadSensor.cpp -o RPI3B_HW_LIB/ReadSensor.o

TFInference/WeatherInference.o: TFInference/WeatherInference.cpp TFInference/WeatherInference.hpp
	$(CXX) $(CXXFLAGS) -c TFInference/WeatherInference.cpp -o TFInference/WeatherInference.o

clean:
	rm -f WeatherApp.o RPI3B_HW_LIB/ReadSensor.o TFInference/WeatherInference.o WeatherApp

transfer:
	scp -r WeatherApp.cpp WeatherApp.hpp Makefile RPI3B_HW_LIB TFInference prediction_model/*.tflite prediction_model/*.keras prediction_model/*.pkl ./weather_app_test.sh dev@192.168.68.105:/home/dev/Pico-Assistant/
	