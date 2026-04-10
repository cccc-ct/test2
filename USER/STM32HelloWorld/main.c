#include "stm32f10x.h"
#include <stdio.h>
#include <usart.h>

int main(void)
{

	delay_ms(3000);
	USART1_Init(9600);
	while(1){
	USART1_printf("20220865206 ≥¬’‹”Ó \r\n");
		delay_ms(3000);
	}
}
	