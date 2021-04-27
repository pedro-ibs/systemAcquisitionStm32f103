/**
 * main.c
 *
 *  @date Created at:	22/02/2021 11:54:32
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
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
#include <FreeRTOS/include/semphr.h>
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
static const u16 *puAdcBuffer	= NULL;
static char pcSwap[10]		= "";
/* Private Functions ---------------------------------------------------------*/
void main_vApp(void * pvParameters);
void main_vInitTasks(void);
void main_vSetup(void);
void main_vLoop(void);


void blink(void * pvParameters);

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
	gpio_vInitAll();
	gpio_vDisableDebug();
	rtc_vInit();
	iwdg_vInit();
	usart_vSetup(ttyUSART1, usart_115k2bps);
	vTaskDelay(_5S);
	gpio_vMode(GPIOC13, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL);
	gpio_vWrite(GPIOC13, TRUE);


	/**
	 * pegar endereço do buffer do adc
	 */
	puAdcBuffer = adc1_puGetBuffer();

}

/**
 * @brief inicia outras tasks
 * @param none 
 * 
 */
void main_vInitTasks(void) { }

/**
 * @brief
 * @param
 * 
 */
void main_vLoop(void){
	
	/**
	 * a aquisição le 50000 pontos por segundo portanto em FreeRTOS/Drivers/adc.h
	 * na macro ADC1_SIZE_BUFFER determina quando pontos o DMA lê buffer antes de
	 * chamar a interrupção (que é chamada quando o DMA popula o buffer por inteiro )
	 * 
	 * com o valor de 7950 a aquisição leva 159ms
	 * 
	 * na interrupção acd1_vBufferDoneHandler é dado o comando para que o DMA pare 
	 * o ciclo de aquisição, dessa maneira o buffer é populado apenas uma única vez
	 * 
	 * o led se manterá ligado enquanto o buffer estiver sendo popiçado pelo DMA
	 */
	gpio_vWrite(GPIOC13, FALSE);
	adc1_vInitGetSample(1, FREG_TO_COUNTER(50000, 1));
	
	/**
	 * esperar que o DMA poule o buffer
	 */
	vTaskDelay(_1S);

	/**
	 * enviar os em formado CSV
	 */	
	for (size_t uIdx = 0; uIdx < ADC1_SIZE_BUFFER; uIdx++){
		itoa(puAdcBuffer[uIdx], pcSwap, DEC);
		strcat(pcSwap, ";");
		usart_vAtomicSendStrLn(ttyUSART1, pcSwap);
	}
	usart_vAtomicSendStrLn(ttyUSART1, "\r\n");
	
	/**
	 * esperar um novo ciclo
	 */
	vTaskDelay(_9S);
}



/**
 * usado pelo adc1 
 */
void acd1_vBufferDoneHandler(BaseType_t *const pxHigherPriorityTaskWoken){
	(void) pxHigherPriorityTaskWoken;

	/**
	 * quando essa interrupção ser chamada significa 
	 * que o buffer está populado, portanto a aquisição
	 * pode ser parada 
	 */
	adc1_vDeInitGetSample();
	tim3_vDeinit();
	gpio_vWrite(GPIOC13, TRUE);
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
	return;
}


/**
 * @brief inicia a task main da aplicação, essa função é execurada dendro da main.c no FreeRTOS 
 * @param None
 */
extern void vStartupSystem(void) {
	/* iniciar task principal */
	if(xTaskCreate( main_vApp, "app_main", configMINIMAL_STACK_SIZE*2, NULL, mainSET_PRIORITY, NULL) == pdFAIL){
		NVIC_SystemReset();		// RESET MCU
	} 
}
