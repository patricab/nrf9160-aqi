#cmake_minimum_required(VERSION 3.13.1)
#
#set(PM_STATIC_YML_FILE
#  ${CMAKE_CURRENT_SOURCE_DIR}/configuration/${BOARD}/pm_static.yml
#  )
#
#find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
#project(lib)

target_sources(app PRIVATE dgso3.c)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR})
