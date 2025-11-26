# Set the minimum required version of CMake (Pico SDK needs 3.13)
cmake_minimum_required(VERSION 3.13)

# --- Project Definition ---
# Define the project name (e.g., PicoAssistant)
# Add C and ASM for the Pico SDK
project(PicoAssistant CXX C ASM)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Your Project Code ---
# Add your hardware interface libraries
add_subdirectory(RPI3B_HW_LIB)

# Create the main executable
add_executable(app
    main.cpp
)
