# Define the library source files
# If you have a .cpp file (e.g. ReadSensor.cpp), add it here next to the .hpp
add_library(RPI3B_HW_LIB STATIC
    ReadSensor.hpp
    # ReadSensor.cpp  <-- Uncomment this if you have the implementation file
)

# Define the include directories
# 'PUBLIC' means any target linking to this lib (like your main app)
# will automatically get this directory added to their include path.
target_include_directories(RPI3B_HW_LIB PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})