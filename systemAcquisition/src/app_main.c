/*
 * main.c
 *
 *  Created at:		22/02/2021 11:54:32
 *      Author:		Pedro Igor B. S.
 *	Email:		pibscontato@gmail.com
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
#include <FreeRTOS/Drivers/rtc.h>
#include <FreeRTOS/Drivers/timer.h>
#include <FreeRTOS/Drivers/watchDogs.h>

#include <lib/textProtocol/protocol.h>


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CCHR pcStartedSystem[]	= "Started System";

/* Private Functions ---------------------------------------------------------*/
void main_vApp(void * pvParameters);
void main_vInitTasks(void);
void main_vSetup(void);


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/


/**
 * @brief inicia configurações básicas da aplicação
 * @param none 
 * 
 */
void main_vSetup(void){
	rtc_vInit();

	gpio_vInitAll();
	gpio_vMode(GPIOC13, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL);

	usart_vSetup(ttyUSART1, 9600);
	usart_vSendStrLn(ttyUSART1, pcStartedSystem, strlen(pcStartedSystem));
}

/**
 * @brief inicia outras tasks
 * @param none 
 * 
 */
void main_vInitTasks(void) {
	iwdg_vInit();
}


/**
 * usado pelo adc1 
 */
void acd1_vBufferDoneHandler(BaseType_t *const pxHigherPriorityTaskWoken){
	(void) pxHigherPriorityTaskWoken;
}

/**
 * usado pelo adc1
 */
void adc1_vDMA1Ch1Handler( BaseType_t *const pxHigherPriorityTaskWoken ){
	(void) pxHigherPriorityTaskWoken;
}

/**
 * usado pelo adc1 e/ou tim3
 */
void tim3_vHandler( BaseType_t *const pxHigherPriorityTaskWoken ){
	(void) pxHigherPriorityTaskWoken;
}



/**
 * @brief 
 * @param pvParameters, ponteiro do parametro passado na criação da task não nao utilizado nesta 
 * função.
 */
void main_vApp(void * pvParameters){
	(void) pvParameters;

	main_vSetup();
	main_vInitTasks();

	vTaskDelay(_1S);

	char pcValue[100] = { "" };

	while (TRUE) {
		iwdg_vStartConter(iwdgVC_0);	
		gpio_vToggle(GPIOC13);
		usart_vSendStrLn(ttyUSART1, rtc_pcGetTimeStamp(pcCleanStr(pcValue)), strlen(pcValue));
		vTaskDelay(_1S);
		iwdg_vStopConter(iwdgVC_0);
	}
}


/**
 * @brief inicia a task main da aplicação, essa função é execurada dendro da main.c no FreeRTOS 
 * @param None
 */
extern void vStartupSystem(void) {
	/* iniciar task principal */
	if(xTaskCreate( main_vApp, "app_main", configMINIMAL_STACK_SIZE*4, NULL, mainSET_PRIORITY, NULL) == pdFAIL){
		NVIC_SystemReset();		// RESET MCU
	}
}
