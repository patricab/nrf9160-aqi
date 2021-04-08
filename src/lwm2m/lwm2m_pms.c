#include <net/lwm2m.h>
#include <drivers/sensor.h>
#include <logging/log.h>
#include <stdio.h>
#include <device.h>

#include "sps30_I2C.h"


struct sps30_data {
	const struct device *dev;
    uint16_t nc_2p5;
    uint16_t nc_10p0;
    uint16_t typ_siz;
};


static const struct device *die_dev;

static int32_t timestamp;

static int read_pms(const struct device *die_dev,  ){

	int ret;

	ret = sps30_particle_read()
	if (ret < 0)
	{
		LOG_ERR("Error: can't read data");
		return 1;
	}
	
	return 0;
}

// Callback function
static void *sps30_read_cb((uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
	size_t *data_len){

	int32_t ts;

	/* Only object instance 0 is currently used */
	if (obj_inst_id != 0) {
			*data_len = 0;
			return NULL;
		}

	read_pms(die_dev, &drv_data);
	lwm2m_engine_set_float32("3303/0/5529", &drv_data);
	*data_len = sizeof(drv_data);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	/* set timestamp */
	lwm2m_engine_set_s32("3303/0/5518", ts);

	return &drv_data;
}
// lwm2m init function
int lwm2m_init_pms(void){

	int ret;

   	die_dev = device_get_binding(DT_LABEL(DT_INST(0, pms_dev)));
	if (!die_dev) {
		LOG_ERR("No data device found.");
	}

	ret=sps30_init(die_dev);

	if (ret < 0)
	{
		LOG_ERR("Cannot initialise sps30")
	return 1;
	}

	lwm2m_engine_create_obj_inst("3303/0/5529");
	lwm2m_engine_register_read_callback("3303/0/5529", pms_read_cb);
	lwm2m_engine_set_res_data("3303/0/5518",
				  &timestamp, sizeof(timestamp), 0);
	return 0;

}



