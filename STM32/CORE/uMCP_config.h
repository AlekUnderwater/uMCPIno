// (C) Aleksandr Dikarev, 2019

#ifndef _UMCP_CONFIG_H_
#define _UMCP_CONFIG_H_

#include <uMCP_types.h>

/// ********************************************************** Configurable items
#define CFG_SELECT_DEFAULT_STATE          (false) // If set to true, the node will regain the SELECT flag by timeout
#define CFG_SELECT_DEFAULT_INTERVAL_MS    (4000) // Default interval of selection in milliseconds
#define CFG_TIMEOUT_INTERVAL_MS           (3000) // Default timeout interval in milliseconds
#define CFG_LINE_BAUDRATE_BPS             (9600) // Physical line connection speed (not a port speed!)
#define CFG_NAGLE_DELAY_MS                (100)  // Nagle algorithm delay in milliseconds
#define CFG_IS_AUTOSTART_ON_POWERON       (true) // If set to true, node will perform autostart on the MCU startup
// ********************************************************** /Configurable items

#define CFG_CR_TICK_FREQUENCY_HZ         (5000.0f)

#define CFG_CR_DC_DEFAULT_BAUDRATE       (DC_BAUDRATE_9600)
#define CFG_CR_DC_DEFAULT_WORDLENGTH     (DC_WORD_LENGTH_8_BIT)
#define CFG_CR_DC_DEFAULT_STOPBITS       (DC_STOPBITS_1)
#define CFG_CR_DC_DEFAULT_PARITY         (DC_PARITY_NONE)
#define CFG_CR_DC_DEFAULT_HW_FLOWCONTROL (DC_HW_FLOW_CONTROL_NONE)

#define CFG_HOST_DC_ID                   (DC_3)
#define CFG_LINE_DC_ID                   (DC_1)

#define CFG_TAL_IWDG_ENABLED

#endif
