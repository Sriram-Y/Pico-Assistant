# Create a static library named 'sensor_lib'
add_library(sensor_lib STATIC
    sensor.cpp
    sensor.hpp
)

# Make the headers in this directory available to consumers
target_include_directories(sensor_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)