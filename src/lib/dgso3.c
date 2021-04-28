#include "dgso3.h"

/* Variables */
static const struct device *dev;
static volatile bool data_received;
static unsigned char rx_buf[65];
static volatile int n;

/* Inital configuration */
LOG_MODULE_REGISTER(app_uart, CONFIG_APP_LOG_LEVEL);

/* UART recieve (RX) callback */
static void uart_cb(const struct device *dev, void *data) {
   /* Definitions */
   uint8_t rx_val = 0;

   // Start processing interrupts in ISR
   uart_irq_update(dev);

   // Check if UART RX interrupt is ready
 	if (uart_irq_rx_ready(dev)) {
      // Read from FIFO queue
      uart_fifo_read(dev, &rx_val, 1);

      // End RX at at EOT (newline)
		if (rx_val == '\n') {
			data_received = true;
		} else {
         rx_buf[n] = rx_val; // Add value to buffer
         n++;
      }
   } else {
      return;
   }
}


/* UART recieve (RX) */
static void uart_rx(void) {
   /* Definitions */
   n = 0; // Zero buffer index

   uart_irq_rx_enable(dev); // Enable interrupts
   data_received = false;
   while (data_received == false) { // Wait for recieved data
   }
   uart_irq_rx_disable(dev); // Disable interrupts
}

/**
   @brief Read sensor gas measurement (single measurement)

   @param rx_val User RX value

   @retval 0 if successful, 1 if errors occured
*/
int read_gas(int32_t *rx_val) {
   /* Definitions */
   unsigned char val_buf[4] = {0};

   /* Send command */
   uart_poll_out(dev, '\r');

   /* Listen for data */
   uart_rx();

   // Find PPB values between first and second comma
   // rx_buf[13:16]
   for (int i = 0; i < 4; i++)
   {
      val_buf[i] = rx_buf[i + 13];
   }
   
   // Output rx buffer
   sscanf(val_buf, "%d", rx_val);

   return 0;
}

/**
   @brief Set sensor to standby low power mode

   @retval none
*/
void standby_gas(void) {
   /* Send command */
   uart_poll_out(dev, 's');
}

/**
   @brief Calibrate sensor zero value

   @retval none
*/
void zero_gas(void) {
   /* Send command */
   uart_poll_out(dev, 'Z');
}

/**
   @brief Set specific sensor zero value

   @param val User zero value

   @retval none
 */

void set_gas(uint8_t val) {
   /* Send command */
   uart_poll_out(dev, 'S');

   /* Send value */
   printk("%d", val);

   /* Send return character */
   uart_poll_out(dev, '\r');
}


/**
   @brief Initialize UART device

   @param die_dev User device

   @retval 0 if successful, 1 if an error occured
*/
int init_uart(const struct device *die_dev) {
   /* Definitions */
   dev = die_dev;
   const struct uart_config conf = {
		.baudrate = 9600,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};

   /* Config function calls */
   int err = uart_configure(die_dev, &conf);
   if (err == -ENOTSUP) {
      LOG_ERR("Error: could not configure UART device");
      return 1;
   }

   /* Configure UART callback */
   uart_irq_callback_set(dev, uart_cb);
   return 0;
}
