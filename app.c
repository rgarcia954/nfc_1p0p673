/**
 * @main.c
 * @brief C file for NFC sample application
 * @copyright @parblock
 * Copyright (c) 2022 Semiconductor Components Industries, LLC (d/b/a
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

#include "app.h"

//volatile bool end_of_comm = 0;

static uint32_t traceOptions[] = {
		SWM_LOG_LEVEL_INFO,              /* In all cases log info messages */
		SWM_UART_RX_PIN | UART_RX_GPIO,  /* Set RX pin for cases when using UART */
		SWM_UART_TX_PIN | UART_TX_GPIO,  /* Set TX pin for cases when using UART */
		SWM_UART_RX_ENABLE,              /* Enable the UART Rx Interrupts */
		SWM_UART_BAUD_RATE | UART_BAUD   /* Set Baud rate */

};

void DisableAppInterrupts(void)
{
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);
}

void EnableAppInterrupts(void)
{
	__set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
}

int main()
{
    uint32_t trim_error __attribute__((unused));

    DisableAppInterrupts();

    // Check for recovery GPIO to enable recovery.
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
        SYS_WATCHDOG_REFRESH();
    }

    // Load default trim values.
    trim_error = SYS_TRIM_LOAD_DEFAULT();

    // Switch to the RC oscillator (multiplied) as the clock source, and update the
    // SystemCoreClock global variable.
    ACS->RCOSC_CTRL = RC_OSC_12MHZ | RC_OSC_P46P5;
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK);

	// Update the SystemCoreClock global variable.
	SystemCoreClockUpdate();

    // Switch to the RC oscillator (multiplied) as the clock source, and update the
    // SystemCoreClock global variable.
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK);

    /* Initialize trace library */
    swmTrace_init(traceOptions, 5);

    /* Initialize NFC hardware and software tables */
    NFC_Initialize();

    EnableAppInterrupts();

    swmLogInfo("	NFC app has started... \r\n");

    while (1)
    {
        /* Respond to any incoming NFC request */
    	NFC_Handler();

    	/* Refresh the watchdog timer */
    	SYS_WATCHDOG_REFRESH();

		/* Wait for interrupt */
          __WFI();
    }
    return 0;
}
