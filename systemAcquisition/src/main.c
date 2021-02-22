/*
 * main.c
 *
 *  Created at:		22/02/2021 11:54:32
 *      Author:		Pedro Igor B. S.
 *	Email:		pedro.igor.ifsp@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 * TODO: Licence
 * ########################################################
 *
 * TODO: documentation or resume or Abstract
 *
 */



/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "sysHandler.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private Functions ---------------------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);

/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	/**
	 * Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState		= RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue	= RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState		= RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState		= RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource		= RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL		= RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		// Error_Handler();
	}

	/**
	 * Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType		= RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK														|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource		= RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider		= RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider	= RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider	= RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK){
		// Error_Handler();
	}
}




/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin	= GPIO_PIN_13;
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull	= GPIO_NOPULL;
	GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}




/**
 * 
 *  
 */
int main(void){
	HAL_Init();
	HAL_IncTick();
	HAL_GetTick();

	SystemClock_Config();
	MX_GPIO_Init();

		// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		// HAL_Delay(1000);
		// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		// HAL_Delay(1000);


	while (1) {
		// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		// HAL_Delay(1000);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(1000);
	}
	
	return 0;
}


