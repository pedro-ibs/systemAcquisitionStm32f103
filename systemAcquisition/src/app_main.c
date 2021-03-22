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
 *   Copyright (C) Pedro Igor B. S 2021
 * -------------------------------------------------------------------
 *
 *   Licença: GNU GPL 2
 * -------------------------------------------------------------------
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; version 2 of the
 *   License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * -------------------------------------------------------------------
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
#include <FreeRTOS/Drivers/usart.h>
#include <FreeRTOS/Drivers/adc.h>
#include <FreeRTOS/Drivers/rtc.h>
#include <FreeRTOS/Drivers/timer.h>
#include <FreeRTOS/Drivers/watchDogs.h>
#include <FreeRTOS/Drivers/flash.h>


#include <lib/textProtocol/protocol.h>


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char alpacas[] = 	"aplacas\r\n";

/* Private Functions ---------------------------------------------------------*/
void main_vApp(void * pvParameters);
void main_vInitTasks(void);
void main_vSetup(void);
void main_vLoop(void);

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
	gpio_vMode(GPIOC14, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);


	usart_vSetup(ttyUSART1, usart_9k6bps);
	usart_vSendStrLn(ttyUSART1, "stared system", strlen("stared system"));



	flash_vInit();
	flash_vTakeAsses();

	// flash_vErase();
	// flash_vWriteBlockToBuffer((const u8*)"123456_78\r\n", strlen("123456_78\r\n"));
	// flash_vBufferToFlash();	
	flash_vFlashToBuffer();
	
	flash_vGiveAsses();
}

/**
 * @brief inicia outras tasks
 * @param none 
 * 
 */
void main_vInitTasks(void) {
}

/**
 * @brief
 * @param
 * 
 */
void main_vLoop(void){

	flash_vTakeAsses();
	usart_vSendStr(ttyUSART1,  (CCHR*)flash_puGetBuffer(), PAGESIZE);
	flash_vGiveAsses();
	

	usart_vSendStr(ttyUSART1, alpacas, strlen(alpacas));
	vTaskDelay(_1S);
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
	while (TRUE) { main_vLoop(); }
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
