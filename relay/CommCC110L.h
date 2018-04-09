/***************************************************************************//**
 *   @file   CommCC110L.h
 *   @brief  Header file of Communication Driver.
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

#ifndef _CommCC110L_H_
#define _CommCC110L_H_

/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include <p18f46k22.h>                                                   

/******************************************************************************/
/* DEFINE REGISTER BITS														  */
/******************************************************************************/

/* Define the SPI bits in the SSP2 Status Register */
#define CommCC110L_CLKEDGE                  SSP1STATbits.CKE // SPI Clock Edge Select bit
#define CommCC110L_SAMPLING                 SSP1STATbits.SMP // SPI Data Input Sample bit
#define CommCC110L_BUFFERFULL               SSP1STATbits.BF // SPI Buffer Full Status bit

/* Define the SPI bits in SSP2 Control Register 1 */
#define CommCC110L_ENABLE                   SSP1CON1bits.SSPEN
#define CommCC110L_CLKPOL                   SSP1CON1bits.CKP
#define CommCC110L_MODE                     SSP1CON1bits.SSPM // set SCLK to run FOSC/4 for SPI

/* Define the SPI bits for the CC110L data buffer */
#define CommCC110L_DATABUFFER               SSP1BUF

/* Define the MSSP bits on the PIR (Peripheral Interrupt Request) Register */
#define CommCC110L_SSPINTERRUPT             PIR1bits.SSP1IF // Synchronous Serial Port 2 Interrupt Flag bit

/* Define the MSSP bits on the PIE (Peripheral Interrupt Enable) Register */
#define CommCC110L_SSPINT_ENABLE            PIE1bits.SSP1IE // Synchronous Serial Port 2 Interrupt Enable bit

/* Define the MSSP bits on the IPR (Peripheral Interrupt Priority) Register */
#define CommCC110L_SSPINT_PRIORITY          IPR1bits.SSP1IP // Synchronous Serial Port 2 Interrupt Priority bit

/* Define the pins of the SPI for the CC110L in the PIC */
#define CommCC110L_SCLK_DIR                 TRISCbits.RC3       // SCLK on RC3 direction
#define CommCC110L_SCLK_PIN                 LATCbits.LATC3      // PIC SCLK pin

#define CommCC110L_DIN_DIR                  TRISCbits.RC4       // Into implant from relay
#define CommCC110L_DIN_ANSEL                ANSELCbits.ANSC4

#define CommCC110L_DOUT_DIR                 TRISCbits.RC5       // Out of implant to relay

#define CommCC110L_CS_DIR                   TRISAbits.RA5       // PIC CS input and output
#define CommCC110L_CS_DPIN                  LATAbits.LATA5
#define CommCC110L_CS_APIN                  PORTAbits.RA5

/******************************************************************************/
/* FUNCTIONS PROTOTYPES														  */
/******************************************************************************/

/* Initializes the SPI communication peripheral. */
unsigned char CommCC110L_Initialize();

/* Writes data to SPI. */
unsigned char CommCC110L_Write(unsigned char* data,
								unsigned char bytesNumber);

/* Reads data from SPI. */
unsigned char CommCC110L_Read(unsigned char* data,
							   unsigned char bytesNumber);

#endif	// _CommCC110L_H_
