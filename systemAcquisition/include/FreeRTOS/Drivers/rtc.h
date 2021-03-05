/*
 * rtc.h
 *
 *  Created at:		03/03/2021 16:09:39
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
#include<FreeRTOS/include/FreeRTOSConfig.h>

#ifndef rtc_H_
#define rtc_H_

/* macro ---------------------------------------------------------------------*/
/* Definition ----------------------------------------------------------------*/
void rtc_vInit(void);

TickType_t rtc_xTaskGetSeconds( void );

RTC_TimeTypeDef rtc_xGetTime(void);
RTC_DateTypeDef rtc_xGetData(void);

CCHR *rtc_pcBuildTimeStamp(const RTC_DateTypeDef xDate, const RTC_TimeTypeDef xTime, char *pcBuffer);
CCHR *rtc_pcGetTimeStamp(char *pcBuffer);

#endif /* rtc_H_ */



