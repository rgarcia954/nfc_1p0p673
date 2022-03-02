/**
 * @file mem.ld
 * @brief Standard memory linker description file
 *
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
MEMORY
{
  ROM  (r) : ORIGIN = 0x00000000, LENGTH = 4K   
  FLASH (xrw) : ORIGIN = 0x00100000, LENGTH = 384K

  DRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 16K
  DRAM_BB (xrw) : ORIGIN = 0x20006000, LENGTH = 8K
}

