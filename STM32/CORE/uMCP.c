/*
    (C) Aleksandr Dikarev, 2018-2019
    aleksandr.v.dikarev@gmail.com

    uMCP Ino - a lightweight byte-oriented point-to-point protocol with handshake, integrity control,
    guaranteed packets ordering and delivery.

    Message framing:

Offset
   0         SIGN = 0xAD: 8 bit - constant, packet start signature
   1         PTYPE : 8 bit - packet type. Can be one of the uMCP_PacketType
   2         TCNT  : 4 bit - number of last transmitted packet
   2         RCNT  : 4 bit - number of last received packet
   3         HCHK  : 8 bit - header checksum

             if PTYPE == uMCP_PTYPE_DTA || PTYPE == uMCP_PTYPE_DTE
   4           DCNT  : 8 bit - data packet size, should be from a range [1..64] inclusively, 0 is not valid value for this field
   ..          DATA  : 8 * DCNT bits - data
   5 + DCNT    DCHK  : 8 bit - packet checksum
             endif

   There are two types of nodes in the protocol: Selected by default (Master) and unselected by default (tributary).
   All control messages transfer the SELECT flag. Only DTE (Date end packet) data messages transfer the SELECT flag.
   A node can transmit only if has SELECT flag set to true.
   If master node has transfered the SELECT flag and it has not returned back, the node will regain it by timeout.
*/

#include <uMCP.h>

DCParams_Struct dcParams;

// buffer from HOST
unsigned char ih_ring[UMCP_IH_RING_SIZE];
volatile unsigned int ih_rPos = 0;
volatile unsigned int ih_wPos = 0;
volatile unsigned int ih_Cnt = 0;
volatile unsigned int ih_TS = 0;

// buffer to HOST
unsigned char oh_buffer[UMCP_NBUFFER_SIZE];
unsigned int oh_cnt = 0;
bool oh_ready = false;

// buffer to LINE
unsigned char ol_buffer[UMCP_MAX_PACKET_SIZE];
unsigned int ol_cnt = 0;
bool ol_ready = false;

// incoming packet
unsigned char ip_datablock[UMCP_PACKET_DATA_SIZE];
volatile bool ip_start = false;
volatile int ip_pos = 0;
volatile uMCP_PacketType ip_type = uMCP_PTYPE_INVALID;
volatile unsigned char ip_tcnt = 0;
volatile unsigned char ip_rcnt = 0;
volatile unsigned char ip_ahchk = 0;
volatile unsigned char ip_dcnt = 0;
volatile bool ip_ready = false;

unsigned char R, N, A;
bool selectDefaultState = CFG_SELECT_DEFAULT_STATE;
bool select = CFG_SELECT_DEFAULT_STATE;
uMCP_PacketType pType = uMCP_PTYPE_INVALID;
uMCP_State state = uMCP_STATE_HALTED;
bool sack = false;
bool srep = false;
bool isTimerPendingOnTxFinish = false;
int lineBaudrate = CFG_LINE_BAUDRATE_BPS;

unsigned int iTimer_Interval_tks[uMCP_Timer_INVALID];
unsigned int iTimer_ExpTime_tks[uMCP_Timer_INVALID];
bool iTimer_State[uMCP_Timer_INVALID];

unsigned char sentBlocksSize[UMCP_PACKETS_NUMBER];
unsigned int sentBlocksRPos[UMCP_PACKETS_NUMBER];
unsigned char sentBlocksCnt = 0;

bool isStartup = true;

// uMCP

// Interval timers management routines
void uMCP_ITimer_Init(uMCP_Timer_ID timerID, unsigned int interval_ms, bool istate)
{
	iTimer_Interval_tks[timerID] = MS2TKS(interval_ms);
	uMCP_ITimer_StateSet(timerID, istate);
}

void uMCP_ITimer_StateSet(uMCP_Timer_ID timerID, bool value)
{
	if (value)
	{
		iTimer_ExpTime_tks[timerID] = TAL_Ticks() + iTimer_Interval_tks[timerID];
		iTimer_State[timerID] = true;
	}
	else
	{
		iTimer_State[timerID] = false;
	}
}

