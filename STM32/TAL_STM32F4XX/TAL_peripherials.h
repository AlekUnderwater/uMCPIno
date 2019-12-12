// (C) Aleksandr Dikarev, 2015

#ifndef _TAL_PERIPHERIALS_H_
#define _TAL_PERIPHERIALS_H_

#include <TAL_pins.h>
#include <uMCP.h>

void TAL_RCC_ClockCmd(unsigned int rccID, TAL_RCC_BUS_ID_ENUM busID, FunctionalState state);
void TAL_TickTimer_Init(void);

void TAL_OutPin_Init(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, uint32_t GPIO_Clk, TAL_RCC_BUS_ID_ENUM busID);
void TAL_Pin_Switch(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, bool state);
void TAL_PinToggle(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin);

void TAL_InPin_Init(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, uint32_t GPIO_Clk, TAL_RCC_BUS_ID_ENUM busID);
void TAL_InPin_EXTI_Init(uint8_t portSource, uint8_t pinSource, uint32_t EXTILine, EXTITrigger_TypeDef EXTITrig, uint8_t IRQChannel);
bool TAL_InPin_GetState(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin);
void TAL_InPin_EXTI_DeInit(uint8_t portSource, uint8_t pinSource, uint32_t EXTILine, EXTITrigger_TypeDef EXTITrig, uint8_t IRQChannel);

void TAL_Init();

#endif
