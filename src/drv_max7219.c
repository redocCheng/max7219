/*
 * Copyright (c) 2019, redocCheng
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-13     redoc        the first version
 */

#include <board.h>
#include "drv_max7219.h"
#include "string.h"
#include "drv_spi.h"

//#define DRV_DEBUG
#define LOG_TAG "drv.max7219"
#include <drv_log.h>

/* 解码数组 */
const uint8_t chip_tab_num_buf[256] =   \
{
0, 1, 1, 2, 1, 2, 2, 3,     \
1, 2, 2, 3, 2, 3, 3, 4,     \
1, 2, 2, 3, 2, 3, 3, 4,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
1, 2, 2, 3, 2, 3, 3, 4,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
1, 2, 2, 3, 2, 3, 3, 4,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
4, 5, 5, 6, 5, 6, 6, 7,     \
1, 2, 2, 3, 2, 3, 3, 4,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
4, 5, 5, 6, 5, 6, 6, 7,     \
2, 3, 3, 4, 3, 4, 4, 5,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
4, 5, 5, 6, 5, 6, 6, 7,     \
3, 4, 4, 5, 4, 5, 5, 6,     \
4, 5, 5, 6, 5, 6, 6, 7,     \
4, 5, 5, 6, 5, 6, 6, 7,     \
5, 6, 6, 7, 6, 7, 7, 8,     \
};

/*  非编码模式十六进制单个编码  */
const uint8_t no_code_buf[16] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x0d, 0x3d, 0x4f, 0x47};

static struct drv_max7219_device _max7219;

static void max7219_reg_write(uint16_t chip, uint8_t cmd, uint8_t data);
static uint8_t scan_limit_get(uint8_t scan_num);
static uint8_t num_to_dig(uint16_t chip, uint8_t num);
static int dig_all_to_chip(uint16_t dig, uint16_t* chip_select, uint8_t *dig_chip);
static int max7219_write_chip(uint16_t chip, uint8_t dig, uint8_t data);
static int max7219_write_num_chip(uint16_t chip, uint8_t dig, uint8_t num);
static void max7219_init(void);

/**
 * Write data to max7219.
 *
 * @param chip select the chip
 * @param cmd  the reg address
 * @param data the write value
 *
 * @return void
 */
static void max7219_reg_write(uint16_t chip, uint8_t cmd, uint8_t data)
{
    RT_ASSERT(chip < MAX7219_CHIPS_NUMBER);

    uint8_t send_buf[2 * MAX7219_CHIPS_NUMBER] = {0};
    uint16_t size = 0;

    send_buf[0] = cmd;
    send_buf[1] = data;

    size = 2 + (2 * chip);

    rt_spi_send(_max7219.spi_device, send_buf, size);
}

/**
 * get the max limit of dig right num
 * @note chip 1 if 0xf0  return 4
 * @note chip 2 if 0xf1  return 8
 *
 * @param scan_num  the dig config
 *
 * @return dig
 */
static uint8_t scan_limit_get(uint8_t scan_num)
{
    RT_ASSERT((scan_num > 0) && (scan_num <= 8));

    if(scan_num == 0)
    {
        return 0;
    }

    for(uint8_t i = 8; i >= 1; i--)
    {
        if(scan_num & 0x01)
        {
            return i;
        }
        else
        {
            scan_num >>= 1;
        }
    }

    return 0;
}

/**
 * get the Discontinuous dig
 * @note chip 1 if 0xf0 num is 3  return 3
 * @note chip 1 if 0xf1 num is 5 return 8
 *
 * @param chip the chip
 * @param num  the dig continuous
 *
 * @return dig
 */
