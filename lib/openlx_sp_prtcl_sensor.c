/*
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2018-2019 Foundries.io
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Source material for OpenLX SP Particulate Sensor object (10314):
 * https://github.com/OpenLX SP/pub/blob/master/docs/OpenLX SP-.pdf
 * Section: "10. OpenLX SP Object: Particulate"
 */

#define LOG_MODULE_NAME net_ipso_prtcl_sensor
#define LOG_LEVEL CONFIG_LWM2M_LOG_LEVEL

#include <logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include <stdint.h>
#include <init.h>

#include "lwm2m_object.h"
#include "lwm2m_engine.h"

#ifdef CONFIG_LWM2M_OPENLX_SP_PRTCL_SENSOR_TIMESTAMP
#define ADD_TIMESTAMPS 1
#else
#define ADD_TIMESTAMPS 0
#endif

/* Server resource IDs */
#define PRTCL_SENSOR_VALUE_ID                    5700
#define PRTCL_UNITS_ID                           5701
#define PRTCL_MIN_MEASURED_VALUE_ID              5601
#define PRTCL_MAX_MEASURED_VALUE_ID              5602
#define PRTCL_MAX_RANGE_VALUE_ID                 5604
#define PRTCL_RESET_MIN_MAX_MEASURED_VALUES_ID   5605
#define PRTCL_MEASURED_PARTICLE_SIZE_ID          6043

#if ADD_TIMESTAMPS
#define PRTCL_TIMESTAMP_ID                       5518
#define PRTCL_MAX_ID		8
#else  /* !ADD_TIMESTAMPS */
#define PRTCL_MAX_ID		7
#endif

#define MAX_INSTANCE_COUNT	CONFIG_LWM2M_OPENLX_SP_PRTCL_SENSOR_INSTANCE_COUNT

#define PRTCL_STRING_SHORT	8

/*
 * Calculate resource instances as follows:
 * start with PRTCL_MAX_ID
 * subtract EXEC resources (1)
 */
#define RESOURCE_INSTANCE_COUNT	(PRTCL_MAX_ID - 1)

/* resource state variables */
static float32_value_t sensor_value[MAX_INSTANCE_COUNT];
static char units[MAX_INSTANCE_COUNT][PRTCL_STRING_SHORT];
static float32_value_t min_measured_value[MAX_INSTANCE_COUNT];
static float32_value_t max_measured_value[MAX_INSTANCE_COUNT];
static float32_value_t min_range_value[MAX_INSTANCE_COUNT];
static float32_value_t max_range_value[MAX_INSTANCE_COUNT];
static float32_value_t measured_particle_size[MAX_INSTANCE_COUNT];

static struct lwm2m_engine_obj temp_sensor;
static struct lwm2m_engine_obj_field fields[] = {
	OBJ_FIELD_DATA(PRTCL_SENSOR_VALUE_ID, R, FLOAT32),
	OBJ_FIELD_DATA(PRTCL_UNITS_ID, R_OPT, STRING),
	OBJ_FIELD_DATA(PRTCL_MIN_MEASURED_VALUE_ID, R_OPT, FLOAT32),
	OBJ_FIELD_DATA(PRTCL_MAX_MEASURED_VALUE_ID, R_OPT, FLOAT32),
	OBJ_FIELD_DATA(PRTCL_MIN_RANGE_VALUE_ID, R_OPT, FLOAT32),
	OBJ_FIELD_DATA(PRTCL_MAX_RANGE_VALUE_ID, R_OPT, FLOAT32),
	OBJ_FIELD_EXECUTE_OPT(PRTCL_RESET_MIN_MAX_MEASURED_VALUES_ID),
	OBJ_FIELD_DATA(PRTCL_MEASURED_PARTICLE_SIZE_ID, R, FLOAT32),
#if ADD_TIMESTAMPS
	OBJ_FIELD_DATA(PRTCL_TIMESTAMP_ID, RW_OPT, TIME),
#endif
};

