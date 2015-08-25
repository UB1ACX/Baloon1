/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Template/main.h 
  * @author  MCD Application Team
  * @version V1.0.3
  * @date    May-2013
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

//#define __DBG_ITM
#define __DEBUG

#ifdef __DEBUG
#define __DBG_ITM
#endif

#include "Beacon_board.h"
#include "system_stm32l1xx.h"
#include "stm32l1xx.h"
#include "serial.h"
#include "core_cm3.h"
#include <stdio.h>
#include "i2c.h"
#include "gps.h"
#include "usart.h"
#include "Si446x.h"
#include "SPI.h"
#include "Timers.h"
#include "RTC.h"
#include "USART.h"

extern uint32_t UpdatePeriod, CountPeriod, Overflow_CNT2;
extern uint32_t StartUp_Error;
extern uint32_t SystemCoreClock;
extern uint32_t ResetFlags;

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
