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

#include"reg.h"
#include"i2c_task.h"
#include"mb.h"
#include"leds.h"

#define SET   1
#define RESET 0

#define UP   1
#define DOWN 0

#define SAVE 1
#define SELECT 0
#define MAX_AMOUNT_OF_FAVOURITES 5

#define HIGHER_END_FREQUENCY 1080
#define LOWER_END_FREQUENCY 875

I2C_Handle 		handle;
I2C_Params		i2cparams;
I2C_Transaction	i2c;
uint8_t			shadow_register[32];
uint16_t 		frequency = LOWER_END_FREQUENCY;
uint16_t	 	volume = 8;

/* LED-functionpointer */
void (*led_on[4])() = {led_d1_on, led_d2_on, led_d3_on, led_d4_on};
void (*led_off[4])() = {led_d1_off, led_d2_off, led_d3_off, led_d4_off};

void i2c_task_fct() {
	uint8_t operating_mode = TUNE_UP;

	I2C_Params_init(&i2cparams);
	i2cparams.bitRate = I2C_400kHz;/*in case this is too fast use I2C_400kHz*/
	i2cparams.transferMode = I2C_MODE_BLOCKING;/*important if you call I2C_transfer in Task context*/
	handle = I2C_open(EK_TM4C1294XL_I2C7, &i2cparams);
	if (handle == NULL) {
		System_abort("I2C was not opened");
	}

	i2c.slaveAddress = SLAVE_ADDRESS;

	memset(shadow_register,0,32);

	read_register();

	init_i2c_fm();

	while(true){
		if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0)){
			operating_mode++;
			if(operating_mode > FAV_SELECT){
				operating_mode = TUNE_UP;
			}

			switch(operating_mode){
				case TUNE_UP:		set_led_on(TUNE_UP); 		post_mb(TUNE_UP, mode); break;
				case TUNE_DOWN:		set_led_on(TUNE_DOWN); 		post_mb(TUNE_DOWN, mode); break;
				case SEEK_UP:		set_led_on(SEEK_UP); 		post_mb(SEEK_UP, mode); break;
				case SEEK_DOWN:		set_led_on(SEEK_DOWN); 		post_mb(SEEK_DOWN, mode); break;
				case VOLUME_UP:		set_led_on(VOLUME_UP); 		post_mb(VOLUME_UP, mode); break;
				case VOLUME_DOWN:	set_led_on(VOLUME_DOWN); 	post_mb(VOLUME_DOWN, mode); break;
				case FAV_SAVE: 		set_led_on(FAV_SAVE); 		post_mb(FAV_SAVE, mode); break;
				case FAV_SELECT: 	set_led_on(FAV_SELECT); 	post_mb(FAV_SELECT, mode); break;
				default: 			System_abort("no valid operator.");
			}
			Task_sleep(150);
		}

		read_register();

		if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1)){
			//post_mb(frequency, frq);
			switch(operating_mode){
				case TUNE_UP: 		frequency_change(UP); fm_tune(frequency); break;
				case TUNE_DOWN: 	frequency_change(DOWN); fm_tune(frequency); break;
				case SEEK_UP: 		fm_seek(UP); break;
				case SEEK_DOWN: 	fm_seek(DOWN); break;
				case VOLUME_UP: 	change_volume(UP); break;
				case VOLUME_DOWN:	change_volume(DOWN); break;
				case FAV_SAVE: 		do_favourites_stuff(FAV_SAVE); break;
				case FAV_SELECT:	do_favourites_stuff(FAV_SELECT);  break;
				default: 			System_abort("no valid operator.");
			}
		}
	}
}

