#pragma once

class LEDDriver {
public:
	LEDDriver() {
		
	}
	
	void init() {
		__HAL_RCC_GPIOD_CLK_ENABLE();
		GPIO_InitTypeDef GPIO_Init;
		GPIO_Init.Pin   = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
		GPIO_Init.Mode  = GPIO_MODE_OUTPUT_PP;
		GPIO_Init.Pull  = GPIO_NOPULL;
		GPIO_Init.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOD, &GPIO_Init);	
	}
	
	void toggleOrange() {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	}
	
	void toggleRed() {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	}
	
	void toggleBlue() {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	}
	
	void toggleGreen() {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	}
};
