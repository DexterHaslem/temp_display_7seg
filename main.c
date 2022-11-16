#include "mcc_generated_files/mcc.h"

#include <stdio.h>

uint8_t seg_nums[] = 
{  
    //dp G F E D C B A
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01100111,
};


uint8_t seg_f = 0b01110001;
uint8_t seg_dp = 0b10000000;

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    LATD = 0xff;
    //LATC = 0b101010101;
    // Enable the Global Interrupt
    INTERRUPT_GlobalInterruptEnable();

    printf("started up\r\n");
    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    //DIG4_COM_SetLow();
    //LATD = 0b11110000;
    LATC = seg_nums[1] | seg_dp;
    while (1)
    {
        for (uint8_t dig = 0; dig < 5; ++dig) {
            LATD = ~((uint8_t)1 << dig);
            __delay_ms(3);
        }
#if 1
        //for (uint8_t v = 0; v < 10; ++v) {
           
        
            //__delay_ms(200);
        //}
        
//        LATD <<= 1;
//        if (PORTD > (1 << 4)) {
//            LATD = 1;
//         
//        }
        
#endif
    }
}