// (C) Aleksandr Dikarev, 2019

#ifndef _UMCP_DEFINES_H_
#define _UMCP_DEFINES_H_

#include <uMCP_config.h>
#include <ff.h>
#include <ustr.h>
#include <crc.h>

#define CR_CORE_MONIKER             "uMCPIno\0"
#define CR_CORE_VERSION             (0x0100)

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

#define UMCP_IH_RING_SIZE                 (UMCP_PACKET_DATA_SIZE * UMCP_PACKETS_NUMBER)  // Size of a buffer from the host

#define UMCP_PTYPE_OFFSET                 (1)
#define UMCP_TRCNT_OFFSET                 (2)
#define UMCP_HCHK_OFFSET                  (3)
#define UMCP_DCNT_OFFSET                  (4)
#define UMCP_DTAE_OVERHEAD                (5)
// ********************************************************** /uMCP Ino related defines

#define CR_DC_TX_BUFFER_SIZE        (255)
#define UMCP_NAGLE_DELAY_TKS        MS2TKS(CFG_NAGLE_DELAY_MS)

#define CR_TICKS_PER_MS             ((unsigned int)(CFG_CR_TICK_FREQUENCY_HZ / 1000.0f))
#define CR_TICKS_PER_SEC            ((int)CFG_CR_TICK_FREQUENCY_HZ)
#define CFG_TAL_IWDG_RELOAD_TICKS   (CFG_TAL_IWDG_RELOAD_MS * CR_TICKS_PER_MS)

#define MS2TKS(ms)                  (ms * CR_TICKS_PER_MS)
#define TKS2SEC(ticks)              (((float)(ticks)) / (float)CFG_CR_TICK_FREQUENCY_HZ)
#define TKS2MS(ticks)               (((float)(ticks)) / CR_TICKS_PER_MS)

#endif
