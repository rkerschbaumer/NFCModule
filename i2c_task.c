#ifndef NULL
#define NULL ((void *) 0)
#endif

#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>

/* Driverlib headers */
#include<driverlib/gpio.h>
#include<ti/drivers/I2C.h>

/* Board Header files */
#include<Board.h>
#include<EK_TM4C1294XL.h>

#include<string.h>

#include"register.h"
#include"i2c_task.h"

#define SET   1
#define RESET 0

#define UP   1
#define DOWN 0

#define HIGHER_END_FREQUENCY 1080
#define LOWER_END_FREQUENCY 875

I2C_Handle      handle;
I2C_Params      i2cparams;
I2C_Transaction i2c;
uint8_t shadow_register[32];
uint16_t frequency=LOWER_END_FREQUENCY;


void i2c_task_fct(UArg arg0, UArg arg1) {

	//init_interrupt();
	I2C_Params_init(&i2cparams);
	i2cparams.bitRate = I2C_400kHz;/*in case this is too fast use I2C_400kHz*/
	i2cparams.transferMode = I2C_MODE_BLOCKING;/*important if you call I2C_transfer in Task context*/
	handle = I2C_open(EK_TM4C1294XL_I2C7, &i2cparams);
	if (handle == NULL) {
		System_abort("I2C was not opened");
	}

	//ENABLE = 1 and DISABLE = 0 puts the device in powerup mode
	i2c.slaveAddress = SLAVE_ADDRESS;

	memset(shadow_register,0,32);

	read_register();

	/*XOSCTEN set*/
	modify_shadow_reg(TEST1, 0x81, 0x00, SET);
	write_register(TEST1);
	Task_sleep(100);


	/*powerup condition*/
	modify_shadow_reg(POWERCFG, 0xC0, 0x01, SET);
	write_register(POWERCFG);
	Task_sleep(100);

	/* pump up the volume, oida = low-nibble-low-byte 0xF, Spacing = high-nibble-low-byte 0x1
	 * SEEKTH = high-byte, value between 0x00 and 0x7F, channels below RSSI Threshold will not be validated, 0x2F is a !TESTVALUE! according to AN230 */
	modify_shadow_reg(SYSCONFIG2, 0x1C, 0x1F, SET);
	write_register(SYSCONFIG2);
	Task_sleep(100);

	read_register();
	Task_sleep(100);

	/* setting SKSNR (SNR Threshold) and SKCNT (FM Impulse Detection Threshold, recommended is 0x48*/
	modify_shadow_reg(SYSCONFIG3, 0x00, 0x7F, SET);
	write_register(SYSCONFIG3);
	Task_sleep(100);

	read_register();
	Task_sleep(100);

	fm_tune();

//	/*start Tuning to 87,5*/
//	modify_shadow_reg(CHANNEL, 0x80, 0x00, SET);
//	write_register(CHANNEL);
//	Task_sleep(100); //GP2 Interrupt could be used, could be used for STC & RDS
//
//	read_register();
//
//	/*stop Tuning */
//	modify_shadow_reg(CHANNEL, 0x80, 0x00, RESET);
//	write_register(CHANNEL);
//	Task_sleep(100);

	read_register();

	while(true){
		if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0)){
			/* FIXME: implement support of both, seek & tune, properly*/
			Task_sleep(500);
			if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0)){
				fm_seek(UP);
			}
			else{
				frequency_change(UP);
				fm_tune();
			}
		}
		if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1)){
			Task_sleep(500);
			if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1)){
				fm_seek(DOWN);
			}
			else{
				frequency_change(DOWN);
				fm_tune();
			}
		}
	}


	/*
	 * idee f�r while loop:
	 * events posten f�r seek mode, volume, etc. (realisierung mit LED-ansteuern)
	 * */
}

