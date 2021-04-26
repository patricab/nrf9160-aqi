#include "sps30_I2C.h"
#include <sys/printk.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(sps30, CONFIG_APP_LOG_LEVEL);

const struct device *dev;
extern struct sps30_data sps30;

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
	// Pointer MSB and LSB
	// Note: LSB = ptr: 000000000 XXXXXXXX
	//		    & 0xFF:		      11111111	
	//				  =			  XXXXXXXX
	unsigned char p[2];
	p[0] = (ptr & 0xFF00) >> 8;
	p[1] = ptr & 0x00FF;

	int ret = i2c_write(dev, p, 2, SPS30_I2C_ADDRESS);
	if (ret)
	{
		printk("\nError: could not set pointer\n");
		return ret;
	}
	
	return 0;
}

// -- I2C set pointer read function -- //
static int sps30_set_pointer_read(const struct device *dev, uint16_t ptr, uint8_t *data)
{
	int err = sps30_set_pointer(dev, ptr);
	if (err)
	{
		return err;
	}

	if (i2c_read(dev, data, sizeof(data), SPS30_I2C_ADDRESS))
	{
		return 1;
	}
	
	return 0;
}

// -- i2c write function -- //
static int sps30_set_pointer_write(const struct device *dev, uint16_t ptr, uint8_t *wr_data)
{
	unsigned char data[5];

	// address pointer
	data[0] = (ptr & 0xFF00) >> 8;
	data[1] = ptr & 0x00FF;

	for (int i = 2; i <= sizeof(wr_data); i++)
	{
		data[i] = wr_data[i-2];
	}

	if (i2c_write(dev, data, sizeof(data), SPS30_I2C_ADDRESS))
	{
		printk("\nError: couln't write data\n");
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
int sps30_particle_read(const struct device *dev)
{
	unsigned char rx_buf[60] = {0};
	int ret;
	
	// ---- Wake up sequence ---- //
	(void)sps30_set_pointer(dev, SPS_CMD_WAKE_UP);
	ret = sps30_set_pointer(dev, SPS_CMD_WAKE_UP);
	if (ret) {
		printk("\nCould not wake up 2\n");
		return ret;
	}
	
	k_sleep(K_MSEC(1000));

	// ---- Start measurement ---- //
	unsigned char start_buf[3];
	unsigned char args_buf[2];
	args_buf[0] = 0x03; // set arg big endian float values
	args_buf[1] = 0x00; // dummy byte

	start_buf[0] = args_buf[0];
	start_buf[1] = args_buf[1];
	start_buf[2] = check(args_buf); // checksum byte

	ret = sps30_set_pointer_write(dev, SPS_CMD_START_MEASUREMENT, start_buf);
	if (ret)
	{
		printk("\nError: Failed starting measurement\n");
		return ret;
	}
	
	k_sleep(K_MSEC(1000));

	// ---- read data ready flag ---- //
	// flag_buf[0] = 0x00; flag_buf[1] Data ready flag byte, 0x00: no new measurements, 0x01: new measurements to read; flag_buf[3]: checksum
	unsigned char flag_buf[3] = {0}; 
	ret = sps30_set_pointer_read(dev, SPS_CMD_START_STET_DATA_READY, flag_buf);
	if (ret)
	{
		printk("\nError: Failed to read data ready flag\n");
		return ret;
	}
	
	while (flag_buf[1] == 0x00)
	{
		#if defined(CONFIG_LOG)
		printk("\n measuring \n");
		k_sleep(K_MSEC(1000));
		#endif
		(void)sps30_set_pointer_read(dev, SPS_CMD_START_STET_DATA_READY, flag_buf);
	}

	#if defined(CONFIG_LOG)
	printk("\n data ready flag = %d \n",flag_buf[1]);
	#endif
	

	ret	= sps30_set_pointer(dev,SPS_CMD_READ_MEASUREMENT);
	if (ret)
	{
		printk("feil i lesing");
	}

	//ret = sps30_set_pointer_read(dev, SPS_CMD_READ_MEASUREMENT, rx_buf); 
	ret = i2c_read(dev,rx_buf,60,SPS30_I2C_ADDRESS);
	k_sleep(K_MSEC(1000));
	if (ret)
	{
		printk("\nFailed to read measurement\n");
		return ret;
	}


	sps30.nc_2p5 = (rx_buf[36] << 24) | (rx_buf[37] << 16) | (rx_buf[39] << 8) | rx_buf[40];
	sps30.nc_10p0 = (rx_buf[48] << 24) | (rx_buf[49] << 16) | (rx_buf[51] << 8) | rx_buf[52];
	sps30.typ_size = (rx_buf[54] << 24) | (rx_buf[55] << 16) | (rx_buf[57] << 8) | rx_buf[58];

	for (size_t i = 0; i < 59; i++)
	{
		printk("\ndata = %d\n", rx_buf[i]);
	}
	


	//printk("\nnc_10p0 = %d %d\n", r[0], r[1]);
	#if defined(CONFIG_LOG)
	// int *r = dec(sps30.nc_2p5);
    // printk("\nnc_2p5 = %d %d\n", r[0], r[1]);
	#endif

	// Stop measurment
	ret = sps30_set_pointer(dev,SPS_CMD_STOP_MEASUREMENT);
	if (ret)
	{
		printk("\nError: Failed to stop measurement\n");
		return ret;
	}

	k_sleep(K_MSEC(1000));
	
	// Sleep mode
	ret = sps30_set_pointer(dev, SPS_CMD_SLEEP);
	if (ret)
	{
		printk("\nError: Failed to set device to sleep\n");
		return ret;
	}
	#if defined(CONFIG_LOG)
	printk("\n entering sleep mode \n");
	#endif

	return 0;
}

/**
	@brief Initialize sps30 library

	@param dev Pointer to user device

	@retval 0 if successful, 1 if errors occured
*/
int sps30_init(const struct device *dev, struct sps30_data *data) 
{
	sps30 = *data; // Make user struct global
	
	// nRF I2C master configuration
	uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_MASTER;
	//uint8_t id[6];

	if (i2c_configure(dev, i2c_cfg))
	{
		printk("\nError: could not configure i2c service\n");
	}
	
	#if defined(CONFIG_LOG)
	printk("\nInit done\n");
	#endif

	//-- device status register --//
	/*
	int ret = sps30_set_pointer_read(dev, SPS_CMD_READ_DEVICE_STATUS_REG, id);
	if (ret)
	{
		printk("\nError: Reading device status register\n");
		return ret;
	}

	uint32_t id32 = (id[0] << 24) | (id[1] << 16) | (id[3] << 8) | id[4];

	// bit 21 fan speed
	if (id32 & SPS30_DEVICE_STATUS_FAN_ERROR_MASK)
	{
		printk("\nError: Fan speed out of range\n");
		return -EIO;
	}

	// bit 5 laser failure
	if (id32 & SPS30_DEVICE_STATUS_LASER_ERROR_MASK)
	{
		printk("\nError: Laser failure\n");
		return -EIO;
	}

	// bit 4 fan failure
	if (id32 & SPS30_DEVICE_STATUS_FAN_SPEED_WARNING)
	{
		printk("\nError: Fan failure, fan is mechanically blocked or broken\n");
		return -EIO;
	}
	*/
	return 0;
}