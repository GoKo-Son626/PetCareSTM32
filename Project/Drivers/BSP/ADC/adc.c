/**
 ****************************************************************************************************
 * @file        adc.c

 * @brief       ADC ��������

 ****************************************************************************************************
 */

#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"


ADC_HandleTypeDef g_adc_handle;   /* ADC��� */

/**
 * @brief       ADC��ʼ������
 *   @note      ������֧��ADC1/ADC2����ͨ��, ���ǲ�֧��ADC3
 *              ����ʹ��12λ����, ADC����ʱ��=12M, ת��ʱ��Ϊ: �������� + 12.5��ADC����
 *              ��������������: 239.5, ��ת��ʱ�� = 252 ��ADC���� = 21us
 * @param       ��
 * @retval      ��
 */
void adc_init(void)
{
    g_adc_handle.Instance = ADC_ADCX;                        /* ѡ���ĸ�ADC */
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;       /* ���ݶ��뷽ʽ���Ҷ��� */
    g_adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;       /* ��ɨ��ģʽ�����õ�һ��ͨ�� */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;          /* �ر�����ת��ģʽ */
    g_adc_handle.Init.NbrOfConversion = 1;                   /* ��ֵ��Χ��1~16����ʵ���õ�1������ͨ������ */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;       /* ��ֹ����ͨ������ģʽ */
    g_adc_handle.Init.NbrOfDiscConversion = 0;               /* ���ü��ģʽ�Ĺ���ͨ����������ֹ����ͨ������ģʽ�󣬴˲������� */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ����ת����ʽ��������� */
    HAL_ADC_Init(&g_adc_handle);                             /* ��ʼ�� */

    HAL_ADCEx_Calibration_Start(&g_adc_handle);              /* У׼ADC */
}

/**
 * @brief       ADC�ײ��������������ã�ʱ��ʹ��
                �˺����ᱻHAL_ADC_Init()����
 * @param       hadc:ADC���
 * @retval      ��
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC_ADCX)
    {
        GPIO_InitTypeDef gpio_init_struct;
        RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
        
        ADC_ADCX_CHY_CLK_ENABLE();                                /* ʹ��ADCxʱ�� */
        ADC_ADCX_CHY_GPIO_CLK_ENABLE();                           /* ����GPIOʱ�� */

        /* ����ADCʱ�� */
        adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;    /* ADC����ʱ�� */
        adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;       /* ��Ƶ����6ʱ��Ϊ72M/6=12MHz */
        HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                 /* ����ADCʱ�� */

        /* ����AD�ɼ�ͨ����ӦIO���Ź���ģʽ */
        gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;             /* ADCͨ��IO���� */
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;                 /* ģ�� */
        HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief       ����ADCͨ������ʱ��
 * @param       adcx : adc���ָ��,ADC_HandleTypeDef
 * @param       ch   : ͨ����, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: ����ʱ��  0~7, ��Ӧ��ϵΪ:
 *   @arg       ADC_SAMPLETIME_1CYCLE_5, 1.5��ADCʱ������        ADC_SAMPLETIME_7CYCLES_5, 7.5��ADCʱ������
 *   @arg       ADC_SAMPLETIME_13CYCLES_5, 13.5��ADCʱ������     ADC_SAMPLETIME_28CYCLES_5, 28.5��ADCʱ������
 *   @arg       ADC_SAMPLETIME_41CYCLES_5, 41.5��ADCʱ������     ADC_SAMPLETIME_55CYCLES_5, 55.5��ADCʱ������
 *   @arg       ADC_SAMPLETIME_71CYCLES_5, 71.5��ADCʱ������     ADC_SAMPLETIME_239CYCLES_5, 239.5��ADCʱ������
 * @param       rank: ��ͨ���ɼ�ʱ��Ҫ���õĲɼ����,
                �����㶨��channle1��rank=1��channle2 ��rank=2��
                ��ô��Ӧ����DMA����ռ�ı�������AdcDMA[0] ��i��channle1��ת�������AdcDMA[1]����ͨ��2��ת������� 
                ��ͨ��DMA����Ϊ ADC_REGULAR_RANK_1
 *   @arg       ���1~16��ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      ��
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    ADC_ChannelConfTypeDef adc_ch_conf;
    
    adc_ch_conf.Channel = ch;                            /* ͨ�� */
    adc_ch_conf.Rank = rank;                             /* ���� */
    adc_ch_conf.SamplingTime = stime;                    /* ����ʱ�� */
    HAL_ADC_ConfigChannel(adc_handle, &adc_ch_conf);     /* ͨ������ */
}

