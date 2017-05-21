/*
 * File Name: trf797x.c
 *
 * Description: TRF797x Driver Functions
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>

#include "trf79xxa_RK.h"
#include "nfcspi.h"

//===============================================================

uint8_t g_pui8TrfBuffer[NFC_FIFO_SIZE];

static uint8_t	g_ui8CollisionPosition;

static uint8_t	g_ui8FifoOffset;
static uint8_t	g_ui8FifoRxLength;

static tTrfStatus g_sTrfStatus;
static tTrfSettings g_eTrfGeneralSettings;

static volatile uint8_t g_ui8IrqFlag;
static volatile uint8_t g_ui8TimeoutFlag;

//*****************************************************************************
//
//! \addtogroup trf797x_api TRF7970A Driver API's
//! @{
//!
//! This section describes all the functions contained within the TRF79xxA
//! driver.
//
//*****************************************************************************

//===============================================================
//
//! TRF79xxA_sendDirectCommand - Send a Direct Command to TRF79xxA.
//!
//! \param ui8Value is the direct command to be issued
//!
//! This function is used to transmit a Direct Command to TRF79xxA.
//!
//! \return None.
//
//===============================================================

void TRF79xxA_sendDirectCommand(uint8_t ui8Value) {
	SPI_directCommand(ui8Value);
}

//===============================================================
//
//! TRF79xxA_resetFIFO - Resets TRF79xxA FIFO
//!
//! This function used to reset the TRF79xxA FIFO.
//!
//! \return None.
//
//===============================================================

void
TRF79xxA_resetFIFO(void) {
	uint8_t	ui8Command;

	ui8Command = TRF79XXA_RESET_FIFO_CMD;
	TRF79xxA_sendDirectCommand(ui8Command);
}

//===============================================================
//
//! TRF79xxA_initialSettings - Initialize TRF79xxA
//!
//! This function configures the TRF79xxA upon power up.
//! Steps include:
//!  - Setup SPI communication
//!  - Send Soft Init and Idle Direct Commands
//!  - Reset the FIFO
//!  - Configure TRF79xxA modulator and regulator registers
//!  - TRF7970A only: Write to regiser 0x18 per errata
//!
//! \return None.
//
//===============================================================

void TRF79xxA_initialSettings(void) {
	g_ui8CollisionPosition = 0;
	g_ui8FifoOffset = 0;
	g_ui8FifoRxLength = 0;

	g_sTrfStatus = TRF_IDLE;
	g_eTrfGeneralSettings.eTrfPowerSetting = TRF79xxA_3V_FULL_POWER;
	g_eTrfGeneralSettings.bRfFieldOn = false;
	g_eTrfGeneralSettings.ui8IsoControl = 0x02;	// TRF79xxA Default

	g_ui8IrqFlag = 0x00;
	g_ui8TimeoutFlag = 0x00;

	// Setup TRF79xxA SPI Module
	//SPI_setup();

	// Delay to allow SPI to finish starting up
	Task_sleep(100);

	// Send out SOFT_INIT + IDLE initial sequence to soft reset TRF7970A
	TRF79xxA_sendDirectCommand(TRF79XXA_SOFT_INIT_CMD);
	Task_sleep(100);
	TRF79xxA_sendDirectCommand(TRF79XXA_IDLE_CMD);

	// Delay to ensure soft reset has processed
	Task_sleep(100);


	g_eTrfGeneralSettings.ui8IsoControl = 0x21;


	TRF79xxA_resetFIFO();			// Reset the FIFO
	TRF79xxA_writeRegister(TRF79XXA_MODULATOR_CONTROL, 0x01); 		// ASK 100%, no SYS_CLK output
	TRF79xxA_writeRegister(TRF79XXA_REGULATOR_CONTROL, 0x01);
	TRF79xxA_writeRegister(TRF79XXA_NFC_TARGET_LEVEL, 0x00); 		// For TRF7970A Errata

}



//===============================================================
//
//! TRF79xxA_writeRegister - Write single to a TRF79xxA Register
//!
//! \param ui8TrfRegister is the register address for the write
//! \param ui8Value is the value to write to the specified
//! register
//!
//! This function writes a new value into a single TRF79xxA
//! register.
//!
//! \return None.
//
//===============================================================

void TRF79xxA_writeRegister(uint8_t ui8TrfRegister, uint8_t ui8Value) {
	uint8_t pui8Write[2];

	if (ui8TrfRegister == TRF79XXA_ISO_CONTROL)	{
		// Attempt to enable Card Emulation/Peer to Peer which is not supported by firmware
		// Exit function to avoid issues with that
		if ((ui8Value & BIT5) == BIT5)		{
			return;
		}

		g_eTrfGeneralSettings.ui8IsoControl = ui8Value;	// Update the ISO Control Register variable
	}

	if (ui8TrfRegister == TRF79XXA_CHIP_STATUS_CONTROL)	{
		if ((ui8Value & BIT5) == BIT5) {
			// Check for RF field bit and update variable
			g_eTrfGeneralSettings.bRfFieldOn = true;
		}
		else		{
			g_eTrfGeneralSettings.bRfFieldOn = false;
		}
	}

	pui8Write[0] = ui8TrfRegister;
	pui8Write[1] = ui8Value;
	SPI_writeSingle(pui8Write);
}

//===============================================================
//
//! TRF79xxA_setTrfPowerSetting - Set the TRF79xxA power setting
//!
//! \param sTrfPowerSetting is the new TRF79xxA Power Setting
//!
//! This function allows for configuration of the TRF79xxA power
//! setting.
//!
//! Options are:
//!  - TRF79xxA_3V_FULL_POWER: 3V TRF79xxA input w/ full power RF output
//!  - TRF79xxA_5V_FULL_POWER: 5V TRF79xxA input w/ full power RF output
//!  - TRF79xxA_3V_HALF_POWER: 3V TRF79xxA input w/ half power RF output
//!  - TRF79xxA_5V_HALF_POWER: 5V TRF79xxA input w/ half power RF output
//!
//! \return None.
//
//===============================================================

void TRF79xxA_setTrfPowerSetting(tTrfPowerOptions sTrfPowerSetting)
{
	g_eTrfGeneralSettings.eTrfPowerSetting = sTrfPowerSetting;
}
//===============================================================
//
//! TRF79xxA_reset - Resets TRF79xxA
//!
//! This function will reset the TRF79xxA through the Software
//! Init direct command followed by reinitializing basic settings
//! and clearing affected global variables.
//!
//! \return None.
//
//===============================================================

void TRF79xxA_reset(void) {
	TRF79xxA_sendDirectCommand(TRF79XXA_SOFT_INIT_CMD);
	TRF79xxA_sendDirectCommand(TRF79XXA_IDLE_CMD);

	Task_sleep(10);

	TRF79xxA_resetFIFO();			// Reset the FIFO

	TRF79xxA_writeRegister(TRF79XXA_MODULATOR_CONTROL, 0x01); 		// ASK 100%, no SYS_CLK output

	TRF79xxA_writeRegister(TRF79XXA_REGULATOR_CONTROL, 0x01);

	TRF79xxA_writeRegister(TRF79XXA_NFC_TARGET_LEVEL, 0x00); 		// For TRF7970A Errata

	g_eTrfGeneralSettings.ui8IsoControl = 0x21;

	g_eTrfGeneralSettings.bRfFieldOn = false;
	g_ui8FifoOffset = 0;
	g_ui8FifoRxLength = 0;
}

//===============================================================
//
//! TRF79xxA_checkExternalRfField - Checks if an external RF field
//! is present.
//!
//! This function performs RF collision avoidance as outlined in
//! TI application notes (sloa192, sloa227) in order to determine
//! if an external RF field is present.
//!
//! \return bExtFieldOn retuns the external RF field status
//
//===============================================================

bool TRF79xxA_checkExternalRfField(void){
	bool bExtFieldOn;
	uint8_t ui8Value;

	TRF79xxA_writeRegister(TRF79XXA_CHIP_STATUS_CONTROL, 0x02);
	TRF79xxA_sendDirectCommand(TRF79XXA_TEST_EXTERNAL_RF_CMD);
	//__delay_cycles(400); 		//	Delay for 50uS
	Task_sleep(1);
	ui8Value = TRF79xxA_readRegister(TRF79XXA_RSSI_LEVELS);

	if ((ui8Value & 0x07) > 1)	// Check for RF field bit and update variable
	{
		bExtFieldOn = true;
	}
	else
	{
		bExtFieldOn = false;
	}

	return bExtFieldOn;
}


//===============================================================
//
//! TRF79xxA_setupInitiator - Write the initial settings for
//! a set of TRF79xxA registers based on which protocol is to be
//! enabled.
//!
//! \param ui8IsoControl is the value to write to the ISO Control
//! register of the TRF79xxA
//!
//! This function is used to configure a series of TRF79xxA
//! registers based on which RFID technology will be enabled in
//! the ISO control register.
//!
//! This function will only enables one RFID technology at a time.
//!
//! \return None.
//
//===============================================================

void TRF79xxA_setupInitiator(uint8_t ui8IsoControl){
	TRF79xxA_reset();			// Reset the TRF7970A to ensure a clean state of operation before changing modes

	TRF79xxA_turnRfOn();		// Turn on the RF field

	g_eTrfGeneralSettings.ui8IsoControl = ui8IsoControl;	// Update the ISO Control Register variable

	// Register 0x01 - ISO Control Register
	TRF79xxA_writeRegister(TRF79XXA_ISO_CONTROL,ui8IsoControl);

	switch (ui8IsoControl) {
	case 0x08: 				// ISO14443A + RX CRC
	case 0x88: 				// ISO14443A + no RX CRC
		// Register 0x09 - System Clock Output, Modulation Scheme
		TRF79xxA_writeRegister(TRF79XXA_MODULATOR_CONTROL, 0x01); 		// Sys Clock Output = 13.56MHz, OOK = 100%
		break;
	/*case 0x0C: 				// ISO14443B
		// Register 0x09 - System Clock Output, Modulation Scheme
		TRF79xxA_writeRegister(TRF79XXA_MODULATOR_CONTROL, 0x00); 		// Sys Clock Output = 13.56MHz, ASK 10%
		break;
	case 0x02:				// ISO15693
		// Register 0x09 - System Clock Output, Modulation Scheme
		TRF79xxA_writeRegister(TRF79XXA_MODULATOR_CONTROL, 0x01); 		// Sys Clock Output = 13.56MHz, OOK = 100%

		// Resgister 0x07 - No Response Wait Time
		TRF79xxA_writeRegister(TRF79XXA_RX_NO_RESPONSE_WAIT_TIME, 0x15);
		break;
	case 0x1A: 				// FeliCa 212kbps
	case 0x1B: 				// FeliCa 424kbps
		// Register 0x09 - System Clock Output, Modulation Scheme
		TRF79xxA_writeRegister(TRF79XXA_MODULATOR_CONTROL, 0x00); 		// Sys Clock Output = 13.56MHz, ASK 10%
		break;*/
	default:
		break;
	}
	// Register 0x14 - Adjustable FIFO Level
	TRF79xxA_writeRegister(TRF79XXA_FIFO_IRQ_LEVELS, 0x0C);

	// Register 0x18 - NFC Target Detection Level
	// This register must be written to 0x00 per TRF7970A Errata
	TRF79xxA_writeRegister(TRF79XXA_NFC_TARGET_LEVEL, 0x00);
}
//===============================================================
//
//! TRF79xxA_turnRfOff - Turn off the transmission of the TRF79xxA
//! RF Field
//!
//! This function stops the TRF79xxA transmitting an RF field
//!
//! \return None.
//
//===============================================================

