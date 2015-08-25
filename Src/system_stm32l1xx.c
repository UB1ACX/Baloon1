#include "system_stm32l1xx.h"

uint32_t StartUp_Error; // mask of system startup error, until use printf() unpossible
uint32_t ResetFlags; // Saved Reset Flags

const uint8_t PLL_mult[16] = {3, 4, 6, 8, 12, 16, 24, 32, 48, 0, 0, 0, 0, 0, 0, 0};
const uint8_t PLL_div[4] = {0, 2, 3, 4};

	/**
  * @brief  Setup the microcontroller system.
  *         Initialize the Embedded Flash Interface, the PLL and update the 
  *         SystemCoreClock variable.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
	uint8_t tmp;
	uint16_t TimeOut;

	/* Force set MSION bit */
	RCC->CR = RCC_CR_MSION;

  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	/* Select the Voltage Range 1 (1.8 V) */
	PWR->CR = PWR_CR_VOS_0;
	/* Wait Until the Voltage Regulator is ready */
	StartUp_Error = 0;
	TimeOut = 65535;
	while (((PWR->CSR & PWR_CSR_VOSF) == PWR_CSR_VOSF) || (TimeOut != 0)) TimeOut--;
	if (TimeOut == 0) StartUp_Error |= SUE_PWR_REG;
  RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;

  /* Flash 0 wait state */
  /* Disable Prefetch Buffer */
  /* Disable 64-bit access */
  FLASH->ACR = 0;
	/* Force lock flash memory */
	FLASH->PECR = FLASH_PECR_OPTLOCK | FLASH_PECR_PRGLOCK | FLASH_PECR_PELOCK;

  /*!< Setup SW[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], MCOSEL[2:0] and MCOPRE[2:0] bits */
  RCC->CFGR = RCC_CFGR_MCO_DIV1 | RCC_CFGR_MCO_NOCLOCK | RCC_CFGR_PLLDIV2 | RCC_CFGR_PLLMUL4 | RCC_CFGR_PLLSRC_HSI |
							RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV1 | RCC_CFGR_SW_MSI;
  
	SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */

	/* Disable and clear all clocks system interrupts */
	RCC->CIR = 0;

	switch (RCC->CSR & (RCC_CSR_OBLRSTF | RCC_CSR_PINRSTF | RCC_CSR_PORRSTF | RCC_CSR_SFTRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_LPWRRSTF))
	{
		case RCC_CSR_PINRSTF:
			{ ResetFlags = RESET_PIN; /*printf("Pin reset. \r\n");*/ break; };
		case RCC_CSR_PORRSTF:
			{ ResetFlags = RESET_POR; /*printf("Power on reset. \r\n");*/ break; };
		case RCC_CSR_IWDGRSTF:
			{ ResetFlags = RESET_WDG; /*printf("Watchdog reset. \r\n");*/ break; };
		default:
			ResetFlags = RESET_UNKNOW; /*printf("Unknow reset. CSR=%X\r\n", RCC->CSR & (RCC_CSR_OBLRSTF | RCC_CSR_PINRSTF | RCC_CSR_PORRSTF | RCC_CSR_SFTRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_LPWRRSTF));*/ 
	};
	RCC->CSR |= RCC_CSR_RMVF;

	/* Reset ports */
	RCC->AHBRSTR |= RCC_AHBRSTR_GPIOARST | RCC_AHBRSTR_GPIOBRST | RCC_AHBRSTR_GPIOCRST |RCC_AHBRSTR_GPIOHRST;
	RCC->AHBRSTR &= ~(RCC_AHBRSTR_GPIOARST | RCC_AHBRSTR_GPIOBRST | RCC_AHBRSTR_GPIOCRST |RCC_AHBRSTR_GPIOHRST);
	/* Setup ports */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOHEN;

	/*
	PA15 - 											input, pulldown
	PA14 - DEBUG	SWD    SWCLK	alt, AFIO=0, med speed, pulldown
	PA13 - DEBUG	SWD    SWDIO	alt, AFIO=0, med speed, pullup
	PA12 - 											input, pulldown
	PA11 - 											input, pulldown
	PA10 - DEBUG	USART1 RX			alt, AFIO=7, pullup
	PA09 - DEBUG	USART1 TX			alt, AFIO=7, low speed
	PA08 - 											input, pulldown
	PA07 - Si4463	SPI1   MOSI		alt, AFIO=5, med speed
	PA06 - Si4463	SPI1   MISO		alt, AFIO=5, pulldown / TIM3_1 alt, AFIO=2
	PA05 - Si4463	SPI1   SCLK		alt, AFIO=5, med speed
	PA04 - Si4463	       nSEL		output, hi, med speed
	PA03 - GPS		USART2 RX			alt, AFIO=7, pullup
	PA02 - GPS		USART2 TX			alt, AFIO=7, low speed
	PA01 - Si4463        SDN		output, hi, med speed
	PA00 - GPS		       STATUS	alt, AFIO=0 (temp. - input, pulldown)
	*/
	GPIOA->ODR = GPIO_ODR_ODR_4 | GPIO_ODR_ODR_1;
	GPIOA->MODER = GPIO_MODER_MODER14_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER10_1 | GPIO_MODER_MODER9_1 |
								 GPIO_MODER_MODER7_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER4_0 |
								 GPIO_MODER_MODER3_1 | GPIO_MODER_MODER2_1 | GPIO_MODER_MODER1_0;// | GPIO_MODER_MODER0_1;
