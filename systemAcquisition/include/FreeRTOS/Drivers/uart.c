/*
 * uart.c
 *
 *  Created at:		23/02/2021 16:49:47
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
 * driver serial para a comunicação serial RS232, esse driver
 * faz uso das interrupções USART_IT_TXE e USART_IT_RXNE para
 * transmitir e ressaber dados. Alem disso há recursos de queue
 * (para transmitir) e mutex para auxiliar no uso do driver
 * 
 * é possível  desativar a interrupção USART_IT_RXNE e transmitir
 * sem o uso da interrupção USART_IT_TXE.
 * 
 * para mais detalhes veja as descrições das funções 
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
#include <FreeRTOS/Drivers/uart.h>

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct {
	USART_TypeDef		*xUSART;
	UART_HandleTypeDef	xHandle;
	IRQn_Type		xIRQ;
	SemaphoreHandle_t	xSemaphore;
	QueueHandle_t		xTXD;
	char 			pcRXD[SIZE_BUFFER_RXD];	
	size_t			uSizeBuffer;
} xUsartHandleData;


static xUsartHandleData xUsart[ttyNUM] = {
	{USART1, {0}, USART1_IRQn, NULL, NULL, "", 0},
	{USART2, {0}, USART2_IRQn, NULL, NULL, "", 0},
	{USART3, {0}, USART3_IRQn, NULL, NULL, "", 0}
};


/* Private Functions ---------------------------------------------------------*/
_bool usart_bCheckError(const xTTY xtty);
void usart_vSetupGPIO(const xTTY xtty);
void usart_vInitVars(const xTTY xtty);
void usart_vInitIT(const xTTY xtty);
void usart_vIT(const xTTY xtty,BaseType_t *const pxHigherPriorityTaskWoken);


/**
 * @brief inicia a uart e variaveis use gpio_vInitAll() antes.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param cuBaudRate, velocidade da porta serial.
 */
void usart_vSetup(xTTY xtty, cu32 cuBaudRate){	
	if(xtty >= ttyNUM) return;

	usart_vInitVars(xtty);
	usart_vSetupGPIO(xtty);

	xUsart[xtty].xHandle.Instance		= xUsart[xtty].xUSART;
	xUsart[xtty].xHandle.Init.BaudRate	= cuBaudRate;
	xUsart[xtty].xHandle.Init.WordLength	= UART_WORDLENGTH_8B;
	xUsart[xtty].xHandle.Init.StopBits	= UART_STOPBITS_1;
	xUsart[xtty].xHandle.Init.Parity	= UART_PARITY_NONE;
	xUsart[xtty].xHandle.Init.Mode		= UART_MODE_TX_RX;
	xUsart[xtty].xHandle.Init.HwFlowCtl	= UART_HWCONTROL_NONE;
	xUsart[xtty].xHandle.Init.OverSampling	= UART_OVERSAMPLING_16;

	usart_vInitIT(xtty);

	if (HAL_UART_DeInit(&xUsart[xtty].xHandle) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_UART_Init(&xUsart[xtty].xHandle) != HAL_OK) {
		Error_Handler();
	}
}


/**
 * @brief pega o acesso ao periferio.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vTakeAccess(const xTTY xtty){
	if(usart_bCheckError(xtty)) return;
	xSemaphoreTake( xUsart[xtty].xSemaphore, portMAX_DELAY );
}


/**
 * @brief devolve o acesso ao periferio.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vGiveAccess(const xTTY xtty){
	if(usart_bCheckError(xtty)) return;
	xSemaphoreGive(xUsart[xtty].xSemaphore);
}


/**
 * @brief desativa a interrupção por recebimento de dados
 * USART_IT_RXNE
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vStopIT_RXD(const xTTY xtty){
	if(usart_bCheckError(xtty)) return;
	__HAL_USART_DISABLE_IT(&xUsart[xtty].xHandle, USART_IT_RXNE);
}

 
/**
 * @brief reativa a interrupção por recebimento de dados
 * USART_IT_RXNE
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vStartIT_RXD(const xTTY xtty){
	if(usart_bCheckError(xtty)) return;
	__HAL_USART_ENABLE_IT(&xUsart[xtty].xHandle, USART_IT_RXNE);	
}


/**
 * @brief calcula o tamanho ocupado do buffer RXD.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 * @return Caso xtty não seja um valor valudo o  valor
 * returnado será -1, do contrario o valor returnado
 * será o tamanho ocupado do buffer RXD .
 */
