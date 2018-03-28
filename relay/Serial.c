/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include "Serial.h"

/******************************************************************************/
/* CONSTANTS																  */
/******************************************************************************/
unsigned char MAX_TX_SIZE = 30, MAX_RX_SIZE = 30;
unsigned char TX_BUFFER[MAX_TX_SIZE], RX_BUFFER[MAX_RX_SIZE];
unsigned char TX_HEAD, TX_TAIL, RX_HEAD, RX_TAIL;

/******************************************************************************/
/* FUNCTIONS																  */
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the EUSART communication. 
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
unsigned char Serial_Initialize() {
	/* Set the bits for the TxSTA1 register */
	Serial_TX_ENABLE = 1;
	Serial_TX_Mode = 0;
	Serial_TX_HIGHRATE = 1;
	
	/* Set the bits for the RCSTA1 register */
	Serial_RX_SERIAL = 1;
	Serial_RX_SINGLE = 0;
	Serial_RX_CONT = 1;
	
	/* Set the bits for the BAUDCON register for the baud rate generator */
	Serial_BAUD_RXPOL = 0;
	Serial_BAUD_TXPOL = 0;
	Serial_BAUD_BITSIZE = 1;
	Serial_BAUD_AUTO = 0;
	
	/* Enable interrupts on the PIE1 register */
	SERIAL_INTEN_RX = 1;
	SERIAL_INTEN_TX = 1;
	
	/* Clear the interrupt flags on the PIR1 register */
	SERIAL_INT_RX = 0;
	SERIAL_INT_TX = 0;
	
	return 1;
}

/***************************************************************************//**
 * @brief Increment the specified index and check if the end of the buffer has
 *        been reached. If the end has been reached, the index is wrapped back 
 *        around to the beginning of the buffer.
 *
 * @param None.
 * 
 * @return Incremented index value.
*******************************************************************************/
unsigned char Serial_IncrementIndex(unsigned char idx, unsigned char max) {
	if (++idx == max) { idx = 0; } // increment index; and if buffer reached, start from beginning
	return idx;
}

/***************************************************************************//**
 * @brief Stores the new character that is received on the serial communication
 *        into the input buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_RX_StoreData(unsigned char data) {
	RX_BUFFER[RX_HEAD] = data;
	RX_HEAD = Serial_IncrementIndex(RX_HEAD, MAX_RX_SIZE);
	
	if (RX_HEAD == RX_TAIL) { RX_TAIL = Serial_IncrementIndex(RX_TAIL, MAX_RX_SIZE);
}

/***************************************************************************//**
 * @brief Checks if there is received data available to be read.
 *
 * @param None.
 * 
 * @return 1 - data is available to be read, 0 - data is not available.
*******************************************************************************/
bool Serial_RX_isDataAvailable() {
	return (RX_HEAD != RX_TAIL);
}

/***************************************************************************//**
 * @brief Resets the RX buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_RX_Clear() {
	RX_HEAD = RX_TAIL = 0;
}

/***************************************************************************//**
 * @brief Stores a character that needs to be transmitted on the TX buffer.
 *
 * @param Byte of data to write to the transmit buffer.
 * 
 * @return None.
*******************************************************************************/
void Serial_TX_StoreData(unsigned char data) {
	TX_BUFFER[TX_HEAD] = data;
	TX_HEAD = Serial_IncrementIndex(TX_HEAD, MAX_TX_SIZE);
	
	if (TX_HEAD == TX_TAIL) { TX_TAIL = Serial_IncrementIndex(TX_TAIL, MAX_RX_SIZE); }
	
	PIE1bits.TXIE = 1;
}

/***************************************************************************//**
 * @brief Checks if there is data available to be transmitted on the TX buffer.
 *
 * @param None.
 * 
 * @return 1 - data is available to be transmitted, 0 - data is not available..
*******************************************************************************/
bool Serial_TX_isDataAvailable() {
	if (TX_HEAD != TX_TAIL) { PIE1bits.TXIE = 1; }
	else { PIE1bits.TXIE = 0; }
	
	return PIE1bits.TXIE;
}

/***************************************************************************//**
 * @brief Resets the TX buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_TX_Clear() {
	TX_HEAD = TX_TAIL = 0;
}

/***************************************************************************//**
 * @brief Retrieves a received value from the RX buffer
 *
 * @param None.
 * 
 * @return Oldest character in the RX buffer.
*******************************************************************************/
unsigned char Serial_RetrieveByte() {
	unsigned char data = RX_BUFFER[RX_TAIL];
	RX_TAIL = Serial_IncrementIndex(RX_TAIL, MAX_RX_SIZE);
	return data;
}

/***************************************************************************//**
 * @brief Outputs a signal byte to the Serial communication line.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_SendByte() {
	TXREG = TX_BUFFER[TX_TAIL];
	TX_TAIL = Serial_IncrementIndex(TX_TAIL, MAX_TX_SIZE);
}

/***************************************************************************//**
 * @brief Puts multiple bytes of data into the transmit buffer.
 *
 * @param None.
 * 
 * @return 1 - transfer of data successful, 0 - transfer of data unsuccessful.
*******************************************************************************/
unsigned char Serial_SendData(unsigned char data[32]) {
	unsigned char i = 0;
	
	while (data[i] != NULL) {
		Serial_TX_StoreData(data[i]);
		i = i + 1;
	}
	
	return 1;
}

/***************************************************************************//**
 * @brief Clears both the receive and transmit buffers.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_ClearAll() [
	Serial_TX_Clear();
	Serial_RX_Clear();
}


/***************************************************************************//**
 * @brief Interrupt service routing for EUSART communication.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_ISR() {
	
}