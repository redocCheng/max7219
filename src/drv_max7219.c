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

#define DRV_DEBUG
#define LOG_TAG "drv.max7219"
#include <drv_log.h>


struct drv_max7219_device
{
    struct rt_spi_device         *spi_device;
    struct rt_device_max7219_info info;
    
    max7219_position_t            *position_buf; 
};


/* Number of one array */
const uint8_t one_number_buf[256] =   \
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

/*  Non-encoding mode Hex single encoding  */
const uint8_t no_code_num_buf[16] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x0d, 0x3d, 0x4f, 0x47};

static struct drv_max7219_device _max7219;

static int max7219_reg_write(uint16_t chip, uint8_t cmd, uint8_t data);
static uint8_t position_of_last_one(uint8_t dig_config);
static uint8_t dig_of_the_chip(uint16_t chip, uint8_t position);
static int position_of_device_cal(uint16_t dig_num, max7219_position_t *position);
static int position_of_device_read(uint16_t dig_num, max7219_position_t *position);
static int max7219_write_chip(max7219_position_t position, uint8_t data);
static int max7219_write_dig_chip(max7219_position_t position, uint8_t data);
static void max7219_init(void);

/**
 * Write data to max7219.
 *
 * @param chip select the chip
 * @param cmd  the reg address
 * @param data the write value
 *
 * @return int return result
 */
static int max7219_reg_write(uint16_t chip, uint8_t cmd, uint8_t data)
{
    RT_ASSERT(chip < MAX7219_CHIPS_NUMBER);

    uint8_t send_buf[2 * MAX7219_CHIPS_NUMBER] = {0};
    uint16_t size = 0;

    send_buf[0] = cmd;
    send_buf[1] = data;

    size = 2 + (2 * chip);

    if(size != rt_spi_send(_max7219.spi_device, send_buf, size))
    {
        log_e("rt spi send size fault.");
        return -RT_ERROR;
    }
    
    return RT_EOK;
}

/**
 *   Calculate the position of right 1
 * @note get the max limit of dig right num
 *
 * @param dig_config  the dig config
 *
 * @return dig
 */
static uint8_t position_of_last_one(uint8_t dig_config)
{
    if(dig_config == 0)
    {
        return 0;
    }

    for(uint8_t i = 8; i >= 1; i--)
    {
        if(dig_config & 0x01)
        {
            return i;
        }
        else
        {
            dig_config >>= 1;
        }
    }

    return 0;
}

/**
 * Calculate the dig on this chip
 * @note postion is continuous,but dig may not be continuous
 * @note chip 0 config is 0xf1 , position is 5 then return 8
 *
 * @param chip the select chip
 * @param position the position
 *
 * @return dig
 */
static uint8_t dig_of_the_chip(uint16_t chip, uint8_t position)
{
    RT_ASSERT((position > 0) && (position <= 8));
    RT_ASSERT(position <= one_number_buf[_max7219.info.scan_num_buf[chip]]);

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
        return position;
    }

    /* Discontinuous */
    for(uint8_t i = 1; i <= 8; i++)
    {
        if(value & 0x80)
        {
            if(position == 1)
            {
                return i;
            }
            else
            {
                position --;
            }
        }
        value <<= 1;
    }
    return 0;
}

/**
 * calculate the position of device,
 * @note dig must less the scan_nums.
 *
 * @param dig the continuous dig num
 * @param position position on the device
 *
 * @return int
 */
static int position_of_device_cal(uint16_t dig_num, max7219_position_t *position)
{
    RT_ASSERT(position != RT_NULL);
    RT_ASSERT((dig_num != 0) && (dig_num <= _max7219.info.scan_nums));

    for(uint8_t chip = 0; chip < MAX7219_CHIPS_NUMBER; chip++)
    {
        if(dig_num <= one_number_buf[_max7219.info.scan_num_buf[chip]])
        {
            position->chip = chip;
            position->dig  = dig_of_the_chip(chip, dig_num);
            return RT_EOK;
        }
        else
        {
            dig_num -= one_number_buf[_max7219.info.scan_num_buf[chip]];
        }
    }

    return -RT_ERROR;
}

/**
 * read the position of device,
 * @note dig must less the scan_nums.
 *
 * @param dig the continuous dig num
 * @param position position on the device
 *
 * @return int
 */
static int position_of_device_read(uint16_t dig_num, max7219_position_t *position)
{
    RT_ASSERT(position != RT_NULL);
    RT_ASSERT((dig_num != 0) && (dig_num <= _max7219.info.scan_nums));

    rt_memcpy(position, &_max7219.position_buf[dig_num - 1], sizeof(max7219_position_t));
     
    return RT_EOK;
}


/**
 * Input line segment to register
 *
 * @param chip the chip select
 * @param dig Position in a single nixie tube
 * @param data Corresponding Segment Line
 *
 * @return int
 */
