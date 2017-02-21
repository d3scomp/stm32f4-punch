#pragma once

#include "stm32f4xx_hal.h"

template<int TIMER_INDEX>
class PWMCapture {
private:
	TIM_HandleTypeDef htim;
	const uint16_t maxTimerValue;
	
	GPIO_TypeDef *GPIO;
	uint32_t GPIOPin;
	uint32_t GPIOAlternate;
	
	TIM_TypeDef *timer;
	

public:
	PWMCapture(const int maxTimerValue);
	
	void init() {
		initPins();
		initTimer();
		initInputCapture();
		initSlaveReset();

		HAL_TIM_IC_Start(&htim, TIM_CHANNEL_1);
		HAL_TIM_IC_Start(&htim, TIM_CHANNEL_2);
	}
	
	/**
	 * Get measured duty cycle
	 * 
	 * @param max Value representing 100% duty cycle
	 */
	int getDutyCycle(const int max) {
		if(__HAL_TIM_GetCounter(&htim) > maxTimerValue) {
			return 0;
		} else {
			uint32_t ch1 =  HAL_TIM_ReadCapturedValue(&htim, TIM_CHANNEL_1);
			uint32_t ch2 =  HAL_TIM_ReadCapturedValue(&htim, TIM_CHANNEL_2);
			return max * ch1 / ch2;
		}
	}
	
private:	
	// Enable GPIO pins for PWM capture
	void initPins() {
		GPIO_InitTypeDef GPIO_Init;
		GPIO_Init.Pin = GPIOPin;
		GPIO_Init.Mode = GPIO_MODE_AF_PP;
		GPIO_Init.Pull = GPIO_PULLUP;
		GPIO_Init.Speed = GPIO_SPEED_HIGH;
		GPIO_Init.Alternate = GPIOAlternate;
		HAL_GPIO_Init(GPIO, &GPIO_Init);
	}
	
	// Enable timer4 for PWM capture
	void initTimer() {
		htim.Instance = timer;
		htim.Init.Prescaler = 0;
		htim.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim.Init.Period = 0xFFFF;
		htim.Init.ClockDivision = 0;
		HAL_TIM_IC_Init(&htim);
	}
	
	// Enable PWM input capture
	void initInputCapture() {
		TIM_IC_InitTypeDef IC_Init;
		IC_Init.ICFilter = 0;
		IC_Init.ICPolarity = TIM_ICPOLARITY_FALLING;
		IC_Init.ICSelection = TIM_ICSELECTION_INDIRECTTI;
		IC_Init.ICPrescaler = TIM_ICPSC_DIV1;
		
		HAL_TIM_IC_ConfigChannel(&htim, &IC_Init, TIM_CHANNEL_1);
		HAL_TIM_IC_ConfigChannel(&htim, &IC_Init, TIM_CHANNEL_2);
	}
	
	void initSlaveReset() {
		TIM_SlaveConfigTypeDef sSlaveConfig;
		sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
		sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
		sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
		sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1 ;
		sSlaveConfig.TriggerFilter = 0;
		HAL_TIM_SlaveConfigSynchronization(&htim, &sSlaveConfig);
	}
};
