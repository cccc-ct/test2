/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	tcpserver.c
	*
	*	作者： 	
	*
	*	日期： 		2022-11-19
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"
//网络设备
#include "esp8266.h"
//协议文件
#include "tcpserver.h"
#include "mqttkit.h"
//硬件驱动
#include "usart.h"
#include "delay.h"
//#include "sht20.h"
#include "led.h"
//C库
#include <string.h>
#include <stdio.h>
//json库
#include"cjson.h"
#define DEVID		"975051928"//设备ID
extern u8 dat[5];

unsigned char OneNet_FillBuf(char *buf)
{
	char text[29];
	memset(text, 0, sizeof(text));
	sprintf(text, "Humidity,%d.%d;",dat[0],dat[1]);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text, "Tempreture,%d.%d;",dat[2],dat[3]);
	strcat(buf, text);
	printf("buf_mqtt=%s\r\n",buf);
	return strlen(buf);

}

//==========================================================
//	函数名称：	TCPServer_SendData
//
//	函数功能：	上传数据到NetAssist TCP Server
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void TCPServer_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	char buf[29];
	short body_len = 0, i = 0;
	memset(buf, 0, sizeof(buf));//清空数组内容
	body_len = OneNet_FillBuf(buf);		//获取当前需要发送的数据流的总长度
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			printf( "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			printf(  "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}
