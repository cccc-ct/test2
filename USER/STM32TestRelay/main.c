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

extern u8 dat[5];
extern u8 flag;
extern volatile u8 usart1_rx;
extern volatile u8 usart1_rx_flag;
u8 a;
uint8_t bh1750_init_ok = 0;  // BH1750初始化状态
float h=0.0f, t=0.0f;
u32 dht_cnt = 0;  // DHT11 读取计数器
float ec_value = 0.0f;  // EC值
float temp_value = 0.0f;  // 温度值
u32 ec_temp_cnt = 0;  // EC和温度读取计数器
uint16_t light_lux = 0;	//光照值
u32 upload_cnt = 0;          // 数据上传计数器
u32 req_state_cnt = 0;       // 状态请求计数器
#define UPLOAD_INTERVAL 5000 // 5秒上传一次数据
#define REQ_STATE_INTERVAL 3000 // 3秒请求一次设备状态
#define POST_URL      "/IOT"
#define GET_STATE_URL "/dev-api/device/control/relay_state" // 获取继电器状态的接口
#define HOST_ADDR     "localhost:8080"
// 继电器状态结构体（存储从服务器获取的目标状态）
typedef struct {
    u8 relayLight; // 0-关，1-开
    u8 relayWater;
    u8 relayNutrient;
    u8 relayHumidifier;
    u8 relayFan;
} Relay_State_TypeDef;
Relay_State_TypeDef relay_target_state = {0}; // 初始化全关


