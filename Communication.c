/***************************************************************************//**
 *   @file   Communication.c
 *   @brief  Implementation of Communication Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: 570
*******************************************************************************/

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "Communication.h"

/***************************************************************************//**
 * @brief Initializes the SPI communication peripheral.
 *
 * @param lsbFirst - Transfer format (0 or 1).
 *                   Example: 0x0 - MSB first.
 *                            0x1 - LSB first.
 *
 * @return 0 - Initialization failed, 1 - Initialization succeeded.
*******************************************************************************/
unsigned char SPI_ADS1298_Init(unsigned char lsbFirst) {
	
	/* Re-initialize the SSP1 control register 1 and the status register */
	SSP1CON1 = 0x00; // SSP control register 1
	SSP1STAT = 0x00; // SSP status register

	/* SSP1 Status Register bits */

	SPI_ADS1298_SAMPLING = 0; // master mode sampling occurs at the middle of data output time
	//SPI_ADS1298_SAMPLING = 1; // master mode sampling occurs at the end of data output time (What we originally thought it was)

	SPI_ADS1298_CLKEDGE = 0; // transmit occurs on transition from active to idle clock state
	//SPI_ADS1298_CLKEDGE = 1; // transmit occurs on transition from idle to active clock state 

	/* SSP1 Control Register 1 bits */

	SPI_ADS1298_CLKPOL = 0; // idle state for clock is low
	//SPI_ADS1298_CLKPOL = 1; // idle state for clock is high
	
	SPI_ADS1298_FOSC = 0000; // set frequency of the shift clock
	
	SPI_ADS1298_ENABLE = 1; // enable the SPI

	/* Properly configure the SPI/communication pins */
	
	SPI_ADS1298_SCLK_DIR = 0; // SCLK on ADS1298 is output
    
	SPI_ADS1298_DOUT_DIR   = 1; // DOUT on ADS1298 is input into the PIC    
	SPI_ADS1298_DOUT_ANSEL = 0; // clear analog select bit for PIC input

	SPI_ADS1298_DIN_DIR = 0; // DIN on ADS1298 is output from PIC
	
	SPI_ADS1298_CS_DIR = 0; // CS on ADS1298 is output from PIC
	
	SPI_ADS1298_DRDY_DIR   = 1; // DRDY on ADS1298 is input into PIC
	SPI_ADS1298_DRDY_ANSEL = 0;	// clear analog select bit for DRDY
	
	SPI_ADS1298_START_DIR = 0;
	
	/* Properly configure the other pins */
	
	ADS1298_RESET_DIR = 0; // RESET is output

	ADS1298_PWR_DIR = 0; // PWRDN on ADS1298 is output from PIC

	return 1;
}

/***************************************************************************//**
 * @brief Writes data to SPI.
 *
 * @param data - data represents the write buffer.
 * @param bytesNumber - Number of bytes to write.
 *
 * @return Number of written bytes.
*******************************************************************************/
unsigned char SPI_ADS1298_Write(unsigned char* data,
                                unsigned char bytesNumber)
{
    unsigned char i;
    
    for(i = 0; i < bytesNumber; i++) {
        SPI_ADS1298_DATABUFFER = *data++;
        while(!SPI_ADS1298_BUFFERFULL);
    }
    
	return bytesNumber;
}

/***************************************************************************//**
 * @brief Reads data from SPI.
 *
 * @param data - Data represents the read buffer.
 * @param bytesNumber - Number of bytes to read.
 *
 * @return Number of read bytes.
*******************************************************************************/
unsigned char SPI_ADS1298_Read(unsigned char* data,
                               unsigned char bytesNumber)
{
    unsigned char i;
    
    for(i = 0; i < bytesNumber; i++) {  
        SPI_ADS1298_DATABUFFER = 0x00; // write 0's to the data buffer to shift bits in
        while(!SPI_ADS1298_BUFFERFULL); // while transmission has yet to be completed, wait
        *data++ = SPI_ADS1298_DATABUFFER; 
    }
    
    return bytesNumber;
}

/***************************************************************************//**
 * @brief Issues "don't care" clock cycles.
 *
 * @param bitsNumber - Number of cycles to wait.
 *
 * @return None.
*******************************************************************************/
void SPI_ADS1298_Wait(unsigned char bitsNumber)
{
	unsigned char i;
	
	for(i = 0; i < bitsNumber; i++) {
		SPI_ADS1298_DATABUFFER = 0x00;
	}
}