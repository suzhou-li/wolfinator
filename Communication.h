/***************************************************************************//**
 *   @file   Communication.h
 *   @brief  Header file of Communication Driver.
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

#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include <p18f46k22.h>                                                   

/******************************************************************************/
/* DEFINE VARIABLES															  */
/******************************************************************************/

/* Define the SPI bits in the SSP1 Status Register */
#define SPI_ADS1298_CLKEDGE			SSP1STATbits.CKE // SPI Clock Edge Select bit
#define SPI_ADS1298_SAMPLING		SSP1STATbits.SMP // SPI Data Input Sample bit
#define SPI_ADS1298_BUFFERFULL		SSP1STATbits.BF // SPI Buffer Full Status bit

/* Define the SPI bits in SSP1 Control Register 1 */
#define SPI_ADS1298_ENABLE  		SSP1CON1bits.SSPEN
#define SPI_ADS1298_CLKPOL			SSP1CON1bits.CKP
#define SPI_ADS1298_FOSC 			SSP1CON1bits.SSPM // set SCLK to run FOSC/4 for SPI

/* Define the SPI bits for the ADS1298 data buffer */
#define SPI_ADS1298_DATABUFFER		SSP1BUF

/* Define the pins of the SPI for the ADS1298 in the PIC */
#define SPI_ADS1298_SCLK_DIR		TRISCbits.RC3       // SCLK on RC3 direction

#define SPI_ADS1298_DOUT_DIR		TRISCbits.RC4       // ADS DOUT to RC4 direction
#define SPI_ADS1298_DOUT_ANSEL		ANSELCbits.ANSC4    // PIC DIN pin analog select bit

#define SPI_ADS1298_DIN_DIR			TRISCbits.RC5       // ADS DIN from RC5 direction

#define SPI_ADS1298_CS_DIR			TRISAbits.RA0       // ADS CS from RA0 direction
#define SPI_ADS1298_CS_PIN			LATAbits.LATA0      // PIC CS pin (output) 

#define SPI_ADS1298_DRDY_DIR		TRISAbits.RA3       // DRDY pin direction
#define SPI_ADS1298_DRDY_ANSEL		ANSELAbits.ANSA3    // DRDY pin analog select bit
#define SPI_ADS1298_DRDY_NOT		PORTAbits.RA3       // DRDY pin (input)

#define SPI_ADS1298_START_DIR		TRISAbits.RA5		// ADS START from RA5 direction
#define SPI_ADS1298_START_PIN		LATAbits.LATA5		// PIC START pin (output)

/* Define the other pins for the ADS1298 */
#define ADS1298_RESET_DIR		TRISAbits.RA2       // RESET pin direction
#define ADS1298_RESET_PIN   	LATAbits.LATA2      // RESET pin (output)

#define ADS1298_PWR_DIR			TRISAbits.RA4		// PWDN pin direction
#define ADS1298_PWR_PIN			LATAbits.LATA4		// PWDN pin (output)

/******************************************************************************/
/* FUNCTIONS PROTOTYPES														  */
/******************************************************************************/

/* Initializes the SPI communication peripheral. */
unsigned char SPI_ADS1298_Init(unsigned char lsbFirst);

/* Writes data to SPI. */
unsigned char SPI_ADS1298_Write(unsigned char* data,
								unsigned char bytesNumber);

/* Reads data from SPI. */
unsigned char SPI_ADS1298_Read(unsigned char* data,
							   unsigned char bytesNumber);

#endif	// _COMMUNICATION_H_
