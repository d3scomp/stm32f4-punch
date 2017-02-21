#include "UART.h"

#include "stm32f4xx_hal.h"

static UART_HandleTypeDef huart2;

/** Custom implementation of write function. This would be syscall, but since
 * we do not have OS we need to implement it ourself by print to console. */
int _write(int file, char* ptr, int len) {
	HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
	return len;
}

void initUARTConsole() {
	// Enable pins used by UART2, set them to their alterantive (UART2) function
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef init;
	init.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	init.Mode = GPIO_MODE_AF_PP;
	init.Pull = GPIO_PULLUP;
	init.Speed = GPIO_SPEED_HIGH;
	init.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &init);

	// Enable UART2
	__HAL_RCC_USART2_CLK_ENABLE();
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 921600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart2);
}
 
