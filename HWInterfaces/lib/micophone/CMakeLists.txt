# Create a static library named 'micophone_lib'
add_library(micophone_lib STATIC
    micophone.cpp
    micophone.hpp
)

# Make the headers in this directory available to consumers
target_include_directories(micophone_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)