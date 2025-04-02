#ifndef __RTC_H
#define __RTC_H
#include "stm32f10x.h"
#include "myiic.h"


#define DS1307_I2C_ADDR			0xD0
extern u8 RTC_Bcd2Bin(u8 bcd);
extern u8 RTC_Bin2Bcd(u8 bin);
extern u8 RTC_CheckMinMax(u8 val, u8 min, u8 max);
void DS1307_Init(void);
extern void RTC_GetDateTime(u8 *gio,u8 *phut,u8 *giay,u8 *thu,u8 *ngay,u8 *thang,u8 *nam) ;
extern void RTC_SetDateTime(u8 gio,u8 phut,u8 giay,u8 thu,u8 ngay,u8 thang,u8 nam);

#endif

