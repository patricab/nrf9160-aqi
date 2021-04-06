cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dk)
zephyr_compile_definitions(PROJECT_NAME=${PROJECT_NAME})

# Main program
target_sources(app PRIVATE dk.c)

# Library files
zephyr_library_include_directories(lib)
add_subdirectory(lib)