void uMCP_ITimer_Expired(uMCP_Timer_ID timerID)
{
	if (timerID == uMCP_Timer_TMO)
	{
		uMCP_ITimer_StateSet(uMCP_Timer_TX, false);
		if (state == uMCP_STATE_ISTART)
		{
			uMCP_CtrlSend(uMCP_PTYPE_STR, 0, 0, true);
		}
		else if (state == uMCP_STATE_ASTART)
		{
			uMCP_CtrlSend(uMCP_PTYPE_STA, 0, 0, true);
		}
		else if (state == uMCP_STATE_RUNNING)
		{
			srep = true;
		}
	}
	else if (timerID == uMCP_Timer_SELECT)
	{
		uMCP_SELECT_Set(CFG_SELECT_DEFAULT_STATE);
	}
	else if (timerID == uMCP_Timer_TX)
	{
		if (isTimerPendingOnTxFinish && !iTimer_State[uMCP_Timer_TMO])
		{
			isTimerPendingOnTxFinish = false;
			uMCP_ITimer_StateSet(uMCP_Timer_TMO, true);
		}
	}
}

void uMCP_ITimers_Process()
{
	int tIdx = 0;
	for (tIdx = 0; tIdx < uMCP_Timer_INVALID; tIdx++)
	{
		if (iTimer_State[tIdx])
		{
			if (TAL_Ticks() >= iTimer_ExpTime_tks[tIdx])
			{
				iTimer_State[tIdx] = false;
				uMCP_ITimer_Expired((uMCP_Timer_ID)tIdx);
			}
		}
	}
}



// Utils
bool uMCP_IsByteInRangeExclusive(unsigned char st, unsigned char nd, unsigned char val)
{
	bool result = false;
	unsigned char idx = st;
	unsigned char _nd = nd;
	_nd--;
	while ((idx != _nd) && (!result))
    {
	   idx = (idx + 1) % UMCP_PACKETS_NUMBER;
	   if (idx == val)
	   {
	      result = true;
	   }
    }
	return result;
}


// uMCP state management routines
void uMCP_SELECT_Set(bool value)
{
	if (selectDefaultState)
	{
		uMCP_ITimer_StateSet(uMCP_Timer_SELECT, value != selectDefaultState);
	}

	select = value;
}

void uMCP_STATE_Set(uMCP_State value)
{
	if ((value == uMCP_STATE_ISTART) ||
		(value == uMCP_STATE_HALTED))
	{
		R = 0;
		N = 0;
		A = 0;
		sentBlocksCnt = 0;
	}
	state = value;
}


// basic protocol routines
void uMCP_DataSend(bool isDTE, unsigned char tcnt, unsigned char rcnt, unsigned int rPos, unsigned int cnt, bool isStartTimer)
{
	uMCP_PacketType ptype = isDTE ? uMCP_PTYPE_DTE : uMCP_PTYPE_DTA;

	StrB_WriterInit(ol_buffer, &ol_cnt, UMCP_LBUFFER_SIZE);
	StrB_WriteByte(ol_buffer, &ol_cnt, UMCP_START_SIGN);
	StrB_WriteByte(ol_buffer, &ol_cnt, ptype);
	StrB_WriteByte(ol_buffer, &ol_cnt, (rcnt & 0x0F) | (tcnt << 4));
	StrB_WriteByte(ol_buffer, &ol_cnt, CRC8_Get(ol_buffer, 0, ol_cnt));

	unsigned int dstart_idx = ol_cnt;
	StrB_WriteByte(ol_buffer, &ol_cnt, (unsigned char)cnt);

	int i;
	unsigned int rposc = rPos;
	for (i = 0; i < cnt; i++)
	{
		ol_buffer[ol_cnt++] = ih_ring[rposc];
		rposc = (rposc + 1) % UMCP_IH_RING_SIZE;
	}

	StrB_WriteByte(ol_buffer, &ol_cnt, CRC8_Get(ol_buffer, dstart_idx, cnt + 1));

	ol_ready = true;
	isTimerPendingOnTxFinish = isStartTimer;

	uMCP_ITimer_Init(uMCP_Timer_TX, UMCP_FIXED_TX_DELAY_MS + 1000 *	(int)(((float)(ol_cnt * 8)) / lineBaudrate), true);
	uMCP_SELECT_Set(!isDTE);
}

