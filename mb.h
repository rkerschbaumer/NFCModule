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
<<<<<<< HEAD
void pend_mb(uint16_t* output);
void post_mb(uint16_t* frequency);
=======
int pend_mb(uint16_t* output);
int post_mb(uint16_t* frequency);
void make_frequency_comma_again(uint16_t input, uint16_t* precomma, uint8_t* postcomma);
>>>>>>> d0415662ebf1149ee1c31c91d65c5d08ca60c49d

#endif /* MB_H_ */
