/*
 * hardware.h
 *
 *  Created on: 12. Mai 2017
 *      Author: Raphael
 */

#ifndef INC_HARDWARE_H_
#define INC_HARDWARE_H_

#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>/*supplies GPIO_PIN_x*/
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <inc/hw_memmap.h>/*supplies GPIO_PORTx_BASE*/

#define BIT0 0b00000001
#define BIT1 0b00000010
#define BIT2 0b00000100
#define BIT3 0b00001000
#define BIT4 0b00010000
#define BIT5 0b00100000
#define BIT6 0b01000000
#define BIT7 0b10000000

void init_nfc_hw(void);

#endif /* INC_HARDWARE_H_ */
