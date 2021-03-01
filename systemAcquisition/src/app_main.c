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

#include <FreeRTOS/Drivers/gpio.h>
#include <FreeRTOS/Drivers/uart.h>
#include <FreeRTOS/Drivers/adc.h>

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t TaskHandleMainApp = NULL;

/* Private Functions ---------------------------------------------------------*/
void vMain_app(void * pvParameters);

/**
 * @brief Inicializar tasks da aplicação, essa função é execurada dendro da main.c no FreeRTOS 
 * @param None
 */
extern void vStartupSystem(void) {
	/* iniciar task principal */
	if(xTaskCreate( vMain_app, "app_main", configMINIMAL_STACK_SIZE*4, NULL, mainSET_PRIORITY, &TaskHandleMainApp) == pdFAIL){
		NVIC_SystemReset();		// RESET MCU
	} else {
		/* iniciar putras tasks */

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

	gpio_vInitAll();
	gpio_vMode(GPIOC13, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL);
	usart_vSetup(ttyUSART1, 9600);

	adc1_vInitGetSample();

	CCHR *pcUsart = usart_pcGetBuffer(ttyUSART1);

	usart_vAtomicSendChr(ttyUSART1, 'A');

	while (TRUE) {
		gpio_vToggle(GPIOC13);
		vTaskDelay(_80MS);
		// usart_vSendStrLn(ttyUSART1, "pcUsart", strlen("pcUsart"));

		if(strstr(pcUsart, "\r\n")){
			usart_vSendStr(ttyUSART1, pcUsart, strlen(pcUsart));
			usart_vCleanBuffer(ttyUSART1);
		}
	}
}




void acd1_vBufferDone(BaseType_t *const pxHigherPriorityTaskWoken){
	(void) pxHigherPriorityTaskWoken;
}


void adc1_vDMA_IRQHandler( BaseType_t *const pxHigherPriorityTaskWoken ){
	(void) pxHigherPriorityTaskWoken;
}


void adc1_vTIM3_IRQHandler( BaseType_t *const pxHigherPriorityTaskWoken ){
	(void) pxHigherPriorityTaskWoken;
}