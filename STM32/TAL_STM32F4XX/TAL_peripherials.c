// (C) Aleksandr Dikarev, Stanislav Dmitriev, 2014-2019

#include <TAL_peripherials.h>

NVIC_InitTypeDef                NVIC_InitStructure;
EXTI_InitTypeDef                EXTI_InitStructure;
GPIO_InitTypeDef                GPIO_InitStructure;
USART_InitTypeDef               USART_InitStructure;
DMA_InitTypeDef                 DMA_InitStructure;
TIM_TimeBaseInitTypeDef         TIM_TimeBaseStructure;

const int USART_BAUDRATES[DC_BAUDRATE_INVALID]              = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
const int USART_PARITIES[DC_PARITY_INVALID]                 = { USART_Parity_No, USART_Parity_Even, USART_Parity_Odd };
const int USART_STOPBITS[DC_STOPBITS_INVALID]               = { USART_StopBits_1, USART_StopBits_0_5, USART_StopBits_2, USART_StopBits_1_5 };
const int USART_WORDLENGTHS[DC_WORD_LENGTH_INVALID]         = { USART_WordLength_8b, USART_WordLength_9b };
const int USART_HW_CONTROLS[DC_HW_FLOW_CONTROL_INVALID]     = { USART_HardwareFlowControl_None, USART_HardwareFlowControl_RTS,
		                                                        USART_HardwareFlowControl_CTS, USART_HardwareFlowControl_RTS_CTS };

