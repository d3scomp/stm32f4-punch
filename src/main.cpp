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
#include "LEDDriver.h"
#include "PWMCapture.h"
#include "UART.h"

#include <cstdio>
#include <cstring>

static void SystemClock_Config(void);
static void Error_Handler(void);

// IN	PB6&PB7	motor X PWM - red + jumper
// IN	PA0&PA1	motor Y PWM - brown + jumper

// IN	PC0		motor X direction - orange
// IN	PC1		motor Y direction - yellow

// IN	PC2		punch control - violet

// OUT	PC3		motor X encoder A - red
// OUT	PC4		motor X encoder B - orange
// OUT	PC5		motor Y encoder A - yellow
// OUT	PC6		motor Y encoder B - green

// OUT 	PC7		safe zone L - blue
// OUT 	PC8		safe zone R - red
// OUT 	PC9		safe zone T - orange
// OUT 	PC10	safe zone B - yellow

// OUT	PC11	head up (head ready to move or punch) - green
// OUT	PC12	FAIL - blue


void initPunchInput() {
	// Enable input pins
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_Init;
	GPIO_Init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
	GPIO_Init.Mode = GPIO_MODE_INPUT;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = 0;
	HAL_GPIO_Init(GPIOC, &GPIO_Init);
}

void initPunchOutput() {
	// Enable input pins
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_Init;
	GPIO_Init.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = 0;
	HAL_GPIO_Init(GPIOC, &GPIO_Init);
}

void writeEncoders(bool xA, bool xB, bool yA, bool yB) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, xA ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, xB ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, yA ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, yB ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void writeSafeZone(bool left, bool right, bool top, bool bottom) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, left ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, right ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, top ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, bottom ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void writeHeadUp(bool headUp) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, headUp?GPIO_PIN_SET:GPIO_PIN_RESET);
}

void writeFail(bool fail) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, fail?GPIO_PIN_SET:GPIO_PIN_RESET);
}

bool readMotorXDirection() {
	return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
}

bool readMotorYDirection() {
	return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
}

bool readHeadState() {
	return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);
}

TIM_HandleTypeDef htim2;
const uint32_t TIM2_TICK_PER_US = 84;

void initTimeCounter() {
	__HAL_RCC_TIM2_CLK_ENABLE();
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xffffffff;
	htim2.Init.ClockDivision = 0;
	HAL_TIM_Base_Init(&htim2);
	HAL_TIM_Base_Start(&htim2);
}

uint32_t getTimerCounter() {
	return __HAL_TIM_GetCounter(&htim2);
}


const int X_AXIS_TIMER = 5;
const int Y_AXIS_TIMER = 4;
const int MIN_PWM_FREQ_HZ = 500;
const int MOTOR_MAX_POWER = 128;
const int MASTER_CLOCK = 168000000;

PWMCapture<X_AXIS_TIMER> pwmCaptureX(MIN_PWM_FREQ_HZ, MASTER_CLOCK);
PWMCapture<Y_AXIS_TIMER> pwmCaptureY(MIN_PWM_FREQ_HZ, MASTER_CLOCK);

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

	LEDDriver leds;
	leds.init();

	initUARTConsole();
	
	initPunchInput();
	initPunchOutput();

	pwmCaptureX.init();
	pwmCaptureY.init();
	
	// Do something with LEDs to demonstrate that the code is running
	for(int cnt = 0; cnt < 8; cnt++) {
		switch(cnt % 4) {
			case 0: leds.toggleOrange(); break;
			case 1: leds.toggleRed(); break;
			case 2: leds.toggleBlue(); break;
			case 3: leds.toggleGreen(); break;
		}
		
		//std::printf("Hello world: %d\r\n", cnt);
		
		HAL_Delay(100); // 100ms
	}
	
	initTimeCounter();
	
	// Initialize punch press simulation
	PunchPress pp;

	pp.setPos(10000000, 10000000);

	//std::printf("Waiting 1s...\r\n");
	HAL_Delay(1000);

	std::printf("\r\n#R\r\n");
	
	uint32_t tim2last = getTimerCounter();
	
	bool oldHeadUp = false;

	// Infinite loop
	while (1) {
		// Obtain motor power
		const uint8_t xDuty = pwmCaptureX.getDutyCycle(MOTOR_MAX_POWER);
		const uint8_t yDuty = pwmCaptureY.getDutyCycle(MOTOR_MAX_POWER);
		const bool xDir = readMotorXDirection();
		const bool yDir = readMotorYDirection();
		const bool headDown = readHeadState();
		const int8_t xPower = xDir ? xDuty : -(MOTOR_MAX_POWER - xDuty);
		const int8_t yPower = yDir ? yDuty : -(MOTOR_MAX_POWER - yDuty);
		
		// Pass input to simulation
		pp.x.power = xPower;
		pp.y.power = yPower;
		pp.punch = headDown;
		
		//std::printf("Power(Direction, DutyCycle): X: %03d (%03d, %03d), Y: %03d (%03d, %03d)\r\n", xPower, xDir, xDuty, yPower, yDir, yDuty);
		
		// Make simulation step
		uint32_t tim2new = getTimerCounter();
		State state = pp.update((tim2new - tim2last) / TIM2_TICK_PER_US);
		tim2last = tim2new;
		/*std::printf("[%ld, %ld] state:%ld f:%d left: %s, top: %s, Px: %03d, Py: %03d\r\n",
					pp.x.headPos_nm,
					pp.y.headPos_nm,
					state,
					pp.failed,
					(state.getSafeLeft()) ? "1" : "0",
					(state.getSafeTop()) ? "1" : "0",
					xPower,
					yPower
		);*/

		if(pp.failed) {
			std::printf("#F\r\n");
		} else {
			std::printf("#H%ld;%ld\r\n", pp.x.headPos_nm, pp.y.headPos_nm);
			//std::printf("#H%ld;%ld;ld;%ld\r\n", pp.x.headPos_nm, pp.y.headPos_nm, pp.x.velocity_um_s, pp.y.velocity_um_s);
		}

		if(state.getHeadUp() && !oldHeadUp) {
			oldHeadUp = true;
			std::printf("#U\r\n");
		}

		if(!state.getHeadUp() && oldHeadUp) {
			oldHeadUp = false;
			std::printf("#D\r\n");
		}

		//printf("%ld	%ld\r\n", pp.x_axis.head_pos, pp.y_axis.head_pos);
		
		// Get output from simulation
		writeEncoders(state.getEncXA(),
			state.getEncXB(),
			state.getEncYA(),
			state.getEncYB());
		
		writeSafeZone(state.getSafeLeft(),
			state.getSafeRight(),
			state.getSafeTop(),
			state.getSafeBottom());
		
		writeHeadUp(state.getHeadUp());
		
		writeFail(state.getFail());
		
		// Signal simulation step (used to analyze simulator performance)
		leds.toggleGreen();
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
