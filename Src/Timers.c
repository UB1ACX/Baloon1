#include "Timers.h"

uint32_t Overflow_CNT2, OldCNT, NewCNT, PPSfirst, CountPeriod, UpdatePeriod;
uint32_t FreqMin, FreqMax;
uint32_t Overflow_CNT4;

/* Timer 2 init */
void Timer2_Init(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER1_1 | GPIO_MODER_MODER15_1;
	GPIOA->AFR[0] &= 0xFFFFFF0F;
	GPIOA->AFR[1] &= 0x0FFFFFFF;
	GPIOA->AFR[0] |= 0x00000010;
	GPIOA->AFR[1] |= 0x10000000;

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->CCMR1 |= TIM_CCMR1_CC2S_0 | TIM_CCMR1_CC1S_0;
	TIM2->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0 | TIM_SMCR_SMS;
	TIM2->CCER |= TIM_CCER_CC2E | TIM_CCER_CC1E;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->DIER |= TIM_DIER_CC2IE | TIM_DIER_UIE;
	TIM2->SR &= ~(TIM_SR_UIF | TIM_SR_CC2IF);
	NVIC_EnableIRQ(TIM2_IRQn);

	PPSfirst = 0;
	FreqMin = 0xFFFFFFFF;
	FreqMax = 0;
}

// Master
void Timer3_Init(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER6_1;
	GPIOA->AFR[0] &= 0xF0FFFFFF;
	GPIOA->AFR[0] |= 0x02000000;

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->ARR = 0x1;
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;
	TIM3->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0 | TIM_SMCR_SMS;
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->CR2 |= TIM_CR2_MMS_1;
	TIM3->CR1 |= TIM_CR1_CEN;
}

// Slave
void Timer4_Init(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER8_1;
	GPIOB->AFR[1] &= 0xFFFFFFF0;
	GPIOB->AFR[1] |= 0x00000002;

	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->CCMR2 |= TIM_CCMR2_CC3S_0;
	TIM4->SMCR |= TIM_SMCR_TS_1 | TIM_SMCR_SMS;
	TIM4->CCER |= TIM_CCER_CC3E;
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM4->DIER |= TIM_DIER_CC3IE | TIM_DIER_UIE;
	TIM4->SR &= ~(TIM_SR_CC3IF | TIM_SR_UIF);
	NVIC_EnableIRQ(TIM4_IRQn);

	PPSfirst = 0;
	FreqMin = 0xFFFFFFFF;
	FreqMax = 0;
}

void TIM2_IRQHandler(void) {
	if ((TIM2->SR & TIM_SR_UIF) != 0) {
		Overflow_CNT2++;
		TIM2->SR &= ~TIM_SR_UIF;
	}
	if ((TIM2->SR & TIM_SR_CC2IF) != 0) {
		if (PPSfirst == 0) {
			OldCNT = TIM2->CNT;
			PPSfirst = 1;
		}
		else {
			NewCNT = TIM2->CNT;
			CountPeriod = ((1 << 16) * Overflow_CNT2) + (NewCNT - OldCNT);
			OldCNT = NewCNT;
			UpdatePeriod = 1;
			if (CountPeriod < FreqMin) FreqMin = CountPeriod;
			if (CountPeriod > FreqMax) FreqMax = CountPeriod;
			printf("Freq: %d %5d %5d\n\r", CountPeriod, FreqMin, FreqMax);

		}
		Overflow_CNT2 = 0;
		TIM2->SR &= ~TIM_SR_CC2IF;
	}
}

void TIM4_IRQHandler(void) {
	if ((TIM4->SR & TIM_SR_UIF) != 0) {
		Overflow_CNT4++;
		TIM4->SR &= ~TIM_SR_UIF;
	}
	if ((TIM4->SR & TIM_SR_CC3IF) != 0) {
		if (PPSfirst == 0) {
			OldCNT = TIM4->CNT;
			PPSfirst = 1;
		}
		else {
			NewCNT = TIM4->CNT;
			CountPeriod = ((1 << 16) * Overflow_CNT4) + (NewCNT - OldCNT);
			OldCNT = NewCNT;
			UpdatePeriod = 1;
			if (CountPeriod < FreqMin) FreqMin = CountPeriod;
			if (CountPeriod > FreqMax) FreqMax = CountPeriod;
			printf("Freq: %d %5d %5d\n\r", CountPeriod, FreqMin, FreqMax);

		}
		Overflow_CNT4 = 0;
		TIM4->SR &= ~TIM_SR_CC3IF;
	}
}

void TIM6_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->PSC = (GetAPB1Clock()/1000000)-1;
	TIM6->CR1 = TIM_CR1_URS | TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM6_IRQn);
}

void TIM6_IRQHandler(void)
{
}

void Delay_uS(uint32_t Delay)
{
	if (Delay <= 65535)
	{
		TIM6->ARR = Delay;
		TIM6->EGR |= TIM_EGR_UG;
		while ((TIM6->SR & TIM_SR_UIF) != TIM_SR_UIF);
		TIM6->SR &= ~TIM_SR_UIF;
	}
}

void Delay_mS(uint32_t Delay)
{
	uint32_t i;
	for (i=1; i<=Delay; i++) Delay_uS(1000);
}
