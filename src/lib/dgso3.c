#include "dgso3.h"

/* Variables */
static unsigned char rx_buf[30] = {0};
static const struct device *dev;
static volatile bool data_received;

/* Inital configuration */
LOG_MODULE_REGISTER(app_uart, CONFIG_APP_LOG_LEVEL);

/* UART recieve (RX) callback */
// static void uart_cb(const struct device *dev, void *data) {
//    // Check if UART RX interrupt is ready
//  	if (uart_irq_rx_ready(dev)) {
//       // Read from FIFO queue
// 		uart_fifo_read(dev, rx_buf, sizeof(rx_buf));

//       // End RX at at EOT (newline)
// 		if (*rx_buf == '\n') {
// 			data_received = true;
// 		}
//   } // }

/* UART recieve (RX) */
// static void uart_rx(struct k_timer *timer) {
// static void uart_rx() {
//    uart_irq_rx_enable(dev);
//    data_received = false;
//    while (data_received == false) {
//       LOG_DBG("%i", data_received);
//    }
//    uart_irq_rx_disable(dev);
// }

/**
   @brief Read sensor gas measurement (single measurement)

   @param rx_val User RX value

   @retval 0 if successful, 1 if errors occured
*/
int read_gas(int32_t rx_val) {
   unsigned char val_buf[15] = {0};
   unsigned char recv_char = 0;
   int counter = 0;

   // Send command 
   // uart_poll_out(dev, '\r');
   printk("\n");

   // Listen for data 
   while (1) { // Run until newline 
      while (uart_poll_in(dev, &recv_char) < 0) { // Wait until character is available
      }
      rx_buf[counter] = recv_char;
      
      if (recv_char == '\n') {
         break;
      }
      // if (counter < sizeof(rx_buf)) { // Check if buffer is full
      //    rx_buf[counter] = recv_char;
      //    uart_poll_in(dev, &recv_char);
      //    counter++;
      // } else {
      //    break;
      // }
   }

   // Get PPB values at second comma value
   // for (int i = 0; i < sizeof(rx_buf); i++) {
   //    if (rx_buf[i] == ',') {
   //       int k = 0;
   //       i += 2; // Skip space after comma
   //       while (rx_buf[i] != ',') {
   //          val_buf[k] = rx_buf[i];
   //          i++; // Increment index for output buffer and rx buffer
   //          k++;
   //       }
   //       break;
   //    }
   // }
   
   // Output modified rx buffer
   // sscanf(val_buf, "%d", &rx_val);
   rx_val = (int32_t)recv_char;
   // rx_val = rx_buf[0];
   // printk("\n %d \n", rx_val);
   
   return 0;
}

/**
   @brief Set sensor to standby low power mode

   @retval none
*/
void standby_gas(void) {
   /* Send command */
   // printk("s");
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
   // delay(K_MSEC(10));
   printk("%u", val);

   /* Send return character */
   // delay(K_MSEC(10));
   uart_poll_out(dev, '\r');
}

// float get_token_float(char *ibuf, int get_token) {
//    uint8_t token = 0;
//    uint8_t i =  0;
//    float ret = 0.0;
//    char return_buf[32];
//    memset(return_buf, 0, sizeof(return_buf));

//    while ((uint8_t) *ibuf != 0) {
//       if (*ibuf == ',') {
//          if (token == get_token) {
//             ret = strtof(return_buf, NULL);
//             printk("get token %i %s %f\n", token, return_buf, ret);
//             break;
//          }
//          token++;
//          i = 0;
//          memset(return_buf, 0, sizeof(return_buf));
//       } else {
//          return_buf[i] = *ibuf;
//          i++;
//       }
//       ibuf++;
//    }
//    return ret;
// }

/**
   @brief Initialize UART device

   @param die_dev User device

   @retval 0 if successful, 1 if an error occured
*/
int init_uart(const struct device *die_dev) {
   /* Definitions */
   struct uart_config conf = {
		.baudrate = 9600,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};
   dev = die_dev;

   /* Config function calls */
   int err = uart_configure(dev, &conf);
   if (err) {
      LOG_ERR("Error: could not configure UART device");
      return 1;
   }

   /* Configure UART callback */
   // uart_irq_callback_set(dev, uart_cb);
   return 0;
}