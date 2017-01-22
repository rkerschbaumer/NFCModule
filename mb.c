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

Mailbox_Params mb_params_frq, mb_params_vol, mb_params_mode;
Mailbox_Handle mailbox_frq_hendl = NULL, mailbox_vol_hendl = NULL, mailbox_mode_hendl = NULL;

void pend_mb(uint16_t* output, enum mb_type x){
	if(x == frq) Mailbox_pend(mailbox_frq_hendl, output, BIOS_WAIT_FOREVER);
	if(x == vol) Mailbox_pend(mailbox_vol_hendl, output, BIOS_WAIT_FOREVER);
	if(x == mode){
		Mailbox_pend(mailbox_mode_hendl, output, BIOS_WAIT_FOREVER);
	}
}

//void post_mb(uint16_t* value, enum mb_type x){
//	if(x == frq) Mailbox_post(mailbox_frq_hendl, value, BIOS_WAIT_FOREVER);
//	if(x == vol) Mailbox_post(mailbox_vol_hendl, value, BIOS_WAIT_FOREVER);
//	if(x == mode) Mailbox_post(mailbox_mode_hendl, value, BIOS_WAIT_FOREVER);
//}
void post_mb(uint16_t value, enum mb_type x){
	if(x == frq) Mailbox_post(mailbox_frq_hendl, &value, BIOS_WAIT_FOREVER);
	if(x == vol) Mailbox_post(mailbox_vol_hendl, &value, BIOS_WAIT_FOREVER);
	if(x == mode) Mailbox_post(mailbox_mode_hendl, &value, BIOS_WAIT_FOREVER);
}

int setup_mb(){
	Error_Block eb;

	Error_init(&eb);
	Mailbox_Params_init(&mb_params_frq);
	Mailbox_Params_init(&mb_params_vol);
	Mailbox_Params_init(&mb_params_mode);
	mailbox_frq_hendl = Mailbox_create(sizeof(uint16_t), 1, &mb_params_frq, &eb);
	mailbox_vol_hendl = Mailbox_create(sizeof(uint8_t), 1, &mb_params_vol, &eb);
	mailbox_mode_hendl = Mailbox_create(sizeof(uint8_t), 1, &mb_params_mode, &eb);
	if (mailbox_frq_hendl == NULL) {
		System_abort("Mailbox frq create failed\n");
	}
	if (mailbox_vol_hendl == NULL) {
		System_abort("Mailbox volume create failed\n");
	}
	if (mailbox_mode_hendl == NULL) {
		System_abort("Mailbox mode create failed\n");
	}
	return 0;
}

void make_frequency_comma_again(uint16_t input, uint16_t* precomma, uint8_t* postcomma){
    *precomma = input/10;
    *postcomma = input%10;
}
