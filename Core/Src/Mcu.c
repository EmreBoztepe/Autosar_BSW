/* This file will init cpu clock pll etc..*/

#include "Mcu.h"



#define SET  1
#define FALSE 0


void Mcu_init(void)
{
	uint32_t tempReg = 0;

	tempReg = RCC_CR_HSEON;
	RCC->CR |= tempReg;

	tempReg = RCC_CFGR_SW_0;
	RCC->CFGR |= tempReg;

	tempReg = RCC_AHB1ENR_GPIODEN;
	RCC->AHB1ENR |= tempReg;
}
