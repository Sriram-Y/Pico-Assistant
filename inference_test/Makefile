# -----------------------------
# Makefile for TFLite Micro inference
# -----------------------------
# Set path to your TFLite Micro installation
TFLITE_MICRO_PATH = /home/sriram/tflite-micro

CXX = g++
CXXFLAGS = -O2 -std=c++17 -I$(TFLITE_MICRO_PATH)
LDFLAGS = 



SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = pico_inference

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)
