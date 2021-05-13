#include <net/lwm2m.h>
#include <drivers/sensor.h>
#include <stdio.h>
#include <device.h>

#include "sps30_I2C.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(app_lwm2m_pms, CONFIG_APP_LOG_LEVEL);

static const struct device *die_dev;
static struct float32_value pms_float[3];
struct sps30_data sps30;

static int32_t timestamp;

static int read_pms(const struct device *die_dev){

	int ret = sps30_particle_read(die_dev);
	if (ret)
	{
		LOG_ERR("Error: can't read data");
		return 1;
	}
	
	pms_float[0].val1 = sps30.nc_2p5;
	pms_float[0].val2 = 0;
	pms_float[1].val1 = sps30.nc_10p0;
	pms_float[1].val2 = 0;
	pms_float[2].val1 = sps30.typ_size;
	pms_float[2].val2 = 0;

	return 0;
}

//Callback function
static void *pms0_read_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
	size_t *data_len){

	int32_t ts;

	read_pms(die_dev);
	lwm2m_engine_set_float32("3300/2/5700", &pms_float[0]);
	*data_len = sizeof(pms_float[0]);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	/* set timestamp */
	lwm2m_engine_set_s32("3300/2/5518", ts);

	return &pms_float[0];
}

static void *pms1_read_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
	size_t *data_len){

	int32_t ts;

	lwm2m_engine_set_float32("3300/3/5700", &pms_float[1]);
	*data_len = sizeof(pms_float[1]);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	/* set timestamp */
	lwm2m_engine_set_s32("3300/3/5518", ts);

	return &pms_float[1];
}

static void *pms2_read_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
	size_t *data_len){

	int32_t ts;

	lwm2m_engine_set_float32("3300/4/5700", &pms_float[2]);
	*data_len = sizeof(pms_float[2]);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	/* set timestamp */
	lwm2m_engine_set_s32("3300/4/5518", ts);

	return &pms_float[2];
}

// lwm2m init function
int lwm2m_init_pms(void)
{
	LOG_DBG("init_pms function");

	die_dev = device_get_binding(DT_LABEL(DT_NODELABEL(i2c2)));
	if (!die_dev) {
		LOG_ERR("No data device found.");
	}

	int ret = sps30_init(die_dev, &sps30);
	if (ret)
	{
		LOG_ERR("Error: Could not initalize SPS30");
		return 1;
	}

	lwm2m_engine_create_obj_inst("3300/2");
	lwm2m_engine_create_obj_inst("3300/3");
	lwm2m_engine_create_obj_inst("3300/4");

	lwm2m_engine_register_read_callback("3300/2/5700", pms0_read_cb);
	lwm2m_engine_register_read_callback("3300/3/5700", pms1_read_cb);
	lwm2m_engine_register_read_callback("3300/4/5700", pms2_read_cb);
	lwm2m_engine_set_res_data("3300/2/5518",
				  &timestamp, sizeof(timestamp), 0);
	lwm2m_engine_set_res_data("3300/3/5518",
				  &timestamp, sizeof(timestamp), 0);
	lwm2m_engine_set_res_data("3300/4/5518",
				  &timestamp, sizeof(timestamp), 0);
	return 0;
}