//	GPIOA->MODER = 0xEBFFFFFF;
	GPIOA->OTYPER = 0;
	GPIOA->OSPEEDR = GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR13_1 | GPIO_OSPEEDER_OSPEEDR9_0 |
									 GPIO_OSPEEDER_OSPEEDR7_1 | GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR4_1 |
									 GPIO_OSPEEDER_OSPEEDR2_0 | GPIO_OSPEEDER_OSPEEDR1_1;
	GPIOA->PUPDR = GPIO_PUPDR_PUPDR15_1 | GPIO_PUPDR_PUPDR14_1 | GPIO_PUPDR_PUPDR13_0 | GPIO_PUPDR_PUPDR12_1 |
								 GPIO_PUPDR_PUPDR11_1 | GPIO_PUPDR_PUPDR10_0 | GPIO_PUPDR_PUPDR8_1 | GPIO_PUPDR_PUPDR6_1 |
								 GPIO_PUPDR_PUPDR3_0 | GPIO_PUPDR_PUPDR0_1;
	GPIOA->AFR[0] = (0x5 << 28) | (0x5 << 24) | (0x5 << 20) | (0x7 << 12) | (0x7 << 8);
	GPIOA->AFR[1] = (0x7 << 8) | (0x7 << 4);
/*	GPIOA->LCKR = GPIO_LCKR_LCKK | 0;
	GPIOA->LCKR = 0;
	GPIOA->LCKR = GPIO_LCKR_LCKK | 0;
	tmp = GPIOA->LCKR; tmp = GPIOA->LCKR;
	if ((GPIOA->LCKR & GPIO_LCKR_LCKK) != GPIO_LCKR_LCKK) StartUp_Error |= SUE_LCK_A;*/

	/*
	PB15 - 										input, pulldown
	PB14 - GREEN LED					output, low, very low speed
	PB13 - 										input, pulldown
	PB12 - SCV			ADC				analog input
	PB11 - Si4463		USART3 RX alt, AFIO=7, pullup
	PB10 - Si4463		USART3 TX alt, AFIO=7, low speed
	PB09 - GPS			RESET			output, open drain, hi, low speed
	PB08 - GPS PPS	TIM4_3		alt, AFIO=2, pulldown
	PB07 - MAX17047 I2C1	SDA	alt, AFIO=4, low speed, open drain
	PB06 - MAX17047 I2C1	SCL	alt, AFIO=4, low speed
	PB05 - GPS			POWER			output, low, very low speed
	PB04 - 										input, pulldown
	PB03 - DEBUG		SWD		SWO	alt, AFIO=0, med speed
	PB02 - SYS			BOOT1			input, pulldown
	PB01 - Si4463		FREQ			input, pulldown
	PB00 - Si4463		nIRQ			input, pulldown
	*/
	GPIOB->ODR = GPIO_ODR_ODR_9 | GPIO_ODR_ODR_5;
	#ifdef __DBG_ITM
	GPIOB->MODER = GPIO_MODER_MODER14_0 | GPIO_MODER_MODER12 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER10_1 |
								 GPIO_MODER_MODER9_0 | GPIO_MODER_MODER8_1 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER6_1 |
								 GPIO_MODER_MODER5_0 | GPIO_MODER_MODER3_1;
	GPIOB->OSPEEDR = GPIO_OSPEEDER_OSPEEDR10_0 | GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR7 |
									 GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR3_0;
	GPIOB->PUPDR = GPIO_PUPDR_PUPDR15_1 | GPIO_PUPDR_PUPDR13_1 | GPIO_PUPDR_PUPDR11_0 | GPIO_PUPDR_PUPDR8_1 |
								 GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR0_0;
	#else
	GPIOB->MODER = GPIO_MODER_MODER14_0 | GPIO_MODER_MODER12 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER10_1 |
								 GPIO_MODER_MODER9_0 | GPIO_MODER_MODER8_1 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER6_1 |
								 GPIO_MODER_MODER5_0;
	GPIOB->OSPEEDR = GPIO_OSPEEDER_OSPEEDR10_0 | GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR7_0 |
									 GPIO_OSPEEDER_OSPEEDR6_0;
	GPIOB->PUPDR = GPIO_PUPDR_PUPDR15_1 | GPIO_PUPDR_PUPDR13_1 | GPIO_PUPDR_PUPDR11_0 | GPIO_PUPDR_PUPDR8_1 |
								 GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR3_0 |
								 GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR0_0;
	#endif
	GPIOB->OTYPER = GPIO_OTYPER_OT_9 | GPIO_OTYPER_OT_7;
	GPIOB->AFR[0] = (0x4 << 28) | (0x4 << 24);
	GPIOB->AFR[1] = (0x7 << 12) | (0x7 << 8) | (0x2 << 0);
	/*GPIOB->LCKR = GPIO_LCKR_LCKK | 0;
	GPIOB->LCKR = 0;
	GPIOB->LCKR = GPIO_LCKR_LCKK | 0;
	tmp = GPIOB->LCKR; tmp = GPIOB->LCKR;
	if ((GPIOB->LCKR & GPIO_LCKR_LCKK) != GPIO_LCKR_LCKK) StartUp_Error |= SUE_LCK_B;*/

	/*
	PC15 - 								input, pulldown
	PC14 - 								input, pulldown
	PC13 - MAX17047	ALERT	input, pullup
	*/
	GPIOC->MODER = 0;
	GPIOC->OTYPER = 0;
	GPIOC->OSPEEDR = 0;
	GPIOC->PUPDR = GPIO_PUPDR_PUPDR15_1 | GPIO_PUPDR_PUPDR14_1 | GPIO_PUPDR_PUPDR13_0;
	GPIOC->ODR = 0;
	GPIOC->AFR[0] = 0;
	GPIOC->AFR[1] = 0;
	GPIOC->LCKR = GPIO_LCKR_LCKK | 0;
	GPIOC->LCKR = 0;
	GPIOC->LCKR = GPIO_LCKR_LCKK | 0;
	tmp = GPIOC->LCKR; tmp = GPIOC->LCKR;
	if ((GPIOC->LCKR & GPIO_LCKR_LCKK) != GPIO_LCKR_LCKK) StartUp_Error |= SUE_LCK_C;

	/*
	PH1 - OSC_OUT (temp. - input, pulldown)
	PH0 - OSC_IN (temp. - input, pulldown)
	*/
	GPIOH->MODER = GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0;
	GPIOH->OTYPER = 0;
	GPIOH->OSPEEDR = 0;
	GPIOH->PUPDR = GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1;
	GPIOH->ODR = 0;
	GPIOH->AFR[0] = 0;
	GPIOH->AFR[1] = 0;
	GPIOH->LCKR = GPIO_LCKR_LCKK | 0;
	GPIOH->LCKR = 0;
	GPIOH->LCKR = GPIO_LCKR_LCKK | 0;
	tmp = GPIOH->LCKR; tmp = GPIOH->LCKR;
	if ((GPIOH->LCKR & GPIO_LCKR_LCKK) != GPIO_LCKR_LCKK) StartUp_Error |= SUE_LCK_H;

	RCC->AHBENR &= ~(RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOHEN);
	
  /* Trim MSI to 2.0 MHz (for SWO debug) */
  RCC->ICSCR &= ~RCC_ICSCR_MSITRIM;
