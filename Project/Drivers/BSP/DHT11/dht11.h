#ifndef _DHT11_H_
#define _DHT11_H_


#include "./SYSTEM/sys/sys.h"


/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	DHT11�¶�ʪ�ȴ�����h�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.8.4
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/


/***************�����Լ��������****************/
//DHT11���ź궨��
#define DHT11_GPIO_PORT  GPIOC
#define DHT11_GPIO_PIN   GPIO_PIN_2
#define DHT11_GPIO_CLK   __HAL_RCC_GPIOC_CLK_ENABLE();
/*********************END**********************/

//���״̬����
#define OUT 1
#define IN  0

//����DHT11��������ߵ͵�ƽ
#define DHT11_High HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET);
#define DHT11_Low HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_RESET);


typedef unsigned char u8;


u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ������
u8 DHT11_Read_Byte(void);//��ȡһ���ֽڵ�����
u8 DHT11_Read_Bit(void);//��ȡһλ������
void DHT11_Mode(u8 mode);//DHT11�������ģʽ����
u8 DHT11_Check(void);//���DHT11
void DHT11_Rst(void);//��λDHT11   

#endif
