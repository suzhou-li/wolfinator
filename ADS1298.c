/***************************************************************************//**
 *   @file   ADS1298.c
 *   @brief  Implementation of ADS1298 Driver.
 *   @author Suzhou Li (suzhou.li@duke.edu)
*******************************************************************************/

/*****************************************************************************/
/* INCLUDE FILES															 */
/*****************************************************************************/
#include "ADS1298.h"			
#include "Communication.h"	

/*****************************************************************************/
/* CONSTANTS    															 */
/*****************************************************************************/
static unsigned long frameSize;
static unsigned long numDevices;

/*****************************************************************************/
/* FUNCTIONS																 */
/*****************************************************************************/

void ADS1298_WriteSingleOpCode(unsigned char writeVal) {
	SPI_ADS1298_Write(&writeVal, 1);
}

/***************************************************************************//**
 * @brief Goes through the power-up sequencing of the device. Before device
 *        power up, all digital and analog inputs must be low. At the time of 
 *        power up, keep all these signals low until the power supplies have
 *        stabilized. Allow time for the supply voltages to reach their final
 *        to reach their final value, and then begin supplying the master 
 *        clock signal to the CLK pin. Wait for time t_POR (wait after power 
 *        up until reset, 2^18 clock cycles), then transmit a reset pulse. 
 *        Issue the reset after t_POR or after VCAP1 voltage is greater than 
 *        1.1 V depends on RC time constant). 
 * 
 * @param None.
 * 
 * @return 1 - power-up success, 0 - power-up failed
*******************************************************************************/
unsigned char ADS1298_PowerUp() {
	unsigned int i = 0;
    
	/* Bring the PWR and RESET pin HIGH to turn on the device */
	ADS1298_PWR_PIN = 1;
	
	/* Wait appropriate amount of time for the device to power up */
	for (i = 0; i < 50000; i++) {} 
	
    /* Reset the device by toggling the RESET pin */
    ADS1298_RESET_PIN = 0;
    for (i = 0; i < 500; i++) {} // wait at least 18 shift clock cycles
    ADS1298_RESET_PIN = 1;
    for (i = 0; i < 500; i++) {} // wait at least 18 shift clock cycles
    
	/* Reset the device by issuing the RESET opcode */
    SPI_ADS1298_CS_PIN = 0;
    ADS1298_WriteSingleOpCode(ADS1298_RESET);
    SPI_ADS1298_CS_PIN = 1;
    for (i = 0; i < 500; i++) {} // wait at least 18 shift clock cycles
    
    /* Stop the read data continuously mode (SDATAC) */
    SPI_ADS1298_CS_PIN = 0;
    ADS1298_WriteSingleOpCode(ADS1298_SDATAC);
    SPI_ADS1298_CS_PIN = 1;
    for (i = 0; i < 50; i++) {} // wait at least 4 shift clock cycles
    
    /* Stop the data conversion (STOP) */
    SPI_ADS1298_CS_PIN = 0;
    ADS1298_WriteSingleOpCode(ADS1298_STOP);
    SPI_ADS1298_CS_PIN = 1;
    for (i = 0; i < 50; i++) {} // wait at least 4 shift clock cycles
    
	return 1;
}

/***************************************************************************//**
 * @brief	Writes data to the registers of the ADS1298. 
 * 
 * @param 	address - Char denoting the initial address to write to.
 * @param 	writeNum - Char denoting the number of registers to write.
 * @param 	regVals - Pointer to the array containing the values to write.
 * 
 * @return 	None.
*******************************************************************************/
void ADS1298_WriteRegisters(unsigned char address, 
							unsigned char writeNum, 
							unsigned char* regVals) {
	unsigned char writeOpCode[2] = {0, 0};
	
	/* Define the opcode */
	writeOpCode[0] = ADS1298_WREG + address;
	writeOpCode[1] = writeNum - 1;
	
	/* Write the opcode and register values */
    SPI_ADS1298_CS_PIN = 0;
	SPI_ADS1298_Write(writeOpCode, 2);
	SPI_ADS1298_Write(regVals, writeNum);
    SPI_ADS1298_CS_PIN = 1;
}

