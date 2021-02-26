/*
 * adc.h
 *
 *  Created at:		26/02/2021 10:53:47
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
#include <FreeRTOS/Drivers/gpio.h>


#ifndef adc_H_
#define adc_H_

/* macro ---------------------------------------------------------------------*/
/* Definition ----------------------------------------------------------------*/
typedef enum {
	ADC1_PA0 = 0,
	ADC1_PA1,
	ADC1_PA2,
	ADC1_PA3,
	ADC1_PA4,
	ADC1_PA5,
	ADC1_PA6,
	ADC1_PA7,
	ADC1_PB0,
	ADC1_PB1,
	ADC1_NUM
} xAdcChannel;


#endif /* adc_H_ */