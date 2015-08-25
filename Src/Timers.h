#ifndef __Timers_H
#define __Timers_H

#include <stdio.h>
#include "Serial.h"
#include "stm32l1xx.h"

void Timer2_Init(void);
void TIM2_IRQHandler(void);
void Timer3_Init(void);
void Timer4_Init(void);
void TIM4_IRQHandler(void);
void TIM6_Init(void);
void Delay_uS(uint32_t Delay);
void Delay_mS(uint32_t Delay);

#endif
