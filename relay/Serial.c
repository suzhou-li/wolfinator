/******************************************************************************/
/* INCLUDE FILES															  */
/******************************************************************************/
#include "Serial.h"

/******************************************************************************/
/* DEFINITIONS  															  */
/******************************************************************************/
#define Serial_MAX_TX_SIZE     32
#define Serial_MAX_RC_SIZE     32

/******************************************************************************/
/* GLOBAL VARIABLES															  */
/******************************************************************************/
unsigned char Serial_TX_BUFFER[Serial_MAX_TX_SIZE], Serial_RC_BUFFER[Serial_MAX_RC_SIZE];
unsigned char Serial_TX_HEAD, Serial_TX_TAIL, Serial_RC_HEAD, Serial_RC_TAIL;

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
    unsigned long baud_rate = 34; //416;
    unsigned char i;
    
    /* Set the pins for the RC pin */
    Serial_RC_DIR = 1;
    Serial_RC_ANSEL = 0;
    
    /* Set the pins for the TX pin */
    Serial_TX_DIR = 1;
    Serial_TX_ANSEL = 0;
    
    /* Set the bits for controlling the baud rate */
    Serial_BAUDRATE_HB = (baud_rate >> 8); // value needs to be 34
    Serial_BAUDRATE_LB = (baud_rate >> 0);
    
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
    INTERRUPT_PRIORITY   = 1; // Enable priority levels on interrupts
    INTERRUPT_GLOBAL     = 0; // Enable all high priority interrupts
    INTERRUPT_PERIPHERAL = 0; // Enable all low priority interrupts
    
    /* Enable interrupts on the PIE1 register */
    Serial_RCINT_ENABLE = 1; // Enables the EUSART1 receive interrupt
    Serial_TXINT_ENABLE = 1; // Enables the EUSART1 transmit interrupt
    
	/* Clear the interrupt flags on the PIR1 register */
    Serial_RC_FULL  = 0; // EUSART1 Receive Interrupt Flag bit
    Serial_TX_EMPTY = 0; // EUSART1 Transmit Interrupt Flag bit
    
    /* Set the interrupt priority on the IPR1 register */
    Serial_RCINT_PRIORITY = 1; // High priority
    Serial_TXINT_PRIORITY = 1; // High priority
    
    /* Initialize the RC and TX buffers */
    Serial_RC_HEAD = Serial_RC_TAIL = Serial_TX_HEAD = Serial_TX_TAIL = 0;
    for (i = 0; i < Serial_MAX_RC_SIZE; i = i + 1) { Serial_RC_BUFFER[i] = 0; }
    
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
void Serial_RC_WriteBuffer(unsigned char data) {
	/* Write the data to the current head of the buffer */
	Serial_RC_BUFFER[Serial_RC_HEAD] = data;
	
	/* Increment the head of the buffer */
	Serial_RC_HEAD = Serial_IncrementIndex(Serial_RC_HEAD, Serial_MAX_RC_SIZE);
	
	/* If you have reached the tail, increment the tail of the buffer */
	if (Serial_RC_HEAD == Serial_RC_TAIL) { Serial_RC_TAIL = Serial_IncrementIndex(Serial_RC_TAIL, Serial_MAX_RC_SIZE); }
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
	INTERRUPT_GLOBAL = 0;
	
	/* Read the data from the end of the buffer */
	data = Serial_RC_BUFFER[Serial_RC_TAIL];
	
	/* Increment the tail of the buffer */
	Serial_RC_TAIL = Serial_IncrementIndex(Serial_RC_TAIL, Serial_MAX_RC_SIZE);
	
	/* Re-enable the interrupt */
	INTERRUPT_GLOBAL = 1;
	
	return data;
}

/***************************************************************************//**
 * @brief Reads a byte off the RC register and writes it to the RC buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_RC_ReadByte() {
	/* Read the data from the RC register */
	Serial_RC_WriteBuffer(Serial_RC_REGISTER);
	
    /* REMOVE THIS LINE OF CODE. Testing used to send characters to the transmit buffer */
    Serial_TX_WriteBuffer(Serial_RC_ReadBuffer());
}

