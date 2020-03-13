/*
 * Copyright (c) 2019, Anke Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-19     redoc        the first version
 */

#include <rtthread.h>
#include "drv_spi.h"
#include "drv_max7219.h"


static int rt_hw_spi_max7219_init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    rt_hw_spi_device_attach(MAX7219_SPI_BUS_NAME, MAX7219_SPI_DEVICE_NAME, GPIOC, GPIO_PIN_0);

    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_spi_max7219_init);


