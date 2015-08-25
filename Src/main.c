#include "main.h"

uint32_t SystemCoreClock; // System Clock Frequency (Core Clock) in Hz

//char cmd[40] = ("PMTK000\0");
char cmd[40] = ("PMTK605\0");
//char cmd[40] = ("PMTK604*30\r\n"); // unsupported EB-500
//	char cmd[50] = ("PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0\0");

int main (void) {
//	float Current_f, CurrentA_f, Voltage_f, Coulm_f;
//	uint16_t Voltage, Current, CurrentA, Coulm;
	uint16_t iled;

	SystemCoreClock = GetCoreClock();

	//	DBGMCU->CR |= DBGMCU_CR_DBG_STOP;

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;
	RTC->ISR &= ~RTC_ISR_WUTF;
	PWR->CR &= ~PWR_CR_DBP;
	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
//	EXTI->PR = EXTI_PR_PR20;

	TIM6_Init(); // Timer for Delay's function

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  for (iled=0; iled<=100; iled++) LED_GREEN_ON;
  if (StartUp_Error == 0) LED_GREEN_OFF;
  RCC->AHBENR &= ~RCC_AHBENR_GPIOBEN;

	printf("StartUp_Error=%X\r\n", StartUp_Error);
	RTC_Init();
	if ((ResetFlags & RTC_TIME_UPDATE) != RTC_TIME_UPDATE) RTC_SetupTime();
	
	GPS_Init();
//	Si446x_Init();
//	Si446x_BootUp();

//	Timer3_Init(); // Reconfig PA6 for Timer !!!
//	Timer4_Init();

  while (1)
	{
		Delay_mS(5000);
		GPS_Comm(&cmd[0]);
/*	I2C1_Init(GetAPB1Clock());
	I2C_ReceiveWord(0x36<<1, 0x09, &Voltage);
	I2C_ReceiveDWord(0x36<<1, 0x0A, &Current, &CurrentA);
	I2C_ReceiveWord(0x36<<1, 0x4D, &Coulm);
	I2C1_DeInit();
	Voltage = Voltage >> 3;
	Voltage_f = 0.000625 * (float)Voltage;
	Current_f = 0.003125 * (float)(int16_t)Current; // mA
	CurrentA_f = 0.003125 * (float)((int16_t)CurrentA); // mA
	Coulm_f = 0.01 * (float)Coulm;*/
//	printf("DEV_ID=%X REV_ID=%X ", (DBGMCU->IDCODE & DBGMCU_IDCODE_DEV_ID), (DBGMCU->IDCODE & DBGMCU_IDCODE_REV_ID) >> 16);
//	printf("%5.3f  %8.5f  %8.5f  %6.2f\r\n", Voltage_f, Current_f, CurrentA_f, Coulm_f);
//	printf("Time=%X\r\n", RTC->TR);

//	StandbyMCU();
/*	USART1_Init(2000000, 57600);
	printf("%5.3f  %8.5f  %8.5f  %6.2f\r\n", Voltage_f, Current_f, CurrentA_f, Coulm_f);
	USART1_DeInit();*/
	}
}
