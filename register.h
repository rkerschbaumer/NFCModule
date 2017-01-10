/*
 * register.h
 *
 *  Created on: 06. Jän. 2017
 *      Author: Raphael
 */

#ifndef REGISTER_H_
#define REGISTER_H_

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
#define STATUSRSSI	0x0A
#define READCHAN	0x0B
#define RDSA		0x0C
#define RDSB		0x0D
#define RDSC		0x0E
#define RDSD		0x0F

#define SLAVE_ADDRESS 0x10 //001 0000b

#endif /* REGISTER_H_ */
