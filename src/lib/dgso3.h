#ifndef DGSO3_H
#define DGSO3_H

#include <init.h>
#include <zephyr.h>
#include <device.h>
#include <kernel.h>
#include <drivers/uart.h>
#include <zephyr/types.h>
#include <drivers/sensor.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/printk.h>

#include <logging/log.h>


/**
   @brief Initialize UART device

   @param die_dev User device

   @retval 0 if successful, 1 if an error occured
*/
int init_uart(const struct device *die_dev);

/**
   @brief Read sensor gas measurement (single measurement)

   @param rx_val User RX value

   @retval 0 if successful, 1 if errors occured
*/
int read_gas(int32_t *rx_val1, int32_t *rx_val2);

/**
   @brief Set sensor to standby low power mode

   @retval none
*/
void standby_gas(void);

/**
   @brief Calibrate sensor zero value

   @retval none
*/
void zero_gas(void);

/**
   @brief Set specific sensor zero value

   @param val User zero value

   @retval none
*/
void set_gas(uint8_t val);

float get_token_float(char *ibuf, int get_token);
#endif
