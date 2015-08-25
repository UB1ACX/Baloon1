#include "SPI.h"

/* SPI3 init */
void SPI3_Init(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODER2_0;
	nSEL_HIGH;
	GPIOC->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11 | GPIO_MODER_MODER12);
	GPIOC->MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1;
	GPIOC->AFR[1] &= ~0x000FFF00; //((6UL << 12) | (6UL << 16) | (6UL << 8));
	GPIOC->AFR[1] |= 0x00066600; //((6UL << 12) | (6UL << 16) | (6UL << 8));
	GPIOC->PUPDR |=GPIO_PUPDR_PUPDR11_1;

	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	Si446x_SPI->CR1 = (SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_BR_0 | SPI_CR1_MSTR);
}

void SPI1_Init(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODER2_0;
	nSEL_HIGH;
	GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
	GPIOA->AFR[0] &= ~0xFFF00000; //((5UL << 28) | (5UL << 24) | (5UL << 20));
	GPIOA->AFR[0] |= 0x55500000; //((5UL << 28) | (5UL << 24) | (5UL << 20));
	GPIOA->PUPDR |=GPIO_PUPDR_PUPDR6_1;

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	Si446x_SPI->CR1 = (SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_BR_0 | SPI_CR1_MSTR);
}

uint32_t SPI_SendCmd(uint8_t CountTX, uint8_t CountRX, Si446x_TypeDef* pData) {
	uint8_t i;

	if ((CountTX > 16) || (CountRX > 16)) return (1);

	if (CountTX == 0) return (2);

	/* Test ready radio for get command */
	while ((Si446x_SPI->SR & SPI_SR_TXE) != SPI_SR_TXE);
	do {
		nSEL_LOW;
		Si446x_SPI->DR = READ_CMD_BUFF;
		while ((Si446x_SPI->SR & SPI_SR_TXE) != SPI_SR_TXE);
		Si446x_SPI->DR = READ_CMD_BUFF; // clocks for read CTS
		while ((Si446x_SPI->SR & SPI_SR_RXNE) != SPI_SR_RXNE);
		i = Si446x_SPI->DR; // clear RX buffer
		while ((Si446x_SPI->SR & SPI_SR_BSY) == SPI_SR_BSY);
		nSEL_HIGH;
	} while ((Si446x_SPI->DR & CTS)!= CTS);

	/* Send target cmd */
	nSEL_LOW;
// Send command as DMA transfer !!!!!
	for (i = 0; i < CountTX; i++) {
		while ((Si446x_SPI->SR & SPI_SR_TXE) != SPI_SR_TXE);
		Si446x_SPI->DR = pData->Stream[i];
	}
	while ((Si446x_SPI->SR & SPI_SR_BSY) == SPI_SR_BSY);
	nSEL_HIGH;

	/* Get answer */
	if (CountRX > 0) {
		do {
			nSEL_LOW;

			Si446x_SPI->DR = READ_CMD_BUFF;
			while ((Si446x_SPI->SR & SPI_SR_BSY) == SPI_SR_BSY);
			Si446x_SPI->DR = READ_CMD_BUFF; // clocks for read CTS
			i = Si446x_SPI->DR; // clear RX buffer
			while ((Si446x_SPI->SR & SPI_SR_BSY) == SPI_SR_BSY);

			if ((Si446x_SPI->DR & CTS) == CTS) break;

			nSEL_HIGH;
		} while (1);

		for (i = 0; i < CountRX-1; i++) { // CTS included in CountRX
			while ((Si446x_SPI->SR & SPI_SR_TXE) != SPI_SR_TXE);
			Si446x_SPI->DR = READ_CMD_BUFF; // clocks for read CTS and data
			while ((Si446x_SPI->SR & SPI_SR_BSY) == SPI_SR_BSY);
			pData->Stream[i] = (uint8_t)(Si446x_SPI->DR & (uint16_t)0xFF);
		}
		nSEL_HIGH;
	}

	return (0);
}
