/*
    (C) Aleksandr Dikarev, 2018-2019
    aleksandr.v.dikarev@gmail.com
    
    uMCP Ino Node - a template sketch for uMCPIno tranciever node


    1) To send data over uMCPIno link use the function:

    bool uMCPIno_SendData(byte* dataToSend, byte dataSize);
    where:
    - dataToSend - a byte buffer to send
    - dataSize - size of the buffer

    2) Before sending data, check if there is enough space in tx buffer:
    bool uMCP_IsCanSend(byte dataSize);
    where:
    - dataSize - number of bytes to send
    
    3) To analyse the received data:    
    The received over uMCP link data is stored in a ring buffer 'il_ring'.
    The amount of received bytes is stored in 'il_Cnt'
    The read pointer is in 'il_rPos'.
    To perform the analysis of the received data, put your code in function:

    void USER_uMCPIno_DataPacketReceived();
    It fires when new data packet has been received from a remote node over uMCP line

    4) If you need to be informed, when the tx buffer is empty, put your code in function:        

    void USER_uMCP_OnTxBufferEmptry();
    it fires when all sent messages has been acknowlegded by the remote node

*/

// ********************************************************** Arduino related items
//#define DEBUG

//#define HAS_SERIAL1
#ifndef HAS_SERIAL1
#include <SoftwareSerial.h>
#define SOFTWARE_SERIAL_TX_PIN (3)
#define SOFTWARE_SERIAL_RX_PIN (2)
SoftwareSerial linePort(SOFTWARE_SERIAL_RX_PIN, SOFTWARE_SERIAL_TX_PIN);
#endif
// ********************************************************** /Arduino related items

// ********************************************************** Configurable items
#define CFG_SELECT_DEFAULT_STATE          (true) // If set to true, the node will regain the SELECT flag by timeout
#define CFG_SELECT_DEFAULT_INTERVAL_MS    (4000) // Default interval of selection in milliseconds
#define CFG_TIMEOUT_INTERVAL_MS           (3000) // Default timeout interval in milliseconds
#define CFG_LINE_BAUDRATE_BPS             (9600) // Physical line connection speed (not a port speed!)
#define CFG_NAGLE_DELAY_MS                (100)  // Nagle algorithm delay in milliseconds
#define CFG_HOST_CONNECTION_BAUDRATE_BPS  (9600) // Host connection port speed
#define CFG_LINE_CONNECTION_BAUDRATE_BPS  (9600) // Line connection port speed
#define CFG_IS_AUTOSTART_ON_POWERON       (true) // If set to true, node will perform autostart on the MCU startup

#define CFG_IL_RING_SIZE                  (255) // Ring buffer to store received data from line
// ********************************************************** /Configurable items

// ********************************************************** uMCP Ino related defines
#define UMCP_START_SIGN                   (0xAD)
#define UMCP_PACKET_DATA_SIZE             (64)
#define UMCP_PACKETS_NUMBER               (8)
#define UMCP_PIPELINING_LIMIT             (4)
#define UMCP_MAX_OVERHEAD                 (6)
#define UMCP_MAX_PACKET_SIZE              (UMCP_PACKET_DATA_SIZE + UMCP_MAX_OVERHEAD)
#define UMCP_FIXED_TX_DELAY_MS            (50)
#define UMCP_NBUFFER_SIZE                 (UMCP_PACKET_DATA_SIZE)
#define UMCP_LBUFFER_SIZE                 (UMCP_MAX_PACKET_SIZE)

#define UMCP_IH_RING_SIZE                 (UMCP_PACKET_DATA_SIZE * UMCP_PACKETS_NUMBER) // Size of a buffer from the host

#define UMCP_PTYPE_OFFSET                 (1)
#define UMCP_TRCNT_OFFSET                 (2)
#define UMCP_HCHK_OFFSET                  (3)
#define UMCP_DCNT_OFFSET                  (4)
#define UMCP_DTAE_OVERHEAD                (5)
// ********************************************************** /uMCP Ino related defines

