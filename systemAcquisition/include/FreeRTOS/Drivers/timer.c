/*
 * timer3.c.c
 *
 *  Created at:		05/03/2021 16:11:33
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
#include <stm32f1xx_hal_tim.h>



/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>

/* include driver */
#include<FreeRTOS/Drivers/timer.h>

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TIM_HandleTypeDef xTim3			= { 0 };
static SemaphoreHandle_t xTim3Semaphore		= NULL;

/* Private Functions ---------------------------------------------------------*/
void tim3_vInitVar(void);

/**
 * @brief Configura e inicializa do trigger do Timer 3
 * responsável cadenciar a aquisição de dados do ADC1 
 * @param uPrescaler, divisor do contador.
 * @param uPeriod, contador do Timer 3
 */
void tim3_vStartAdc1Trigger(cu32 uPrescaler, cu32 uPeriod) {

	tim3_vInitVar();
	xSemaphoreTake(xTim3Semaphore, portMAX_DELAY);	

	TIM_ClockConfigTypeDef xClockSourceConfig	= {0};
	TIM_MasterConfigTypeDef xMasterConfig		= {0};
	TIM_OC_InitTypeDef xConfigOC			= {0};

	__HAL_RCC_TIM3_CLK_ENABLE();

	xTim3.Instance					= TIM3;
	xTim3.Init.Prescaler				= uPrescaler;
	xTim3.Init.CounterMode				= TIM_COUNTERMODE_UP;
	xTim3.Init.Period				= uPeriod;
	xTim3.Init.ClockDivision			= TIM_CLOCKDIVISION_DIV1;
	xTim3.Init.AutoReloadPreload			= TIM_AUTORELOAD_PRELOAD_ENABLE;

	if (HAL_TIM_Base_Init(&xTim3) != HAL_OK) {
		Error_Handler();
	}

	xClockSourceConfig.ClockSource			= TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&xTim3, &xClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_TIM_OC_Init(&xTim3) != HAL_OK) {
		Error_Handler();
	}

	xMasterConfig.MasterOutputTrigger		= TIM_TRGO_UPDATE;
	xMasterConfig.MasterSlaveMode			= TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&xTim3, &xMasterConfig) != HAL_OK){
		Error_Handler();
	}

	xConfigOC.OCMode				= TIM_OCMODE_TIMING;
	xConfigOC.Pulse					= 0;
	xConfigOC.OCPolarity				= TIM_OCPOLARITY_HIGH;
	xConfigOC.OCFastMode				= TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&xTim3, &xConfigOC, TIM_CHANNEL_2) != HAL_OK){
		Error_Handler();
	}
	
	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

	HAL_TIM_Base_Start_IT(&xTim3);
	xSemaphoreGive(xTim3Semaphore);

}

/**
 * @brief Desativa o timer 3
 * @param none
 * 
 */
void tim3_vDeinit( void ){
	xSemaphoreTake(xTim3Semaphore, portMAX_DELAY);
	__HAL_RCC_TIM3_CLK_DISABLE();
	HAL_TIM_Base_Stop_IT(&xTim3);
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
	HAL_TIM_Base_DeInit(&xTim3);
	xSemaphoreGive(xTim3Semaphore);


}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/


/**
 * @brief Inicializa as variaveis utilizada no driver adc.h
 * @param none
 */
void tim3_vInitVar(void){
	if(xTim3Semaphore == NULL){
		xTim3Semaphore = xSemaphoreCreateMutex();
	}
}


/** ################################################################################################ **/
/** ######################################### Interrupções ######################################### **/
/** ################################################################################################ **/


void TIM3_IRQHandler(void) {
	HAL_TIM_IRQHandler(&xTim3);
	long xHigherPriorityTaskWoken = pdFALSE;
	tim3_vHandler(&xHigherPriorityTaskWoken);	
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}