/*
 *  ======== UART_Task.c ========
 *  Author: Michael Kramer / Matthias Wenzl
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

/* TI-RTOS Header files */
#include <driverlib/sysctl.h>
#include <ti/drivers/UART.h>

/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>

/*Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include "UART_Task.h"
#include "mb.h"

/* for loop weg, if(precomma >= 100) dafür */
void frequency_to_string(uint16_t precomma, uint8_t postcomma, char* buffer){
	uint8_t i=0,j=0;
    while(precomma!=0){
        buffer[i++] = precomma%10+'0';
        precomma=precomma/10;
    }

    for(j = 0; j < i/2; j++){
        buffer[j] ^= buffer[i-j-1];
        buffer[i-j-1] ^= buffer[j];
        buffer[j] ^= buffer[i-j-1];
    }

    buffer[i] = ',';
    buffer[++i] = postcomma+'0';
    buffer[++i] = '\n';
    buffer[++i] = '\r';
    buffer[++i] = '\0';
}

void UARTFxn(){
    UART_Handle uart;
    UART_Params uartParams;
    uint16_t outfreq=0, precomma;
    uint8_t postcomma, i;
    char outstring[9];

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    /* Echo Loopoopoopoop */
    while (1) {
        pend_mb(&outfreq, frq);
   		make_frequency_comma_again(outfreq, &precomma, &postcomma);

   		frequency_to_string(precomma, postcomma, outstring);

   		for(i = 0; outstring[i] != '\0'; i++){
   			UART_write(uart, &outstring[i], 1);
   		}

   	    Task_sleep(5);
    }
}

int setup_UART_Task(void){
    Task_Params taskUARTParams;
    Task_Handle taskUART;
    Error_Block eb;

    Error_init(&eb);
    Task_Params_init(&taskUARTParams);
    taskUARTParams.stackSize = 1024; /* stack in bytes */
    taskUARTParams.priority = 9; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskUART = Task_create((Task_FuncPtr)UARTFxn, &taskUARTParams, &eb);
    if (taskUART == NULL) {
        System_abort("TaskUART create failed");
    }

    return (0);
}