// ********************************************************** Custom enums
// uMCP_PacketType
typedef enum
{
  uMCP_PTYPE_ACK = (1 | 32),
  uMCP_PTYPE_REP = (2 | 32),
  uMCP_PTYPE_STA = (4 | 32),
  uMCP_PTYPE_STR = (8 | 32),
  uMCP_PTYPE_DTA = (1 | 16),
  uMCP_PTYPE_DTE = (1 | 16 | 32),
  uMCP_PTYPE_INVALID

} uMCP_PacketType;

// uMCP_State
typedef enum
{
  uMCP_STATE_HALTED,
  uMCP_STATE_ISTART,
  uMCP_STATE_ASTART,
  uMCP_STATE_RUNNING,
  uMCP_STATE_INVALID

} uMCP_State;

// uMCP_Timer_ID
typedef enum
{
  uMCP_Timer_TMO = 0,
  uMCP_Timer_SELECT = 1,
  uMCP_Timer_TX = 2,
  uMCP_Timer_INVALID

} uMCP_Timer_ID;
// ********************************************************** /Custom enums

// ********************************************************** Variables
byte il_ring[CFG_IL_RING_SIZE];
volatile unsigned int il_rPos = 0;
volatile unsigned int il_wPos = 0;
volatile unsigned int il_Cnt = 0;
bool il_ready = false;


// buffer from HOST
byte ih_ring[UMCP_IH_RING_SIZE];
volatile unsigned int ih_rPos = 0;
volatile unsigned int ih_wPos = 0;
volatile unsigned int ih_Cnt = 0;
volatile unsigned long ih_TS = 0;

// buffer to HOST
byte oh_buffer[UMCP_NBUFFER_SIZE];
unsigned int oh_cnt = 0;
bool oh_ready = false;

// buffer to LINE
byte ol_buffer[UMCP_MAX_PACKET_SIZE];
unsigned int ol_cnt = 0;
bool ol_ready = false;

// incoming packet
byte ip_datablock[UMCP_PACKET_DATA_SIZE];
volatile bool ip_start = false;
volatile byte ip_pos = 0;
volatile uMCP_PacketType ip_type = uMCP_PTYPE_INVALID;
volatile byte ip_tcnt = 0;
volatile byte ip_rcnt = 0;
volatile byte ip_ahchk = 0;
volatile byte ip_dcnt = 0;
volatile bool ip_ready = false;

byte R, N, A;
bool selectDefaultState = CFG_SELECT_DEFAULT_STATE;
bool select = CFG_SELECT_DEFAULT_STATE;
uMCP_PacketType pType = uMCP_PTYPE_INVALID;
uMCP_State state = uMCP_STATE_HALTED;
bool sack = false;
bool srep = false;
bool isTimerPendingOnTxFinish = false;
int lineBaudrate = CFG_LINE_BAUDRATE_BPS;

unsigned int iTimer_Interval_ms[uMCP_Timer_INVALID];
unsigned long iTimer_ExpTime_ms[uMCP_Timer_INVALID];
bool iTimer_State[uMCP_Timer_INVALID];

byte sentBlocksSize[UMCP_PACKETS_NUMBER];
unsigned int sentBlocksRPos[UMCP_PACKETS_NUMBER];
byte sentBlocksCnt = 0;

bool isStartup = true;

// ********************************************************** /Variables

// ********************************************************** Functions

// ********************************************************** Protocol related timers
void uMCP_ITimer_Init(uMCP_Timer_ID timerID, unsigned int interval_ms, bool istate)
{
  iTimer_Interval_ms[timerID] = interval_ms;
  uMCP_ITimer_StateSet(timerID, istate);
}

void uMCP_ITimer_StateSet(uMCP_Timer_ID timerID, bool value)
{
  if (value)
  {
    iTimer_ExpTime_ms[timerID] = millis() + iTimer_Interval_ms[timerID];
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
    uMCP_SELECT_Set(selectDefaultState);
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
      if (millis() >= iTimer_ExpTime_ms[tIdx])
      {
        iTimer_State[tIdx] = false;
        uMCP_ITimer_Expired((uMCP_Timer_ID)tIdx);
      }
    }
  }
}
// ********************************************************** /Protocol related timers

