/*
 * rtc.c.c
 *
 *  Created at:		03/03/2021 16:10:56
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
/* stm includes */
#include <core/includes.h>
#include <stm32f1xx_hal_rtc.h>
#include <stm32f1xx_hal_rtc_ex.h>


/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>

/* include driver */
#include <FreeRTOS/Drivers/rtc.h>


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef xRtc		= { 0 };


/* Private Functions ---------------------------------------------------------*/



/**
 * @brief inicializa o periferico RTC com clock interno 
 * A data inicial é 20 de Julio de 1969, as 23:56:19
 * 
 */
void rtc_vInit(void){

	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_BKP_CLK_ENABLE();
	__HAL_RCC_RTC_ENABLE();	

	/** Inicar apenas o RTC **/
	xRtc.Instance		= RTC;
	xRtc.Init.AsynchPrediv	= RTC_AUTO_1_SECOND;
	xRtc.Init.OutPut 	= RTC_OUTPUTSOURCE_NONE;
	if (HAL_RTC_Init(&xRtc) != HAL_OK) {
		Error_Handler();
	}


	/** Iniciar RTC e setar o tempo e data **/
	RTC_TimeTypeDef xTime		= { 0 };
	xTime.Hours			= 0x23;
	xTime.Minutes			= 0x56;
	xTime.Seconds			= 0x19;
	if (HAL_RTC_SetTime(&xRtc, &xTime, RTC_FORMAT_BCD) != HAL_OK) {
		Error_Handler();
	}

	RTC_DateTypeDef xDateToUpdate	= { 0 };
	xDateToUpdate.WeekDay		= RTC_WEEKDAY_SUNDAY;
	xDateToUpdate.Month		= RTC_MONTH_JULY;
	xDateToUpdate.Date		= 0x20;
	xDateToUpdate.Year		= 0x69;
	if (HAL_RTC_SetDate(&xRtc, &xDateToUpdate, RTC_FORMAT_BCD) != HAL_OK) {
		Error_Handler();
	}
}



/**
 * @brief lé a quantidade de segunsdos que uma task está ativa.
 * @note utiliza xTaskGetTickCount, que lê o valor do cotador
 * de ticks de uma task. como o FreeRTOS está configurado para
 * contar um tick acada 1 ms essa função pega a milezima parte
 * disso.
 * @param none.
 * @return segundos, [TickType_t]
 * 
 */
TickType_t rtc_xTaskGetSeconds( void ){
	return xTaskGetTickCount() / 1000;
}


/**
 * @brief ler tempo, horas, minutos e segundos
 * @param none
 * @return RTC_TimeTypeDef
 */
RTC_TimeTypeDef rtc_xGetTime(void){
	RTC_TimeTypeDef xTime = { 0 };
	HAL_RTC_GetTime(&xRtc, &xTime, RTC_FORMAT_BCD);
	return xTime;
}


/**
 * @brief ler dia, dia da semana, mes e ano 
 * @param none
 * @return RTC_DateTypeDef
 */
RTC_DateTypeDef rtc_xGetData(void){
	RTC_DateTypeDef xDate = { 0 };
	HAL_RTC_GetDate(&xRtc, &xDate, RTC_FORMAT_BCD);
	return xDate;
}



/**
 * @brief monta um time stamp em formato de uma string como 20/07/69-20:56:19
 * @param xDate: RTC_DateTypeDef, data como dia, dia da semana, mes e ano
 * @param xTime: RTC_TimeTypeDef, tempo como tempo, horas, minutos e segundos
 * @param pcBuffer: vetor ondo o time stamp será montado
 * @return pcBuffer com a data montada
 */
CCHR *rtc_pcBuildTimeStamp(const RTC_DateTypeDef xDate, const RTC_TimeTypeDef xTime, char *pcBuffer){
	char pcSwap[4];

	// dia
	itoa(xDate.Date, pcSwap, HEX);
	strcat(pcBuffer, pcSwap);
	strcat(pcBuffer, "/");

	// mes
	itoa(xDate.Month, pcSwap, HEX);
	strcat(pcBuffer, pcSwap);
	strcat(pcBuffer, "/");

	// ano
	itoa(xDate.Year, pcSwap, HEX);
	strcat(pcBuffer, pcSwap);
	strcat(pcBuffer, "-");

	// horas
	itoa(xTime.Hours, pcSwap, HEX);
	strcat(pcBuffer, pcSwap);
	strcat(pcBuffer, ":");

	// minutos
	itoa(xTime.Minutes, pcSwap, HEX);
	strcat(pcBuffer, pcSwap);
	strcat(pcBuffer, ":");

	// segundos
	itoa(xTime.Seconds, pcSwap, HEX);
	strcat(pcBuffer, pcSwap);
	strcat(pcBuffer, "");

	return pcBuffer;
}


/**
 * @brief monta o time stamp em formato de uma string como 20/07/69-20:56:19
 * de acordo com a tata configurada no sistema.
 * @param pcBuffer: vetor ondo o time stamp será montado
 * @return pcBuffer com a data montada
 */
CCHR *rtc_pcGetTimeStamp(char *pcBuffer){
	 return rtc_pcBuildTimeStamp( rtc_xGetData(), rtc_xGetTime(), pcBuffer);
}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/
