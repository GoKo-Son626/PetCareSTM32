#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/ADC/adc3.h"
#include "./BSP/LSENS/lsens.h"
#include "./BSP/DHT11/dht11.h"


int main(void)
{
	u8 temp2;
	u8 humi2;
	short temp = 0;
	short adc_val = 0;

	HAL_Init();                             /* ��ʼ��HAL�� */
	sys_stm32_clock_init(RCC_PLL_MUL9);     /* ����ʱ��, 72Mhz */
	delay_init(72);                         /* ��ʱ��ʼ�� */
	usart_init(115200);                     /* ���ڳ�ʼ��Ϊ115200 */
	led_init();                             /* ��ʼ��LED */
	lcd_init();										/* ��ʼ��LCD */
	adc_temperature_init();						/* ��ʼ��ADC�ڲ��¶ȴ������ɼ� */
	lsens_init();									/* ��ʼ������������ */
	
	lcd_show_string(30, 70, 288, 32, 32, "PetCareSTM32 TEST", RED);
	lcd_show_string(30, 120, 200, 24, 24, "TEMPERATE: 00.00C", BLUE);
	lcd_show_string(30, 144, 200, 24, 24, "HUMIDITY: --", BLUE);
	lcd_show_string(30, 168, 240, 24, 24, "Light intensity:00%", BLUE);
	while(DHT11_Init())
	{
		printf("DHT11 Error \r\n");
		delay_ms(1000);
	}
	

	while (1)
	{
		  // ��ȡ����ʾ�¶�ֵ
		  temp = adc_get_temperature();   /* �õ��¶�ֵ */
        if (temp < 0)
        {
            temp = -temp;
            lcd_show_string(30 + 10 * 12, 120, 12, 12, 24, "-", BLUE);   /* ��ʾ���� */
        }
        else
        {
            lcd_show_string(30 + 10 * 12, 120, 12, 12, 24, "+", BLUE);   /* �޷��� */
        }
        lcd_show_xnum(30 + 11 * 12, 120, temp / 100, 2, 24, 0, BLUE);    /* ��ʾ�������� */
        lcd_show_xnum(30 + 14 * 12, 120, temp % 100, 2, 24, 0X80, BLUE); /* ��ʾС������ */
		  
		  //��ȡ����ʾ������������ֵ
		  adc_val = lsens_get_val();
        lcd_show_xnum(30 + 16 * 12, 168, adc_val, 3, 24, 0, BLUE); /* ��ʾADC��ֵ */
        
		DHT11_Read_Data(&temp2,&humi2);//
		printf("temp %d ,humi %d\r\n",temp2,humi2);
		delay_ms(1000);
		//��ʾ�¶�����
		  lcd_show_xnum(30 + 11 * 12, 192, temp2, 2, 24, 0, BLUE);    /* ��ʾ�������� */
		//lcd_show_string(30, 70, 288, 32, 32, temp2, RED);
		//��ʾʪ������
		  lcd_show_xnum(30 + 11 * 12, 216, humi2, 2, 24, 0, BLUE);    /* ��ʾ�������� */
		//lcd_show_string(30, 70, 288, 32, 32, humi2, RED);
	lcd_show_xnum(30 + 10 * 12, 144, humi2, 2, 24, 0, BLUE);

        LED0_TOGGLE();  /* LED0��˸,��ʾ�������� */
		  
	  if(adc_val < 35)
		{
			LED1(0);
		}else
		{
			LED1(1);
		}
	
	}
		

}
