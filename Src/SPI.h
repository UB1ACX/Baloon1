#ifndef __SPI_H
#define __SPI_H

#include "stm32l1xx.h"
#include <stdio.h>
#include "Serial.h"
#include "Si446x_def.h"
#include "Si446x.h"

#define nSEL_LOW	GPIOD->BSRRH	= (1UL << 2)
#define nSEL_HIGH	GPIOD->BSRRL	= (1UL << 2)

void SPI1_Init(void);
uint32_t SPI_SendCmd(uint8_t CountTX, uint8_t CountRX, Si446x_TypeDef* pData);

#endif
