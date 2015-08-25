#ifndef __USART_H
#define __USART_H

#include "stm32l1xx.h"
#include <stdio.h>
#include "string.h"
#include "Serial.h"

/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART 
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

#define	NMEA_WAIT_SYNC	0x01
#define	NMEA_CMD				0x02
#define	NMEA_DATA				0x03
#define	NMEA_CRC1				0x04
#define	NMEA_CRC2				0x05

#define NMEA_CMD_MAX	5
#define NMEA_DATA_MAX	(79-NMEA_CMD_MAX-4) // 4 = {',' '*23'}

void USART1_Init(uint32_t BaudRate);
void USART1_DeInit(void);
void USART1_SendChar(char ch);

void USART2_Init(uint32_t BaudRate);
void USART2_DeInit(void);
void USART2_SendChar(char ch);
void USART2_SendStr(char* pstr);
void USART2_IRQHandler(void);

#endif
