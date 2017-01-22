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
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>

/* Application headers */
#include "font.h"
#include "oled_task.h"
#include "oled_c.h"
#include "mb.h"
#include "UART_Task.h"

char modes[8][6] = {"TN up", "TN dw", "SK up", "SK dw", "VO up", "VO dw", "FV sv", "FV sl"};

//Send command to OLED C display
//fixme spi_write umbennen + value und data umbennnen
void oled_command(unsigned char reg, unsigned char value){
//Select index addr
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, 0);
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7, 0);
    spi_write(reg);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, 0xFF);
//Write data to reg
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, 0);
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7, 0xFF);
    spi_write(value);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, 0xFF);
}

//Send data to OLED C display
void oled_data(uint16_t data){
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, 0);
	GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7, 0xFF);
	spi_write(data);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, 0xFF);
}

void oled_init(){
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, 0);
    Task_sleep(10);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, 0xFF);
    Task_sleep(10);

    /*  Soft reser */
    oled_command(SEPS114A_SOFT_RESET,0x00);
    /* Standby ON/OFF*/
    oled_command(SEPS114A_STANDBY_ON_OFF,0x01);          // Standby on
    Task_sleep(5);                                           // Wait for 5ms (1ms Delay Minimum)
    oled_command(SEPS114A_STANDBY_ON_OFF,0x00);          // Standby oFF
    Task_sleep(5);                                           // 1ms Delay Minimum (1ms Delay Minimum)
    /* Display OFF */
    oled_command(SEPS114A_DISPLAY_ON_OFF,0x00);
    /* Set Oscillator operation */
    oled_command(SEPS114A_ANALOG_CONTROL,0x00);          // using external resistor and internal OSC
    /* Set frame rate */
    oled_command(SEPS114A_OSC_ADJUST,0x03);              // frame rate : 95Hz
    /* Set active display area of panel */
    oled_command(SEPS114A_DISPLAY_X1,0x00);
    oled_command(SEPS114A_DISPLAY_X2,0x5F);
    oled_command(SEPS114A_DISPLAY_Y1,0x00);
    oled_command(SEPS114A_DISPLAY_Y2,0x5F);
    /* Select the RGB data format and set the initial state of RGB interface port */
    oled_command(SEPS114A_RGB_IF,0x00);                 // RGB 8bit interface
    /* Set RGB polarity */
    oled_command(SEPS114A_RGB_POL,0x00);
    /* Set display mode control */
    oled_command(SEPS114A_DISPLAY_MODE_CONTROL,0x80);   // SWAP:BGR, Reduce current : Normal, DC[1:0] : Normal
    /* Set MCU Interface */
    oled_command(SEPS114A_CPU_IF,0x00);                 // MPU External interface mode, 8bits
    /* Set Memory Read/Write mode */
    oled_command(SEPS114A_MEMORY_WRITE_READ,0x00);
    /* Set row scan direction */
    oled_command(SEPS114A_ROW_SCAN_DIRECTION,0x00);     // Column : 0 --> Max, Row : 0 Â�--> Max
    /* Set row scan mode */
    oled_command(SEPS114A_ROW_SCAN_MODE,0x00);          // Alternate scan mode
    /* Set column current */
    oled_command(SEPS114A_COLUMN_CURRENT_R,0x6E);
    oled_command(SEPS114A_COLUMN_CURRENT_G,0x4F);
    oled_command(SEPS114A_COLUMN_CURRENT_B,0x77);
    /* Set row overlap */
    oled_command(SEPS114A_ROW_OVERLAP,0x00);            // Band gap only
    /* Set discharge time */
    oled_command(SEPS114A_DISCHARGE_TIME,0x01);         // Discharge time : normal discharge
    /* Set peak pulse delay */
    oled_command(SEPS114A_PEAK_PULSE_DELAY,0x00);
    /* Set peak pulse width */
    oled_command(SEPS114A_PEAK_PULSE_WIDTH_R,0x02);
    oled_command(SEPS114A_PEAK_PULSE_WIDTH_G,0x02);
    oled_command(SEPS114A_PEAK_PULSE_WIDTH_B,0x02);
    /* Set precharge current */
    oled_command(SEPS114A_PRECHARGE_CURRENT_R,0x14);
    oled_command(SEPS114A_PRECHARGE_CURRENT_G,0x50);
    oled_command(SEPS114A_PRECHARGE_CURRENT_B,0x19);
    /* Set row scan on/oFF  */
    oled_command(SEPS114A_ROW_SCAN_ON_OFF,0x00);        // Normal row scan
    /* Set scan oFF level */
    oled_command(SEPS114A_SCAN_OFF_LEVEL,0x04);         // VCC_C*0.75
    /* Set memory access point */
    oled_command(SEPS114A_DISPLAYSTART_X,0x00);
    oled_command(SEPS114A_DISPLAYSTART_Y,0x00);
    /* Display ON */
    oled_command(SEPS114A_DISPLAY_ON_OFF,0x01);
}

void DDRAM_access(){
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, 0);
	GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7, 0);
	spi_write(0x08);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, 0xFF);
}

//Set memory area(address) to write a display data
void OLED_C_MemorySize(char X1, char X2, char Y1, char Y2){
    oled_command(SEPS114A_MEM_X1,X1);
    oled_command(SEPS114A_MEM_X2,X2);
    oled_command(SEPS114A_MEM_Y1,Y1);
    oled_command(SEPS114A_MEM_Y2,Y2);
}

