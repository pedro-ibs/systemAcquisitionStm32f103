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
/* stm includes */
#include <core/includes.h>
#include <FreeRTOS/Drivers/adc.h>
#include <FreeRTOS/Drivers/gpio.h> 
#include <stm32f1xx_hal_adc.h>


/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>
#include <FreeRTOS/Drivers/uart.h>

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct {
	cu8	uGPIO;
	cu32	xChannel;
	cu32	xRANK;
	cu32	uSamplingTime;
} xAdcGpio;

static const xAdcGpio xAdcCH[ADC1_NUM] = {
	{GPIOA0, ADC_CHANNEL_0, ADC1_PA0_RANK,	ADC1_PA0_SAMPLETIME },
	{GPIOA1, ADC_CHANNEL_1, ADC1_PA1_RANK,	ADC1_PA1_SAMPLETIME },
	{GPIOA2, ADC_CHANNEL_2, ADC1_PA2_RANK,	ADC1_PA2_SAMPLETIME },
	{GPIOA3, ADC_CHANNEL_3, ADC1_PA3_RANK,	ADC1_PA3_SAMPLETIME },
	{GPIOA4, ADC_CHANNEL_4, ADC1_PA4_RANK,	ADC1_PA4_SAMPLETIME },
	{GPIOA5, ADC_CHANNEL_5, ADC1_PA5_RANK,	ADC1_PA5_SAMPLETIME },
	{GPIOA6, ADC_CHANNEL_6, ADC1_PA6_RANK,	ADC1_PA6_SAMPLETIME },
	{GPIOA7, ADC_CHANNEL_7, ADC1_PA7_RANK,	ADC1_PA7_SAMPLETIME },
	{GPIOB0, ADC_CHANNEL_8, ADC1_PB0_RANK,	ADC1_PB0_SAMPLETIME },
	{GPIOB1, ADC_CHANNEL_9, ADC1_PB1_RANK,	ADC1_PB1_SAMPLETIME }
};

static ADC_HandleTypeDef xAdc1			= { 0 };
static DMA_HandleTypeDef xDmaAdc1		= { 0 };
static TIM_HandleTypeDef xTim3			= { 0 };

static u16 puAdc1Buffer[ADC1_SIZE_BUFFER]	= { 0 };
static u16 puAdc1Swap[ADC1_RANK_NUM]		= { 0 };


static SemaphoreHandle_t xAdc1Semaphore		= NULL;

/* Private Functions ---------------------------------------------------------*/
void adc1_vInitChannel(const xAdcChannel cxChannel);
void adc1_vInitTIM3(cu32 uPrescaler, cu32 uPeriod);
void adc1_vDMA1(void);
void adc_vInitVar(void);

/**
 * @brief inicializa o processo de aquisição de dados
 * dos canais o adc1
 * @note, veja as configuração das macros dem adc.h
 */
void adc1_vInitGetSample(void){

	adc_vInitVar();

	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	xAdc1.Instance				= ADC1;
	xAdc1.Init.ScanConvMode			= ADC_SCAN_ENABLE;
	xAdc1.Init.ContinuousConvMode		= DISABLE;
	xAdc1.Init.DiscontinuousConvMode	= DISABLE;
	xAdc1.Init.ExternalTrigConv		= ADC_EXTERNALTRIGCONV_T3_TRGO;
	xAdc1.Init.DataAlign			= ADC_DATAALIGN_RIGHT;
	xAdc1.Init.NbrOfConversion 		= ADC1_RANK_NUM;

	if (HAL_ADC_Init(&xAdc1) != HAL_OK) {
		Error_Handler();
	}

	for (xAdcChannel xIdx=0; xIdx < ADC1_NUM; xIdx++){
		if( xAdcCH[xIdx].xRANK != ADC1_CHANNEL_DISABLE ){
			adc1_vInitChannel(xIdx);
		}
	}

	adc1_vDMA1();
	adc1_vInitTIM3( 1000, 65535 );	
}


/**
 * @brief Lé o primeiro valor de um "canal" / "RANK" do ADC1
 * armazenado no buffer.
 * @note a vergável swap (variavel de troca) é atualizada 
 * pela função e interrupção HAL_ADC_ConvCpltCallback
 * @param cxChannel, de ADC1_PA0 até ADC1_PA7. ADC1_PB0
 * e ADC1_PB1, caso o ADC_PXX seja invalido a função
 * não irá ser executada
 * @return -1, caso a leitura seja de um canal invalido
 * ou não habilitado
 */
int adc1_iGetValue(const xAdcChannel cxChannel){
	if(cxChannel >= ADC1_NUM){
		return -1;
	}

	if(xAdcCH[cxChannel].xRANK == ADC1_CHANNEL_DISABLE){
		return -1;
	}

	int iSwap = -1;
	xSemaphoreTake(xAdc1Semaphore, portMAX_DELAY);
	for (size_t uRank=0; uRank<ADC1_RANK_NUM; uRank++) {
		for (xAdcChannel xIdx=0; xIdx < ADC1_NUM; xIdx++){		
			if( xAdcCH[xIdx].xRANK != ADC1_CHANNEL_DISABLE ){
				if(xIdx == cxChannel){
					iSwap = puAdc1Swap[uRank];
				}
			}
		}
	}
	xSemaphoreGive(xAdc1Semaphore);
	return iSwap;
}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/



