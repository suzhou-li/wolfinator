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

#include "CommCC110L.h"
#include "CC110L.h"
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
	unsigned char status;
    unsigned char data[6] = {'a', 'b', 'c', 'd', 0x00, 0x00};
	
	/* Set the PIC clock frequency */
    OSCCON = 0b01110110; // set clock to 16 MHz
	
	/* Initialize the EUSART communication */
	status = Serial_Initialize();
	
    /* Initialize the SPI communication */
    status &= CC110L_Initialize();
    
	/* Run code indefinitely */
	if (status) {
		while (1) {
            CommCC110L_CS_DPIN = 0;
            CommCC110L_Read(data, 0x01);
            CommCC110L_CS_DPIN = 1;
		}
	}
}


