cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lib)
zephyr_compile_definitions(PROJECT_NAME=${PROJECT_NAME})

#target_sources(app PRIVATE dgso3.c)
target_sources(app PRIVATE sps30_I2C.c)

zephyr_include_directories(.)