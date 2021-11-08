/**
 * macrosAppConfig.h
 *
 *  @date Created at:	23/03/2021 15:28:06
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 * TODO: Licence
 * ########################################################
 *
 * esse documento é dedicado para configurações clobais da aplicação
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "hardware.h"
#include "FreeRTOS/include/FreeRTOSConfig.h"

#ifndef macros_app_config_H_
#define macros_app_config_H_

/* macro ---------------------------------------------------------------------*/

#define FREQUENCY_HZ		( 25600					)
#define TIME_TO_GET_S		( 296					)
#define SAMPLE_SIZE		( ( FREQUENCY_HZ*TIME_TO_GET_S ) / 1000	)


#define SAMPLES_MAX		( 12 )



#define CMD_START		"_START_"
#define CMD_STOP		"_STOP_"
#define SIGINAL_FINISH		"_END_" 
#define SIGINAL_COLUMNS 	"ADC,SAMPLE,VALUE\n" 




/* Definition ----------------------------------------------------------------*/
#endif /* macros_app_config_H_ */