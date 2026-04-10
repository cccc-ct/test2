#ifndef __IOT_SERVER_CONFIG_H
#define __IOT_SERVER_CONFIG_H

/* Keep consistent with AT+CIPSTART/HTTP Host: set LAN/WAN reachable IP/domain */
#define IOT_SERVER_HOST     "192.168.43.185"
#define IOT_SERVER_PORT     8080

#define IOT_POST_PATH       "/IOT"
#define IOT_GET_CONFIG_PATH "/api/device/config"

/* Optional: device auth, appended to GET query string, e.g. ?token=xxx */
#define IOT_DEVICE_TOKEN    ""

/*
 * Example GET response JSON (all fields optional; gateway uses defaults):
 * {"controlMode":"auto","fanOn":false,"relay1On":false,
 *  "airTempHigh":32,"airTempLow":10,"airHumidityHigh":85,"airHumidityLow":25,
 *  "lightHigh":50000,"ecHigh":2.5,"waterTempHigh":35,"waterTempLow":8,"tempHysteresis":2}
 */

#endif