void uMCP_NextDataBlockSend()
{
	bool isDTE = (sentBlocksCnt >= UMCP_PIPELINING_LIMIT) || (ih_Cnt < UMCP_PACKET_DATA_SIZE);
	unsigned char pcnt = (ih_Cnt < UMCP_PACKET_DATA_SIZE) ? ih_Cnt : UMCP_PACKET_DATA_SIZE;

	uMCP_DataSend(isDTE, N, R, ih_rPos, pcnt, isDTE);

	sentBlocksRPos[N] = ih_rPos;
	sentBlocksSize[N] = pcnt;
	sentBlocksCnt++;

	ih_rPos = (ih_rPos + pcnt) % UMCP_IH_RING_SIZE;
	ih_Cnt -= pcnt;
}

void uMCP_DataBlockResend(unsigned char blockId, bool isDTE, bool isStartTimer)
{
	if (sentBlocksSize[blockId] != 0)
	{
		uMCP_DataSend(isDTE, blockId, R, sentBlocksRPos[blockId], sentBlocksSize[blockId], isStartTimer);
	}
	// else
	//    wtf???
}

void uMCP_CtrlSend(uMCP_PacketType ptype, unsigned char tcnt, unsigned char rcnt, bool isStartTimer)
{
	StrB_WriterInit(ol_buffer, &ol_cnt, UMCP_LBUFFER_SIZE);
	StrB_WriteByte(ol_buffer, &ol_cnt, UMCP_START_SIGN);
	StrB_WriteByte(ol_buffer, &ol_cnt, ptype);
	StrB_WriteByte(ol_buffer, &ol_cnt, (rcnt & 0x0F) | (tcnt << 4));
	StrB_WriteByte(ol_buffer, &ol_cnt, CRC8_Get(ol_buffer, 0, ol_cnt));
	ol_ready = true;
	isTimerPendingOnTxFinish = isStartTimer;

	uMCP_ITimer_Init(uMCP_Timer_TX, UMCP_FIXED_TX_DELAY_MS + 1000 *	(int)(((float)(ol_cnt * 8)) / lineBaudrate), true);
	uMCP_SELECT_Set(false);
}

void uMCP_AcknowledgeSentItems(int to)
{
	unsigned char from = A;
	unsigned char a = from;
    while (a != to)
    {
    	a = (a + 1) % UMCP_PACKETS_NUMBER;
		A = (A + 1) % UMCP_PACKETS_NUMBER;
    	sentBlocksSize[A] = 0; // sent block is free
	    sentBlocksCnt--;
    }
}

void uMCP_Protocol_Perform()
{
	 if (state == uMCP_STATE_RUNNING)
	 {
		if ((!iTimer_State[uMCP_Timer_TX]) &&
			(!iTimer_State[uMCP_Timer_TMO]) &&
			(select))
		{
			if (ih_Cnt == 0)
			{
				if (srep)
				{
					uMCP_CtrlSend(uMCP_PTYPE_REP, N, 0, true);
					srep = false;
				}
				else if (sentBlocksCnt > 0)
				{
					uMCP_DataBlockResend((A + 1) % UMCP_PACKETS_NUMBER, true, true);
				}
				else if ((!selectDefaultState) || (sack))
				{
					uMCP_CtrlSend(uMCP_PTYPE_ACK, N, R, false);
					sack = false;
				}
			}
			else if (ih_Cnt > 0)
			{
				if ((ih_Cnt >= UMCP_PACKET_DATA_SIZE) || (TAL_Ticks() >= ih_TS + UMCP_NAGLE_DELAY_TKS))
				{
					N = (N + 1) % UMCP_PACKETS_NUMBER;
					uMCP_NextDataBlockSend();
				}
			}
		}
	}
}

