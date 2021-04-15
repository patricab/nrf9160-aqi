#include "sps30_I2C.h"

#include <dk_buttons_and_leds.h>
#include <sys/printk.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(dk, CONFIG_APP_LOG_LEVEL);

static const struct device *die_dev;
struct sps30_data sps30;

static int * dec(float num) {
    static int r[2];
    r[0] = (int)num;
    r[1] = num - r[0];

    return r;
}

static void btn_cb(uint32_t button_states, uint32_t has_changed)
{
    if (has_changed & button_states & DK_BTN1_MSK) {
        // printk("\nBtn1\n");
        int err = sps30_init(die_dev, &sps30);
        if (err)
        {
            printk("Error: Could not initialize UART");
        }
    }
    else if (has_changed & button_states & DK_BTN2_MSK)
    {
        // printk("\nBtn2\n");
     
     
        int err = sps30_particle_read(die_dev);
        if (err)
        {
            printk("Error: could not read measurement");

        }
        k_sleep(K_MSEC(1000));
        int *r = dec(sps30.nc_2p5);
        printk("\nnc_2p5 = %d %d\n", r[0], r[1]);
        r = dec(sps30.nc_10p0);
        printk("\nnc_10 = %d %d\n", r[0], r[1]);
        r = dec(sps30.typ_size);
        printk("\ntyp_size = %d %d\n", r[0], r[1]);
      
      

       
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
	die_dev = device_get_binding(DT_LABEL(DT_NODELABEL(i2c3)));
	if (!die_dev)
	{
		printk("No device found.");
	}

    int err = dk_buttons_init(btn_cb);
    if (err < 0)
    {
        printk("Error: could not initalize button 1");
    }

    // Wait for input
    while (1) {
        k_cpu_idle();
    }
}