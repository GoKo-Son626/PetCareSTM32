/**
 ****************************************************************************************************
 * @file        lsens.c

 * @brief       光敏传感器 驱动代码

 ****************************************************************************************************
 */

#include "./BSP/ADC/adc3.h"
#include "./BSP/LSENS/lsens.h"


/**
 * @brief       初始化光敏传感器
 * @param       无
 * @retval      无
 */
void lsens_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    LSENS_ADC3_CHX_GPIO_CLK_ENABLE();   /* IO口时钟使能 */

    /* 设置AD采集通道对应IO引脚工作模式 */
    gpio_init_struct.Pin = LSENS_ADC3_CHX_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(LSENS_ADC3_CHX_GPIO_PORT, &gpio_init_struct);

    adc3_init();                       /* 初始化ADC */
}

/**
 * @brief       读取光敏传感器值
 * @param       无
 * @retval      0~100:0,最暗;100,最亮
 */
uint8_t lsens_get_val(void)
{
    uint32_t temp_val = 0;
    temp_val = adc3_get_result_average(LSENS_ADC3_CHX, 10);  /* 读取平均值 */
    temp_val /= 40;

    if (temp_val > 100)temp_val = 100;

    return (uint8_t)(100 - temp_val);
}












