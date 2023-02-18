/*
 * Port.c
 *
 *  Created on: 18 Şub 2023
 *      Author: Emre
 */

#include "Port.h"

#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */

void Port_Init (void)
{

	GPIOD->MODER |= GPIO_MODER_MODER13_0;
	GPIOD->MODER |= GPIO_MODER_MODER14_0;
	GPIOD->MODER |= GPIO_MODER_MODER15_0;

	GPIOD->BSRR |= GPIO_PIN_13;
	GPIOD->BSRR |= GPIO_PIN_14;
	GPIOD->BSRR |= GPIO_PIN_15;
}