void OLED_C_Beckground(){
    unsigned int j;

    oled_command(0x1D,0x02);                //Set Memory Read/Write mode
    /* setting background to yellow*/
    OLED_C_MemorySize(0x00,0x5F,0x00,0x5F);
    DDRAM_access();
    for(j=0;j<9216;j++){
    	oled_data(0xFF00);
    }
}

//fixme variablennamen Ã¤ndern
void buchstabensuppe(uint8_t x_coordinate, uint8_t y_coordinate, uint8_t font_width, uint8_t font_height, uint16_t font_color, uint16_t background_color, char char_to_draw){
	int i, j;
	OLED_C_MemorySize(x_coordinate, x_coordinate + font_width - 1, y_coordinate, y_coordinate + font_height - 1);
	DDRAM_access();
	for(i=0; i < font_height; i++)
	{
    	for(j=0; j < font_width; j++)
    	{
    		if((font[char_to_draw][i]>>j) & 0x1)
    			oled_data(font_color);
    		else
    			oled_data(background_color);
    	}
    }
}

void oled_function(UArg arg0){
	uint8_t i, x_val = 0;
	char displaystring[] = "Penis", outval[7], frqstring[] = "Frq:", volstring[] = "Vol:",outvol[3];
	char modestring[] = "Mode:";
	uint16_t valtowrite, precomma;
	uint8_t postcomma;

	oled_init();

	oled_command(0x1D,0x02);                //Set Memory Read/Write mode
    OLED_C_MemorySize(0x00,0x5F,0x00,0x5F);
    DDRAM_access();
    for(i = 0; i < sizeof(displaystring); i++){
    	buchstabensuppe(x_val, 0x00, 0x08, 0x0E, 0x0AA0, 0xFF00, displaystring[i]);
    	x_val += 0x08;
    }

    x_val = 0;

    if((enum mb_type)arg0 == frq){
    	for(i = 0; i < sizeof(frqstring); i++){
    		 buchstabensuppe(x_val, 0x1E, 0x08, 0x0E, 0x0AA0, 0xFF00, frqstring[i]);
    	     x_val += 0x08;
    	 }
		while(1){
			pend_mb(&valtowrite, frq);
			make_frequency_comma_again(valtowrite, &precomma, &postcomma);
			to_string(precomma, postcomma, outval, frq);
			x_val = sizeof(frqstring)*8;
			for(i = 0; i < sizeof(outval); i++){
				buchstabensuppe(x_val, 0x1E, 0x08, 0x0E, 0x0AA0, 0xFF00, outval[i]);
				x_val += 0x08;
			}
		}
    }
    else if((enum mb_type)arg0 == vol){
    	 for(i = 0; i < sizeof(volstring); i++){
    		 buchstabensuppe(x_val, 0x0F, 0x08, 0x0E, 0x0AA0, 0xFF00, volstring[i]);
    	     x_val += 0x08;
    	 }
    	while(1){
			pend_mb(&valtowrite, vol);
			x_val = sizeof(volstring)*8;
			valtowrite &= 0x000F;
			to_string(valtowrite, 0, outvol, vol);
			for(i = 0; i < sizeof(outvol); i++){
				buchstabensuppe(x_val, 0x0F, 0x08, 0x0E, 0x0AA0, 0xFF00, outvol[i]);
				x_val += 0x08;
			}
		}
    }
    else if((enum mb_type)arg0 == mode){
     	 for(i = 0; i < sizeof(modestring); i++){
       		 buchstabensuppe(x_val, 0x2D, 0x08, 0x0E, 0x0AA0, 0xFF00, modestring[i]);
       	     x_val += 0x08;
       	 }
     	 System_printf("%s\n", modestring);
     	 System_flush();
       	while(1){
   			pend_mb(&valtowrite, mode);
   			x_val = sizeof(modestring)*8;
   			valtowrite &= 0x0007;
   			for(i = 0; i < 6; i++){
   				buchstabensuppe(x_val, 0x2D, 0x08, 0x0E, 0x0AA0, 0xFF00, modes[valtowrite][i]);
   				x_val += 0x08;
   			}
   		}
    }
}

void setup_oled_task(enum mb_type x, int prio, xdc_String name){
    Task_Params task_params_oled;
    Task_Handle task_hendl_oled;
    Error_Block eb;
    Error_init(&eb);
    Task_Params_init(&task_params_oled);
    task_params_oled.instance->name = name;
    task_params_oled.stackSize = 1024;
    task_params_oled.priority = prio;
    task_params_oled.arg0 = (UArg)x;
    task_hendl_oled = Task_create((Task_FuncPtr)oled_function, &task_params_oled, &eb);
        if (task_hendl_oled == NULL) {
            System_abort("Create Oled_task_setup failed");
        }
}

void spi_write(uint16_t data){
	SSIDataPut(SSI3_BASE, data);
	while(SSIBusy(SSI3_BASE)){
	}
}

void to_string(uint16_t precomma, uint8_t postcomma, char* buffer, enum mb_type x){
	uint8_t i=0,j=0;
    do{
        buffer[i++] = precomma%10+'0';
        precomma=precomma/10;
    }while(precomma!=0);

    for(j = 0; j < i/2; j++){
        buffer[j] ^= buffer[i-j-1];
        buffer[i-j-1] ^= buffer[j];
        buffer[j] ^= buffer[i-j-1];
    }

    if(x == frq){
		buffer[i] = ',';
		buffer[++i] = postcomma+'0';
		buffer[++i] = '\0';
    }
    else buffer[i] = '\0';
}
