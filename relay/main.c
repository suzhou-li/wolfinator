/******************************************************************************/
/* COMBILER INFORMATION														  */
/******************************************************************************/

/* Configuration settings */
#pragma config WDTEN = OFF
#pragma config FOSC  = INTIO67
#pragma config XINST = OFF

/* Configure the interrupt settings */
#pragma interrupt InterruptHigh
#pragma code InterruptVectorHigh = 0x08

/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include <p18f46k22.h>
#include "Serial.h"

/******************************************************************************/
/* INTERRUPTS																  */
/******************************************************************************/
void InterruptVectorHigh() { 
	_asm
		goto InterruptHigh
	_endasm
}

void InterruptHigh() {
	//Serial_ISR();
    Serial_ISR_SimpleResponse();
}

/******************************************************************************/
/* MAIN FUNCTION															  */
/******************************************************************************/
void main() {
	unsigned char status, i;
    unsigned char write[6] = {'a', 'b', 'c', 'd', 0x00, 0x00};
	
	/* Set the PIC clock frequency */
    OSCCON = 0b01110110; // set clock to 16 MHz
	
	/* Initialize the PIC pins for output */
	TRISBbits.RB0 = 0; // output at pin 0
	TRISBbits.RB1 = 0; 
	TRISBbits.RB2 = 0; 
	TRISBbits.RB3 = 0; 
    LATBbits.LATB0 = 0; // initialize the pin LOW
    LATBbits.LATB1 = 1;
    LATBbits.LATB2 = 1;
    LATBbits.LATB3 = 1;
	
	/* Initialize the EUSART communication */
	status = Serial_Initialize();
	
	/* Run code indefinitely */
	if (status) {
		while (1) {
            //Serial_TX_WriteBufferMultiple(write);
			//for (i = 0; i < 4; i = i + 1) {
			//	Serial_TX_SendByte();
			//}
		}
	}
}