/**
 * @brief inicial/configura o canal do periferico adc1
 * @param cxChannel, de ADC1_PA0 até ADC1_PA7. ADC1_PB0
 * e ADC1_PB1, caso o ADC_PXX seja invalido a função
 * não irá ser executada
 */
void adc1_vInitChannel(const xAdcChannel cxChannel){
	gpio_vAnalogMode(xAdcCH[cxChannel].uGPIO);

	ADC_ChannelConfTypeDef xConfig	= {0};

	xConfig.Channel			= xAdcCH[cxChannel].xChannel;
	xConfig.Rank			= xAdcCH[cxChannel].xRANK;
	xConfig.SamplingTime		= xAdcCH[cxChannel].uSamplingTime;
	
	if (HAL_ADC_ConfigChannel(&xAdc1, &xConfig) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief Inicializa as variaveis utilizada no driver adc.h
 * @param none
 */
void adc_vInitVar(void){
	if(xAdc1Semaphore == NULL){
		xAdc1Semaphore = xSemaphoreCreateMutex();
	}
}


/**
 * @brief Configura e inicializa do Timer 3
 * @param uPrescaler, divisor do contador.
 * @param uPeriod, contador do Timer 3
 */
void adc1_vInitTIM3(cu32 uPrescaler, cu32 uPeriod){
	TIM_ClockConfigTypeDef xClockSourceConfig	= {0};
	TIM_MasterConfigTypeDef xMasterConfig		= {0};
	TIM_OC_InitTypeDef xConfigOC			= {0};

	__HAL_RCC_TIM3_CLK_ENABLE();

	xTim3.Instance				= TIM3;
	xTim3.Init.Prescaler			= uPrescaler;
	xTim3.Init.CounterMode			= TIM_COUNTERMODE_UP;
	xTim3.Init.Period			= uPeriod;
	xTim3.Init.ClockDivision		= TIM_CLOCKDIVISION_DIV1;
	xTim3.Init.AutoReloadPreload		= TIM_AUTORELOAD_PRELOAD_ENABLE;

	if (HAL_TIM_Base_Init(&xTim3) != HAL_OK) {
		Error_Handler();
	}

	xClockSourceConfig.ClockSource		= TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&xTim3, &xClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_TIM_OC_Init(&xTim3) != HAL_OK) {
		Error_Handler();
	}

	xMasterConfig.MasterOutputTrigger	= TIM_TRGO_UPDATE;
	xMasterConfig.MasterSlaveMode		= TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&xTim3, &xMasterConfig) != HAL_OK){
		Error_Handler();
	}

	xConfigOC.OCMode			= TIM_OCMODE_TIMING;
	xConfigOC.Pulse				= 0;
	xConfigOC.OCPolarity			= TIM_OCPOLARITY_HIGH;
	xConfigOC.OCFastMode			= TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&xTim3, &xConfigOC, TIM_CHANNEL_2) != HAL_OK){
		Error_Handler();
	}

	

	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

	HAL_TIM_Base_Start_IT(&xTim3);
}


/**
 * @brief Configura e inicializa do DMA
 * @param none
 */
void adc1_vDMA1(void){

	__HAL_RCC_DMA1_CLK_ENABLE();

	xDmaAdc1.Instance			= DMA1_Channel1;
	xDmaAdc1.Init.Direction			= DMA_PERIPH_TO_MEMORY;
	xDmaAdc1.Init.PeriphInc			= DMA_PINC_DISABLE;
	xDmaAdc1.Init.MemInc			= DMA_MINC_ENABLE;
	xDmaAdc1.Init.PeriphDataAlignment	= DMA_PDATAALIGN_HALFWORD;
	xDmaAdc1.Init.MemDataAlignment		= DMA_MDATAALIGN_HALFWORD;
	xDmaAdc1.Init.Mode			= DMA_CIRCULAR;
	xDmaAdc1.Init.Priority			= DMA_PRIORITY_LOW;
	
	if (HAL_DMA_Init(&xDmaAdc1) != HAL_OK) {
		Error_Handler();
	}

	__HAL_LINKDMA(&xAdc1, DMA_Handle, xDmaAdc1);

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	HAL_ADC_Start_DMA(&xAdc1, (uint32_t*)&puAdc1Buffer, ADC1_SIZE_BUFFER);
}

/** ################################################################################################ **/
/** ######################################### Interrupções ######################################### **/
/** ################################################################################################ **/

void DMA1_Channel1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&xDmaAdc1);
	long xHigherPriorityTaskWoken = pdFALSE;
	adc1_vDMA_IRQHandler(&xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

}

void TIM3_IRQHandler(void) {
	HAL_TIM_IRQHandler(&xTim3);
	long xHigherPriorityTaskWoken = pdFALSE;
	adc1_vTIM3_IRQHandler(&xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	long xHigherPriorityTaskWoken = pdFALSE;
	
	/**
	 * copia queue com os primeiro elementos do buffer 
	 */
	xSemaphoreTakeFromISR( xAdc1Semaphore, &xHigherPriorityTaskWoken );
	memcpy(puAdc1Swap, puAdc1Buffer, ( sizeof(u16)*ADC1_RANK_NUM ));
	xSemaphoreGive(xAdc1Semaphore);

	acd1_vBufferDone(&xHigherPriorityTaskWoken);

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}