void uMCP_OnIncomingPacket()
{
	unsigned char tcnt = ip_tcnt;
	unsigned char rcnt = ip_rcnt;
	unsigned char dcnt = ip_dcnt;
	uMCP_PacketType pType = ip_type;
	ip_ready = false;

	switch (pType)
	{
	case uMCP_PTYPE_STR:
	{
		uMCP_SELECT_Set(true);
		if ((state == uMCP_STATE_HALTED) || (state == uMCP_STATE_ISTART))
		{
			uMCP_STATE_Set(uMCP_STATE_ASTART);
			uMCP_ITimer_StateSet(uMCP_Timer_TX, false);
			uMCP_ITimer_StateSet(uMCP_Timer_TMO, false);
			uMCP_CtrlSend(uMCP_PTYPE_STA, 0, 0, true);
		}
		else if (state == uMCP_STATE_ASTART)
		{
			uMCP_STATE_Set(uMCP_STATE_RUNNING);
			uMCP_ITimer_StateSet(uMCP_Timer_TX, false);
			uMCP_ITimer_StateSet(uMCP_Timer_TMO, false);
			uMCP_CtrlSend(uMCP_PTYPE_ACK, 0, 0, false);
		}
		else
		{
			uMCP_STATE_Set(uMCP_STATE_HALTED);
		}
		break;
	}
	case uMCP_PTYPE_STA:
	{
		uMCP_SELECT_Set(true);
		if ((state == uMCP_STATE_ISTART) || (state == uMCP_STATE_ASTART) || (state == uMCP_STATE_RUNNING))
		{
			uMCP_STATE_Set(uMCP_STATE_RUNNING);
			uMCP_ITimer_StateSet(uMCP_Timer_TX, false);
			uMCP_ITimer_StateSet(uMCP_Timer_TMO, false);
			uMCP_CtrlSend(uMCP_PTYPE_ACK, 0, 0, false);
		}
		break;
	}
	case uMCP_PTYPE_REP:
	{
		if (state == uMCP_STATE_RUNNING)
		{
			sack = true;
			srep = false;
			uMCP_SELECT_Set(true);
		}
		break;
	}
	case uMCP_PTYPE_ACK:
	{
		if (state == uMCP_STATE_ASTART)
		{
			if (rcnt == 0)
			{
				uMCP_ITimer_StateSet(uMCP_Timer_TX, false);
				uMCP_ITimer_StateSet(uMCP_Timer_TMO, false);
				uMCP_STATE_Set(uMCP_STATE_RUNNING);
			}
		}
		else if (state == uMCP_STATE_RUNNING)
		{
			srep = false;
			uMCP_ITimer_StateSet(uMCP_Timer_TX, false);
			uMCP_ITimer_StateSet(uMCP_Timer_TMO, false);
			if ((rcnt == N) || (uMCP_IsByteInRangeExclusive(A, N, rcnt)))
			{
				uMCP_AcknowledgeSentItems(rcnt);
			}
		}

		uMCP_SELECT_Set(true);
		break;
	}
	case uMCP_PTYPE_DTA:
	case uMCP_PTYPE_DTE:
	{
		if (state == uMCP_STATE_RUNNING)
		{
			if (tcnt <= (unsigned char)(R + 1))
			{
				if (tcnt == ((R + 1) % UMCP_PACKETS_NUMBER))
				{
					R = (R + 1) % UMCP_PACKETS_NUMBER;
					ff_copy_u8(ip_datablock, oh_buffer, dcnt);
					oh_cnt = dcnt;
					oh_ready = true;
				}
				sack = true;
			}

			uMCP_ITimer_StateSet(uMCP_Timer_TX, false);
			uMCP_ITimer_StateSet(uMCP_Timer_TMO, false);

			if ((rcnt == N) || (uMCP_IsByteInRangeExclusive(A, N, rcnt)))
			{
				uMCP_AcknowledgeSentItems(rcnt);
			}

			if (pType == uMCP_PTYPE_DTA)
			{
				uMCP_ITimer_StateSet(uMCP_Timer_TMO, true);
			}

			uMCP_SELECT_Set(pType == uMCP_PTYPE_DTE);
		}
		break;
	}
	default:
		break;
	}
}


// Events
void uMCP_DC_Output_Process()
{
	if ((oh_ready) && (!IsDCLock[CFG_HOST_DC_ID]))
	{
		TAL_DC_Write_Block(CFG_HOST_DC_ID, oh_buffer, oh_cnt);
		oh_ready = false;
	}
	if ((ol_ready) && (!IsDCLock[CFG_LINE_DC_ID]))
	{
		TAL_DC_Write_Block(CFG_LINE_DC_ID, ol_buffer, ol_cnt);
		ol_ready = false;
	}
}

