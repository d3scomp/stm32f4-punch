#pragma once

#include <cstdio>

#include "stm32f4xx_hal.h"

template<int TIMER_INDEX>
class PWMCapture {
private:
	TIM_HandleTypeDef htim;
	const int MIN_PWM_FREQ_HZ;
	const int MASTER_CLOCK;
	const uint16_t COUNTER_PERIOD = 0xffff;
	const uint16_t TIMER_PRESCALER = (MASTER_CLOCK / COUNTER_PERIOD) / MIN_PWM_FREQ_HZ;
	const uint16_t MAX_TIMER_VALUE = (MASTER_CLOCK / MIN_PWM_FREQ_HZ) / TIMER_PRESCALER;

	
	GPIO_TypeDef *GPIO;
	uint32_t GPIOPin;
	uint32_t GPIOAlternate;

	TIM_TypeDef *timer;
	

public:
	PWMCapture(const int MIN_PWM_FREQ_HZ, const int MASTER_CLOCK);
	
	void init() {
		initPins();
		initTimer();
		initInputCapture();
		initSlaveReset();

		HAL_TIM_IC_Start(&htim, TIM_CHANNEL_2);
		HAL_TIM_IC_Start(&htim, TIM_CHANNEL_1);
	}
	
	/**
	 * Get measured duty cycle
	 * 
	 * @param max Value representing 100% duty cycle
	 */
	int getDutyCycle(const int max) {
		if(__HAL_TIM_GetCounter(&htim) > MAX_TIMER_VALUE) {
			//std::printf("!!! PWM too slow !!!\r\n");
			return 0;
		}

		uint32_t ch1 =  HAL_TIM_ReadCapturedValue(&htim, TIM_CHANNEL_1);
		uint32_t ch2 =  HAL_TIM_ReadCapturedValue(&htim, TIM_CHANNEL_2);

		if(ch1 == 0) {
			return 0;
		}

		const uint32_t duty = max * ch1 / ch2;
		//const uint32_t freq = (TIMER_PRESCALER * MASTER_CLOCK) / ch2;
		//std::printf("PWM duty: %ld frq: %ld !!! %ld, %ld\r\n",  duty, freq, ch1, ch2);
		return duty;
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
	
	// Enable timer for PWM capture
	void initTimer() {
		htim.Instance = timer;
		htim.Init.Prescaler = TIMER_PRESCALER;
		htim.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim.Init.Period = COUNTER_PERIOD;
		htim.Init.ClockDivision = 0;
		HAL_TIM_IC_Init(&htim);
	}
	
	// Enable PWM input capture
	void initInputCapture() {
		TIM_IC_InitTypeDef IC_Init;
		IC_Init.ICFilter = 0;
		IC_Init.ICPrescaler = TIM_ICPSC_DIV1;

		IC_Init.ICPolarity = TIM_ICPOLARITY_FALLING;
		IC_Init.ICSelection = TIM_ICSELECTION_INDIRECTTI;
		HAL_TIM_IC_ConfigChannel(&htim, &IC_Init, TIM_CHANNEL_1);

		IC_Init.ICPolarity = TIM_ICPOLARITY_RISING;
		IC_Init.ICSelection = TIM_ICSELECTION_DIRECTTI;
		HAL_TIM_IC_ConfigChannel(&htim, &IC_Init, TIM_CHANNEL_2);
	}
	
	void initSlaveReset() {
		TIM_SlaveConfigTypeDef sSlaveConfig;
		sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
		sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
		sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
		sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1 ;
		sSlaveConfig.TriggerFilter = 0;
		HAL_TIM_SlaveConfigSynchronization(&htim, &sSlaveConfig);

		TIM_MasterConfigTypeDef sMasterConfig;
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);
	}
};
