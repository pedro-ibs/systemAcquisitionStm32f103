/**
 * hardwareConfig.h
 *
 *  @date Created at:	25/03/2021 12:06:37
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 * TODO: Licence
 * ########################################################
 *
 * Esse arquivo é dedicado para definir as entradas e saidas de hardware
 * do micro controlador
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <FreeRTOS/Drivers/all.h>


#ifndef hardware_config_H_
#define hardware_config_H_

/* macro ---------------------------------------------------------------------*/
#define STDIO		( ttyUSART1 )

#define LED 		( GPIOC13 )

#define	CH1		( ADC1_PA0 )
#define	CH2		( ADC1_PA1 )
#define	CH3		( ADC1_PA2 )


/* Definition ----------------------------------------------------------------*/
#endif /* hardware_config_H_ */