/**
 * @brief       ���ADCת����Ľ��
 * @param       ch: ͨ��ֵ 0~17��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_17
 * @retval      ��
 */
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&g_adc_handle , ch, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);    /* ����ͨ�������кͲ���ʱ�� */

    HAL_ADC_Start(&g_adc_handle);                            /* ����ADC */
    HAL_ADC_PollForConversion(&g_adc_handle, 10);            /* ��ѯת�� */
    return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);        /* �������һ��ADC1�������ת����� */
}

/**
 * @brief       ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ��
 * @param       ch      : ͨ����, 0~17
 * @param       times   : ��ȡ����
 * @retval      ͨ��ch��times��ת�����ƽ��ֵ
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)     /* ��ȡtimes������ */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;        /* ����ƽ��ֵ */
}

/*********************************************�ڲ��¶ȴ�����ʵ�����***************************************************/

/**
 * @brief       ADC �ڲ��¶ȴ����� ��ʼ������
 *   @note      ����������ʹ��adc_init��ADC���д󲿷�����,�в���ĵط��ٵ�������
 *              ע��: STM32F103�ڲ��¶ȴ�����ֻ������ADC1��ͨ��16��, ����ADC�޷�����ת��.
 *
 * @param       ��
 * @retval      ��
 */
void adc_temperature_init(void)
{
    adc_init();                                            /* �ȳ�ʼ��ADC */
    SET_BIT(g_adc_handle.Instance->CR2, ADC_CR2_TSVREFE);  /* TSVREFE = 1, �����ڲ��¶ȴ�������Vrefint */
}

/**
 * @brief       ��ȡ�ڲ��¶ȴ������¶�ֵ
 * @param       ��
 * @retval      �¶�ֵ(������100��,��λ:��.)
 */
short adc_get_temperature(void)
{
    uint32_t adcx;
    short result;
    double temperature;

    adcx = adc_get_result_average(ADC_TEMPSENSOR_CHX, 20);  /* ��ȡ�ڲ��¶ȴ�����ͨ��,10��ȡƽ�� */
    temperature = (float)adcx * (3.3 / 4096);               /* ת��Ϊ��ѹֵ */
    temperature = (1.43 - temperature) / 0.0043 + 25;       /* �����¶� */
    result = temperature *= 100;                            /* ����100��. */
    return result;
}


///***************************************��ͨ��ADC�ɼ�(DMA��ȡ)ʵ�����*****************************************/

//DMA_HandleTypeDef g_dma_adc_handle = {0};                                   /* ����Ҫ����ADC���ݵ�DMA��� */
//ADC_HandleTypeDef g_adc_dma_handle = {0};                                   /* ����ADC��DMA��ȡ����� */
//uint8_t g_adc_dma_sta = 0;                                                  /* DMA����״̬��־, 0,δ���; 1, ����� */

///**
// * @brief       ADC DMA��ȡ ��ʼ������
// *   @note      ����������ʹ��adc_init��ADC���д󲿷�����,�в���ĵط��ٵ�������
// * @param       par         : �����ַ
// * @param       mar         : �洢����ַ
// * @retval      ��
// */
//void adc_dma_init(uint32_t mar)
//{
//    GPIO_InitTypeDef gpio_init_struct;
//    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
//    ADC_ChannelConfTypeDef adc_ch_conf = {0};

//    ADC_ADCX_CHY_CLK_ENABLE();                                              /* ʹ��ADCxʱ�� */
//    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                                         /* ����GPIOʱ�� */

