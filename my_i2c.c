#include"my_i2c.h"
#include <stdbool.h> /*driverlib header requires stdbool.h to be included a first header file before any driverlib header*/
#include <stdint.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>/*supplies GPIO_PIN_x*/
#include <driverlib/sysctl.h>
#include <driverlib/i2c.h>
#include <inc/hw_memmap.h>/*supplies GPIO_PORTx_BASE*/
#include<ti/drivers/I2C.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

void i2c_init(void){
	uint32_t sysclock = Board_initGeneral(120*1000*1000);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); //PC6=GP1 & PC7=RST
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //SDI=PD1 & CLK=PD0
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); //PE4=GP2
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH); //PH2=SEN
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C7);
	GPIOPinConfigure(GPIO_PD0_I2C7SCL);
	GPIOPinConfigure(GPIO_PD1_I2C7SDA);
	GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);
	I2CMasterInitExpClk(I2C7_BASE, sysclock, I2C_400kHz);
	I2CMasterEnable(I2C7_BASE);
}

void i2c_read(void){

}

void i2c_write(void){

}

