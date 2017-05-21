/*
 * nfc_spi_task.c
 *
 *  Created on: 15. Mai 2017
 *      Author: Raphael
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
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>

#include "nfc_spi_task.h"
#include "nfcspi.h"
#include "trf79xxa_RK.h"
#include"nfc_app.h"
#include"iso14443a.h"

void nfc_function(UArg arg0){
	uint8_t regval=5;
	TRF79xxA_initialSettings();
	// trying to set TRF7970 initial settings by myself. F U TI

	//TRF79xxA_writeRegister(TRF79XXA_CHIP_STATUS_CONTROL, 0x20);

	System_printf("regval: %d\n", regval);
	System_flush();

	TRF79xxA_setTrfPowerSetting(TRF79xxA_3V_FULL_POWER);

//	NFC_init();


	regval = TRF79xxA_readRegister(0x04);

	System_printf("Register TRF79XXA_MODULATOR_CONTROL: %d\n", regval);
    System_flush();
	while(1){
		//FIXME: wait for NFC Tag -> do stuff with NFC information
		Task_sleep(500);
		//NFC_findTag();

	}
}

void setup_nfc_spi_task(int prio, xdc_String name){
    Task_Params task_params_nfc;
    Task_Handle task_hendl_nfc;
    Error_Block eb;
    Error_init(&eb);
    Task_Params_init(&task_params_nfc);
    task_params_nfc.instance->name = name;
    task_params_nfc.stackSize = 1024;
    task_params_nfc.priority = prio;
    task_hendl_nfc = Task_create((Task_FuncPtr)nfc_function, &task_params_nfc, &eb);
        if (task_hendl_nfc == NULL) {
            System_abort("Create NFC task failed");
        }
}

