/*
 * usart_dma.c
 *
 *  Created at:		16/03/2021 11:28:05
 *      Author:		Pedro Igor B. S.
 *	Email:		pibscontato@gmail.com
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
/* stm includes */
#include <core/includes.h>
#include <stm32f1xx_hal_uart.h>
#include <stm32f1xx_hal_usart.h>

/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>
#include <FreeRTOS/Drivers/usart.h>

#if(USART_USE == USART_DMA)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

typedef struct {
	USART_TypeDef		*xUSART;
	UART_HandleTypeDef	xHandle;
	DMA_HandleTypeDef	xDMA_RXD;
	DMA_HandleTypeDef	xDMA_TXD;
	SemaphoreHandle_t	xSemaphore;
	char			pcRXD[SIZE_BUFFER_TXD];
	char 			pcRXD[SIZE_BUFFER_RXD];	
} xUsartHandleData;


static xUsartHandleData xUsart[ttyNUM] = {
	{USART1, {0}, {0}, NULL, "", ""},
	{USART2, {0}, {0}, NULL, "", ""},
	{USART3, {0}, {0}, NULL, "", ""}
};



// static UART_HandleTypeDef huart1;
// static UART_HandleTypeDef huart2;
// static UART_HandleTypeDef huart3;
// static DMA_HandleTypeDef hdma_usart1_rx;
// static DMA_HandleTypeDef hdma_usart1_tx;
// static DMA_HandleTypeDef hdma_usart2_rx;
// static DMA_HandleTypeDef hdma_usart2_tx;
// static DMA_HandleTypeDef hdma_usart3_rx;
// static DMA_HandleTypeDef hdma_usart3_tx;


/* Private Functions ---------------------------------------------------------*/


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/




/** ################################################################################################ **/
/** ######################################### Interrupções ######################################### **/
/** ################################################################################################ **/


void DMA1_Channel2_IRQHandler(void) {
	// HAL_DMA_IRQHandler(&hdma_usart3_tx);
}

void DMA1_Channel3_IRQHandler(void) {
	// HAL_DMA_IRQHandler(&hdma_usart3_rx);
}

void DMA1_Channel4_IRQHandler(void) {
	// HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

void DMA1_Channel5_IRQHandler(void) {
	// HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void DMA1_Channel6_IRQHandler(void) {
	// HAL_DMA_IRQHandler(&hdma_usart2_rx);
}

void DMA1_Channel7_IRQHandler(void) {
	// HAL_DMA_IRQHandler(&hdma_usart2_tx);
}


#endif /* #if(USART_USE == USART_DMA) */