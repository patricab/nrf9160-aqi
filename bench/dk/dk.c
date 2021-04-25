#include "dgso3.h"

#include <dk_buttons_and_leds.h>

LOG_MODULE_REGISTER(dk, CONFIG_APP_LOG_LEVEL);

static int16_t *rx_val;
static const struct device *die_dev;

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
        unsigned char rx = 'x';
        while (1)
        {
            // while (uart_poll_in(die_dev, &rx) < 0) {
            // }
            int err = uart_poll_in(die_dev, &rx);
            if (err < 0)
            {
                LOG_DBG("No bueno");
            }
            if (rx == 's')
            {
                (void)dk_set_leds(DK_LED1_MSK);
            }
        }
        
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
	die_dev = device_get_binding(DT_LABEL(DT_NODELABEL(uart0)));
	if (!die_dev)
	{
		LOG_ERR("No device found.");
	}

    // Configure UART
	int err = init_uart(die_dev);
	if (err == 1)
	{
		LOG_ERR("Error: Could not initialize UART");
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

    // Wait for input
    while (1) {
            k_cpu_idle();
        }
}