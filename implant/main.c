/******************************************************************************/
/* COMBILER INFORMATION														  */
/******************************************************************************/
#pragma config WDTEN = OFF
#pragma config FOSC  = INTIO67
#pragma config XINST = OFF

/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include <p18f46k22.h>
#include <stdio.h>
#include <stdlib.h>

#include "ADS1298.h"
#include "LogicAnalyzer.h"
#include "SPI_ADS1298.h"

/******************************************************************************/
/* MAIN FUNCTION															  */
/******************************************************************************/
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

unsigned char mode;

unsigned char changeMode(unsigned char cmd, unsigned char* data) {
	unsigned char status;
	switch (cmd) {
		
		/* Mode 0x00
		 * Device is powered OFF.
		 */
		case 0x00:
			if (cmd) {
				status = ADS1298_PowerUp();
				if (status) mode = 0x01;
			} else {
				mode = mode;
			}
			
		/* Mode 0x01
		 * Device is powered ON. All channels are turned off and shorted. Device is idle.
		 */
		case 0x01:
			switch (cmd) {
				/* Turn off the device */
				case 0x00:
					mode = 0x00;
					
				/* Select the channel */
				case 0x01:
					ADS1298_SetChannels(data);
					mode = 0x01;
					
				/* No channels selected */
				default:
					mode = mode;
			}
		
		/* Mode 0x02
		 * Device is powered ON. Channel(s) is/are turned on. Device is idle.
		 */
		case 0x02:
			switch (cmd) {
				/* Turn off the channels */
				case 0x00:
					ADS1298_SetChannels(data);
					mode = 0x01;
					
				/* Start data conversions */
				case 0x01:
					mode = 0x03;
				
				/* Select different channels */
				default:
					ADS1298_SetChannels(data);
					mode = mode;
			}
		
		/* Mode 0x03
		 * Device is converting data. No data is being collected.
		 */
		case 0x03:
			switch (cmd) {
				/* Stop data conversions */
				case 0x00:
					mode = 0x02;
					
				/* Start collecting data */
				case 0x01:
					mode = 0x04;
				
				/* Turn off all channels */
				case 0x02:
					mode = 0x01;
				
				/* Change the channel */
				default:
					mode = mode;
			}
		
		/* Mode 0x04
		 * Device is converting data. Data is being collected.
		 */
		case 0x04:
			switch (cmd) {
				/* Stop collecting data */
				case 0x00:
					mode = 0x03;
					
				/* Send collected data to the relay box */
				case 0x01:
					mode = 0x05;
				
				/* Change the channel */
				default:
					mode = mode;
			}
		
		/* Mod 0x05
		 * Device is sending data out. Currently, PIC to PIC communication.
		 */
		case 0x05:
			if (cmd) {
				mode = 0x04;
			} else {
				mode = mode;
			}
	}
	
	return mode
}