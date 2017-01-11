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

/* Driverlib headers */
#include<driverlib/gpio.h>
#include<ti/drivers/I2C.h>

/* Board Header files */
#include<Board.h>
#include<EK_TM4C1294XL.h>

#include<string.h>

#include"register.h"
#include"i2c_task.h"

void i2c_task_fct(UArg arg0, UArg arg1) {
	I2C_Handle      handle;
	I2C_Params      i2cparams;
	I2C_Transaction i2c;

	uint8_t writebuffer[4]; //, readbuffer[4];
	uint8_t shadow_register[44];

	I2C_Params_init(&i2cparams);
	i2cparams.bitRate = I2C_400kHz;/*in case this is too fast use I2C_400kHz*/
	i2cparams.transferMode = I2C_MODE_BLOCKING;/*important if you call I2C_transfer in Task context*/
	handle = I2C_open(EK_TM4C1294XL_I2C7, &i2cparams);
	if (handle == NULL) {
		System_abort("I2C was not opened");
	}


	//ENABLE = 1 and DISABLE = 0 puts the device in powerup mode
	i2c.slaveAddress = SLAVE_ADDRESS;

	//memset(readbuffer,0,4);
	//memset(writebuffer,0,4);

	memset(shadow_register,0,44);


	//i2c.readBuf = (uint8_t*)&readbuffer[0];
	i2c.readBuf = shadow_register;
	i2c.writeBuf = writebuffer;

	i2c.writeCount = 1;
		i2c.readCount = 2;
		writebuffer[0] = 0x0A;

		if (!I2C_transfer(handle, &i2c)) {
			System_abort("Bad I2C read transfer!");
		}

		i2c.readBuf = shadow_register;
				//each register has 2 bytes and there are 16 registers so 2*16 = 0x20
				i2c.readCount = 0x20;
				//i2c.readCount = 1;
				i2c.writeCount = 0;
				i2c.writeBuf = NULL;
				if(!I2C_transfer(handle, &i2c)) {
					System_printf("error");
				}



	writebuffer[0] = POWERCFG;/*address of register to write*/
	writebuffer[1] = 0x00;
	writebuffer[2] = 0x00;/*data to be written to that address*/

	shadow_register[16]=0x00;
	shadow_register[17]=0x00;

	i2c.writeBuf = shadow_register;

	i2c.writeCount = 3;
	i2c.readCount = 0;

	if (!I2C_transfer(handle, &i2c)) {
		System_abort("Bad I2C transfer!");
	}

	Task_sleep(100);

	/* Read test */

	i2c.readBuf = shadow_register;
				//each register has 2 bytes and there are 16 registers so 2*16 = 0x20
				i2c.readCount = 0x20;
				//i2c.readCount = 1;
				i2c.writeCount = 0;
				i2c.writeBuf = NULL;
				if(!I2C_transfer(handle, &i2c)) {
					System_printf("error");
				}

	int x=0;

	for(x=0;x<3;x++);

//	writebuffer[0] = SYSCONFIG2;/*data to be written to that address*/
//	i2c.writeCount = 1;
//	if (!I2C_transfer(handle, &i2c)) {
//		System_abort("Bad I2C transfer!");
//	}
//	Task_sleep(100);
//	writebuffer[0] = 0x10;/*data to be written to that address*/
//	writebuffer[1] = 0x15;
//	System_printf("writebuffer[0]: %d %x\nwritebuffer[1]: %d %x\n",writebuffer[0],writebuffer[0], writebuffer[1], writebuffer[1]);
//	System_flush();
//	i2c.writeCount = 2;
//
//
//	if (!I2C_transfer(handle, &i2c)) {
//		System_abort("Bad I2C transfer!");
//	}
//
//
//	Task_sleep(100);
//	writebuffer[0] = TEST1;/*data to be written to that address*/
//	i2c.writeCount = 1;
//	if (!I2C_transfer(handle, &i2c)) {
//		System_abort("Bad I2C transfer!");
//	}
//
//	Task_sleep(100);
//	writebuffer[0] = 0x81;/*data to be written to that address*/
//	writebuffer[1] = 0x00;
//	System_printf("writebuffer[0]: %d %x\nwritebuffer[1]: %d %x\n",writebuffer[0],writebuffer[0], writebuffer[1], writebuffer[1]);
//	System_flush();
//	i2c.writeCount = 2;
//
//	if (!I2C_transfer(handle, &i2c)) {
//		System_abort("Bad I2C transfer!");
//	}
//	Task_sleep(100);
//	writebuffer[0] = POWERCFG;/*address of register to write*/
//		i2c.writeCount = 1;
//		if (!I2C_transfer(handle, &i2c)) {
//			System_abort("Bad I2C transfer!");
//		}
//
//		Task_sleep(100);
//		writebuffer[0] = 0x41;/*data to be written to that address*/
//		writebuffer[1] = 0x01;
//		System_printf("writebuffer[0]: %d %x\nwritebuffer[1]: %d %x\n",writebuffer[0],writebuffer[0], writebuffer[1], writebuffer[1]);
//		System_flush();
//		i2c.writeCount = 2;
//		if (!I2C_transfer(handle, &i2c)) {
//			System_abort("Bad I2C transfer!");
//		}
//

	//uint16_t value;

	/*
	 * idee f�r while loop:
	 * events posten f�r seek mode, volume, etc. (realisierung mit LED-ansteuern)
	 * */

	/*while (true) {
		value = 0;
	    i2c.writeCount = 1;
	    i2c.readCount = 1;
	    writebuffer[0] =  READLO;address to be read

	    if (!I2C_transfer(handle, &i2c)) {
			GPIO_write(Board_LED1, Board_LED_ON);
			System_abort("Bad I2C transfer!");
	    }

	    value =  readbuffer[0];read back value
	    i2c.writeCount = 1;
	    i2c.readCount = 1;
	    writebuffer[0] =  READHI;

	    if (!I2C_transfer(handle, &i2c)) {
	    	GPIO_write(Board_LED1, Board_LED_ON);
	    	System_abort("Bad I2C transfer!");
	    }
	    value +=  (readbuffer[0] << 8);
	    System_printf("value %u\n",value);
	    System_flush();
	    Task_sleep(1000);
	} */
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

	if(task_hendl = Task_create((Task_FuncPtr)i2c_task_fct, &task_params, &eb) == NULL){
		System_abort("Task_create i2c_task failed");
	}
	return 0;
}
