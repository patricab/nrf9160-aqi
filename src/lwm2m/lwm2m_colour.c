/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr.h>
#include <drivers/sensor.h>
#include <net/lwm2m.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_lwm2m_colour, CONFIG_APP_LOG_LEVEL);

/* use 25.5C if no sensor available */
static struct float32_value colour_float = { 25, 500000 };
static const struct device *die_dev;
static int32_t timestamp;

#if defined(CONFIG_COLOUR_NRF5_NAME)
static int read_colour(const struct device *colour_dev,
			    struct float32_value *float_val)
{
	const char *name = colour_dev->config->name;
	struct sensor_value colour_val;
	int ret;

	ret = sensor_sample_fetch(colour_dev);
	if (ret) {
		LOG_ERR("%s: I/O error: %d", name, ret);
		return ret;
	}

	ret = sensor_channel_get(colour_dev, SENSOR_CHAN_DIE_COLOUR, &colour_val);
	if (ret) {
		LOG_ERR("%s: can't get data: %d", name, ret);
		return ret;
	}

	LOG_DBG("%s: read %d.%d C", name, colour_val.val1, colour_val.val2);
	float_val->val1 = colour_val.val1;
	float_val->val2 = colour_val.val2;

	return 0;
}
#endif

static void *colour_read_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
			  size_t *data_len)
{
	int32_t ts;

	/* Only object instance 0 is currently used */
	if (obj_inst_id != 0) {
		*data_len = 0;
		return NULL;
	}

#if defined(CONFIG_COLOUR_NRF5_NAME)
	/*
	 * No need to check if read was successful, just reuse the
	 * previous value which is already stored at colour_float.
	 * This is because there is currently no way to report read_cb
	 * failures to the LWM2M engine.
	 */
	read_colour(die_dev, &colour_float);
#endif
	lwm2m_engine_set_float32("3303/0/5706", &colour_float);
	*data_len = sizeof(colour_float);
	/* get current time from device */
	lwm2m_engine_get_s32("3/0/13", &ts);
	/* set timestamp */
	lwm2m_engine_set_s32("3303/0/5518", ts);

	return &colour_float;
}

int lwm2m_init_colour(void)
{
#if defined(CONFIG_COLOUR_NRF5_NAME)
	die_dev = device_get_binding(CONFIG_COLOUR_NRF5_NAME);
	LOG_INF("%s on-die colour sensor %s",
		die_dev ? "Found" : "Did not find", CONFIG_COLOUR_NRF5_NAME);
#endif

	if (!die_dev) {
		LOG_ERR("No colour device found.");
	}

	lwm2m_engine_create_obj_inst("3303/0");
	lwm2m_engine_register_read_callback("3303/0/5700", colour_read_cb);
	lwm2m_engine_set_res_data("3303/0/5518",
				  &timestamp, sizeof(timestamp), 0);
	return 0;
}

/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <stdio.h>
#include <sys/__assert.h>

#define THRESHOLD_UPPER                 50
#define THRESHOLD_LOWER                 0
#define TRIGGER_ON_DATA_READY           0

static K_SEM_DEFINE(sem, 0, 1);

static void trigger_handler(const struct device *dev,
			    struct sensor_trigger *trigger)
{
	ARG_UNUSED(dev);
	switch (trigger->type) {
	case SENSOR_TRIG_THRESHOLD:
		printk("Threshold trigger\r\n");
		break;
	case SENSOR_TRIG_DATA_READY:
		printk("Data ready trigger\r\n");
		break;
	default:
		printk("Unknown trigger event %d\r\n", trigger->type);
		break;
	}
	k_sem_give(&sem);
}

static int bh1479_set_attribute(const struct device *dev,
				enum sensor_channel chan,
				enum sensor_attribute attr, int value)
{
	int ret;
	struct sensor_value sensor_val;

	sensor_val.val1 = (value);

	ret = sensor_attr_set(dev, chan, attr, &sensor_val);
	if (ret) {
		printk("sensor_attr_set failed ret %d\n", ret);
	}

	return ret;
}

// This sets up  the sensor to signal valid data when a threshold
 // value is reached.
static void process(const struct device *dev)
{
	int ret;
	struct sensor_value temp_val;
	struct sensor_trigger sensor_trig_conf = {
		#if (TRIGGER_ON_DATA_READY)
		.type = SENSOR_TRIG_DATA_READY,
		#else
		.type = SENSOR_TRIG_THRESHOLD,
		#endif
		.chan = SENSOR_CHAN_RED,
	};

	if (IS_ENABLED(CONFIG_BH1749_TRIGGER)) {
		bh1479_set_attribute(dev, SENSOR_CHAN_ALL,
				     SENSOR_ATTR_LOWER_THRESH,
				     THRESHOLD_LOWER);
		bh1479_set_attribute(dev, SENSOR_CHAN_ALL,
				     SENSOR_ATTR_UPPER_THRESH,
				     THRESHOLD_UPPER);

		if (sensor_trigger_set(dev, &sensor_trig_conf,
				trigger_handler)) {
			printk("Could not set trigger\n");
			return;
		}
	}

	while (1) {
		if (IS_ENABLED(CONFIG_BH1749_TRIGGER)) {
			// Waiting for a trigger event 
			k_sem_take(&sem, K_FOREVER);
		}
		ret = sensor_sample_fetch_chan(dev, SENSOR_CHAN_ALL);
		//The sensor does only support fetching SENSOR_CHAN_ALL 
		if (ret) {
			printk("sensor_sample_fetch failed ret %d\n", ret);
			return;
		}

		ret = sensor_channel_get(dev, SENSOR_CHAN_RED, &temp_val);
		if (ret) {
			printk("sensor_channel_get failed ret %d\n", ret);
			return;
		}
		printk("BH1749 RED: %d\n", temp_val.val1);

		ret = sensor_channel_get(dev, SENSOR_CHAN_GREEN, &temp_val);
		if (ret) {
			printk("sensor_channel_get failed ret %d\n", ret);
			return;
		}
		printk("BH1749 GREEN: %d\n", temp_val.val1);

		ret = sensor_channel_get(dev, SENSOR_CHAN_BLUE, &temp_val);
		if (ret) {
			printk("sensor_channel_get failed ret %d\n", ret);
			return;
		}
		printk("BH1749 BLUE: %d\n", temp_val.val1);

		ret = sensor_channel_get(dev, SENSOR_CHAN_IR, &temp_val);
		if (ret) {
			printk("sensor_channel_get failed ret %d\n", ret);
			return;
		}
		printk("BH1749 IR: %d\n", temp_val.val1);
		k_sleep(K_MSEC(2000));
	}
}

//void main(void)
int lwm2m_init_colour(void)
{
	const struct device *dev;

	if (IS_ENABLED(CONFIG_LOG_BACKEND_RTT)) {
		//Give RTT log time to be flushed before executing tests 
		k_sleep(K_MSEC(500));
	}
	dev = device_get_binding("BH1749");
	if (dev == NULL) {
		printk("Failed to get device binding\n");
		return;
	}
	printk("device is %p, name is %s\n", dev, dev->name);

	lwm2m_engine_create_obj_inst("3335/0");
	lwm2m_engine_register_read_callback("3335/0/5706", string_values);
	lwm2m_engine_set_res_data("3335/0/5518",
				  &timestamp, sizeof(timestamp), 0);
	process(dev);
}
