/***************************************************************************//**
 *   @file   SPI_CC110L.c
 *   @brief  Implementation of Communication Driver.
 *   @author Suzhou Li (suzhou.li@duke.edu)
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
#include "SPI_CC110L.h"

/***************************************************************************//**
 * @brief Initializes the SPI communication peripheral for the CC110L chip.
 *
 * @param None.
 *
 * @return 0 - Initialization failed, 1 - Initialization succeeded.
*******************************************************************************/
unsigned char SPI_CC110L_Initialize() {
	
	/* Re-initialize the SSP1 control register 1 and the status register */
	SSP2CON1 = 0x00; // SSP control register 1
	SSP2STAT = 0x00; // SSP status register

	/* SSP1 Status Register bits */

	//SPI_CC110L_SAMPLING = 0; // master mode sampling occurs at the middle of data output time
	SPI_CC110L_SAMPLING = 1; // master mode sampling occurs at the end of data output time (What we originally thought it was)

	SPI_CC110L_CLKEDGE = 0; // transmit occurs on transition from active to idle clock state
	//SPI_CC110L_CLKEDGE = 1; // transmit occurs on transition from idle to active clock state 

	/* SSP1 Control Register 1 bits */

	SPI_CC110L_CLKPOL = 0; // idle state for clock is low
	//SPI_CC110L_CLKPOL = 1; // idle state for clock is high
	
	SPI_CC110L_FOSC = 0000; // set frequency of the shift clock (divide clock frequency by 4)
	
	SPI_CC110L_ENABLE = 1; // enable the SPI

	/* Properly configure the SPI/communication pins */
	
	SPI_CC110L_SCLK_DIR = 0; // SCLK on CC110L is output
    
	SPI_CC110L_DOUT_DIR   = 1; // DOUT on CC110L is input into the PIC    
	SPI_CC110L_DOUT_ANSEL = 0; // clear analog select bit for PIC input

	SPI_CC110L_DIN_DIR = 0; // DIN on CC110L is output from PIC
	
    SPI_CC110L_DRDY1_DIR   = 1; // DRDY on CC110L is input into PIC (device 1)
	SPI_CC110L_DRDY1_ANSEL = 0;	// clear analog select bit for DRDY (device 1)
	
    SPI_CC110L_DRDY2_DIR   = 1; // DRDY on CC110L is input into PIC (device 2)
	SPI_CC110L_DRDY2_ANSEL = 0;	// clear analog select bit for DRDY (device 2)
    
	SPI_CC110L_CS1_DIR = 0; // CS on CC110L is output from PIC (device 1)
    
	SPI_CC110L_CS2_DIR = 0; // CS on CC110L is output from PIC (device 2)
    
	/* Properly configure the other pins */
    
	CC110L_RESET_DIR = 0; // RESET is output

	CC110L_PWR_DIR = 0; // PWRDN on CC110L is output from PIC

	return 1;
}

/***************************************************************************//**
 * @brief Writes data to SPI of the CC110L chip.
 *
 * @param data - data represents the write buffer.
 * @param bytesNumber - Number of bytes to write.
 *
 * @return Number of written bytes.
*******************************************************************************/
unsigned char SPI_CC110L_Write(unsigned char* data,
                                unsigned char bytesNumber)
{
    unsigned char i;
    
    for(i = 0; i < bytesNumber; i++) {
        SPI_CC110L_DATABUFFER = *data++;
        while (!SPI_CC110L_BUFFERFULL);
        SPI_CC110L_INTERRUPT = 0; // reset the interrupt flag
    }
    
	return bytesNumber;
}

/***************************************************************************//**
 * @brief Reads data from SPI of the CC110L chip.
 *
 * @param data - Data represents the read buffer.
 * @param bytesNumber - Number of bytes to read.
 *
 * @return Number of read bytes.
*******************************************************************************/
unsigned char SPI_CC110L_Read(unsigned char* data,
                               unsigned char bytesNumber)
{
    unsigned char i;
    
    for(i = 0; i < bytesNumber; i++) {  
        SPI_CC110L_DATABUFFER = 0x00; // write 0's to the data buffer to shift bits in
        while (!SPI_CC110L_BUFFERFULL); // while transmission has yet to be completed, wait
        *data++ = SPI_CC110L_DATABUFFER; 
        SPI_CC110L_INTERRUPT = 0; // reset the interrupt flag
    }
    
    return bytesNumber;
}