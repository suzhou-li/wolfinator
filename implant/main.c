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
#include <stdio.h>
#include <stdlib.h>

#include "CommADS1298.h"
#include "ADS1298.h"
#include "CommCC110L.h"
#include "CC110L.h"
#include "LogicAnalyzer.h"

/******************************************************************************/
/* INTERRUPTS																  */
/******************************************************************************/
void InterruptVectorHigh() { 
	_asm
		goto InterruptHigh
	_endasm
}

void InterruptHigh() {
    CC110L_ISR();
}

/******************************************************************************/
/* MAIN FUNCTION															  */
/******************************************************************************/
void main() {
	unsigned char status;
    unsigned char dummy[2] = {0x51,'b'};
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
	
    TRISBbits.TRISB0 = 0;
    LATBbits.LATB7 = 0;
    
	/* Initialize the ADS1298 */
	channels[0] = 0b10000000; // device 1 channels
	channels[1] = 0b00000000; // device 2 channels
	status = ADS1298_Initialize(channels);
	
    /* Initialize the SPI communication */
    status &= CC110L_Initialize();
    
	/* Initialize the Logic Analyzer */
	status &= LogicAnalyzer_Initialize();
    
    CC110L_TX_WriteBuffer(0x01);
    CC110L_TX_SendByte();
    
	/* Keep reading these registers */
	if (status) {
		while (1) {
            /* Read register data */
            //ADS1298_ReadRegisters(1, ADS1298_ID, 1, dummy);
            //ADS1298_ReadRegisters(2, ADS1298_ID, 1, dummy);
            
            /* Read the test data */
            //ADS1298_ReadData(dummy, 30ul);
            
            /* Print the data to the logic analyzer */
            //for (i = 0; i < 30; i = i + 1) {
            //    LogicAnalyzer_OutputChar(dummy[(i * 3) + 1]); // output value of the first channel
            //}
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
		 * Device is converting data. Data is being collected.
		 */
		case 0x03:
			switch (cmd) {
				/* Stop collecting data */
				case 0x00:
					mode = 0x02;
					
				/* Send collected data to the relay box */
				case 0x01:
					mode = 0x04;
				
				/* Change the channel */
				default:
					mode = mode;
			}
		
		/* Mod 0x04
		 * Device is sending data out. Currently, PIC to PIC communication.
		 */
		case 0x04:
			if (cmd) {
				mode = 0x03;
			} else {
				mode = mode;
			}
	}
	
	return mode;
}