#
# Copyright (c) 2019 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

zephyr_include_directories(include)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_device.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_security.c)

#target_sources_ifdef(CONFIG_LWM2M_LOCATION_OBJ_SUPPORT
#	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_location.c
#	)
target_sources_ifdef(CONFIG_LWM2M_FIRMWARE_UPDATE_OBJ_SUPPORT
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_firmware.c
	)
target_sources_ifdef(CONFIG_LWM2M_CONN_MON_OBJ_SUPPORT
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_connmon.c
	)
#target_sources_ifdef(CONFIG_LWM2M_IPSO_LIGHT_CONTROL
#	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_light_control.c
#	)
target_sources_ifdef(CONFIG_LWM2M_IPSO_TEMP_SENSOR
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_temp.c
	)
target_sources_ifdef(CONFIG_LWM2M_IPSO_HUMIDITY_SENSOR
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_humidity.c
	)
	
target_sources_ifdef(CONFIG_LWM2M_IPSO_CONC_SENSOR
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_gas_res.c
	)
target_sources_ifdef(CONFIG_GAS
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_gas.c
)
target_sources_ifdef(CONFIG_PMS
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_pms.c
)

target_sources_ifdef(CONFIG_UI_BUZZER
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_buzzer.c
	)
target_sources_ifdef(CONFIG_UI_BUTTON
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_button.c
	)
#target_sources_ifdef(CONFIG_LWM2M_IPSO_ACCELEROMETER
#	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_accelerometer.c
#	)
target_sources_ifdef(CONFIG_LWM2M_OPENLX_SP_PRTCL_SENSOR
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_particulate.c
	)
target_sources_ifdef(CONFIG_LWM2M_IPSO_COLOUR_SENSOR
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lwm2m_colour.c
	)
