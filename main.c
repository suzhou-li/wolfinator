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
    OSCCON = 0b01110110; // set clock to 16 MHz
	
	/* Initialize the ADS1298 */
	status = ADS1298_Initialize(1ul);
	
	/* Keep reading these registers */
	if (status) {
		while (1) {
            //SPI_ADS1298_Wait(1);
            ADS1298_ReadData(dummy, 10ul);
			//ADS1298_ReadRegisters(ADS1298_ID, 12, dummy);
		}
	}
}