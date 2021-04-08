#include "dgso3.h"

/* Variables */
static uint8_t *rx_buf = {NULL};
static const struct device *dev;
static volatile bool data_received;

/* Function declarations */
// static void uart_rx(struct k_timer *delay_timer);

/* Inital configuration */
// K_TIMER_DEFINE(delay_timer, uart_rx, NULL);
LOG_MODULE_REGISTER(app_uart, CONFIG_APP_LOG_LEVEL);


/* UART recieve (RX) callback */
static void uart_cb(const struct device *dev, void *data) {
   // Check if UART RX interrupt is ready
 	if (uart_irq_rx_ready(dev)) {
      // Read from FIFO queue
		uart_fifo_read(dev, rx_buf, sizeof(rx_buf));

      // End RX at at EOT (newline)
		if (*rx_buf == '\n') {
			data_received = true;
		}
  }
}

/* Delay function */
// static void delay(k_timeout_t t) {
//    k_timer_start(&delay_timer, t, K_NO_WAIT);
// }

/* UART recieve (RX) */
// static void uart_rx(struct k_timer *timer) {
static void uart_rx() {
   uart_irq_rx_enable(dev);
   data_received = false;
   while (data_received == false) {
   }
   uart_irq_rx_disable(dev);
}

/**
   @brief Read sensor gas measurement (single measurement)

   @param rx_val User RX value

   @retval 0 if successful, 1 if errors occured
*/
int read_gas(int16_t *rx_val) {
   /* Send command */
   printk("\r");

   /* Listen for data */
   // uart_rx(&delay_timer);
   uart_rx();
   if (!rx_buf[0]) { // Check for empty buffer
      LOG_ERR("Error: device RX timed out");
      return 1;
   }

   // // Output rx buffer
   sscanf(rx_buf, "%d", (int *)&rx_val);
   return 0;
}

/**
   @brief Set sensor to standby low power mode

   @retval none
*/
void standby_gas(void) {
   /* Send command */
   printk("s");
}

/**
   @brief Calibrate sensor zero value

   @retval none
*/
void zero_gas(void) {
   /* Send command */
   printk("Z");
}

/**
   @brief Set specific sensor zero value

   @param val User zero value

   @retval none
 */
void set_gas(uint8_t val) {
   /* Send command */
   printk("S");

   /* Send value */
   // delay(K_MSEC(10));
   printk(&val);

   /* Send return character */
   // delay(K_MSEC(10));
   printk("\r");
}

/**
   @brief Initialize UART device

   @param die_dev User device

   @retval 0 if successful, 1 if an error occured
*/
int init_uart(const struct device *die_dev) {
   /* Definitions */
   uint32_t baud = 9600;
   enum uart_config_parity parity = UART_CFG_PARITY_NONE;
   enum uart_config_stop_bits stop = UART_CFG_STOP_BITS_1;
   enum uart_config_data_bits data = UART_CFG_DATA_BITS_8;
   enum uart_config_flow_control flow = UART_CFG_FLOW_CTRL_NONE;
   const struct uart_config conf = {baud, parity, stop, data, flow};
   dev = die_dev;

   /* Config function calls */
   int err = uart_configure(dev, &conf);
   if (err != 0) {
      LOG_ERR("Error: could not configure UART device");
      return 1;
   }

   /* Configure UART callback */
   uart_irq_callback_set(dev, uart_cb);
   return 0;
}