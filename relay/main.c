/******************************************************************************/
/* CONFIGURATION															  */
/******************************************************************************/
#pragma config WDTEN = OFF
#pragma config FOSC =  INTIO67
#pragma config XINST = OFF

/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include <p18f46k22.h>
#include "Serial.h"

/******************************************************************************/
/* INTERRUPTS																  */
/******************************************************************************/
#pragma interrupt interruptFunction
void interruptFunction() {
	LATCbits.LATC0 = !LATCbits.LATC0;
}

/******************************************************************************/
/* MAIN FUNCTION															  */
/******************************************************************************/
void main() {
	unsigned char status;
	
	/* Set the PIC clock frequency */
    OSCCON = 0b01110110; // set clock to 16 MHz
	
	/* Initialize the PIC pin for output */
	TRISCbits.RC0 = 0; // output at pin 0
	LATCbits.LATC0 = 0; // initialize the pin LOW
	
	/* Initialize the EUSART communication */
	status = Serial_Initialize();
	
	/* Run code indefinitely */
	if (status) {
		while (1);
	}
}


