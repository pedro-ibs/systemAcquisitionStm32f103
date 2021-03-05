/*
 * gpio.h
 *
 *  Created at:		23/02/2021 09:07:11
 *      Author:		Pedro Igor B. S.
 *	Email:		pibscontato@gmail.com
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
/* Definition ----------------------------------------------------------------*/
typedef enum {
	GPIOA0	 = 0,
	GPIOA1,
	GPIOA2,
	GPIOA3,
	GPIOA4,
	GPIOA5,
	GPIOA6,
	GPIOA7,
	GPIOA8,
	GPIOA9,
	GPIOA10,
	GPIOA11,
	GPIOA12,
	GPIOA13,
	GPIOA14,
	GPIOA15,
	GPIOB0,
	GPIOB1,
	GPIOB2,
	GPIOB3,
	GPIOB4,
	GPIOB5,
	GPIOB6,
	GPIOB7,
	GPIOB8,
	GPIOB9,
	GPIOB10,
	GPIOB11,
	GPIOB12,
	GPIOB13,
	GPIOB14,
	GPIOB15,
	GPIOC13,
	GPIOC14,
	GPIOC15,
	GPIOD0,
	GPIOD1,
	GPIO_NUM
}xGpioLabel;


void gpio_vInitAll( void );
void gpio_vDisableDebug ( void );
void gpio_vMode (const xGpioLabel cxGPIOxx, cu32 cuMode, cu32 cuPull );
void gpio_vAnalogMode (const xGpioLabel cxGPIOxx);
void gpio_vDeinit(const xGpioLabel cxGPIOxx);
_bool gpio_bRead(const xGpioLabel cxGPIOxx);
void gpio_vWrite(const xGpioLabel cxGPIOxx, _bool bValue );
void gpio_vToggle(const xGpioLabel cxGPIOxx);

#endif /* gpio_H_ */