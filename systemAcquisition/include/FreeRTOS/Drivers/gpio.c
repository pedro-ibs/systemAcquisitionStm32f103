/*
 * gpio.c
 *
 *  Created at:		23/02/2021 09:07:00
 *      Author:		Pedro Igor B. S.
 *	Email:		pedro.igor.ifsp@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 * TODO: Licence
 * ########################################################
 *
 * Controle digital dos pinos GPIO, utilise as macros GPIOXX
 * para trabalhar com os pinos, por exemplo GPIOC, GPIO_PIN_13
 * é equivalente ao GPIOC13
 * 
 * esse driver pode ler escrever e trocar o estados de todos
 * os pinos. Além disso é possível desativar a comunicação SWJ.
 * 
 * TODO: Está em desenvolvimente o tratamento de interrupções
 * externas.
 *
 */



/* Includes ------------------------------------------------------------------*/
#include <FreeRTOS/Drivers/gpio.h>
#include <stm32f103xb.h>
#include <stm32f1xx_ll_gpio.h>

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct{
	GPIO_TypeDef *pxGPIOx;		// GPIO PORT
	u32 uPINx;			// GPIO PIN
}GpioPin;


/* Private Functions ---------------------------------------------------------*/
GpioPin gpio_xTranslator(cu8 cuGPIOxx);	

/**
 * @brief habilitar clock das portas GPIO
 * @param none
 * 
 */
void gpio_vInitAll( void ) {

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
}

/**
 * @brief desabilita a comunicação JTAG. para poder 
 * usar livremente os pinos GPIO3, GPIOB4 e GPIOA15
 * @param none.
 */
void gpio_vDisableDebug (void) {
	__HAL_RCC_AFIO_CLK_ENABLE();	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();	// desativar JTAG
}

/**
 * @brief configura o momo de operação do pino GPIO
 * @param cuGPIOxx, pino a ser configurado
 * @param cuMode, modo de operação:
 *	GPIO_MODE_INPUT		Entrada alta impedância ( flutuasão ) 
 *	GPIO_MODE_OUTPUT_PP	Saida "Push Pull";
 *	GPIO_MODE_OUTPUT_OD	Saida "Open Drain";
 *	GPIO_MODE_AF_PP		Funçãp Alternativa "Push Pull";
 *	GPIO_MODE_AF_OD		Funçãp Alternativa "Open Drain";
 *	GPIO_MODE_AF_INPUT	Funçãp Alternativa Entrada alta impedância ( flutuasão );
 * @param cuPull, estado padrão do pino:
 *	GPIO_NOPULL  		"Pull-up" ou "Pull-down" desativados, alta impedância ( flutuasão );
 *	GPIO_PULLUP  		"Pull-up"	ativado;             
 *	GPIO_PULLDOWN		"Pull-down" 	ativado ;               
 */
void gpio_vMode (cu32 cuGPIOxx, cu32 cuMode, cu32 cuPull) {
	GpioPin xGpio = gpio_xTranslator(cuGPIOxx);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	HAL_GPIO_WritePin(xGpio.pxGPIOx, xGpio.uPINx, GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin	= xGpio.uPINx;
	GPIO_InitStruct.Mode	= cuMode;
	GPIO_InitStruct.Pull	= cuPull;
	GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(xGpio.pxGPIOx, &GPIO_InitStruct);
}

/**
 * @brief lê o estado de um pino
 * @param cuGPIOxx pino a ser lido
 * @return FALSE:	estado 0/LOW/GPIO_PIN_RESET
 * @return TRUE:	estado 1/HIGH/GPIO_PIN_SET
 * 
 */
_bool gpio_bRead(cu32 cuGPIOxx) {
	GpioPin xGpio = gpio_xTranslator(cuGPIOxx);
	if(HAL_GPIO_ReadPin(xGpio.pxGPIOx, xGpio.uPINx) == GPIO_PIN_RESET) return FALSE;
	return TRUE;
}

/**
 * @brief escreve um valor em um pino
 * @param bValue TRUE:	para estado 1/HIGH/GPIO_PIN_SET
 * @param bValue FALSE:	para estado 0/LOW/GPIO_PIN_RESET
 */
void gpio_vWrite(cu32 cuGPIOxx, _bool bValue) {
	GpioPin xGpio = gpio_xTranslator(cuGPIOxx);
	HAL_GPIO_WritePin(xGpio.pxGPIOx, xGpio.uPINx, (bValue)?(GPIO_PIN_SET):(GPIO_PIN_RESET));
}

/**
 * @brief troca o estado do pino
 * @param cuGPIOxx, pino que trocará de estado
 */
void gpio_vToggle(cu32 cuGPIOxx) {
	GpioPin xGpio = gpio_xTranslator(cuGPIOxx);
	HAL_GPIO_TogglePin(xGpio.pxGPIOx, xGpio.uPINx);
}

/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/

/**
 * @brief traduz a macro GPIOAx, GPIOBx, GPIOCx, GPIODx para porta e pino.
 * essa função é usada para facilitar o uso dos GPIOs na inicialisação, leitura
 * e escrita.
 * @param cuGPIOxx, pino a ser traduzido em porta e pino
 * @return GpioPin, tipo que armasena a poeta e o pino.
 */
GpioPin gpio_xTranslator(cu8 cuGPIOxx) {
	GpioPin		xGpio;								// TRANSLATOR HARDWARE PIN TO GPIO
	switch(cuGPIOxx){
		//=========================== A ============================//
		case 10: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_3;	break;
		case 11: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_1;	break;
		case 12: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_2;	break;
		case 13: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_3;	break;
		case 14: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_4; 	break;
		case 15: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_5; 	break;
		case 16: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_6; 	break;
		case 17: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_7; 	break;
		case 29: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_8; 	break;
		case 30: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_9; 	break;
		case 31: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_10;	break;
		case 32: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_11;	break;
		case 33: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_12;	break;
		case 34: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_13;	break;
		case 37: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_14;	break;
		case 38: xGpio.pxGPIOx = GPIOA; xGpio.uPINx = GPIO_PIN_15;	break;
		//=========================== B ============================//
		case 18: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_0;	break;
		case 19: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_1;	break;
		case 20: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_2;	break;
		case 21: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_10;	break;
		case 22: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_11;	break;
		case 25: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_12;	break;
		case 26: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_13;	break;
		case 27: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_14;	break;
		case 28: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_15;	break;
		case 39: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_3;	break;
		case 40: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_4;	break;
		case 41: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_5;	break;
		case 42: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_6;	break;
		case 43: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_7;	break;
		case 45: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_8;	break;
		case 46: xGpio.pxGPIOx = GPIOB; xGpio.uPINx = GPIO_PIN_9;	break;
		//=========================== C ============================//
		case 2: xGpio.pxGPIOx  = GPIOC; xGpio.uPINx = GPIO_PIN_13;	break;
		case 3: xGpio.pxGPIOx  = GPIOC; xGpio.uPINx = GPIO_PIN_14;	break;
		case 4: xGpio.pxGPIOx  = GPIOC; xGpio.uPINx = GPIO_PIN_15;	break;
		//=========================== D ============================//
		case 5: xGpio.pxGPIOx  = GPIOD; xGpio.uPINx = GPIO_PIN_0;	break;
		case 6: xGpio.pxGPIOx  = GPIOD; xGpio.uPINx = GPIO_PIN_1;	break;
	}
	return xGpio;
}