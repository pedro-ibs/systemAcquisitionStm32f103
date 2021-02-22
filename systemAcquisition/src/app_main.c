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
 * Use este aquivo para inicialisar os perifericos de hardware
 * e todas as outras tasks da aplicação, veja FreeRTOSConfig.h
 * para identifica as configurações do sistema operacional
 * 
 * em ./core/ estão os inicialisadores básicos do framework
 * stm32cube
 *
 */



/* Includes ------------------------------------------------------------------*/
#include <core/includes.h>
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/portable.h>

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t TaskHandleMainApp = NULL;

/* Private Functions ---------------------------------------------------------*/
void vMain_app(void * pvParameters);
void vMX_GPIO_Init(void);

/**
 * @brief Inicializar tasks da aplicação, essa função é execurada dendro da main.c no FreeRTOS 
 * @param None
 */
extern void vStartupSystem(void) {
	/* init main app tasks */
	if(xTaskCreate( vMain_app, "app_main", configMINIMAL_STACK_SIZE*4, NULL, mainSET_PRIORITY, &TaskHandleMainApp) == pdFAIL){
		NVIC_SystemReset();		// RESET MCU
	} else {
		/* init other tasks */

	}	
}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/



/**
 * @brief Inicializar tasks da aplicação, essa função é execurada dendro da main.c no FreeRTOS 
 * @param pvParameters, ponteiro do parametro passado na criação da task não nao utilizado nesta 
 * função.
 */
void vMain_app(void * pvParameters){
	(void) pvParameters;

	vMX_GPIO_Init();

	while (TRUE) {
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		vTaskDelay(1000);
	}

}



/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void vMX_GPIO_Init(void){
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
