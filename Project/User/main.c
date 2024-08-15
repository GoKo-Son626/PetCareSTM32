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

	HAL_Init();                             /* 初始化HAL库 */
	sys_stm32_clock_init(RCC_PLL_MUL9);     /* 设置时钟, 72Mhz */
	delay_init(72);                         /* 延时初始化 */
	usart_init(115200);                     /* 串口初始化为115200 */
	led_init();                             /* 初始化LED */
	lcd_init();										/* 初始化LCD */
	adc_temperature_init();						/* 初始化ADC内部温度传感器采集 */
	lsens_init();									/* 初始化光敏传感器 */
	
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
		  // 获取并显示温度值
		  temp = adc_get_temperature();   /* 得到温度值 */
        if (temp < 0)
        {
            temp = -temp;
            lcd_show_string(30 + 10 * 12, 120, 12, 12, 24, "-", BLUE);   /* 显示负号 */
        }
        else
        {
            lcd_show_string(30 + 10 * 12, 120, 12, 12, 24, "+", BLUE);   /* 无符号 */
        }
        lcd_show_xnum(30 + 11 * 12, 120, temp / 100, 2, 24, 0, BLUE);    /* 显示整数部分 */
        lcd_show_xnum(30 + 14 * 12, 120, temp % 100, 2, 24, 0X80, BLUE); /* 显示小数部分 */
		  
		  //获取并显示光敏传感器的值
		  adc_val = lsens_get_val();
        lcd_show_xnum(30 + 16 * 12, 168, adc_val, 3, 24, 0, BLUE); /* 显示ADC的值 */
        
		DHT11_Read_Data(&temp2,&humi2);//
		printf("temp %d ,humi %d\r\n",temp2,humi2);
		delay_ms(1000);
		//显示温度数据
		  lcd_show_xnum(30 + 11 * 12, 192, temp2, 2, 24, 0, BLUE);    /* 显示整数部分 */
		//lcd_show_string(30, 70, 288, 32, 32, temp2, RED);
		//显示湿度数据
		  lcd_show_xnum(30 + 11 * 12, 216, humi2, 2, 24, 0, BLUE);    /* 显示整数部分 */
		//lcd_show_string(30, 70, 288, 32, 32, humi2, RED);
	lcd_show_xnum(30 + 10 * 12, 144, humi2, 2, 24, 0, BLUE);

        LED0_TOGGLE();  /* LED0闪烁,提示程序运行 */
		  
	  if(adc_val < 35)
		{
			LED1(0);
		}else
		{
			LED1(1);
		}
	
	}
		

}
