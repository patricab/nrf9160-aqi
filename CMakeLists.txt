#
<<<<<<< HEAD
# Copyright (c) 2021 Nordic Semiconductor ASA
=======
# Copyright (c) 2018 Nordic Semiconductor
>>>>>>> 309af76... Application is now based on LWM2M-client sample from NCS v1.5.0
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.13.1)

<<<<<<< HEAD
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(asset_tracker_v2)

# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)
# NORDIC SDK APP END

# Include source directory
target_include_directories(app PRIVATE src)

# Application directories
add_subdirectory(src/modules)
add_subdirectory(src/events)

add_subdirectory_ifdef(CONFIG_CLOUD_MODULE src/cloud)
add_subdirectory_ifdef(CONFIG_UI_MODULE src/led)
add_subdirectory_ifdef(CONFIG_SENSOR_MODULE src/ext_sensors)
add_subdirectory_ifdef(CONFIG_WATCHDOG_APPLICATION src/watchdog)
=======
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
  sensor/bh1749
  sensor/bme680
  sensor/dgso3
  sensor/sensor_sim
  sensor/sps30
  lib
  )

# Application sources
add_subdirectory(src/lwm2m)
add_subdirectory(src/ui)
>>>>>>> 309af76... Application is now based on LWM2M-client sample from NCS v1.5.0
