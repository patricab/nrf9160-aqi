#include "dgso3.h"

/* Variables */
static const uint8_t *tx_buf;
static uint8_t *rx_buf = {NULL};
static const struct device *dev;
struct k_thread uart_data;

/* Inital configuration */
K_THREAD_STACK_DEFINE(uart_stack, STACKSIZE);
K_TIMER_DEFINE(delay_timer, uart_rx, NULL);
LOG_MODULE_REGISTER(app_uart, CONFIG_APP_LOG_LEVEL);


/* UART transmit (TX) callback */
void uart_tx_cb() {
   int err = uart_tx(dev, tx_buf, BUFF_SIZE, SYS_FOREVER_MS);
   if (err != 0) {
      LOG_ERR("Error: device TX timed out");
   }
}

/* UART transmit (TX) */
static void tx_uart(const uint8_t *buf) {
   /* Fill TX buffer */
   tx_buf = buf;

   /* Create TX thread */
   k_tid_t uart_tid = k_thread_create(&uart_data, uart_stack,
                                    K_THREAD_STACK_SIZEOF(uart_stack),
                                    uart_tx_cb,
                                    NULL, NULL, NULL,
                                    -1, 0, K_NO_WAIT);
   (void)uart_tid;
}

/* UART recieve (RX) callback */
static void uart_cb(const struct device *dev) {
   int data_length = 0;
	uart_irq_update(dev);

	if (uart_irq_rx_ready(dev)) {
		rx_buf[data_length] = 0;
		data_length = uart_fifo_read(dev, rx_buf, sizeof(rx_buf));
	}

   /* Clear remaining buffer at EOT (newline) */
   for (int i = 0; i < sizeof(rx_buf); i++) {
      if (rx_buf[i] == '\n') {
         for (int k = i+1; k < sizeof(rx_buf); k++) {
            rx_buf[k] = 0;
         }
         break;
      }
   }
}

/* Delay function */
static void delay(k_timeout_t t) {
   k_timer_start(&delay_timer, t, K_NO_WAIT);
}

/* UART recieve (RX) */
static void uart_rx(struct k_timer *timer) {
   uart_irq_rx_enable(dev);
   delay(K_SECONDS(2));
   uart_irq_rx_disable(dev);
}

/**
   @brief Read sensor gas measurement (single measurement)

   @param rx_val User RX value

   @retval 0 if successful, 1 if errors occured
*/
int read_gas(int16_t *rx_val) {
   unsigned char c = 0x0D; /* '\r' */
   tx_buf = &c;

   /* Send command */
   tx_uart(tx_buf);

   /* Listen for data */
   uart_rx(&delay_timer);
   if (!rx_buf[0]) { // Check for empty buffer
      LOG_ERR("Error: device RX timed out");
      return 1;
   }

   // Output rx buffer
   sscanf(&rx_buf, "%d", &rx_val);
   /* rx_val = rx_buf; */
   return 0;
}

/**
   @brief Set sensor to standby low power mode

   @retval none
*/
void standby_gas(void) {
   char c = 's';
   tx_buf = &c;

   /* Send command */
   tx_uart(tx_buf);
}

/**
   @brief Calibrate sensor zero value

   @retval none
*/
void zero_gas(void) {
   char c = 'Z';
   tx_buf = &c;

   /* Send command */
   tx_uart(tx_buf);
}

/**
   @brief Set specific sensor zero value

   @param val User zero value

   @retval none
 */
void set_gas(uint8_t val) {
   /* Send command */
   char c = 'S';
   tx_buf = &c;

   tx_uart(tx_buf);

   /* Send value */
   tx_buf = &val;

   delay(K_MSEC(10));
   tx_uart(tx_buf);

   /* Send return character */
   unsigned char d = 0x0D; /* '\r' */
   tx_buf = &d;

   delay(K_MSEC(10));
   tx_uart(tx_buf);
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
   const struct uart_config *conf = {baud, parity, stop, data, flow};
   dev = die_dev;

   /* Config function calls */
   int err = uart_configure(dev, conf);
   if (err != 0) {
      LOG_ERR("Error: could not configure UART device");
      return 1;
   }

   /* Configure UART callback */
   uart_irq_callback_set(dev, uart_cb);
   return 0;
}
