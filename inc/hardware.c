/*
 * hardware.c
 *
 *  Created on: 12. Mai 2017
 *      Author: Raphael
 */


#include "hardware.h"

void init_nfc(){
	uint32_t ui32Strength;
	uint32_t ui32PinType;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);

	//Slave Select Pin High - PN5
	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_5, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_5, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_5);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF);

	SysCtlDelay(100);

	//Turn TRF7970 on - PN4
	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_4, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_4, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_4);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, 0xFF);

	SysCtlDelay(100);

	//Turn LEDs for Tag-indication off
	GPIOPadConfigGet(GPIO_PORTP_BASE, GPIO_PIN_3, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTP_BASE, GPIO_PIN_3, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_3, 0x00);

	GPIOPadConfigGet(GPIO_PORTQ_BASE, GPIO_PIN_1, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTQ_BASE, GPIO_PIN_1, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, 0x00);

	GPIOPadConfigGet(GPIO_PORTM_BASE, GPIO_PIN_6, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_6, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_6);
	GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, 0x00);
}


