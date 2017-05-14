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

void SPI_directCommand(uint8_t ui8Command);
void SPI_sendByte(uint8_t ui8TxByte);
void SPI_writeSingle(uint8_t * pui8Buffer);
void SPI_setup(void);
void SPI_usciSet(void);


#endif /* INC_NFCSPI_H_ */
