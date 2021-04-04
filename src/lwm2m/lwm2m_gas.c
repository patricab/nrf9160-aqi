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

#include <logging/log.h>
LOG_MODULE_REGISTER(app_lwm2m_gas, CONFIG_APP_LOG_LEVEL);

/* use 0 PPB if no sensor available */
static int16_t *val;
static const struct device *die_dev;

static int32_t timestamp;

static int read_val(const struct device *die_dev,
					int16_t *sens_val)
{
	int err = init_uart(die_dev);
	if (err != 0)
	{
		LOG_ERR("I/O error");
		return 1;
	}

	err = read_gas(sens_val);
	/* ret = sensor_channel_get(temp_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp); */
	if (err != 0)
	{
		LOG_ERR("Error: can't get data");
		return 1;
	}

	standby_gas(); // Set sensor to low power mode
	return 0;
}

static void *gas_read_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
						 size_t *data_len)
{
	int32_t ts;

	/* Only object instance 0 is currently used */
	if (obj_inst_id != 0)
	{
		*data_len = 0;
		return NULL;
	}

	read_val(die_dev, &val);
	lwm2m_engine_set_s16("3325/0/5529", &val);
	*data_len = sizeof(val);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	/* set timestamp */
	lwm2m_engine_set_s32("3325/0/5518", ts);

	return &val;
}

int lwm2m_init_gas(void)
{
	die_dev = device_get_binding(DT_LABEL(DT_NODELABEL(uart0)));
	if (!die_dev)
	{
		LOG_ERR("No device found.");
	}

	lwm2m_engine_create_obj_inst("3325/0");
	lwm2m_engine_register_read_callback("3325/0/5529", gas_read_cb);
	lwm2m_engine_set_res_data("3325/0/5518",
							  &timestamp, sizeof(timestamp), 0);
	return 0;
}
