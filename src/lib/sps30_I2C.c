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

struct sps30_measurement {
	const struct device *dev;
    uint16_t nc_2p5;
    uint16_t nc_10p0;
    uint16_t typ_siz;
};


// ---- i2c set pointer function ---- //

static int sps30_set_pointer(const struct device *dev, uint16_t addr)
{

struct sps30_data *data = dev->data;
uint8_t wr_addr[2]; = {0};
struct i2c_msg msgs;

	// address pointer 
	wr_addr[0] = (addr >> 8) & 0xFF;
	wr_addr[1] = addr & 0xFF;

	// Send the address to set
	msgs.buf = wr_addr;
	msgs.len = 2U;
	msgs.flags = I2C_MSG_WRITE | I2C_MSG_STOP;

return i2c_transfer(data->i2c, &msgs, 1, SPS30_I2C_ADDRESS);
}

// -- i2c write function -- //
static int sps30_i2c_write(const struct device *dev, uint16_t addr, uint8_t *data, uint32_t num_bytes)
{

struct sps30_data *data = dev->data;
uint8_t wr_addr[2]; // address reg 2 x 8 bits
struct i2c_msg msgs[2]; 

	// address pointer
	wr_addr[0] = (addr >> 8) & 0xFF;
	wr_addr[1] = addr & 0xFF;

	/* Setup I2C messages */

	/* Send the address to write to */
	msgs[0].buf = wr_addr;
	msgs[0].len = 2U;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Data to be written, and STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	return i2c_transfer(data->i2c, &msgs, 1, SPS30_I2C_ADDRESS);
}

// -- i2c read function -- //
static int sps30_i2c_read(const struct device *dev,uint16_t addr, uint8_t *data, uint32_t num_bytes) 
{

struct sps30_data *data = dev->data;

	// address pointer
    uint8_t wr_addr[2];
	struct i2c_msg msgs[2];


	/* sps30 pointer address */
	wr_addr[0] = (addr >> 8) & 0xFF;
	wr_addr[1] = addr & 0xFF;

	/* Setup I2C messages */

	/* Send the address to read from */
	msgs[0].buf = wr_addr;
	msgs[0].len = 2U;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Read from device. STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	return i2c_transfer(data->i2c, &msgs, 1, SPS30_I2C_ADDRESS);
}



int sps30_particle_read(const struct device *dev,uint16_t addr, uint8_t *data, uint32_t num_bytes)	
{

	struct sps30_data *drv_data = dev->data;
	int ret;
	uint8_t pms_receive_buffer[30];

	// ---- Wake up sequence ---- //
	(void)sps30_set_pointer(dev,SPS_CMD_WAKE_UP);
	ret = sps30_set_pointer(dev,SPS_CMD_WAKE_UP); 
	if(ret)
		return ret;

	// delay(SPS_CMD_DELAY_USEC);	
	// ------------------------ //


	ret = sps30_i2c_write(dev,SPS_CMD_START_MEASUREMENT,&data[0],1); //start measurement
	
	if (ret < 0)
		LOG_DBG("Error starting measurement");
		return ret;

	// wait function here 1s

	ret = sps30_i2c_read(dev,SPS_CMD_READ_MEASUREMENT,&pms_receive_buffer[0],30); //set data from measurement

	if (ret < 0) 
	{
	LOG_DBG("Failed to read measurement");
	}
	else {

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

	ret = sps30_set_pointer(dev,SPS_CMD_SLEEP); // Sleep mode

	if (ret < 0) 
	{
		LOG_DBG("Failed to set device to sleep");
	return ret;
	}

	//sps30_i2c_write(pms_dev,SPS_CMD_RESET,&data[0],1)


	// 
	return 0;
}

/*
static struct sensor_driver_api sps30_api_funcs = {
      .do_this = my_driver_do_this,
      .do_that = my_driver_do_that
};
*/
int sps30_init(const struct device *dev)
{
	struct sps30_data *drv_data = dev->data;
	int ret;
	uint8_t id[4];
	uint32_t id32;


	drv_data->i2c = device_get_binding(DT_INST_BUS_LABEL(0));
	if (drv_data->i2c == NULL) {
		LOG_ERR("Failed to get pointer to %s device!",
			    DT_INST_BUS_LABEL(0));
		return -EINVAL;
	}

//-- device status register --//
ret = sps30_i2c_read(dev,SPS_CMD_READ_DEVICE_STATUS_REG,&id[0],2);

	if(ret < 0) {
		return ret;
	}
	LOG_ERR("Error reading status register");

id32 = (id[4] << 24)+ (id[3] << 16) + (id[2] << 8) + id[1];


// bit 21 fan speed 
	if(id32 << SPS30_DEVICE_STATUS_FAN_ERROR_MASK) {
		LOG_ERR("Error: Fan speed out of range");
		return -EIO;
	}

// bit 5 laser failure
	if(id32 << SPS30_DEVICE_STATUS_LASER_ERROR_MASK) {
		LOG_ERR("Error: Laser failure");
		return -EIO;
	}

// bit 4 fan failure
	if(id32 << SPS30_DEVICE_STATUS_FAN_SPEED_WARNING) {
		LOG_ERR("Error:Fan failure, fan is mechanically blocked or broken");
		return -EIO;
	}
	return 0;
}

static struct sps30_data;

/*
DEVICE_DT_INST_DEFINE(0,sps30_init,
&sps30_data,&sps30_config,POST_KERNEL,
CONFIG_SENSOR_INIT_PRIORITY,&sps30_api_funcs);
*/