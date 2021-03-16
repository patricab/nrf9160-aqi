#define DT_DRV_COMPAT spec_sensors_dgso3

/* sensor dgso3.c - Driver for spec sensor DGS-O3 968-042 sensor
 * DGS-O3 product: https://www.digikey.no/product-detail/en/spec-sensors-llc/968-042/1684-1046-ND/7689227
 * DGS-O3 spec: https://www.spec-sensors.com/wp-content/uploads/2017/01/DGS-O3-968-042_9-6-17.pdf
 */

#include <drivers/sensor.h>
#include <drivers/uart.h>
#include <logging/log.h>
#include <kernel.h>
#include <init.h>

LOG_MODULE_REGISTER(DGSO3, CONFIG_SENSOR_LOG_LEVEL);

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
