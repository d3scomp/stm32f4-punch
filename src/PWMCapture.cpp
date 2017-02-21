#include "PWMCapture.h"

template<>
PWMCapture<4>::PWMCapture(const int maxTimerValue):
		maxTimerValue(maxTimerValue) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	GPIO = GPIOB;
	GPIOPin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIOAlternate = GPIO_AF2_TIM4;
	timer = TIM4;
}

template<>
PWMCapture<5>::PWMCapture(const int maxTimerValue):
		maxTimerValue(maxTimerValue) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_TIM5_CLK_ENABLE();
	GPIO = GPIOA;
	GPIOPin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIOAlternate = GPIO_AF2_TIM5;
	timer = TIM5;
}

