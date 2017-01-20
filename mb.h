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
int pend_mb(uint16_t* output);
int post_mb(uint16_t* frequency);
void make_frequency_comma_again(uint16_t input, uint16_t* precomma, uint8_t* postcomma);

#endif /* MB_H_ */
