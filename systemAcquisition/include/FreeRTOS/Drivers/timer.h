/*
 * timer3.h
 *
 *  Created at:		05/03/2021 16:11:57
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
#include <core/includes.h>
#include <FreeRTOS/include/FreeRTOS.h>

#ifndef timer_3_H_
#define timer_3_H_

/* macro ---------------------------------------------------------------------*/
/* Definition ----------------------------------------------------------------*/


void tim3_vStartAdc1Trigger(cu32 uPrescaler, cu32 uPeriod);
void tim3_vStartIT(cu32 uPrescaler, cu32 uPeriod);
void tim3_vDeinit( void );



/**
 * @brief Função dentro da interrupção TIM3_IRQHandler
 * @note Não trada nenhuma flag de interrupção, porem
 * pxHigherPriorityTaskWoken é tratado no termino da interrupção
 * 
 * Quando o TIM3 é inicializado por tim3_vStartAdc1Trigger o
 * TIM3 fica responsável por cadenciar a leitura dos canais do
 * ADC1, quando interrupção é chamada significa que o ADC1
 * comessou uma nova leitura.
 * 
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS
 * para o controle de troca de contexto em interruições.
 */
extern void tim3_vHandler(BaseType_t *const pxHigherPriorityTaskWoken);


#endif /* timer_3_H_ */