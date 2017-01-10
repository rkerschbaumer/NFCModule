#include <stdbool.h>
#include <stdint.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>



#include"fm.h"

/* set GP1 = PC6 to 1, set RST = PC7 to 0 */
void fm_busmode_selection(void){
    uint32_t ui32Strength;
    uint32_t ui32PinType;
    int i;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    /* only RST Pin activation and Start condition */

//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

//    GPIOPadConfigGet(GPIO_PORTH_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
//    GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_2,ui32Strength,GPIO_PIN_TYPE_STD);
//    GPIOPadConfigGet(GPIO_PORTD_BASE, GPIO_PIN_1, &ui32Strength, &ui32PinType);
//    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1,ui32Strength,GPIO_PIN_TYPE_STD);
//    GPIOPadConfigGet(GPIO_PORTD_BASE, GPIO_PIN_0, &ui32Strength, &ui32PinType);
//    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0,ui32Strength,GPIO_PIN_TYPE_STD);
//
//    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE,GPIO_PIN_2);
//    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_1);
//    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_0);

    /* configure GPIO_PIN_6 & GPIO_PIN_7 on port C as std pin */
//    GPIOPadConfigGet(GPIO_PORTC_BASE, GPIO_PIN_6, &ui32Strength, &ui32PinType);
//    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_6,ui32Strength,GPIO_PIN_TYPE_STD);

    GPIOPadConfigGet(GPIO_PORTC_BASE, GPIO_PIN_7, &ui32Strength, &ui32PinType);
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_7,ui32Strength,GPIO_PIN_TYPE_STD);

    /* set pin 6 & 7 of gpio port C to output */
//    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_6);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_7);

    /* write on the specified pins */
 //   GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x40);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x00);
//    GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_2, 0x04);
//    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);
//    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0x01);
    for(i=0;i<1000;i++);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x80);



}
