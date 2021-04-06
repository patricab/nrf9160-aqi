#include "dgso3.h"
#include <dk_buttons_and_leds.h>

LOG_MODULE_REGISTER(button, CONFIG_APP_LOG_LEVEL);

static int16_t *rx_val;
static const struct device *die_dev;

static void btn_cb(uint32_t button_state, uint32_t has_changed) {
    button_state = dk_get_buttons(); // Read current button states

    if ((button_state & DK_BTN1_MSK) == DK_BTN1_MSK) // Check if button 1 has been pressed
    {
        has_changed = DK_BTN1_MSK;
        int err = read_gas(rx_val);
        if (err == 1)
        {
            LOG_ERR("Error: could not read measurement");
        }
    }
    else if ((button_state & DK_BTN2_MSK) == DK_BTN2_MSK) // Check if button 2 has been pressed
    {
        has_changed = DK_BTN2_MSK;
        standby_gas();
    }
    else if ((button_state & DK_BTN3_MSK) == DK_BTN3_MSK) // Check if button 3 has been pressed
    {
        has_changed = DK_BTN3_MSK;
        zero_gas();
    }
}

// Initalize buttons library and wait for button state change
void main(void) {
	die_dev = device_get_binding(DT_LABEL(DT_NODELABEL(uart0)));
	if (!die_dev)
	{
		LOG_ERR("No device found.");
	}

	int err = init_uart(die_dev);
	if (err == 1)
	{
		LOG_ERR("I/O error");
	}

    err = dk_buttons_init(btn_cb);
    if (err == 1) {
        LOG_ERR("Error: Could not initalize buttons");
    }

    while (1) {
            k_cpu_idle();
        }
}