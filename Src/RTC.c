#include "RTC.h"

void RTC_Init(void)
{
	if ((RCC->CSR & RCC_CSR_RTCEN) != RCC_CSR_RTCEN)
	{
	/* Allow access to the RTC */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_DBP;

		/* Reset Backup Domain */
		RCC->CSR |=  RCC_CSR_RTCRST;
		RCC->CSR &= ~RCC_CSR_RTCRST;
		RCC->CSR |= RCC_CSR_RTCEN;

		/* Select the RTC clock source */
		RCC->CSR &= ~RCC_CSR_RTCSEL;
		RCC->CSR |=  RCC_CSR_RTCSEL_LSI;

		RTC->WPR = 0xCA; RTC->WPR = 0x53;
		
		RTC->ISR |= RTC_ISR_INIT;
		while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF);
		RTC->PRER = PREDIV_S - 1;
		RTC->PRER |= (PREDIV_A - 1) << 16;
		RTC->ISR &= ~RTC_ISR_INIT;

		RTC->CR &= ~RTC_CR_WUTE;
		while ((RTC->ISR & RTC_ISR_WUTWF) != RTC_ISR_WUTWF);
		RTC->WUTR = (uint32_t)RTC_WUTR-1;
		RTC->CR &= ~RTC_CR_WUCKSEL;
		RTC->CR |= RTC_CR_WUCKSEL_2;
		RTC->CR |= RTC_CR_WUTIE;
		RTC->CR |= RTC_CR_WUTE;

		RTC->WPR = 0xFF;
		PWR->CR &= ~PWR_CR_DBP;
		RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;

/*		EXTI->PR = EXTI_PR_PR20;
		EXTI->IMR |= EXTI_IMR_MR20;
		EXTI->RTSR |= EXTI_RTSR_TR20;
*/

		NVIC_EnableIRQ(RTC_WKUP_IRQn);
		ResetFlags |= RTC_TIME_UPDATE;
	}
}

void RTC_SetupTime(void)
{
	/* Allow access to the RTC */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;

	RTC->WPR = 0xCA; RTC->WPR = 0x53;
	
	RTC->ISR |= RTC_ISR_INIT;
	while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF);

	RTC->TR = 0x20;
	RTC->DR = (1UL << 20) | (5UL << 16);
	RTC->CR &= ~RTC_CR_FMT;

	RTC->WPR = 0xFF;
	PWR->CR &= ~PWR_CR_DBP;
	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;

	ResetFlags &= ~RTC_TIME_UPDATE;
}

void RTC_WKUP_IRQHandler(void)
{
/*RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	LED_GREEN_ON;
	Delay_mS(50); // Enable TIM6_Init
	LED_GREEN_OFF;
	RCC->AHBENR &= ~RCC_AHBENR_GPIOBEN;*/
	printf("Time=%X\r\n", RTC->TR);


	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;
	RTC->ISR &= ~RTC_ISR_WUTF;
	PWR->CR &= ~PWR_CR_DBP;
	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
	EXTI->PR = EXTI_PR_PR20;
}
