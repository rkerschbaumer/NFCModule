/*
 * i2c_task.h
 *
 *  Created on: 07. Jän. 2017
 *      Author: Raphael
 */

#ifndef I2C_TASK_H_
#define I2C_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "xdc/std.h"

void i2c_task_fct(UArg arg0, UArg arg1);
int setup_i2c_task(int prio, xdc_String name);

#endif /* I2C_TASK_H_ */
