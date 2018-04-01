#ifndef _SERIAL_H
#define _SERIAL_H

/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include <p18f46k22.h>

/******************************************************************************/
/* DEFINE VARIABLES															  */
/******************************************************************************/

/* Define the EUSART bits on the TxSTA1 (Transmit Status and Control) Register */
#define Serial_TX_ENABLE			TXSTA1bits.TXEN // Transmit Enable bit
#define Serial_TX_MODE				TXSTA1bits.SYNC // EUSART Mode Select bit
#define Serial_TX_HIGHRATE			TXSTA1bits.BRGH // High Baud Rate Select bit
#define Serial_TX_STATUS			TXSTA1bits.TRMT // Transmit Shift Register Status bit

/* Define the EUSART bits on the RCSTA1 (Receive Status and Control) Register */
#define Serial_RC_SERIAL			RCSTA1bits.SPEN // Serial Port Enable bit
#define Serial_RC_SINGLE			RCSTA1bits.SREN // Single Receive Enable bit
#define Serial_RC_CONT				RCSTA1bits.CREN // Continuous Receive Enable bit

/* Define the EUSART bits on the BAUDCON1 (Baud Rate Control) Register */
#define Serial_BAUD_RCPOL			BAUDCON1bits.DTRXP // Data/Receive Polarity Select bit
#define Serial_BAUD_TXPOL			BAUDCON1bits.CKTXP // Clock/Transmit Polarity Select bit
#define Serial_BAUD_BITSIZE			BAUDCON1bits.BRG16 // 16-bit Baud Rate Generator bit
#define	Serial_BAUD_AUTO			BAUDCON1bits.ABDEN // Auto-Baud Detect Enable bit

/* Define the EUSART bits for the RCREG1 Receive Register */
#define Serial_RC_REGISTER			RCREG1

/* Define the EUSART bits for the TXREG1 Transmit Register */
#define Serial_TX_REGISTER      	TXREG1

/* Define the EUSART bits for the SPBRGH:SPBRG (Baud Rate Generator) Registers */
#define Serial_BAUDRATE_HB      	SPBRGH1 // High byte
#define Serial_BAUDRATE_LB      	SPBRG   // Low byte

/* Define the EUSART bits on the PIR (Peripheral Interrupt Request) Register */
#define Serial_INT_RC           	PIR1bits.RC1IF // EUSART1 Receive Interrupt Flag bit
#define Serial_INT_TX           	PIR1bits.TX1IF // EUSART1 Transmit Interrupt Flag bit

/* Define the EUSART bits on the PIE (Peripheral Interrupt Enable) Register */
#define Serial_INTEN_RC         	PIE1bits.RC1IE // EUSART1 Receive Interrupt Enable bit
#define Serial_INTEN_TX         	PIE1bits.TX1IE // EUSART1 Transmit Interrupt Enable bit

/* Define the EUSART bits on the IPR1 (Peripheral Interrupt Priority) Register */
#define Serial_INTPRIORITY_RC   	IPR1bits.RC1IP
#define Serial_INTPRIORITY_TX   	IPR1bits.TX1IP

/* Define the interrupt bits */
#define Serial_INT_ENABLE			RCONbits.IPEN
#define Serial_INT_PRIOROTY			INTCONbits.GIEH

/* Define the RC pins on the EUSART interface */
#define Serial_RC_PIN				PORTCbits.RC7
#define Serial_RC_DIR           	TRISCbits.RC7
#define Serial_RC_ANSEL         	ANSELCbits.ANSC7

/* Define the TX pins on the EUSART interface */
#define Serial_TX_PIN           	LATCbits.LATC6
#define Serial_TX_DIR           	TRISCbits.RC6
#define Serial_TX_ANSEL         	ANSELCbits.ANSC6

/******************************************************************************/
/* FUNCTION PROTOTYPES														  */
/******************************************************************************/

/******************************************************************************/
/* General Functions Part 1													  */
/******************************************************************************/

/* Initialize the EUSART interface */
unsigned char Serial_Initialize();

/* Increments the index value */
unsigned char Serial_IncrementIndex(unsigned char idx, unsigned char max);

/******************************************************************************/
/* Receive Functions														  */
/******************************************************************************/

/* Stores new character to the RC buffer */
void Serial_RC_WriteBuffer(unsigned char data);

/* Reads a byte of data from the RC buffer */
unsigned char Serial_RC_ReadBuffer();

/* Reads a byte of data from the RC register */
void Serial_RC_ReadByte();

/* Checks if data is available on the RC buffer */
unsigned char Serial_RC_isDataAvailable();

/* Resets the RC buffer */
void Serial_RC_Clear();

/******************************************************************************/
/* Transmit Functions														  */
/******************************************************************************/

/* Stores a new character to the TX buffer */
void Serial_TX_WriteBuffer(unsigned char data);

/* Stores multiple new characters to the TX buffer */
void Serial_TX_WriteBufferMultiple(unsigned char* data);

/* Sends out a byte on the TX register */
void Serial_TX_SendByte();

/* Checks if data is available on the TX buffer */
unsigned char Serial_TX_isDataAvailable();

/* Resets the TX buffer */
void Serial_TX_Clear();

/******************************************************************************/
/* General Functions Part 2													  */
/******************************************************************************/

/* Clears both RC and TX buffers */
void Serial_ClearAll();

/* Interrupt service routing for EUSART communication */
void Serial_ISR();

#endif /* _SERIAL_H */