// ********************************************************** uMCP state mangement routines
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
// ********************************************************** /uMCP state mangement routines

// ********************************************************** Utils
bool uMCP_IsByteInRangeExclusive(byte st, byte nd, byte val)
{
  bool result = false;
  byte idx = st;
  byte _nd = nd;
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

void ff_fill_u8(byte* dst, byte value, unsigned int size)
{
  unsigned int blkCnt = size;
  while (blkCnt > 0u)
  {
    *dst++ = value;
    blkCnt--;
  }
}

void ff_copy_u8(byte* src, byte* dst, unsigned int size)
{
  unsigned int blkCnt = size;
  while (blkCnt > 0u)
  {
    *dst++ = (*src++);
    blkCnt--;
  }
}

void StrB_WriterInit(byte* buffer, unsigned int* srcIdx, unsigned int size)
{
  *srcIdx = 0;
  ff_fill_u8(buffer, 0, size);
}

void StrB_WriteByte(byte* buffer, unsigned int* srcIdx, byte c)
{
  buffer[*srcIdx] = c;
  (*srcIdx)++;
}

static byte CRC8Table[] = { 0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
                            0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
                            0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
                            0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
                            0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
                            0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
                            0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
                            0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
                            0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
                            0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
                            0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
                            0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
                            0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
                            0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
                            0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
                            0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
                            0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
                            0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
                            0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
                            0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
                            0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
                            0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
                            0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
                            0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
                            0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
                            0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
                            0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
                            0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
                            0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
                            0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
                            0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
                            0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
                          };

byte CRC8_Update(byte prev, byte next)
{
  return CRC8Table[prev ^ next];
}

byte CRC8_Get(byte* buffer, unsigned int sIdx, unsigned int cnt)
{
  unsigned int i;
  byte crc = 0xff;
  for (i = 0; i < cnt; i++)
    crc = CRC8Table[crc ^ buffer[i + sIdx]];
  return crc;
}
// ********************************************************** /Utils

// ********************************************************** Main protocol routines
void uMCP_DataSend(bool isDTE, byte tcnt, byte rcnt, unsigned int rPos, byte cnt, bool isStartTimer)
{
  uMCP_PacketType ptype = isDTE ? uMCP_PTYPE_DTE : uMCP_PTYPE_DTA;

  #ifdef DEBUG  
  Serial.print("out:");
  switch (ptype)
  { 
    case uMCP_PTYPE_DTA: Serial.print("DTA("); break;
    case uMCP_PTYPE_DTE: Serial.print("DTE("); break;    
  }
 
  Serial.print(tcnt);
  Serial.print(",");
  Serial.print(rcnt);
  Serial.print(",");
  Serial.print(cnt);        
  Serial.println(")");
  #endif

  StrB_WriterInit(ol_buffer, &ol_cnt, UMCP_LBUFFER_SIZE);
  StrB_WriteByte(ol_buffer, &ol_cnt, UMCP_START_SIGN);
  StrB_WriteByte(ol_buffer, &ol_cnt, ptype);

  StrB_WriteByte(ol_buffer, &ol_cnt, (rcnt & 0x0F) | (tcnt << 4));
  StrB_WriteByte(ol_buffer, &ol_cnt, CRC8_Get(ol_buffer, 0, ol_cnt));

  unsigned int dstart_idx = ol_cnt;
  StrB_WriteByte(ol_buffer, &ol_cnt, cnt);

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

  uMCP_ITimer_Init(uMCP_Timer_TX, UMCP_FIXED_TX_DELAY_MS + 1000 * (int)(((float)(ol_cnt * 8)) / lineBaudrate), true);
  uMCP_SELECT_Set(!isDTE);
}

void uMCP_NextDataBlockSend()
{
  bool isDTE = (sentBlocksCnt >= UMCP_PIPELINING_LIMIT) || (ih_Cnt < UMCP_PACKET_DATA_SIZE);
  byte pcnt = (ih_Cnt < UMCP_PACKET_DATA_SIZE) ? ih_Cnt : UMCP_PACKET_DATA_SIZE;

  uMCP_DataSend(isDTE, N, R, ih_rPos, pcnt, isDTE);

  sentBlocksRPos[N] = ih_rPos;
  sentBlocksSize[N] = pcnt;
  sentBlocksCnt++;

  ih_rPos = (ih_rPos + pcnt) % UMCP_IH_RING_SIZE;
  ih_Cnt -= pcnt;
}

void uMCP_DataBlockResend(byte blockId, bool isDTE, bool isStartTimer)
{
  if (sentBlocksSize[blockId] != 0)
  {
    uMCP_DataSend(isDTE, blockId, R, sentBlocksRPos[blockId], sentBlocksSize[blockId], isStartTimer);
  }
}

void uMCP_CtrlSend(uMCP_PacketType ptype, byte tcnt, byte rcnt, bool isStartTimer)
{
  #ifdef DEBUG  
  Serial.print("out:");
  switch (ptype)
  { 
    case uMCP_PTYPE_ACK: Serial.print("ACK("); break;
    case uMCP_PTYPE_REP: Serial.print("REP("); break;
    case uMCP_PTYPE_STA: Serial.print("STA("); break;
    case uMCP_PTYPE_STR: Serial.print("STR("); break;    
  }
 
  Serial.print(tcnt);
  Serial.print(",");
  Serial.print(rcnt);
  Serial.println(")");
  #endif
  
  StrB_WriterInit(ol_buffer, &ol_cnt, UMCP_LBUFFER_SIZE);
  StrB_WriteByte(ol_buffer, &ol_cnt, UMCP_START_SIGN);
  StrB_WriteByte(ol_buffer, &ol_cnt, ptype);
  StrB_WriteByte(ol_buffer, &ol_cnt, (rcnt & 0x0F) | (tcnt << 4));
  StrB_WriteByte(ol_buffer, &ol_cnt, CRC8_Get(ol_buffer, 0, ol_cnt));
  ol_ready = true;
  isTimerPendingOnTxFinish = isStartTimer;

  uMCP_ITimer_Init(uMCP_Timer_TX, UMCP_FIXED_TX_DELAY_MS + 1000 * (int)(((float)(ol_cnt * 8)) / lineBaudrate), true);
  uMCP_SELECT_Set(false);
}

void uMCP_AcknowledgeSentItems(byte to)
{
  byte from = A;
  byte a = from;
  while (a != to)
  {
    a = (a + 1) % UMCP_PACKETS_NUMBER;
    A = (A + 1) % UMCP_PACKETS_NUMBER;
    sentBlocksSize[A] = 0; // sent block is free
    sentBlocksCnt--;    
  }

  if (sentBlocksCnt == 0)
  {
    USER_uMCP_OnTxBufferEmptry();
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
          uMCP_CtrlSend(uMCP_PTYPE_REP, N, R, true);
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
        if ((ih_Cnt >= UMCP_PACKET_DATA_SIZE) || (millis() >= ih_TS + CFG_NAGLE_DELAY_MS))
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
  byte tcnt = ip_tcnt;
  byte rcnt = ip_rcnt;
  byte dcnt = ip_dcnt;
  uMCP_PacketType pType = ip_type;
  ip_ready = false;

  #ifdef DEBUG  
  Serial.print("in:");
  switch (pType)
  { 
    case uMCP_PTYPE_ACK: Serial.print("ACK("); break;
    case uMCP_PTYPE_REP: Serial.print("REP("); break;
    case uMCP_PTYPE_STA: Serial.print("STA("); break;
    case uMCP_PTYPE_STR: Serial.print("STR("); break;
    case uMCP_PTYPE_DTA: Serial.print("DTA("); break;
    case uMCP_PTYPE_DTE: Serial.print("DTE("); break;
    case uMCP_PTYPE_INVALID: Serial.print("INVALID("); break;
  }
  Serial.print(tcnt);
  Serial.print(",");
  Serial.print(rcnt);
  if ((pType == uMCP_PTYPE_DTA) || (pType == uMCP_PTYPE_DTE))
  {
    Serial.print(",");
    Serial.print(dcnt);        
  }
  Serial.println(")");  
  #endif

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
          if (tcnt <= R + 1)
          {
            if (tcnt == ((R + 1) % UMCP_PACKETS_NUMBER))
            {
              R = (R + 1) % UMCP_PACKETS_NUMBER;
              
              //
              byte c = 0;
              for (c = 0; c < dcnt; c++)
              {
                il_ring[il_wPos] = ip_datablock[c];
                il_wPos = (il_wPos + 1) % CFG_IL_RING_SIZE;
                il_Cnt++;
              }
              il_ready = true;
              //
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

void On_NewByte_From_Line(byte c)
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

// ********************************************************** Platform specific
void DC_Output_Process()
{  
  if (ol_ready)
  {
#ifdef HAS_SERIAL1
    Serial1.write(ol_buffer, ol_cnt);
#else
    linePort.write(ol_buffer, ol_cnt);
#endif
    ol_ready = false;
  }
}

void DC_Input_Process()
{
  
#ifdef HAS_SERIAL1
  if (Serial1.available() > 0)
  {
    On_NewByte_From_Line(Serial1.read());
  }
#else
  if (linePort.available() > 0)
  {
    On_NewByte_From_Line(linePort.read());
  }
#endif
}
// ********************************************************** /Platform specific


// ********************************************************** /Functions


// Call it to determine if specified amount of bytes can be sent to uMCP line at the moment
bool uMCPIno_IsCanSend(byte dataSize)
{
  return (ih_Cnt + dataSize <= UMCP_IH_RING_SIZE);
}

// Call it to send specified data over uMCP line to a remote node
bool uMCPIno_SendData(byte* dataToSend, byte dataSize)
{
  if (uMCPIno_IsCanSend(dataSize))
  {
    while (dataSize > 0)
    {
      ih_ring[ih_wPos] = *dataToSend++;
      ih_wPos = (ih_wPos + 1) % UMCP_IH_RING_SIZE;
      ih_Cnt++;
      dataSize--;
    }    
    ih_TS = millis();
    return true;
  }
  else
  {
    return false;
  }
}

// Fires when all sent messages has been acknowlegded by the remote node
void USER_uMCP_OnTxBufferEmptry()
{
  // put your code here 
  // Here you can send messages to a remote node
}


// Fires when new data packet has been received from a remote node over uMCP line
void USER_uMCPIno_DataPacketReceived()
{
  // put your code to analyze incoming data here
  // at this moment, the received data is stored in 'il_ring'
  // the amount of unprocessed data is in 'il_Cnt'
  // to read this data you should do the cycle:

  byte c;
  while (il_Cnt > 0)
  {
    c = il_ring[il_rPos];
    il_rPos = (il_rPos + 1) %  CFG_IL_RING_SIZE;
    il_Cnt--;

    // for example, here we just send received data to the debug console - Serial
    Serial.write(c);
    //
  }
}

void setup()
{
  uMCP_ITimer_Init(uMCP_Timer_TMO, CFG_TIMEOUT_INTERVAL_MS, false);
  uMCP_ITimer_Init(uMCP_Timer_SELECT, CFG_SELECT_DEFAULT_INTERVAL_MS, false);

  // Here Serial is used for debug purposes
  Serial.begin(CFG_HOST_CONNECTION_BAUDRATE_BPS);

  // If the board has Serial1 it is used for line communication
#ifdef HAS_SERIAL1
  Serial1.begin(CFG_LINE_CONNECTION_BAUDRATE_BPS);
#else
  // If the board doesn't have Serial1 than SoftwareSerial is used for line communication
  linePort.begin(CFG_LINE_CONNECTION_BAUDRATE_BPS);
#endif
}

void loop()
{
  uMCP_ITimers_Process();
  DC_Input_Process();
  DC_Output_Process();

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

    if (il_ready)
    {
      il_ready = false;
      USER_uMCPIno_DataPacketReceived();
    }  
  }
}
