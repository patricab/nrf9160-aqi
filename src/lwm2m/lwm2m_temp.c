/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr.h>
#include <drivers/sensor.h>
#include <net/lwm2m.h>
#include <device.h>
#include <stdio.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_lwm2m_temp, CONFIG_APP_LOG_LEVEL);

/* use 25.5C if no sensor available */
static struct float32_value temp_float = { 25, 500000 };
static const struct device *die_dev;
/* const struct device  */

static int32_t timestamp;

static int read_temperature(const struct device *temp_dev,
			    struct float32_value *float_val)
{
	/* const char *name = temp_dev->config->name; */
	struct sensor_value temp;
	int ret;

	ret = sensor_sample_fetch(temp_dev);
	if (ret) {
		LOG_ERR("BME680: I/O error: %d", ret);
		return ret;
	}

	ret = sensor_channel_get(temp_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
	if (ret) {
		LOG_ERR("BME680: can't get data: %d", ret);
		return ret;
	}

	/* LOG_DBG("%s: read %d.%d C", name, temp.val1, temp.val2); */
	float_val->val1 = temp.val1;
	float_val->val2 = temp.val2;

	return 0;
}

static void *temp_read_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
			  size_t *data_len)
{
	int32_t ts;

	/* Only object instance 0 is currently used */
	if (obj_inst_id != 0) {
		*data_len = 0;
		return NULL;
	}

	read_temperature(die_dev, &temp_float);
	lwm2m_engine_set_float32("3303/0/5700", &temp_float);
	*data_len = sizeof(temp_float);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	/* set timestamp */
	lwm2m_engine_set_s32("3303/0/5518", ts);

	return &temp_float;
}

int lwm2m_init_temp(void)
{
/* #if defined(CONFIG_TEMP_NRF5_NAME) */
/* 	die_dev = device_get_binding(CONFIG_TEMP_NRF5_NAME); */
/* 	LOG_INF("%s on-die temperature sensor %s", */
/* 		die_dev ? "Found" : "Did not find", CONFIG_TEMP_NRF5_NAME); */
/* #endif */

   die_dev = device_get_binding(DT_LABEL(DT_INST(0, bosch_bme680)));
	if (!die_dev) {
		LOG_ERR("No temperature device found.");
	}

	lwm2m_engine_create_obj_inst("3303/0");
	lwm2m_engine_register_read_callback("3303/0/5700", temp_read_cb);
	lwm2m_engine_set_res_data("3303/0/5518",
				  &timestamp, sizeof(timestamp), 0);
	return 0;
}
