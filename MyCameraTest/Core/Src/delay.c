#include "delay.h"
extern TIM_HandleTypeDef htim1;


void us_delay(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < us); //May want to change this to enable an interrupt, so that we aren't blocking. 1us = 48 sys clk cycles
}

void delay_ms(uint16_t ms)
{
	ms *= 1000;
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < ms);
}