void TRF79xxA_turnRfOff(void) {
	uint8_t	ui8Value;

	ui8Value = g_eTrfGeneralSettings.eTrfPowerSetting;

	TRF79xxA_writeRegister(TRF79XXA_CHIP_STATUS_CONTROL,ui8Value);

	g_eTrfGeneralSettings.bRfFieldOn = false;	// Update RF Field variable
}

//===============================================================
//
//! TRF79xxA_readRegister - Read out a single TRF79xxA register
//!
//! This function reads a specific TRF79xxA register.
//!
//! \return pui8Value returns the value of the TRF79xxA Register
//
//===============================================================

uint8_t TRF79xxA_readRegister(uint8_t ui8TrfRegister) {
	//uint8_t pui8Value[1]; -> BULLSHIT, TI
	uint8_t pui8Value[2];
	pui8Value[0] = ui8TrfRegister;
	SPI_readSingle(pui8Value);
	System_printf("pui8Value[0]: %d\n&pui8Value[0]: %d\npui8Value[1]: %d\n&pui8Value[1]: %d\n",pui8Value[0],&pui8Value[0],pui8Value[1],&pui8Value[1]);
	System_flush();
	return pui8Value[1];
}

//===============================================================
//
//! TRF79xxA_turnRfOn - Turns on the transmission of the TRF79xxA
//! RF Field
//!
//! This function enables the TRF79xxA transmit an RF field
//!
//! \return None.
//
//===============================================================

