/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include "Serial.h"

/******************************************************************************/
/* DEFINITIONS  															  */
/******************************************************************************/
#define MAX_TX_SIZE     30
#define MAX_RC_SIZE     30

/******************************************************************************/
/* GLOBAL VARIABLES															  */
/******************************************************************************/
unsigned char TX_BUFFER[MAX_TX_SIZE], RC_BUFFER[MAX_RC_SIZE];
unsigned char TX_HEAD, TX_TAIL, RC_HEAD, RC_TAIL;

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
    unsigned char i;
    
    /* Set the bits for controlling the baud rate */
    Serial_BAUDRATE_HB = (0x8A << 8); // value needs to be 
    Serial_BAUDRATE_LB = (0x8A << 0);
    
	/* Set the bits for the TxSTA1 register */
	Serial_TX_ENABLE = 1;
	Serial_TX_MODE = 0;
	Serial_TX_HIGHRATE = 1;
	
	/* Set the bits for the RCSTA1 register */
	Serial_RC_SERIAL = 1;
	Serial_RC_SINGLE = 0;
	Serial_RC_CONT = 1;
	
	/* Set the bits for the BAUDCON register for the baud rate generator */
	Serial_BAUD_RCPOL = 0;
	Serial_BAUD_TXPOL = 0;
	Serial_BAUD_BITSIZE = 1;
	Serial_BAUD_AUTO = 0;
	
	/* Enable interrupts on the PIE1 register */
    Serial_INTEN_RC = 1;
    Serial_INTEN_TX = 1;
	
	/* Clear the interrupt flags on the PIR1 register */
    Serial_INT_RC = 0;
    Serial_INT_TX = 0;
    
    /* Initialize the RC and TX buffers */
    RC_HEAD = RC_TAIL = TX_HEAD = TX_TAIL = 0;
    for (i = 0; i < MAX_RC_SIZE; i = i + 1) { RC_BUFFER[i] = 0; }
    
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
void Serial_RC_StoreData(unsigned char data) {
	RC_BUFFER[RC_HEAD] = data;
	RC_HEAD = Serial_IncrementIndex(RC_HEAD, MAX_RC_SIZE);
	
	if (RC_HEAD == RC_TAIL) { RC_TAIL = Serial_IncrementIndex(RC_TAIL, MAX_RC_SIZE); }
}

/***************************************************************************//**
 * @brief Checks if there is received data available to be read.
 *
 * @param None.
 * 
 * @return 1 - data is available to be read, 0 - data is not available.
*******************************************************************************/
unsigned char Serial_RC_isDataAvailable() {
	return (RC_HEAD != RC_TAIL);
}

/***************************************************************************//**
 * @brief Resets the RC buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_RC_Clear() {
	RC_HEAD = RC_TAIL = 0;
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
	
	if (TX_HEAD == TX_TAIL) { TX_TAIL = Serial_IncrementIndex(TX_TAIL, MAX_RC_SIZE); }
	
	PIE1bits.TXIE = 1;
}

/***************************************************************************//**
 * @brief Checks if there is data available to be transmitted on the TX buffer.
 *
 * @param None.
 * 
 * @return 1 - data is available to be transmitted, 0 - data is not available..
*******************************************************************************/
unsigned char Serial_TX_isDataAvailable() {
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
 * @brief Retrieves a received value from the RC buffer
 *
 * @param None.
 * 
 * @return Oldest character in the RC buffer.
*******************************************************************************/
unsigned char Serial_RetrieveByte() {
	unsigned char data = RC_BUFFER[RC_TAIL];
	RC_TAIL = Serial_IncrementIndex(RC_TAIL, MAX_RC_SIZE);
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
	Serial_TX_REGISTER = TX_BUFFER[TX_TAIL];
	TX_TAIL = Serial_IncrementIndex(TX_TAIL, MAX_TX_SIZE);
}

/***************************************************************************//**
 * @brief Puts multiple bytes of data into the transmit buffer.
 *
 * @param None.
 * 
 * @return 1 - transfer of data successful, 0 - transfer of data unsuccessful.
*******************************************************************************/
unsigned char Serial_SendData(unsigned char* data) {
	unsigned char i = 0;
	
	while (data[i] != 0x00) {
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
void Serial_ClearAll() {
	Serial_TX_Clear();
	Serial_RC_Clear();
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