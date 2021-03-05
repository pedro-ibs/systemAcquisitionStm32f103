/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Includes ------------------------------------------------------------------*/
/* Standard includes. */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* stm standard includes.  */
#include <core/includes.h>

/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/portable.h>
#include <FreeRTOS/Drivers/gpio.h>
#include <FreeRTOS/Drivers/uart.h>

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private Functions ---------------------------------------------------------*/
static void svSetup( void );
void vStartupSystem(void);
void vSystemClock_Config(void);
void vInitUsart1( void );

int main( void ) {
	#ifdef DEBUG
	debug();
	#endif

	vTaskSuspendAll();
	SystemCoreClockUpdate();
	svSetup();
	vStartupSystem();
	xTaskResumeAll();

	/* Start the scheduler. */
	vTaskStartScheduler();
	/* Will only get here if there was insufficient memory to create the idle
	 * task.  The idle task is created within vTaskStartScheduler(). */
	for( ;; );

	return 0;
}


/*==================================================================================================
  Function    : vApplicationIdleHook


  Description : The idle task can optionally call an application defined hook (or callback)
                function - the idle hook. The idle task runs at the very lowest priority, so such
                an idle hook function will only get executed when there are no tasks of higher
                priority that are able to run. This makes the idle hook function an ideal place to
                put the processor into a low power state - providing an automatic power saving
                whenever there is no processing to be performed.

                The idle hook will only get called if configUSE_IDLE_HOOK is set to 1 within
                FreeRTOSConfig.h.

                The idle hook is called repeatedly as long as the idle task is running. It is
                paramount that the idle hook function does not call any API functions that could
                cause it to block. Also, if the application makes use of the vTaskDelete() API
                function then the idle task hook must be allowed to periodically return (this is
                because the idle task is responsible for cleaning up the resources that were
                allocated by the RTOS kernel to the task that has been deleted).

  Parameters  : None

  Returns     : None
==================================================================================================*/

void vApplicationIdleHook(void){
	// TODO: Put the processor into a low power state.
	const char msg[] = "\r\nvApplicationIdleHookz";
	usart_vAtomicSendStrLn(ttyUSART1, msg, strlen(msg));

}

/*==================================================================================================
  Function    : vApplicationStackOverflowHook


  Description : Stack overflow is a very common cause of application instability. FreeRTOS therefore
                provides two optional mechanisms that can be used to assist in the detection and
                correction of just such an occurrence. The option used is configured using the
                configCHECK_FOR_STACK_OVERFLOW configuration constant. The application must provide
                a stack overflow hook function if configCHECK_FOR_STACK_OVERFLOW is not set to 0.

                The xTask and pcTaskName parameters pass to the hook function the handle and name of
                the offending task respectively. Note however, depending on the severity of the
                overflow, these parameters could themselves be corrupted, in which case the
                pxCurrentTCB variable can be inspected directly.

                Stack overflow checking introduces a context switch overhead so its use is only
                recommended during the development or testing phases.

  Parameters  : xTask       Handle of the offending task.
                pcTaskName  Name of the offending task.

  Returns     : None
==================================================================================================*/

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName){
	/* This function will get called if a task overflows its stack.   If the
	parameters are corrupt then inspect pxCurrentTCB to find which was the
	offending task. */
	vTaskSuspendAll();
	const char msg[] = "\r\nStack Overflow in task: ";
	usart_vAtomicSendStr(ttyUSART1, msg, strlen(msg));
	usart_vAtomicSendStrLn(ttyUSART1, (char*)pcTaskName, strlen((char*)pcTaskName));
	xTaskResumeAll();

	( void ) xTask;

	for( ;; );
}



/*==================================================================================================
  Function    : vApplicationTickHook


  Description : The tick interrupt can optionally call an application defined hook (or callback)
                function - the tick hook. The tick hook provides a convenient place to implement
                timer functionality.

                The tick hook will only get called if configUSE_TICK_HOOK is set to 1 within
                FreeRTOSConfig.h.

                vApplicationTickHook() executes from within an ISR so must be very short, not use
                much stack, and not call any API functions that don't end in "FromISR" or "FROM_ISR".

  Parameters  : None

  Returns     : None
==================================================================================================*/

void vApplicationTickHook(void){
	// TODO: It is a place to add timer processing.
	const char msg[] = "\r\nvApplicationTickHook";
	usart_vAtomicSendStrLn(ttyUSART1, msg, strlen(msg));
}