static int max7219_write_dig_chip(max7219_position_t position, uint8_t data)
{
    return max7219_reg_write(position.chip, position.dig, data);
}

/**
 * Write numbers and characters to the register through the digital tube number
 * @note
 *
 * @param position position on the device
 * @param data Numbers and characters
 *
 * @return int return result
 */
static int max7219_write_chip(max7219_position_t position, uint8_t data)
{
    RT_ASSERT(DECODE_MODE_NO_DEC_FOR_8_1 == _max7219.info.decode_mode);

    uint8_t value = 0;
    uint8_t dp = 0;

    if(data & 0x80)
    {
        dp = 0x80;
        data &= 0x7f;
    }

    if(data <= 0x0f)
    {
        value = no_code_num_buf[data];
    }
    else
    {
        switch(data)
        {
        case ' ':
            value = 0x00;
            break;
        case '_':
            value = 0x08;
            break;
        case '-':
            value = 0x01;
            break;
        case '.':
            value = 0x80;
            break;
        case 'r':
            value = 0x05;
            break;
        case 'p':
        case 'P':
            value = 0x67;
            break;
        case 'L':
            value = 0x0E;
            break;
        case 'H':
            value = 0x37;
            break;
        case 'h':
            value = 0x17;
            break;
        default:
            return -RT_ERROR;
        }
    }

    value |= dp;

    return max7219_reg_write(position.chip, position.dig, value);
}

/**
 * all the Digital tubes blanking.
 *
 * @return int
 */
int max7219_clear_all(void)
{
    for(uint16_t dig = 1; dig <= _max7219.info.scan_nums; dig++)
    {
        if(-RT_ERROR == max7219_reg_write(_max7219.position_buf[dig - 1].chip, _max7219.position_buf[dig - 1].dig, 0))
        {
            log_e("max7219 clear fail.");
            return -RT_ERROR;
        }
    }
    
    return RT_EOK;
}

/**
 * Write data directly to the register through the digital tube number
 * @note with data sheet (DP/A/B/C/D/E/F/G)
 *
 * @param dig Position in all digital tubes
 * @param data  Corresponding Segment Line
 *
 * @return int return result
 */
int max7219_write_dig(uint16_t dig, uint8_t data)
{
    RT_ASSERT(dig != 0);

    max7219_position_t position;

    if(dig > _max7219.info.scan_nums)
    {
        log_e("dig num fault.");
        return -RT_ERROR;
    }

    if(RT_EOK == position_of_device_read(dig, &position))
    {
        return max7219_write_dig_chip(position, data);
    }

    return -RT_ERROR;
}

/**
 * Write numbers and characters to the register through the digital tube number
 * @note support digtal 0x0 to 0xf.,support char ' ' '-' '_' 'H' 'h' 'P' 'p' 'r' '.'
 * @note support x+dp ,If the most significant bit is valid, dots are also displayed.
 *
 * @param dig Position in all digital tubes
 * @param data numbers and characters
 *
 * @return int
 */
int max7219_write(uint16_t dig, uint8_t data)
{
    RT_ASSERT(dig != 0);

    max7219_position_t position;

    if(dig > _max7219.info.scan_nums)
    {
        log_e("dig num fault.");
        return -RT_ERROR;
    }

    if(RT_EOK == position_of_device_read(dig, &position))
    {
        return max7219_write_chip(position, data);
    }

    return -RT_ERROR;
}

/**
 * Adjust brightness
 * @note no note.
 *
 * @param value (0x0 ~ 0xf)
 *
 * @return int
 */
int max7219_intensity_set(uint8_t value)
{
    if(value > 0xf)
    {
        log_e("intensity param fault.");
        return -RT_ERROR;
    }
    
    for(uint8_t chip = 0; chip < MAX7219_CHIPS_NUMBER; chip++)
    {
        if(-RT_ERROR == max7219_reg_write(chip, REG_ADDR_INTENSITY, value))
        {
            log_e("intensity set fail.");
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

        max7219_reg_write(chip, REG_ADDR_SCANLIMIT, (position_of_last_one(_max7219.info.scan_num_buf[chip]) - 1));
        max7219_reg_write(chip, REG_ADDR_INTENSITY, _max7219.info.intensity);

        _max7219.info.scan_nums += one_number_buf[_max7219.info.scan_num_buf[chip]];
    }

    _max7219.position_buf = rt_malloc(_max7219.info.scan_nums * sizeof(max7219_position_t));
    
    for(uint16_t dig = 1; dig <= _max7219.info.scan_nums; dig++)
    {
        position_of_device_cal(dig, &_max7219.position_buf[dig - 1]);
    }
     
    log_d("max7219 init.");
}

int drv_max7219_hw_init(void)
{
    struct rt_device_max7219_info info = MAX7219_INFO_DEFAULT;

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