void TRF79xxA_turnRfOn(void) {
	uint8_t	ui8Value;
	ui8Value = g_eTrfGeneralSettings.eTrfPowerSetting | 0x20;	// Turn RF field On
	TRF79xxA_writeRegister(TRF79XXA_CHIP_STATUS_CONTROL,ui8Value);
	g_eTrfGeneralSettings.bRfFieldOn = true;	// Update RF Field variable
}

//===============================================================
//
//! TRF79xxA_getCollisionPosition - Return the current Collision
//! Position value
//!
//! This function will return the current Collision Position
//! value.
//! This is only used for the ISO14443 Type A anti-collision
//! process.
//!
//! \return g_ui8CollisionPosition returns the current Collision
//! Position value
//
//===============================================================

uint8_t TRF79xxA_getCollisionPosition(void){
	return g_ui8CollisionPosition;
}

//===============================================================
//
//! TRF79xxA_getIsoControlValue - Fetch the latest Iso Control
//! Register value
//!
//! This function returns the current TRF79xxA ISO Control
//! Register setting .
//!
//! The ISO Control Register value is updated whenever a read or
//! write to the ISO Control Register occurs.
//!
//! \return g_ui8IsoControlValue returns the current ISO Control
//! Register value
//
//===============================================================

uint8_t TRF79xxA_getIsoControlValue(void){
	return g_eTrfGeneralSettings.ui8IsoControl;
}