/***************************************************************************//**
 * @brief	Reads data from the registers of the ADS1298. 
 * 
 * @param	address - Char denoting the initial address to read from.
 * @param	writeNum - Char denoting the number of registers to read.
 * @param	regVals - Pointer to the array storing the read register data.
 * 
 * @return	None.
*******************************************************************************/
void ADS1298_ReadRegisters(unsigned char address,
						   unsigned char readNum, 
						   unsigned char* regVals) {
	unsigned char readOpCode[2] = {0, 0};
	
	/* Define the opcode */
	readOpCode[0] = (unsigned char) ADS1298_RREG + address;
	readOpCode[1] = readNum - 1;
	
	/* Write the opcode and read the register values */
    SPI_ADS1298_CS_PIN = 0;
	SPI_ADS1298_Write(readOpCode, 2);
	SPI_ADS1298_Read(regVals, readNum);
    SPI_ADS1298_CS_PIN = 1;
}

/***************************************************************************//**
 * @brief	Computes the size of a single frame of data in bits. Every frame 
 *          has a 24 bit status word. If there are multiple devices, there will 
 *          be a single bit separating the frames between each device.
 * 
 * @param	numDevices - number of devices installed
 * 
 * @return	Size of the frame in bits.
*******************************************************************************/
unsigned char ADS1298_ComputeFrameSize(unsigned char numDevices) {
    unsigned char numChannels = 0;
    unsigned char i;
    unsigned char chRegVals[8];
    
    /* Read the register values containing the channel settings */
    ADS1298_ReadRegisters(ADS1298_CH1SET, 8, chRegVals);
    
    /* Iterate through the channel values to see which ones are powered down */
    i = 0;
    for (i = 0; i < 8; i = i + 1) {
        if ((chRegVals[i] & 0b10000000) == 0x00) {
            numChannels = numChannels + 1; // if not powered down, increment
        }
    }
    
    /* Calculate and return the frame size */
    return (numDevices * ((numChannels * 8) + 24)) + (numDevices - 1);
}

/***************************************************************************//**
 * @brief	Streams electrogram data from the ADS1298. You could use the START
 *          pin, but using the START opcode means less wires are needed.
 * 
 * @param	pDataBuffer - Pointer to the array storing the streamed data.
 * @param	frameCnt - Long denoting the number of frames to collect.
 * 
 * @return	None.
*******************************************************************************/
void ADS1298_ReadData(unsigned char* pDataBuffer, 
					  unsigned long frameCnt) {
	unsigned char i, j;
    
	/* Issue the start opcode */
    SPI_ADS1298_CS_PIN = 0;
	if (frameCnt > 1) { // if you want to collect more than 1 frame
		ADS1298_WriteSingleOpCode(ADS1298_RDATAC);
	} else { // if you only want to collect a single frame
        ADS1298_WriteSingleOpCode(ADS1298_RDATA);
        
		/* Wait for the DRDY_NOT line to go low */
		while (!SPI_ADS1298_DRDY_NOT);
		
        /* Read all frames except for the last one */
		for(j = 0; j < numDevices - 1; j = j + 1) {
            /* Read the data in the frame */
            SPI_ADS1298_Read(pDataBuffer, (frameSize - numDevices + 1) / numDevices / 8);
            SPI_ADS1298_Wait(1); // issue 1 SCLK cycle
        }
        
        /* Read the last frame */
        SPI_ADS1298_Read(pDataBuffer, (frameSize - numDevices + 1) / numDevices / 8);
        
        /* Exit from the function */
        SPI_ADS1298_CS_PIN = 1;
		return;
	}
    
	/* Collect the specified number of frames */
    i = 0;
	for (i = 0; i < frameCnt; i = i + 1) {
		/* Wait for the DRDY_NOT line to go low */
		while (!SPI_ADS1298_DRDY_NOT);
		
        /* Read all frames except for the last one */
		for(j = 0; j < numDevices - 1; j = j + 1) {
            /* Read the data in the frame */
            SPI_ADS1298_Read(pDataBuffer, (frameSize - numDevices + 1) / numDevices / 8);
            SPI_ADS1298_Wait(1); // issue 1 SCLK cycle
        }
        
        /* Read the last frame */
        SPI_ADS1298_Read(pDataBuffer, (frameSize - numDevices + 1) / numDevices / 8);
	}
	
	/* Issue the stop opcode */
	SPI_ADS1298_Write(ADS1298_SDATAC, 1);
    SPI_ADS1298_CS_PIN = 1;
}

