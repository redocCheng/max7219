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

#define DRV_DEBUG
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

void max7219_clear(uint8_t dig)
{
    if(dig > (_max7219.info.scan_num))
    {
        log_e("dig num fault.");
        return;
    }

    max7219_reg_write(dig, 0);
}

void max7219_clear_all(void)
{
    for(uint8_t i = 0; i <=_max7219.info.scan_num; i++)
    {
        max7219_clear(i);
    }
}

void max7219_write_num(uint8_t dig, uint8_t data)
{
    uint8_t value = 0;

    if(dig > (_max7219.info.scan_num))
    {
        log_e("dig num fault.");
        return;
    }

    if(value > 0x0f)
    {
        log_e("value falut ,it can't max 0x0f.");
        return;
    }

    max7219_reg_write(dig, no_code_buf[data]);
}

void max7219_write(uint8_t dig, uint8_t data)
{
    if(dig > (_max7219.info.scan_num))
    {
        log_e("dig num fault.");
        return;
    }

    max7219_reg_write(dig, data);
}

void max7219_intensity_set(uint8_t value)
{
    if(value > 0x0f)
    {
        log_e("intnsity value over 0x0f.");
    }
    else
    {
        _max7219.info.intensity = value;
        max7219_reg_write(REG_ADDR_INTENSITY, value);
    }
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
    rt_err_t result = RT_EOK;
    struct rt_spi_configuration cfg = RT_MAX7219_DEFAULT_SPI_CFG;
    struct rt_device_max7219_info info = MAX7219_INFO_DEFAULT;

    memset(&_max7219, 0x00, sizeof(_max7219));

    _max7219.spi_device = (struct rt_spi_device *)rt_device_find(MAX7219_SPI_DEVICE_NAME);
    rt_spi_configure(_max7219.spi_device, &cfg);

    memcpy(&_max7219.info, &info,sizeof(struct rt_device_max7219_info));

    return result;
}
INIT_APP_EXPORT(drv_max7219_hw_init);


#if defined(RT_USING_FINSH) && defined(FINSH_USING_MSH)
#include <finsh.h>
static void max7219_intensity(uint8_t argc, char **argv)
{
	
#define CMD_INDEX  0
	
    size_t i = 0;
	
    const char* help_info[] =
    {
        [CMD_INDEX]      = "max7219_intensity [0-15]    - set intensity ",
    };
    
    if (argc != 2)
    {
        rt_kprintf("Usage:\n");
        for (i = 0; i < sizeof(help_info) / sizeof(char*); i++)
        {
            rt_kprintf("%s\n", help_info[i]);
        }
        rt_kprintf("\n");
    }
    else
    {
        max7219_intensity_set(strtol(argv[1], NULL, 0));
    }
}
MSH_CMD_EXPORT(max7219_intensity, max7219 intensity set);
#endif /* defined(RT_USING_FINSH) && defined(FINSH_USING_MSH) */