static uint8_t num_to_dig(uint16_t chip, uint8_t num)
{
    RT_ASSERT((num > 0) && (num <= 8));
    RT_ASSERT(num <= chip_tab_num_buf[_max7219.info.scan_num_buf[chip]]);

    uint8_t value = _max7219.info.scan_num_buf[chip];

    /* continuous */
    switch(value)
    {
    case 0xff:
    case 0xfe:
    case 0xfc:
    case 0xf8:
    case 0xf0:
    case 0xe0:
    case 0xc0:
    case 0x80:
        return num;
        break;
    }

    /* Discontinuous */
    for(uint8_t i = 1; i <= 8; i++)
    {
        if((value & 0x80) && (num == 1))
        {
            return i;
        }

        if(num > 1)
        {
            num --;
        }
        value <<= 1;
    }
    return 0;
}

/**
 * get chip and the dig of the chip
 * @note dig must less the scan_nums.
 *
 * @param dig the dig num(eg.1-->n)
 * @param chip_select return the chip
 * @param dig_chip    return the dig
 *
 * @return int
 */
static int dig_all_to_chip(uint16_t dig_all, uint16_t* chip_select, uint8_t *dig_chip)
{
    RT_ASSERT(chip_select != RT_NULL);
    RT_ASSERT(dig_chip != RT_NULL);
    RT_ASSERT((dig_all != 0) && (dig_all <= _max7219.info.scan_nums));

    for(uint8_t chip = 0; chip < MAX7219_CHIPS_NUMBER; chip++)
    {
        if(dig_all <= chip_tab_num_buf[_max7219.info.scan_num_buf[chip]])
        {
            *chip_select = chip;
            *dig_chip = num_to_dig(chip, dig_all);
            return RT_EOK;
        }
        else
        {
            dig_all -= chip_tab_num_buf[_max7219.info.scan_num_buf[chip]];
        }
    }

    return -RT_ERROR;
}

/**
 * the Digital tubes blanking.

 * @param dig  1 to scan_nums
 *
 * @return rt_err_t
 */
int max7219_clear(uint16_t dig)
{
    RT_ASSERT(dig != 0);

    uint16_t chip_select = 0;
    uint8_t  dig_chip = 0;

    if(dig > _max7219.info.scan_nums)
    {
        log_e("dig num fault.");
        return -RT_ERROR;
    }

    if(RT_EOK == dig_all_to_chip(dig, &chip_select, &dig_chip))
    {
        return max7219_write_chip(chip_select, dig_chip, 0);
    }

    return -RT_ERROR;
}

/**
 * all the Digital tubes blanking.
 * @note 1 to scan_num.
 *
 * @return rt_err_t
 */
int max7219_clear_all(void)
{
    uint8_t limit;

    for(uint8_t chip = 0; chip < MAX7219_CHIPS_NUMBER; chip++)
    {
        limit = scan_limit_get(_max7219.info.scan_num_buf[chip]);

        for(uint8_t dig = 1; dig <= limit; dig++)
        {
            max7219_reg_write(chip, dig, 0);
        }
    }
    return RT_EOK;
}

/**
 * Write data to max7219.
 * @note dig must less the scan_num.
 *
 * @param chip the chip select
 * @param dig the dig num(eg.1-->8)
 * @param data write the dig data(eg.0x7e is num 0)
 *
 * @return int
 */
static int max7219_write_chip(uint16_t chip, uint8_t dig, uint8_t data)
{
    RT_ASSERT(dig != 0);

    if(dig > (_max7219.info.scan_num_buf[chip]))
    {
        log_e("chip dig num fault.");
        return -RT_ERROR;
    }

    max7219_reg_write(chip, dig, data);
    return RT_EOK;
}

/**
 * Write data to max7219.with chip
 * @note set num if use BCODE
 *
 * @param chip the chip select
 * @param dig the dig of the chip
 * @param data write the data
 *
 * @return int return result
 */
int max7219_write(uint16_t dig, uint8_t data)
{
    RT_ASSERT(dig != 0);

    uint16_t chip_select = 0;
    uint8_t  dig_chip = 0;

    if(dig > _max7219.info.scan_nums)
    {
        log_e("dig num fault.");
        return -RT_ERROR;
    }

    if(RT_EOK == dig_all_to_chip(dig, &chip_select, &dig_chip))
    {
        return max7219_write_chip(chip_select, dig_chip, data);
    }

    return -RT_ERROR;
}

