#ifndef __GPS_H
#define __GPS_H

#include "Beacon_board.h"
#include "stm32l1xx.h"
#include "USART.h"
#include "Timers.h"
#include <stdio.h>
extern uint32_t ResetFlags;


typedef struct
{
  uint32_t Satel;
  uint32_t Time;
  uint32_t Locate;
} GPS_Data_TypeDef;

void GPS_Init(void);
void GPS_Comm(char *pstr);

//bool I2C_SendByte(uint8_t Address, uint8_t Register, uint8_t Data);

#endif
