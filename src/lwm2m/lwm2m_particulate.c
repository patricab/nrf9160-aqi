/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr.h>
#include <drivers/sensor.h>
#include <net/lwm2m.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_lwm2m_particulate, CONFIG_APP_LOG_LEVEL);

/* use 5.5ppm if no sensor available */
static struct float32_value prtcl_float = { 5, 500000 };
static const struct device *die_dev;
static int32_t timestamp;

#if defined(CONFIG_PRTCL_NRF5_NAME)
static int read_particulate(const struct device *prtcl_dev,
			    struct float32_value *float_val)
{
	const char *name = prtcl_dev->config->name;
	struct sensor_value prtcl_val;
	int ret;

	ret = sensor_sample_fetch(prtcl_dev);
	if (ret) {
		LOG_ERR("%s: I/O error: %d", name, ret);
		return ret;
	}

	ret = sensor_channel_get(prtcl_dev, SENSOR_CHAN_DIE_PRTCL, &prtcl_val);
	if (ret) {
		LOG_ERR("%s: can't get data: %d", name, ret);
		return ret;
	}

	LOG_DBG("%s: read %d.%d C", name, prtcl_val.val1, prtcl_val.val2);
	float_val->val1 = prtcl_val.val1;
	float_val->val2 = prtcl_val.val2;

	return 0;
}
#endif

static void *prtcl_read_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
			  size_t *data_len)
{
	int32_t ts;

	/* Only object instance 0 is currently used */
	if (obj_inst_id != 0) {
		*data_len = 0;
		return NULL;
	}

#if defined(CONFIG_PRTCL_NRF5_NAME)
	/*
	 * No need to check if read was successful, just reuse the
	 * previous value which is already stored at prtcl_float.
	 * This is because there is currently no way to report read_cb
	 * failures to the LWM2M engine.
	 */
	read_particulate(die_dev, &prtcl_float);
#endif
	lwm2m_engine_set_float32("10314/0/5700", &prtcl_float);
	*data_len = sizeof(prtcl_float);
	/* get current time from device 
	lwm2m_engine_get_s32("3/0/13", &ts);
	 set timestamp 
	lwm2m_engine_set_s32("10314/0/5518", ts);*/

	return &prtcl_float;
}

int lwm2m_init_prtcl(void)
{
#if defined(CONFIG_PRTCL_NRF5_NAME)
	die_dev = device_get_binding(CONFIG_PRTCL_NRF5_NAME);
	LOG_INF("%s on-die particulate sensor %s",
		die_dev ? "Found" : "Did not find", CONFIG_PRTCL_NRF5_NAME);
#endif

	if (!die_dev) {
		LOG_ERR("No particulate device found.");
	}

	lwm2m_engine_create_obj_inst("10314/0");
	lwm2m_engine_register_read_callback("10314/0/5700", prtcl_read_cb);
	//lwm2m_engine_set_res_data("10314/0/5518",
				  //&timestamp, sizeof(timestamp), 0);
	return 0;
}
