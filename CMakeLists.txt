# Set the minimum required version of CMake (Pico SDK needs 3.13)
cmake_minimum_required(VERSION 3.13)

# --- Pico SDK Setup (NEW) ---
# Set the paths to the SDKs you added
set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk)
set(PICO_EXTRAS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pico-extras)

# Include the main SDK initialization script
# This MUST come before the project() command
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# --- Project Definition ---
# Define the project name (e.g., PicoAssistant)
# Add C and ASM for the Pico SDK
project(PicoAssistant CXX C ASM)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Initialize the SDK for this project
pico_sdk_init()

# --- Your Project Code ---
# Add your hardware interface libraries
add_subdirectory(HWInterfaces)

# Create the main executable
add_executable(app
    main.cpp
)

# Add include directories for your other libs
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/LLM/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/WakeWordDetection/lib
)

# Link all libraries to the 'app' executable
target_link_libraries(app PRIVATE
    # Your custom libraries
    display_lib
    micophone_lib
    sensor_lib

    # Standard Pico library (required)
    pico_stdlib

    # Add any other SDK libs you use (e.g., hardware_i2c, hardware_spi)
)

# --- Output Files (NEW) ---
# Create the .uf2 file for drag-and-drop programming
pico_add_extra_outputs(app)