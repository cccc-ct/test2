#ifndef __JSON_MINI_H
#define __JSON_MINI_H

#include <stdint.h>

int json_mini_get_float(const char *json, const char *key, float *out);
int json_mini_get_uint32(const char *json, const char *key, uint32_t *out);
int json_mini_get_bool(const char *json, const char *key, uint8_t *out);
int json_mini_get_control_mode(const char *json, uint8_t *is_manual);

#endif
