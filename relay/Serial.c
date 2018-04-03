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

/******************************************************************************/
/* General Functions Part 1													  */
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
    
    /* Set the pins for the RC pin */
    Serial_RC_DIR = 1;
    Serial_RC_ANSEL = 0;
    
    /* Set the pins for the TX pin */
    Serial_TX_DIR = 1;
    Serial_TX_ANSEL = 0;
    
    /* Set the bits for controlling the baud rate */
    Serial_BAUDRATE_HB = (416ul >> 8); // value needs to be 34
    Serial_BAUDRATE_LB = (416ul >> 0);
    
	/* Set the bits for the TxSTA1 register */
	Serial_TX_ENABLE   = 1; // Transmit enabled
	Serial_TX_MODE     = 0; // Asynchronous mode
	Serial_TX_HIGHRATE = 1; // High speed
	
	/* Set the bits for the RCSTA1 register */
	Serial_RC_SERIAL = 1; // Serial port enabled (configures RXx and TXx pins as serial port pins)
	Serial_RC_CONT   = 1; // Enables receiver
	
	/* Set the bits for the BAUDCON register for the baud rate generator */
	Serial_BAUD_RCPOL   = 0; // Receive data (RXx) is not inverted (active-high)
	Serial_BAUD_TXPOL   = 0; // Idle state for transmit (TXx) is low
	Serial_BAUD_BITSIZE = 1; // 16-bit Baud Rate Generator is used
	Serial_BAUD_AUTO    = 0; // Auto-Baud Detect mode is disabled
	
    /* Activate the interrupts */
    Serial_INT_PRIORITY   = 1; // Enable priority levels on interrupts
    Serial_INT_GLOBAL     = 1; // Enable all high priority interrupts
    Serial_INT_PERIPHERAL = 1; // Enable all low priority interrupts
    
	/* Clear the interrupt flags on the PIR1 register */
    Serial_RC_FULL  = 0; // EUSART1 Receive Interrupt Flag bit
    Serial_TX_EMPTY = 1; // EUSART1 Transmit Interrupt Flag bit
    
    /* Set the interrupt priority on the IPR1 register */
    Serial_INTPRIORITY_RC = 0; // High priority
    Serial_INTPRIORITY_TX = 1; // High priority
    
    /* Initialize the RC and TX buffers */
    RC_HEAD = RC_TAIL = TX_HEAD = TX_TAIL = 0;
    for (i = 0; i < MAX_RC_SIZE; i = i + 1) { RC_BUFFER[i] = 0; }
    
    /* Enable interrupts on the PIE1 register */
    Serial_INTEN_RC = 0; // Enables the EUSART1 receive interrupt
    Serial_INTEN_TX = 1; // Enables the EUSART1 transmit interrupt
    
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
	return idx; // return the index
}

/******************************************************************************/
/* Receive Functions														  */
/******************************************************************************/

/***************************************************************************//**
 * @brief Stores the new character that is received on the serial communication
 *        into the input buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_RC_WriteBuffer(unsigned char data) {
	/* Write the data to the current head of the buffer */
	RC_BUFFER[RC_HEAD] = data;
	
	/* Increment the head of the buffer */
	RC_HEAD = Serial_IncrementIndex(RC_HEAD, MAX_RC_SIZE);
	
	/* If you have reached the tail, increment the tail of the buffer */
	if (RC_HEAD == RC_TAIL) { RC_TAIL = Serial_IncrementIndex(RC_TAIL, MAX_RC_SIZE); }
}

/***************************************************************************//**
 * @brief Retrieves a received value from the RC buffer
 *
 * @param None.
 * 
 * @return Oldest character in the RC buffer.
*******************************************************************************/
unsigned char Serial_RC_ReadBuffer() {
	unsigned char data;
	
	/* Temporarily disable the interrupts */
	Serial_INT_GLOBAL = 0;
	
	/* Read the data from the end of the buffer */
	data = RC_BUFFER[RC_TAIL];
	
	/* Increment the tail of the buffer */
	RC_TAIL = Serial_IncrementIndex(RC_TAIL, MAX_RC_SIZE);
	
	/* Re-enable the interrupt */
	Serial_INT_GLOBAL = 1;
	
	return data;
}

