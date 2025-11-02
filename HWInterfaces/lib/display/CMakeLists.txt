# /HWInterfaces/lib/display/CMakeLists.txt

# Create a static library
add_library(display_lib STATIC
    display.cpp
    display.hpp
)

# Make headers available
target_include_directories(display_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link the Pico SDK libraries this module needs (NEW)
# 'PUBLIC' ensures that 'app' also links these automatically
target_link_libraries(display_lib PUBLIC
    pico_stdlib
    hardware_i2c  # Or hardware_spi, hardware_gpio, etc.
)