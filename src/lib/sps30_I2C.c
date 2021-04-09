#define DT_DRV_COMPAT sensirion_sps30

/* sensor sps30.c - Driver for sensirion SPS30 sensor
 * SPS30 product: https://no.mouser.com/ProductDetail/Sensirion/SPS30/?qs=lc2O%252BfHJPVbEPY0RBeZmPA==
 * SPS30 spec: https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/9.6_Particulate_Matter/Datasheets/Sensirion_PM_Sensors_Datasheet_SPS30.pdf
 */

#include <drivers/sensor.h>
#include <stdlib.h>
#include <string.h>
#include <arch/cpu.h>
#include <errno.h>
#include <drivers/i2c.h>
#include <logging/log.h>
#include <kernel.h>
#include <init.h>
#include <device.h>

#include "sps30_I2C.h"

LOG_MODULE_REGISTER(SPS30, CONFIG_SENSOR_LOG_LEVEL);

const struct device *dev;

static uint8_t check(uint8_t data[2])
{
	uint8_t crc = 0xFF;
	for (int i = 0; i < 2; i++)
	{
		crc ^= data[i];
		for (uint8_t bit = 8; bit > 0; --bit)
		{
			if (crc & 0x80)
			{
				crc = (crc << 1) ^ 0x31u;
			}
			else
			{
				crc = (crc << 1);
			}
		}
	}
	return crc;
}

// ---- I2C set pointer function ---- //
static int sps30_set_pointer(const struct device *dev, uint16_t ptr)
{
	// Pointer MSB + LSB
	const uint8_t p[2] = {ptr >> 8, ptr << 8};

	int err = i2c_write(dev, p, sizeof(), SPS30_I2C_ADDRESS);
	if (err == -EIO)
	{
		LOG_ERR("Error: could not set pointer");
		return 1;
	}
	
	return 0;
}

// -- I2C set pointer read function -- //
static int sps30_set_pointer_read(const struct device *dev, uint16_t ptr, uint8_t *data)
{
	int err = sps30_set_pointer(dev, SPS30_I2C_ADDRESS, ptr);
	if (err)
	{
		return err;
	}

	err = i2c_read(dev, data, sizeof(data), SPS30_I2C_ADDRESS);
	if (err == -EIO)
	{
		return 1;
	}
	
	return 0;
}

// -- i2c write function -- //
static int sps30_set_pointer_write(const struct device *dev, uint16_t ptr, uint8_t *wr_data)
{
	const uint8_t data[5];

	// address pointer
	data[0] = ptr >> 8;
	data[1] = ptr << 8;

	for (int i = 2; i < sizeof(wr_data); i++)
	{
		data[i] = wr_data[i-2];
	}

	int err = i2c_write(dev, data, sizeof(data), SPS30_I2C_ADDRESS);
	if (err == -EIO)
	{
		LOG_ERR("Error: couln't write data");
		return 1;
	}
	return 0;
}

/**
	@brief Read particle measurment

	@param dev Pointer to user device
	@param sps30_data Pointer to sensor struct

	@retval 0 if successful, 1 if errors occured
*/
int sps30_particle_read(const struct device *dev, struct *sps30_data)
{
	uint8_t rx_buf[60];
	// Set pointer to sps30 struct
	sps30_data = &sps30;


	// ---- Wake up sequence ---- //
	(void)sps30_set_pointer(dev, SPS_CMD_WAKE_UP);
	int ret = sps30_set_pointer(dev, SPS_CMD_WAKE_UP);
	if (ret) {
		return ret;
	}


	// ---- Start measurement ---- //
	uint8_t args_buf[2];
	uint8_t start_buf[3];

	args_buf[0] = 0x03; // set arg big endian float values
	args_buf[1] = 0x00; // dummy byte

	start_buf[0] = args_buf[0]; 
	start_buf[1] = args_buf[1]; 
	start_buf[2] = check(args_buf); // checksum byte

	ret = sps30_set_pointer_write(dev, SPS_CMD_START_MEASUREMENT, start_buf); // start measurement with arg
	if (ret)
	{
		LOG_ERR("Error: Failed starting measurement");
		return ret;
	}

	// ---- read data ready flag ---- //
	uint8_t flag_buf[3]; 
	// flag_buf[0] = 0x00; flag_buf[1] Data ready flag byte, 0x00: no new measurements, 0x01: new measurements to read; flag_buf[3]: checksum
	ret = sps30_set_pointer_read(dev, SPS_CMD_START_STET_DATA_READY, flag_buf);
	if (ret)
	{
		LOG_ERR("Error: Failed to read data ready flag");
		return ret;
	}

	// Read measured values (60 bytes)
	while (flag_buf[1] == 0x00) { // Wait for data-ready flag
	}
	ret = sps30_set_pointer_read(dev, SPS_CMD_READ_MEASUREMENT, rx_buf, sizeof(rx_buf)); 
	if (ret)
	{
		LOG_ERR("Failed to read measurement");
		return ret;
	}

	sps30_data->nc_2p5 = (rx_buf[36] << 32) | (rx_buf[37] << 16) | (rx_buf[39] << 8) | rx_buf[40];
	sps30_data->nc_10p0 = (rx_buf[48] << 32) | (rx_buf[49] << 16) | (rx_buf[51] << 8) | rx_buf[52];
	sps30_data->typ_siz = (rx_buf[54] << 32) | (rx_buf[55] << 16) | (rx_buf[57] << 8) | rx_buf[58];

	// LOG_DBG("nc_2p5 = %d", drv_data->nc_2p5);
	// LOG_DBG("nc_10 = %d", drv_data->nc_10p0);
	// LOG_DBG("typ_siz = %d", drv_data->typ_siz);

	// Stop measurment
	ret = sps30_set_pointer(dev, SPS_CMD_STOP_MEASUREMENT);
	if (ret)
	{
		LOG_ERR("Error: Failed to stop measurement");
		return ret;
	}

	// Sleep mode
	ret = sps30_set_pointer(dev, SPS_CMD_SLEEP); 
	if (ret)
	{
		LOG_ERR("Error: Failed to set device to sleep");
		return ret;
	}

	return 0;
}

/**
	@brief Initialize sps30 library

	@param dev Pointer to user device

	@retval 0 if successful, 1 if errors occured
*/
int sps30_init(const struct device *dev)
{
	// nRF I2C master configuration
	uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_MASTER;

	uint8_t id[4];
	uint32_t id32;

	i2c_configure(dev, i2c_cfg);

	//-- device status register --//
	// int ret = sps30_i2c_read(dev, SPS_CMD_READ_DEVICE_STATUS_REG, &id[0], 2);

	// if (ret < 0)
	// {
	// 	return ret;
	// }
	// LOG_ERR("Error reading status register");

	// id32 = (id[4] << 24) + (id[3] << 16) + (id[2] << 8) + id[1];

	// // bit 21 fan speed
	// if (id32 << SPS30_DEVICE_STATUS_FAN_ERROR_MASK)
	// {
	// 	LOG_ERR("Error: Fan speed out of range");
	// 	return -EIO;
	// }

	// // bit 5 laser failure
	// if (id32 << SPS30_DEVICE_STATUS_LASER_ERROR_MASK)
	// {
	// 	LOG_ERR("Error: Laser failure");
	// 	return -EIO;
	// }

	// // bit 4 fan failure
	// if (id32 << SPS30_DEVICE_STATUS_FAN_SPEED_WARNING)
	// {
	// 	LOG_ERR("Error:Fan failure, fan is mechanically blocked or broken");
	// 	return -EIO;
	// }
	return 0;
}