void uMCP_OnNewByte(DCID_Enum chID, unsigned char c)
{
	if (chID == CFG_HOST_DC_ID)
	{
		if (ih_Cnt < UMCP_IH_RING_SIZE)
		{
			ih_ring[ih_wPos] = c;
			ih_wPos = (ih_wPos + 1) % UMCP_IH_RING_SIZE;
			ih_Cnt++;
			ih_TS = TAL_Ticks();
		}
		else
		{
			// TODO: buffer overflow
		}
	}
	else if (chID == CFG_LINE_DC_ID)
	{
		if (ip_start)
		  {
		    if (ip_pos == UMCP_PTYPE_OFFSET)
		    {
		      ip_ahchk = CRC8_Update(ip_ahchk, c);
		      ip_type = (uMCP_PacketType)c;
		      if (ip_type == uMCP_PTYPE_INVALID)
		      {
		        ip_start = false;
		      }
		      else
		      {
		        ip_pos++;
		      }
		    }
		    else if (ip_pos == UMCP_TRCNT_OFFSET)
		    {
		      ip_ahchk = CRC8_Update(ip_ahchk, c);
		      ip_tcnt = (c >> 4) & 0x0F;
		      ip_rcnt = c & 0x0F;
		      ip_pos++;
		    }
		    else if (ip_pos == UMCP_HCHK_OFFSET)
		    {
		      if (ip_ahchk == c) // Header CRC OK
		      {
		        if ((ip_type != uMCP_PTYPE_DTA) && (ip_type != uMCP_PTYPE_DTE))
		        {
		          ip_start = false;
		          ip_ready = true;
		        }
		        else
		        {
		          ip_pos++;
		        }
		      }
		      else
		      {
		        // crc is not ok
		        ip_start = false;
		      }
		    }
		    else if (ip_pos == UMCP_DCNT_OFFSET)
		    {
		      ip_dcnt = c;

		      if (ip_dcnt >= 1)
		      {
		        ip_ahchk = CRC8_Update(0xff, c);
		        ip_pos++;
		      }
		      else
		      {
		        ip_start = false;
		        ip_ready = true;
		        ip_type = uMCP_PTYPE_ACK;
		      }
		    }
		    else if (ip_pos == UMCP_DTAE_OVERHEAD + ip_dcnt)
		    {
		      if (ip_ahchk != c) // data block CRC is not ok
		      {
		        ip_type = uMCP_PTYPE_ACK;
		      }
		      ip_ready = true;
		      ip_start = false;
		    }
		    else
		    {
		      ip_datablock[ip_pos - UMCP_DTAE_OVERHEAD] = c;
		      ip_ahchk = CRC8_Update(ip_ahchk, c);
		      ip_pos++;
		    }
		  }
		  else if (c == UMCP_START_SIGN)
		  {
		    ip_start = true;
		    ip_pos = 1;
		    ip_ahchk = CRC8_Update(0xff, UMCP_START_SIGN);
		    ip_type = uMCP_PTYPE_INVALID;
		    ip_dcnt = 0;
		  }
	}
}

void uMCP_PPNode_Init()
{
	// Interval timers
	uMCP_ITimer_Init(uMCP_Timer_SELECT, CFG_SELECT_DEFAULT_INTERVAL_MS, false);
	uMCP_ITimer_Init(uMCP_Timer_TMO, CFG_TIMEOUT_INTERVAL_MS, false);

	// Data channels
	dcParams.Baudrate       = CFG_CR_DC_DEFAULT_BAUDRATE;
	dcParams.Wordlength     = CFG_CR_DC_DEFAULT_WORDLENGTH;
	dcParams.Stopbits       = CFG_CR_DC_DEFAULT_STOPBITS;
	dcParams.Parity         = CFG_CR_DC_DEFAULT_PARITY;
	dcParams.HW_FlowControl = CFG_CR_DC_DEFAULT_HW_FLOWCONTROL;
	TAL_DC_Config(CFG_HOST_DC_ID, &dcParams);
	TAL_DC_Config(CFG_LINE_DC_ID, &dcParams);

	TAL_IWDG_Init();
}

void uMCP_PPNode_Run()
{
	while (1)
	{
		uMCP_DC_Output_Process();
		uMCP_ITimers_Process();

		if (ip_ready)
		{
			uMCP_OnIncomingPacket();
		}

		if ((state == uMCP_STATE_HALTED) && ((ih_Cnt > 0) || (isStartup && CFG_IS_AUTOSTART_ON_POWERON)))
		  {
		    if (isStartup)
		    {
		      isStartup = false;
		    }

		    uMCP_STATE_Set(uMCP_STATE_ISTART);
		    uMCP_CtrlSend(uMCP_PTYPE_STR, 0, 0, true);
		  }
		  else if (state == uMCP_STATE_RUNNING)
		  {
		    uMCP_Protocol_Perform();
		  }

#ifdef CFG_TAL_IWDG_ENABLED
		TAL_IWDG_Reload();
#endif
	}
}