USART_TypeDef*            DC_ID[DC_INVALID]            = { USART1,                 USART2,                 USART3,                 UART4,                  UART5,                  USART6 };
const unsigned int        DC_DR[DC_INVALID]            = { USART1_DR_ADDRESS,      USART2_DR_ADDRESS,      USART3_DR_ADDRESS,      UART4_DR_ADDRESS,       UART5_DR_ADDRESS,       USART6_DR_ADDRESS };
const unsigned int        DC_RCC[DC_INVALID]           = { TAL_DC1_RCC,            TAL_DC2_RCC,            TAL_DC3_RCC,            TAL_DC4_RCC,            TAL_DC5_RCC,            TAL_DC6_RCC };
const TAL_RCC_BUS_ID_ENUM DC_BUS_ID[DC_INVALID]        = { TAL_DC1_BUS_ID,         TAL_DC2_BUS_ID,         TAL_DC3_BUS_ID,         TAL_DC4_BUS_ID,         TAL_DC5_BUS_ID,         TAL_DC6_BUS_ID };
GPIO_TypeDef*             DC_TX_PORT[DC_INVALID]       = { TAL_DC1_TX_GPIO_PORT,   TAL_DC2_TX_GPIO_PORT,   TAL_DC3_TX_GPIO_PORT,   TAL_DC4_TX_GPIO_PORT,   TAL_DC5_TX_GPIO_PORT,   TAL_DC6_TX_GPIO_PORT };
const unsigned short      DC_TX_PIN[DC_INVALID]        = { TAL_DC1_TX_GPIO_PIN,    TAL_DC2_TX_GPIO_PIN,    TAL_DC3_TX_GPIO_PIN,    TAL_DC4_TX_GPIO_PIN,    TAL_DC5_TX_GPIO_PIN,    TAL_DC6_TX_GPIO_PIN };
const unsigned char       DC_TX_PINSRC[DC_INVALID]     = { TAL_DC1_TX_PINSOURCE,   TAL_DC2_TX_PINSOURCE,   TAL_DC3_TX_PINSOURCE,   TAL_DC4_TX_PINSOURCE,   TAL_DC5_TX_PINSOURCE,   TAL_DC6_TX_PINSOURCE };
GPIO_TypeDef*             DC_RX_PORT[DC_INVALID]       = { TAL_DC1_RX_GPIO_PORT,   TAL_DC2_RX_GPIO_PORT,   TAL_DC3_RX_GPIO_PORT,   TAL_DC4_RX_GPIO_PORT,   TAL_DC5_RX_GPIO_PORT,   TAL_DC6_RX_GPIO_PORT };
const unsigned short      DC_RX_PIN[DC_INVALID]        = { TAL_DC1_RX_GPIO_PIN,    TAL_DC2_RX_GPIO_PIN,    TAL_DC3_RX_GPIO_PIN,    TAL_DC4_RX_GPIO_PIN,    TAL_DC5_RX_GPIO_PIN,    TAL_DC6_RX_GPIO_PIN };
const unsigned char       DC_RX_PINSRC[DC_INVALID]     = { TAL_DC1_RX_PINSOURCE,   TAL_DC2_RX_PINSOURCE,   TAL_DC3_RX_PINSOURCE,   TAL_DC4_RX_PINSOURCE,   TAL_DC5_RX_PINSOURCE,   TAL_DC6_RX_PINSOURCE };
const unsigned int        DC_PINS_RCC[DC_INVALID]      = { TAL_DC1_PINS_RCC,       TAL_DC2_PINS_RCC,       TAL_DC3_PINS_RCC,       TAL_DC4_PINS_RCC,       TAL_DC5_PINS_RCC,       TAL_DC6_PINS_RCC };
const TAL_RCC_BUS_ID_ENUM DC_PINS_BUS_ID[DC_INVALID]   = { TAL_DC1_PINS_BUS_ID,    TAL_DC2_PINS_BUS_ID,    TAL_DC3_PINS_BUS_ID,    TAL_DC4_PINS_BUS_ID,    TAL_DC5_PINS_BUS_ID,    TAL_DC6_PINS_BUS_ID };
const unsigned char       DC_AF[DC_INVALID]            = { TAL_DC1_GPIO_AF,        TAL_DC2_GPIO_AF,        TAL_DC3_GPIO_AF,        TAL_DC4_GPIO_AF,        TAL_DC5_GPIO_AF,        TAL_DC6_GPIO_AF };
const unsigned int        DC_TX_DMA_RCC[DC_INVALID]    = { TAL_DC1_TX_DMA_RCC,     TAL_DC2_TX_DMA_RCC,     TAL_DC3_TX_DMA_RCC,     TAL_DC4_TX_DMA_RCC,     TAL_DC5_TX_DMA_RCC,     TAL_DC6_TX_DMA_RCC };
const TAL_RCC_BUS_ID_ENUM DC_TX_DMA_BUS_ID[DC_INVALID] = { TAL_DC1_TX_DMA_BUS_ID,  TAL_DC2_TX_DMA_BUS_ID,  TAL_DC3_TX_DMA_BUS_ID,  TAL_DC4_TX_DMA_BUS_ID,  TAL_DC5_TX_DMA_BUS_ID,  TAL_DC6_TX_DMA_BUS_ID };
const unsigned int        DC_TX_DMA_CH[DC_INVALID]     = { TAL_DC1_TX_DMA_CHANNEL, TAL_DC2_TX_DMA_CHANNEL, TAL_DC3_TX_DMA_CHANNEL, TAL_DC4_TX_DMA_CHANNEL, TAL_DC5_TX_DMA_CHANNEL, TAL_DC6_TX_DMA_CHANNEL };
DMA_Stream_TypeDef*       DC_TX_DMA_STREAM[DC_INVALID] = { TAL_DC1_TX_DMA_STREAM,  TAL_DC2_TX_DMA_STREAM,  TAL_DC3_TX_DMA_STREAM,  TAL_DC4_TX_DMA_STREAM,  TAL_DC5_TX_DMA_STREAM,  TAL_DC6_TX_DMA_STREAM };
const unsigned int        DC_TX_DMA_IRQ[DC_INVALID]    = { TAL_DC1_TX_DMA_IRQ,     TAL_DC2_TX_DMA_IRQ,     TAL_DC3_TX_DMA_IRQ,     TAL_DC4_TX_DMA_IRQ,     TAL_DC5_TX_DMA_IRQ,     TAL_DC6_TX_DMA_IRQ };
const unsigned int        DC_RX_DMA_RCC[DC_INVALID]    = { TAL_DC1_RX_DMA_RCC,     TAL_DC2_RX_DMA_RCC,     TAL_DC3_RX_DMA_RCC,     TAL_DC4_RX_DMA_RCC,     TAL_DC5_RX_DMA_RCC,     TAL_DC6_RX_DMA_RCC };
const TAL_RCC_BUS_ID_ENUM DC_RX_DMA_BUS_ID[DC_INVALID] = { TAL_DC1_RX_DMA_BUS_ID,  TAL_DC2_RX_DMA_BUS_ID,  TAL_DC3_RX_DMA_BUS_ID,  TAL_DC4_RX_DMA_BUS_ID,  TAL_DC5_RX_DMA_BUS_ID,  TAL_DC6_RX_DMA_BUS_ID };
const unsigned int        DC_RX_DMA_CH[DC_INVALID]     = { TAL_DC1_RX_DMA_CHANNEL, TAL_DC2_RX_DMA_CHANNEL, TAL_DC3_RX_DMA_CHANNEL, TAL_DC4_RX_DMA_CHANNEL, TAL_DC5_RX_DMA_CHANNEL, TAL_DC6_RX_DMA_CHANNEL };
DMA_Stream_TypeDef*       DC_RX_DMA_STREAM[DC_INVALID] = { TAL_DC1_RX_DMA_STREAM,  TAL_DC2_RX_DMA_STREAM,  TAL_DC3_RX_DMA_STREAM,  TAL_DC4_RX_DMA_STREAM,  TAL_DC5_RX_DMA_STREAM,  TAL_DC6_RX_DMA_STREAM };
const unsigned int        DC_RX_DMA_IRQ[DC_INVALID]    = { TAL_DC1_RX_DMA_IRQ,     TAL_DC2_RX_DMA_IRQ,     TAL_DC3_RX_DMA_IRQ,     TAL_DC4_RX_DMA_IRQ,     TAL_DC5_RX_DMA_IRQ,     TAL_DC6_RX_DMA_IRQ };
const unsigned int        DC_RX_IRQ[DC_INVALID]        = { TAL_DC1_RX_IRQ,         TAL_DC2_RX_IRQ,         TAL_DC3_RX_IRQ,         TAL_DC4_RX_IRQ,         TAL_DC5_RX_IRQ,         TAL_DC6_RX_IRQ };