void modify_shadow_reg(uint8_t reg_addr, uint8_t reg_hi_val, uint8_t reg_lo_val, uint8_t set_reset){
	uint8_t shreg_addr;
	i2c.writeBuf = &shadow_register[16];
	shreg_addr = 12 + (2*reg_addr); //12 - Startvalue of reg 0x00 in shadow register

	if(set_reset == SET){
		shadow_register[shreg_addr]   |= reg_hi_val;
		shadow_register[++shreg_addr] |= reg_lo_val;
	}
	else if(set_reset == RESET){
		shadow_register[shreg_addr]   &= ~reg_hi_val;
		shadow_register[++shreg_addr] &= ~reg_lo_val;
	}
}

void write_register(uint8_t reg_addr){
	i2c.writeCount = (reg_addr - 1)*2;
	i2c.readCount = 0;

	if (!I2C_transfer(handle, &i2c)) {
		System_abort("Bad I2C write-transfer!");
	}
}

void read_register(void){
	i2c.readBuf = shadow_register;
	//each register has 2 bytes and there are 16 registers so 2*16 = 0x20
	i2c.readCount = 0x20;
	i2c.writeCount = 0;
	i2c.writeBuf = NULL;
	if(!I2C_transfer(handle, &i2c)) {
		System_abort("Bad I2C read-transfer!");
	}
}

void fm_seek(uint8_t direction){
	uint8_t dir;
	if(direction == UP){
		modify_shadow_reg(POWERCFG, 0x03, 0x00, SET);
	}
	if(direction == DOWN){
		modify_shadow_reg(POWERCFG, 0x02, 0x00, RESET);
		modify_shadow_reg(POWERCFG, 0x01, 0x00, SET);
	}

	write_register(POWERCFG);
	Task_sleep(100);

	do{
		read_register();
		/*FIXME  bis doher gehts, dann switcht er wieder auf an anderen channel oasch wenn if unten einkommentiert -> sollte ja eigentlich das komplette Spektrum von 87,5 bis 108 abdecken, hmmm... */
		/* SF/BL bit set is bad :( */
//		if((shadow_register[0] && 0x20) == 1 ){
//			System_printf("no matching Frequency found\n");
//			break;
//		}
		//Task_sleep(50);
	}while((shadow_register[0] && 0x40) != 1 ); //while STC bit (seek/tune) is not set

	modify_shadow_reg(POWERCFG, 0x01, 0x00, RESET);
	write_register(POWERCFG);

	Task_sleep(100);

	frequency = (shadow_register[3]) + LOWER_END_FREQUENCY;
	fm_tune();
}

void frequency_change(uint8_t direction){
	if(direction == UP){
		frequency++;
	}
	if(direction == DOWN){
		frequency--;
	}
	/* in Hex between 0x00 for 87,5 MHz and 0xCD for 108 MHz*/
	if (frequency > HIGHER_END_FREQUENCY){
		frequency = LOWER_END_FREQUENCY;
	}
	if (frequency < LOWER_END_FREQUENCY){
		frequency = HIGHER_END_FREQUENCY;
	}
}

void fm_tune(){
	uint8_t channel;

	channel = frequency - LOWER_END_FREQUENCY;
	modify_shadow_reg(CHANNEL,0x80,0xFF, RESET);
	modify_shadow_reg(CHANNEL,0x80, channel, SET);
	write_register(CHANNEL);
	Task_sleep(100);

	modify_shadow_reg(CHANNEL,0x80, 0x00, RESET);
	write_register(CHANNEL);
	Task_sleep(100);
	System_printf("Channel: %d, Frequency: %d\n",channel,frequency);
	System_flush();
	read_register();
}

int setup_i2c_task(int prio, xdc_String name){
	Task_Params task_params;
	Task_Handle task_hendl;
	Error_Block eb;

	Error_init(&eb);
	Task_Params_init(&task_params);
	task_params.instance->name = name;
	task_params.stackSize = 1024;
	task_params.priority = prio;
	//task_params.arg0/1 ??

	if((task_hendl = Task_create((Task_FuncPtr)i2c_task_fct, &task_params, &eb)) == NULL){
		System_abort("Task_create i2c_task failed");
	}
	return 0;
}