/**
 * Write num to max7219.with chip
 * @note support 0x0 to 0xf.
 *
 * @param chip the chip select
 * @param dig the dig of the chip
 * @param num write the num
 *
 * @return int return result
 */
static int max7219_write_num_chip(uint16_t chip, uint8_t dig, uint8_t num)
{
    RT_ASSERT((DECODE_MODE_NO_DEC_FOR_8_1 == _max7219.info.decode_mode)
            || (DECODE_MODE_CODE_B_FOR_8_1 == _max7219.info.decode_mode));

    uint8_t value = 0;

    if(value > 0x0f)
    {
        log_e("value falut ,it can't max 0x0f.");
        return -RT_ERROR;
    }

    if(DECODE_MODE_NO_DEC_FOR_8_1 == _max7219.info.decode_mode)
    {
        max7219_reg_write(chip, dig, no_code_buf[num]);
    }
    else
    {
        max7219_reg_write(chip, dig, num);
    }

    return RT_EOK;
}

/**
 * Write num to max7219.
 * @note support 0x0 to 0xf.
 *
 * @param dig the dig of all the digs
 * @param num write the num
 *
 * @return int return result
 */
int max7219_write_num(uint16_t dig, uint8_t num)
{
    RT_ASSERT(dig != 0);

    uint16_t chip_select = 0;
    uint8_t  dig_chip = 0;

    if(dig > _max7219.info.scan_nums)
    {
        log_e("dig num fault.");
        return -RT_ERROR;
    }

    if(RT_EOK == dig_all_to_chip(dig, &chip_select, &dig_chip))
    {
        return max7219_write_num_chip(chip_select, dig_chip, num);
    }

    return -RT_ERROR;
}

/**
 * intensity set.
 * @note no note.
 *
 * @param value set the intensity (0x0 to 0xf)
 *
 * @return int return result
 */
int max7219_intensity_set(uint8_t value)
{
    for(uint8_t chip = 0; chip < MAX7219_CHIPS_NUMBER; chip++)
    {
        if(-RT_ERROR == max7219_intensity_set_chip(chip, value))
        {
            return -RT_ERROR;
        }
    }
    return RT_EOK;
}

static void max7219_init(void)
{
    _max7219.info.scan_nums = 0;

    for(uint8_t chip = 0; chip < MAX7219_CHIPS_NUMBER; chip++)
    {
        max7219_reg_write(chip, REG_ADDR_SHUTDOWN, _max7219.info.shutdown_mode);
        max7219_reg_write(chip, REG_ADDR_DISPTEST, _max7219.info.work_mode);
        max7219_reg_write(chip, REG_ADDR_DECODEMODE, _max7219.info.decode_mode);

        max7219_reg_write(chip, REG_ADDR_SCANLIMIT, scan_limit_get(_max7219.info.scan_num_buf[chip]));
        max7219_reg_write(chip, REG_ADDR_INTENSITY, _max7219.info.intensity);

        _max7219.info.scan_nums += chip_tab_num_buf[_max7219.info.scan_num_buf[chip]];
    }

    log_d("max7219 init.");
}

int drv_max7219_hw_init(void)
{
    struct rt_device_max7219_info info = MAX7219_INFO_DEFAULT;

    memset(&_max7219, 0x00, sizeof(_max7219));
    memcpy(&_max7219.info, &info,sizeof(struct rt_device_max7219_info));

    _max7219.spi_device = (struct rt_spi_device *)rt_device_find(MAX7219_SPI_DEVICE_NAME);

    if(_max7219.spi_device == RT_NULL)
    {
        log_e("max7219 spi device not find.");
        return -RT_ERROR;
    }

    max7219_init();

    return RT_EOK;
}
INIT_DEVICE_EXPORT(drv_max7219_hw_init);

