#include "stm32f10x.h"
#include "LED.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


#include "myiic.h" 
#include "ds1307.h"

u8 scans_IIC( u8 add)	{
	
	IIC_Start();	
	IIC_Send_Byte(add|0);
	if(IIC_Wait_Ack()) { IIC_Stop(); return 1;}
	IIC_Stop();
	return 0;
}
 int main(void)
 {	
	 u8 buffer[2]={0,0};
	 u16 temp=0;
	 float LUX=0;
	 delay_init();
	 pinMode(PB5,OUTPUT);
	 pinMode(PB6,INPUT);
	 
	 USARTx_Init(USART2, Pins_PA2PA3, 115200);
	 printf("abc\r\n");
	 DS1307_Init();
	 delay_ms(1300);
  while(1)
	{
		
		printf("scanning...\r\n");
		IIC_Write(0x23<<1,0x10,0x00); delay_ms(200);
		IIC_ReadMulti(0x46,0x00,2,buffer);
		temp=(u16)(buffer[0]<<8)|buffer[1];
		LUX=temp/1.2;
		printf("LUX= %4.2f\r\n",LUX);
	}
 }