/***************************************************************************//**
 * @brief Initialize the ADS1298 registers. 
 * 
 * @param None.
 * 
 * @return 1 - initialization success, 0 - initialization failed
*******************************************************************************/
unsigned char ADS1298_Initialize(unsigned long devices) {
	unsigned char status = 0;
	unsigned char writeVals[25] = {0, 0, 0, 0, 0, \
                                   0, 0, 0, 0, 0, \
                                   0, 0, 0, 0, 0, \
								   0, 0, 0, 0, 0, \
								   0, 0, 0, 0, 0};
	
	/* Initialize the device */
	status = SPI_ADS1298_Init();
	if (!status) { return 0; } // if initialization was unsuccessful, return 0
	
	/* Power up the device */
	status = ADS1298_PowerUp(); // if initialization was successful, power up the device
	if (!status) { return 0; } // if the power up was unsuccessful, return 0
	
	/* Define the register values to write*/
	/* CONFIG1    */ writeVals[0]  = ADS1298_CONFIG1_HR | ADS1298_CONFIG1_DR_2K; // 0x84
	/* CONFIG2    */ writeVals[1]  = ADS1298_CONFIG2_WCTCHOPCONST | ADS1298_CONFIG2_INTTEST | ADS1298_CONFIG2_TESTAMP | ADS1298_CONFIG2_TESTFREQ_AC20;
	/* CONFIG3    */ writeVals[2]  = ADS1298_CONFIG3_INTREFEN | (0b1u << 6);
	/* LOFF       */ writeVals[3]  = 0x00;
	/* CH1SET     */ writeVals[4]  = ADS1298_CHSET_GAIN_12 | ADS1298_CHSET_MUX_TEST; //ADS1298_MUX_ELEC;
	/* CH2SET     */ writeVals[5]  = ADS1298_CHSET_PD | ADS1298_CHSET_MUX_SHORT; //ADS1298_CHSET_GAIN_12 | ADS1298_CHSET_MUX_TEST; //ADS1298_MUX_ELEC;
	/* CH3SET     */ writeVals[6]  = ADS1298_CHSET_PD | ADS1298_CHSET_MUX_SHORT; //ADS1298_CHSET_GAIN_12 | ADS1298_CHSET_MUX_TEST; //ADS1298_MUX_ELEC;
	/* CH4SET     */ writeVals[7]  = ADS1298_CHSET_PD | ADS1298_CHSET_MUX_SHORT; //ADS1298_CHSET_GAIN_12 | ADS1298_CHSET_MUX_TEST; //ADS1298_MUX_ELEC;
	/* CH5SET     */ writeVals[8]  = ADS1298_CHSET_PD | ADS1298_CHSET_MUX_SHORT; //ADS1298_CHSET_GAIN_12 | ADS1298_CHSET_MUX_TEST; //ADS1298_MUX_ELEC;
	/* CH6SET     */ writeVals[9]  = ADS1298_CHSET_PD | ADS1298_CHSET_MUX_SHORT; //ADS1298_CHSET_GAIN_12 | ADS1298_CHSET_MUX_TEST; //ADS1298_MUX_ELEC;
	/* CH7SET     */ writeVals[10] = ADS1298_CHSET_PD | ADS1298_CHSET_MUX_SHORT; //ADS1298_CHSET_GAIN_12 | ADS1298_CHSET_MUX_TEST; //ADS1298_MUX_ELEC;
	/* CH8SET     */ writeVals[11] = ADS1298_CHSET_PD | ADS1298_CHSET_MUX_SHORT; //ADS1298_CHSET_GAIN_12 | ADS1298_CHSET_MUX_TEST; //ADS1298_MUX_ELEC;
	/* RLD_SENSP  */ writeVals[12] = 0x00;
	/* RLD_SENSN  */ writeVals[13] = 0x00;
	/* LOFF_SENSP */ writeVals[14] = 0x00;
	/* LOFF_SENSN */ writeVals[15] = 0x00;
	/* LOFF_FLIP  */ writeVals[16] = 0x00;
	/* LOFF_STATP */ writeVals[17] = 0x00;
	/* LOFF_STATN */ writeVals[18] = 0x00;
	/* GPIO       */ writeVals[19] = 0x00;
	/* PACE       */ writeVals[20] = 0x00;
	/* RESP       */ writeVals[21] = 0x00;
	/* CONFIG4    */ writeVals[22] = 0x00;
	/* WCT1       */ writeVals[23] = 0x00;
	/* WCT2       */ writeVals[24] = 0x00;
	
	/* Send the register values */
	ADS1298_WriteRegisters(ADS1298_CONFIG1, 12, writeVals);
                    
    /* Compute the frame size */
    numDevices = devices;
    frameSize = ADS1298_ComputeFrameSize(numDevices);
    
	return 1;
}