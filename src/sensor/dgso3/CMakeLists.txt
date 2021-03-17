if(CONFIG_LWM2M_IPSO_CONC_SENSOR)
  zephyr_library()
  zephyr_library_sources_ifdef(dgso3.c)
endif()