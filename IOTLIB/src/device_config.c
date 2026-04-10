#include "device_config.h"
#include "json_mini.h"
#include "l9110_fan.h"
#include "relay.h"
#include <stdint.h>
#include <string.h>

extern float h, t;
extern float ec_value, temp_value;
extern uint16_t light_lux;

gateway_config_t g_gateway_cfg;
uint8_t g_fan_running;
uint8_t g_relay1_state;

void GatewayConfig_InitDefaults(void)
{
	g_gateway_cfg.control_mode = CONTROL_MODE_AUTO;
	g_gateway_cfg.fan_on_manual = 0;
	g_gateway_cfg.relay1_on_manual = 0;

	g_gateway_cfg.air_temp_high = 32.0f;
	g_gateway_cfg.air_temp_low = 10.0f;
	g_gateway_cfg.air_humidity_high = 85.0f;
	g_gateway_cfg.air_humidity_low = 25.0f;
	g_gateway_cfg.light_high = 50000u;
	g_gateway_cfg.ec_high = 2.5f;
	g_gateway_cfg.water_temp_high = 35.0f;
	g_gateway_cfg.water_temp_low = 8.0f;
	g_gateway_cfg.temp_hysteresis = 2.0f;
}

void GatewayConfig_ApplyFromJson(const char *json_body)
{
	float f;
	uint32_t u;
	uint8_t b;
	uint8_t is_manual = 0;

	if (json_mini_get_control_mode(json_body, &is_manual))
		g_gateway_cfg.control_mode = is_manual ? CONTROL_MODE_MANUAL : CONTROL_MODE_AUTO;

	if (json_mini_get_bool(json_body, "fanOn", &b))
		g_gateway_cfg.fan_on_manual = b;
	if (json_mini_get_bool(json_body, "relay1On", &b))
		g_gateway_cfg.relay1_on_manual = b;

	if (json_mini_get_float(json_body, "airTempHigh", &f))
		g_gateway_cfg.air_temp_high = f;
	if (json_mini_get_float(json_body, "airTempLow", &f))
		g_gateway_cfg.air_temp_low = f;
	if (json_mini_get_float(json_body, "airHumidityHigh", &f))
		g_gateway_cfg.air_humidity_high = f;
	if (json_mini_get_float(json_body, "airHumidityLow", &f))
		g_gateway_cfg.air_humidity_low = f;
	if (json_mini_get_uint32(json_body, "lightHigh", &u))
		g_gateway_cfg.light_high = u;
	if (json_mini_get_float(json_body, "ecHigh", &f))
		g_gateway_cfg.ec_high = f;
	if (json_mini_get_float(json_body, "waterTempHigh", &f))
		g_gateway_cfg.water_temp_high = f;
	if (json_mini_get_float(json_body, "waterTempLow", &f))
		g_gateway_cfg.water_temp_low = f;
	if (json_mini_get_float(json_body, "tempHysteresis", &f))
		g_gateway_cfg.temp_hysteresis = f;
}

void Gateway_ApplyManualOutputs(void)
{
	if (g_gateway_cfg.control_mode != CONTROL_MODE_MANUAL)
		return;

	if (g_gateway_cfg.fan_on_manual) {
		L9110_Fan_Forward();
		g_fan_running = 1;
	} else {
		L9110_Fan_Stop();
		g_fan_running = 0;
	}
	RELAY_1(g_gateway_cfg.relay1_on_manual);
	g_relay1_state = g_gateway_cfg.relay1_on_manual;
}

void Gateway_ApplyThresholdControl(void)
{
	float hyst;
	int need_fan;
	int allow_off;

	if (g_gateway_cfg.control_mode != CONTROL_MODE_AUTO)
		return;

	hyst = g_gateway_cfg.temp_hysteresis;

	need_fan = (t > g_gateway_cfg.air_temp_high) ||
		(h > g_gateway_cfg.air_humidity_high) ||
		((uint32_t)light_lux > g_gateway_cfg.light_high) ||
		(ec_value > g_gateway_cfg.ec_high) ||
		(temp_value > g_gateway_cfg.water_temp_high) ||
		(temp_value < g_gateway_cfg.water_temp_low);

	allow_off = (t < g_gateway_cfg.air_temp_high - hyst) &&
		(h < g_gateway_cfg.air_humidity_high - 5.0f) &&
		((uint32_t)light_lux + 1000u < g_gateway_cfg.light_high) &&
		(ec_value < g_gateway_cfg.ec_high - 0.1f) &&
		(temp_value < g_gateway_cfg.water_temp_high - 1.0f) &&
		(temp_value > g_gateway_cfg.water_temp_low + 1.0f);

	if (need_fan) {
		L9110_Fan_Forward();
		g_fan_running = 1;
	} else if (allow_off) {
		L9110_Fan_Stop();
		g_fan_running = 0;
	}

	if (ec_value > g_gateway_cfg.ec_high) {
		RELAY_1(1);
		g_relay1_state = 1;
	} else if (ec_value < g_gateway_cfg.ec_high - 0.05f) {
		RELAY_1(0);
		g_relay1_state = 0;
	}
}
