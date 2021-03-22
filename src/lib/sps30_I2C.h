#define SPS_CMD_START_MEASUREMENT 0x0010
#define SPS_CMD_START_MEASUREMENT_ARG 0x0300
#define SPS_CMD_STOP_MEASUREMENT 0x0104
#define SPS_CMD_READ_MEASUREMENT 0x0300
#define SPS_CMD_START_STOP_DELAY_USEC 20000
#define SPS_CMD_GET_DATA_READY 0x0202
#define SPS_CMD_AUTOCLEAN_INTERVAL 0x8004
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

/**
 * sps30_read_device_status_register() - Read the Device Status Register
 *
 * Reads the Device Status Register which reveals info, warnings and errors
 * about the sensor's current operational state. Note that the flags are
 * self-clearing, i.e. they reset to 0 when the condition is resolved.
 * Note that this command only works on firmware 2.2 or more recent.
 *
 * @device_status_flags:    Memory where the device status flags are written
 *                          into
 *
 * Return:          0 on success, an error code otherwise (e.g. if the firmware
 *                  does not support the command)
 */
int16_t sps30_read_device_status_register(uint32_t* device_status_flags);

/**
 * sps30_wake_up() - Wake up the sensor from sleep
 *
 * Use this command to wake up the sensor from sleep mode into idle mode.
 * Note that this command only works on firmware 2.0 or more recent.
 *
 * Return:          0 on success, an error code otherwise (e.g. if the firmware
 *                  does not support the command)
 */
int16_t sps30_wake_up(void);

/**
 * sps30_read_datda_ready() - reads the current data-ready flag
 *
 * The data-ready flag indicates that new (not yet retrieved) measurements are
 * available
 *
 * @data_ready: Memory where the data-ready flag (0|1) is stored.
 * Return:      0 on success, an error code otherwise
 */

int16_t sps30_read_data_ready(uint16_t* data_ready);

/**
 * sps30_get_fan_auto_cleaning_interval() - read the current(*) auto-cleaning
 * interval
 *
 * Note that interval_seconds must be discarded when the return code is
 * non-zero.
 *
 * (*) Note that due to a firmware bug on FW<2.2, the reported interval is only
 * updated on sensor restart/reset. If the interval was thus updated after the
 * last reset, the old value is still reported. Power-cycle the sensor or call
 * sps30_reset() first if you need the latest value.
 *
 * @interval_seconds:   Memory where the interval in seconds is stored
 * Return:              0 on success, an error code otherwise
 */

int16_t sps30_get_fan_auto_cleaning_interval(uint32_t* interval_seconds);

/**
 * sps30_set_fan_auto_cleaning_interval() - set the current auto-cleaning
 * interval
 *
 * @interval_seconds:   Value in seconds used to sets the auto-cleaning
 *                      interval, 0 to disable auto cleaning
 * Return:              0 on success, an error code otherwise
 */

int16_t sps30_set_fan_auto_cleaning_interval(uint32_t interval_seconds);

/**
 * sps30_reset() - reset the SGP30
 *
 * The sensor reboots to the same state as before the reset but takes a few
 * seconds to resume measurements.
 *
 * The caller should wait at least SPS30_RESET_DELAY_USEC microseconds before
 * interacting with the sensor again in order for the sensor to restart.
 * Interactions with the sensor before this delay might fail.
 *
 * Note that the interface-select configuration is reinterpreted, thus Pin 4
 * must be pulled to ground during the reset period for the sensor to remain in
 * i2c mode.
 *
 * Return:          0 on success, an error code otherwise
 */
int16_t sps30_reset(void);

/**
 * sps30_sleep() - Send the (idle) sensor to sleep
 *
 * The sensor will reduce its power consumption to a minimum, but must be woken
 * up again with sps30_wake_up() prior to resuming operations. It will only
 * suceed if the sensor is idle, i.e. not currently measuring.
 * Note that this command only works on firmware 2.0 or more recent.
 *
 * Return:          0 on success, an error code otherwise (e.g. if the firmware
 *                  does not support the command)
 */
int16_t sps30_sleep(void);

