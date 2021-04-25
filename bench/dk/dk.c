#include "dgso3.h"

#include <dk_buttons_and_leds.h>

LOG_MODULE_REGISTER(dk, CONFIG_APP_LOG_LEVEL);

static uint8_t rx_val;
// static int16_t *rx_val;
static volatile bool data_received;
static const struct device *die_dev;

static void uart_cb(const struct device *dev, void *data) {
   // Check if UART RX interrupt is ready
 	if (uart_irq_rx_ready(dev)) {
        // Read from FIFO queue
        uart_fifo_read(dev, &rx_val, 1);

        // End RX at at EOT (newline)
		if (rx_val == 's') {
			data_received = true;
		}
  }
}

static void btn_cb(uint32_t button_states, uint32_t has_changed)
{
    if (has_changed & button_states & DK_BTN1_MSK) {
        // int err = read_gas(rx_val);
        // if (err == 1)
        // {
        //     LOG_ERR("Error: could not read measurement");
        // }
        uart_poll_out(die_dev, '\r');
        // printk("\r");
    }
    else if (has_changed & button_states & DK_BTN2_MSK)
    {
        uart_irq_rx_enable(die_dev);

        data_received = false;
        while (data_received == false)
        {
        }
        // uart_irq_rx_disable(die_dev);
        (void)dk_set_leds(DK_LED1_MSK);
        
        // standby_gas();
        // printk("s");
    }
    else if (has_changed & button_states & DK_BTN3_MSK)
    {
        zero_gas();
        // printk("Z");
    }
}

// Initalize buttons library and wait for button state change
void main(void) {
    // Configure device
	die_dev = device_get_binding("UART_0");
	if (!die_dev)
	{
		LOG_ERR("No device found.");
	}

    // Configure UART
	int err = init_uart(die_dev);
	if (err == 1)
	{
		LOG_ERR("Error: Could not initialize UART");
        (void)dk_set_leds(DK_LED4_MSK);
	}
    
    err = dk_buttons_init(btn_cb);
    if (err < 0)
    {
        LOG_ERR("Error: could not initalize button");
    }

    err = dk_leds_init();
    if (err < 0)
    {
        LOG_ERR("Error: could not initalize leds");
    }

    (void)dk_set_leds(DK_LED3_MSK);
    uart_irq_callback_set(die_dev, uart_cb);

    // Wait for input
    while (1) {
            k_cpu_idle();
        }
}