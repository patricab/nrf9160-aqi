<<<<<<< HEAD
zephyr_include_directories(include)

=======
>>>>>>> feature/pms
target_sources_ifdef(CONFIG_LWM2M_IPSO_TEMP_SENSOR
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ipso_temp_sensor.c
	)
target_sources_ifdef(CONFIG_LWM2M_IPSO_CONC_SENSOR
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ipso_conc_sensor.c
	)
target_sources_ifdef(CONFIG_LWM2M_IPSO_COLOUR_SENSOR
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ipso_colour_sensor.c
	)
target_sources_ifdef(CONFIG_LWM2M_OPENLX_SP_SENSOR
	app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/openlx_sp_prtcl_sensor.c
	)
