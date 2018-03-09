#pragma config WDTEN = OFF
#pragma config FOSC  = INTIO67
#pragma config XINST = OFF

#include <p18f46k22.h>
#include <stdio.h>
#include <stdlib.h>

#include "ADS1298.h"
#include "Communication.h"
#include "LogicAnalyzer.h"

void main() {
	unsigned char status;
	unsigned char* dummy;
	
	/* Set the PIC clock frequency */
    OSCCON = 0b01110110; // set clock to 16 MHz
	
	/* Initialize the ADS1298 */
	status = ADS1298_Initialize(1ul);
	LogicAnalyzer_Init();
    
	/* Keep reading these registers */
	if (status) {
		while (1) {
            /* Read the test data */
            ADS1298_ReadData(dummy, 1ul);
            
            /* Print the data to the logic analyzer */
            LogicAnalyzer_OutChar(*(dummy + 3));
		}
	}
}