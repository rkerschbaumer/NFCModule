/*
 * nfcspi.c
 *
 *  Created on: 13. Mai 2017
 *      Author: Raphael
 */

#include "nfcspi.h"


//===============================================================
// NAME: void SPI_directCommand (uint8_t ui8command)
//
// BRIEF: Is used in SPI mode to transmit a Direct Command to
// reader chip.
//
// INPUTS:
//	Parameters:
//		uint8_t		ui8command		Direct Command
//
// OUTPUTS:
//
// PROCESS:	[1] transmit Direct Command
//
// CHANGE:
// DATE  		WHO	DETAIL
// 24Nov2010	RP	Original Code
// 07Dec2010	RP	integrated wait while busy loops
// 14May2017	RK	mapped for Tiva TM4C1294 Board
//===============================================================

void SPI_directCommand(uint8_t ui8Command) {
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0x00); // Start SPI Mode - BP2
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x00); // Start SPI Mode - BP1

	// set Address/Command Word Bit Distribution to command
	ui8Command = (0x80 | ui8Command);					// command
	ui8Command = (0x9f & ui8Command);					// command code

	SPI_sendByte(ui8Command);

	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF); //Stop SPI Mode - BP2
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF); //Stop SPI Mode - BP1
}


//===============================================================
// NAME: void SPI_sendByte(uint8_t ui8TxByte)
//===============================================================

void SPI_sendByte(uint8_t ui8TxByte) {
	SSIDataPut(SSI3_BASE, ui8TxByte);
	while(SSIBusy(SSI3_BASE));

	/*
	UCB0TXBUF = ui8TxByte;
	while (UCB0STAT & UCBUSY);
	*/
}

//===============================================================
// NAME: void SPI_writeSingle (uint8_t *pui8Buffer, uint8_t length)
//
// BRIEF: Is used in SPI mode to write to a specified reader chip
// registers.
//
// INPUTS:
//	uint8_t	*pui8Buffer	addresses of the registers followed by the
//					contends to write
//	uint8_t	length	number of registers * 2
//
// OUTPUTS:
//
// PROCESS:	[1] write to the registers
//
// CHANGE:
// DATE  		WHO	DETAIL
// 24Nov2010	RP	Original Code
// 07Dec2010	RP	integrated wait while busy loops
//===============================================================

void SPI_writeSingle(uint8_t * pui8Buffer) {
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0x00); // Start SPI Mode - BP2
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x00); //Start SPI Mode - BP1

	// Address/Command Word Bit Distribution
	// address, write, single (fist 3 bits = 0)
	pui8Buffer[0] = (0x1f & pui8Buffer[0]);				// register address


	//SPI_sendByte(*pui8Buffer++);
	//SPI_sendByte(*pui8Buffer++);
	SPI_sendByte(pui8Buffer[0]);
	SPI_sendByte(pui8Buffer[1]);

	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF); //Stop SPI Mode - BP2
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF); //Stop SPI Mode - BP1

}

//===============================================================
// Settings for SPI Mode                                        ;
// 02DEC2010	RP	Original Code
//===============================================================

void SPI_setup(void) {
	//GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, 0xFF);  //TRF Enable set - BP2
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xFF);  //TRF Enable set - BP1

	//FIXME - should probably be done... don't know what is needed here
	/*
	IRQ_PIN_SET;
	IRQ_EDGE_SET;								// rising edge interrupt
	 */

	//SPI_usciSet();								// Set the USART

	//LED_ALL_OFF Allready done in hardware.c
	//FIXME - LED_PORT_SET -> what is that  doing?
	/*
	LED_ALL_OFF;
	LED_PORT_SET;
	*/
}

