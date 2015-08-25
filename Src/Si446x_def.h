/*----------------------------------------------------------------------------
 * Name:    Si446x_def.h
 * Purpose: Si446x transceiver evalution definitions
 * Note(s):
 *----------------------------------------------------------------------------*/

#ifndef __Si446x_def_H
#define __Si446x_def_H

#include "stdint.h"

/*-----------------------------------------------------------------------------
 * API definitions
 *---------------------------------------------------------------------------*/

#define	NOP						(uint16_t)0x00

#define	PART_INFO			(uint16_t)0x01

#define	POWER_UP			(uint16_t)0x02
#define XTAL 0
#define TCXO 1

#define SET_PROPERTY	(uint16_t)0x11
#define GET_PROPERTY	(uint16_t)0x12
#define GPIO_PIN_CFG	(uint16_t)0x13

#define READ_CMD_BUFF (uint16_t)0x44

#define CTS (uint16_t)0xFF

typedef struct {
	uint8_t CMD;
} nop_as, part_info_as, func_info_as, request_device_state_as;

typedef struct {
	uint8_t		CHIPREV;
	uint16_t	PART;
	uint8_t		PBUILD;
	uint16_t	ID;
	uint8_t		CUSTOMER;
	uint8_t		ROMID;
} part_info_rs;

typedef struct {
	uint8_t	CMD;
	struct {
		uint8_t	FUNC	:6;
		uint8_t				:1;
		uint8_t	PATCH	:1;
	} BOOT_OPTIONS;
	uint8_t		XTAL_OPTIONS;
	uint32_t	XO_FREQ;
} power_up_as;

typedef struct {
	uint8_t	REVEXT;
	uint8_t	REVBRANCH;
	uint8_t	REVINT;
	uint8_t	:8;
	uint8_t	:8;
	uint8_t	FUNC;
} func_info_rs;

typedef struct {
	uint8_t		CMD;
	uint8_t		GROUP;
	uint8_t		NUM_PROPS;
	uint8_t		START_PROPS;
	uint8_t		DATA[12];
} set_property_as;

typedef struct {
	uint8_t		CMD;
	uint8_t		GROUP;
	uint8_t		NUM_PROPS;
	uint8_t		START_PROPS;
} get_property_as;

typedef struct {
	uint8_t		DATA[16];
} get_property_rs;

typedef struct {
	uint8_t	CMD;
	struct {
		uint8_t	GPIO_MODE	:6;
		uint8_t	PULL_CTL	:1;
		uint8_t						:1;
	} GPIO[4];
	struct {
		uint8_t	NIRQ_MODE	:6;
		uint8_t	PULL_CTL	:1;
		uint8_t						:1;
	} NIRQ;
	struct {
		uint8_t	SDO_MODE	:6;
		uint8_t	PULL_CTL	:1;
		uint8_t						:1;
	} SDO;
	struct {
		uint8_t								:5;
		uint8_t	DRV_STRENGTH	:2;
		uint8_t								:1;
	} GEN_CONFIG;
} gpio_pin_cfg_as;

typedef struct {
	struct {
		uint8_t	GPIO_MODE		:6;
		uint8_t							:1;
		uint8_t	GPIO_STATE	:1;
	} GPIO[4];
	struct {
		uint8_t	NIRQ_MODE		:6;
		uint8_t							:1;
		uint8_t	NIRQ_STATE	:1;
	} NIRQ;
	struct {
		uint8_t	SDO_MODE		:6;
		uint8_t							:1;
		uint8_t	SDO_STATE		:1;
	} SDO;
	struct {
		uint8_t								:5;
		uint8_t	DRV_STRENGTH	:2;
		uint8_t								:1;
	} GEN_CONFIG;
} gpio_pin_cfg_rs;

typedef struct {
	uint8_t		CMD;
	struct {
		uint8_t	TX	:1;
		uint8_t	RX	:1;
		uint8_t			:6;
	} FIFO;
} fifo_info_as;

typedef struct {
	uint8_t	RX_FIFO_COUNT;
	uint8_t	TX_FIFO_SPACE;
} fifo_info_rs;

