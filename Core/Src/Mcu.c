/* This file will init cpu clock pll etc..*/

#include "Mcu.h"




// Main PLL = N * (source_clock / M) / P
// HSE = 8 Mhz
// fCLK =   N * (8Mhz / M) / P
#define PLL_M   8
#define PLL_Q   7
#define PLL_P   2


/* stm32f407 runs at 168Mhz max */

#define PLL_N   336


void Mcu_init(void)
{

	uint32_t tempReg = 0;
	/*
	//RCC->PLLCFGR &= tempReg;

	tempReg = RCC_PLLCFGR_PLLM_3;	//8; // PLLM = 8
	RCC->PLLCFGR &= tempReg;

	tempReg = RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_8;
	RCC->PLLCFGR |= tempReg;

	tempReg = RCC_PLLCFGR_PLLP_1;
	RCC->PLLCFGR |= tempReg;

	tempReg = RCC_CR_HSEON;
	RCC->CR |= tempReg;

	tempReg = RCC_CR_PLLON;
	RCC->CR |= tempReg;

	tempReg = 0x02;
	RCC->CFGR |= tempReg;

	tempReg = RCC_AHB1ENR_GPIODEN;
	RCC->AHB1ENR |= tempReg; */

	/* Enable HSE (CR: bit 16) */
	RCC->CR |= (1U << 16);
	/* Wait till HSE is ready (CR: bit 17) */
	while(!(RCC->CR & (1 << 17)));

	/* Enable power interface clock (APB1ENR:bit 28) */			///olmasada olur
	RCC->APB1ENR |= (1 << 28);

	/* set voltage scale to 1 for max frequency (PWR_CR:bit 14)
	 * (0b0) scale 2 for fCLK <= 144 Mhz
	 * (0b1) scale 1 for 144 Mhz < fCLK <= 168 Mhz
	 */
	PWR->CR |= (1 << 14);

	/* set AHB prescaler to /1 (CFGR:bits 7:4) */
	RCC->CFGR |= (0 << 4);
	/* set APB low speed prescaler to /4 (APB1) (CFGR:bits 12:10) */
	RCC->CFGR |= (5 << 10);
	/* set APB high speed prescaler to /2 (APB2) (CFGR:bits 15:13) */
	RCC->CFGR |= (4 << 13);

	/* Set M, N, P and Q PLL dividers
	 * PLLCFGR: bits 5:0 (M), 14:6 (N), 17:16 (P), 27:24 (Q)
	 * Set PLL source to HSE, PLLCFGR: bit 22, 1:HSE, 0:HSI
	 */
	RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
	               (PLL_Q << 24) | (1 << 22);
	/* Enable the main PLL (CR: bit 24) */
	RCC->CR |= (1 << 24);
	/* Wait till the main PLL is ready (CR: bit 25) */
	while(!(RCC->CR & (1 << 25)));
	/* Configure Flash
	 * prefetch enable (ACR:bit 8)
	 * instruction cache enable (ACR:bit 9)
	 * data cache enable (ACR:bit 10)
	 * set latency to 5 wait states (ARC:bits 2:0)
	 *   see Table 10 on page 80 in RM0090
	 */
	FLASH->ACR = (1 << 8) | (1 << 9) | (1 << 10 ) | (5 << 0);

	/* Select the main PLL as system clock source, (CFGR:bits 1:0)
	 * 0b00 - HSI
	 * 0b01 - HSE
	 * 0b10 - PLL
	 */
	RCC->CFGR &= ~(3U << 0);
	RCC->CFGR |= (2 << 0);
	/* Wait till the main PLL is used as system clock source (CFGR:bits 3:2) */
	while (!(RCC->CFGR & (2U << 2)));

	tempReg = RCC_AHB1ENR_GPIODEN;
		RCC->AHB1ENR |= tempReg;
}
