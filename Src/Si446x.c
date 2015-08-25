#include "Si446x.h"

#ifdef Si446x
	#if ((Si446x == Si4460) || (Si446x == Si4461) || (Si446x == Si4463) || (Si446x == Si4464))
//		printf("Used transceiver: %s", Si446x);
	#else
		#error Unknow transceiver (predefined: Si4460, Si4461, Si4463, Si4464)
	#endif
#else
	#error Variable Si446x not defined
#endif

#if ((REF_FREQ < 25000000) || (REF_FREQ > 32000000))
	#error Reference clock out of band
#endif

const	uint8_t PART_INFO_REPLY[] = {0x11, 0x44, 0x63, 0x00, 0x00, 0x0F, 0x00, 0x03};

int8_t FVCO_Div;
uint8_t PTT_State = TX_OFF;

/*--------------------------------------------------------------------------*/
void Si446x_Init (void) {
	uint8_t i;
	Si446x_TypeDef	Radio;

	/* SDN pin */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER4_0;
	Si446x_SDN_ON;

	SPI1_Init();

	Si446x_SDN_OFF;
	Delay_mS(4000);

	/* Test part info */
	Radio.PartInfoA.CMD = PART_INFO;
	if (SPI_SendCmd(1, 9, &Radio) != 0) { LED_GREEN_ON; while(1); };
	for (i = 0; i <= 7; i++)
		if (Radio.Stream[i] != PART_INFO_REPLY[i]) { LED_GREEN_ON; while(1); };
}

/*--------------------------------------------------------------------------*/
void Si446x_BootUp (void) {
//	uint8_t i;
	Si446x_TypeDef	Radio;

/* Wakeup Si446x by SDN pin */
//	for (i = 0; i < 100; i++) Si446x_SDN_OFF;

/* Boot-up */
	Radio.PowerUpA.CMD = POWER_UP;
	Radio.PowerUpA.BOOT_OPTIONS.PATCH = 0x00;
	Radio.PowerUpA.BOOT_OPTIONS.FUNC = 0x01;
	Radio.PowerUpA.XTAL_OPTIONS = TYPE_REF_OSC;
	Radio.PowerUpA.XO_FREQ = REF_FREQ;
	if (SPI_SendCmd(7, WAIT_READY_ONLY, &Radio) != 0) { LED_GREEN_ON; while(1); };

/* Config CLK prescaler */
	Radio.SetPropertyA.CMD = SET_PROPERTY;
	Radio.SetPropertyA.GROUP = GROUP_GLOBAL;
	Radio.SetPropertyA.START_PROPS = GLOBAL_CLK_CFG;
	Radio.SetPropertyA.NUM_PROPS = 1;
	Radio.SetPropertyA.DATA[0] = DIVIDED_CLK_EN_EN | DIVIDED_CLK_SEL_DIV_30 | CLK_32K_SEL_OFF;
	if (SPI_SendCmd(5, WAIT_READY_ONLY, &Radio) != 0 ) { LED_GREEN_ON; while(1); };

/* Set GPIO */
	Radio.GpioPinCfgA.CMD = GPIO_PIN_CFG;
	Radio.GpioPinCfgA.GPIO[0].GPIO_MODE = 28; // Pull-Up + input (TX Data)
	Radio.GpioPinCfgA.GPIO[1].GPIO_MODE = 7;	// output divided clock signal
	Radio.GpioPinCfgA.GPIO[2].GPIO_MODE = 33;	// RX_STATE (=1)
	Radio.GpioPinCfgA.GPIO[3].GPIO_MODE = 32;	// TX_STATE (=1)
	Radio.GpioPinCfgA.NIRQ.NIRQ_MODE = 39;		// nIRQ (=0)
	Radio.GpioPinCfgA.SDO.SDO_MODE = 7; //11;			// SDO
	Radio.GpioPinCfgA.GEN_CONFIG.DRV_STRENGTH = 0;
	if (SPI_SendCmd(8, WAIT_READY_ONLY, &Radio) != 0 ) { LED_GREEN_ON; while(1); };
}

