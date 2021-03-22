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
#include <sps30_I2C.h>

LOG_MODULE_REGISTER(SPS30, CONFIG_SENSOR_LOG_LEVEL);

#define CFG_SPS30_SERIAL_TIMEOUT 1000

struct sps30_measurement {
	const struct device *pms_dev;
    uint16_t nc_2p5;
    uint16_t nc_10p0;
    uint16_t typ_siz;
};
uint8_t data[16];

/** Checksum calculation */
// uint8_t CalcCrc(uint8_t data[2]) {
//     uint8_t crc = 0xFF;
//     for(int i = 0; i < 2; i++) {
//         crc ^= data[i];
//         for(uint8_t bit = 8; bit > 0; --bit) {
//             if(crc & 0x80) {
//                 crc = (crc << 1) ^ 0x31u;
//             } else {
//                 crc = (crc << 1);
//             }
//         }
//     }
//     return crc;
// }

// -- i2c write function -- //
static int sps30_i2c_write(const struct device *pms_dev, uint16_t addr, uint8_t *data, uint32_t num_bytes){

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

	return i2c_transfer(pms_dev, &msgs[0], 2, SPS30_I2C_ADDRESS);
};

// -- i2c read function -- //
static int sps30_i2c_read(const struct device *pms_dev,uint16_t addr, uint8_t *data, uint32_t num_bytes){

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

	return i2c_transfer(pms_dev, &msgs[0], 2, SPS30_I2C_ADDRESS);
};



static int sps_sample_fetch(const struct device *pms_dev,uint16_t addr, uint8_t *data, uint32_t num_bytes)	{

struct pms_dev *drv_data = dev->data;
uint8_t ret;
uint8_t pms_receive_buffer[30];

// ---- set idle mode ---- //
sps30_i2c_write(pms_dev,SPS_CMD_WAKE_UP,&data[0],0);
sps30_i2c_write(pms_dev,SPS_CMD_WAKE_UP,&data[0],0); 
// ------------------------ //


sps30_i2c_write(pms_dev,SPS_CMD_START_MEASUREMENT,&data[0],1); //start measurement

data[0]=0x00;
ret=sps30_i2c_read(pms_dev,SPS_CMD_READ_DEVICE_STATUS_REG,&data[0],1); // rdata ready flag must be looked at first

if(ret){
	printk("No data to read");
return 1;
}	else{
	printk("Read 0x%X from status reg",data[0]);
}

sps30_i2c_write(pms_dev,SPS_CMD_STOP_MEASUREMENT,&data[0],0); //stop measurement

sps30_i2c_read(pms_dev,SPS_CMD_READ_MEASUREMENT,&pms_receive_buffer[0],30); //set data from measurement


	drv_data->nc_2p5 =
	    (pms_receive_buffer[10] << 8) + pms_receive_buffer[11];
	drv_data->nc_10p0 =
	    (pms_receive_buffer[14] << 8) + pms_receive_buffer[15];
	drv_data->typ_siz =
	    (pms_receive_buffer[12] << 8) + pms_receive_buffer[13];

	LOG_DBG("nc_2p5 = %d", drv_data->nc_2p5);
	LOG_DBG("nc_10 = %d", drv_data->nc_10p0);
	LOG_DBG("typ_siz = %d", drv_data->typ_siz);


sps30_i2c_write(pms_dev,SPS_CMD_SLEEP,&data[0],0); // Sleep mode


//sps30_i2c_write(pms_dev,SPS_CMD_RESET,&data[0],1)


// 

}