//    if ((uint32_t)ADC_ADCX_DMACx > (uint32_t)DMA1_Channel7)                 /* ����DMA1_Channel7, ��ΪDMA2��ͨ���� */
//    {
//        __HAL_RCC_DMA2_CLK_ENABLE();                                        /* DMA2ʱ��ʹ�� */
//    }
//    else
//    {
//        __HAL_RCC_DMA1_CLK_ENABLE();                                        /* DMA1ʱ��ʹ�� */
//    }

//    /* ����ADCʱ�� */
//    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;                  /* ADC����ʱ�� */
//    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;                     /* ��Ƶ����6ʱ��Ϊ72M/6=12MHz */
//    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                               /* ����ADCʱ�� */

//    /* ����AD�ɼ�ͨ����ӦIO���Ź���ģʽ */
//    gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;                           /* ADCͨ����Ӧ��IO���� */
//    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                               /* ģ�� */
//    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);

//    /* ��ʼ��DMA */
//    g_dma_adc_handle.Instance = ADC_ADCX_DMACx;                             /* ����DMAͨ�� */
//    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* �����赽�洢��ģʽ */
//    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* ���������ģʽ */
//    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* �洢������ģʽ */
//    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* �������ݳ���:16λ */
//    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* �洢�����ݳ���:16λ */
//    g_dma_adc_handle.Init.Mode = DMA_NORMAL;                                /* ��������ģʽ */
//    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* �е����ȼ� */
//    HAL_DMA_Init(&g_dma_adc_handle);

//    __HAL_LINKDMA(&g_adc_dma_handle, DMA_Handle, g_dma_adc_handle);         /* ��DMA��adc��ϵ���� */

//    g_adc_dma_handle.Instance = ADC_ADCX;                                   /* ѡ���ĸ�ADC */
//    g_adc_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                  /* ���ݶ��뷽ʽ���Ҷ��� */
//    g_adc_dma_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                  /* ��ɨ��ģʽ�����õ�һ��ͨ�� */
//    g_adc_dma_handle.Init.ContinuousConvMode = ENABLE;                      /* ʹ������ת��ģʽ */
//    g_adc_dma_handle.Init.NbrOfConversion = 1;                              /* ��ֵ��Χ��1~16����ʵ���õ�1������ͨ������ */
//    g_adc_dma_handle.Init.DiscontinuousConvMode = DISABLE;                  /* ��ֹ����ͨ������ģʽ */
//    g_adc_dma_handle.Init.NbrOfDiscConversion = 0;                          /* ���ü��ģʽ�Ĺ���ͨ����������ֹ����ͨ������ģʽ�󣬴˲������� */
//    g_adc_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;            /* ����ת����ʽ��������� */
//    HAL_ADC_Init(&g_adc_dma_handle);                                        /* ��ʼ�� */

//    HAL_ADCEx_Calibration_Start(&g_adc_dma_handle);                         /* У׼ADC */

//    /* ����ADCͨ�� */
//    adc_ch_conf.Channel = ADC_ADCX_CHY;                                     /* ͨ�� */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                                  /* ���� */
//    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* ����ʱ�䣬��������������:239.5��ADC���� */
//    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* ͨ������ */

//    /* ����DMA�����������ж����ȼ� */
//    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 3, 3);
//    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);

//    HAL_DMA_Start_IT(&g_dma_adc_handle, (uint32_t)&ADC1->DR, mar, 0);       /* ����DMA���������ж� */
//    HAL_ADC_Start_DMA(&g_adc_dma_handle, &mar, 0);                          /* ����ADC��ͨ��DMA������ */
//}

///*************************��ͨ��ADC�ɼ�(DMA��ȡ)ʵ��Ͷ�ͨ��ADC�ɼ�(DMA��ȡ)ʵ�鹫�ô���*******************************/

//DMA_HandleTypeDef g_dma_nch_adc_handle = {0};                               /* ����Ҫ����ADC��ͨ�����ݵ�DMA��� */
//ADC_HandleTypeDef g_adc_nch_dma_handle = {0};                               /* ����ADC����ͨ��DMA��ȡ����� */

