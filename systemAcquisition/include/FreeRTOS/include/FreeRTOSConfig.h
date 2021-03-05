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
 * 1 tab == 8 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/


/*
 * Special note to ARM Cortex-M users: ARM Cortex-M3, ARM Cortex-M4 and ARM Cortex-M4F
 * ports need FreeRTOS handlers to be installed on the SysTick, PendSV and SVCCall interrupt
 * vectors. The vector table can be populated directly with the FreeRTOS defined xPortSysTickHandler(),
 * xPortPendSVHandler() and vPortSVCHandler() functions respectively, or, if the interrupt vector
 * table is CMSIS compliant, the following three lines can be added to FreeRTOSConfig.h to map
 * the FreeRTOS function names to their CMSIS equivalents:
 *
 * see the reference it is so important for ARM Cortex-M3, ARM Cortex-M4 and ARM Cortex-M4F
 * see at ExceptionHandlers.h
 * https://www.freertos.org/FAQHelp.html
 */
#define vPortSVCHandler				SVC_Handler
#define xPortPendSVHandler			PendSV_Handler
#define xPortSysTickHandler			SysTick_Handler


#define configUSE_PREEMPTION			( 1 )
#define configUSE_IDLE_HOOK			( 0 )
#define configUSE_TICK_HOOK			( 0 )
// #define congigCPU_CLOCK_HZ_VALUE		( 48000000 )		// when use SSB driver
#define congigCPU_CLOCK_HZ_VALUE		( 72000000 )
#define configCPU_CLOCK_HZ			( ( unsigned long ) congigCPU_CLOCK_HZ_VALUE )
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES			( 5 )
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 30 )
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 9 * 1024 ) )
#define configMAX_TASK_NAME_LEN			( 16 )
#define configUSE_TRACE_FACILITY		( 0 )
#define configUSE_16_BIT_TICKS			( 0 )
#define configIDLE_SHOULD_YIELD			( 1 )

//#define configSUPPORT_STATIC_ALLOCATION         ( 1 )
//#define configSUPPORT_DYNAMIC_ALLOCATION        ( 1 )
//#define configAPPLICATION_ALLOCATED_HEAP        ( 1 )

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 			( 0 )
#define configMAX_CO_ROUTINE_PRIORITIES		( 2 )

#define configUSE_MUTEXES			( 1 )
#define configUSE_COUNTING_SEMAPHORES 		( 1 )
#define configUSE_ALTERNATIVE_API 		( 0 )
#define configCHECK_FOR_STACK_OVERFLOW		( 2 )
#define configUSE_RECURSIVE_MUTEXES		( 1 )
#define configQUEUE_REGISTRY_SIZE		( 0 )
#define configGENERATE_RUN_TIME_STATS		( 0 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		( 1 )
#define INCLUDE_uxTaskPriorityGet		( 1 )
#define INCLUDE_vTaskDelete			( 1 )
#define INCLUDE_vTaskCleanUpResources		( 0 )
#define INCLUDE_vTaskSuspend			( 1 )
#define INCLUDE_vTaskDelayUntil			( 1 )
#define INCLUDE_vTaskDelay			( 1 )


/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY 	( 255 )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( 191 ) /* equivalent to 0xb0, or priority 11. */


/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	( 15 )

/*
 * my config
 */

/* Enable the heap, it is must necessary!!
 * just can have the values 1 to 5
 * see: .src/FreRTOS/portable/MemMang/
 * 	heap_1.c		( 1 )
 * 	heap_2.c		( 2 )
 * 	heap_3.c		( 3 )
 * 	heap_4.c		( 4 )
 * 	heap_5.c		( 5 )
 */
#define MY_HEAP_CONFIG		( 4 )


#include <core/includes.h>

typedef const char 		(CCHR);

typedef uint16_t		(u16);
typedef uint8_t			(u8);
typedef uint32_t 		(u32);
typedef uint64_t 		(u64);

typedef const uint16_t		(cu16);
typedef const uint8_t		(cu8);
typedef const uint32_t 		(cu32);
typedef const uint64_t 		(cu64);


typedef enum {
	FALSE = 0,
	TRUE,
} _bool;

#define NOT( var )		( var == FALSE )?( TRUE ):( FALSE )
#define SetBit( RES, BIT ) 	( RES |=   ( 1 << BIT ) )
#define ClrBit( RES, BIT ) 	( RES &= ~ ( 1 << BIT ) )
#define TstBit( RES, BIT ) 	( RES &    ( 1 << BIT ) )
#define CplBit( RES, BIT ) 	( RES ^=   ( 1 << BIT ) )

#define mainSET_PRIORITY	( tskIDLE_PRIORITY + 1 )
#define DEC			( 10 )
#define HEX			( 16 )


