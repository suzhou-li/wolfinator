#pragma config WDTEN = OFF
#pragma config FOSC  = INTIO67
#pragma config XINST = OFF

#include <p18f46k22.h>
#include <stdio.h>
#include <stdlib.h>

#include "ADS1298.h"
#include "LogicAnalyzer.h"
#include "SPI_ADS1298.h"

void main() {
	unsigned char status;
    unsigned char dummy[100];
    unsigned char channels[2] = {0, 0};
    unsigned long i;
    
	/* Set the PIC clock frequency */
	/* bit 7   (IDLEN): 0 = Device enter Sleep mode on SLEEP instruction
	 * bit 6-4 (IRCF): 111 = 16 MHz internal RC oscillator
	 * bit 3   (OSTS): 0 = Device is running from internal oscillator
	 * bit 2   (HFIOFS): 1 = HFINTOSC frequency is stable
	 * bit 1-0 (SCS): 1x = Internal oscillator block
	 */
    OSCCON = 0b01110110; // set clock to 16 MHz
	
	/* Initialize the ADS1298 */
	channels[0] = 0b10000000; // device 1 channels
	channels[1] = 0b00000000; // device 2 channels
	status = ADS1298_Initialize(channels);
	
	/* Initialize the Logic Analyzer */
	status &= LogicAnalyzer_Initialize();
    
	/* Keep reading these registers */
	if (status) {
		while (1) {
            /* Read register data */
            //ADS1298_ReadRegisters(1, ADS1298_ID, 1, dummy);
            //ADS1298_ReadRegisters(2, ADS1298_ID, 1, dummy);
            
            /* Read the test data */
            ADS1298_ReadData(dummy, 30ul);
            
            /* Print the data to the logic analyzer */
            for (i = 0; i < 30; i = i + 1) {
                LogicAnalyzer_OutputChar(dummy[(i * 3) + 1]); // output value of the first channel
            }
		}
	}
}