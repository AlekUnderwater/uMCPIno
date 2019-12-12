// (C) Aleksandr Dikarev, 2015-2019

#ifndef _UMCP_TYPES_
#define _UMCP_TYPES_

typedef unsigned char bool;

#define true  ((bool)(1))
#define false (!true)

#define IS_BOOL(value) ((value == true) || (value == false))

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


// uMCP_LERR_Enum
typedef enum
{
	LERR_OK = 0,
	LERR_ARGUMENT_OUT_OF_RANGE = 1,
	LERR_CHECKSUM = 2,
	LERR_UNSUPPORTED = 3,
	LERR_TX_OVERFLOW = 4,
	LERR_UNKNOWN
} uMCP_LERR_Enum;


// DCBaudrate_Enum
typedef enum
{
	DC_BAUDRATE_1200   = 0,
	DC_BAUDRATE_2400   = 1,
	DC_BAUDRATE_4800   = 2,
	DC_BAUDRATE_9600   = 3,
	DC_BAUDRATE_19200  = 4,
	DC_BAUDRATE_38400  = 5,
	DC_BAUDRATE_57600  = 6,
	DC_BAUDRATE_115200 = 7,
	DC_BAUDRATE_INVALID
} DCBaudrate_Enum;
#define IS_DC_BAUDRATE(value) ((value >= DC_BAUDRATE_1200) && (value < DC_BAUDRATE_INVALID))

// DCStopBits_Enum
typedef enum
{
	DC_STOPBITS_1   = 0,
	DC_STOPBITS_0_5 = 1,
	DC_STOPBITS_2   = 2,
	DC_STOPBITS_1_5 = 3,
	DC_STOPBITS_INVALID
} DCStopBits_Enum;
#define IS_DC_STOPBITS(value) ((value >= DC_STOPBITS_1) && (value < DC_STOPBITS_INVALID))

// DCParity_Enum
typedef enum
{
	DC_PARITY_NONE = 0,
	DC_PARITY_EVEN = 1,
	DC_PARITY_ODD  = 2,
	DC_PARITY_INVALID
} DCParity_Enum;
#define IS_DC_PARITY(value) ((value >= DC_PARITY_NONE) && (value < DC_PARITY_INVALID))

// DCWordLength_Enum
typedef enum
{
	DC_WORD_LENGTH_8_BIT = 0,
	DC_WORD_LENGTH_9_BIT = 1,
	DC_WORD_LENGTH_INVALID
} DCWordLength_Enum;
#define IS_DC_WORD_LENGTH(value) ((value >= DC_WORD_LENGTH_8_BIT) && (value < DC_WORD_LENGTH_INVALID))

// DCHardwareFlowControl_Enum
typedef enum
{
	DC_HW_FLOW_CONTROL_NONE    = 0,
	DC_HW_FLOW_CONTROL_RTC     = 1,
	DC_HW_FLOW_CONTROL_CTS     = 2,
	DC_HW_FLOW_CONTROL_RTC_CTS = 3,
	DC_HW_FLOW_CONTROL_INVALID
} DCHardwareFlowControl_Enum;
#define IS_DC_HARDWARE_FLOW_CONTROL(value) ((value >= DC_HW_FLOW_CONTROL_NONE) && (value < DC_HW_FLOW_CONTROL_INVALID))

// DCID_Enum
typedef enum
{
	DC_1 = 0,
	DC_2 = 1,
	DC_3 = 2,
	DC_4 = 3,
	DC_5 = 4,
	DC_6 = 5,
	DC_INVALID
} DCID_Enum;

// DCParams_Struct
typedef struct
{
	DCBaudrate_Enum            Baudrate;
	DCParity_Enum              Parity;
	DCStopBits_Enum            Stopbits;
	DCWordLength_Enum          Wordlength;
	DCHardwareFlowControl_Enum HW_FlowControl;
} DCParams_Struct;

#endif
