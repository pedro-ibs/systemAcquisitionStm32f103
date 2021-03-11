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
 * @brief 
 * @param pvParameters, ponteiro do parametro passado na criação da task não nao utilizado nesta 
 * função.
 */
void vMain_app(void * pvParameters){
	(void) pvParameters;

	gpio_vInitAll();
	gpio_vMode(GPIOC13, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL);

	usart_vSetup(ttyUSART1, 9600);
	

	// usart_vSendStrLn(ttyUSART1, "start get sample", strlen("start get sample"));
	// adc1_vInitGetSample( 1000, 65535 );
	// vTaskDelay(_10S);

	// usart_vSendStrLn(ttyUSART1, "stop get sample", strlen("stop get sample"));
	// adc1_vDeInitGetSample();
	// vTaskDelay(_3S);
	

	// usart_vSendStrLn(ttyUSART1, "start timer it", strlen("start timer it"));
	// tim3_vStartIT( 500, 65535 );
	// vTaskDelay(_10S);
	// usart_vSendStrLn(ttyUSART1, "stop timer it", strlen("stop timer it"));
	// tim3_vDeinit();
	// vTaskDelay(_3S);

	rtc_vInit();
	
	usart_vSendStrLn(ttyUSART1, "alpacas", strlen("alpacas"));


	// CCHR *pcUsart	= usart_pcGetBuffer(ttyUSART1);
	char pcValue[100] = { "" };

	while (TRUE) {		
		
		gpio_vToggle(GPIOC13);

		// usart_vSendStrLn(ttyUSART1, rtc_pcGetTimeStamp(pcValue), strlen(pcValue));
		
		// itoa(adc1_iGetFirstValue(ADC1_PA7), pcValue, DEC);
		// usart_vSendStrLn(ttyUSART1, pcValue, strlen(pcValue));
		vTaskDelay(_1S);


		// if(strstr(pcUsart, "\r\n")){
		// 	usart_vSendStr(ttyUSART1, pcUsart, strlen(pcUsart));
		// 	usart_vCleanBuffer(ttyUSART1);
		// }
	}
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