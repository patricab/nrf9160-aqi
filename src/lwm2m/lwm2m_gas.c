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

#include "dgso3.h"

LOG_MODULE_REGISTER(app_lwm2m_gas, CONFIG_APP_LOG_LEVEL);

/* use 0 PPB if no sensor available */
static struct float32_value gas_float;
static const struct device *die_dev;

static int32_t timestamp;

static int read_val(const struct device *temp_dev,
			    struct float32_value *float_val)
{
	int32_t *val = 0;
	int err = read_gas(&val);
	if (err)
	{
		LOG_ERR("Error: can't get data");
		// return 1;
	}

	// standby_gas(); // Set sensor to low power mode
	float_val->val1 = val;
	float_val->val2 = 0;
	return 0;
}

static void *gas_read_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
						 size_t *data_len)
{
	int32_t ts;

	/* Only object instance 0 is currently used */
	// if (obj_inst_id != 0)
	// {
	// {
	// 	*data_len = 0;
	// 	return NULL;
	// }

	read_val(die_dev, &gas_float);
	lwm2m_engine_set_float32("3300/1/5700", &gas_float);
	*data_len = sizeof(gas_float);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	// set timestamp 
	lwm2m_engine_set_s32("3300/1/5518", ts);

	return &gas_float;
}

int lwm2m_init_gas(void)
{
	die_dev = device_get_binding(DT_LABEL(DT_NODELABEL(uart1)));
	if (!die_dev)
	{
		LOG_ERR("No device found.");
	}

	int err = init_uart(die_dev);
	if (err)
	{
		LOG_ERR("I/O error");
	}

	lwm2m_engine_create_obj_inst("3300/1");
	lwm2m_engine_register_read_callback("3300/1/5700", gas_read_cb);
	lwm2m_engine_set_res_data("3300/1/5518",
							  &timestamp, sizeof(timestamp), 0);
	return 0;
}