/***************************************************************************//**
 * @brief Checks if there is received data available to be read.
 *
 * @param None.
 * 
 * @return 1 - data is available to be read, 0 - data is not available.
*******************************************************************************/
unsigned char Serial_RC_isDataAvailable() {
	return (Serial_RC_HEAD != Serial_RC_TAIL);
}

/***************************************************************************//**
 * @brief Resets the RC buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_RC_Clear() {
	Serial_RC_HEAD = Serial_RC_TAIL = 0; // reset the head and the tail to the beginning of the buffer
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
	INTERRUPT_GLOBAL = 0;
	
	/* Write the data to the current head of the buffer */
	Serial_TX_BUFFER[Serial_TX_HEAD] = data;
	
	/* Increment the head of the buffer */
	Serial_TX_HEAD = Serial_IncrementIndex(Serial_TX_HEAD, Serial_MAX_TX_SIZE);
	
	/* If you have reached the tail, increment the tail of the buffer */
	if (Serial_TX_HEAD == Serial_TX_TAIL) { Serial_TX_TAIL = Serial_IncrementIndex(Serial_TX_TAIL, Serial_MAX_RC_SIZE); }
	
	/* Re-enable the interrupts */
	Serial_TXINT_ENABLE = 1;
	INTERRUPT_GLOBAL = 1;
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
	INTERRUPT_GLOBAL = 0;
	
	/* Write the data at the end of the transmit buffer to the transmit register */
	Serial_TX_REGISTER = Serial_TX_BUFFER[Serial_TX_TAIL];
	
	/* Increment the tail of the buffer */
	Serial_TX_TAIL = Serial_IncrementIndex(Serial_TX_TAIL, Serial_MAX_TX_SIZE);
	
	/* Re-enable interrupts */
	INTERRUPT_GLOBAL = 1;
}

/***************************************************************************//**
 * @brief Checks if there is data available to be transmitted on the TX buffer.
 *
 * @param None.
 * 
 * @return 1 - data is available to be transmitted, 0 - data is not available..
*******************************************************************************/
unsigned char Serial_TX_isDataAvailable() {
	/* If data is available, enable the TX interrupts */
	if (Serial_TX_HEAD != Serial_TX_TAIL) { Serial_TXINT_ENABLE = 1; }
	
	/* If data is not available, disable the TX interrupt */
	else { Serial_TXINT_ENABLE = 0; }
	
	return Serial_TXINT_ENABLE;
}

/***************************************************************************//**
 * @brief Resets the TX buffer.
 *
 * @param None.
 * 
 * @return None.
*******************************************************************************/
void Serial_TX_Clear() {
	Serial_TX_HEAD = Serial_TX_TAIL = 0; // reset the head and the tail to the beginning of the buffer
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
    /* If there is data in the RC register and the RC interrupts are enabled */
	if (Serial_RC_FULL && Serial_RCINT_ENABLE) {
        /* Read the byte on the RC register and write it to the RC buffer */
		Serial_RC_ReadByte();
		if (Serial_RC_FULL) { Serial_RC_ReadByte(); }
	}
	
    /* If the TX register is available and the TX interrupts are enabled */
	if (Serial_TX_EMPTY && Serial_TXINT_ENABLE) {
        /* If data is available to be transmitted */
		if (Serial_TX_isDataAvailable()) {
            /* Write data from the TX buffer to the TX register*/
			Serial_TX_SendByte();
			if (Serial_TX_EMPTY) { Serial_TX_SendByte(); }
        }
	}
    
    /* Clear the interrupt flags */
    Serial_RC_FULL  = 0;
    Serial_TX_EMPTY = 0;
}

void Serial_ISR_SimpleResponse() {
    unsigned char data;
    
    /* If the RC register is full and we have RC interrupts enabled */
    if (Serial_RC_FULL && Serial_RCINT_ENABLE) {
        
        /* Read data from the RC register */
        data = Serial_RC_REGISTER;
        
        /* If the TX register is empty and we have TX interrupts enabled, send out the received data */
        if (Serial_TX_EMPTY && Serial_TXINT_ENABLE) { Serial_TX_REGISTER = data; }
    }
    
    /* Clear the interrupt flags */
    Serial_RC_FULL  = 0;
    Serial_TX_EMPTY = 0;
}