/*--------------------------------------------------------------------------*/
uint16_t Si446x_SetFreq(uint32_t Freq) {
/*
	Input: Freq [Hz] - required frequency
	Output: Freq [Hz] - actual frequency
	Output func:	0 - no error;
								1 - frequency in range
								2 - wrong INTE/FRAC
								3 - frequency change denided
								65535 - unknow error
*/

	uint8_t	Freq_Integer;
	uint32_t	Freq_Fractional;
	unsigned long long Temp;
	Si446x_TypeDef	Radio;

	if (PTT_State == TX_ON) return (5);

#if Si446x != Si4464
	if 			((Freq >= 142000000) && (Freq <=  175000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_24;
	else if ((Freq >= 284000000) && (Freq <=  350000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_12;
	else if	((Freq >= 420000000) && (Freq <=  525000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_8;
	else if ((Freq >= 850000000) && (Freq <= 1050000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_4;
	else return (1);	
#else
	if 			((Freq >= 119000000) && (Freq <= 159000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_24;
	else if ((Freq >= 177000000) && (Freq <= 237000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_16;
	else if	((Freq >  237000000) && (Freq <= 319000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_12;
	else if	((Freq >= 353000000) && (Freq <= 475000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_8;
	else if	((Freq >  475000000) && (Freq <= 639000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_6;
	else if ((Freq >= 705000000) && (Freq <= 960000000)) Radio.SetPropertyA.DATA[0] = FVCO_DIV_4;
	else return (1);	
#endif

	switch (Radio.SetPropertyA.DATA[0]) {
		case FVCO_DIV_4:		FVCO_Div = 4; break;
		case FVCO_DIV_6:		FVCO_Div = 6; break;
		case FVCO_DIV_8:		FVCO_Div = 8; break;
		case FVCO_DIV_12:		FVCO_Div = 12; break;
		case FVCO_DIV_16:		FVCO_Div = 16; break;
		case FVCO_DIV_24:		FVCO_Div = 24; break;
		case FVCO_DIV_24_1:	FVCO_Div = 24; break;
		case FVCO_DIV_24_2:	FVCO_Div = 24; break;
		default: return (UNKNOW_ERROR);	}

	Temp = Freq * FVCO_Div;
	Freq_Integer = (Temp / (NPRESC * REF_FREQ)) - 1;
	Temp = ((Temp * 524288) / (NPRESC * REF_FREQ)) - (Freq_Integer * 524288);
	Freq_Fractional = (uint32_t)Temp;
	if ((Freq_Integer > 127) || (Freq_Fractional < 524288) || (Freq_Fractional > 1048575)) return (2);

/* Frequency band */
	Radio.SetPropertyA.CMD = SET_PROPERTY;
	Radio.SetPropertyA.GROUP = GROUP_MODEM;
	Radio.SetPropertyA.START_PROPS = MODEM_CLKGEN_BAND;
	Radio.SetPropertyA.NUM_PROPS = 1;
	Radio.SetPropertyA.DATA[0] |= (SY_SEL << 3);
	if (SPI_SendCmd(5, WAIT_READY_ONLY, &Radio) != 0 ) { LED_GREEN_ON; while(1); };

/* Set frequency */
	Radio.SetPropertyA.CMD = SET_PROPERTY;
	Radio.SetPropertyA.GROUP = GROUP_FREQ_CONTROL;
	Radio.SetPropertyA.START_PROPS = FREQ_CONTROL_INTE; // + FREQ_CONTROL_FRAC
	Radio.SetPropertyA.NUM_PROPS = 1 + 3;
	Radio.SetPropertyA.DATA[0] = Freq_Integer;		// INTE
	Radio.SetPropertyA.DATA[1] = (Freq_Fractional >> 16) & 0xFF;		// FRAC (high)
	Radio.SetPropertyA.DATA[2] = (Freq_Fractional >>  8) & 0xFF;		// FRAC (mid)
	Radio.SetPropertyA.DATA[3] = (Freq_Fractional >>  0) & 0xFF;		// FRAC (low)
	if (SPI_SendCmd(8, NO_WAIT_READY, &Radio) != 0 ) { LED_GREEN_ON; while(1); };

	return (NO_ERROR);
}

/*--------------------------------------------------------------------------*/
void Si446x_FreqCorr (void) {

	Si446x_TypeDef	Radio;

	/* Set GLOBAL_CLK_CFG */
	Radio.SetPropertyA.CMD = SET_PROPERTY;
	Radio.SetPropertyA.GROUP = GROUP_GLOBAL;
	Radio.SetPropertyA.START_PROPS = GLOBAL_CLK_CFG;
	Radio.SetPropertyA.NUM_PROPS = 1;
	Radio.SetPropertyA.DATA[0] = (DIVIDED_CLK_EN_DIS | DIVIDED_CLK_SEL_DIV_15	| CLK_32K_SEL_OFF);
	if (SPI_SendCmd(5, WAIT_READY_ONLY, &Radio) != 0 ) { LED_GREEN_ON; while(1); };
}

uint16_t Si446x_Offset (int16_t Freq) {
	Si446x_TypeDef	Radio;
	signed long long Temp;

	Temp = 524288 * FVCO_Div * Freq;
	Temp = Temp / (NPRESC * REF_FREQ);
	if ((Temp <= 32767) && (Temp >= -32768)) Freq = (uint16_t)Temp;
	else return (1);

/* Frequency offset */
	Radio.SetPropertyA.CMD = SET_PROPERTY;
	Radio.SetPropertyA.GROUP = GROUP_MODEM;
	Radio.SetPropertyA.START_PROPS = MODEM_FREQ_OFFSET;
	Radio.SetPropertyA.NUM_PROPS = 2;
	Radio.SetPropertyA.DATA[0] = (Freq >> 8);		// high
	Radio.SetPropertyA.DATA[1] = (Freq & 0xFF);		// low
	if (SPI_SendCmd(6, WAIT_READY_ONLY, &Radio) != 0 ) { LED_GREEN_ON; while(1); };
	
	return (NO_ERROR);
}

/*--------------------------------------------------------------------------*/
/* Set GLOBAL_XO_TUNE */
/*	if ((ad_val >> 5) != ad_val_old) {
		Radio.SetPropertyA.CMD = 0x11;
		Radio.SetPropertyA.GROUP = 0x00;
		Radio.SetPropertyA.START_PROPS = 0x00;
		Radio.SetPropertyA.NUM_PROPS = 0x01;
		Radio.SetPropertyA.DATA[0] = (ad_val >> 5);
		ad_val_old = (ad_val >> 5);
		if (SPI1_SendCmd(5, WAIT_READY_ONLY, &Radio) != 0 ) { LED_RED_ON; while(1); };
    sprintf(text, "0x%02X", (ad_val >> 5));
    GLCD_DisplayString(2,  9, __FI,  (unsigned char *)text);
	};*/

/* Get state radio */
/*	uint16_t ad_val_old = 0x0040;
	Radio.ReqDevStateA.CMD = 0x33;
	if (SPI_SendCmd(1, 3, &Radio) != 0 ) { LED_RED_ON; while(1); };
	switch (Radio.ReqDevStateR.CURR_STATE.MAIN_STATE) {
		case 0x01:
			sprintf(resp, "State: SLEEP");
			break;
		case 0x02:
			sprintf(resp, "State: SPI_ACTIVE");
			break;
		case 0x03:
			sprintf(resp, "State: READY");
			break;
		case 0x04:
			sprintf(resp, "State: READY2");
			break;
		case 0x05:
			sprintf(resp, "State: TX_TUNE");
			break;
		case 0x06:
			sprintf(resp, "State: RX_TUNE");
			break;
		case 0x07:
			sprintf(resp, "State: TX");
			break;
		case 0x08:
			sprintf(resp, "State: RX");
			break;
		default:
			sprintf(resp, "State: Unknow");
			LED_RED_ON;
			while(1);
	}
	GLCD_SetTextColor(Green);
	GLCD_DisplayString(1, 0, __FI, (unsigned char *)resp);
*/
