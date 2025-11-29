# Makefile (fixed example)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I. # Add -I. to include current dir in search path
LDFLAGS = -lpigpio -ltensorflow-lite -ldl -lpthread

# List all your object files
OBJS = App.o RPI3B_HW_LIB/ReadSensor.o TFInference/WeatherInference.o

app: $(OBJS)
	$(CXX) $(CXXFLAGS) -o app $(OBJS) $(LDFLAGS)

App.o: App.cpp App.hpp RPI3B_HW_LIB/ReadSensor.hpp
	$(CXX) $(CXXFLAGS) -c App.cpp

RPI3B_HW_LIB/ReadSensor.o: RPI3B_HW_LIB/ReadSensor.cpp RPI3B_HW_LIB/ReadSensor.hpp
	$(CXX) $(CXXFLAGS) -c RPI3B_HW_LIB/ReadSensor.cpp -o RPI3B_HW_LIB/ReadSensor.o

TFInference/WeatherInference.o: TFInference/WeatherInference.cpp TFInference/WeatherInference.hpp
	$(CXX) $(CXXFLAGS) -c TFInference/WeatherInference.cpp -o TFInference/WeatherInference.o

clean:
	rm -f App.o RPI3B_HW_LIB/ReadSensor.o TFInference/WeatherInference.o app


# scp transfer main.cpp, Makefile, RPI3B_HW_LIB (whole directory)
# to dev@192.168.68.105:/home/dev/Pico-Assistant/
transfer:
	scp -r App.cpp App.hpp Makefile RPI3B_HW_LIB TFInference prediction_model/*.tflite prediction_model/*.keras prediction_model/*.pkl dev@192.168.68.105:/home/dev/Pico-Assistant/
	