///**
// * @brief       ADC Nͨ��(6ͨ��) DMA��ȡ ��ʼ������
// *   @note      ����������ʹ��adc_init��ADC���д󲿷�����,�в���ĵط��ٵ�������
// *              ����,���ڱ������õ���6��ͨ��, �궨���Ƚ϶�����, ���,�������Ͳ����ú궨��ķ�ʽ���޸�ͨ����,
// *              ֱ���ڱ����������޸�, ��������Ĭ��ʹ��PA0~PA5��6��ͨ��.
// *
// *              ע��: ����������ʹ�� ADC_ADCX(Ĭ��=ADC1) �� ADC_ADCX_DMACx( DMA1_Channel1 ) ������ض���
// *              ��Ҫ���޸�adc.h�����������������, ���������ԭ��Ļ����Ͻ����޸�, ������ܵ����޷�����ʹ��.
// *
// * @param       mar         : �洢����ַ 
// * @retval      ��
// */
//void adc_nch_dma_init(uint32_t mar)
//{
//    GPIO_InitTypeDef gpio_init_struct;
//    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
//    ADC_ChannelConfTypeDef adc_ch_conf = {0};

//    ADC_ADCX_CHY_CLK_ENABLE();                                                /* ʹ��ADCxʱ�� */
//    __HAL_RCC_GPIOA_CLK_ENABLE();                                             /* ����GPIOAʱ�� */

//    if ((uint32_t)ADC_ADCX_DMACx > (uint32_t)DMA1_Channel7)                   /* ����DMA1_Channel7, ��ΪDMA2��ͨ���� */
//    {
//        __HAL_RCC_DMA2_CLK_ENABLE();                                          /* DMA2ʱ��ʹ�� */
//    }
//    else
//    {
//        __HAL_RCC_DMA1_CLK_ENABLE();                                          /* DMA1ʱ��ʹ�� */
//    }

//    /* ����ADCʱ�� */
//    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;                    /* ADC����ʱ�� */
//    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;                       /* ��Ƶ����6ʱ��Ϊ72M/6=12MHz */
//    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                                 /* ����ADCʱ�� */

//    /* 
//        ����ADC1ͨ��0~5��Ӧ��IO��ģ������
//        AD�ɼ�����ģʽ����,ģ������
//        PA0��Ӧ ADC1_IN0
//        PA1��Ӧ ADC1_IN1
//        PA2��Ӧ ADC1_IN2
//        PA3��Ӧ ADC1_IN3
//        PA4��Ӧ ADC1_IN4
//        PA5��Ӧ ADC1_IN5
//    */
//    gpio_init_struct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;  /* GPIOA0~5 */
//    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                                 /* ģ�� */
//    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

//    /* ��ʼ��DMA */
//    g_dma_nch_adc_handle.Instance = ADC_ADCX_DMACx;                           /* ����DMAͨ�� */
//    g_dma_nch_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;               /* �����赽�洢��ģʽ */
//    g_dma_nch_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                   /* ���������ģʽ */
//    g_dma_nch_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                       /* �洢������ģʽ */
//    g_dma_nch_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;  /* �������ݳ���:16λ */
//    g_dma_nch_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;     /* �洢�����ݳ���:16λ */
//    g_dma_nch_adc_handle.Init.Mode = DMA_NORMAL;                              /* ��������ģʽ */
//    g_dma_nch_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                 /* �е����ȼ� */
//    HAL_DMA_Init(&g_dma_nch_adc_handle);

//    __HAL_LINKDMA(&g_adc_nch_dma_handle, DMA_Handle, g_dma_nch_adc_handle);   /* ��DMA��adc��ϵ���� */

//    /* ��ʼ��ADC */
//    g_adc_nch_dma_handle.Instance = ADC_ADCX;                                 /* ѡ���ĸ�ADC */
//    g_adc_nch_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                /* ���ݶ��뷽ʽ���Ҷ��� */
//    g_adc_nch_dma_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;                 /* ʹ��ɨ��ģʽ */
//    g_adc_nch_dma_handle.Init.ContinuousConvMode = ENABLE;                    /* ʹ������ת�� */
//    g_adc_nch_dma_handle.Init.NbrOfConversion = 6;                            /* ��ֵ��Χ��1~16����ʵ���õ�6������ͨ������ */
//    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;                /* ��ֹ����ͨ������ģʽ */
//    g_adc_nch_dma_handle.Init.NbrOfDiscConversion = 0;                        /* ���ü��ģʽ�Ĺ���ͨ����������ֹ����ͨ������ģʽ�󣬴˲������� */
//    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;          /* ������� */
//    HAL_ADC_Init(&g_adc_nch_dma_handle);                                      /* ��ʼ�� */

