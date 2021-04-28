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
LOG_MODULE_REGISTER(app_lwm2m_gas_res, CONFIG_APP_LOG_LEVEL);

/* use 25.5C if no sensor available */
static struct float32_value gas_res_float = { 25, 500000 };
static const struct device *die_dev;

static int32_t timestamp;

static int read_gas_resistance(const struct device *gas_res_dev,
			    struct float32_value *float_val)
{
	struct sensor_value gas_res;
	int ret;

	ret = sensor_sample_fetch(gas_res_dev);
	if (ret) {
		LOG_ERR("BME680: I/O error: %d", ret);
		return ret;
	}

	ret = sensor_channel_get(gas_res_dev, SENSOR_CHAN_GAS_RES, &gas_res);
	if (ret) {
		LOG_ERR("BME680: can't get data: %d", ret);
		return ret;
	}

	float_val->val1 = gas_res.val1;
	float_val->val2 = gas_res.val2;

	return 0;
}

static void *gas_res_read_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
			  size_t *data_len)
{
	int32_t ts;

	/* Only object instance 0 is currently used */
	// if (obj_inst_id != 0) {
	// 	*data_len = 0;
	// 	return NULL;
	// }

	read_gas_resistance(die_dev, &gas_res_float);
	lwm2m_engine_set_float32("3300/0/5700", &gas_res_float);
	*data_len = sizeof(gas_res_float);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	/* set timestamp */
	lwm2m_engine_set_s32("3300/0/5518", ts);

	return &gas_res_float;
}

int lwm2m_init_gas_res(void)
{
   die_dev = device_get_binding(DT_LABEL(DT_INST(0, bosch_bme680)));
	if (!die_dev) {
		LOG_ERR("No device found.");
	}

	lwm2m_engine_create_obj_inst("3300/0");
	lwm2m_engine_register_read_callback("3300/0/5700", gas_res_read_cb);
	lwm2m_engine_set_res_data("3300/0/5518",
				  &timestamp, sizeof(timestamp), 0);
	return 0;
}
