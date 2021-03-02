/*
 * adc.h
 *
 *  Created at:		26/02/2021 10:53:47
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
#include <FreeRTOS/Drivers/gpio.h>

#ifndef adc_H_
#define adc_H_

/* macro ---------------------------------------------------------------------*/

#define ADC1_CHANNEL_DISABLE ((u32)0x00000000U)

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
#define ADC1_PA1_RANK		( ADC_REGULAR_RANK_2	)
#define ADC1_PA2_RANK		( ADC_REGULAR_RANK_3	)
#define ADC1_PA3_RANK		( ADC_REGULAR_RANK_4	)
#define ADC1_PA4_RANK		( ADC_REGULAR_RANK_5	)
#define ADC1_PA5_RANK		( ADC_REGULAR_RANK_6	)
#define ADC1_PA6_RANK		( ADC_REGULAR_RANK_7	)
#define ADC1_PA7_RANK		( ADC_REGULAR_RANK_8	)
#define ADC1_PB0_RANK		( ADC_REGULAR_RANK_9	)
#define ADC1_PB1_RANK		( ADC_REGULAR_RANK_10	)

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
 */
#define ADC1_RANK_NUM 		( 10 )
#define ADC1_SIZE_BUFFER 	( ADC1_RANK_NUM * 1 )

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

/* Definition ----------------------------------------------------------------*/
typedef enum
{
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
} xAdcChannel;

void adc1_vInitGetSample(void);
int adc1_iGetValue(const xAdcChannel cuChannel);

/**
 * @brief Função dentro da interrupção TIM3_IRQHandler
 * @note Não trada nenhuma flag de interrupção. A interrupção
 * por tempo é ativa toda vez que há o estouro do contador. O 
 * TIM3 é responsável por  cadenciar a leitura dos canais  do
 * ADC1, quando interrupção é chamada significa que o ADC1
 * comessou uma nova leitura de TODOS os canais habilitados de
 * acordo com as macros  ADC1_XXX_RANK.
 * 
 * O DMA1 é esposável de por copiar os dados das leituras para
 * o buffer. 
 * 
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS
 * para o controle de troca de contexto em interruições.
 */
extern void adc1_vTIM3_IRQHandler(BaseType_t *const pxHigherPriorityTaskWoken);

/**
 * @brief Função dentro da interrupção DMA1_Channel1_IRQHandler
 * @note não trada nenhuma flag de interrupção 
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS
 * para o controle de troca de contexto em interruições.
 */
extern void adc1_vDMA_IRQHandler(BaseType_t *const pxHigherPriorityTaskWoken);

/**
 * @brief Função dentro da HAL_ADC_ConvCpltCallback.
 * @note O DMA1 é esposável de por copiar os dados das leituras para
 * o buffer. Toda vez que esta função for chamada significa que o buffer
 * fou completamente populado. e está pronto para ser lido.
 * 
 * A HAL_ADC_ConvCpltCallback interrupção tambem é usara atualizar os dados
 * na swap usada na função adc1_iGetValue. ambas fazem uso de um mutex.
 * 
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS para o controle
 * de troca de contexto em interruições.
 */
extern void acd1_vBufferDone(BaseType_t *const pxHigherPriorityTaskWoken);

#endif /* adc_H_ */