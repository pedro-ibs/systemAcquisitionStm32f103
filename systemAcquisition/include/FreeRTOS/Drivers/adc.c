/**
 * adc.c
 *
 *  @date Created at:	26/02/2021 10:53:26
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 *   Copyright (C) Pedro Igor B. S 2021
 * -------------------------------------------------------------------
 *
 *   Licença: GNU GPL 2
 * -------------------------------------------------------------------
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; version 2 of the
 *   License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * -------------------------------------------------------------------
 * ########################################################
 *
 * Esse driver é dedicado para trabalhar com aquisição de
 * dados. e faz uso de 3 prerifericos do micro controlador
 * ADC1, TIM3 e DMA1 canal 1.
 * 
 * O TIM3 é responsável por cadenciar a leiruta dos canais
 * do ADC1. Toda vez que o TIM3 tem um estouro no contador
 * uma interrupção é gerada e os canais habilitados são 
 * lidos. O DMA por sua vez copia os dados do ADC1 para um
 * buffer apos o termino da leitura, e quando esse buffer
 * for completamente populado o DMA dera outra interrupção
 * onde é possível acessar e trabalhar com os dados coletados.
 * 
 * ###################### COMO USAR ######################
 * 
 * use  a macro  ADC1_XXX_RANK para configurar o quais canais
 * serão utilizado na aquisição. para desativar o canal coloque
 * "ADC1_CHANNEL_DISABLE", e para ativar "ADC_REGULAR_RANK_X"
 * de 1 a 10. Os RANKS devem ser inseridos sempre em ordem
 * crescente, NÃO PULE RANKS.
 * 
 * Apos isso "ADC1_RANK_NUM" deve ser configurado om o número
 * de canais ativos. e ADC1_SIZE_BUFFER deve ser configurado
 * com o tamanho do buffer desejado, ele deve ser sempre um
 * numero multiplo de "ADC1_RANK_NUM" e nunca menor.
 * 
 * O "ADC1_XXX_SAMPLETIME" é o tempo de clock dedicado para
 * a aquisição de cafa canal, quando maior o tempo mais precisa
 * será a medida.
 * 
 * A função adc1_vInitGetSample() inicializa todo o driver,
 * nela ceve ser configurado o estrouro do timer 3 (TIM3) o
 * que determina a velocidade de aquisição.
 * 
 * A função adc1_iGetValue os primeiros dados armazenado no
 * buffer.
 * 
 * 
 * Cada posição do BUFFER se refere um canal do adc1 (de
 * acordo com o RANK). Por exemplo utilizando os 10 canais e com
 * o buffer de tamanho 30 vamos ter um vetor assim:
 * 	ch0 ch1 ch2 ch3 ch4 ch5 ch6 ch7 ch8 ch9
 *	ch0 ch1 ch2 ch3 ch4 ch5 ch6 ch7 ch8 ch9
 *	ch0 ch1 ch2 ch3 ch4 ch5 ch6 ch7 ch8 ch9
 * Onde cada chX é um índice do vetor que vai de 0 a 29.
 * e 0 a 9, corespondem sequencialmente a um canal do ADC1
 * 
 */



/* Includes ------------------------------------------------------------------*/
/* stm includes */
#include <core/includes.h>
#include <stm32f1xx_hal_adc.h>
#include <stm32f1xx_hal_dma.h>


/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>

/* include driver */
#include <FreeRTOS/Drivers/adc.h>
#include <FreeRTOS/Drivers/gpio.h>
#include <FreeRTOS/Drivers/timer.h> 

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
typedef struct {
	cu8	uGPIO;
	cu32	uChannel;
	cu32	uRANK;
	cu32	uSamplingTime;
} xAdcGpio;


