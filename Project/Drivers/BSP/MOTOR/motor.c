#include "./BSP/MOTOR/motor.h"

TIM_HandleTypeDef htim1;

void MX_TIM1_Init(void) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    // 基本定时器1初始化
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 72 - 1; // 预分频器72-1
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数模式
    htim1.Init.Period = 1000 - 1; // 自动重装载寄存器1000-1
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
        // 错误处理
    }

    // 配置时钟源
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
        // 错误处理
    }

    // 初始化PWM
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
        // 错误处理
    }

    // 设置主从模式
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
        // 错误处理
    }

    // 配置PWM输出通道1
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;  // 初始占空比为50%
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        // 错误处理
    }

    // 启动PWM
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	 
	GPIO_InitTypeDef  GPIO_InitStructure;	
	__HAL_RCC_GPIOA_CLK_ENABLE();											//使能端口时钟
	
 	GPIO_InitStructure.Pin = GPIO_PIN_4;				 //端口配置
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//推挽输出
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);				 //初始化IO口
	GPIO_InitStructure.Pin = GPIO_PIN_5;				 //端口配置
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);				 //初始化IO口
	 
	 // 配置 PA8 为 TIM1 CH1 的复用功能 (PWM 输出)
	 GPIO_InitTypeDef GPIO_InitStructure1 = {0};
    GPIO_InitStructure1.Pin = GPIO_PIN_8;
    GPIO_InitStructure1.Mode = GPIO_MODE_AF_PP;    // 复用推挽模式
    GPIO_InitStructure1.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure1);
}


void motor_Control(int speed){
	
		if(speed>=0)
		{
			if(speed>=1000)
            {
			    speed=1000;
			}
		
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1,speed);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
		}
		else
		{	if(speed<=-1000)
            {
			    speed=-1000;
			}
		
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,GPIO_PIN_RESET);
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1,-speed);
		}

}

void motor_Stop(int a) {
	  // 停止通道1对应的电机
	  __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 0);
}
