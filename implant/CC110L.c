/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include "CommCC110L.h"
#include "CC110L.h"

/******************************************************************************/
/* DEFINITIONS  															  */
/******************************************************************************/
#define MAX_TX_SIZE     32
#define MAX_RC_SIZE     32

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
unsigned char CC110L_Initialize() {
    unsigned char i;
    
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
unsigned char CC110L_IncrementIndex(unsigned char idx, unsigned char max) {
	if (++idx == max) { idx = 0; } // increment index; and if buffer end reached, start from beginning
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
void CC110L_RC_WriteBuffer(unsigned char data) {
	/* Write the data to the current head of the buffer */
	RC_BUFFER[RC_HEAD] = data;
	
	/* Increment the head of the buffer */
	RC_HEAD = CC110L_IncrementIndex(RC_HEAD, MAX_RC_SIZE);
	
	/* If you have reached the tail, increment the tail of the buffer */
	if (RC_HEAD == RC_TAIL) { RC_TAIL = CC110L_IncrementIndex(RC_TAIL, MAX_RC_SIZE); }
}

/***************************************************************************//**
 * @brief Retrieves a received value from the RC buffer
 *
 * @param None.
 * 
 * @return Oldest character in the RC buffer.
*******************************************************************************/
unsigned char CC110L_RC_ReadBuffer() {
	unsigned char data;
	
	/* Temporarily disable the interrupts */
	CC110L_INT_GLOBAL = 0;
	
	/* Read the data from the end of the buffer */
	data = RC_BUFFER[RC_TAIL];
	
	/* Increment the tail of the buffer */
	RC_TAIL = CC110L_IncrementIndex(RC_TAIL, MAX_RC_SIZE);
	
	/* Re-enable the interrupt */
	CC110L_INT_GLOBAL = 1;
	
	return data;
}

/***************************************************************************//**
 * @brief Reads a byte off the RC register and writes it to the RC buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void CC110L_RC_ReadByte() {
    unsigned char data;
    
    /* Read a byte of data from the SPI */
    CommCC110L_Read(&data, 1);
    
	/* Read the data from the RC register */
	CC110L_RC_WriteBuffer(data);
	
    /* REMOVE THIS LINE OF CODE. Testing used to send characters to the transmit buffer */
    CC110L_TX_WriteBuffer(CC110L_RC_ReadBuffer());
}

/***************************************************************************//**
 * @brief Checks if there is received data available to be read.
 *
 * @param None.
 * 
 * @return 1 - data is available to be read, 0 - data is not available.
*******************************************************************************/
unsigned char CC110L_RC_isDataAvailable() {
	return (RC_HEAD != RC_TAIL);
}

/***************************************************************************//**
 * @brief Resets the RC buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void CC110L_RC_Clear() {
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
void CC110L_TX_WriteBuffer(unsigned char data) {
	/* Temporarily disable interrupts */
	CC110L_INT_GLOBAL = 0;
	
	/* Write the data to the current head of the buffer */
	TX_BUFFER[TX_HEAD] = data;
	
	/* Increment the head of the buffer */
	TX_HEAD = CC110L_IncrementIndex(TX_HEAD, MAX_TX_SIZE);
	
	/* If you have reached the tail, increment the tail of the buffer */
	if (TX_HEAD == TX_TAIL) { TX_TAIL = CC110L_IncrementIndex(TX_TAIL, MAX_RC_SIZE); }
	
	/* Re-enable the interrupts */
	CC110L_INT_GLOBAL = 1;
}

/***************************************************************************//**
 * @brief Puts multiple bytes of data into the transmit buffer.
 *
 * @param None.
 * 
 * @return 1 - transfer of data successful, 0 - transfer of data unsuccessful.
*******************************************************************************/
void CC110L_TX_WriteBufferMultiple(unsigned char* data) {
	/* Iterate through the data */
	while (*data != 0x00) { // while there is still data in the vector
		
		/* Write the data to the buffer */
		CC110L_TX_WriteBuffer(*data);
		
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
void CC110L_TX_SendByte() {
	/* Temporarily disable interrupts */
	CC110L_INT_GLOBAL = 0;
	
	/* Write the data at the end of the transmit buffer to the transmit register */
	CommCC110L_Write(&TX_BUFFER[TX_TAIL], 1);
	
	/* Increment the tail of the buffer */
	TX_TAIL = CC110L_IncrementIndex(TX_TAIL, MAX_TX_SIZE);
	
	/* Re-enable interrupts */
	CC110L_INT_GLOBAL = 1;
}

/***************************************************************************//**
 * @brief Checks if there is data available to be transmitted on the TX buffer.
 *
 * @param None.
 * 
 * @return 1 - data is available to be transmitted, 0 - data is not available..
*******************************************************************************/
unsigned char CC110L_TX_isDataAvailable() {
	/* If data is available, enable the TX interrupts */
	if (TX_HEAD != TX_TAIL) { return 1; }
	
	/* If data is not available, disable the TX interrupt */
	else { return 0; }
}

/***************************************************************************//**
 * @brief Resets the TX buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void CC110L_TX_Clear() {
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
void CC110L_ClearAll() {
	CC110L_TX_Clear();
	CC110L_RC_Clear();
}

/***************************************************************************//**
 * @brief Interrupt service routing for EUSART communication.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void CC110L_ISR() {
    
}