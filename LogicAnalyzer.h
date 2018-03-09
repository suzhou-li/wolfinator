/* 
 * File:   LogicAnalyzer.h
 * Author: BME464
 *
 * Created on November 29, 2017, 11:15 AM
 */

#ifndef LOGICANALYZER_H
#define	LOGICANALYZER_H

#include <p18f46k22.h>

#define LogicAnalyzer_BIT7          LATDbits.LATD7
#define LogicAnalyzer_BIT7_DIR      TRISDbits.RD7

#define LogicAnalyzer_BIT6          LATDbits.LATD6
#define LogicAnalyzer_BIT6_DIR      TRISDbits.RD6

#define LogicAnalyzer_BIT5          LATDbits.LATD5
#define LogicAnalyzer_BIT5_DIR      TRISDbits.RD5

#define LogicAnalyzer_BIT4          LATDbits.LATD4
#define LogicAnalyzer_BIT4_DIR      TRISDbits.RD4

#define LogicAnalyzer_BIT3          LATDbits.LATD3
#define LogicAnalyzer_BIT3_DIR      TRISDbits.RD3

#define LogicAnalyzer_BIT2          LATDbits.LATD2
#define LogicAnalyzer_BIT2_DIR      TRISDbits.RD2

#define LogicAnalyzer_BIT1          LATDbits.LATD1
#define LogicAnalyzer_BIT1_DIR      TRISDbits.RD1

#define LogicAnalyzer_BIT0          LATDbits.LATD0
#define LogicAnalyzer_BIT0_DIR      TRISDbits.RD0

#define LogicAnalyzer_CLK           LATEbits.LATE0
#define LogicAnalyzer_CLK_DIR       TRISEbits.RE0

void LogicAnalyzer_init();
void LogicAnalyzer_outChar(unsigned char data);

#endif	/* LOGICANALYZER_H */