static struct lwm2m_engine_obj_inst inst[MAX_INSTANCE_COUNT];
static struct lwm2m_engine_res res[MAX_INSTANCE_COUNT][PRTCL_MAX_ID];
static struct lwm2m_engine_res_inst
		res_inst[MAX_INSTANCE_COUNT][RESOURCE_INSTANCE_COUNT];

static void update_min_measured(uint16_t obj_inst_id, int index)
{
	min_measured_value[index].val1 = sensor_value[index].val1;
	min_measured_value[index].val2 = sensor_value[index].val2;
	NOTIFY_OBSERVER(IPSO_OBJECT_PRTCL_SENSOR_ID, obj_inst_id,
			PRTCL_MIN_MEASURED_VALUE_ID);
}

static void update_max_measured(uint16_t obj_inst_id, int index)
{
	max_measured_value[index].val1 = sensor_value[index].val1;
	max_measured_value[index].val2 = sensor_value[index].val2;
	NOTIFY_OBSERVER(IPSO_OBJECT_PRTCL_SENSOR_ID, obj_inst_id,
			PRTCL_MAX_MEASURED_VALUE_ID);
}

static int reset_min_max_measured_values_cb(uint16_t obj_inst_id,
					    uint8_t *args, uint16_t args_len)
{
	int i;

	LOG_DBG("RESET MIN/MAX %d", obj_inst_id);
	for (i = 0; i < MAX_INSTANCE_COUNT; i++) {
		if (inst[i].obj && inst[i].obj_inst_id == obj_inst_id) {
			update_min_measured(obj_inst_id, i);
			update_max_measured(obj_inst_id, i);
			return 0;
		}
	}

	return -ENOENT;
}

static int measured_particle_size(uint16_t obj_inst_id, int index)
{
	measured_particle_size[index].val1 = sensor_value[index].val1;
	measured_particle_size[index].val2 = sensor_value[index].val2;
	NOTIFY_OBSERVER(IPSO_OBJECT_PRTCL_SENSOR_ID, obj_inst_id,
			PRTCL_MEASURED_PARTICLE_SIZE_ID);
}

static int sensor_value_write_cb(uint16_t obj_inst_id,
				 uint16_t res_id, uint16_t res_inst_id,
				 uint8_t *data, uint16_t data_len,
				 bool last_block, size_t total_size)
{
	int i;
	bool update_min = false;
	bool update_max = false;

	for (i = 0; i < MAX_INSTANCE_COUNT; i++) {
		if (inst[i].obj && inst[i].obj_inst_id == obj_inst_id) {
			/* update min / max */
			if (sensor_value[i].val1 < min_measured_value[i].val1) {
				update_min = true;
			} else if (sensor_value[i].val1 ==
					min_measured_value[i].val1 &&
				   sensor_value[i].val2 <
					min_measured_value[i].val2) {
				update_min = true;
			}

			if (sensor_value[i].val1 > max_measured_value[i].val1) {
				update_max = true;
			} else if (sensor_value[i].val1 ==
					max_measured_value[i].val1 &&
				   sensor_value[i].val2 >
					max_measured_value[i].val2) {
				update_max = true;
			}

			if (update_min) {
				update_min_measured(obj_inst_id, i);
			}

			if (update_max) {
				update_max_measured(obj_inst_id, i);
			}
		}
	}

	return 0;
}

static struct lwm2m_engine_obj_inst *prtcl_sensor_create(uint16_t obj_inst_id)
{
	int index, i = 0, j = 0;

	/* Check that there is no other instance with this ID */
	for (index = 0; index < MAX_INSTANCE_COUNT; index++) {
		if (inst[index].obj && inst[index].obj_inst_id == obj_inst_id) {
			LOG_ERR("Can not create instance - "
				"already existing: %u", obj_inst_id);
			return NULL;
		}
	}