static xAdcGpio pxAdcCH[ADC1_NUM] = {
	{GPIOA0, ADC_CHANNEL_0, ADC1_PA0_RANK, ADC1_PA0_SAMPLETIME },
	{GPIOA1, ADC_CHANNEL_1, ADC1_PA1_RANK, ADC1_PA1_SAMPLETIME },
	{GPIOA2, ADC_CHANNEL_2, ADC1_PA2_RANK, ADC1_PA2_SAMPLETIME },
	{GPIOA3, ADC_CHANNEL_3, ADC1_PA3_RANK, ADC1_PA3_SAMPLETIME },
	{GPIOA4, ADC_CHANNEL_4, ADC1_PA4_RANK, ADC1_PA4_SAMPLETIME },
	{GPIOA5, ADC_CHANNEL_5, ADC1_PA5_RANK, ADC1_PA5_SAMPLETIME },
	{GPIOA6, ADC_CHANNEL_6, ADC1_PA6_RANK, ADC1_PA6_SAMPLETIME },
	{GPIOA7, ADC_CHANNEL_7, ADC1_PA7_RANK, ADC1_PA7_SAMPLETIME },
	{GPIOB0, ADC_CHANNEL_8, ADC1_PB0_RANK, ADC1_PB0_SAMPLETIME },
	{GPIOB1, ADC_CHANNEL_9, ADC1_PB1_RANK, ADC1_PB1_SAMPLETIME }
};

static ADC_HandleTypeDef xAdc1			= { 0 };
static DMA_HandleTypeDef xDmaAdc1		= { 0 };

static u16 puAdc1Buffer[ADC1_SIZE_BUFFER]	= { 0 };
static u16 puAdc1Swap[ADC1_RANK_NUM]		= { 0 };

static SemaphoreHandle_t xAdc1Semaphore		= NULL;

/* Private Functions ---------------------------------------------------------*/
void adc1_vInitChannel(const AdcChannel cuChannel);
void adc1_vDeInitChannel(const AdcChannel cuChannel);
void adc1_vDMA1(void);
void adc1_vInitVar(void);


/**
 * @brief inicializa o processo de aquisição de dados
 * dos canais o adc1
 * @note, veja as configuração das macros dem adc.h
 * @param uPrescaler, divisor do contador.
 * @param uPeriod, contador do Timer 3
 */
void adc1_vInitGetSample(cu32 uPrescaler, cu32 uPeriod){

	adc1_vInitVar();
	xSemaphoreTake(xAdc1Semaphore, portMAX_DELAY);	
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

	for (AdcChannel xIdx=0; xIdx < ADC1_NUM; xIdx++) {
		if( pxAdcCH[xIdx].uRANK != ADC1_CHANNEL_DISABLE ){
			adc1_vInitChannel(xIdx);
		}
	}

	adc1_vDMA1();
	xSemaphoreGive(xAdc1Semaphore);

	tim3_vStartAdc1Trigger( uPrescaler, uPeriod );		
}


/**
 * @brief pega o acesso do adc1
 */
void adc1_vTakeAccess(){
	adc1_vInitVar();
	xSemaphoreTake(xAdc1Semaphore, portMAX_DELAY);	
}


/**
 * @brief devolve o acesso do adc1
 */
void adc1_vGiveAccess(){
	adc1_vInitVar();
	xSemaphoreGive(xAdc1Semaphore);
}

/**
 * @brief desativa acoleta de dados
 * @note desativa o ADC1 e canaisrespectivos DMA e TIMER
 * @param none
 */
void adc1_vDeInitGetSample(void){

	if(xAdc1Semaphore == NULL){
		return;
	}
	
	tim3_vDeinit();

	xSemaphoreTake(xAdc1Semaphore, portMAX_DELAY);	
	__HAL_RCC_ADC1_CLK_DISABLE();

	HAL_DMA_DeInit(xAdc1.DMA_Handle);

	for (AdcChannel xIdx=0; xIdx < ADC1_NUM; xIdx++) {
		if( pxAdcCH[xIdx].uRANK != ADC1_CHANNEL_DISABLE ){
			adc1_vDeInitChannel(xIdx);
		}
	}
	xSemaphoreGive(xAdc1Semaphore);
}


