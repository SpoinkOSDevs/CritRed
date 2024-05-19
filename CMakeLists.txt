cmake_minimum_required(VERSION 3.10)
project(CritRed)

# Add a custom target to compile CritRed.cpp
add_custom_target(CritRed
    COMMAND g++ CritRed.cpp -o CritRed
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Compiling CritRed.cpp"
)
