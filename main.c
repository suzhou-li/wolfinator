#pragma config WDTEN = OFF
#pragma config FOSC  = INTIO67
#pragma config XINST = OFF

#include <p18f46k22.h>
#include <stdio.h>
#include <stdlib.h>
#include "ADS1298.h"
#include "Communication.h"

void main() {
	unsigned char status;
	unsigned char* dummy;
	
	/* Set the PIC clock frequency */
    OSCCON = 0b01110110; // set clock to 16 MHz //moved from below RNinit() to here)
	
	/* Initialize the ADS1298 */
	status = ADS1298_Initialize();
	
	/* Keep reading these registers */
	if (status) {
		while (1) {
            //ADS1298_ReadData(dummy, 1, 24);
			ADS1298_ReadRegisters(ADS1298_ID, 4, dummy);
		}
	}
}