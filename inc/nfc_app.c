/*
 * nfc_app.c
 *
 *  Created on: 15. Mai 2017
 *      Author: Raphael
 */

#include "nfc_app.h"
#include "iso14443a.h"
#include "ndef.h"
//*****************************************************************************
//
//! NFC_init - Initialize the layers for each enabled tag type.
//!
//! This function calls the needed initialization functions for
//! each enabled tag type technology and any other related files.
//!
//! \return None
//
//*****************************************************************************
void NFC_init(void) {
	ISO14443A_init();
	NDEF_setBlockNumberBit(0);
}

void NFC_findTag(void){
	// Clear IRQ Flags
	//IRQ_CLR;
	TRF79xxA_reset();			// Soft Reset the TRF79xxA to ensure it is in a good state at the start of each process
								// This is a recommended best practice
 	NFC_appIso14443a();			// Scan for NFC Type 4A / ISO14443A tags
								// For NFC Type 4A tags - Attempt to read NDEF contents
								// For NFC Type 2 tags - Read block data from the tag
}

//*****************************************************************************
//
//! NFC_appIso14443a - Customizeable application to search for ISO14443A/NFC
//! Type 2/4A Tag Platform compliant tags.
//!
//! Function configures TRF79xxA for ISO14443A and waits for a guard time which
//! allows the PICC to become powered by the RF field before sending a command.
//!
//! Tag detection is handled in this example by issuing a REQA
//! command, and if a reply is received, the ISO14443A anti-
//! collision process is run until a single tag has provided it's
//! entire UID. If the tag is Type 4 compliant, an NDEF read
//! function is called. If the tag is not Type 4 Compliant, then
//! a Type 2 Application is called.
//!
//! \return None.
//
//*****************************************************************************

uint8_t NFC_appIso14443a(void) {
//#if (TRF79xxA_VERSION == 70)
	if (TRF79xxA_checkExternalRfField() == true) {
		return STATUS_FAIL;
	}
//#endif

	TRF79xxA_setupInitiator(0x88);		// Configure the TRF79xxA for ISO14443A @ 106kbps and Receive no CRC

	// When a PICC is exposed to an unmodulated operating field
	// it shall be able to accept a quest within 5 ms.
	// PCDs should periodically present an unmodulated field of at least
	// 5.1 ms duration. (ISO14443-3)
	Task_sleep(6);
	ISO14443A_setRecursionCount(0); 		// Clear the recursion count for anticollision loops
	if (ISO14443A_selectTag(REQA) == STATUS_SUCCESS) {	//  Do a complete anticollision sequence as described in ISO14443-3 standard for type A
		if (ISO14443A_getType4ACompliance() == true) {
			NFC_appIso14443aType4NDEF();	// For a Type 4A compliant tag, the tag is put into Layer 4, and in order to attempt to read/write NDEF contents
		}
		else {
			NFC_appIso14443aType2(0x10);	// If a tag that is not Type 4A compliant then assume it is NFC Type 2. Proceed to read data block(s)
		}
	}
	TRF79xxA_turnRfOff();

	return STATUS_SUCCESS;
}

//*****************************************************************************
//
//! NFC_appIso14443aType4NDEF - Customizeable application to read NDEF data
//! from a Type 4A NDEF Formatted tag.
//!
//! This function sends the needed commands to select and read NDEF data from
//! a Type 4A Compliant NFC tag. Tags which are Type 4A compliant are selected
//! via RATS. The PPS command can then be used to change the data rate from
//! 106kbps up to 848kbps.
//!
//! If the tag contains an NDEF message, then the NDEF API's will attempt to
//! select the NDEF file and read the data contained within it.
//!
//! \return None.
//
//*****************************************************************************
void NFC_appIso14443aType4NDEF(void){
	TRF79xxA_writeRegister(TRF79XXA_ISO_CONTROL,0x08);	// Enable RX with CRC
	if(ISO14443A_sendRATS() == STATUS_SUCCESS) {
		if(ISO14443A_sendPPS() == STATUS_SUCCESS) {
			NDEF_setBlockNumberBit(0);

			if(NDEF_selectApplication() == STATUS_SUCCESS) {
				// Selects NDEF Application
				Task_sleep(1);						// Short delay before sending next command
				NDEF_readCapabilityContainer();
				Task_sleep(1);						// Short delay before sending next command
				NDEF_readApplication();
			}
		}
	}
}

//*****************************************************************************
//
//! NFC_appIso14443aType2 - Customizeable application to read data from
//! ISO14443A compliant tags that do not support NFC Forum Type 4A commands.
//!
//! \param ui8ReadBlocks is the number of blocks to read from the tag.
//!
//! This function reads block data from Type 2 Tags using the Read Four Blocks
//! command. The number of blocks to be read needs to be customzied by the
//! user either in an application specific manner, or by implementing a
//! look-up table feature that uses the SAK response to determine what the tag
//! is and how large it's memory contents are.
//!
//! \return None.
//
//*****************************************************************************

void NFC_appIso14443aType2(uint8_t ui8ReadBlocks){
	uint8_t ui8BlockNumber = 0;

	// If tag is not ISO14443-4 Compliant, then attempt to read data blocks for Type 2 Tag
	for (ui8BlockNumber = 0x00; ui8BlockNumber < ui8ReadBlocks; ui8BlockNumber = ui8BlockNumber+4) {
		if (ISO14443A_sendT2TReadFourBlocks(ui8BlockNumber) == STATUS_FAIL) {
			break;
		}
	}
}
