#include "./BSP/DHT11/dht11.h"
#include "./SYSTEM/delay/delay.h"


/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	DHT11�¶�ʪ�ȴ�����c�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.8.4
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	��dht11.h�ļ�							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/			
			
//��λDHT11
void DHT11_Rst(void)	   
{                 
	DHT11_Mode(OUT); 	//SET OUTPUT
	DHT11_Low; 	      //����DQ
	delay_ms(20);    	//��������18~30ms
	DHT11_High; 			//DQ=1 
	delay_us(13);     	//��������10~35us
}

//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	DHT11_Mode(IN);//SET INPUT	 
    while (HAL_GPIO_ReadPin(DHT11_GPIO_PORT,DHT11_GPIO_PIN)&&retry<100)//DHT11������40~80us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!HAL_GPIO_ReadPin(DHT11_GPIO_PORT,DHT11_GPIO_PIN)&&retry<100)//DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

//��DHT11��ȡһ��λ
//����ֵ��1/0
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(HAL_GPIO_ReadPin(DHT11_GPIO_PORT,DHT11_GPIO_PIN)&&retry<100)//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!HAL_GPIO_ReadPin(DHT11_GPIO_PORT,DHT11_GPIO_PIN)&&retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//�ȴ�40us
	if(HAL_GPIO_ReadPin(DHT11_GPIO_PORT,DHT11_GPIO_PIN))return 1;
	else return 0;		   
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
u8 DHT11_Read_Byte(void)    
{        
	u8 i,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
		dat<<=1; 
		dat|=DHT11_Read_Bit();
	}						    
	return dat;
}

//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
	}
	else return 1;
	return 0;	    
}

//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
//����1:������
//����0:����    	 
u8 DHT11_Init(void)
{	 
 	GPIO_InitTypeDef  GPIO_InitStructure;	
 	DHT11_GPIO_CLK	 													//ʹ��PC�˿�ʱ��
	
 	GPIO_InitStructure.Pin = DHT11_GPIO_PIN;				 //PG11�˿�����
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//�������
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);				 //��ʼ��IO��
	
	

	DHT11_Rst();  //��λDHT11
	return DHT11_Check();//�ȴ�DHT11�Ļ�Ӧ
} 

void DHT11_Mode(u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(mode)
	{
		GPIO_InitStructure.Pin = DHT11_GPIO_PIN;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		GPIO_InitStructure.Pin =  DHT11_GPIO_PIN;
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	}
	HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}