volatile bool IsDCLock[DC_INVALID];
volatile bool IsDCTxFinished[DC_INVALID];
unsigned char DC_TX_BUFFER[DC_INVALID][CR_DC_TX_BUFFER_SIZE];

void TAL_IWDG_Reload(void)
{
	IWDG_ReloadCounter();
}


unsigned int TAL_Ticks()
{
	return TAL_TICK_TIMER->CNT;
}

void TAL_TicksReset()
{
	TIM_Cmd(TAL_TICK_TIMER, DISABLE);
	TAL_TICK_TIMER->CNT = 0;
	TIM_Cmd(TAL_TICK_TIMER, ENABLE);
}

void TAL_Delay_ms(int delay_ms)
{
	if (delay_ms < 0) return;
	else
	{
		unsigned int tickTo = TAL_TICK_TIMER->CNT + delay_ms * TAL_TICKS_PER_MS;
		while (TAL_TICK_TIMER->CNT < tickTo) { __NOP(); }
	}
}


void TAL_DC_Config(DCID_Enum chID, DCParams_Struct* dataChannelParams)
{
	TAL_RCC_ClockCmd(DC_RCC[chID],        DC_BUS_ID[chID],        ENABLE);
	TAL_RCC_ClockCmd(DC_PINS_RCC[chID],   DC_PINS_BUS_ID[chID],   ENABLE);
	TAL_RCC_ClockCmd(DC_TX_DMA_RCC[chID], DC_TX_DMA_BUS_ID[chID], ENABLE);

	GPIO_PinAFConfig(DC_TX_PORT[chID], DC_TX_PINSRC[chID], DC_AF[chID]);
	GPIO_PinAFConfig(DC_RX_PORT[chID], DC_RX_PINSRC[chID], DC_AF[chID]);

    GPIO_InitStructure.GPIO_Pin                          = DC_TX_PIN[chID];
	GPIO_InitStructure.GPIO_Mode                         = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType                        = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd                         = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed                        = GPIO_Speed_25MHz;
	GPIO_Init(DC_TX_PORT[chID], &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin                          = DC_RX_PIN[chID];
	GPIO_InitStructure.GPIO_Mode                         = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType                        = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd                         = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed                        = GPIO_Speed_25MHz;
	GPIO_Init(DC_RX_PORT[chID], &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate                   = USART_BAUDRATES[dataChannelParams->Baudrate];
	USART_InitStructure.USART_WordLength                 = USART_WORDLENGTHS[dataChannelParams->Wordlength];
	USART_InitStructure.USART_StopBits                   = USART_STOPBITS[dataChannelParams->Stopbits];
	USART_InitStructure.USART_Parity                     = USART_PARITIES[dataChannelParams->Parity];
	USART_InitStructure.USART_HardwareFlowControl        = USART_HW_CONTROLS[dataChannelParams->HW_FlowControl];
	USART_InitStructure.USART_Mode                       = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(DC_ID[chID], &USART_InitStructure);

	USART_ITConfig(DC_ID[chID], USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel                   = DC_RX_IRQ[chID];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_InitStructure.DMA_Channel                        = DC_TX_DMA_CH[chID];
	DMA_InitStructure.DMA_PeripheralBaseAddr             = DC_DR[chID];
	DMA_InitStructure.DMA_Memory0BaseAddr                = (uint32_t)DC_TX_BUFFER[chID];
	DMA_InitStructure.DMA_DIR                            = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize                     = CR_DC_TX_BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc                  = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc                      = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize             = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize                 = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode                           = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority                       = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode                       = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold                  = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst                    = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst                = DMA_PeripheralBurst_Single;
	DMA_Init(DC_TX_DMA_STREAM[chID], &DMA_InitStructure);

	DMA_Cmd(DC_TX_DMA_STREAM[chID],      DISABLE);
	DMA_ITConfig(DC_TX_DMA_STREAM[chID], DMA_IT_TC,       ENABLE);
	USART_DMACmd(DC_ID[chID],            USART_DMAReq_Tx, ENABLE);

	USART_Cmd(DC_ID[chID], ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel                   = DC_TX_DMA_IRQ[chID];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TAL_DC_Write(DCID_Enum chID, unsigned char* data, unsigned int dataSize)
{
	while(dataSize--)
	{
		while (USART_GetFlagStatus(DC_ID[chID], USART_FLAG_TXE) == RESET) {};
		USART_SendData(DC_ID[chID], *data++);
	}
}

void TAL_DC_Write_Block(DCID_Enum chID, unsigned char* data, unsigned int dataSize)
{
	IsDCLock[chID] = true;
	IsDCTxFinished[chID] = false;
	int i;
	for (i = 0; i < dataSize; i++) { DC_TX_BUFFER[chID][i] = data[i]; }
	(DC_TX_DMA_STREAM[chID])->NDTR = dataSize;
	DMA_Cmd(DC_TX_DMA_STREAM[chID], ENABLE);
}

void TAL_DC_DeInit(DCID_Enum chID)
{

	USART_ITConfig(DC_ID[chID], USART_IT_RXNE, DISABLE);
	DMA_Cmd(DC_TX_DMA_STREAM[chID], DISABLE);
	DMA_ITConfig(DC_TX_DMA_STREAM[chID], DMA_IT_TC, DISABLE);
	USART_DMACmd(DC_ID[chID], USART_DMAReq_Tx, DISABLE);
	USART_Cmd(DC_ID[chID], DISABLE);
	TAL_RCC_ClockCmd(DC_RCC[chID], DC_BUS_ID[chID], DISABLE);
}


void TAL_RCC_ClockCmd(unsigned int rccID, TAL_RCC_BUS_ID_ENUM busID, FunctionalState state)
{
	switch (busID)
	{
		case TAL_RCC_BUS_ID_AHB1:
		{
			RCC_AHB1PeriphClockCmd(rccID, state);
			break;
		}
		case TAL_RCC_BUS_ID_AHB2:
		{
			RCC_AHB2PeriphClockCmd(rccID, state);
			break;
		}
		case TAL_RCC_BUS_ID_AHB3:
		{
			RCC_AHB3PeriphClockCmd(rccID, state);
			break;
		}
		case TAL_RCC_BUS_ID_APB1:
		{
			RCC_APB1PeriphClockCmd(rccID, state);
			break;
		}
		case TAL_RCC_BUS_ID_APB2:
		{
			RCC_APB2PeriphClockCmd(rccID, state);
			break;
		}
		default:
			break;
	}
}

void TAL_TickTimer_Init()
{
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TAL_RCC_ClockCmd(TAL_TICK_TIMER_RCC, TAL_TICK_TIMER_BUS_ID, ENABLE);
	TIM_TimeBaseStructure.TIM_Prescaler = TAL_TICK_TIMER_PRESCALER;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TAL_TICK_TIMER, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TAL_TICK_TIMER, ENABLE);
	TIM_Cmd(TAL_TICK_TIMER, ENABLE);
}

void TAL_IWDG_Init()
{

#ifdef CFG_TAL_IWDG_ENABLED


	DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;
	RCC_LSICmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) { }
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	IWDG_SetReload(0xFFF);
	IWDG_ReloadCounter();
	IWDG_Enable();

#endif

}


void TAL_OutPin_Init(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, uint32_t GPIO_Clk, TAL_RCC_BUS_ID_ENUM busID)
{
	TAL_RCC_ClockCmd(GPIO_Clk, busID, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_Port, &GPIO_InitStructure);
}

void TAL_Pin_Switch(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, bool state)
{
	if (state) GPIO_Port->BSRRL = GPIO_Pin;
	else GPIO_Port->BSRRH = GPIO_Pin;
}

void TAL_PinToggle(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin)
{
	GPIO_Port->ODR ^= GPIO_Pin;
}

void TAL_InPin_Init(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, uint32_t GPIO_Clk, TAL_RCC_BUS_ID_ENUM busID)
{
	TAL_RCC_ClockCmd(GPIO_Clk, busID, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // TODO: refactor

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_Init(GPIO_Port, &GPIO_InitStructure);
}

void TAL_InPin_EXTI_Init(uint8_t portSource, uint8_t pinSource, uint32_t EXTILine, EXTITrigger_TypeDef EXTITrig, uint8_t IRQChannel)
{
	SYSCFG_EXTILineConfig(portSource, pinSource);
	EXTI_InitStructure.EXTI_Line = EXTILine;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTITrig;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

bool TAL_InPin_GetState(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin)
{
	return (bool)GPIO_ReadInputDataBit(GPIO_Port, GPIO_Pin);
}

void TAL_InPin_EXTI_DeInit(uint8_t portSource, uint8_t pinSource, uint32_t EXTILine, EXTITrigger_TypeDef EXTITrig, uint8_t IRQChannel)
{
    SYSCFG_EXTILineConfig(portSource, pinSource);
    EXTI_InitStructure.EXTI_Line = EXTILine;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTITrig;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;

    NVIC_Init(&NVIC_InitStructure);
}



// IRQ Handlers

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		unsigned char newByte =  (unsigned char)USART_ReceiveData(USART1);
		uMCP_OnNewByte(DC_1, newByte);
	}
}

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		unsigned char newByte =  (unsigned char)USART_ReceiveData(USART2);
		uMCP_OnNewByte(DC_2, newByte);
	}
}

void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		unsigned char newByte =  (unsigned char)USART_ReceiveData(USART3);
		uMCP_OnNewByte(DC_3, newByte);
	}
}

