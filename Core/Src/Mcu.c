/* This file will init cpu clock pll etc..*/

#include "Mcu.h"



#define TRUE  1
#define FALSE 0


void Mcu_init(void)
{
	RCC->CR = RCC_CR_PLLON & TRUE;
	RCC->CR = RCC_CR_HSEON & TRUE;
}
