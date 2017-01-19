#include <stdbool.h> /*driverlib header requires stdbool.h to be included a first header file before any driverlib header*/
#include <stdint.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>/*supplies GPIO_PIN_x*/
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <inc/hw_memmap.h>/*supplies GPIO_PORTx_BASE*/

#include "leds.h"


void led_d1_on(){
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,0x02);
}
void led_d2_on(){
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0,0x01);
}
void led_d3_on(){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4,0x10);
}
void led_d4_on(){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x01);
}

void led_d1_off(){
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,0x00);
}
void led_d2_off(){
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0,0x00);
}
void led_d3_off(){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4,0x00);
}
void led_d4_off(){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x00);
}