//  tmp = 0xF8;
  tmp = 0xFE;
  RCC->ICSCR |= tmp << 24;

	/* Allow access to the RCC_CSR */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;

	/* Run LSI oscillator for WatchDog and RTC */
	RCC->CSR |= RCC_CSR_LSION;
	TimeOut = 65535;
	while (((RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY) || (TimeOut != 0)) TimeOut--;
//	if (TimeOut == 0) StartUp_Error |= SUE_LSI_ON;

	PWR->CR &= ~PWR_CR_DBP;
	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
}

uint32_t GetSystemClock(void) // in Hz
{
	uint32_t	Temp, Temp1, Temp2;

	switch (RCC->CFGR & RCC_CFGR_SWS) {
		case RCC_CFGR_SWS_MSI:
			Temp = (MSI_VALUE << 1) >> (6-((RCC->ICSCR & RCC_ICSCR_MSIRANGE) >> 13));
			break;
		case RCC_CFGR_SWS_HSI:
			Temp = HSI_VALUE;
			break;
		case RCC_CFGR_SWS_HSE:
			Temp = HSE_VALUE;
			break;
		case RCC_CFGR_SWS_PLL:
			if ((RCC->CFGR & RCC_CFGR_PLLSRC) == RCC_CFGR_PLLSRC_HSI) Temp = HSI_VALUE;
			else Temp = HSE_VALUE;
			Temp1 = PLL_mult[((RCC->CFGR & RCC_CFGR_PLLMUL) >> 18)];
			Temp2 = PLL_div[((RCC->CFGR & RCC_CFGR_PLLDIV) >> 22)];
			if ((Temp1 > 0) && (Temp2 > 0)) Temp = (Temp * Temp1) / Temp2;
			else Temp = 0;
			break;
		default:
			Temp = 0;
	}
	return(Temp);
}

