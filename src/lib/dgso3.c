#include <init.h>
#include <zephyr.h>
#include <device.h>
#include <kernel.h>
#include <sys/printk.h>
#include <drivers/uart.h>
static const struct device *dev;
static const uint8_t *tx_buf;
static const uint8_t *rx_buf;
size_t len = 8;

/**
 * @brief wait for an array data from uart device with a timeout
 *
 * @param dev UART device structure.
 * @param data the data array to be matched
 * @param len the data array len
 * @param timeout the timeout in milliseconds
 * @return 0 if success; -ETIME if timeout
 */
static int uart_wait_for(struct device *dev, u8_t *data, int len, int timeout)
{
	int matched_size = 0;
	s64_t timeout_time = k_uptime_get() + K_MSEC(timeout);

	while (1) {
		u8_t c;

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
   @brief Read sensor gas measurement

   @retval 0 if successful, 1 if errors occured
*/
int read_gas(void) {
   unsigned char c = 0x0D; /* '\r' */
   *tx_buf = (int)(c);

   int err = uart_tx(dev, &tx_buf, len, SYS_FOREVER_MS);
   if (err == 0) {
      printk("Error: device TX timed out");
      return 1;
   }

   err = uart_wait_for(dev, &rx_buf, len, 300);
   if (err == 0) {
      printk("Error: device RX timed out");
      return 1;
   }
   return 0;
}

/**
   @brief Set sensor to standby low power mode

   @retval 0 if successful, 1 if errors occured
*/
void standby_gas(void) {
   *tx_buf = (int)('S');

   int err = uart_tx(dev, &tx_buf, len, SYS_FOREVER_MS);
   if (err == 0) {
      printk("Error: device TX timed out");
      return 1;
   }
   return 0;
}

/**
   @brief Calibrate sensor zero value

   @retval 0 if successful, 1 if errors occured
*/
int zero_gas(void) {
   *tx_buf = (int)('Z');

   int err = uart_tx(dev, &tx_buf, len, SYS_FOREVER_MS);
   if (err == 0) {
      printk("Error: device TX timed out");
      return 1;
   }
   return 0;
}

/**
 * @brief  Set specific sensor zero value

   @param val User zero value

   @retval 0 if successful, 1 if errors occured
 */
int set_gas(uint8_t val) {

}

/**
   @brief Initialize UART device

   @retval 0 if successful, 1 if an error occured
*/
void init_uart(void) {
   /* Definitions */
   enum uart_config_parity parity = UART_CFG_PARITY_NONE;
   enum uart_config_stop_bits stop = UART_CFG_STOP_BITS_1;
   enum uart_config_data_bits data = UART_CFG_DATA_BITS_8;
   enum uart_config_flow_control flow = UART_CFG_FLOW_CTRL_NONE;
   struct uart_config *conf = {9600, parity, data, flow};

   /* Function calls */
   /* dev = device_get_binding("dgso3"); */
   int err = uart_configure(dev, &conf);
   if (err == 0) {
      printk("Error: could not configure uart_device")
      return 1;
   }
   return 0;
}