#define BIT00			( 1 << 0  )
#define BIT01			( 1 << 1  )
#define BIT02			( 1 << 2  )
#define BIT03			( 1 << 3  )
#define BIT04			( 1 << 4  )
#define BIT05			( 1 << 5  )
#define BIT06			( 1 << 6  )
#define BIT07			( 1 << 7  )
#define BIT08			( 1 << 8  )
#define BIT09			( 1 << 9  )
#define BIT10			( 1 << 10 )
#define BIT11			( 1 << 11 )
#define BIT12			( 1 << 12 )
#define BIT13			( 1 << 13 )
#define BIT14			( 1 << 14 )
#define BIT15			( 1 << 15 )
#define BIT16			( 1 << 16 )
#define BIT17			( 1 << 17 )
#define BIT18			( 1 << 18 )
#define BIT19			( 1 << 19 )
#define BIT20			( 1 << 20 )
#define BIT21			( 1 << 21 )
#define BIT22			( 1 << 22 )
#define BIT23			( 1 << 23 )

/**
 *  @brief, delays fot vTaskDelay
 */
#define _10MS			( pdMS_TO_TICKS( 10	))
#define _20MS			( pdMS_TO_TICKS( 20	))
#define _40MS			( pdMS_TO_TICKS( 40	))
#define _80MS			( pdMS_TO_TICKS( 80	))
#define _100MS			( pdMS_TO_TICKS( 100	))
#define _110MS			( pdMS_TO_TICKS( 110	))
#define _120MS			( pdMS_TO_TICKS( 120	))
#define _130MS			( pdMS_TO_TICKS( 130	))
#define _140MS			( pdMS_TO_TICKS( 140	))
#define _150MS			( pdMS_TO_TICKS( 150	))
#define _160MS			( pdMS_TO_TICKS( 160	))
#define _170MS			( pdMS_TO_TICKS( 170	))
#define _180MS			( pdMS_TO_TICKS( 180	))
#define _190MS			( pdMS_TO_TICKS( 190	))
#define _200MS			( pdMS_TO_TICKS( 200	))
#define _250MS			( pdMS_TO_TICKS( 200	))
#define _173MS			( pdMS_TO_TICKS( 173	))
#define _300MS			( pdMS_TO_TICKS( 300	))
#define _350MS			( pdMS_TO_TICKS( 350	))
#define _400MS			( pdMS_TO_TICKS( 400	))
#define _450MS			( pdMS_TO_TICKS( 450	))
#define _500MS			( pdMS_TO_TICKS( 500	))
#define _800MS			( pdMS_TO_TICKS( 800	))
#define _1S			( pdMS_TO_TICKS( 1000	))
#define _2S			( pdMS_TO_TICKS( 2000	))
#define _3S			( pdMS_TO_TICKS( 3000	))
#define _4S			( pdMS_TO_TICKS( 4000	))
#define _5S			( pdMS_TO_TICKS( 5000	))
#define _6S			( pdMS_TO_TICKS( 6000	))
#define _7S			( pdMS_TO_TICKS( 7000	))
#define _8S			( pdMS_TO_TICKS( 8000	))
#define _9S			( pdMS_TO_TICKS( 9000	))
#define _10S			( pdMS_TO_TICKS( 10000	))
#define _20S			( pdMS_TO_TICKS( 20000	))

/*
 * times for RTC conter
 */
#define RTC_1S			( 1 )
#define RTC_2S			( 2 )
#define RTC_3S			( 3 )
#define RTC_4S			( 4 )
#define RTC_5S			( 5 )
#define RTC_10S			( 10 )
#define RTC_15S			( 15 )
#define RTC_20S			( 20 )
#define RTC_25S			( 25 )
#define RTC_30S			( 30 )
#define RTC_35S			( 35 )
#define RTC_40S			( 40 )
#define RTC_45S			( 45 )
#define RTC_50S			( 50 )
#define RTC_55S			( 55 )
#define RTC_1MIN		( 60 )
#define RTC_2MIN		( RTC_1MIN*2 )
#define RTC_3MIN		( RTC_1MIN*3 )
#define RTC_4MIN		( RTC_1MIN*4 )
#define RTC_5MIN		( RTC_1MIN*5 )
#define RTC_10MIN		( RTC_1MIN*10 )
#define RTC_20MIN		( RTC_1MIN*20 )
#define RTC_30MIN		( RTC_1MIN*30 )
#define RTC_1HORA		( RTC_1MIN*60 )
#define RTC_3HORA		( RTC_1HORA*3 )
#define RTC_4HORA		( RTC_1HORA*4 )
#define RTC_24HORA		( RTC_1HORA*24 )

#endif /* FREERTOS_CONFIG_H */

