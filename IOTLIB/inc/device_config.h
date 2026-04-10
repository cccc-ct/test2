#ifndef __DEVICE_CONFIG_H
#define __DEVICE_CONFIG_H

#include <stdint.h>

typedef enum {
	CONTROL_MODE_AUTO = 0,
	CONTROL_MODE_MANUAL = 1
} control_mode_t;

typedef struct {
	control_mode_t control_mode;
	uint8_t fan_on_manual;
	uint8_t relay1_on_manual;

	float air_temp_high;
	float air_temp_low;
	float air_humidity_high;
	float air_humidity_low;
	uint32_t light_high;
	float ec_high;
	float water_temp_high;
	float water_temp_low;
	float temp_hysteresis;
} gateway_config_t;

extern gateway_config_t g_gateway_cfg;
extern uint8_t g_fan_running;
extern uint8_t g_relay1_state;

void GatewayConfig_InitDefaults(void);
void GatewayConfig_ApplyFromJson(const char *json_body);
void Gateway_ApplyThresholdControl(void);
void Gateway_ApplyManualOutputs(void);

#endif
