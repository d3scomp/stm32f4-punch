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
	
	
	inline void toggleGreen() {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	}
	
	inline void toggleOrange() {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	}
	
	inline void toggleRed() {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	}
	
	inline void toggleBlue() {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	}
	
	
	
	inline void onGreen() {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	}
	
	inline void onOrange() {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	}
	
	inline void onRed() {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	}
	
	inline void onBlue() {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	}
	
	
	
	inline void offGreen() {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	}
	
	inline void offOrange() {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	}
	
	inline void offRed() {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	}
	
	inline void offBlue() {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	}
};