int usart_iSizeBuffer(const xTTY xtty){
	if(usart_bCheckError(xtty)) return -1;
	return xUsart[xtty].uSizeBuffer;
}


/**
 * @brief apaga os dados armazenados no buffer.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vCleanBuffer(const xTTY xtty){
	if(usart_bCheckError(xtty)) return;
	for(size_t uIdx=0; uIdx<SIZE_BUFFER_RXD; uIdx++){
		xUsart[xtty].pcRXD[uIdx] = 0x00U;
	}
	xUsart[xtty].uSizeBuffer = 0x00U;
}


/**
 * @brief pegar o buffer RXD da usart.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 * @return CCHR: retorna o ponteiro do tipo CCHR (const char *)
 * do buffer pcRXD. caso xtty seja invalido será retornado NULL
 */
CCHR *usart_pcGetBuffer(xTTY xtty){
	if(usart_bCheckError(xtty)) return NULL;
	return (CCHR*)xUsart[xtty].pcRXD;
}


/**
 * @brief envia um caractere. As funções usart_vAtomicX
 * utilizam o periferio uart se o uso da interrupção
 * USART_FLAG_TXE
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param ccChr, caractere tipo const char
 */
void usart_vAtomicSendChr(const xTTY xtty, CCHR ccChr){
	if(usart_bCheckError(xtty)) return;
	HAL_UART_Transmit(&xUsart[xtty].xHandle, (u8*)&ccChr, 1, UART_TRANSMIT_TIMEOUT);
};


/**
 * @brief envia uma cadeia de caracteres. As funções usart_vAtomicX
 * utilizam o periferio uart se o uso da interrupção USART_FLAG_TXE
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vAtomicSendStr(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize){
	if(usart_bCheckError(xtty))	return;
	if(pcBuffer == NULL)	return;
	HAL_UART_Transmit(&xUsart[xtty].xHandle, (u8*)pcBuffer, cuSize, UART_TRANSMIT_TIMEOUT);
}


/**
 * @brief envia uma cadeia de caracteres, ao final do envio de pcBuffer
 * a função irá enviar "\r\n". As funções usart_vAtomicX utilizam o
 * periferio uart se o uso da interrupção USART_FLAG_TXE
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vAtomicSendStrLn(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize){
	usart_vAtomicSendStr(xtty, pcBuffer, cuSize);
	usart_vAtomicSendStr(xtty, "\r\n", 2);
}


/**
 * @brief envia um caractere. Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * onde por por meio dessa função a queue é populada enquanto a interrupção
 * envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param ccChr, caractere tipo const char
 */
void usart_vSendChr(const xTTY xtty, CCHR ccChr){
	if(usart_bCheckError(xtty)) return;

	/* se a queue estiver cheia esse while possibilita a troca de
	 * contexto entre tasks, caso a mensagem transmitida seja muito
	 * grandeve a velocidade da cominicassão muito baixa.
	 */
	while( xQueueSend( xUsart[ xtty ].xTXD, &ccChr, 0 ) != pdPASS ){
		/* casso a queue esteja cheia espere espere esvaziar */
		__HAL_USART_ENABLE_IT(&xUsart[xtty].xHandle, UART_IT_TXE);
		vTaskDelay( _10MS );
	}
	__HAL_USART_ENABLE_IT(&xUsart[xtty].xHandle, UART_IT_TXE);
	
};


