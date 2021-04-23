#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.13.1)

set(PM_STATIC_YML_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/configuration/${BOARD}/pm_static.yml
  )

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf9160-aqi)
zephyr_compile_definitions(PROJECT_NAME=${PROJECT_NAME})

# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/settings.c)
zephyr_include_directories(src)
# NORDIC SDK APP END

# Include application events and configuration headers
zephyr_library_include_directories(
  src/lwm2m
  src/ui
  src/lib
 )

# Application sources
add_subdirectory(src/lwm2m)
add_subdirectory(src/ui)
add_subdirectory(src/lib)
add_subdirectory(src/sensor/ipso)
