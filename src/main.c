/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @version V1.2.5
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "simulation.h"

#include <stdio.h>



static void SystemClock_Config(void);
static void Error_Handler(void);

UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

/** Custom implementation of write function. This would be syscall, but since
 * we do not have OS we need to implement it ourself by print to console. */
int _write(int file, char* ptr, int len) {
	HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
	return len;
}

int main(void) {
	/* STM32F4xx HAL library initialization:
		- Configure the Flash prefetch, Flash preread and Buffer caches
		- Systick timer is configured by default as source of time base, but user 
				can eventually implement his proper time base source (a general purpose 
				timer for example or other time source), keeping in mind that Time base 
				duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
				handled in milliseconds basis.
		- Low Level Initialization
		*/
	HAL_Init();

	// Configure the system clock to 168 MHz
	SystemClock_Config();

	// Enable GPIO pins connected to LEDs and set them as output */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_Init;
	GPIO_Init.Pin   = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_Init.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pull  = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_Init);
	
	// Enable pins used by UART2, set them to their alterantive (UART2) function
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct2;
	GPIO_InitStruct2.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct2.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct2.Pull = GPIO_PULLUP;
	GPIO_InitStruct2.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct2.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct2);

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
	
	// Enable GPIO pins for PWM capture - X axis
	__HAL_RCC_GPIOB_CLK_ENABLE();
//	GPIO_InitTypeDef GPIO_Init;
	GPIO_Init.Pin   = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_Init.Mode  = GPIO_MODE_AF_PP;
	GPIO_Init.Pull  = GPIO_PULLUP;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOB, &GPIO_Init);
	
	// Enable GPIO pins for PWM capture - Y axis
	__HAL_RCC_GPIOA_CLK_ENABLE();
//	GPIO_InitTypeDef GPIO_Init;
	GPIO_Init.Pin   = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_Init.Mode  = GPIO_MODE_AF_PP;
	GPIO_Init.Pull  = GPIO_PULLUP;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = GPIO_AF2_TIM5;
	HAL_GPIO_Init(GPIOA, &GPIO_Init);

	// Enable timer4 for PWM capture - X axis
	__HAL_RCC_TIM4_CLK_ENABLE();
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 0;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 0xFFFF;
    htim4.Init.ClockDivision = 0;
	HAL_TIM_IC_Init(&htim4);
	
	// Enable timer5 for PWM capture - Y axis
	__HAL_RCC_TIM5_CLK_ENABLE();
	htim5.Instance = TIM5;
	htim5.Init.Prescaler = 0;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim5.Init.Period = 0xFFFF;
    htim5.Init.ClockDivision = 0;
	HAL_TIM_IC_Init(&htim5);
	
	// Enable PWM input capture - X axis
	TIM_IC_InitTypeDef IC_Init;
	IC_Init.ICFilter = 0;
	IC_Init.ICPolarity = TIM_ICPOLARITY_FALLING;
	IC_Init.ICSelection = TIM_ICSELECTION_INDIRECTTI;
	IC_Init.ICPrescaler = TIM_ICPSC_DIV1;
	
	HAL_TIM_IC_ConfigChannel(&htim4, &IC_Init, TIM_CHANNEL_1);
	HAL_TIM_IC_ConfigChannel(&htim4, &IC_Init, TIM_CHANNEL_2);

	TIM_SlaveConfigTypeDef sSlaveConfig;
	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
	sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
	HAL_TIM_SlaveConfigSynchronization(&htim4, &sSlaveConfig);


	HAL_TIM_IC_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&htim4, TIM_CHANNEL_2);
	
	// Enable PWM input capture - Y axis
	IC_Init.ICFilter = 0;
	IC_Init.ICPolarity = TIM_ICPOLARITY_FALLING;
	IC_Init.ICSelection = TIM_ICSELECTION_INDIRECTTI;
	IC_Init.ICPrescaler = TIM_ICPSC_DIV1;
	
	HAL_TIM_IC_ConfigChannel(&htim5, &IC_Init, TIM_CHANNEL_1);
	HAL_TIM_IC_ConfigChannel(&htim5, &IC_Init, TIM_CHANNEL_2);

	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
	sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
	HAL_TIM_SlaveConfigSynchronization(&htim5, &sSlaveConfig);

	HAL_TIM_IC_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&htim5, TIM_CHANNEL_2);
	
	// Do something with LEDs to demonstrate that the code is running
	for(int cnt = 0; cnt < 8; cnt++) {
		switch(cnt % 4) {
			case 0: HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);  break;
			case 1: HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);  break;
			case 2: HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);  break;
			case 3: HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);  break;
		}
		
		iprintf("Hello world: %d\r\n", cnt);
		
		HAL_Delay(100); // 100ms
	}
	
	// Initialize punch press simulation
	struct pp_t pp;
	
	pp.use_init_pos = 1;
	pp.x_init_pos = 0;
	pp.y_init_pos = 0;
	
	pp_init(&pp);
	
	pp.x_axis.power = 32;
	pp.y_axis.power = 45;
	
	// Infinite loop
	while (1) {
		//HAL_Delay(100); // 100ms
		pp_update(&pp, 1000);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		
	//	iprintf("[%ld, %ld] f:%d\r\n", pp.x_axis.head_pos, pp.y_axis.head_pos, pp.failed);
		
		uint32_t T4ch1 =  HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_1);
		uint32_t T4ch2 =  HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_2);
		uint32_t T5ch1 =  HAL_TIM_ReadCapturedValue(&htim5, TIM_CHANNEL_1);
		uint32_t T5ch2 =  HAL_TIM_ReadCapturedValue(&htim5, TIM_CHANNEL_2);
		
		
		iprintf("Duty cycle: T4ch1: %ld, T4ch2: %ld, T5ch1: %ld, T5ch2: %ld, tim4: %ld, tim5: %ld\r\n", T4ch1, T4ch2, T5ch1, T5ch2, __HAL_TIM_GetCounter(&htim4), __HAL_TIM_GetCounter(&htim5));
	}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	// Enable Power Control clock
	__HAL_RCC_PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is 
		clocked below the maximum system frequency, to update the voltage scaling value 
		regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		// Initialization Error
		Error_Handler();
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
		clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		// Initialization Error
		Error_Handler();
	}

	// STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported
	if (HAL_GetREVID() == 0x1001) {
		// Enable the Flash prefetch
		__HAL_FLASH_PREFETCH_BUFFER_ENABLE();
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void) {
	/* User may add here some code to deal with this error */
	while(1) {}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
		ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1) {}
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
