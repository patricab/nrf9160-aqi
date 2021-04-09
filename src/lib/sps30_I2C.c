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

struct sps30_measurement
{
	uint16_t nc_2p5; // Number concentration PM2.5 [#/cm^3]
	uint16_t nc_10p0; // Number concentration PM10 [#/cm^3]
	uint16_t typ_size; // Typical Particle Size [nm]
};

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

	int err = i2c_write(dev, p, sizeof(), addr);
	if (err == -EIO)
	{
		LOG_ERR("Error: could not set pointer");
		return 1;
	}
	
	return 0;
}

// -- I2C set pointer read function -- //
static int sps30_set_pointer_read(const struct device *dev, uint16_t ptr, uint16_t addr, uint8_t *data, uint32_t num_bytes)
{
	int err = sps30_set_pointer(dev, addr, ptr);
	if (err)
	{
		return err;
	}

	err = i2c_read(dev, data, num_bytes, addr);
	if (err == -EIO)
	{
		return 1;
	}
	
	return 0;
}

// -- i2c write function -- //
static int sps30_set_pointer_write(const struct device *dev, uint16_t addr, uint8_t *wr_data, uint16_t ptr)
{
	const uint8_t data[5];

	// address pointer
	data[0] = ptr >> 8;
	data[1] = ptr << 8;

	for (int i = 2; i < sizeof(wr_data); i++)
	{
		data[i] = wr_data[i-2];
	}

	int err = i2c_write(dev, data, sizeof(data) , addr);
	
	if (err == -EIO)
	{
		LOG_ERR("Error: couln't write data");
		return 1;
	}
	return 0;
}


int sps30_particle_read(const struct device *dev, uint16_t addr, uint8_t *data, uint32_t num_bytes)
{
	struct sps30_data *drv_data = dev->data;
	int ret;
	uint8_t pms_receive_buffer[30];

	// ---- Wake up sequence ---- //
	(void)sps30_set_pointer(dev, SPS_CMD_WAKE_UP);
	ret = sps30_set_pointer(dev, SPS_CMD_WAKE_UP);
	if (ret) {
		return ret;
	}


	// delay(SPS_CMD_DELAY_USEC);
	// ------------------------ //

	ret = sps30_i2c_write(dev, SPS_CMD_START_MEASUREMENT, &data[0], 1); //start measurement

	if (ret < 0)
		LOG_DBG("Error starting measurement");
	return ret;

	// wait function here 1s

	ret = sps30_i2c_read(dev, SPS_CMD_READ_MEASUREMENT, &pms_receive_buffer[0], 30); //set data from measurement

	if (ret < 0)
	{
		LOG_DBG("Failed to read measurement");
	}
	else
	{

		drv_data->nc_2p5 =
			(pms_receive_buffer[20] << 8) + pms_receive_buffer[18];
		drv_data->nc_10p0 =
			(pms_receive_buffer[24] << 8) + pms_receive_buffer[26];
		drv_data->typ_siz =
			(pms_receive_buffer[27] << 8) + pms_receive_buffer[29];

		LOG_DBG("nc_2p5 = %d", drv_data->nc_2p5);
		LOG_DBG("nc_10 = %d", drv_data->nc_10p0);
		LOG_DBG("typ_siz = %d", drv_data->typ_siz);
	}

	ret = sps30_set_pointer(dev, SPS_CMD_SLEEP); // Sleep mode

	if (ret < 0)
	{
		LOG_DBG("Failed to set device to sleep");
		return ret;
	}

	//sps30_i2c_write(pms_dev,SPS_CMD_RESET,&data[0],1)

	//
	return 0;
}

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