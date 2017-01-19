/*
 * test_task.c
 *
 *  Created on: Jan 18, 2017
 *      Author: raphael
 */


#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include "mb.h"

void make_frequency_great_again(uint16_t input, uint16_t* precomma, uint8_t* postcomma){
    *precomma = input/10;
    *postcomma = input%10;
}

void test_function(UArg arg0, UArg arg1){

	uint16_t outfreq=0, precomma;
	uint8_t postcomma;
	while(1){
		pend_mb(&outfreq);
		System_printf("frq: %d\n",outfreq);
		//	make_frequency_great_again(outfreq, &precomma, &postcomma);
		//System_printf("Frequency: %d,%d\n", precomma, postcomma);
		System_flush();
	}
}

int setup_test_task(){
    Task_Params test_task_params;
    Task_Handle test_task_hendl;
    Error_Block eb;

    Error_init(&eb);
    Task_Params_init(&test_task_params);
    test_task_params.stackSize = 1024;
    test_task_params.priority = 15;
    test_task_hendl = Task_create((Task_FuncPtr)test_function, &test_task_params, &eb);
    if (test_task_hendl == NULL) {
        System_abort("Test-Task create failed");
    }

    return (0);
}
