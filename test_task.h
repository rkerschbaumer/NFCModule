/*
 * test_task.h
 *
 *  Created on: 19. Jän. 2017
 *      Author: Raphael
 */

#ifndef TEST_TASK_H_
#define TEST_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "xdc/std.h"

void make_frequency_comma_again(uint16_t input, uint16_t* precomma, uint8_t* postcomma);
void test_function(UArg arg0, UArg arg1);
int setup_test_task(void);


#endif /* TEST_TASK_H_ */
