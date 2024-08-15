#ifndef _DHT11_H_
#define _DHT11_H_


#include "./SYSTEM/sys/sys.h"


/*****************辰哥单片机设计******************
											STM32
 * 文件			:	DHT11温度湿度传感器h文件                   
 * 版本			: V1.0
 * 日期			: 2024.8.4
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/


/***************根据自己需求更改****************/
//DHT11引脚宏定义
#define DHT11_GPIO_PORT  GPIOC
#define DHT11_GPIO_PIN   GPIO_PIN_2
#define DHT11_GPIO_CLK   __HAL_RCC_GPIOC_CLK_ENABLE();
/*********************END**********************/

//输出状态定义
#define OUT 1
#define IN  0

//控制DHT11引脚输出高低电平
#define DHT11_High HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET);
#define DHT11_Low HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_RESET);


typedef unsigned char u8;


u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度数据
u8 DHT11_Read_Byte(void);//读取一个字节的数据
u8 DHT11_Read_Bit(void);//读取一位的数据
void DHT11_Mode(u8 mode);//DHT11引脚输出模式控制
u8 DHT11_Check(void);//检测DHT11
void DHT11_Rst(void);//复位DHT11   

#endif
