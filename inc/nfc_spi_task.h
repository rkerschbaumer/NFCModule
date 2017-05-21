/*
 * nfc_spi_task.h
 *
 *  Created on: 15. Mai 2017
 *      Author: Raphael
 */

#ifndef INC_NFC_SPI_TASK_H_
#define INC_NFC_SPI_TASK_H_

void nfc_function(UArg arg0);
void setup_nfc_spi_task(int prio, xdc_String name);


#endif /* INC_NFC_SPI_TASK_H_ */
