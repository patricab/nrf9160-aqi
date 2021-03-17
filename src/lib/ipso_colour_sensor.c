/*
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2018-2019 Foundries.io
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Source material for IPSO Colour Sensor object (3335):
 * https://github.com/IPSO-Alliance/pub/blob/master/docs/IPSO-Smart-Objects.pdf
 * Section: "10. IPSO Object: Colour"
 */

#define LOG_MODULE_NAME net_ipso_colour_sensor
#define LOG_LEVEL CONFIG_LWM2M_LOG_LEVEL

#include <logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include <stdint.h>
#include <init.h>

#include "lwm2m_object.h"
#include "lwm2m_engine.h"

#ifdef CONFIG_LWM2M_IPSO_COLOUR_SENSOR_TIMESTAMP
#define ADD_TIMESTAMPS 1
#else
#define ADD_TIMESTAMPS 0
#endif

/* Server resource IDs */
#define COLOUR_VALUE_ID                         5706
#define COLOUR_UNITS_ID                         5701
#define COLOUR_APPLICATION_TYPE_ID              5750
#if ADD_TIMESTAMPS
#define COLOUR_TIMESTAMP_ID			            5518

#define COLOUR_MAX_ID		4
#else  /* !ADD_TIMESTAMPS */
#define COLOUR_MAX_ID		3
#endif

#define MAX_INSTANCE_COUNT	CONFIG_LWM2M_IPSO_COLOUR_SENSOR_INSTANCE_COUNT

#define COLOUR_STRING_SHORT	8

/*
 * Calculate resource instances as follows:
 * start with COLOUR_MAX_ID
 * subtract EXEC resources (1)
 */
#define RESOURCE_INSTANCE_COUNT	(COLOUR_MAX_ID - 1)

/* resource state variables */
static char colour_value[MAX_INSTANCE_COUNT][COLOUR_STRING_SHORT];
static char units[MAX_INSTANCE_COUNT][COLOUR_STRING_SHORT];
static char application_type[MAX_INSTANCE_COUNT][COLOUR_STRING_SHORT];

static struct lwm2m_engine_obj colour_sensor;
static struct lwm2m_engine_obj_field fields[] = {
	OBJ_FIELD_DATA(COLOUR_VALUE_ID, RW, STRING),
	OBJ_FIELD_DATA(COLOUR_UNITS_ID, R_OPT, STRING),
	OBJ_FIELD_DATA(COLOUR_APPLICATION_TYPE_ID, RW_OPT, STRING),
#if ADD_TIMESTAMPS
	OBJ_FIELD_DATA(COLOUR_TIMESTAMP_ID, RW_OPT, TIME),
#endif
};

static struct lwm2m_engine_obj_inst inst[MAX_INSTANCE_COUNT];
static struct lwm2m_engine_res res[MAX_INSTANCE_COUNT][COLOUR_MAX_ID];
static struct lwm2m_engine_res_inst
		res_inst[MAX_INSTANCE_COUNT][RESOURCE_INSTANCE_COUNT];

static void application_type(uint16_t obj_inst_id, int index)
{
	application_type[index] = sensor_value[index];
	NOTIFY_OBSERVER(IPSO_OBJECT_COLOUR_SENSOR_ID, obj_inst_id,
			COLOUR_APPLICATION_TYPE_ID);
}

static int sensor_value_write_cb(uint16_t obj_inst_id,
				 uint16_t res_id, uint16_t res_inst_id,
				 uint8_t *data, uint16_t data_len,
				 bool last_block, size_t total_size)
{
	return 0;
}

static struct lwm2m_engine_obj_inst *colour_sensor_create(uint16_t obj_inst_id)
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
	colour_value[index].val = "#000000";
	units[index][0] = '\0';
	application_type[index].val = "colour";

	(void)memset(res[index], 0,
		     sizeof(res[index][0]) * ARRAY_SIZE(res[index]));
	init_res_instance(res_inst[index], ARRAY_SIZE(res_inst[index]));

	/* initialize instance resource data */
	INIT_OBJ_RES(COLOUR_SENSOR_VALUE_ID, res[index], i,
		     res_inst[index], j, 1, false, true,
		     &sensor_value[index], sizeof(*sensor_value),
		     NULL, NULL, sensor_value_write_cb, NULL);
	INIT_OBJ_RES_DATA(COLOUR_UNITS_ID, res[index], i, res_inst[index], j,
			  units[index], COLOUR_STRING_SHORT);
	INIT_OBJ_RES_DATA(COLOUR_APPLICATION_TYPE_ID, res[index], i,
			  res_inst[index], j, &application_type[index],
			  sizeof(*application_type));
#if ADD_TIMESTAMPS
	INIT_OBJ_RES_OPTDATA(COLOUR_TIMESTAMP_ID, res[index], i,
			     res_inst[index], j);
#endif

	inst[index].resources = res[index];
	inst[index].resource_count = i;
	LOG_DBG("Create IPSO Concentration Sensor instance: %d", obj_inst_id);
	return &inst[index];
}

static int ipso_colour_sensor_init(const struct device *dev)
{
	colour_sensor.obj_id = IPSO_OBJECT_COLOUR_SENSOR_ID;
	colour_sensor.fields = fields;
	colour_sensor.field_count = ARRAY_SIZE(fields);
	colour_sensor.max_instance_count = MAX_INSTANCE_COUNT;
	colour_sensor.create_cb = colour_sensor_create;
	lwm2m_register_obj(&colour_sensor);

	return 0;
}

SYS_INIT(ipso_colour_sensor_init, APPLICATION,
	 CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