/**
 * @brief envia uma cadeia de caracteres. Essa função utilizam o periferio uart
 * junto da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma
 * queue, onde por por meio dessa função a queue é populada enquanto a interrupção
 * envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vSendStr(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize){
	if(usart_bCheckError(xtty))	return;
	if(pcBuffer == NULL)	return;
	
	for(size_t idx = 0; idx<cuSize; idx++){
		usart_vSendChr(xtty, pcBuffer[idx]);
	}
}


/**
 * @brief envia uma cadeia de caracteres, ao final do envio de pcBuffer
 * a função irá enviar "\r\n". Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * onde por por meio dessa função a queue é populada enquanto a interrupção
 * envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vSendStrLn(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize){
	usart_vSendStr(xtty, pcBuffer, cuSize);
	usart_vSendStr(xtty, "\r\n", 2);
}



/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/

/**
 * @brief testa a se as variaves teterente s ttyX foram inicializadas
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 * @return TRUE: variavel não inicialisadas
 * @return FALSE: variavel inicialisadas
 */
_bool usart_bCheckError(const xTTY xtty){
	if(xtty >= ttyNUM)			return	TRUE;
	if(xUsart[xtty].pcRXD == NULL)		return	TRUE;
	if(xUsart[xtty].xSemaphore == NULL)	return	TRUE;
	return 						FALSE;
}


/**
 * @brief inicialisa os GPIO para cada USART
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 */
void usart_vSetupGPIO(const xTTY xtty){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	switch (xtty) {
		case ttyUSART1:
			/*
			 * habilitando uart1 e trocando so pinos 
			 * padroes para PB6 e PB 7
			 */			
			__HAL_RCC_GPIOB_CLK_ENABLE();
			__HAL_RCC_AFIO_CLK_ENABLE();
			__HAL_RCC_USART1_CLK_ENABLE();
			__HAL_AFIO_REMAP_I2C1_DISABLE();
			__HAL_AFIO_REMAP_USART1_ENABLE();

			// PB6 USART1_TX
			GPIO_InitStruct.Pin	= GPIO_PIN_6;
			GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
			HAL_GPIO_DeInit(GPIOB,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOB,	&GPIO_InitStruct);

			// PB7 USART1_RX
			GPIO_InitStruct.Pin	= GPIO_PIN_7;
			GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull	= GPIO_NOPULL;
			HAL_GPIO_DeInit(GPIOB,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOB,	&GPIO_InitStruct);
			break;
		
		case ttyUSART2:
			__HAL_RCC_GPIOA_CLK_ENABLE();
			__HAL_RCC_USART2_CLK_ENABLE();
			
			// PA2 USART1_TX
			GPIO_InitStruct.Pin	= GPIO_PIN_2;
			GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
			HAL_GPIO_DeInit(GPIOA,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOA,	&GPIO_InitStruct);

			// PA3 USART1_RX
			GPIO_InitStruct.Pin	= GPIO_PIN_3;
			GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull	= GPIO_NOPULL;
			HAL_GPIO_DeInit(GPIOA,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOA, 	&GPIO_InitStruct);
			break;

		case ttyUSART3:
		default:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			__HAL_RCC_USART3_CLK_ENABLE();

			// PB10 USART1_TX
			GPIO_InitStruct.Pin	= GPIO_PIN_10;
			GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
			HAL_GPIO_DeInit(GPIOB,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOB,	&GPIO_InitStruct);

			// PB11 USART1_RX
			GPIO_InitStruct.Pin	= GPIO_PIN_11;
			GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull	= GPIO_NOPULL;
			HAL_GPIO_DeInit(GPIOB,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOB,	&GPIO_InitStruct);
			break;
	}
}


/**
 * @brief inicialisa as queue para cada USART
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 */
void usart_vInitVars(const xTTY xtty){
	if(xUsart[xtty].xSemaphore == NULL){
		xUsart[xtty].xSemaphore = xSemaphoreCreateMutex();
	}

	if(xUsart[xtty].xTXD == NULL){
		xUsart[xtty].xTXD = xQueueCreate(SIZE_BUFFER_TXD, sizeof(char));
	}

	for(size_t uIdx=0; uIdx<SIZE_BUFFER_RXD; uIdx++){
		xUsart[xtty].pcRXD[uIdx] = 0x00U;
	}
	xUsart[xtty].uSizeBuffer = 0x00U;
}


