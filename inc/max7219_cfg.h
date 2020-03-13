/*
 * Copyright (c) 2019, redocCheng
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-13     redoc        the first version
 */

#ifndef __DRV_MAX7219_CFG_H
#define __DRV_MAX7219_CFG_H

#include "rtconfig.h"


#define MAX7219_SPI_BUS_NAME     PKG_MAX7219_SPI_BUS_NAME

#define MAX7219_SPI_DEVICE_NAME  PKG_MAX7219_SPI_DEVICE_NAME

#define MAX7219_CHIPS_NUMBER     PKG_MAX7219_CHIPS_NUMBER

/* scan digs of the chips */
#define MAX7219_CHIPS_SCAN_NUMBER_TABLE     \
{                                           \
    6,                                      \
}

/* eg.

#define MAX7219_CHIPS_NUMBER               (4)

#define MAX7219_CHIPS_SCAN_NUMBER_TABLE     \
{                                           \
    8,                                      \
    8,                                      \
    8,                                      \
    8,                                      \
}

*/



#endif
