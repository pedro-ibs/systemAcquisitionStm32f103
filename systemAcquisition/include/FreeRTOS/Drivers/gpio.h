/*
 * gpio.h
 *
 *  Created at:		23/02/2021 09:07:11
 *      Author:		Pedro Igor B. S.
 *	Email:		pedro.igor.ifsp@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 * TODO: Licence
 * ########################################################
 *
 * Controle digital dos pinos GPIO, utilise as macros GPIOXX
 * para trabalhar com os pinos, por exemplo GPIOC, GPIO_PIN_13
 * é equivalente ao GPIOC13
 * 
 * esse driver pode ler escrever e trocar o estados de todos
 * os pinos. Além disso é possível desativar a comunicação SWJ.
 * 
 * TODO: Está em desenvolvimente o tratamento de interrupções
 * externas.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <core/includes.h>

#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>


#ifndef gpio_H_
#define gpio_H_

/* macro ---------------------------------------------------------------------*/
//=========================== A ============================//
#define GPIOA0		(10)
#define GPIOA1		(11)
#define GPIOA2		(12)
#define GPIOA3		(13)
#define GPIOA4		(14)
#define GPIOA5		(15)
#define GPIOA6		(16)
#define GPIOA7		(17)
#define GPIOA8		(29)
#define GPIOA9		(30)
#define GPIOA10		(31)
#define GPIOA11		(32)
#define GPIOA12		(33)
#define GPIOA13		(34)
#define GPIOA14		(37)
#define GPIOA15		(38)
//=========================== B ============================//
#define GPIOB0		(18)
#define GPIOB1		(19)
#define GPIOB2		(20)
#define GPIOB10		(21)
#define GPIOB11		(22)
#define GPIOB12		(25)
#define GPIOB13		(26)
#define GPIOB14		(27)
#define GPIOB15		(28)
#define GPIOB3		(39)
#define GPIOB4		(40)
#define GPIOB5		(41)
#define GPIOB6		(42)
#define GPIOB7		(43)
#define GPIOB8		(45)
#define GPIOB9		(46)
//=========================== C ============================//
#define GPIOC13		(2)
#define GPIOC14		(3)
#define GPIOC15		(4)
//=========================== D ============================//
#define GPIOD0		(5)
#define GPIOD1		(6)

/* Definition ----------------------------------------------------------------*/
void gpio_vInitAll( void );
void gpio_vDisableDebug ( void );
void gpio_vMode (cu32 cuGPIOxx, cu32 cuMode, cu32 cuPull );
_bool gpio_bRead(cu32 cuGPIOxx);
void gpio_vWrite(cu32 cuGPIOxx, _bool bValue );
void gpio_vToggle(cu32 cuGPIOxx);

#endif /* gpio_H_ */