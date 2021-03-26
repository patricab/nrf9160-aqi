#include <net/lwm2m.h>
#include <drivers/sensor.h>
#include <logging/log.h>
#include <stdio.h>













// Callback function
static int ptcl_read_cb(const struct device *pms_dev, )



// lwm2m init function
int lwm2m_init_pms(void){

   die_dev = device_get_binding(DT_LABEL(DT_INST(0, pms_dev)));
	if (!die_dev) {
		LOG_ERR("No data device found.");
	}

	lwm2m_engine_create_obj_inst("3303/0");
	lwm2m_engine_register_read_callback("3303/0/5700", ptcl_read_cb);
	lwm2m_engine_set_res_data("3303/0/5518",
				  &timestamp, sizeof(timestamp), 0);
	return 0;

}



