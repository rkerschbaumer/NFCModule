/*
 * i2c_task.h
 *
 *  Created on: 07. J�n. 2017
 *      Author: Raphael
 */

#ifndef I2C_TASK_H_
#define I2C_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "xdc/std.h"

void i2c_task_fct(UArg arg0, UArg arg1);
int setup_i2c_task(int prio, xdc_String name);

void  modify_shadow_reg(uint8_t reg_addr, uint8_t reg_hi_val, uint8_t reg_lo_val, uint8_t set_reset);

void read_register(void);
void write_register(uint8_t reg_addr);

void frequency_change(uint8_t direction);
void fm_tune(void);
void fm_seek(uint8_t direction);

#endif /* I2C_TASK_H_ */
