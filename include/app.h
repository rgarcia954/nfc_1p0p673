/**
 * @app.h
 * @brief Header file for NFC sample application
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#ifndef APP_H
#define APP_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include <hw.h>
#include "nfc_api.h"
#include "swmTrace_api.h"

/* ----------------------------------------------------------------------------
 * Global Macros
 * --------------------------------------------------------------------------*/
 
/* GPIO number that is used for easy re-flashing (recovery mode) */
#define RECOVERY_GPIO                   7

/* The GPIO pin to use for TX when using the UART mode */
#define UART_TX_GPIO    				(5)

/* The GPIO pin to use for RX when using the UART mode */
#define UART_RX_GPIO    				(6)

/* The selected baud rate for the application when using UART mode */
#define UART_BAUD       				(115200)


/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
