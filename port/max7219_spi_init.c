/*
 * Copyright (c) 2019, Anke Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-01-19     redoc        the first version
 */

#include <rtthread.h>
#include "drv_spi.h"
#include "drv_max7219.h"


#define RT_MAX7219_DEFAULT_SPI_CFG                         \
{                                                          \
    .mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB,    \
    .data_width = 8,                                       \
    .max_hz = 500 * 1000,                                  \
}

static int rt_hw_spi_max7219_init(void)
{
    struct rt_spi_configuration cfg = RT_MAX7219_DEFAULT_SPI_CFG;
    struct rt_spi_device *spi_device;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    rt_hw_spi_device_attach(MAX7219_SPI_BUS_NAME, MAX7219_SPI_DEVICE_NAME, GPIOC, GPIO_PIN_0);

    spi_device = (struct rt_spi_device *)rt_device_find(MAX7219_SPI_DEVICE_NAME);
    rt_spi_configure(spi_device, &cfg);
    
    return RT_EOK;
}
INIT_PREV_EXPORT(rt_hw_spi_max7219_init);


