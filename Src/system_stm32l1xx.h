/**
  ******************************************************************************
  * @file    system_stm32l1xx.h
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    09-March-2012
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Header File.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#ifndef __SYSTEM_STM32L1XX_H
#define __SYSTEM_STM32L1XX_H

#include "Beacon_board.h"
#include "stm32l1xx.h"
#include "main.h"
#include "USART.h"

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */ 
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field. 
                                  This value must be a multiple of 0x200. */

/* Error of system init */
#define	SUE_PWR_REG			((uint32_t)0x00000001)	// Timeout setup power regulator (PWR_CR)
#define	SUE_LCK_A				((uint32_t)0x00000002)	// No locking port A
#define	SUE_LCK_B				((uint32_t)0x00000004)	// No locking port B
#define	SUE_LCK_C				((uint32_t)0x00000008)	// No locking port C
#define	SUE_LCK_H				((uint32_t)0x00000010)	// No locking port H

/* Reset reasons */
#define RESET_UNKNOW		((uint32_t)0x00000001)
#define RESET_PIN				((uint32_t)0x00000002)
#define RESET_POR				((uint32_t)0x00000004)
#define RESET_WDG				((uint32_t)0x00000008)
#define RTC_TIME_UPDATE	((uint32_t)0x00000010)

/* Set SYSCLK */
#define	MSI_R0		((RCC_CFGR_SW_MSI << 8) | (RCC_ICSCR_MSIRANGE_0 >> (13-4)))
#define	MSI_R1		((RCC_CFGR_SW_MSI << 8) | (RCC_ICSCR_MSIRANGE_1 << 4))
#define	MSI_R2		((RCC_CFGR_SW_MSI << 8) | (RCC_ICSCR_MSIRANGE_2 << 4))
#define	MSI_R3		((RCC_CFGR_SW_MSI << 8) | (RCC_ICSCR_MSIRANGE_3 << 4))
#define	MSI_R4		((RCC_CFGR_SW_MSI << 8) | (RCC_ICSCR_MSIRANGE_4 << 4))
#define	MSI_R5		((RCC_CFGR_SW_MSI << 8) | (RCC_ICSCR_MSIRANGE_5 << 4))
#define	MSI_R6		((RCC_CFGR_SW_MSI << 8) | (RCC_ICSCR_MSIRANGE_6 << 4))

#define	HSI_M075	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV4 >> (22-4)) | RCC_CFGR_PLLMUL3>>18)
#define	HSI_M100	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV3 << 4) | RCC_CFGR_PLLMUL3)	// w/o PLL
#define	HSI_M133	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV3 << 4) | RCC_CFGR_PLLMUL4)
#define	HSI_M150	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL3)
#define	HSI_M200	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL4)
#define	HSI_M266	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV3 << 4) | RCC_CFGR_PLLMUL8)
#define	HSI_M300	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL6)
#define	HSI_M400	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL8)
#define	HSI_M533	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV3 << 4) | RCC_CFGR_PLLMUL16)
#define	HSI_M600	((RCC_CFGR_SW_HSI << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL12)

#define	HSE_M075	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV4 << 4) | RCC_CFGR_PLLMUL3)
#define	HSE_M100	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV3 << 4) | RCC_CFGR_PLLMUL3)	// w/o PLL
#define	HSE_M133	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV3 << 4) | RCC_CFGR_PLLMUL4)
#define	HSE_M150	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL3)
#define	HSE_M200	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL4)
#define	HSE_M266	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV3 << 4) | RCC_CFGR_PLLMUL8)
#define	HSE_M300	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL6)
#define	HSE_M400	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL8)
#define	HSE_M533	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV3 << 4) | RCC_CFGR_PLLMUL16)
#define	HSE_M600	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL12)
#define	HSE_M800	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL16)
#define	HSE_M1066	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV3 << 4) | RCC_CFGR_PLLMUL32)
#define	HSE_M1200	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL24)
#define	HSE_M1600	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL32)
#define	HSE_M2400	((RCC_CFGR_SW_HSE << 8) | (RCC_CFGR_PLLDIV2 << 4) | RCC_CFGR_PLLMUL48)

#define	PWR_CR_VOS_CURRENT	0

void SystemInit(void);
void SystemCoreClockUpdate(void);
void StopMCU(void);
void StandbyMCU(void);

uint32_t GetCoreClock(void);
uint32_t GetAPB1Clock(void);
uint32_t GetAPB2Clock(void);

#endif /*__SYSTEM_STM32L1XX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
