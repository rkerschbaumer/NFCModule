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

enum mb_type{ frq, vol, mode };

int setup_mb(void);
void pend_mb(uint16_t* output, enum mb_type x);
//void post_mb(uint16_t* value, enum mb_type x);
void post_mb(uint16_t value, enum mb_type x);
void make_frequency_comma_again(uint16_t input, uint16_t* precomma, uint8_t* postcomma);


#endif /* MB_H_ */
