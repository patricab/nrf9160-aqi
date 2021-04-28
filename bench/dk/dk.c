#include "dgso3.h"

#include <dk_buttons_and_leds.h>

LOG_MODULE_REGISTER(dk, CONFIG_APP_LOG_LEVEL);

unsigned char rx_buf[65];
static volatile bool data_received;
static const struct device *die_dev;
static volatile int n;

// void uart_cb(const struct device *dev, void *data) {
//     unsigned char rx_val = 0;

//     // Start processing interrupts in ISR
//     uart_irq_update(dev);

//    // Check if UART RX interrupt is ready
//  	if (uart_irq_rx_ready(dev)) {
//         // Read from FIFO queue
//         uart_fifo_read(dev, &rx_val, 1);

//         // End RX at at EOT (newline)
// 		if (rx_val == '\n') {
// 			data_received = true;
//             (void)dk_set_leds(DK_LED3_MSK);
//             // k_sleep(K_MSEC(100));
// 		} else {
//             rx_buf[n] = rx_val; // Add value to buffer
//             n++;
//         }
//     } else {
//         return;
//     }
// }

static void btn_cb(uint32_t button_states, uint32_t has_changed)
{
    // if (has_changed & button_states & DK_BTN1_MSK) {
    //     // int err = read_gas(rx_val);
    //     // if (err == 1)
    //     // {
    //     //     LOG_ERR("Error: could not read measurement");
    //     // }
    //     uart_poll_out(die_dev, '\r');
        // (void)dk_set_leds(DK_LED4_MSK);
    //     // printk("\r");
    // }
    // else if (has_changed & button_states & DK_BTN2_MSK)
    if (has_changed & button_states & DK_BTN2_MSK)
    {
        // n = 0;
        // uart_poll_out(die_dev, '\r');

        // uart_irq_rx_enable(die_dev);
        // // k_sleep(K_MSEC(100));

        // data_received = false;
        // while (data_received == false)
        // {
        // }

        // (void)dk_set_leds(DK_NO_LEDS_MSK);
        // (void)dk_set_leds(DK_LED1_MSK);
        
        // uart_irq_rx_disable(die_dev);
        
        // for (int i = 0; i < 65; i++)
        // {
        //     printk("\n%c\n", rx_buf[i]);
        // }
        int32_t *val = 0;
        (void)read_gas(&val);
        printk("\n%d\n", val);
    }
    // else if (has_changed & button_states & DK_BTN3_MSK)
    // {
    //     zero_gas();
    //     // printk("Z");
    // }
}

// Initalize buttons library and wait for button state change
void main(void) {
    // Configure device
	die_dev = device_get_binding(DT_LABEL(DT_NODELABEL(uart1)));
	if (!die_dev)
	{
		LOG_ERR("No device found.");
	}

    // Configure UART
	if (init_uart(die_dev))
	{
		LOG_ERR("Error: Could not initialize UART");
        (void)dk_set_leds(DK_LED4_MSK);
	}
    // if (uart_err_check(die_dev))
    // {
    //     (void)dk_set_leds(DK_LED4_MSK);
    // }
    
    int err = dk_buttons_init(btn_cb);
    if (err < 0)
    {
        LOG_ERR("Error: could not initalize button");
    }

    err = dk_leds_init();
    if (err < 0)
    {
        LOG_ERR("Error: could not initalize leds");
    }

    // uart_irq_callback_set(die_dev, uart_cb);

    // Wait for input
    while (1) {
        k_cpu_idle();
    }
}