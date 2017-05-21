/*
 * hardware.c
 *
 *  Created on: 12. Mai 2017
 *      Author: Raphael
 */


#include "hardware.h"

void init_nfc_hw(){
	uint32_t ui32Strength;
	uint32_t ui32PinType;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);

//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //SPI SlaveSel PB2, TRF Enable PB3
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); //ISO15693 PN2, ISO14443A PN3
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP); //ISO14443B PP2
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //MISO PD0, MOSI PD1, SPI Clock PD3
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); //IRQ PC7

	//Slave Select Pin High - PN5 BP2 - PB2 BP1
	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_5, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_5, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_5);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF);

//	GPIOPadConfigGet(GPIO_PORTB_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
//	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, ui32Strength, GPIO_PIN_TYPE_STD);
//	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2);
//	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF);

	SysCtlDelay(160000);

	//Turn TRF7970 on - PN4 BP2 - PB3 BP1
	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_4, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_4, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_4);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, 0xFF);

//	GPIOPadConfigGet(GPIO_PORTB_BASE, GPIO_PIN_3, &ui32Strength, &ui32PinType);
//	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_3, ui32Strength, GPIO_PIN_TYPE_STD);
//	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
//	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xFF);

	SysCtlDelay(200000);

	//Turn LEDs for Tag-indication off - PP3, PQ1, PM6 BP2 - PN2, PN3, PP2 BP1
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

//	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
//	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_2, ui32Strength, GPIO_PIN_TYPE_STD);
//	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_2);
//	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, 0x00);
//
//	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_3, &ui32Strength, &ui32PinType);
//	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_3, ui32Strength, GPIO_PIN_TYPE_STD);
//	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_3);
//	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, 0x00);
//
//	GPIOPadConfigGet(GPIO_PORTP_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
//	GPIOPadConfigSet(GPIO_PORTP_BASE, GPIO_PIN_2, ui32Strength, GPIO_PIN_TYPE_STD);
//	GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, GPIO_PIN_2);
//	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_2, 0x00);
}