/*==================================================================================================
  Function    : vApplicationMallocFailedHook


  Description : The memory allocation schemes implemented by heap_1.c, heap_2.c, heap_3.c and heap_4
                can optionally include a malloc() failure hook (or callback) function that can be
                configured to get called if pvPortMalloc() ever returns NULL.

                Defining the malloc() failure hook will help identify problems caused by lack of
                heap memory - especially when a call to pvPortMalloc() fails within an API function.

                The malloc failed hook will only get called if configUSE_MALLOC_FAILED_HOOK is set
                to 1 within FreeRTOSConfig.h.

  Parameters  : None

  Returns     : None
==================================================================================================*/

void vApplicationMallocFailedHook(void){
	// It is a place to add processing for error on allocating memory.

	static char msg[15];
	const char msg2[] = "\r\nfailure to allocate dynamic memory\r\n memory in use: ";

	for(u8 idx=0; idx<10; idx++) msg[idx] = 0x00;


	usart_vAtomicSendStr(ttyUSART1, msg2, strlen(msg2));
	itoa(xPortGetFreeHeapSize() , msg, 10);
	usart_vAtomicSendStrLn(ttyUSART1, msg, strlen(msg));
}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/


static void svSetup( void ) {
	HAL_Init();
	vSystemClock_Config();

	// TODO: iniciar base dos Drivers do FreeRTOS
	// TODO: iniciar porta serial do FreeRTOS	
}


#if(congigCPU_CLOCK_HZ_VALUE == 72000000  )
	void vSystemClock_Config(void) {
		RCC_OscInitTypeDef RCC_OscInitStruct 	= {0};
		RCC_ClkInitTypeDef RCC_ClkInitStruct	= {0};
		/**
		 * Initializes the RCC Oscillators according to the specified parameters
		 * in the RCC_OscInitTypeDef structure.
		 */
		RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE;
		RCC_OscInitStruct.HSEState		= RCC_HSE_ON;
		RCC_OscInitStruct.HSEPredivValue	= RCC_HSE_PREDIV_DIV1;
		RCC_OscInitStruct.HSIState		= RCC_HSI_ON;
		RCC_OscInitStruct.PLL.PLLState		= RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource		= RCC_PLLSOURCE_HSE;
		RCC_OscInitStruct.PLL.PLLMUL		= RCC_PLL_MUL9;
		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
			Error_Handler();
		}

		/**
		 * Initializes the CPU, AHB and APB buses clocks
		 */
		RCC_ClkInitStruct.ClockType		= RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK														|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
		RCC_ClkInitStruct.SYSCLKSource		= RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.AHBCLKDivider		= RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider	= RCC_HCLK_DIV2;
		RCC_ClkInitStruct.APB2CLKDivider	= RCC_HCLK_DIV1;
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK){
			Error_Handler();
		}
	}

#else

	void vSystemClock_Config(void) {
		RCC_OscInitTypeDef RCC_OscInitStruct		= {0};
		RCC_ClkInitTypeDef RCC_ClkInitStruct		= {0};
		RCC_PeriphCLKInitTypeDef PeriphClkInit		= {0};

		/**
		 * Initializes the RCC Oscillators according to the specified parameters
		 * in the RCC_OscInitTypeDef structure.
		 */
		RCC_OscInitStruct.OscillatorType		= RCC_OSCILLATORTYPE_HSE;
		RCC_OscInitStruct.HSEState 			= RCC_HSE_ON;
		RCC_OscInitStruct.HSEPredivValue		= RCC_HSE_PREDIV_DIV1;
		RCC_OscInitStruct.HSIState			= RCC_HSI_ON;
		RCC_OscInitStruct.PLL.PLLState			= RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource			= RCC_PLLSOURCE_HSE;
		RCC_OscInitStruct.PLL.PLLMUL			= RCC_PLL_MUL6;
		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
			Error_Handler();
		}

		/** 
		 * Initializes the CPU, AHB and APB buses clocks
		 */
		RCC_ClkInitStruct.ClockType			= RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
		RCC_ClkInitStruct.SYSCLKSource			= RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.AHBCLKDivider			= RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider		= RCC_HCLK_DIV2;
		RCC_ClkInitStruct.APB2CLKDivider		= RCC_HCLK_DIV1;
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
			Error_Handler();
		}

		PeriphClkInit.PeriphClockSelection		= RCC_PERIPHCLK_USB;
		PeriphClkInit.UsbClockSelection			= RCC_USBCLKSOURCE_PLL;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
			Error_Handler();
		}
	}

#endif