typedef struct {
	uint8_t		CMD;
	uint8_t		CHANNEL;
	struct {
		uint8_t	START							:2;
		uint8_t	RETRANSMIT				:1;
		uint8_t										:1;
		uint8_t	TXCOMPLETE_STATE	:4;
	} CONDITION;
	uint16_t	TX_LEN;
} start_tx_as;

typedef struct {
	struct {
		uint8_t	MAIN_STATE	:4;
		uint8_t							:4;
	} CURR_STATE;
	uint8_t	CURRENT_CHANNEL;
} request_device_state_rs;

typedef struct {
	uint8_t	CMD;
	struct {
		uint8_t	NEW_STATE	:4;
		uint8_t						:4;
	} NEXT_STATE1;
} change_state_as;

typedef union {
  uint8_t									Stream[16];
	nop_as									NopA;						// 0x00
	part_info_as						PartInfoA;			// 0x01
	part_info_rs						PartInfoR;
	power_up_as							PowerUpA;				// 0x02
	func_info_as						FuncInfoA;			// 0x10
	func_info_rs						FuncInfoR;
	set_property_as					SetPropertyA;		// 0x11
	get_property_as					GetPropertyA;		// 0x12
	get_property_rs					GetPropertyR;
	gpio_pin_cfg_as					GpioPinCfgA;		// 0x13
	gpio_pin_cfg_rs					GpioPinCfgR;
	fifo_info_as						FifoInfoA;			// 0x15
	fifo_info_rs						FifoInfoR;
	start_tx_as							StartTxA;				// 0x31
	request_device_state_as	ReqDevStateA;		// 0x33
	request_device_state_rs	ReqDevStateR;
	change_state_as					ChangeStateA;		// 0x34
} Si446x_TypeDef;

/*-----------------------------------------------------------------------------
 * Properties definitions
 *---------------------------------------------------------------------------*/

/*----------------- GLOBAL---------------------------------------------------*/
#define GROUP_GLOBAL 0x00

/* GLOBAL_CLK_CFG */
#define GLOBAL_CLK_CFG 			0x01

#define DIVIDED_CLK_EN_MSK	0x40

#define DIVIDED_CLK_EN_DIS	0x00
#define DIVIDED_CLK_EN_EN		0x40

#define DIVIDED_CLK_SEL_MSK	0x38

#define DIVIDED_CLK_SEL_DIV_1		0x00
#define DIVIDED_CLK_SEL_DIV_2		0x08
#define DIVIDED_CLK_SEL_DIV_3		0x10
#define DIVIDED_CLK_SEL_DIV_7_5	0x18
#define DIVIDED_CLK_SEL_DIV_10	0x20
#define DIVIDED_CLK_SEL_DIV_15	0x28
#define DIVIDED_CLK_SEL_DIV_30	0x30

#define CLK_32K_SEL_MSK			0x03

#define CLK_32K_SEL_OFF			0x00
#define CLK_32K_SEL_RC			0x01
#define CLK_32K_SEL_CRYSTAL	0x02

/*----------------- MODEM ---------------------------------------------------*/
#define GROUP_MODEM 0x20

/* MODEM_FREQ_OFFSET */
#define MODEM_FREQ_OFFSET 0x0D

/* MODEM_CLKGEN_BAND */
#define MODEM_CLKGEN_BAND 0x51
#define MODEM_SY_SEL_MSK	0x08
#define MODEM_SY_SEL_POS	3
#define MODEM_BAND_MSK		0x07

#define Div_by_4			0
#define Div_by_2			1
#define NPRESC				(4 - (2 * SY_SEL))

#define FVCO_DIV_4		0
#define FVCO_DIV_6		1
#define FVCO_DIV_8		2
#define FVCO_DIV_12		3
#define FVCO_DIV_16		4
#define FVCO_DIV_24		5
#define FVCO_DIV_24_1	6
#define FVCO_DIV_24_2	7

/*----------------- FREQ_CONTROL --------------------------------------------*/
#define GROUP_FREQ_CONTROL 0x40

#define FREQ_CONTROL_INTE 0x00
#define FREQ_CONTROL_FRAC 0x01

#endif
