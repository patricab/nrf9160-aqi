#include <net/lwm2m.h>
#include <drivers/sensor.h>
#include <stdio.h>
#include <device.h>

#include "sps30_I2C.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(app_lwm2m_pms, CONFIG_APP_LOG_LEVEL);

static const struct device *die_dev;
static struct float32_value pms_float[3];

static int32_t timestamp;

// Convert float to float32 structure values
static void float_struct(float num, struct float32_value *strct) {
	int intpart = (int)num;
	int decpart = num - intpart;

	float_val->val1 = intpart;
	float_val->val2 = decpart;
}

static int read_pms(const struct device *die_dev, struct float32_value *float_val){

	int ret = sps30_particle_read(die_dev);
	if (ret)
	{
		LOG_ERR("Error: can't read data");
		return 1;
	}
	
	float_struct(sps30.nc_2p5, &pms_float[0]);
	float_struct(sps30.nc_10p0, &pms_float[1]);
	float_struct(sps30.typ_size, &pms_float[2]);

	return 0;
}

// Callback function
static void *pms_read_cb((uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
	size_t *data_len){

	int32_t ts;

	/* Only object instance 0 is currently used */
	if (obj_inst_id != 0) {
			*data_len = 0;
			return NULL;
		}

	read_pms(die_dev, &pms_float);
	lwm2m_engine_set_float32("3303/0/5529", &pms_float[0]);
	lwm2m_engine_set_float32("3303/1/5529", &pms_float[1]);
	lwm2m_engine_set_float32("3303/2/5529", &pms_float[2]);
	*data_len = sizeof(pms_float);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	/* set timestamp */
	lwm2m_engine_set_s32("3303/0/5518", ts);
	lwm2m_engine_set_s32("3303/1/5518", ts);
	lwm2m_engine_set_s32("3303/2/5518", ts);

	return &drv_data;
}
// lwm2m init function
int lwm2m_init_pms(void)
{
   	die_dev = device_get_binding(DT_LABEL(DT_INST(0, pms_dev)));
	if (!die_dev) {
		LOG_ERR("No data device found.");
	}

	int ret = sps30_init(die_dev);
	if (ret)
	{
		LOG_ERR("Error: Could not initalize SPS30")
		return 1;
	}

	lwm2m_engine_create_obj_inst("3303/0/5529");
	lwm2m_engine_create_obj_inst("3303/1/5529");
	lwm2m_engine_create_obj_inst("3303/2/5529");

	lwm2m_engine_register_read_callback("3303/0/5529", pms_read_cb);
	lwm2m_engine_register_read_callback("3303/1/5529", pms_read_cb);
	lwm2m_engine_register_read_callback("3303/2/5529", pms_read_cb);
	lwm2m_engine_set_res_data("3303/0/5518",
				  &timestamp, sizeof(timestamp), 0);
	lwm2m_engine_set_res_data("3303/1/5518",
				  &timestamp, sizeof(timestamp), 0);
	lwm2m_engine_set_res_data("3303/2/5518",
				  &timestamp, sizeof(timestamp), 0);
	return 0;
}



