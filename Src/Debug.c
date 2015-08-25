#define DWT_CYCCNT *(volatile uint32_t *)0xE0001004
#define DWT_CONTROL *(volatile uint32_t *)0xE0001000

//static void Debug_Init()
//{
//	CoreDebug->DHCSR |= CoreDebug_DHCSR_C_DEBUGEN_Msk | (0xA05FUL << CoreDebug_DHCSR_DBGKEY_Pos); // DEBUGEN
//	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; //enable TRCENA

//DWT_CYCCNT = 0;
//DWT_CONTROL|= 1; // enable the counter
	
//	TPI->CSPSR = 0x00000001;
//	TPI->FFCR  = 0x00000102;
//	TPI->SPPR  = 0x00000002;

//	DBGMCU->CR = 0x00000027;
	
//	ITM->LAR = 0xC5ACCE55;
//	ITM->TCR = 0x00010005;
//	ITM->TER = 0x00000001;
//	ITM->TPR = 0x00000001;
//}