void modify_shadow_reg(uint8_t reg_addr, uint8_t reg_hi_val, uint8_t reg_lo_val, uint8_t set_reset){
	uint8_t shreg_addr;
	i2c.writeBuf = &shadow_register[16];
	shreg_addr = 12 + (2*reg_addr); //12 - Startvalue of reg 0x00 in shadow register

	if(set_reset == SET){
		shadow_register[shreg_addr] |= reg_hi_val;
		shadow_register[++shreg_addr] |= reg_lo_val;
	}
	else if(set_reset == RESET){
		shadow_register[shreg_addr] &= ~reg_hi_val;
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
	if(direction == UP){
		modify_shadow_reg(POWERCFG, 0x03, 0x00, SET);
	}
	if(direction == DOWN){
		modify_shadow_reg(POWERCFG, 0x02, 0x00, RESET);
		modify_shadow_reg(POWERCFG, 0x01, 0x00, SET);
	}

	write_register(POWERCFG);
	Task_sleep(10);

	do{
		read_register();
		/*FIXME  bis doher gehts, dann switcht er wieder auf an anderen channel oasch wenn if unten einkommentiert -> sollte ja eigentlich das komplette Spektrum von 87,5 bis 108 abdecken, hmmm... */
		/* SF/BL bit set is bad :( EVTL IN MAIN LOOP ABFANGEN!!! */
//		if((shadow_register[0] && 0x20) == 1 ){
//			System_printf("no matching Frequency found\n");
//			break;
//		}
		Task_sleep(10);
	}while((shadow_register[0] && 0x40) != 1 ); //while STC bit (seek/tune) is not set

	modify_shadow_reg(POWERCFG, 0x01, 0x00, RESET);
	write_register(POWERCFG);

	Task_sleep(150);

	frequency = (shadow_register[3]) + LOWER_END_FREQUENCY;
	fm_tune(frequency);
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

void fm_tune(uint16_t frequ){
	uint8_t channel;

	channel = frequ - LOWER_END_FREQUENCY;
	modify_shadow_reg(CHANNEL,0x80,0xFF, RESET);
	modify_shadow_reg(CHANNEL,0x80, channel, SET);
	write_register(CHANNEL);
	Task_sleep(10);

	modify_shadow_reg(CHANNEL,0x80, 0x00, RESET); 
	write_register(CHANNEL);
	Task_sleep(150);

	post_mb(frequ, frq);
	//post_mb(frequ, frq);

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
	task_hendl = Task_create((Task_FuncPtr)i2c_task_fct, &task_params, &eb);
	if(task_hendl == NULL){
		System_abort("Task_create i2c_task failed");
	}
	return 0;
}

void init_i2c_fm(){
	/*XOSCTEN set*/
	modify_shadow_reg(TEST1, 0x81, 0x00, SET);
	write_register(TEST1);
	Task_sleep(500);

	/*powerup condition*/
	modify_shadow_reg(POWERCFG, 0xC0, 0x01, SET);
	write_register(POWERCFG);
	Task_sleep(500);

	/* pump up the volume = low-byte-low-nibble 0x8, Spacing = low-byte-high-nibble 0x1
	 * SEEKTH = high-byte, value between 0x00 and 0x7F, channels below RSSI Threshold will not be validated, 0x2F is a !TESTVALUE! according to AN230 */
	modify_shadow_reg(SYSCONFIG2, 0x1C, 0x18, SET);
	write_register(SYSCONFIG2);
	Task_sleep(500);

	/* setting SKSNR (SNR Threshold) and SKCNT (FM Impulse Detection Threshold, recommended is 0x48*/
	modify_shadow_reg(SYSCONFIG3, 0x00, 0x7F, SET);
	write_register(SYSCONFIG3);
	Task_sleep(500);

	fm_tune(frequency);

	led_on[0]();
	post_mb(volume, vol);
	post_mb(TUNE_UP, mode);
	Task_sleep(500);
	read_register();

	/* RDS TESTCONFIG setting RDSM to verbose */
//	modify_shadow_reg(POWERCFG, 0x08, 0x00, SET);
//	write_register(POWERCFG);
//	Task_sleep(50);
}

void change_volume(uint8_t direction){
	if((volume >= 0) && (volume <= 15)){
		if((direction == UP) && (volume < 15)){
			volume++;
		}
		else if((direction == DOWN) && (volume > 0)){
			volume--;
		}

		modify_shadow_reg(SYSCONFIG2, 0x00, 0x0F, RESET);
		modify_shadow_reg(SYSCONFIG2, 0x00, volume, SET);
		write_register(SYSCONFIG2);
		post_mb(volume, vol);
		Task_sleep(100);
	}
}
void do_favourites_stuff(uint8_t mode){
	static uint8_t current_amount_of_fav = 0;
	static uint8_t switcher = 0,i;
	static uint16_t favourites[MAX_AMOUNT_OF_FAVOURITES];

	if(mode == FAV_SAVE){
		if(current_amount_of_fav < MAX_AMOUNT_OF_FAVOURITES){
			favourites[current_amount_of_fav] = frequency;
			current_amount_of_fav++;
		}
		Task_sleep(150);
	}
	else if(mode == FAV_SELECT){

		//while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) || )
		if(current_amount_of_fav != 0) post_mb(favourites[switcher], frq);
		Task_sleep(400);
		if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) && (current_amount_of_fav != 0)){
			fm_tune(favourites[switcher]);
			frequency = favourites[switcher];
			return;
		}
		if(switcher < (current_amount_of_fav-1)) switcher++;
		else switcher = 0;
		System_printf("switcher:%d, current_amount_of_fav:%d\n", switcher, current_amount_of_fav);
		for(i=0;i<=current_amount_of_fav;i++){
			System_printf("sender[%d] = %d\n", i, favourites[i]);
		}
		System_flush();
	}
}

void set_led_on(uint8_t ledval){
	uint8_t i;
	for(i = 0; i < 4; i++){
		led_off[i]();
	}
	if(ledval < 4){
		led_on[ledval]();
	}
	else{
		if(ledval == 4){
			led_on[2]();
			led_on[3]();
		}
		else if(ledval == 5){
			led_on[0]();
			led_on[1]();
		}
		else if(ledval == 6){
			led_on[0]();
			led_on[3]();
		}
		else if(ledval == 7){
			led_on[1]();
			led_on[2]();
		}
	}
}

