/*
 * adc.c
 *
 *  Created at:		26/02/2021 10:53:26
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
#include <FreeRTOS/Drivers/adc.h>
#include <FreeRTOS/Drivers/gpio.h>


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct {
	cu8	uGPIO;
	cu32	xChannel;
} xAdcGpio;

static const xAdcGpio xAdcCH[ADC1_NUM] = {
	{GPIOA0, ADC_CHANNEL_0},
	{GPIOA1, ADC_CHANNEL_1},
	{GPIOA2, ADC_CHANNEL_2},
	{GPIOA3, ADC_CHANNEL_3},
	{GPIOA4, ADC_CHANNEL_4},
	{GPIOA5, ADC_CHANNEL_5},
	{GPIOA6, ADC_CHANNEL_6},
	{GPIOA7, ADC_CHANNEL_7},
	{GPIOB0, ADC_CHANNEL_8},
	{GPIOB1, ADC_CHANNEL_9}
};

static ADC_HandleTypeDef xAdc1;



/* Private Functions ---------------------------------------------------------*/



// adc_vInit(void){
// }

/**
 * @brief inicial/configura o canal do periferico adc1
 * @param xChannel, de ADC1_PA0 até ADC1_PA7. ADC1_PB0
 * e ADC1_PB1, caso o ADC_PXX seja invalido a função
 * não irá ser executada
 */
void adc_vInitChannel(const xAdcChannel xChannel){
	if(xChannel >= ADC1_NUM ) return;
	gpio_vMode(xAdcCH[xChannel].uGPIO, GPIO_MODE_ANALOG, GPIO_NOPULL);
	ADC_ChannelConfTypeDef xConfig = {0};
	xConfig.Channel		= ADC_CHANNEL_0;
	xConfig.Rank		= ADC_REGULAR_RANK_1;
	xConfig.SamplingTime	= ADC_SAMPLETIME_1CYCLE_5;
	
	if (HAL_ADC_ConfigChannel(&xAdc1, &xConfig) != HAL_OK) {
		Error_Handler();
	}
}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/
