#define DT_DRV_COMPAT sensirion_sps30

/* sensor sps30.c - Driver for sensirion SPS30 sensor
 * SPS30 product: https://no.mouser.com/ProductDetail/Sensirion/SPS30/?qs=lc2O%252BfHJPVbEPY0RBeZmPA==
 * SPS30 spec: https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/9.6_Particulate_Matter/Datasheets/Sensirion_PM_Sensors_Datasheet_SPS30.pdf
 */

#include <drivers/sensor.h>
#include <drivers/i2c.h>
#include <drivers/uart.h>
#include <logging/log.h>
#include <kernel.h>
#include <init.h>

LOG_MODULE_REGISTER(SPS30, CONFIG_SENSOR_LOG_LEVEL);

#define CFG_SPS30_SERIAL_TIMEOUT 1000

struct sps30_data {
	const struct device *i2c_dev;
	uint16_t pm_1_0;
	uint16_t pm_2_5;
	uint16_t pm_10;
};

/**
 * @brief wait for an array data from uart device with a timeout
 *
 * @param dev the uart device
 * @param data the data array to be matched
 * @param len the data array len
 * @param timeout the timeout in milliseconds
 * @return 0 if success; -ETIME if timeout
 */
static int uart_wait_for(const struct device *dev, uint8_t *data, int len,
			 int timeout)
{
	int matched_size = 0;
	int64_t timeout_time = k_uptime_get() + K_MSEC(timeout);

	while (1) {
		uint8_t c;

		if (k_uptime_get() > timeout_time) {
			return -ETIME;
		}

		if (uart_poll_in(dev, &c) == 0) {
			if (c == data[matched_size]) {
				matched_size++;

				if (matched_size == len) {
					break;
				}
			} else if (c == data[0]) {
				matched_size = 1;
			} else {
				matched_size = 0;
			}
		}
	}
	return 0;
}

/**
 * @brief read bytes from uart
 *
 * @param data the data buffer
 * @param len the data len
 * @param timeout the timeout in milliseconds
 * @return 0 if success; -ETIME if timeout
 */
static int uart_read_bytes(const struct device *dev, uint8_t *data, int len,
			   int timeout)
{
	int read_size = 0;
	int64_t timeout_time = k_uptime_get() + K_MSEC(timeout);

	while (1) {
		uint8_t c;

		if (k_uptime_get() > timeout_time) {
			return -ETIME;
		}

		if (uart_poll_in(dev, &c) == 0) {
			data[read_size++] = c;
			if (read_size == len) {
				break;
			}
		}
	}
	return 0;
}

static int sps30_sample_fetch(comnst struct device *dev,
				enum sensor_channel chan)
{
	struct sps30_data *drv_data = dev->data;

	uint8_t sps30_start_bytes[] = {0x7e, 0x00};
	uint8_t sps30_receive_buffer[40];

	if (uart_wait_for(drv_data->uart_dev, sps30_start_bytes,
		sizeof(sps30_start_bytes),
		CFG_SPS30_SERIAL_TIMEOUT) < 0) {
		LOG_WRN("waiting for start bytes is timeout");
		return -ETIME;
	}

	drv_data->pm_1_0 = 
		(sps30_receive_buffer[0] << 8) + sps30_receive_buffer[1];
	drv_data->pm_2_5 = 
		(sps30_receive_buffer[4] << 8) + sps30_receive_buffer[5];
	drv_data->pm_10 = 
		(sps30_receive_buffer[12] << 8) + sps30_receive_buffer[13];

	LOG_DBG("pm1.0 = %d", drv_data->pm_1_0);
	LOG_DBG("pm2.5 = %d", drv_data->pm_2_5);
	LOG_DBG("pm10 = %d", drv_data->pm_10);
	return 0;
}

static int sps30_channel_get(const struct device *dev,
		          enum sensor_channel chan,
		          struct sensor_value *val)
{
	struct sps30_data *drv_data = dev->data;

	if (chan == SENSOR_CHAN_PM_1_0) {
		val->val1 = drv_data->pm_1_0;
		val->val2 = 0;
	} else if (chan == SENSOR_CHAN_PM_2_5) {
		val->val1 = drv_data->pm_2_5;
		val->val2 = 0;
	} else if (chan == SENSOR_CHAN_PM_10) {
		val->val1 = drv_data->pm_10;
		val->val2 = 0;
	} else {
		return -EINVAL;
	}
	return 0;
}

static const struct sensor_driver_api sps30_api = {
	.sample_fetch = &sps30_sample_fetch,
	.channel_get = &sps30_channel_get,
};

static int sps30_init(const struct device *dev)
{
	struct sps30_data *drv_data = dev->data;

	drv_data->uart_dev = device_get_binding(DT_INST_BUS_LABEL(0));

	if (!drv_data->uart_dev) {
		LOG_DBG("uart device is not found: %s",
			DT_INST_BUS_LABEL(0));
		return -EINVAL;
	}
	return 0;
}

static struct sps30_data sps30_data;

DEVICE_DT_INST_DEFINE(0, &sps30_init, device_pm_control_nop,
	&sps30_data, NULL, POST_KERNEL,
	CONFIG_SENSOR_INIT_PRIORITY, &sps30_api);