	for (index = 0; index < MAX_INSTANCE_COUNT; index++) {
		if (!inst[index].obj) {
			break;
		}
	}

	if (index >= MAX_INSTANCE_COUNT) {
		LOG_ERR("Can not create instance - no more room: %u",
			obj_inst_id);
		return NULL;
	}

	/* Set default values */
	sensor_value[index].val1 = 0;
	sensor_value[index].val2 = 0;
	units[index][0] = '\0';
	min_measured_value[index].val1 = INT32_MAX;
	min_measured_value[index].val2 = 0;
	max_measured_value[index].val1 = -INT32_MAX;
	max_measured_value[index].val2 = 0;
	min_range_value[index].val1 = 0;
	min_range_value[index].val2 = 0;
	max_range_value[index].val1 = 0;
	max_range_value[index].val2 = 0;
	measured_particle_size[index].val1 = 5;
	measured_particle_size[index].val2 = 0;

	(void)memset(res[index], 0,
		     sizeof(res[index][0]) * ARRAY_SIZE(res[index]));
	init_res_instance(res_inst[index], ARRAY_SIZE(res_inst[index]));

	/* initialize instance resource data */
	INIT_OBJ_RES(PRTCL_SENSOR_VALUE_ID, res[index], i,
		     res_inst[index], j, 1, false, true,
		     &sensor_value[index], sizeof(*sensor_value),
		     NULL, NULL, sensor_value_write_cb, NULL);
	INIT_OBJ_RES_DATA(PRTCL_UNITS_ID, res[index], i, res_inst[index], j,
			  units[index], PRTCL_STRING_SHORT);
	INIT_OBJ_RES_DATA(PRTCL_MIN_MEASURED_VALUE_ID, res[index], i,
			  res_inst[index], j, &min_measured_value[index],
			  sizeof(*min_measured_value));
	INIT_OBJ_RES_DATA(PRTCL_MAX_MEASURED_VALUE_ID, res[index], i,
			  res_inst[index], j, &max_measured_value[index],
			  sizeof(*max_measured_value));
	INIT_OBJ_RES_DATA(PRTCL_MIN_RANGE_VALUE_ID, res[index], i,
			  res_inst[index], j, &min_range_value[index],
			  sizeof(*min_range_value));
	INIT_OBJ_RES_DATA(PRTCL_MAX_RANGE_VALUE_ID, res[index], i,
			  res_inst[index], j, &max_range_value[index],
			  sizeof(*max_range_value));
	INIT_OBJ_RES_EXECUTE(PRTCL_RESET_MIN_MAX_MEASURED_VALUES_ID,
			     res[index], i, reset_min_max_measured_values_cb);
	INIT_OBJ_RES_DATA(PRTCL_MEASURED_PARTICLE_SIZE_ID,
			  res_inst[Index], j, &measured_particle_size[index],
			  sizeof(*measured_particle_size));
#if ADD_TIMESTAMPS
	INIT_OBJ_RES_OPTDATA(PRTCL_TIMESTAMP_ID, res[index], i,
			     res_inst[index], j);
#endif

	inst[index].resources = res[index];
	inst[index].resource_count = i;
	LOG_DBG("Create OpenLX SP Particulate Sensor instance: %d", obj_inst_id);
	return &inst[index];
}

static int openlx_sp_prtcl_sensor_init(const struct device *dev)
{
	prtcl_sensor.obj_id = OPNELX_SP_OBJECT_PRTCL_SENSOR_ID;
	prtcl_sensor.fields = fields;
	prtcl_sensor.field_count = ARRAY_SIZE(fields);
	prtcl_sensor.max_instance_count = MAX_INSTANCE_COUNT;
	prtcl_sensor.create_cb = prtcl_sensor_create;
	lwm2m_register_obj(&prtcl_sensor);

	return 0;
}

SYS_INIT(ipso_prtcl_sensor_init, APPLICATION,
	 CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
