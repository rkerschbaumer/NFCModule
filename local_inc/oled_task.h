/*
 * oled_task.h
 *
 *  Created on: 20. Jän. 2017
 *      Author: Raphael
 */

#ifndef OLED_TASK_H_
#define OLED_TASK_H_

#include"mb.h"
#include"reg.h"
void OLED_C_command(unsigned char reg_index, unsigned char reg_value);
//void OLED_C_data(unsigned char data_value);
void OLED_C_data(uint16_t data_value);

void OLED_C_Init(void);

void DDRAM_access(void);
void OLED_C_MemorySize(char X1, char X2, char Y1, char Y2);
//void OLED_C_Color(char colorMSB, char colorLSB);
void oled_data(uint16_t data);

void OLED_C_Beckground(void);
void buchstabensuppe(uint8_t x_coordinate, uint8_t y_coordinate, uint8_t font_width, uint8_t font_height, uint16_t font_color, uint16_t background_color, char char_to_draw);

void setup_oled_task(enum mb_type x, int prio, xdc_String name);
void oled_function(UArg arg0);

void spi_write(uint16_t data);
void to_string(uint16_t precomma, uint8_t postcomma, char* buffer, enum mb_type x);

#endif /* OLED_TASK_H_ */
