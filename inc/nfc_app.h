/*
 * nfc_app.h
 *
 *  Created on: 15. Mai 2017
 *      Author: Raphael
 */

#ifndef INC_NFC_APP_H_
#define INC_NFC_APP_H_

#include "iso14443a.h"

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>

void NFC_init(void);
void NFC_findTag(void);
uint8_t NFC_appIso14443a(void);
void NFC_appIso14443aType4NDEF(void);
void NFC_appIso14443aType2(uint8_t ui8ReadBlocks);

#endif /* INC_NFC_APP_H_ */
