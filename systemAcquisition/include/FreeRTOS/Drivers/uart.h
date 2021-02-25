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
#include <core/includes.h>
#include <FreeRTOS/include/FreeRTOSConfig.h>

#ifndef uart_H_
#define uart_H_

/* macro ---------------------------------------------------------------------*/

/**
 * Evite sar valores grandes, recomendo
 * valores iguais ou menores que 1024 bytes
 * 
 * Evite tambem um valor para TXD muito pequeno
 * pode ocasionar pausa muito frequentes para a
 * task
 * 
 * O temanho de RXD deve ser coerente com a aplicação
 * levando em conta o tamanho maximo das mensagens
 * enviadas para o microcontrolador 
 */
#define SIZE_BUFFER_TXD		( 80 )
#define SIZE_BUFFER_RXD		( 140 )

/*
 * tempo limite para enviar dados com a função 
 * HAL_UART_Transmit
 * 
 * TODO: NOTA acredito que esse timeout da função
 * não funcione com o freertos ativo
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


void usart_vSetup(xTTY xtty, cu32 cuBaudRate);

void usart_vTakeAccess(const xTTY xtty);
void usart_vGiveAccess(const xTTY xtty);
void usart_vStopIT_RXD(const xTTY xtty);
void usart_vStartIT_RXD(const xTTY xtty);


void usart_vCleanBuffer(const xTTY xtty);
int usart_iSizeBuffer(const xTTY xtty);
CCHR *usart_pcGetBuffer(xTTY xtty);

void usart_vAtomicSendChr(const xTTY xtty, CCHR ccChr);
void usart_vAtomicSendStr(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vAtomicSendStrLn(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);

void usart_vSendChr(const xTTY xtty, CCHR ccChr);
void usart_vSendStr(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vSendStrLn(const xTTY xtty, CCHR *pcBuffer, const size_t cuSize);



#endif /* uart_H_ */