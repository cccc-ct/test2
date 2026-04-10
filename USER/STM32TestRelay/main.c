#include "stm32f10x.h" //STM32XH
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "relay.h"
#include "l9110_fan.h"
#include "WS2812B.h"
#include "DHT11.h"
#include "ec_sensor.h"
#include "ds18b20.h"
#include "BH1750.h"
#include "esp8266.h"
#include "iot_server_config.h"
#include "device_config.h"
#include "http_client.h"
#include <stdio.h>
#include <string.h>

extern u8 dat[5];
extern u8 flag;
extern volatile u8 usart1_rx;
extern volatile u8 usart1_rx_flag;
u8 a;
uint8_t bh1750_init_ok = 0;  // BH1750 init status
float h=0.0f, t=0.0f;
u32 dht_cnt = 0;  // DHT11 read counter
float ec_value = 0.0f;  // EC value
float temp_value = 0.0f;  // Water temperature
u32 ec_temp_cnt = 0;  // EC/temp update counter
uint16_t light_lux = 0;	// Light (lux)
u32 upload_cnt = 0;          // Upload counter
u32 sync_cnt = 0;            // Config sync counter
#define UPLOAD_INTERVAL 5000 // 5 seconds upload interval
#define SYNC_INTERVAL   15000 // 15 seconds config sync interval

static void ApplyGatewayOutputs(void)
{
	if (g_gateway_cfg.control_mode == CONTROL_MODE_AUTO)
		Gateway_ApplyThresholdControl();
	else
		Gateway_ApplyManualOutputs();
}

// System clock init
void System_Clock_Init(void)
{
    // Enable peripheral clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | 
                          RCC_APB2Periph_GPIOB |
                          RCC_APB2Periph_GPIOC |
                          RCC_APB2Periph_AFIO |
                          RCC_APB2Periph_USART1, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
    // Optional: disable JTAG pins
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

void Build_HTTP_POST_Data(char *post_buf)
{
    char json_buf[256] = {0};
    char host[48];
    snprintf(host, sizeof(host), "%s:%d", IOT_SERVER_HOST, IOT_SERVER_PORT);
		sprintf(json_buf,
			"{\"airTemperature\":%.1f,\"airHumidity\":%.1f,\"lightIntensity\":%d,\"ecValue\":%.2f,\"waterTemperature\":%.2f,\"controlMode\":\"%s\",\"fanOn\":%d,\"relay1On\":%d}",
			t, h, light_lux, ec_value, temp_value,
			g_gateway_cfg.control_mode == CONTROL_MODE_MANUAL ? "manual" : "auto",
			(int)g_fan_running, (int)g_relay1_state);

    sprintf(post_buf, "POST " IOT_POST_PATH " HTTP/1.1\r\nHost: %s\r\nContent-Type:application/json\r\nContent-Length:%d\r\n\r\n%s",
            host, (int)strlen(json_buf), json_buf);
}

// Check TCP connection and auto-reconnect
_Bool ESP8266_Check_Conn(void)
{
	ESP8266_Clear();
	// Check TCP status: CONNECT=online, CLOSED=disconnected
	if(ESP8266_SendCmd("AT+CIPSTATUS\r\n", "CONNECT"))
	{
		USART1_printf("TCP disconnected, reconnecting...\r\n");
		// Re-init ESP8266 and rebuild WiFi/TCP connection
		ESP8266_Init();
		return 0;
	}
	return 1; // TCP connected
}
int main(void){

	char post_buf[400] = {0}; // JSON buffer
	System_Clock_Init();
	USART1_Init(115200);
	USART_ClearFlag(USART1, USART_FLAG_TC);  // Clear TX complete flag
	USART_ClearFlag(USART1, USART_FLAG_RXNE); // Clear RX flag
	// Init EC sensor
  EC_Sensor_Init();
	// Init DS18B20
  DS18B20_Init(); 
	RELAY_Init();
	L9110_Fan_Init();
	WS2812B_Init();
	BH1750_Init();
	USART2_Init(9600);
	ESP8266_Init();
	if (HttpClient_SyncConfigFromServer()) {
		ApplyGatewayOutputs();
		USART1_printf("Config sync OK\r\n");
	} else
		USART1_printf("Config sync failed (using defaults)\r\n");
	USART1_printf("ESP8266 init done, start sensing...\r\n\r\n");
	while(1){
			// Read EC/temp periodically (ec_temp_cnt)
        if(++ec_temp_cnt >= 2000)
        {
            ec_temp_cnt = 0;
            
            // Read DS18B20 temperature
            temp_value = DS18B20_Get_Temp();
            
            // Update EC with temp compensation
            EC_Sensor_Update(temp_value);
            
            // Get computed EC value
            ec_value = EC_Sensor_GetECValue();
						// Read light intensity
						light_lux=Light_Intensity();
						if(DHT_Read())
						{
						h = dat[0] + dat[1] / 10.0;
            t = dat[2] + dat[3] / 10.0;
						}
						USART1_printf("Temp: %.1f C, Humidity: %.1f\r\n", t, h);
						// Light intensity
						USART1_printf("Light: %d lux\r\n", light_lux);
            // EC and water temperature
            USART1_printf("EC: %.2f mS/cm, WaterTemp: %.2f C\r\n", ec_value, temp_value);

						ApplyGatewayOutputs();
        }
			if(++upload_cnt >= UPLOAD_INTERVAL)
			{
				upload_cnt = 0;
				Build_HTTP_POST_Data(post_buf); // 1. Build JSON
				if (!ESP8266_Check_Conn()) {
					if (HttpClient_SyncConfigFromServer())
						ApplyGatewayOutputs();
				}
				ESP8266_SendData((unsigned char *)post_buf, strlen(post_buf)); // 2. Send HTTP POST
				USART1_printf("Upload OK: %s\r\n\r\n", post_buf);
			}
			if (++sync_cnt >= SYNC_INTERVAL)
			{
				sync_cnt = 0;
				ESP8266_Check_Conn();
				if (HttpClient_SyncConfigFromServer()) {
					ApplyGatewayOutputs();
					USART1_printf("Config synced\r\n");
				}
			}
			delay_ms(1);  // 1 ms delay
	}
}
