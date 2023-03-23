/*
 * Mcu.h
 *
 *  Created on: 12 Şub 2023
 *      Author: Emre
 */
#include "stm32f407xx.h"


typedef enum
{
	HSI_SOURCE,
	HSE_SOURCE,
	PLL_SOURCE
}e_clockSource;
typedef struct
{
	uint32_t McuClockReferencePointFrequency;
	uint16_t PllN;
	uint8_t PllQ;
	uint8_t PllP;
	uint8_t PllM;

} Mcu_ClockSettingConfigType;


typedef struct
{
	Mcu_ClockSettingConfigType *clockConfig;
	uint8_t	clockSource;
} Mcu_ConfigType;

extern Mcu_ConfigType McuConfig;
#ifndef CORE_INC_MCU_H_
#define CORE_INC_MCU_H_

void Mcu_init(Mcu_ConfigType *configPtr);



#endif /* CORE_INC_MCU_H_ */
