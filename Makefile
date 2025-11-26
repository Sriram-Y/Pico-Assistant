CXX = aarch64-linux-gnu-g++
SRC = main.cpp
TARGET = app

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)


# scp transfer main.cpp, Makefile, RPI3B_HW_LIB (whole directory)
# to dev@192.168.68.105:/home/dev/Pico-Assistant/
transfer:
	scp -r main.cpp Makefile RPI3B_HW_LIB dev@192.168.68.105:/home/dev/Pico-Assistant/
	