//===============================================================
//
//! TRF79xxA_getRxBytesReceived - Returns the Number of RX Bytes
//! received by the TRF79xxA FIFO
//!
//! This function returns the number of bytes received by the
//! TRF79xxA during the last packet reception.
//!
//! This is used to check for how much data has been received
//! and to ensure packets with expected lengths were fully
//! received.
//!
//! \return g_ui8FifoRxLength returns the current FIFO RX Length
//
//===============================================================

uint8_t TRF79xxA_getRxBytesReceived(void){
	return g_ui8FifoRxLength;
}

//===============================================================
//
//! TRF79xxA_setTrfStatus - Set the TRF79xxA driver status
//!
//! \param sTrfStatus is the new TRF79xxA driver status
//!
//! This function sets the TRF79xxA driver status manually.
//! This can be used to modify the TRF driver status without an
//! IRQ event. Use with caution.
//!
//! \return None.
//
//===============================================================

void TRF79xxA_setTrfStatus(tTrfStatus sTrfStatus){
	g_sTrfStatus = sTrfStatus;
}

//===============================================================
//
//! TRF79xxA_waitRxData -
//!
//! \return g_sTrfStatus returns the current TRF79xxA drive
//! status
//
//===============================================================

tTrfStatus TRF79xxA_waitRxData(uint8_t ui8TxTimeout, uint8_t ui8RxTimeout){
	switch (g_sTrfStatus)
	{
	case TRF_IDLE:
	case TX_WAIT:
		TRF79xxA_waitTxIRQ(ui8TxTimeout);		// TX timeout
		TRF79xxA_waitRxIRQ(ui8RxTimeout);		// RX timeout
		break;
	case TX_COMPLETE:
		TRF79xxA_waitRxIRQ(ui8RxTimeout);		// RX timeout
		break;
	case NO_RESPONSE_RECEIVED_15693:
		// Do Nothing
		break;
	case COLLISION_ERROR:
		if ((g_eTrfGeneralSettings.ui8IsoControl == 0x02) || (g_eTrfGeneralSettings.ui8IsoControl == 0x08) || (g_eTrfGeneralSettings.ui8IsoControl == 0x88))
		{
			// Do Nothing
		}
		else
		{
			return g_sTrfStatus;
		}
		break;
	case RX_COMPLETE:
	case RX_WAIT:
	case RX_WAIT_EXTENSION:
		// Do Nothing
		break;
	default:
		g_sTrfStatus = TX_ERROR;
		break;
	}

	return g_sTrfStatus;
}

