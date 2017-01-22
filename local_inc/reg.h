/*
 * register.h
 *
 *  Created on: 06. J�n. 2017
 *      Author: Raphael
 */

#ifndef REGISTER_H_
#define REGISTER_H_

/* Register 0x00 - 0x09 are stored from shadowReg[12] to shadowReg[31] */
#define DEVICEID 	0x00
#define CHIPID 		0x01
#define POWERCFG	0x02
#define CHANNEL		0x03
#define SYSCONFIG1	0x04
#define SYSCONFIG2	0x05
#define SYSCONFIG3	0x06
#define TEST1		0x07
#define TEST2		0x08
#define BOOTCONFIG	0x09
/*Register 0x0A - 0x0F are stored from shadowReg[0] to shadowReg[11] */
#define STATUSRSSI	0x0A
#define READCHAN	0x0B
#define RDSA		0x0C
#define RDSB		0x0D
#define RDSC		0x0E
#define RDSD		0x0F

#define SLAVE_ADDRESS 0x10 //001 0000b

//enum operating_modes{
//TUNE_UP,
//TUNE_DOWN,
//SEEK_UP,
//SEEK_DOWN,
//VOLUME_UP,
//VOLUME_DOWN,
//FAV_SAVE,
//FAV_SELECT };
/* Operating Modes*/
#define TUNE_UP 0
#define TUNE_DOWN 1
#define SEEK_UP 2
#define SEEK_DOWN 3
#define VOLUME_UP 4
#define VOLUME_DOWN 5
#define FAV_SAVE 6
#define FAV_SELECT 7

#endif /* REGISTER_H_ */