//    HAL_ADCEx_Calibration_Start(&g_adc_nch_dma_handle);                       /* У׼ADC */

//    /* ����ADCͨ�� */
//    adc_ch_conf.Channel = ADC_CHANNEL_0;                                      /* ����ʹ�õ�ADCͨ�� */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                                    /* ����������ĵ�1�� */
//    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                    /* ����ʱ�䣬��������������:239.5��ADC���� */
//    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);               /* ͨ������ */
//    
//    adc_ch_conf.Channel = ADC_CHANNEL_1;                                      /* ����ʹ�õ�ADCͨ�� */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_2;                                    /* ����������ĵ�2�� */
//    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);               /* ����ADCͨ�� */

//    adc_ch_conf.Channel = ADC_CHANNEL_2;                                      /* ����ʹ�õ�ADCͨ�� */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_3;                                    /* ����������ĵ�3�� */
//    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);               /* ����ADCͨ�� */

//    adc_ch_conf.Channel = ADC_CHANNEL_3;                                      /* ����ʹ�õ�ADCͨ�� */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_4;                                    /* ����������ĵ�4�� */
//    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);               /* ����ADCͨ�� */

//    adc_ch_conf.Channel = ADC_CHANNEL_4;                                      /* ����ʹ�õ�ADCͨ�� */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_5;                                    /* ����������ĵ�5�� */
//    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);               /* ����ADCͨ�� */

//    adc_ch_conf.Channel = ADC_CHANNEL_5;                                      /* ����ʹ�õ�ADCͨ�� */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_6;                                    /* ����������ĵ�6�� */
//    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);               /* ����ADCͨ�� */

//    /* ����DMA�����������ж����ȼ� */
//    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 3, 3);
//    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);

//    HAL_DMA_Start_IT(&g_dma_nch_adc_handle, (uint32_t)&ADC1->DR, mar, 0);     /* ����DMA���������ж� */
//    HAL_ADC_Start_DMA(&g_adc_nch_dma_handle, &mar, 0);                        /* ����ADC��ͨ��DMA������ */
//}

///*************************��ͨ��ADC�ɼ�(DMA��ȡ)ʵ��Ͷ�ͨ��ADC�ɼ�(DMA��ȡ)ʵ�鹫�ô���*******************************/

///**
// * @brief       ʹ��һ��ADC DMA����
// *   @note      �ú����üĴ�������������ֹ��HAL������������������޸�,ҲΪ�˼�����
// * @param       ndtr: DMA����Ĵ���
// * @retval      ��
// */
//void adc_dma_enable(uint16_t cndtr)
//{
//    ADC_ADCX->CR2 &= ~(1 << 0);                 /* �ȹر�ADC */

//    ADC_ADCX_DMACx->CCR &= ~(1 << 0);           /* �ر�DMA���� */
//    while (ADC_ADCX_DMACx->CCR & (1 << 0));     /* ȷ��DMA���Ա����� */
//    ADC_ADCX_DMACx->CNDTR = cndtr;              /* DMA���������� */
//    ADC_ADCX_DMACx->CCR |= 1 << 0;              /* ����DMA���� */

//    ADC_ADCX->CR2 |= 1 << 0;                    /* ��������ADC */
//    ADC_ADCX->CR2 |= 1 << 22;                   /* ��������ת��ͨ�� */
//}

///**
// * @brief       ADC DMA�ɼ��жϷ�����
// * @param       �� 
// * @retval      ��
// */
//void ADC_ADCX_DMACx_IRQHandler(void)
//{
//    if (ADC_ADCX_DMACx_IS_TC())
//    {
//        g_adc_dma_sta = 1;                      /* ���DMA������� */
//        ADC_ADCX_DMACx_CLR_TC();                /* ���DMA1 ������7 ��������ж� */
//    }
//}