// 统一时钟初始化函数
void System_Clock_Init(void)
{
    // 使能所有需要的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | 
                          RCC_APB2Periph_GPIOB |
                          RCC_APB2Periph_GPIOC |
                          RCC_APB2Periph_AFIO |
                          RCC_APB2Periph_USART1, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
    // 如果需要，禁用JTAG释放引脚
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

void Build_HTTP_POST_Data(char *post_buf)
{
    // 第一步：先拼接JSON纯数据
    char json_buf[150] = {0};
		// STM32里的JSON拼接行，直接替换这行即可
		sprintf(json_buf, "{\"airTemperature\":%.1f,\"airHumidity\":%.1f,\"lightIntensity\":%d,\"ecValue\":%.2f,\"waterTemperature\":%.2f}",t, h, light_lux, ec_value, temp_value);

    // 第二步：拼接完整的HTTP POST请求头 + JSON数据 
    sprintf(post_buf, "POST %s HTTP/1.1\r\nHost:%s\r\nContent-Type:application/json\r\nContent-Length:%d\r\n\r\n%s",
            POST_URL, HOST_ADDR, strlen(json_buf), json_buf);
}

// 构建HTTP GET请求数据（新增）
void Build_HTTP_GET_Data(char *get_buf)
{
    sprintf(get_buf, "GET %s HTTP/1.1\r\nHost:%s\r\nConnection: keep-alive\r\n\r\n",
            GET_STATE_URL, HOST_ADDR);
}

// 解析服务器返回的继电器状态JSON（新增）
// 假设返回格式：{"relay1":1,"relay2":0,"relay3":1,"relay4":0,"relay5":1}
void Parse_Relay_State(char *recv_buf)
{
    // 定位各继电器状态字段
    char *p_relay1 = strstr(recv_buf, "\"relayLight\":");
    char *p_relay2 = strstr(recv_buf, "\"relayWater\":");
    char *p_relay3 = strstr(recv_buf, "\"relayNutrient\":");
    char *p_relay4 = strstr(recv_buf, "\"relayHumidifier\":");
    char *p_relay5 = strstr(recv_buf, "\"relayFan\":");
    
    if(p_relay1) relay_target_state.relayLight = atoi(p_relay1 + 13); 
    if(p_relay2) relay_target_state.relayWater = atoi(p_relay2 + 13);
    if(p_relay3) relay_target_state.relayNutrient = atoi(p_relay3 + 16);
    if(p_relay4) relay_target_state.relayHumidifier = atoi(p_relay4 + 18);
    if(p_relay5) relay_target_state.relayFan = atoi(p_relay5 + 11);
    
    // 打印解析结果（调试用）
    USART1_printf("解析继电器状态：RELAY1=%d, RELAY2=%d, RELAY3=%d, RELAY4=%d, RELAY5=%d\r\n",
                 relay_target_state.relayLight, relay_target_state.relayWater,
                 relay_target_state.relayNutrient, relay_target_state.relayHumidifier,
                 relay_target_state.relayFan);
}

// 执行继电器控制（新增）
void Execute_Relay_Control(void)
{
		if(relay_target_state.relayLight!=2)
		{
			if(relay_target_state.relayLight==1){
				WS2812B_All_White(30);
				WS2812B_RefreshPixel();
			}
			RELAY_1(relay_target_state.relayLight);
		}
		if(relay_target_state.relayWater!=2)
		{
			RELAY_2(relay_target_state.relayWater);
		}
		if(relay_target_state.relayNutrient!=2)
		{
			RELAY_3(relay_target_state.relayNutrient);
		}
		if(relay_target_state.relayHumidifier!=2)
		{
			RELAY_4(relay_target_state.relayHumidifier);
		}
		if(relay_target_state.relayFan!=2)
		{
			if(relay_target_state.relayFan==1)
			{
				L9110_Fan_Reverse();
			}
			RELAY_5(relay_target_state.relayFan);
    }
    USART1_printf("继电器控制完成\r\n");
}



// 新增：检测TCP连接状态 + 自动重连函数
_Bool ESP8266_Check_Conn(void)
{
	ESP8266_Clear();
	// 发送查询连接状态指令，返回CONNECT则在线，返回CLOSED则断开
	if(ESP8266_SendCmd("AT+CIPSTATUS\r\n", "CONNECT"))
	{
		USART1_printf("TCP连接已断开，开始重连...\r\n");
		// 重新完整初始化ESP+重建WiFi+TCP连接
		ESP8266_Init();
		return 0;
	}
	return 1; //连接正常
}


// 新增：ESP8266发送GET请求并接收返回数据
u8 ESP8266_Send_GET_Request(char *get_buf, char *recv_buf, u16 recv_buf_len)
{
    u8 res = 0;
    ESP8266_Clear();
    
    // 1. 检查TCP连接
    if(!ESP8266_Check_Conn())
    {
        USART1_printf("TCP重连失败，GET请求终止\r\n");
        return 0;
    }
    
    // 3. 发送GET请求数据
    Usart_SendString(USART2, (unsigned char *)get_buf, strlen(get_buf));
    delay_ms(100); // 等待发送完成
    
    // 4. 接收服务器返回数据（需根据ESP8266驱动调整，此处假设recv_buf用于存储返回数据）
    // 注：实际项目中需根据ESP8266的接收逻辑实现，比如通过USART2中断接收
    // 这里简化处理，假设ESP8266_SendCmd的接收缓冲区可以复用
    res = ESP8266_ReceiveData(recv_buf, recv_buf_len); // 需确保ESP8266驱动有该函数
    
    delay_ms(100);
    
    return res;
}

int main(void){

	char post_buf[400] = {0}; // 存储JSON数据
	char get_buf[200] = {0};  // 存储GET请求数据
	char recv_buf[500] = {0}; // 存储服务器返回数据
	System_Clock_Init();
	USART1_Init(115200);
	USART_ClearFlag(USART1, USART_FLAG_TC);  //清空发送完成标志位
	USART_ClearFlag(USART1, USART_FLAG_RXNE); //清空接收数据标志位
	// 初始化EC传感器
  EC_Sensor_Init();
	// 初始化DS18B20
  DS18B20_Init(); 
	RELAY_Init();
	L9110_Fan_Init();
	WS2812B_Init();
	BH1750_Init();
	USART2_Init(9600);
	ESP8266_Init();
	USART1_printf("ESP8266初始化完成，传感器采集开始！\r\n\r\n");
	while(1){
			// EC值和温度读取（每秒更新一次）
        if(++ec_temp_cnt >= 2000)
        {
            ec_temp_cnt = 0;
            
            // 读取DS18B20温度
            temp_value = DS18B20_Get_Temp();
            
            // 更新EC值（自动进行温度补偿）
            EC_Sensor_Update(temp_value);
            
            // 获取计算后的EC值
            ec_value = EC_Sensor_GetECValue();
						// 读取光照强度
						light_lux=Light_Intensity();
						if(DHT_Read())
						{
						h = dat[0] + dat[1] / 10.0;
            t = dat[2] + dat[3] / 10.0;
						}
						USART1_printf("温度：%.1f C，湿度：%.1f  \r\n", t,h);
						// 串口打印光照值，格式：光照强度：xxx lux
						USART1_printf("光照强度：%d lux\r\n", light_lux);
            // 可以通过串口打印EC值和温度值
            USART1_printf("EC: %.2f mS/cm, 水温: %.2f C\r\n", ec_value, temp_value);
        }
			if(++upload_cnt >= UPLOAD_INTERVAL)
			{
				upload_cnt = 0;
				Build_HTTP_POST_Data(post_buf); // 1. 构建JSON数据
				ESP8266_Check_Conn();            //检测连接，断开则自动重连
				ESP8266_SendData((unsigned char *)post_buf, strlen(post_buf)); // 2. 调用驱动发送数据
				USART1_printf("数据上传成功：%s\r\n\r\n", post_buf);
			}
			// 3. 定时请求继电器状态并控制（3秒一次）
		if(++req_state_cnt >= REQ_STATE_INTERVAL)
		{
			req_state_cnt = 0;
			USART1_printf("请求继电器状态...\r\n");
			
			// 构建GET请求
			Build_HTTP_GET_Data(get_buf);
			
			// 发送GET请求并接收返回数据
			memset(recv_buf, 0, sizeof(recv_buf)); // 清空接收缓冲区
			if(ESP8266_Send_GET_Request(get_buf, recv_buf, sizeof(recv_buf)))
			{
				USART1_printf("状态请求成功，返回数据：%s\r\n", recv_buf);
				
				// 解析继电器状态
				Parse_Relay_State(recv_buf);
				
				// 执行继电器控制
				Execute_Relay_Control();
			}
			else
			{
				USART1_printf("状态请求失败\r\n");
			}
		}

			delay_ms(1);  // 每次循环延时1ms
	}
}