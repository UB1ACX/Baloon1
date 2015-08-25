
#ifndef __RTC_H
#define __RTC_H

#include "stm32l1xx.h"

#define	PREDIV_A 100
#define PREDIV_S 370
#define RTC_WUTR 10

void RTC_Init(void);
void RTC_SetupTime(void);

#endif