//===============================================================
//
//! TRF79xxA_writeRaw - Write data to TRF79xxA
//!
//! \param pui8Payload is the buffer with data packet contents
//! \param ui8Length is the size of the data packet
//!
//! This function writes provided data directly to the TRF79xxA.
//!
//! \return None.
//
//===============================================================

void TRF79xxA_writeRaw(uint8_t * pui8Payload, uint8_t ui8Length) {
	uint8_t ui8TxBytesRemaining;
	uint8_t ui8TxIndex;
	uint8_t ui8FifoTxLength;
	uint8_t ui8TxBytesAvailable;
	bool bContinuedSend = false;

	g_sTrfStatus = TRF_IDLE;

	// First send includes 5 bytes for command overhead
	if (TRF79xxA_MAX_FIFO_SIZE+5 > ui8Length){
		SPI_rawWrite(pui8Payload, ui8Length, bContinuedSend);
	}
	else{
		ui8TxBytesRemaining = ui8Length;
		ui8TxIndex = 0;
		ui8TxBytesAvailable = TRF79xxA_MAX_FIFO_SIZE+5; // First send includes 5 bytes for command overhead
														// (Reset FIFO, Transmit with or without CRC, Continuous Write, Length High and Length Low)
		bContinuedSend = false;							// First send is not continued

		while(ui8TxBytesRemaining > 0){
			if (ui8TxBytesRemaining > TRF79xxA_MAX_FIFO_SIZE){

				SPI_rawWrite(&pui8Payload[ui8TxIndex], ui8TxBytesAvailable, bContinuedSend);
				ui8TxBytesRemaining = ui8TxBytesRemaining - ui8TxBytesAvailable;
				ui8TxIndex = ui8TxIndex + ui8TxBytesAvailable;
				bContinuedSend = true;
			}
			else{
				// Last send
				SPI_rawWrite(&pui8Payload[ui8TxIndex], ui8TxBytesRemaining, bContinuedSend);
				bContinuedSend = false;
				ui8TxBytesRemaining = 0;
			}

			g_ui8TimeoutFlag = 0x00;
			// Clear the IRQ Flag
			g_ui8IrqFlag = 0x00;
			// FIXME - Timer functions needed Setup for the Timer
			//MCU_setCounter(5);
			while((g_ui8IrqFlag == 0x00) && (g_ui8TimeoutFlag == 0x00)); // Wait for an interrupt & Do Nothing

			// RESET_COUNTER; //FIXME wos isn des?

			if (g_sTrfStatus == TX_WAIT) {
				ui8FifoTxLength = TRF79xxA_readRegister(TRF79XXA_FIFO_STATUS);
				ui8FifoTxLength &= 0x7F;
				ui8TxBytesAvailable = TRF79xxA_MAX_FIFO_SIZE-ui8FifoTxLength;
			}
			else if (g_sTrfStatus == TX_COMPLETE){
				if (ui8TxBytesRemaining == 0){
					// Packet is sent
					break;
				}
				else{
					ui8FifoTxLength = TRF79xxA_readRegister(TRF79XXA_FIFO_STATUS);
					ui8FifoTxLength &= 0x7F;
					ui8TxBytesAvailable = TRF79xxA_MAX_FIFO_SIZE-ui8FifoTxLength;

					bContinuedSend = true;
				}
			}
			else{
				// Error occurred, break
				g_sTrfStatus = TX_ERROR;
				break;
			}
		}
	}
}


