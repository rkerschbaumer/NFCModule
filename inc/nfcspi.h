/*
 * spi.h
 *
 *  Created on: 13. Mai 2017
 *      Author: Raphael
 */

#ifndef INC_NFCSPI_H_
#define INC_NFCSPI_H_

#include <stdint.h>
#include <stdbool.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <inc/hw_memmap.h>
#include <driverlib/ssi.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>

void SPI_directCommand(uint8_t ui8Command);
void SPI_sendByte(uint8_t ui8TxByte);
void SPI_writeSingle(uint8_t * pui8Buffer);
void SPI_setup(void);
void SPI_usciSet(void);
void SPI_readSingle(uint8_t * pui8Buffer);
uint32_t SPI_receiveByte(void);
void SPI_rawWrite(uint8_t * pui8Buffer, uint8_t ui8Length, bool bContinuedSend);

#endif /* INC_NFCSPI_H_ */
