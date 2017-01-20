/*
 * mb.h
 *
 *  Created on: Jan 18, 2017
 *      Author: raphael
 */

#ifndef MB_H_
#define MB_H_

#include <stdbool.h>
#include <stdint.h>

int setup_mb(void);
void pend_mb(uint16_t* output);
void post_mb(uint16_t* frequency);

#endif /* MB_H_ */
