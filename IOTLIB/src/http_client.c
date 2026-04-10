#include "http_client.h"
#include "esp8266.h"
#include "iot_server_config.h"
#include "device_config.h"
#include <stdio.h>
#include <string.h>

static const char *extract_json_body(const char *raw)
{
	const char *p = strstr(raw, "\r\n\r\n");
	if (p) {
		p += 4;
		while (*p == ' ' || *p == '\t')
			p++;
		if (*p == '{' || *p == '[')
			return p;
	}
	return strchr(raw, '{');
}

int HttpClient_SyncConfigFromServer(void)
{
	char req[512];
	static unsigned char resp[768];
	char host[48];
	const char *body;

	snprintf(host, sizeof(host), "%s:%d", IOT_SERVER_HOST, IOT_SERVER_PORT);
	if (IOT_DEVICE_TOKEN[0] != '\0') {
		snprintf(req, sizeof(req),
			"GET " IOT_GET_CONFIG_PATH "?token=%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n",
			IOT_DEVICE_TOKEN, host);
	} else {
		snprintf(req, sizeof(req),
			"GET " IOT_GET_CONFIG_PATH " HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n",
			host);
	}

	if (!ESP8266_SendHttpRequest((unsigned char *)req, (unsigned short)strlen((const char *)req), resp, sizeof(resp)))
		return 0;
	body = extract_json_body((const char *)resp);
	if (!body)
		return 0;
	GatewayConfig_ApplyFromJson(body);
	return 1;
}