uint32_t GetCoreClock(void)
{
	const uint16_t AHB_presc[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

	return(GetSystemClock() >> AHB_presc[(RCC->CFGR & RCC_CFGR_HPRE) >> 4]);
}

uint32_t GetAPB1Clock(void)
{
	const uint16_t APB1_presc[8] = {0, 0, 0, 0, 1, 2, 3, 4};

	return(GetCoreClock() >> APB1_presc[(RCC->CFGR & RCC_CFGR_PPRE1) >> 8]);
}

uint32_t GetAPB2Clock(void) {
	const uint16_t APB2_presc[8] = {0, 0, 0, 0, 1, 2, 3, 4};

	return(GetSystemClock() >> APB2_presc[(RCC->CFGR & RCC_CFGR_PPRE2) >> 11]);
}

int32_t SetSystemClock(uint32_t Freq, uint32_t VOS) // Freq in Hz
{
	uint8_t VoltRange;
	uint16_t Timeout;
	uint32_t Temp;
	const uint32_t MaxFreq[3][4] = /* Range 1 */ {{16000000, 24000000, 96000000, 32000000},
																 /* Range 2 */  {16000000, 16000000, 48000000, 16000000},
																 /* Range 3 */ 				 {0,  8000000, 24000000,  4000000}}; // HSI, HSE, PLL_VCO, PLL_OUT

	VoltRange = VOS & PWR_CR_VOS;
	if (VoltRange == PWR_CR_VOS_CURRENT) VoltRange = PWR->CR & PWR_CR_VOS;
	VoltRange = VoltRange >> 11;

	switch (Freq & RCC_CFGR_SW) {
		case RCC_CFGR_SW_MSI: {
			RCC->ICSCR &= ~RCC_ICSCR_MSIRANGE;
			RCC->ICSCR |= (Freq & RCC_ICSCR_MSIRANGE);
			RCC->CR |= RCC_CR_MSION;
			Timeout = 10000;
			while ((RCC->CR & RCC_CR_MSIRDY) != RCC_CR_MSIRDY) {
				Timeout--;
				if (Timeout == 0) {
					RCC->CR &= ~RCC_CR_MSION;
					printf("Timeout MSI ready\n\r");
					return (-2);
				}
			}
			Temp = RCC->CFGR;
			Temp &= ~RCC_CFGR_SW;
			Temp |= RCC_CFGR_SW_MSI;
			RCC->CFGR = Temp;
			Timeout = 1000;
			while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI) {
				Timeout--;
				if (Timeout == 0) {
					printf("Source clock not switch/n/r");
					return(-4);
				}
			}
			RCC->CR &= ~(RCC_CR_PLLON | RCC_CR_HSION | RCC_CR_HSEON);
			break;
		}
		case RCC_CFGR_SW_HSI: {
			if (MaxFreq[VoltRange][0] > HSI_VALUE) {
				printf("HSI=%d above of Fmax=%d for voltage range%d/n/r", HSI_VALUE, MaxFreq[VoltRange][0], VoltRange);
				return(-3);
			}
			if (Freq != HSI_M100) {
				Temp = HSI_VALUE * PLL_mult[(Freq & RCC_CFGR_PLLMUL) >> 18];
				if (MaxFreq[VoltRange][2] > Temp) {
					printf("PLLVCO=%d above of Fmax=%d for voltage range%d/n/r", Temp, MaxFreq[VoltRange][2], VoltRange);
					return(-3);
				}
				Temp = Temp / PLL_div[(Freq & RCC_CFGR_PLLDIV) >> 22];
				if (MaxFreq[VoltRange][3] > Temp) {
					printf("SYSCLK=%d above of Fmax=%d for voltage range%d/n/r", Temp, MaxFreq[VoltRange][3], VoltRange);
					return(-3);
				}
			}
			RCC->CR |= RCC_CR_HSION;
			Timeout = 1000;
			while ((RCC->CR & RCC_CR_HSIRDY) == RCC_CR_HSIRDY) {
				Timeout--;
				if (Timeout == 0) {
					RCC->CR &= ~RCC_CR_HSION;
					printf("Timeout HSI ready\n\r");
					return(-3);
				}
			}
			if (Freq == HSI_M100) {
				Temp = RCC->CFGR;
				Temp &= ~RCC_CFGR_SW;
				Temp |= RCC_CFGR_SW_HSI;
				RCC->CFGR = Temp;
				Timeout = 1000;
				while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
					Timeout--;
					if (Timeout == 0) {
						printf("Source clock not switch/n/r");
						return(-4);
					}
				}
				RCC->CR &= ~(RCC_CR_MSION | RCC_CR_HSEON | RCC_CR_PLLON);
			} else {
				RCC->CFGR &= ~RCC_CFGR_PLLSRC;
				RCC->CFGR &= ~(RCC_CFGR_PLLMUL | RCC_CFGR_PLLDIV);
				RCC->CFGR |= (Freq & (RCC_CFGR_PLLMUL | RCC_CFGR_PLLDIV));
				RCC->CR |= RCC_CR_PLLON;
				Timeout = 1000;
				while ((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY) {
					Timeout--;
					if (Timeout == 0) {
						RCC->CR &= ~RCC_CR_PLLON;
						printf("Timeout PLL ready\n\r");
						return(-3);
					}
				}
				Temp = RCC->CFGR;
				Temp &= ~RCC_CFGR_SW;
				Temp |= RCC_CFGR_SW_PLL;
				RCC->CFGR = Temp;
				Timeout = 1000;
				while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
					Timeout--;
					if (Timeout == 0) {
						printf("Source clock not switch/n/r");
						return(-4);
					}
				}
				RCC->CR &= ~(RCC_CR_MSION | RCC_CR_HSEON);
			}
			break;
		}
		case RCC_CFGR_SW_HSE: {
			if (MaxFreq[VoltRange][1] > HSE_VALUE) {
				printf("HSE=%d above of Fmax=%d for voltage range%d/n/r", HSE_VALUE, MaxFreq[VoltRange][1], VoltRange);
				return(-3);
			}
			Temp = HSI_VALUE * PLL_mult[(Freq & RCC_CFGR_PLLMUL) >> 18];
			if (MaxFreq[VoltRange][2] > Temp) {
				printf("PLLVCO=%d above of Fmax=%d for voltage range%d/n/r", Temp, MaxFreq[VoltRange][2], VoltRange);
				return(-3);
			}
			Temp = Temp / PLL_div[(Freq & RCC_CFGR_PLLDIV) >> 22];
			if (MaxFreq[VoltRange][3] > Temp) {
				printf("SYSCLK=%d above of Fmax=%d for voltage range%d/n/r", Temp, MaxFreq[VoltRange][3], VoltRange);
				return(-3);
			}
			RCC->CR |= RCC_CR_HSEON;
			Timeout = 1000;
			while ((RCC->CR & RCC_CR_HSERDY) == RCC_CR_HSERDY) {
				Timeout--;
				if (Timeout == 0) {
					RCC->CR &= ~RCC_CR_HSEON;
					printf("Timeout HSE ready\n\r");
					return(-3);
				}
			}
			if (Freq == HSE_M100) {
				Temp = RCC->CFGR;
				Temp &= ~RCC_CFGR_SW;
				Temp |= RCC_CFGR_SW_HSE;
				RCC->CFGR = Temp;
				Timeout = 1000;
				while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {
					Timeout--;
					if (Timeout == 0) {
						printf("Source clock not switch/n/r");
						return(-4);
					}
				}
				RCC->CR &= ~(RCC_CR_MSION | RCC_CR_HSION | RCC_CR_PLLON);
			} else {
				RCC->CFGR |= RCC_CFGR_PLLSRC;
				RCC->CFGR &= ~(RCC_CFGR_PLLMUL | RCC_CFGR_PLLDIV);
				RCC->CFGR |= (Freq & (RCC_CFGR_PLLMUL | RCC_CFGR_PLLDIV));
				RCC->CR |= RCC_CR_PLLON;
				Timeout = 1000;
				while ((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY) {
					Timeout--;
					if (Timeout == 0) {
						printf("Timeout PLL ready\n\r");
						return(-3);
					}
				}
				Temp = RCC->CFGR;
				Temp &= ~RCC_CFGR_SW;
				Temp |= RCC_CFGR_SW_PLL;
				RCC->CFGR = Temp;
				Timeout = 1000;
				while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
					Timeout--;
					if (Timeout == 0) {
						printf("Source clock not switch/n/r");
						return(-4);
					}
				}
				RCC->CR &= ~(RCC_CR_MSION | RCC_CR_HSION);
			}
			break;
		}
		default: {
			printf("Unknow clock source\n\r");
			return(-1);
		}
	}
	return(0);
}

