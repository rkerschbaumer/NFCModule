/*
 *  ======== StartBIOS.c ========
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
#include <ti/sysbios/knl/Mailbox.h>
//#include <ti/sysbios/knl/Clock.h>
//#include <ti/sysbios/knl/Event.h>

/* Instrumentation headers */
#include <ti/uia/runtime/LogSnapshot.h>

/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/ssi.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>


/* Application headers */
#include"inc/hardware.h"
#include"inc/leds.h"
#include"inc/nfc_spi_task.h"

int main(void) {
	uint32_t ui32SysClock = Board_initGeneral(120*1000*1000);

	init_nfc_hw();
	Board_initSPI();

	SSIClockSourceSet(SSI3_BASE, SSI_CLOCK_SYSTEM);
	SSIConfigSetExpClk(SSI3_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, 2 * 1000 * 1000, 8);
	SSIEnable(SSI3_BASE);

	setup_nfc_spi_task(13, "NFC Task");

	BIOS_start();

	return 0;
}

/**** The code below is only for Instrumentation purposes! ****/

/* The redefinition of ti_uia_runtime_LogSnapshot_writeNameOfReference
 * is necessary due to bug UIA-23 fixed in UIA 2.00.06.52, cf.
 * http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/uia/2_00_06_52/exports/docs/uia_2_00_06_52_release_notes.html
 */
#undef ti_uia_runtime_LogSnapshot_writeNameOfReference
#define ti_uia_runtime_LogSnapshot_writeNameOfReference(refId, fmt, startAdrs, lengthInMAUs) \
( \
(ti_uia_runtime_LogSnapshot_putMemoryRange(ti_uia_events_UIASnapshot_nameOfReference, Module__MID, \
(IArg)refId,(IArg)__FILE__,(IArg)__LINE__, \
(IArg)fmt, (IArg)startAdrs, (IArg)lengthInMAUs)) \
)

/* Log the task name whenever a task is created.
 * This works around a limitation of UIA where tasks sharing a "main"
 * function do not show up separately in the execution analyzer, cf.
 * http://processors.wiki.ti.com/index.php/System_Analyzer_Tutorial_3A#Going_Further:_How_to_have_Analysis_tables_and_graphs_display_the_names_of_tasks_created_at_run_time
 */
#include <string.h>
Void tskCreateHook(Task_Handle hTask, Error_Block *eb) {
   String name = Task_Handle_name(hTask);
   LogSnapshot_writeNameOfReference(hTask, "Task_create: handle=%x", name, strlen(name)+1);
   ti_uia_runtime_LogSnapshot_writeNameOfReference(hTask, "Task_create: handle=%x", name, strlen(name)+1);
}