/**
 * @brief Lé o primeiro valor de um "canal" / "RANK" do ADC1
 * armazenado no buffer.
 * @note a vergável swap (variavel de troca) é atualizada 
 * pela função e interrupção HAL_ADC_ConvCpltCallback
 * @param cuChannel, de ADC1_PA0 até ADC1_PA7. ADC1_PB0
 * e ADC1_PB1, caso o ADC_PXX seja invalido a função
 * não irá ser executada
 * @return -1, caso a leitura seja de um canal invalido
 * ou não habilitado
 */
int adc1_iGetFirstValue(const AdcChannel cuChannel){
	if(pxAdcCH[cuChannel].uRANK == ADC1_CHANNEL_DISABLE){
		return -1;
	}

	if(xAdc1Semaphore == NULL){
		return -1;
	}

	xSemaphoreTake(xAdc1Semaphore, portMAX_DELAY);

	u16 uSwap = 0x0000U;
	
	uSwap = puAdc1Swap[pxAdcCH[ cuChannel].uRANK - 1 ];
	
	xSemaphoreGive(xAdc1Semaphore);
	return uSwap;
}



/**
 *  
 * @brief copia o buffer do adc para outro
 * @note utilize essa função dentro de acd1_vBufferDoneHandler para copar
 * e trabalhe com o buffer copiado fora dela
 * @param puSaveAt onde o buffer do adc1 será salvo
 * @return (u16*): retorna puSaveAt
 * 
 */
u16 *adc1_puCopyBuffer( u16 *puSaveAt ){
	memcpy (puSaveAt, puAdc1Buffer, ADC1_SIZE_BUFFER);
	return puSaveAt;
}



/**
 * @brief retorne o endereço do buffer de aquisição
 * do adc1
 * @param none
 * @return (const u16*)
 */
const u16 *adc1_puGetBuffer(void){
	return (const u16*)puAdc1Buffer;
}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/



/**
 * @brief iniciar/configura o canal do periferico ADC1
 * @param cuChannel, de ADC1_PA0 até ADC1_PA7. ADC1_PB0
 * e ADC1_PB1
 */
void adc1_vInitChannel(const AdcChannel cuChannel){
	gpio_vAnalogMode(pxAdcCH[cuChannel].uGPIO);

	ADC_ChannelConfTypeDef xConfig		= {0};

	xConfig.Channel				= pxAdcCH[cuChannel].uChannel;
	xConfig.Rank				= pxAdcCH[cuChannel].uRANK;
	xConfig.SamplingTime			= pxAdcCH[cuChannel].uSamplingTime;
	
	if (HAL_ADC_ConfigChannel(&xAdc1, &xConfig) != HAL_OK) {
		Error_Handler();
	}
}



/**
 * @brief Desativa o canal do periferico ADC1
 * @param cuChannel, de ADC1_PA0 até ADC1_PA7. ADC1_PB0
 * e ADC1_PB1
 */
void adc1_vDeInitChannel(const AdcChannel cuChannel){
	gpio_vDeinit(cuChannel);	
}



/**
 * @brief Inicializa as variaveis utilizada no driver adc.h
 * @param none
 */
void adc1_vInitVar(void){
	if(xAdc1Semaphore == NULL){
		xAdc1Semaphore = xSemaphoreCreateMutex();
	}
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
	xDmaAdc1.Init.Priority			= DMA_PRIORITY_VERY_HIGH;
	
	if (HAL_DMA_Init(&xDmaAdc1) != HAL_OK) {
		Error_Handler();
	}

	__HAL_LINKDMA(&xAdc1, DMA_Handle, xDmaAdc1);

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, ADC1_DMA1_NVIC_PRIORITY, ADC1_DMA1_NVIC_SUBPRIORITY);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);


	HAL_ADC_Start_DMA(&xAdc1, (uint32_t*)&puAdc1Buffer, ADC1_SIZE_BUFFER);
}




/** ################################################################################################ **/
/** ######################################### Interrupções ######################################### **/
/** ################################################################################################ **/


void DMA1_Channel1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&xDmaAdc1);
	long xHigherPriorityTaskWoken = pdFALSE;
	adc1_vDMA1Ch1Handler(&xHigherPriorityTaskWoken);
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
	
	acd1_vBufferDoneHandler(&xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}