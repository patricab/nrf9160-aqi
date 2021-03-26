#define SPS_CMD_START_MEASUREMENT 0x0010
#define SPS_CMD_START_MEASUREMENT_ARG 0x0300
#define SPS_CMD_STOP_MEASUREMENT 0x0104
#define SPS_CMD_READ_MEASUREMENT 0x0300
#define SPS_CMD_START_STET_DATA_READY 0x0202
#define SPS_CMD_AOP_DELAY_USEC 20000
#define SPS_CMD_GUTOCLEAN_INTERVAL 0x8004
#define SPS_CMD_GET_FIRMWARE_VERSION 0xd100
#define SPS_CMD_GET_SERIAL 0xd033
#define SPS_CMD_RESET 0xd304
#define SPS_CMD_SLEEP 0x1001
#define SPS_CMD_READ_DEVICE_STATUS_REG 0xd206
#define SPS_CMD_START_MANUAL_FAN_CLEANING 0x5607
#define SPS_CMD_WAKE_UP 0x1103
#define SPS_CMD_DELAY_USEC 5000
#define SPS_CMD_DELAY_WRITE_FLASH_USEC 20000

#define SPS30_I2C_ADDRESS 0x69
#define SPS30_MAX_SERIAL_LEN 32
/* 1s measurement intervals */
#define SPS30_MEASUREMENT_DURATION_USEC 1000000
/* 100ms delay after resetting the sensor */
#define SPS30_RESET_DELAY_USEC 100000
/** The fan is switched on but not running */
#define SPS30_DEVICE_STATUS_FAN_ERROR_MASK (1 << 4)
/** The laser current is out of range */
#define SPS30_DEVICE_STATUS_LASER_ERROR_MASK (1 << 5)
/** The fan speed is out of range */
#define SPS30_DEVICE_STATUS_FAN_SPEED_WARNING (1 << 21)

int sps30_init(const struct device *dev);

int sps30_set_pointer(const struct device *dev, uint16_t addr);

int sps30_i2c_read(const struct device *dev,uint16_t addr, uint8_t *data, uint32_t num_bytes);

int sps30_i2c_write(const struct device *dev, uint16_t addr, uint8_t *data, uint32_t num_bytes);

int sps30_particle_read(const struct device *pms_dev,uint16_t addr, uint8_t *data, uint32_t num_bytes);



struct sps30_data {
	const struct device *i2c;
};