void USART4_IRQHandler(void)
{
	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		unsigned char newByte =  (unsigned char)USART_ReceiveData(UART4);
		uMCP_OnNewByte(DC_4, newByte);
	}
}

void USART5_IRQHandler(void)
{
	if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		unsigned char newByte =  (unsigned char)USART_ReceiveData(UART5);
		uMCP_OnNewByte(DC_5, newByte);
	}
}

void USART6_IRQHandler(void)
{
	if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
	{
		unsigned char newByte =  (unsigned char)USART_ReceiveData(USART6);
		uMCP_OnNewByte(DC_6, newByte);
	}
}


// USART1 TX DMA transfer finished IRQ handler
void DMA2_Stream7_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) == SET)
    {
    	DMA_Cmd(DMA2_Stream7, DISABLE);
    	IsDCLock[DC_1] = false;
    	IsDCTxFinished[DC_1] = true;
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
    }
}

// USART3 TX DMA transfer finished IRQ handler
void DMA1_Stream3_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3) == SET)
    {
    	DMA_Cmd(DMA1_Stream3, DISABLE);
    	IsDCLock[DC_3] = false;
    	IsDCTxFinished[DC_3] = true;
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
    }
}

// USART4 TX DMA transfer finished IRQ handler
void DMA1_Stream4_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET)
    {
    	DMA_Cmd(DMA1_Stream4, DISABLE);
    	IsDCLock[DC_4] = false;
    	IsDCTxFinished[DC_4] = true;
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
    }
}

// USART5 TX DMA transfer finished IRQ handler
void DMA1_Stream0_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0) == SET)
    {
    	DMA_Cmd(DMA1_Stream0, DISABLE);
    	IsDCLock[DC_5] = false;
    	IsDCTxFinished[DC_5] = true;
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
    }
}

// USART6 TX DMA transfer finished IRQ handler
void DMA2_Stream6_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6) == SET)
    {
    	DMA_Cmd(DMA2_Stream6, DISABLE);
    	IsDCLock[DC_6] = false;
    	IsDCTxFinished[DC_6] = true;
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6);
    }
}


// Public

void TAL_Init()
{
	TAL_TickTimer_Init();
}

//
