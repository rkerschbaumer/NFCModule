/*
 * mb.c
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
//#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>


/* Instrumentation headers */
#include <ti/uia/runtime/LogSnapshot.h>

/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

#include"mb.h"

int pend_mb(){

}

int post_mb(){

}

int setup_mb(){
	Mailbox_Params mboxParams;
	Mailbox_Handle mailbox_hendl = NULL;
	Error_Block eb;

	Error_init(&eb);
	Mailbox_Params_init(&mboxParams);
	mailbox_hendl = Mailbox_create(sizeof(char), 2, &mboxParams, &eb);
	if (mailbox_hendl == NULL) {
		System_abort("Mailbox create failed\nAborting...");
	}
	return 0;
}
