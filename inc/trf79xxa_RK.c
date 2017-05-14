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
	SPI_setup();

	// Delay to allow SPI to finish starting up
	SysCtlDelay(100);

	// Send out SOFT_INIT + IDLE initial sequence to soft reset TRF7970A
	TRF79xxA_sendDirectCommand(TRF79XXA_SOFT_INIT_CMD);
	TRF79xxA_sendDirectCommand(TRF79XXA_IDLE_CMD);

	// Delay to ensure soft reset has processed
	SysCtlDelay(100);

#if (TRF79xxA_VERSION == 70)
	g_eTrfGeneralSettings.ui8IsoControl = 0x21;
#elif (TRF79xxA_VERSION == 60)
	g_eTrfGeneralSettings.ui8IsoControl = 0x02;
#endif

	TRF79xxA_resetFIFO();			// Reset the FIFO

	TRF79xxA_writeRegister(TRF79XXA_MODULATOR_CONTROL, 0x01); 		// ASK 100%, no SYS_CLK output

	TRF79xxA_writeRegister(TRF79XXA_REGULATOR_CONTROL, 0x01);

#if (TRF79xxA_VERSION == 70)
	TRF79xxA_writeRegister(TRF79XXA_NFC_TARGET_LEVEL, 0x00); 		// For TRF7970A Errata
#endif
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
