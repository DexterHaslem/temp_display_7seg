/**
  Generated Interrupt Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    interrupt_manager.h

  @Summary:
    This is the Interrupt Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description:
    This header file provides implementations for global interrupt handling.
    For individual peripheral handlers please see the peripheral driver for
    all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F47K42
        Driver Version    :  2.12
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above or later
        MPLAB 	          :  MPLAB X 6.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "interrupt_manager.h"
#include "mcc.h"

void  INTERRUPT_Initialize (void)
{
    // Enable Interrupt Priority Vectors
    INTCON0bits.IPEN = 1;

    // Assign peripheral interrupt priority vectors

    // TMRI - high priority
    IPR6bits.TMR3IP = 1;


    // ICEI - low priority
    IPR3bits.I2C1EIP = 0;    

    // ICRXI - low priority
    IPR2bits.I2C1RXIP = 0;    

    // ICI - low priority
    IPR3bits.I2C1IP = 0;    

    // ICTXI - low priority
    IPR3bits.I2C1TXIP = 0;    

    // TMRI - low priority
    IPR3bits.TMR0IP = 0;    

    // UTXI - low priority
    IPR3bits.U1TXIP = 0;    

    // URXI - low priority
    IPR3bits.U1RXIP = 0;    

}

void __interrupt() INTERRUPT_InterruptManagerHigh (void)
{
   // interrupt handler
    if(PIE6bits.TMR3IE == 1 && PIR6bits.TMR3IF == 1)
    {
        TMR3_ISR();
    }
    else
    {
        //Unhandled Interrupt
    }
}

void __interrupt(low_priority) INTERRUPT_InterruptManagerLow (void)
{
    // interrupt handler
    if(PIE3bits.I2C1EIE == 1 && PIR3bits.I2C1EIF == 1)
    {
        I2C1_InterruptHandler();
    }
    else if(PIE2bits.I2C1RXIE == 1 && PIR2bits.I2C1RXIF == 1)
    {
        I2C1_InterruptHandler();
    }
    else if(PIE3bits.I2C1IE == 1 && PIR3bits.I2C1IF == 1)
    {
        I2C1_InterruptHandler();
    }
    else if(PIE3bits.I2C1TXIE == 1 && PIR3bits.I2C1TXIF == 1)
    {
        I2C1_InterruptHandler();
    }
    else if(PIE3bits.TMR0IE == 1 && PIR3bits.TMR0IF == 1)
    {
        TMR0_ISR();
    }
    else if(PIE3bits.U1TXIE == 1 && PIR3bits.U1TXIF == 1)
    {
        UART1_TxInterruptHandler();
    }
    else if(PIE3bits.U1RXIE == 1 && PIR3bits.U1RXIF == 1)
    {
        UART1_RxInterruptHandler();
    }
    else
    {
        //Unhandled Interrupt
    }
}
/**
 End of File
*/
