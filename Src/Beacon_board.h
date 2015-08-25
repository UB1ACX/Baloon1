
#ifndef __BEACON_BOARD_H
#define __BEACON_BOARD_H

/* Includes ------------------------------------------------------------------*/

#define bool _Bool
#define FALSE 0
#define TRUE (!FALSE)

#define	HSE_VALUE	((uint32_t)16000000) // [Hz]

/* LED definitions */
#define	LED_GREEN		14

#define LED_GREEN_ON		GPIOB->BSRRL = (1UL << LED_GREEN)
#define LED_GREEN_OFF		GPIOB->BSRRH = (1UL << LED_GREEN)
#define LED_GREEN_TGL		GPIOB->ODR ^= (1UL << LED_GREEN)

/* GPS definitions */

#define	GPS_BAUDRATE	9600

#define	GPS_POWER		5

#define GPS_POWER_ON		GPIOB->BSRRH = (1UL << GPS_POWER)
#define GPS_POWER_OFF		GPIOB->BSRRL = (1UL << GPS_POWER)

/* MACROs for SET, RESET or TOGGLE Output port */

#define GPIO_HIGH(a,b) 		a->BSRRL = b
#define GPIO_LOW(a,b)			a->BSRRH = b
#define GPIO_TOGGLE(a,b) 	a->ODR ^= b 

#endif
