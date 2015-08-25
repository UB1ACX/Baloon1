/*----------------------------------------------------------------------------
 * Name:    Si446x.h
 * Purpose: Si446x transceiver API and properties definitions
 * Note(s):
 *----------------------------------------------------------------------------*/

#ifndef __Si446x_H
#define __Si446x_H

#include "stm32l1xx.h"
#include "Si446x_def.h"
#include "SPI.h"
#include "Timers.h"

#define Si4460	4460
#define Si4461	4461
#define	Si4463	4463
#define	Si4464	4464

#define Si446x				Si4463		// Si4460/Si4461/Si4463/Si4464
#define TYPE_REF_OSC	XTAL			// XTAL/TCXO
#define REF_FREQ			30000000	// [Hz]
#define SY_SEL				Div_by_2	// See MODEM_CLKGEN_BAND properties

#define Si446x_SDN			PB4
#define Si446x_nIRQ			PA1
#define Si446x_nSEL			PD2
#define Si446x_SPI			SPI1

#define Si446x_SDN_ON		GPIOB->BSRRL	= (1UL << 4) // Shutdown Si446x
#define Si446x_SDN_OFF	GPIOB->BSRRH	= (1UL << 4) // Wakeup Si446x

void Si446x_Init(void);
void Si446x_BootUp(void);
uint16_t Si446x_SetFreq(uint32_t Freq);
uint16_t Si446x_Offset(int16_t Freq);

extern int8_t FVCO_Div;
extern uint8_t PTT_State;

#define TX_OFF	0
#define TX_ON		1

#define NO_WAIT_READY		0
#define WAIT_READY_ONLY	1

#define NO_ERROR			0
#define UNKNOW_ERROR	65535

#endif