//===============================================================
//
//! TRF79xxA_waitRxIRQ - Timeout sequence for just RX
//!
//! \param ui8RxTimeout is the RX timeout in milliseconds
//!
//! This function determines if any data has been received prior
//! to the RX timeout only.
//!
//! When the RX timeout occurs before data is received, then mark
//! the TRF79xxA status as a No Response Received status.
//!
//! \return None.
//
//===============================================================

void TRF79xxA_waitRxIRQ(uint8_t ui8RxTimeout){
	g_ui8FifoOffset = 0; // Reset the FIFO Offset prior to receiving data

	g_sTrfStatus = RX_WAIT;
	g_ui8TimeoutFlag = 0x00;
	while(g_sTrfStatus == RX_WAIT){
		// Wait for end of RX or timeout
		// Clear the IRQ Flag
		g_ui8IrqFlag = 0x00;
		// Setup for the Timer
		//MCU_setCounter(ui8RxTimeout);
		while((g_ui8IrqFlag == 0x00) && (g_ui8TimeoutFlag == 0x00));	// Wait for an interrupt &  Do Nothing

		while (g_sTrfStatus == RX_WAIT_EXTENSION){
			g_ui8IrqFlag = 0x00;
			if ((g_eTrfGeneralSettings.ui8IsoControl & 0x1F) > 0x07){
				// FIXME MCU_setCounter(7);
			}
			else{
				//FIXME MCU_setCounter(50);
			}

			while((g_ui8IrqFlag == 0x00) && (g_ui8TimeoutFlag == 0x00));	// Wait for an interrupt & Do Nothing
		}
		//RESET_COUNTER;
		if (g_sTrfStatus == RX_WAIT){
			// Exit the while loop
			g_sTrfStatus = NO_RESPONSE_RECEIVED;
		}
	}
}

//===============================================================
//
//! TRF79xxA_waitTxIRQ - Timeout sequence for just TX
//!
//! \param ui8TxTimeout is the TX timeout in milliseconds
//!
//! This function ensures data has been transmitted correctly
//! only.
//!
//! \return None.
//
//===============================================================

void TRF79xxA_waitTxIRQ(uint8_t ui8TxTimeout){
	g_sTrfStatus = RX_WAIT;
	g_ui8TimeoutFlag = 0x00;
	while((g_sTrfStatus != TX_COMPLETE) && (g_sTrfStatus != TX_ERROR))	{										// Wait for end of TX
		// Clear the IRQ Flag
		g_ui8IrqFlag = 0x00;
		// Setup for the Timer
		//FIXME MCU_setCounter(ui8TxTimeout);
		while((g_ui8IrqFlag == 0x00) && (g_ui8TimeoutFlag == 0x00));	// Wait for an interrupt & Do Nothing

		// FIXME - wer oder was bist du?  RESET_COUNTER;
		if (g_sTrfStatus != TX_COMPLETE) {
			if (g_sTrfStatus == TX_WAIT) {
				// Wait longer since we received an 0xA0
				TRF79xxA_waitTxIRQ(ui8TxTimeout);	// Wait longer for transmission to complete
			}
			else {
				// Failed to send packet properly - Exit TX Timeout
				g_sTrfStatus = TX_ERROR;	// Set status to error
			}
		}
	}
}