/***************************************************************************//**
 * @brief Reads a byte off the RC register.
 *
 * @param None.
 * 
 * @return Oldest character in the RC buffer.
*******************************************************************************/
void Serial_RC_ReadByte() {
	/* Read the data from the RC register */
	Serial_RC_WriteBuffer(Serial_RC_REGISTER);
	
	/* Clear the interrupt flag */
	Serial_RC_FULL = 0;
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
	RC_HEAD = RC_TAIL = 0; // reset the head and the tail to the beginning of the buffer
}

/******************************************************************************/
/* Transmit Functions														  */
/******************************************************************************/

/***************************************************************************//**
 * @brief Stores a character that needs to be transmitted on the TX buffer.
 *
 * @param Byte of data to write to the transmit buffer.
 * 
 * @return None.
*******************************************************************************/
void Serial_TX_WriteBuffer(unsigned char data) {
	/* Temporarily disable interrupts */
	Serial_INT_GLOBAL = 0;
	
	/* Write the data to the current head of the buffer */
	TX_BUFFER[TX_HEAD] = data;
	
	/* Increment the head of the buffer */
	TX_HEAD = Serial_IncrementIndex(TX_HEAD, MAX_TX_SIZE);
	
	/* If you have reached the tail, increment the tail of the buffer */
	if (TX_HEAD == TX_TAIL) { TX_TAIL = Serial_IncrementIndex(TX_TAIL, MAX_RC_SIZE); }
	
	/* Re-enable the interrupts */
	Serial_INTEN_TX = 1;
	Serial_INT_GLOBAL = 1;
}

/***************************************************************************//**
 * @brief Puts multiple bytes of data into the transmit buffer.
 *
 * @param None.
 * 
 * @return 1 - transfer of data successful, 0 - transfer of data unsuccessful.
*******************************************************************************/
void Serial_TX_WriteBufferMultiple(unsigned char* data) {
	/* Iterate through the data */
	while (*data != 0x00) { // while there is still data in the vector
		
		/* Write the data to the buffer */
		Serial_TX_WriteBuffer(*data);
		
		/* Increment the data address */
		data = data + 1;
	}
}

/***************************************************************************//**
 * @brief Outputs a signal byte to the Serial communication line.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_TX_SendByte() {
	/* Temporarily disable interrupts */
	Serial_INT_GLOBAL = 0;
	
	/* Write the data at the end of the transmit buffer to the transmit register */
	Serial_TX_REGISTER = TX_BUFFER[TX_TAIL];
	
	/* Increment the tail of the buffer */
	TX_TAIL = Serial_IncrementIndex(TX_TAIL, MAX_TX_SIZE);
	
	/* Re-enable interrupts */
	Serial_INT_GLOBAL = 1;
}

/***************************************************************************//**
 * @brief Checks if there is data available to be transmitted on the TX buffer.
 *
 * @param None.
 * 
 * @return 1 - data is available to be transmitted, 0 - data is not available..
*******************************************************************************/
unsigned char Serial_TX_isDataAvailable() {
	/* If you are not at the end of the buffer, enable the TX interrupts */
	if (TX_HEAD != TX_TAIL) { Serial_INTEN_TX = 1; }
	
	/* If you are at the end of the buffer, disable the TX interrupt */
	else { Serial_INTEN_TX = 0; }
	
	return Serial_INTEN_TX;
}

/***************************************************************************//**
 * @brief Resets the TX buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_TX_Clear() {
	TX_HEAD = TX_TAIL = 0; // reset the head and the tail to the beginning of the buffer
}

/******************************************************************************/
/* General Functions Part 2													  */
/******************************************************************************/

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
	unsigned char isData;
	
	if (Serial_RC_FULL && Serial_INTEN_RC) {
		Serial_RC_ReadByte();
		if (Serial_RC_FULL) { Serial_RC_ReadByte(); }
	}
	
	if (Serial_TX_EMPTY && Serial_INTEN_TX) {
		isData = Serial_TX_isDataAvailable();
		if (isData) {
			Serial_TX_SendByte();
			if (Serial_TX_EMPTY) { Serial_TX_SendByte(); }
		} else {
			Serial_INTEN_TX = 0;
		}
	}
}