void StopMCU(void)
{
	SCB->SCR |= SCB_SCR_SLEEPDEEP;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CSR &= ~PWR_CSR_WUF;
	PWR->CR &= ~PWR_CR_PDDS; // Stop
	PWR->CR |= PWR_CR_ULP; // VREFINT is switched off
	PWR->CR |= PWR_CR_CWUF; // Clear Wakeup flag
	PWR->CR |= PWR_CR_LPSDSR;
	PWR->CR |= PWR_CR_LPRUN;
	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
	__WFI();
	__NOP();
}

void StandbyMCU(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_CWUF; // Clear Wakeup flag
	PWR->CR |= PWR_CR_ULP; // VREFINT is switched off
	PWR->CR |= PWR_CR_PDDS; // Standby
	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
	SCB->SCR |= SCB_SCR_SLEEPDEEP;
	__WFE();
	while(1);
}

void MCOcontrol(uint8_t OutType, uint8_t Prescaler, uint8_t Switch)
{
/*	uint32_t Save_Mode, Save_OType, Save_OSpeed, Save_PUPD, Save_OD, Save_AF;

	if (Switch > 0)
	{
		Save_Mode = GPIOA->MODER & GPIO_MODER_MODER8;
		Save_OType = GPIOA->OTYPER & GPIO_OTYPER_OT_8;
		Save_OSpeed = GPIOA->OSPEEDR & GPIO_OSPEEDER_OSPEEDR8;
		Save_PUPD = GPIOA->PUPDR & GPIO_PUPDR_PUPDR8;
		Save_OD = GPIOA->ODR & GPIO_ODR_ODR_8;
		Save_AF = GPIOA->AFR[1] & GPIO_AFRH_AFRH8;
	}
	else
	{
	}*/
//	RCC->CFGR = RCC_CFGR_MCO_DIV1 | RCC_CFGR_MCO_MSI | RCC_CFGR_PLLDIV2 | RCC_CFGR_PLLMUL3 | RCC_CFGR_PLLSRC_HSI |
//							RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV1 | RCC_CFGR_SW_MSI;
//	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_MCOSEL));
//	RCC->CFGR |= (uint32_t)RCC_CFGR_MCO_SYSCLK;
//	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//	GPIOA->MODER |= GPIO_MODER_MODER8_1;
//	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;
//	RCC->AHBENR &= ~RCC_AHBENR_GPIOAEN;
}
