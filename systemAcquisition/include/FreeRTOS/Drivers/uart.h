/*
 * uart.h
 *
 *  Created at:		23/02/2021 16:50:19
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
#include <core/includes.h>

#ifndef uart_H_
#define uart_H_

/* macro ---------------------------------------------------------------------*/
#define SIZE_BUFFER_TXD		( 80 )
#define SIZE_BUFFER_RXD		( 140 )

#define UART_TRANSMIT_TIMEOUT	( 1000 )

#define NVIC_PRIORITY_USART	( 0 )
#define NVIC_SUBPRIORITY_USART	( 0 )

/* Definition ----------------------------------------------------------------*/
typedef enum{
	ttyUSART1 = 0,
	ttyUSART2,
	ttyUSART3,
	ttyNUM
}xTTY;


void usart_vSetup(xTTY xtty, cu32 cuBaudRate);

void usart_vAtomicSendChr(const xTTY xtty, CCHR ccChr);
void usart_vAtomicSendStr(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vAtomicSendStrLn(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);

void usart_vSendChr(const xTTY xtty, CCHR ccChr);
void usart_vSendStr(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vSendStrLn(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);



#endif /* uart_H_ */