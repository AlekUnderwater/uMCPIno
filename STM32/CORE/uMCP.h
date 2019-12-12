// (C) Aleksandr Dikarev, 2015

#ifndef _UMCP_H_
#define _UMCP_H_

#include <uMCP_defines.h>

extern volatile bool IsDCLock[DC_INVALID];
extern volatile bool IsDCTxFinished[DC_INVALID];

extern unsigned int  TAL_Ticks(void);

extern void          TAL_IWDG_Init(void);
extern void          TAL_IWDG_Reload(void);

extern void          TAL_DC_Config(DCID_Enum chID, DCParams_Struct* dcParams);
extern void          TAL_DC_Write(DCID_Enum chID, unsigned char* data, unsigned int size);
extern void          TAL_DC_Write_Block(DCID_Enum chID, unsigned char* data, unsigned int size);
extern void          TAL_DC_DeInit(DCID_Enum chID);

void uMCP_ITimer_Init(uMCP_Timer_ID timerID, unsigned int interval_ms, bool istate);
void uMCP_ITimer_StateSet(uMCP_Timer_ID timerID, bool value);
void uMCP_ITimer_Expired(uMCP_Timer_ID timerID);
void uMCP_ITimers_Process(void);

bool uMCP_IsByteInRangeExclusive(unsigned char st, unsigned char nd, unsigned char val);

void uMCP_SELECT_Set(bool value);
void uMCP_STATE_Set(uMCP_State value);

void uMCP_DataSend(bool isDTE, unsigned char tcnt, unsigned char rcnt, unsigned int rPos, unsigned int cnt, bool isStartTimer);
void uMCP_NextDataBlockSend(void);
void uMCP_DataBlockResend(unsigned char blockId, bool isDTE, bool isStartTimer);
void uMCP_CtrlSend(uMCP_PacketType ptype, unsigned char tcnt, unsigned char rcnt, bool isStartTimer);
void uMCP_AcknowledgeSentItems(int to);
void uMCP_Protocol_Perform(void);
void uMCP_OnIncomingPacket(void);
void uMCP_DC_Output_Process(void);

void uMCP_OnNewByte(DCID_Enum chID, unsigned char c);
void uMCP_PPNode_Init(void);
void uMCP_PPNode_Run(void);
#endif
