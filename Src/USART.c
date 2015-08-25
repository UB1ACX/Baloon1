#include "USART.h"
//			printf("\033[2J\033[;H");

uint8_t USART2_State;
uint8_t NMEA_Str_Count, NMEA_CheckSum, CheckSum;
char NMEA_Cmd_Str[NMEA_CMD_MAX];
char NMEA_Data_Str[NMEA_DATA_MAX+1]; // +1 for terminate string (/0)

void USART1_Init(uint32_t BaudRate)
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->BRR = __USART_BRR(SystemCoreClock, BaudRate);
	USART1->CR1 = USART_CR1_UE | USART_CR1_TE;
}

void USART1_DeInit(void)
{
	while ((USART1->SR & USART_SR_TC) != USART_SR_TC);
	USART1->CR1 = 0x00;
	RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
}

void USART1_SendChar(char ch)
{
	while ((USART1->SR & USART_SR_TXE) != USART_SR_TXE);
	USART1->DR = ch;
}

void USART1_SendStr(char* pstr)
{
	int i;
	i = 0;
	while (pstr[i] != 0x00) {
		USART1_SendChar(pstr[i]);
		i++;
	}
}


/* USART2 init */
void USART2_Init(uint32_t BaudRate)
{
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	USART2->BRR = __USART_BRR(SystemCoreClock, BaudRate);
	USART2->CR1 |= USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE;
	USART2_State = NMEA_WAIT_SYNC;
	NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_DeInit(void)
{
	while ((USART2->SR & USART_SR_TC) != USART_SR_TC);
	USART2->CR1 = 0x00;
	RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
}

void USART2_SendChar(char ch)
{
	while ((USART2->SR & USART_SR_TXE) != USART_SR_TXE);
	USART2->DR = ch;
}

void USART2_SendStr(char* str)
{
	int i;
	i = 0;
	while (str[i] != 0x00) {
		USART2_SendChar(str[i]);
		i++;
	}
}

void USART2_IRQHandler(void)
{
	char ch;
	uint8_t data;

	data = USART2->SR;
	if ((data & USART_SR_ORE) == USART_SR_ORE)
	{
		data = USART2->DR;
		USART2_State = NMEA_WAIT_SYNC;
		return;
	}
  if ((data & USART_SR_RXNE) == USART_SR_RXNE)
	{
		ch = USART2->DR;
		SER_PutChar(ch);
		switch (USART2_State)
		{
			case NMEA_WAIT_SYNC:
				if (ch == '$')
				{
					NMEA_Str_Count = 0;
					NMEA_CheckSum = 0;
					USART2_State = NMEA_CMD;
				}
				break;
			case NMEA_CMD:
				if (ch != ',') //&& (ch != '*'))
					if (NMEA_Str_Count < NMEA_CMD_MAX)
					{
						NMEA_Cmd_Str[NMEA_Str_Count++] = ch;
						NMEA_CheckSum ^= ch;
					}
					else USART2_State = NMEA_WAIT_SYNC;
				else
				{
					NMEA_CheckSum ^= ch;
					if (strstr(NMEA_Cmd_Str, "GPGGA"))
					{
						printf("GGA ");
						NMEA_Str_Count = 0;
						USART2_State = NMEA_DATA;
					}
					else
						if (strstr(NMEA_Cmd_Str, "GPGSA"))
						{
							printf("GSA ");
							NMEA_Str_Count = 0;
							USART2_State = NMEA_DATA;
						}
						else
							if (strstr(NMEA_Cmd_Str, "GPRMC"))
							{
								printf("RMC ");
								NMEA_Str_Count = 0;
								USART2_State = NMEA_DATA;
							}
							else USART2_State = NMEA_WAIT_SYNC;
				}
				break;
			case NMEA_DATA:
				if (ch != '*')
					if (NMEA_Str_Count < NMEA_DATA_MAX)
					{
						NMEA_Data_Str[NMEA_Str_Count++] = ch;
						NMEA_CheckSum ^= ch;
					}
					else USART2_State = NMEA_WAIT_SYNC;
				else
				{
					NMEA_Data_Str[NMEA_Str_Count] = '\0';
	//				printf(NMEA_Data_Str);
					USART2_State = NMEA_CRC1;
				}
				break;
			case NMEA_CRC1:
				if ((ch - '0') <= 9) CheckSum = (ch - '0') << 4;
				else CheckSum = (ch - 'A' + 10) << 4;
				USART2_State = NMEA_CRC2;
				break;
			case NMEA_CRC2:
				if ((ch - '0') <= 9) CheckSum |= (ch - '0');
				else CheckSum |= (ch - 'A' + 10);
				if (NMEA_CheckSum == CheckSum) printf(" Checksum OK!\n\r");
				else printf(" Checksum WRONG!\n\r");
				USART2_State = NMEA_WAIT_SYNC;
				break;
			default:
				USART2_State = NMEA_WAIT_SYNC;
		}
	}
	ResetFlags |= 0x80000000;
}