//===============================================================
// NAME: void SPI_usciSet (void)
//
// BRIEF: Is used to set USCI B0 for SPI communication
//
// INPUTS:
//
// OUTPUTS:
//
// PROCESS:	[1] make settings
//
// CHANGE:
// DATE  		WHO	DETAIL
// 24Nov2010	RP	Original Code
// 07Dec2010	RP	reduced SPI clock frequency
//===============================================================
//FIXME bigtime - wos geht do mit den UCsdaflokjwetroij dingern ab?
/*void SPI_usciSet(void) {
	//Uses USCI_B0
	UCB0CTL1 |= UCSWRST;						// Enable SW reset
	UCB0CTL0 |= UCMSB + UCMST + UCSYNC;			// 3-pin, 8-bit SPI master

	#if (TRF79xxA_VERSION == 60)
	UCB0CTL0 |= UCCKPH;
#endif

	UCB0CTL1 |= UCSSEL_2;						// Source from SMCLK

	UCB0BR0 = 0x04;
	UCB0BR1 = 0;
	P1SEL |= BIT5 + BIT6 + BIT7;				// P1.5,1.6,1.7 UCBOCLK,UCB0SIMO,UCB0SOMI, option select
	P1SEL2 |= BIT5 + BIT6 + BIT7;				// P1.5,1.6,1.7 UCBOCLK,UCB0SIMO,UCB0SOMI, option select

	SLAVE_SELECT_PORT_SET;						// Set the Slave Select Port
	SLAVE_SELECT_HIGH;							// Slave Select => inactive (high)

	UCB0CTL1 &= ~UCSWRST;						// **Initialize USCI state machine**
}*/

//===============================================================
// NAME: void SPI_readSingle (uint8_t *pui8Buffer)
//
// BRIEF: Is used in SPI mode to read specified reader chip
// registers.
//
// INPUTS:
//	Parameters:
//		uint8_t		*pui8Buffer		addresses of the registers
//
// OUTPUTS:
//
// PROCESS:	[1] read registers
//			[2] write contents to *pui8Buffer
//
//===============================================================

void SPI_readSingle(uint8_t * pui8Buffer) {
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0x00); // Start SPI Mode - BP2
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x00); //Start SPI Mode - BP1

	// Address/Command Word Bit Distribution
	*pui8Buffer = (0x40 | *pui8Buffer); 			// address, read, single
	*pui8Buffer = (0x5f & *pui8Buffer);				// register address
	//SPI_sendByte(*pui8Buffer);					// Previous data to TX, RX
	//*(pui8Buffer+1) = SPI_receiveByte();
	SPI_sendByte(pui8Buffer[0]);
	pui8Buffer[1]=SPI_receiveByte();

	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF); //Stop SPI Mode - BP2
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF); //Stop SPI Mode - BP1
}


//===============================================================
// NAME: uint8_t SPI_receiveByte(void)
//===============================================================

uint32_t SPI_receiveByte(void) {
	/* is no longer a single byte... but without uint32 - SSIDataGet isn't working*/
	uint32_t retval;

	SSIDataGet(SSI3_BASE, &retval);
	while(SSIBusy(SSI3_BASE));

	System_printf("retval SPI_recByte()=%d\n",retval);
	System_flush();

	return retval;

	/*UCB0TXBUF = 0x00;

	while (UCB0STAT & UCBUSY);

	return UCB0RXBUF;
	*/
}

//===============================================================
// NAME: void SPI_rawWrite (uint8_t *pui8Buffer, uint8_t length)
//
// BRIEF: Is used in SPI mode to write direct to the reader chip.
//
// INPUTS:
//	Parameters:
//		uint8_t		*pui8Buffer		raw data
//		uint8_t		length		number of data bytes
//
// OUTPUTS:
//
// PROCESS:	[1] send raw data to reader chip
//
//===============================================================

void SPI_rawWrite(uint8_t * pui8Buffer, uint8_t ui8Length, bool bContinuedSend) {
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0x00); // Start SPI Mode - BP2
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x00); //Start SPI Mode - BP1

	if (bContinuedSend){
		SPI_sendByte(0x3F);
	}

	while(ui8Length-- > 0){
		// Check if USCI_B0 TX buffer is ready
/*		FIXME bigtime - wos geht do mit den UCsdaflokjwetroij dingern ab?
 * 		while (!(IFG2 & UCB0TXIFG));

		// Transmit data
		UCB0TXBUF = *pui8Buffer;

		while(UCB0STAT & UCBUSY);	// Wait while SPI state machine is busy
*/
		pui8Buffer++;
	}

	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF); //Stop SPI Mode - BP2
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF); //Stop SPI Mode - BP1
}
