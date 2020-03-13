/*
 * Copyright (c) 2019, Anke Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-19     redoc        the first version
 */
 
#include <board.h>
#include "drv_max7219.h"
#include "string.h"
#include "drv_spi.h"

//#define DRV_DEBUG
#define LOG_TAG "drv.max7219"
#include <drv_log.h>

// 非编码模式十六进制单个编码 
uint8_t no_code_buf[16] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x0d, 0x3d, 0x4f, 0x47};

static struct drv_max7219_device _max7219;


static void max7219_reg_write(uint8_t cmd, uint8_t value)
{
    struct rt_spi_message msg1, msg2;

    msg1.send_buf   = &cmd;
    msg1.recv_buf   = RT_NULL;
    msg1.length     = 1;
    msg1.cs_take    = 1;
    msg1.cs_release = 0;
    msg1.next       = &msg2;

    msg2.send_buf   = &value;
    msg2.recv_buf   = RT_NULL;
    msg2.length     = 1;
    msg2.cs_take    = 0;
    msg2.cs_release = 1;
    msg2.next       = RT_NULL;

    rt_spi_transfer_message(_max7219.spi_device, &msg1);
}

/**
 * the Digital tubes blanking.
 * @note 1 to scan_num.
 *
 * @return rt_err_t
 */
rt_err_t max7219_clear(uint8_t dig)
{
    if(dig > (_max7219.info.scan_num))
    {
        log_e("dig num fault.");
        return -RT_ERROR;
    }

    max7219_reg_write(dig, 0);
    return RT_EOK;
}

/**
 * all the Digital tubes blanking.
 * @note 1 to scan_num.
 *
 * @return rt_err_t
 */
rt_err_t max7219_clear_all(void)
{
    for(uint8_t i = 0; i <=_max7219.info.scan_num; i++)
    {
        if( -RT_ERROR == max7219_clear(i))
        {
            return -RT_ERROR;
        }
    }
    return RT_EOK;
}

/**
 * Write data to max7219.
 * @note dig must less the scan_num.
 *
 * @param dig the dig num(eg.1-->6)
 * @param data write the dig data(eg.0x7e is num 0)
 *
 * @return rt_err_t
 */
rt_err_t max7219_write(uint8_t dig, uint8_t data)
{
    if(dig > (_max7219.info.scan_num))
    {
        log_e("dig num fault.");
        return -RT_ERROR;
    }

    max7219_reg_write(dig, data);
    return RT_EOK;
}

/**
 * Write num to max7219.
 * @note support 0x0 to 0xf.
 *
 * @param dig the dig num(1----scan_num)
 * @param num write the num
 *
 * @return rt_err_t return result
 */
rt_err_t max7219_write_num(uint8_t dig, uint8_t num)
{
    uint8_t value = 0;

    if(value > 0x0f)
    {
        log_e("value falut ,it can't max 0x0f.");
        return -RT_ERROR;
    }

    max7219_reg_write(dig, no_code_buf[num]);
    
    return RT_EOK;
}

rt_err_t max7219_intensity_set(uint8_t value)
{
    if(value > 0x0f)
    {
        log_e("intnsity value over 0x0f.");
        return -RT_ERROR;
    }
    else
    {
        _max7219.info.intensity = value;
        max7219_reg_write(REG_ADDR_INTENSITY, value);
    }
    return RT_EOK;
}

void max7219_init(void)
{
    max7219_reg_write(REG_ADDR_SHUTDOWN, _max7219.info.shutdown_mode);         
    max7219_reg_write(REG_ADDR_DISPTEST, _max7219.info.work_mode);      
    max7219_reg_write(REG_ADDR_DECODEMODE, _max7219.info.decode_mode);       
    max7219_reg_write(REG_ADDR_SCANLIMIT, _max7219.info.scan_num);        
    max7219_reg_write(REG_ADDR_INTENSITY, _max7219.info.intensity);           

    log_d("max7219 init.");
}

int drv_max7219_hw_init(void)
{
    struct rt_spi_configuration cfg = RT_MAX7219_DEFAULT_SPI_CFG;
    struct rt_device_max7219_info info = MAX7219_INFO_DEFAULT;

    memset(&_max7219, 0x00, sizeof(_max7219));
    memcpy(&_max7219.info, &info,sizeof(struct rt_device_max7219_info));
    
    _max7219.spi_device = (struct rt_spi_device *)rt_device_find(MAX7219_SPI_DEVICE_NAME);
    
    rt_spi_configure(_max7219.spi_device, &cfg);

    return RT_EOK;
}
INIT_APP_EXPORT(drv_max7219_hw_init);

