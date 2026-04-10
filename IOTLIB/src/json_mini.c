#include "json_mini.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static void skip_ws(const char **p)
{
	while (**p == ' ' || **p == '\t' || **p == '\r' || **p == '\n')
		(*p)++;
}

int json_mini_get_float(const char *json, const char *key, float *out)
{
	char pat[40];
	snprintf(pat, sizeof(pat), "\"%s\":", key);
	const char *p = strstr(json, pat);
	if (!p)
		return 0;
	p += strlen(pat);
	skip_ws(&p);
	return (sscanf(p, "%f", out) == 1) ? 1 : 0;
}

int json_mini_get_uint32(const char *json, const char *key, uint32_t *out)
{
	char pat[40];
	unsigned int v;
	snprintf(pat, sizeof(pat), "\"%s\":", key);
	const char *p = strstr(json, pat);
	if (!p)
		return 0;
	p += strlen(pat);
	skip_ws(&p);
	if (sscanf(p, "%u", &v) != 1)
		return 0;
	*out = (uint32_t)v;
	return 1;
}

int json_mini_get_bool(const char *json, const char *key, uint8_t *out)
{
	char pat[40];
	snprintf(pat, sizeof(pat), "\"%s\":", key);
	const char *p = strstr(json, pat);
	if (!p)
		return 0;
	p += strlen(pat);
	skip_ws(&p);
	if (strncmp(p, "true", 4) == 0) {
		*out = 1;
		return 1;
	}
	if (strncmp(p, "false", 5) == 0) {
		*out = 0;
		return 1;
	}
	if (*p == '1' && (p[1] == ',' || p[1] == '}' || p[1] == '\r' || p[1] == '\n' || isspace((unsigned char)p[1]))) {
		*out = 1;
		return 1;
	}
	if (*p == '0' && (p[1] == ',' || p[1] == '}' || p[1] == '\r' || p[1] == '\n' || isspace((unsigned char)p[1]))) {
		*out = 0;
		return 1;
	}
	return 0;
}

int json_mini_get_control_mode(const char *json, uint8_t *is_manual)
{
	if (strstr(json, "\"controlMode\":\"manual\"") || strstr(json, "\"controlMode\": \"manual\"")) {
		*is_manual = 1;
		return 1;
	}
	if (strstr(json, "\"controlMode\":\"auto\"") || strstr(json, "\"controlMode\": \"auto\"")) {
		*is_manual = 0;
		return 1;
	}
	return 0;
}
