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
#define Serial_TX_ENABLE		TXSTA1bits.TXEN // Transmit Enable bit
#define Serial_TX_MODE			TXSTA1bits.SYNC // EUSART Mode Select bit
#define Serial_TX_HIGHRATE		TXSTA1bits.BRGH // High Baud Rate Select bit
#define Serial_TX_STATUS		TXSTA1bits.TRMT // Transmit Shift Register Status bit

/* Define the EUSART bits on the RCSTA1 (Receive Status and Control) Register */
#define Serial_RX_SERIAL		RXSTA1bits.SPEN // Serial Port Enable bit
#define Serial_RX_SINGLE		RXSTA1bits.SREN // Single Receive Enable bit
#define Serial_RX_CONT			RXSTA1bits.CREN // Continuous Receive Enable bit

/* Define the EUSART bits on the BAUDCON1 (Baud Rate Control) Register */
#define Serial_BAUD_RXPOL		BAUDCON1bits.DTRXP // Data/Receive Polarity Select bit
#define Serial_BAUD_TXPOL		BAUDCON1bits.CKTXP // Clock/Transmit Polarity Select bit
#define Serial_BAUD_BITSIZE		BAUDCON1bits.BRG16 // 16-bit Baud Rate Generator bit
#define	Serial_BAUD_AUTO		BAUDCON1bits.ABDEN // Auto-Baud Detect Enable bit

/* Define the EUSART bits on the PIR (Peripheral Interrupt Request) Register */
#define Serial_INT_RX           PIR1bits.RC1IF; // EUSART1 Receive Interrupt Flag bit
#define Serial_INT_TX           PIR1bits.TX1IF; // EUSART1 Transmit Interrupt Flag bit

/* Define the EUSART bits on the PIE (Peripheral Interrupt Enable) Register */
#define Serial_INTEN_RX         PIE1bits.RC1IE; // EUSART1 Receive Interrupt Enable bit
#define Serial_INTEN_TX         PIE1bits.TX1IE; // EUSART1 Transmit Interrupt Enable bit

/******************************************************************************/
/* FUNCTION PROTOTYPES														  */
/******************************************************************************/

/* Initialize the EUSART interface */
unsigned char Serial_Initialize();

unsigned char Serial_IncrementIndex(unsigned char idx, unsigned char max);

void Serial_RX_StoreData(unsigned char data);

unsigned char Serial_RX_isDataAvailable();

void Serial_RX_Clear();

void Serial_TX_StoreData(unsigned char data);

unsigned char Serial_TX_isDataAvailable();

void Serial_TX_Clear();

unsigned char Serial_RetrieveByte();

void Serial_SendByte();

void Serial_SendData(unsigned char data[32]);

void Serial_ClearAll();

/* Interrupt service routing for EUSART communication */
void Serial_ISR();

#endif /* _SERIAL_H */