/**
 * @brief inicialisa as interrupções para cada USART
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 */
void usart_vInitIT(const xTTY xtty){
	__HAL_USART_CLEAR_FLAG(&xUsart[xtty].xHandle,	USART_FLAG_RXNE | USART_FLAG_TXE );
	__HAL_USART_DISABLE_IT(&xUsart[xtty].xHandle,	USART_IT_TXE);
	__HAL_USART_ENABLE_IT(&xUsart[xtty].xHandle,	USART_IT_RXNE);
	
	HAL_NVIC_SetPriority(xUsart[xtty].xIRQ, NVIC_PRIORITY_USART, NVIC_SUBPRIORITY_USART);
	HAL_NVIC_EnableIRQ(xUsart[xtty].xIRQ);
}


/**
 * @brief
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS para o controle
 * de troca de contexto em interruições. 
 */
void usart_vIT(const xTTY xtty, BaseType_t *const pxHigherPriorityTaskWoken){

	// interrupção por recebimento de um framer 
	if(__HAL_UART_GET_FLAG(&xUsart[xtty].xHandle, USART_FLAG_RXNE) == SET){
		__HAL_USART_CLEAR_FLAG(&xUsart[xtty].xHandle, USART_FLAG_RXNE);
		
		u8 uBuffer = 0x00U;

		if ( (xUsart[xtty].xHandle.Init.WordLength == UART_WORDLENGTH_9B) || ((xUsart[xtty].xHandle.Init.WordLength == UART_WORDLENGTH_8B) && (xUsart[xtty].xHandle.Init.Parity == UART_PARITY_NONE))) {
			uBuffer = ( xUsart[xtty].xHandle.Instance->DR & 0x00FF);
		} else {
			uBuffer = ( xUsart[xtty].xHandle.Instance->DR & 0x007F);
		}

		if(xUsart[xtty].uSizeBuffer < SIZE_BUFFER_RXD){
			xUsart[xtty].pcRXD[xUsart[xtty].uSizeBuffer++] = uBuffer;
		}


	// interrupção por envio de um framer 
	} else if(__HAL_UART_GET_FLAG(&xUsart[xtty].xHandle, USART_FLAG_TXE) == SET ){
		__HAL_USART_CLEAR_FLAG(&xUsart[xtty].xHandle, USART_FLAG_TXE);

		u8 uBuffer = 0x00U;
		
		if(xQueueReceiveFromISR(xUsart[xtty].xTXD, &uBuffer, pxHigherPriorityTaskWoken) == pdPASS ){
			// xUsart[xtty].xHandle.Instance->DR = (uBuffer & 0xFFU);
			HAL_UART_Transmit(&xUsart[xtty].xHandle, (u8*)&uBuffer, 1, UART_TRANSMIT_TIMEOUT);
		} else {
			// quando a queue estiver vazei a interrupção não deve ser chamada novamente
			// amentos que a queue seja populada novamente
			__HAL_USART_DISABLE_IT(&xUsart[xtty].xHandle, USART_IT_TXE);
		}
	}
}





/** ################################################################################################ **/
/** ######################################### Interrupções ######################################### **/
/** ################################################################################################ **/

void USART1_IRQHandler(void){
	HAL_UART_IRQHandler(&xUsart[ttyUSART1].xHandle);
	long xHigherPriorityTaskWoken = pdFALSE;
	usart_vIT(ttyUSART1, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void USART2_IRQHandler(void){
	HAL_UART_IRQHandler(&xUsart[ttyUSART2].xHandle);
	long xHigherPriorityTaskWoken = pdFALSE;
	usart_vIT(ttyUSART2, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void USART3_IRQHandler(void){
	HAL_UART_IRQHandler(&xUsart[ttyUSART3].xHandle);
	long xHigherPriorityTaskWoken = pdFALSE;
	usart_vIT(ttyUSART3, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}