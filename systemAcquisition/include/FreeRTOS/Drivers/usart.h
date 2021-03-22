/*
 * usart.h
 *
 *  Created at:		16/03/2021 11:34:54
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
 * 
 * ------------------------ USART_IT ------------------------
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
 * 
 * ------------------------ USART_DMA ------------------------
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <core/includes.h>
#include <FreeRTOS/include/FreeRTOSConfig.h>

#ifndef usart_it_or_dma_H_
#define usart_it_or_dma_H_

/* macro ---------------------------------------------------------------------*/

/**
 * 
 * Configure de acordo com a forma que deseje que a usart funcione
 * 
 * 
 */
#define USART_IT		( 1 )
#define USART_DMA		( 2 )
#define USART_USE		( USART_IT )


/**
 * TODO: coc
 */
#define SIZE_BUFFER_TXD		( 80 )
#define SIZE_BUFFER_RXD		( 140 )


/*
 * tempo limite para enviar dados com a função 
 * HAL_UART_Transmit
 * 
 */
#define UART_TRANSMIT_TIMEOUT	( 1000 )


/**
 * prioridades das interruições de todos 
 * os perifericos usart
 */
#define NVIC_PRIORITY_USART	( 0 )
#define NVIC_SUBPRIORITY_USART	( 0 )



/* Definition ----------------------------------------------------------------*/
typedef enum{
	ttyUSART1 = 0,
	ttyUSART2,
	ttyUSART3,
	ttyNUM
}xTTY;


typedef enum{
	usart_0k3bps	= 300,
	usart_0k6bps	= 600,
	usart_1k2bps	= 1200,
	usart_2k4bps	= 2400,
	usart_4k8bps	= 4800,
	usart_9k6bps	= 9600,
	usart_14k4bps	= 14400,
	usart_19k2bps	= 19200,
	usart_38k4bps	= 38400,
	usart_57k6bps	= 57600,
	usart_115k2bps	= 115200,
	usart_230k4bps	= 230400,
	usart_250k0bps	= 250000,
	usart_460k8bps	= 460800,
	usart_500k0bps	= 500000,
	usart_1M0bps	= 1000000,
}xBaudRate;


void usart_vInit(UART_HandleTypeDef *xHandle, const xTTY xtty,  xBaudRate cuBaudRate);

void usart_vSetup(const xTTY xtty, const xBaudRate cuBaudRate);

void usart_vTakeAccess(const xTTY xtty);
void usart_vGiveAccess(const xTTY xtty);

void usart_vCleanBuffer(const xTTY xtty);
int usart_iSizeBuffer(const xTTY xtty);
CCHR *usart_pcGetBuffer(xTTY xtty);

void usart_vAtomicSendChr(const xTTY xtty, CCHR ccChr);
void usart_vAtomicSendStr(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vAtomicSendStrLn(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);

void usart_vSendChr(const xTTY xtty, CCHR ccChr);
void usart_vSendStr(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vSendStrLn(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);




#if(USART_USE == USART_IT)

#elif(USART_USE == USART_DMA)

#endif


#endif /* usart_it_or_dma_H_ */