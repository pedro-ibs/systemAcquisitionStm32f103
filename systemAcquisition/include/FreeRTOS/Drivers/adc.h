/**
 * adc.h
 *
 *  @date Created at:	26/02/2021 10:53:47
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
 *
 */

/* Includes ------------------------------------------------------------------*/

#include <core/includes.h>
#include <FreeRTOS/Drivers/gpio.h>

#ifndef adc_H_
#define adc_H_

/* macro ---------------------------------------------------------------------*/
#define ADC1_CHANNEL_DISABLE		((u32)0x00000000U)

/**
 * utilise essas macros para configurar os RANK de cada canal do adc1. 
 * O RANK determina quem será lido primeiro, do ADC_REGULAR_RANK_1 ao
 * ADC_REGULAR_RANK_10 sendo que ADC_REGULAR_RANK_1 será o primeiro a
 * ser lido e ADC_REGULAR_RANK_10 o ultimo a ser lido.
 * 
 * Para desativar o pino utilise ADC1_CHANNEL_DISABLE no lugar de
 * ADC_REGULAR_RANK_X
 * 
 * ############################ IMPORTANTE ############################
 * 
 * DEVE SER CONFIGURADO "ADC1_RANK_NUM" COM A QUANTIDADE DE CANIS SENDO
 * USADOS NO PROCESSO DE AQUISIÇÃO, LOGO SE TODOS OS PINOS ESTIVEREM 
 * SENDO USADOS O "ADC1_RANK_NUM" DEVE SER 10
 * 
 * OS RANKS CONFIGURADOS DEVEM SEGUIR A ORDEM CRESCENTE COMO
 * "1, 2, 3, ..." E NUNCA DEVE PULAR A ORDEM COMO "1, 3, 4, ..."
 * 
 * CONFIGURE A "ADC1_SIZE_BUFFER" COM UM NUMERO MULTIPLO DA QUANTIDADE
 * DE CANAIS USADOS POIS.
 */
#define ADC1_PA0_RANK		( ADC_REGULAR_RANK_1	)
#define ADC1_PA1_RANK		( ADC1_CHANNEL_DISABLE	)
#define ADC1_PA2_RANK		( ADC1_CHANNEL_DISABLE	)
#define ADC1_PA3_RANK		( ADC1_CHANNEL_DISABLE	)
#define ADC1_PA4_RANK		( ADC1_CHANNEL_DISABLE	)
#define ADC1_PA5_RANK		( ADC1_CHANNEL_DISABLE	)
#define ADC1_PA6_RANK		( ADC1_CHANNEL_DISABLE	)
#define ADC1_PA7_RANK		( ADC1_CHANNEL_DISABLE	)
#define ADC1_PB0_RANK		( ADC1_CHANNEL_DISABLE	)
#define ADC1_PB1_RANK		( ADC1_CHANNEL_DISABLE	)

/**
 * ADC1_RANK_NUM, é a quantidade de canais usados (canais 
 * habilitado), deve ser configurado de acordo com as
 * macros ateriores.
 * 
 * ADC1_SIZE_BUFFER é o tamanho do buffer que será populado
 * pelo DMA1. ele deve ser um numero multiplo de ADC1_RANK_NUM
 * pois  cada posição do buffer se refere um canal do adc1 (de
 * acordo com o RANK). Por exemplo utilizando os 10 canais e com
 * o buffer de tamanho 30 vamos ter um vetor assim:
 * 	ch0 ch1 ch2 ch3 ch4 ch5 ch6 ch7 ch8 ch9
 *	ch0 ch1 ch2 ch3 ch4 ch5 ch6 ch7 ch8 ch9
 *	ch0 ch1 ch2 ch3 ch4 ch5 ch6 ch7 ch8 ch9
 * Onde cada chX é um índice do vetor que vai de 0 a 29.
 * 
 */
#define ADC1_RANK_NUM 		( 1 )
#define ADC1_SIZE_BUFFER 	( ADC1_RANK_NUM * 7950 )

/**
 * Utilise essas macros para o tempo de amostragem de cada 
 * canal do adc por exemplo ADC_SAMPLETIME_7CYCLES_5 são
 * 7.5 ciclos de clock.
 */
#define ADC1_PA0_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )
#define ADC1_PA1_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )
#define ADC1_PA2_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )
#define ADC1_PA3_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )
#define ADC1_PA4_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )
#define ADC1_PA5_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )
#define ADC1_PA6_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )
#define ADC1_PA7_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )
#define ADC1_PB0_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )
#define ADC1_PB1_SAMPLETIME	( ADC_SAMPLETIME_7CYCLES_5 )


/**
 * prioridades das interruições do DMA1
 */
#define ADC1_DMA1_NVIC_PRIORITY		( 5 )
#define ADC1_DMA1_NVIC_SUBPRIORITY	( 5 )




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
} AdcChannel;

void adc1_vInitGetSample(cu32 uPrescaler, cu32 uPeriod);
void adc1_vDeInitGetSample(void);
int adc1_iGetFirstValue(const AdcChannel cuChannel);

u16 *adc1_puCopyBuffer( u16 *puSaveAt );
const u16 *adc1_puGetBuffer(void);

void adc1_vTakeAccess();
void adc1_vGiveAccess();


/**
 * @brief Função dentro da interrupção DMA1_Channel1_IRQHandler
 * @note não trada nenhuma flag de interrupção, porem
 * pxHigherPriorityTaskWoken é tratado no termino da interrupção  
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS
 * para o controle de troca de contexto em interruições.
 */
extern void adc1_vDMA1Ch1Handler(BaseType_t *const pxHigherPriorityTaskWoken);

/**
 * @brief Função dentro da HAL_ADC_ConvCpltCallback.
 * @note O DMA1 é esposável de por copiar os dados das leituras para
 * o buffer. Toda vez que esta função for chamada significa que o buffer
 * fou completamente populado. e está pronto para ser lido.
 * 
 * A HAL_ADC_ConvCpltCallback interrupção tambem é usara atualizar os dados
 * na swap usada na função adc1_iGetValue. ambas fazem uso de um mutex.
 * 
 * pxHigherPriorityTaskWoken é tratado no termino da interrupção
 * 
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS para o controle
 * de troca de contexto em interruições.
 */
extern void acd1_vBufferDoneHandler(BaseType_t *const pxHigherPriorityTaskWoken);

#endif /* adc_H_ */