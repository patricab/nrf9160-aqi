cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dk)
zephyr_compile_definitions(PROJECT_NAME=${PROJECT_NAME})

#target_sources(app PRIVATE dgso3.c)
target_sources(app PRIVATE dk.c)
zephyr_include_directories(.)

add_subdirectory(lib)
zephyr_library_include_directories(lib)