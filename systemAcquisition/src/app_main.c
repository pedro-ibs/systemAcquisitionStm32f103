/**
 * app_main.c
 *
 *  @date Created at:	13/07/2021 14:09:30
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
 * Os pinos e serial usados estão definidos em hardware.h
 * Demais configurações estão em consfig.h 
 * 
 * funcionameno:
 * 
 * * Inicia o freeRTOS.
 * 	Inicia os drives e configura o MCU isso ocorre em main.c
 * 	Configurações avançadas do sietema encontram se em:
 * 		FreeRTOS/include/FreeRTOSConfig.h
 * 	
 * 
 * * Inicia o setup da aplicação
 * 	Antes do loop é enviado ascolunas do csv.
 * 
 * * Loop:
 * 	O Semaforo é utilizado para controle de acesso do buffer entre o
 * 	loop app_main e o conjundo de aquisição (sendo eles o ADC1, DMA e
 * 	TIM3) alem disso detemina o fluxo de operação, como se fosse uma
 * 	flag.
 * 
 * 	Quando o semaforo é devolvido pelo conjundo de aquisição o loop
 * 	do app_main pega o acesso e trasmite os dados, e imediatamente após
 * 	o logo apos devolver o semaforo o conjunto de aquisição pega o semaforo
 * 	e começa a coleta de dados e devolve o semaforo e o loop se inicia.
 * 
 * 	NOTA: Enquanto o conjunto de aquisição estiver om o semaforo o loop 
 * 	ficará travado. 
 * 
 */



/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include <config.h>
#include <core.h>
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/includes.h>

/* Private macro ----------------------------------------------------------------------------------------------------------------------------------------*/
/* Private variables ------------------------------------------------------------------------------------------------------------------------------------*/
static const AdcChannel psxAdc[] 	= {CH1, CH2, CH3};	// Canais que serão lidos 
static u16 spuBuffer[SAMPLE_SIZE]	= { 0 };		// Buffer de aquisição
static SemaphoreHandle_t spxSemaphore	= NULL;			// Samaforo usado como MUTEX e flag
static u8 suIdx 			= 0;			// Indice do canal adc


/* Private Functions ------------------------------------------------------------------------------------------------------------------------------------*/
void app_vSatartGetSample(const AdcChannel cxChannel);
void app_vStopGetSample(const AdcChannel cxChannel, BaseType_t *const pxHigherPriorityTaskWoken);
void app_vWaitComand(const char * cpcCommand);
/* Functions --------------------------------------------------------------------------------------------------------------------------------------------*/


void main_vApp(void * pvParameters){
	(void) pvParameters;

	if ( spxSemaphore == NULL){
		spxSemaphore = xSemaphoreCreateMutex();
	}

	// usart_vSetup(STDIO,	usart_115k2bps);
	usart_vSetup(STDIO,	usart_500k0bps);
	
	gpio_vMode(LED, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL);

	for (u16 uIdx = 0; uIdx < SAMPLE_SIZE; uIdx++) {
		spuBuffer[uIdx] = 0;
	}
	

	vTaskDelay(_3S);

	u8 uSample = 0;

	while (TRUE) {

		/**
		 * Iniciar a o programa pele primeira vez
		 * 
		 * envie o comando _START_ para inicar 
		 */
		if(uSample == 0){
			uSample = 1;

			usart_vSendStr(STDIO, "Wait command to satart aquisition\n");		
			
			app_vWaitComand(CMD_START);
			
			usart_vSendStr(STDIO, SIGINAL_COLUMNS);
		}


		/**
		 * trocar o canal ADC apos atingir  SAMPLES_MAX. Ao completar
		 * o ciclo é preciso enviar o comando _START_ novamente
		 */
		if( uSample > SAMPLES_MAX) {
			uSample = 1;
			suIdx++;
			if(suIdx >= 3) suIdx = 0;

			usart_vSendStr(STDIO, SIGINAL_FINISH);

			app_vWaitComand(CMD_START);

			usart_vSendStr(STDIO, SIGINAL_COLUMNS);
		}

		app_vSatartGetSample(psxAdc[suIdx]);

		/**
		 * Para que a trasmissão prossiga o DMA tem que devolver 
		 * o semaforo 
		 */
		if(xSemaphoreTake(spxSemaphore, portMAX_DELAY) == pdPASS){
			char puSwap[20];


			/**
			 * mostrar valores lidos no formato CSV
			 */
			for (u16 uIdx = 0; uIdx < SAMPLE_SIZE; uIdx++) {
				usart_vSendStr(STDIO, itoa(suIdx, puSwap, HEX));
				usart_vSendChr(STDIO, ',');
				usart_vSendStr(STDIO, itoa(uSample, puSwap, HEX));
				usart_vSendChr(STDIO, ',');
				usart_vSendStr(STDIO, itoa(spuBuffer[uIdx], puSwap, HEX));
				usart_vSendChr(STDIO, '\n');
				spuBuffer[uIdx] = 0;
			}

			uSample++;

			xSemaphoreGive(spxSemaphore);
		}
	}
}



/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/


void app_vWaitComand(const char * cpcCommand){
	while (TRUE) {
		if (usart_iSizeBuffer(STDIO) > 1){
			/**
			 *  esperar receber todos bytes
			 */
			vTaskDelay(_200MS);
		
			if ( textp_bFindString(usart_pcGetBuffer(STDIO), cpcCommand) ){
				/**
				 * apenas inicia a aquisição por dma caso não 
				 * esteja travada 
				 */
				break;
			}

		}
		vTaskDelay(_200MS);
	}

	usart_vCleanBuffer(STDIO);
}

void app_vSatartGetSample(const AdcChannel cxChannel){
	if(xSemaphoreTake(spxSemaphore, portMAX_DELAY) == pdPASS){
		adc1_vSetGetSampleMode(1);
		adc1_vSetChannel(cxChannel, ADC_SAMPLETIME_1CYCLE_5,  ADC_REGULAR_RANK_1);
		adc1_vStartGetSampleMode( spuBuffer, SAMPLE_SIZE, 1, FREG_TO_COUNTER(FREQUENCY_HZ, 1) );
		gpio_vWrite(LED, TRUE);
	}
}

void app_vStopGetSample(const AdcChannel cxChannel, BaseType_t *const pxHigherPriorityTaskWoken){
	adc1_vDeInitChannel(cxChannel);
	adc1_vDeInitFromISR(pxHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(spxSemaphore, pxHigherPriorityTaskWoken);
	gpio_vWrite(LED, FALSE);
}



void tim3_vHandler(BaseType_t *const pxHigherPriorityTaskWoken){
(void) pxHigherPriorityTaskWoken;
}

void adc1_vDMA1Ch1Handler(BaseType_t *const pxHigherPriorityTaskWoken){
	(void) pxHigherPriorityTaskWoken;
}

void acd1_vBufferDoneHandler(BaseType_t *const pxHigherPriorityTaskWoken){
	app_vStopGetSample(psxAdc[suIdx], pxHigherPriorityTaskWoken);
}
