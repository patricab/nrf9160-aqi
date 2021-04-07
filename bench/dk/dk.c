#include "dgso3.h"

#include "ui.h"

LOG_MODULE_REGISTER(dk, CONFIG_APP_LOG_LEVEL);

static int16_t *rx_val;
static const struct device *die_dev;

// Button state callback
static void btn_cb(struct ui_evt *evt) {
    int err;

    if (!evt) {
        return;
    }
    // Check if/which button is active
    if (evt->type == UI_EVT_BUTTON_ACTIVE)
    {
        switch (evt->button)
        {
            case UI_BUTTON_1:
                err = read_gas(rx_val);
                if (err == 1)
                {
                    LOG_ERR("Error: could not read measurement");
                }
                break;
            case UI_BUTTON_2:
                standby_gas();
                break;
            case UI_SWITCH_1:
                zero_gas();
                break;
        }
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

    // Configure buttons
    err = ui_init(btn_cb);
    if (err < 0)
    {
        LOG_ERR("Error: could not initalize button 1");
    }

    // Wait for input
    while (1) {
            k_cpu_idle();
        }
}