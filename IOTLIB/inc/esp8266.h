#ifndef _ESP8266_H_
#define _ESP8266_H_





#define REV_OK		0	//?????????
#define REV_WAIT	1	//????¦Ä?????


_Bool ESP8266_SendCmd(char *cmd, char *res);

void ESP8266_Init(void);

void ESP8266_Clear(void);

void ESP8266_SendData(unsigned char *data, unsigned short len);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);

unsigned char ESP8266_SendHttpRequest(const unsigned char *data, unsigned short len,
	unsigned char *resp_buf, unsigned short resp_max);

#endif
